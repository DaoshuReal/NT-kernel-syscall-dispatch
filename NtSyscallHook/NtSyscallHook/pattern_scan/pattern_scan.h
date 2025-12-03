#pragma once
#include <ntifs.h>

namespace pattern_scan
{
    PUCHAR findPattern(PUCHAR moduleBase, const CHAR* pattern, const CHAR* mask);
    bool matchBytes(const UCHAR* data, const UCHAR* pattern, const CHAR* mask, SIZE_T len);
}
