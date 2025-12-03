#pragma once
#include <ntifs.h>

namespace kernel_utils
{
	bool getTextSection(PUCHAR base, PUCHAR& textBase, SIZE_T& textSize);
	uintptr_t getKernelModuleBase(const char* name);
	ULONG getKernelModuleSize(const char* name);
}
