#include "pattern_scan.h"
#include "../kernel_utils/kernel_utils.h"

namespace pattern_scan
{
    PUCHAR findPattern(PUCHAR moduleBase, const CHAR* pattern, const CHAR* mask)
    {
        PUCHAR textBase = nullptr;
        SIZE_T textSize = 0;

        if (!kernel_utils::getTextSection(moduleBase, textBase, textSize))
            return nullptr;

        SIZE_T patternLength = strlen(mask);
        if (patternLength == 0)
            return nullptr;

        for (SIZE_T i = 0; i <= textSize - patternLength; i++)
        {
            if (matchBytes(textBase + i, (PUCHAR)pattern, mask, patternLength))
                return textBase + i;
        }

        return nullptr;
    }

    bool matchBytes(const UCHAR* data, const UCHAR* pattern, const CHAR* mask, SIZE_T len)
    {
        for (SIZE_T i = 0; i < len; i++)
        {
            if (mask[i] != '?' && pattern[i] != data[i])
                return false;
        }
        return true;
    }
}
