#pragma once
#include <ntifs.h>

extern "C" int hookHandler(int input);

namespace hook_handler
{
    bool installHook(PUCHAR patternAddress);
    void removeHook();

    extern PVOID* g_qwordPtr;
    extern PVOID g_originalValue;
}
