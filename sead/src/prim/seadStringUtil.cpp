#include <prim/seadStringUtil.h>

#include <cstdio>

namespace sead { namespace StringUtil {

s32 snprintf(char* dst, size_t dstSize, const char* format, ...)
{
    std::va_list args;
    va_start(args, format);

    s32 result = vsnprintf(dst, dstSize, format, args);

    va_end(args);
    return result;
}

s32 vsnprintf(char* dst, size_t dstSize, const char* format, std::va_list args)
{
    if (dstSize == 0)
        return -1;

    s32 result = std::vsnprintf(dst, dstSize, format, args);
    if (result < 0 || result >= dstSize)
        dst[dstSize - 1] = SafeString::cNullChar;

    if (result >= dstSize)
        result = static_cast<s32>(dstSize) - 1;

    return result;
}

} } // namespace sead::StringUtil
