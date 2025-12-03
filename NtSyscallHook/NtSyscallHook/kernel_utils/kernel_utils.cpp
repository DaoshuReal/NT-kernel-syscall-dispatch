#include "kernel_utils.h"

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	SHORT LoadCount;
	SHORT TlsIndex;
	LIST_ENTRY HashLinks;
	PVOID SectionPointer;
	ULONG CheckSum;
	ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

extern "C" PVOID PsLoadedModuleList;

namespace kernel_utils
{
	uintptr_t getKernelModuleBase(const char* name)
	{
		PLIST_ENTRY list = (PLIST_ENTRY)PsLoadedModuleList;
		if (!list) return 0;

		for (PLIST_ENTRY entry = list->Flink; entry != list; entry = entry->Flink)
		{
			PLDR_DATA_TABLE_ENTRY mod = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

			CHAR modName[128] = { 0 };

			size_t i;
			for (i = 0; i < mod->BaseDllName.Length / sizeof(WCHAR) && i < sizeof(modName) - 1; i++)
				modName[i] = (CHAR)mod->BaseDllName.Buffer[i];
			modName[i] = '\0';

			if (_stricmp(modName, name) == 0)
				return (uintptr_t)mod->DllBase;
		}

		return 0;
	}

	ULONG getKernelModuleSize(const char* name)
	{
		PLIST_ENTRY list = (PLIST_ENTRY)PsLoadedModuleList;
		if (!list) return 0;

		for (PLIST_ENTRY entry = list->Flink; entry != list; entry = entry->Flink)
		{
			PLDR_DATA_TABLE_ENTRY mod = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

			CHAR modName[128] = { 0 };

			size_t i;
			for (i = 0; i < mod->BaseDllName.Length / sizeof(WCHAR) && i < sizeof(modName) - 1; i++)
				modName[i] = (CHAR)mod->BaseDllName.Buffer[i];
			modName[i] = '\0';

			if (_stricmp(modName, name) == 0)
				return mod->SizeOfImage;
		}

		return 0;
	}
}
