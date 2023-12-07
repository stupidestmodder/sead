#pragma once

#include <prim/seadSafeString.h>

#include <cstdarg>

// TODO
namespace sead { namespace StringUtil {

struct Char16Pair
{
    char16 before;
    char16 after;
};

enum class CardinalNumber
{
    eAuto = -1,
    eBase2 = 2,
    eBase8 = 8,
    eBase10 = 10,
    eBase16 = 16
};

s32 snprintf(char* dst, size_t dstSize, const char* format, ...);
s32 vsnprintf(char* dst, size_t dstSize, const char* format, std::va_list args);

s32 sw16printf(char16* dst, size_t dstLen, const char16* format, ...);
s32 vsw16printf(char16* dst, size_t dstLen, const char16* format, std::va_list args);

} } // namespace sead::StringUtil
