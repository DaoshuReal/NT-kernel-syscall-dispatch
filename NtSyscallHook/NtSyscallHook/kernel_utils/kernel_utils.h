#pragma once
#include <ntifs.h>

namespace kernel_utils
{
	uintptr_t getKernelModuleBase(const char* name);
	ULONG getKernelModuleSize(const char* name);
}
