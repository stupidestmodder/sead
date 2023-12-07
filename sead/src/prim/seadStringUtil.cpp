#include <prim/seadStringUtil.h>

#include <cstdio>
#include <cwchar>

namespace sead { namespace StringUtil {

s32 snprintf(char* dst, size_t dstSize, const char* format, ...)
{
    std::va_list vargs;
    va_start(vargs, format);

    s32 result = vsnprintf(dst, dstSize, format, vargs);

    va_end(vargs);
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

s32 sw16printf(char16* dst, size_t dstLen, const char16* format, ...)
{
    std::va_list vargs;
    va_start(vargs, format);

    s32 result = vsw16printf(dst, dstLen, format, vargs);

    va_end(vargs);
    return result;
}

s32 vsw16printf(char16* dst, size_t dstLen, const char16* format, std::va_list args)
{
    if (dstLen == 0)
        return -1;

    //* Nintendo made their own implementation here
    // TODO: Implement this

    return std::vswprintf(dst, dstLen, format, args);
}

} } // namespace sead::StringUtil
