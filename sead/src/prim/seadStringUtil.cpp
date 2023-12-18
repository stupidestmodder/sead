#include <prim/seadStringUtil.h>

#include <cstdio>
#include <cwchar>

namespace sead { namespace StringUtil {

template <typename T>
static bool tryParseImpl_(T* value, const SafeString& str, CardinalNumber cardinalNumber)
{
    // TODO
    SEAD_UNUSED(value);
    SEAD_UNUSED(str);
    SEAD_UNUSED(cardinalNumber);
    SEAD_ASSERT(false);
    return false;
}

template <typename T>
static T parseImpl_(const SafeString& str, CardinalNumber cardinalNumber)
{
    T value = 0;

    bool success = tryParseImpl_(&value, str, cardinalNumber);
    SEAD_ASSERT_MSG(success, "parse failed. (str = \"%s\")", str.cstr());

    return value;
}

s32 parseS32(const SafeString& str, CardinalNumber cardinalNumber)
{
    return parseImpl_<s32>(str, cardinalNumber);
}

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
    if (result < 0 || result >= static_cast<s32>(dstSize))
        dst[dstSize - 1] = SafeString::cNullChar;

    if (result >= static_cast<s32>(dstSize))
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
