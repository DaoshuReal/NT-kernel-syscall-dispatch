#include "process_utils.h"

typedef struct _SYSTEM_PROCESS_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    UCHAR Reserved1[48];
    UNICODE_STRING ImageName;
    KPRIORITY BasePriority;
    HANDLE UniqueProcessId;
    PVOID Reserved2;
    ULONG HandleCount;
    ULONG SessionId;
    PVOID Reserved3;
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG Reserved4;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    PVOID Reserved5;
    SIZE_T QuotaPagedPoolUsage;
    PVOID Reserved6;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER Reserved7[6];
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;

extern "C" NTSTATUS ZwQuerySystemInformation(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
);

namespace process_utils
{
    HANDLE findProcessByName(const WCHAR* processName)
    {
        if (!processName)
            return nullptr;

        ULONG bufferSize = 0x100000;
        PVOID buffer = ExAllocatePoolWithTag(NonPagedPool, bufferSize, 'proc');
        if (!buffer)
            return nullptr;

        HANDLE pid = nullptr;
        NTSTATUS status = ZwQuerySystemInformation(5, buffer, bufferSize, &bufferSize);

        if (NT_SUCCESS(status))
        {
            PSYSTEM_PROCESS_INFORMATION processInfo = (PSYSTEM_PROCESS_INFORMATION)buffer;

            while (processInfo->NextEntryOffset)
            {
                if (processInfo->ImageName.Buffer && wcsstr(processInfo->ImageName.Buffer, processName))
                {
                    pid = processInfo->UniqueProcessId;
                    break;
                }
                processInfo = (PSYSTEM_PROCESS_INFORMATION)((PUCHAR)processInfo + processInfo->NextEntryOffset);
            }
        }

        ExFreePoolWithTag(buffer, 'proc');
        return pid;
    }
}
