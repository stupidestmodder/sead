#pragma once

#include <prim/seadSafeString.h>

namespace sead {

template <typename CharType>
inline const CharType& SafeStringBase<CharType>::at(s32 idx) const
{
    s32 len = calcLength();
    if (idx < 0 || idx > len)
    {
        SEAD_ASSERT_MSG(false, "index(%d) out of range[0, %d]", idx, len);
        return cNullChar;
    }

    return mStringTop[idx];
}

template <typename CharType>
inline s32 SafeStringBase<CharType>::calcLength() const
{
    SEAD_ASSERT(mStringTop);

    assureTerminationImpl_();
    s32 length = 0;

    for (;;)
    {
        if (length > cMaximumLength || mStringTop[length] == cNullChar)
            break;

        length++;
    }

    if (length > cMaximumLength)
    {
        SEAD_ASSERT_MSG(false, "too long string\n");
        return 0;
    }

    return length;
}

template <typename CharType>
inline const SafeStringBase<CharType> SafeStringBase<CharType>::getPart(s32 at) const
{
    s32 len = calcLength();
    if (at < 0 || at > len)
    {
        SEAD_ASSERT_MSG(false, "index(%d) out of range[0, %d]", at, len);
        return cEmptyString;
    }

    return SafeStringBase(mStringTop + at);
}

template <typename CharType>
inline const SafeStringBase<CharType> SafeStringBase<CharType>::getPart(const iterator& it) const
{
    return getPart(it.getIndex());
}

template <typename CharType>
inline const SafeStringBase<CharType> SafeStringBase<CharType>::getPart(const token_iterator& it) const
{
    return getPart(it.getIndex());
}

template <typename CharType>
inline bool SafeStringBase<CharType>::include(const CharType& c) const
{
    assureTerminationImpl_();

    for (s32 i = 0; i <= cMaximumLength; i++)
    {
        if (unsafeAt_(i) == cNullChar)
            return false;

        if (unsafeAt_(i) == c)
            return true;
    }

    return false;
}

template <typename CharType>
inline bool SafeStringBase<CharType>::include(const SafeStringBase& str) const
{
    return findIndex(str) != -1;
}

template <typename CharType>
inline bool SafeStringBase<CharType>::isEqual(const SafeStringBase& rhs) const
{
    assureTerminationImpl_();

    if (cstr() == rhs.cstr())
        return true;

    for (s32 i = 0; i <= cMaximumLength; i++)
    {
        if (unsafeAt_(i) != rhs.unsafeAt_(i))
            return false;
        else if (unsafeAt_(i) == cNullChar)
            return true;
    }

    SEAD_ASSERT_MSG(false, "too long string\n");
    return false;
}

template <typename CharType>
inline s32 SafeStringBase<CharType>::comparen(const SafeStringBase& rhs, s32 n) const
{
    assureTerminationImpl_();

    if (cstr() == rhs.cstr())
        return 0;

    if (n > cMaximumLength)
    {
        SEAD_ASSERT_MSG(false, "paramater(%d) out of bounds [0, %d]", n, cMaximumLength);
        n = cMaximumLength;
    }

    for (s32 i = 0; i < n; i++)
    {
        if (unsafeAt_(i) == cNullChar && rhs.unsafeAt_(i) == cNullChar)
            return 0;
        else if (unsafeAt_(i) == cNullChar)
            return -1;
        else if (rhs.unsafeAt_(i) == cNullChar)
            return 1;
        else if (unsafeAt_(i) < rhs.unsafeAt_(i))
            return -1;
        else if (unsafeAt_(i) > rhs.unsafeAt_(i))
            return 1;
    }

    return 0;
}

template <typename CharType>
inline s32 SafeStringBase<CharType>::compare(const SafeStringBase& rhs) const
{
    return comparen(rhs, cMaximumLength);
}

template <typename CharType>
inline SafeStringBase<CharType>::iterator SafeStringBase<CharType>::findIterator(const SafeStringBase& token) const
{
    s32 idx = findIndex(token);
    if (idx != -1)
        return iterator(this, idx);
    else
        return end();
}

// TODO: Is this correct ?
template <typename CharType>
inline SafeStringBase<CharType>::iterator SafeStringBase<CharType>::findIterator(const SafeStringBase& token, const iterator& startIt) const
{
    s32 idx = findIndex(token, startIt.getIndex());
    if (idx != -1)
        return iterator(this, idx);
    else
        return end();
}

template <typename CharType>
inline s32 SafeStringBase<CharType>::findIndex(const SafeStringBase& token) const
{
    s32 len = calcLength();
    s32 tokenLen = token.calcLength();

    for (s32 i = 0; i <= len - tokenLen; i++)
    {
        if (SafeStringBase(&mStringTop[i]).comparen(token, tokenLen) == 0)
            return i;
    }

    return -1;
}

template <typename CharType>
inline s32 SafeStringBase<CharType>::findIndex(const SafeStringBase& token, s32 startPos) const
{
    s32 len = calcLength();
    if (startPos < 0 || startPos > len)
    {
        SEAD_ASSERT_MSG(false, "startPos(%d) out of range[0, %d]", startPos, len);
        return -1;
    }

    s32 tokenLen = token.calcLength();

    for (s32 i = startPos; i <= len - tokenLen; i++)
    {
        if (SafeStringBase(&mStringTop[i]).comparen(token, tokenLen) == 0)
            return i;
    }

    return -1;
}

template <typename CharType>
inline SafeStringBase<CharType>::iterator SafeStringBase<CharType>::rfindIterator(const SafeStringBase& token) const
{
    s32 idx = rfindIndex(token);
    if (idx != -1)
        return iterator(this, idx);
    else
        return end();
}

template <typename CharType>
inline s32 SafeStringBase<CharType>::rfindIndex(const SafeStringBase& token) const
{
    s32 len = calcLength();
    s32 tokenLen = token.calcLength();
    len -= tokenLen;

    if (len < 0)
        return -1;

    for (; len >= 0; len--)
    {
        if (SafeStringBase(&mStringTop[len]).comparen(token, tokenLen) == 0)
            break;
    }

    return len;
}

template <typename CharType>
inline bool SafeStringBase<CharType>::startsWith(const SafeStringBase& token) const
{
    assureTerminationImpl_();

    const CharType* str = mStringTop;
    const CharType* tokenStr = token.mStringTop;

    for (s32 i = 0; tokenStr[i] != cNullChar; i++)
    {
        if (str[i] != tokenStr[i])
            return false;
    }

    return true;
}

template <typename CharType>
inline bool SafeStringBase<CharType>::endsWith(const SafeStringBase& token) const
{
    assureTerminationImpl_();

    s32 tokenLen = token.calcLength();
    if (tokenLen == 0)
        return true;

    s32 len = calcLength();
    if (len < tokenLen)
        return false;

    const CharType* str = mStringTop;
    const CharType* tokenStr = token.mStringTop;

    for (s32 i = 0; i < tokenLen; i++)
    {
        if (str[len - tokenLen + i] != tokenStr[i])
            return false;
    }

    return true;
}

} // namespace sead
