#include <prim/seadSafeString.h>

#include <prim/seadStringUtil.h>

namespace sead {

template <>
const char SafeStringBase<char>::cNullChar = '\0';

template <>
const char SafeStringBase<char>::cLineBreakChar = '\n';

template <>
const SafeStringBase<char> SafeStringBase<char>::cEmptyString("");

template <>
const char16 SafeStringBase<char16>::cNullChar = L'\0';

template <>
const char16 SafeStringBase<char16>::cLineBreakChar = L'\n';

template <>
const SafeStringBase<char16> SafeStringBase<char16>::cEmptyString(L"");

template <>
SafeStringBase<char>& SafeStringBase<char>::operator=(const SafeStringBase<char>& rhs)
{
    mStringTop = rhs.mStringTop;
    return *this;
}

template <>
SafeStringBase<char16>& SafeStringBase<char16>::operator=(const SafeStringBase<char16>& rhs)
{
    mStringTop = rhs.mStringTop;
    return *this;
}

template <>
BufferedSafeStringBase<char>& BufferedSafeStringBase<char>::operator=(const SafeStringBase<char>& rhs)
{
    copy(rhs);
    return *this;
}

template <>
BufferedSafeStringBase<char16>& BufferedSafeStringBase<char16>::operator=(const SafeStringBase<char16>& rhs)
{
    copy(rhs);
    return *this;
}

template <>
s32 BufferedSafeStringBase<char>::format(const char* formatString, ...)
{
    std::va_list va;

    va_start(va, formatString);
    s32 ret = formatV(formatString, va);
    va_end(va);

    return ret;
}

template <>
s32 BufferedSafeStringBase<char16>::format(const char16* formatString, ...)
{
    std::va_list va;

    va_start(va, formatString);
    s32 ret = formatV(formatString, va);
    va_end(va);

    return ret;
}

template <>
s32 BufferedSafeStringBase<char>::formatV(const char* formatString, std::va_list varg)
{
    char* mutableString = getMutableStringTop_();
    return BufferedSafeStringBase::formatImpl_(mutableString, getBufferSize(), formatString, varg);
}

template <>
s32 BufferedSafeStringBase<char16>::formatV(const char16* formatString, std::va_list varg)
{
    char16* mutableString = getMutableStringTop_();
    return BufferedSafeStringBase::formatImpl_(mutableString, getBufferSize(), formatString, varg);
}

template <>
s32 BufferedSafeStringBase<char>::appendWithFormat(const char* formatString, ...)
{
    std::va_list va;

    va_start(va, formatString);
    s32 ret = appendWithFormatV(formatString, va);
    va_end(va);

    return ret;
}

template <>
s32 BufferedSafeStringBase<char16>::appendWithFormat(const char16* formatString, ...)
{
    std::va_list va;

    va_start(va, formatString);
    s32 ret = appendWithFormatV(formatString, va);
    va_end(va);

    return ret;
}

template <>
s32 BufferedSafeStringBase<char>::appendWithFormatV(const char* formatString, std::va_list varg)
{
    char* mutableStringTop = getMutableStringTop_();
    s32 length = calcLength();
    s32 ret = BufferedSafeStringBase::formatImpl_(mutableStringTop + length, getBufferSize() - length, formatString, varg);

    return length + ret;
}

template <>
s32 BufferedSafeStringBase<char16>::appendWithFormatV(const char16* formatString, std::va_list varg)
{
    char16* mutableStringTop = getMutableStringTop_();
    s32 length = calcLength();
    s32 ret = BufferedSafeStringBase::formatImpl_(mutableStringTop + length, getBufferSize() - length, formatString, varg);

    return length + ret;
}

template <>
s32 BufferedSafeStringBase<char>::formatImpl_(char* dst, s32 dstSize, const char* formatString, std::va_list varg)
{
    s32 ret = StringUtil::vsnprintf(dst, dstSize, formatString, varg);
    if (ret < 0)
        ret = dstSize - 1;

    return ret;
}

template <>
s32 BufferedSafeStringBase<char16>::formatImpl_(char16* dst, s32 dstSize, const char16* formatString, std::va_list varg)
{
    s32 ret = StringUtil::vsw16printf(dst, dstSize, formatString, varg);
    if (ret < 0 || ret >= dstSize)
    {
        dst[dstSize - 1] = cNullChar;
        ret = dstSize - 1;
    }

    return ret;
}

template <>
void BufferedSafeStringBase<char>::assureTerminationImpl_() const
{
    BufferedSafeStringBase* mutablePtr = const_cast<BufferedSafeStringBase*>(this);
    char* mutableStringTop = mutablePtr->getMutableStringTop_();
    mutableStringTop[getBufferSize() - 1] = cNullChar;
}

template <>
void BufferedSafeStringBase<char16>::assureTerminationImpl_() const
{
    BufferedSafeStringBase* mutablePtr = const_cast<BufferedSafeStringBase*>(this);
    char16* mutableStringTop = mutablePtr->getMutableStringTop_();
    mutableStringTop[getBufferSize() - 1] = cNullChar;
}

template <>
HeapSafeStringBase<char>& HeapSafeStringBase<char>::operator=(const SafeStringBase<char>& rhs)
{
    copy(rhs);
    return *this;
}

template <>
HeapSafeStringBase<char16>& HeapSafeStringBase<char16>::operator=(const SafeStringBase<char16>& rhs)
{
    copy(rhs);
    return *this;
}

} // namespace sead
