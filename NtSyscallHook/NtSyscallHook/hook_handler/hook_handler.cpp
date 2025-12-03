#include "hook_handler.h"

#include <intrin.h>

int hookHandler(int input)
{
    DbgPrint("[+] hookHandler called with input: %d\n", input);
    return input;
}

namespace hook_handler
{
    PVOID* g_qwordPtr = nullptr;
    PVOID g_originalValue = nullptr;

    bool installHook(PUCHAR patternAddress)
    {
        INT32 offset = *(INT32*)(patternAddress + 3);

        g_qwordPtr = (PVOID*)(patternAddress + 7 + offset);

        DbgPrint("[+] Original qword ptr: 0x%p\n", g_qwordPtr);
        DbgPrint("[+] Original qword value: 0x%p\n", *g_qwordPtr);

        g_originalValue = *g_qwordPtr;

        KIRQL irql = KeRaiseIrqlToDpcLevel();
        ULONG_PTR cr0 = __readcr0();
        __writecr0(cr0 & ~0x10000);

        *g_qwordPtr = (PVOID)&hookHandler;

        __writecr0(cr0);
        KeLowerIrql(irql);

        DbgPrint("[+] Hook installed successfully\n");
        return true;
    }

    void removeHook()
    {
        if (!g_qwordPtr)
            return;

        KIRQL irql = KeRaiseIrqlToDpcLevel();
        ULONG_PTR cr0 = __readcr0();
        __writecr0(cr0 & ~0x10000);

        *g_qwordPtr = g_originalValue;

        __writecr0(cr0);
        KeLowerIrql(irql);

        DbgPrint("[+] Hook removed successfully\n");
    }
}
