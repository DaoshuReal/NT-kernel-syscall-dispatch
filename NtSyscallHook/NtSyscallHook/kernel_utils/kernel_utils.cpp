#include "kernel_utils.h"

#include <ntimage.h>

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
extern "C" PIMAGE_NT_HEADERS NTAPI RtlImageNtHeader(PVOID Base);

namespace kernel_utils
{
	bool getTextSection(PUCHAR base, PUCHAR& textBase, SIZE_T& textSize)
	{
		PIMAGE_NT_HEADERS ntHeaders = RtlImageNtHeader(base);
		if (!ntHeaders)
			return false;

		PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);

		for (ULONG i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++)
		{
			if (memcmp(section[i].Name, ".text", 5) == 0)
			{
				textBase = base + section[i].VirtualAddress;
				textSize = section[i].Misc.VirtualSize;
				return true;
			}
		}
		return false;
	}

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
