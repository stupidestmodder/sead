#pragma once

#include <prim/seadMemUtil.h>
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

template <typename CharType>
inline const CharType& BufferedSafeStringBase<CharType>::operator[](s32 idx) const
{
    if (idx < 0 || idx > getBufferSize() - 1)
    {
        SEAD_ASSERT_MSG(false, "index(%d) out of range[0, %d]", idx, getBufferSize() - 1);
        return SafeStringBase<CharType>::cNullChar;
    }

    return this->mStringTop[idx];
}

template <typename CharType>
inline s32 BufferedSafeStringBase<CharType>::copy(const SafeStringBase<CharType>& rhs, s32 size)
{
    CharType* dst = getMutableStringTop_();
    const CharType* src = rhs.cstr();
    if (dst == src)
        return 0;

    if (size < 0)
        size = rhs.calcLength();

    if (size >= getBufferSize())
    {
        SEAD_ASSERT_MSG(false, "Buffer overflow. (Buffer Size: %d, Copy Size: %d)", getBufferSize(), size);
        size = getBufferSize() - 1;
    }

    MemUtil::copy(dst, src, size * sizeof(CharType));
    dst[size] = SafeStringBase<CharType>::cNullChar;

    return size;
}

template <typename CharType>
inline s32 BufferedSafeStringBase<CharType>::copyAt(s32 at, const SafeStringBase<CharType>& src, s32 cpyLength)
{
    CharType* dst = getMutableStringTop_();
    s32 len = this->calcLength();

    if (at < 0)
    {
        at = len + at + 1;
        if (at < 0)
        {
            SEAD_ASSERT_MSG(false, "at(%d) out of range[0, %d]", at, len);
            at = 0;
        }
    }

    if (cpyLength < 0)
        cpyLength = src.calcLength();

    if (at + cpyLength >= getBufferSize())
    {
        SEAD_ASSERT_MSG(false, "Buffer overflow. (Buffer Size: %d, At: %d, Copy Length: %d)", getBufferSize(), at, cpyLength);
        cpyLength = getBufferSize() - at - 1;
    }

    if (cpyLength <= 0)
        return 0;

    MemUtil::copy(dst + at, src.cstr(), cpyLength * sizeof(CharType));
    if (at + cpyLength > len)
        dst[at + cpyLength] = SafeStringBase<CharType>::cNullChar;

    return cpyLength;
}

template <typename CharType>
inline s32 BufferedSafeStringBase<CharType>::cutOffCopy(const SafeStringBase<CharType>& rhs, s32 size)
{
    CharType* dst = getMutableStringTop_();
    const CharType* src = rhs.cstr();
    if (dst == src)
        return 0;

    if (size < 0)
        size = rhs.calcLength();

    if (size >= getBufferSize())
        size = getBufferSize() - 1;

    MemUtil::copy(dst, src, size * sizeof(CharType));
    dst[size] = SafeStringBase<CharType>::cNullChar;

    return size;
}

template <typename CharType>
inline s32 BufferedSafeStringBase<CharType>::cutOffCopyAt(s32 at, const SafeStringBase<CharType>& src, s32 cpyLength)
{
    CharType* dst = getMutableStringTop_();
    s32 len = this->calcLength();

    if (at < 0)
    {
        at = len + at + 1;
        if (at < 0)
        {
            SEAD_ASSERT_MSG(false, "at(%d) out of range[0, %d]", at, len);
            at = 0;
        }
    }

    if (cpyLength < 0)
        cpyLength = src.calcLength();

    if (at + cpyLength >= getBufferSize())
        cpyLength = getBufferSize() - at - 1;

    if (cpyLength <= 0)
        return 0;

    MemUtil::copy(dst + at, src.cstr(), cpyLength * sizeof(CharType));
    if (at + cpyLength > len)
        dst[at + cpyLength] = SafeStringBase<CharType>::cNullChar;

    return cpyLength;
}

template <typename CharType>
inline s32 BufferedSafeStringBase<CharType>::copyAtWithTerminate(s32 at, const SafeStringBase<CharType>& src, s32 cpyLength)
{
    CharType* dst = getMutableStringTop_();

    if (at < 0)
    {
        s32 len = this->calcLength();
        at = len + at + 1;
        if (at < 0)
        {
            SEAD_ASSERT_MSG(false, "at(%d) out of range[0, %d]", at, len);
            at = 0;
        }
    }

    if (cpyLength < 0)
        cpyLength = src.calcLength();

    if (at + cpyLength >= getBufferSize())
    {
        SEAD_ASSERT_MSG(false, "Buffer overflow. (Buffer Size: %d, At: %d, Copy Length: %d)", getBufferSize(), at, cpyLength);
        cpyLength = getBufferSize() - at - 1;
    }

    if (cpyLength <= 0)
        return 0;

    MemUtil::copy(dst + at, src.cstr(), cpyLength * sizeof(CharType));
    dst[at + cpyLength] = SafeStringBase<CharType>::cNullChar;

    return cpyLength;
}

template <typename CharType>
inline s32 BufferedSafeStringBase<CharType>::append(const SafeStringBase<CharType>& src, s32 appendLength)
{
    return copyAt(-1, src, appendLength);
}

template <typename CharType>
inline s32 BufferedSafeStringBase<CharType>::append(CharType srcChr)
{
    s32 len = this->calcLength();
    if (len >= getBufferSize() - 1)
    {
        SEAD_ASSERT_MSG(false, "Buffer overflow. (Buffer Size: %d, Length: %d)", getBufferSize(), len);
        return 0;
    }

    CharType* mutableStringTop = getMutableStringTop_();
    mutableStringTop[len] = srcChr;
    mutableStringTop[len + 1] = SafeStringBase<CharType>::cNullChar;

    return 1;
}

template <typename CharType>
inline s32 BufferedSafeStringBase<CharType>::append(CharType srcChr, s32 num)
{
    SEAD_ASSERT_MSG(num >= 0, "append error. num < 0, num = %d", num);

    if (num < 1)
        return 0;

    s32 len = this->calcLength();
    if (num >= getBufferSize() - len)
    {
        SEAD_ASSERT_MSG(false, "Buffer overflow. (Buffer Size: %d, Length: %d, Num: %d)", getBufferSize(), len, num);
        num = getBufferSize() - len - 1;
    }

    CharType* mutableStringTop = getMutableStringTop_();
    for (s32 i = 0; i < num; i++)
    {
        mutableStringTop[len + i] = srcChr;
    }

    mutableStringTop[len + num] = SafeStringBase<CharType>::cNullChar;

    return num;
}

template <typename CharType>
inline s32 BufferedSafeStringBase<CharType>::chop(s32 chopNum)
{
    s32 len = this->calcLength();
    if (chopNum < 0 || chopNum > len)
    {
        SEAD_ASSERT_MSG(false, "chopNum(%d) out of range[0, %d]", chopNum, len);

        if (chopNum < 0)
            return 0;

        chopNum = len;
    }

    s32 pos = len - chopNum;

    CharType* str = getMutableStringTop_();
    str[pos] = SafeStringBase<CharType>::cNullChar;

    return chopNum;
}

template <typename CharType>
inline s32 BufferedSafeStringBase<CharType>::chopMatchedChar(CharType chopChar)
{
    s32 len = this->calcLength();
    if (len == 0)
        return 0;

    CharType* str = getMutableStringTop_();
    if (str[len - 1] == chopChar)
    {
        str[len - 1] = SafeStringBase<CharType>::cNullChar;
        return 1;
    }

    return 0;
}

template <typename CharType>
inline s32 BufferedSafeStringBase<CharType>::chopMatchedChar(const SafeStringBase<CharType>& chopCharList)
{
    s32 len = this->calcLength();
    if (len == 0)
        return 0;

    CharType* str = getMutableStringTop_();
    if (chopCharList.include(str[len - 1]))
    {
        str[len - 1] = SafeStringBase<CharType>::cNullChar;
        return 1;
    }

    return 0;
}

template <typename CharType>
inline s32 BufferedSafeStringBase<CharType>::chopUnprintableAsciiChar()
{
    s32 len = this->calcLength();
    if (len == 0)
        return 0;

    CharType* str = getMutableStringTop_();
    if (isUnprintableChar_(str[len - 1]))
    {
        str[len - 1] = SafeStringBase<CharType>::cNullChar;
        return 1;
    }

    return 0;
}

template <typename CharType>
inline s32 BufferedSafeStringBase<CharType>::rstrip(const SafeStringBase<CharType>& stripCharList)
{
    s32 len = this->calcLength();
    if (len == 0)
        return 0;

    CharType* str = getMutableStringTop_();
    s32 count = 0;

    for (s32 i = len - 1; i >= 0; i--)
    {
        if (stripCharList.include(str[i]))
        {
            str[i] = SafeStringBase<CharType>::cNullChar;
            count++;
        }
        else
        {
            break;
        }
    }

    return count;
}

template <typename CharType>
inline s32 BufferedSafeStringBase<CharType>::rstripUnprintableAsciiChars()
{
    s32 len = this->calcLength();
    if (len == 0)
        return 0;

    CharType* str = getMutableStringTop_();
    s32 count = 0;

    for (s32 i = len - 1; i >= 0; i--)
    {
        if (isUnprintableChar_(str[i]))
        {
            str[i] = SafeStringBase<CharType>::cNullChar;
            count++;
        }
        else
        {
            break;
        }
    }

    return count;
}

template <typename CharType>
inline s32 BufferedSafeStringBase<CharType>::trim(s32 trimLength)
{
    if (trimLength >= getBufferSize())
    {
        SEAD_ASSERT_MSG(false, "trimLength(%d) out of bounds. [0,%d) \n", trimLength, getBufferSize());
        return this->calcLength();
    }

    if (trimLength < 0)
    {
        SEAD_ASSERT_MSG(false, "trimLength(%d) out of bounds. [0,%d) \n", trimLength, getBufferSize());
        trimLength = 0;
    }

    CharType* mutableStringTop = getMutableStringTop_();
    mutableStringTop[trimLength] = SafeStringBase<CharType>::cNullChar;

    return trimLength;
}

template <typename CharType>
inline s32 BufferedSafeStringBase<CharType>::trimMatchedString(const SafeStringBase<CharType>& trimString)
{
    s32 len = this->calcLength();

    s32 tokenLen = trimString.calcLength();
    if (len < tokenLen)
        return len;

    if (SafeStringBase<CharType>(&this->mStringTop[len - tokenLen]).comparen(trimString, tokenLen) == 0)
    {
        CharType* str = getMutableStringTop_();
        str[len - tokenLen] = SafeStringBase<CharType>::cNullChar;
        return len - tokenLen;
    }

    return len;
}

template <typename CharType>
template <typename Other>
inline s32 BufferedSafeStringBase<CharType>::convertFromOtherType_(const SafeStringBase<Other>& src, s32 srcSize)
{
    s32 srcLength = src.calcLength();

    if (srcSize == -1)
    {
        srcSize = srcLength;
    }
    else if (srcSize < 0)
    {
        SEAD_ASSERT_MSG(false, "srcSize(%d) out of bounds [%d,%d]", srcSize, 0, srcLength);
        return 0;
    }
    else if (srcSize > srcLength)
    {
        SEAD_ASSERT_MSG(false, "srcSize(%d) out of bounds [%d,%d]", srcSize, 0, srcLength);
        srcSize = srcLength;
    }

    if (srcSize >= getBufferSize())
    {
        SEAD_ASSERT_MSG(false, "copySize(%d) out of bounds [%d,%d)", srcSize, 0, getBufferSize());
        srcSize = getBufferSize() - 1;
    }

    CharType* dst = getMutableStringTop_();
    Other* srcArray = const_cast<Other*>(src.cstr());

    for (s32 i = 0; i < srcSize; i++)
    {
        dst[i] = static_cast<CharType>(srcArray[i]);
    }

    dst[srcSize] = SafeStringBase<CharType>::cNullChar;

    return srcSize;
}

template <typename CharType>
inline bool BufferedSafeStringBase<CharType>::isUnprintableChar_(CharType c) const
{
    if (c <= 0x20 || c == 0x7F)
        return true;

    return false;
}

} // namespace sead
