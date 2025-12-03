#pragma once
#include <ntifs.h>

namespace process_utils
{
    HANDLE findProcessByName(const WCHAR* processName);
}
