#include <ntifs.h>

#include "pattern_scan/pattern_scan.h"
#include "kernel_utils/kernel_utils.h"
#include "process_utils/process_utils.h"
#include "hook_handler/hook_handler.h"

#define BYTE_SIGNATURE "\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x74\x06\xFF\x15\x00\x00\x00\x00\x48\x83\xC4\x28\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x48\x83\xEC\x28\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x74\x08\xFF\x15\x00\x00\x00\x00\xEB\x03\x83\xC8\xFF\x48\x83\xC4\x28\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x48\x83\xEC\x28\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x74\x08\xFF\x15\x00\x00\x00\x00\xEB\x03\x83\xC8\xFF\x48\x83\xC4\x28\xC3"
#define BYTE_SIGNATURE_MASK "xxx????xxxxxx????xxxxxxxxxxxxxxxxxxxxxx??????xxxxx?????xxxxxxxxxxxxxxxxxxxxxxxx?????xxxxxx????xxxxxxxxxx"

VOID DriverUnload(_In_ PDRIVER_OBJECT driverObject)
{
    UNREFERENCED_PARAMETER(driverObject);

    if (hook_handler::g_qwordPtr)
    {
        HANDLE explorerPid = process_utils::findProcessByName(L"explorer.exe");
        if (explorerPid)
        {
            PEPROCESS process = nullptr;
            if (NT_SUCCESS(PsLookupProcessByProcessId(explorerPid, &process)))
            {
                KAPC_STATE apc;
                KeStackAttachProcess(process, &apc);

                hook_handler::removeHook();

                KeUnstackDetachProcess(&apc);
                ObDereferenceObject(process);
            }
        }
    }

    DbgPrint("[+] Driver unloaded\n");
}

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT driverObject, _In_ PUNICODE_STRING registryPath)
{
    UNREFERENCED_PARAMETER(registryPath);
    driverObject->DriverUnload = DriverUnload;

    HANDLE explorerPid = process_utils::findProcessByName(L"explorer.exe");
    if (!explorerPid)
        return STATUS_NOT_FOUND;

    PEPROCESS process = nullptr;
    NTSTATUS status = PsLookupProcessByProcessId(explorerPid, &process);
    if (!NT_SUCCESS(status))
        return status;

    KAPC_STATE apc;
    KeStackAttachProcess(process, &apc);

    uintptr_t moduleBase = kernel_utils::getKernelModuleBase("win32k.sys");
    PUCHAR result = nullptr;

    if (moduleBase)
    {
        result = pattern_scan::findPattern(
            (PUCHAR)moduleBase,
            BYTE_SIGNATURE,
            BYTE_SIGNATURE_MASK
        );
    }

    if (result)
    {
        DbgPrint("[+] Bytes at pattern: %02X %02X %02X %02X %02X %02X %02X\n",
            result[0], result[1], result[2], result[3], result[4], result[5], result[6]);

        hook_handler::installHook(result);

        if (hook_handler::g_originalValue == nullptr)
            DbgPrint("[!] Warning: Original value was NULL\n");
    }
    else
    {
        DbgPrint("[-] Pattern not found\n");
    }

    KeUnstackDetachProcess(&apc);
    ObDereferenceObject(process);

    return result ? STATUS_SUCCESS : STATUS_NOT_FOUND;
}
