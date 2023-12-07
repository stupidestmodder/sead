#pragma once

#include <basis/seadAssert.h>
#include <basis/seadNew.h>
#include <basis/seadTypes.h>

#include <cstdarg>

namespace sead {

template <typename CharType>
class BufferedSafeStringBase;

template <typename CharType>
class SafeStringBase
{
public:
    static const CharType cNullChar;
    static const CharType cLineBreakChar;
    static const SafeStringBase cEmptyString;
    static const s32 cMaximumLength = 512*1024;

public:
    class iterator
    {
    public:
        explicit iterator(const SafeStringBase* str)
            : mString(str)
            , mIndex(0)
        {
        }

        iterator(const SafeStringBase* str, s32 idx)
            : mString(str)
            , mIndex(idx)
        {
        }

        inline iterator& operator++()
        {
            ++mIndex;
            return *this;
        }

        inline iterator& operator--()
        {
            --mIndex;
            return *this;
        }

        friend bool operator==(const iterator& a, const iterator& b)
        {
            return a.mString == b.mString && a.mIndex == b.mIndex;
        }

        friend bool operator!=(const iterator& a, const iterator& b)
        {
            return !(a == b);
        }

        const CharType& operator*() const
        {
            return mString->at(mIndex);
        }

        s32 getIndex() const
        {
            return mIndex;
        }

        const SafeStringBase* getString() const
        {
            return mString;
        }

    protected:
        const SafeStringBase* mString;
        s32 mIndex;
    };

    // TODO
    class token_iterator : public iterator
    {
    public:
        token_iterator(const SafeStringBase* str, const SafeStringBase& delimiter)
            : iterator(str)
            , mDelimiter(delimiter)
        {
        }

        token_iterator(const SafeStringBase* str, s32 idx, const SafeStringBase& delimiter)
            : iterator(str, idx)
            , mDelimiter(delimiter)
        {
        }

        inline token_iterator& operator++();
        inline token_iterator& operator--();

        inline s32 get(BufferedSafeStringBase<CharType>* out) const;
        inline s32 getAndForward(BufferedSafeStringBase<CharType>* out);

    private:
        SafeStringBase mDelimiter;
    };

public:
    SafeStringBase()
        : mStringTop(&cNullChar)
    {
    }

    /* Implicit */
    SafeStringBase(const CharType* str)
        : mStringTop(str)
    {
        SEAD_ASSERT_MSG(mStringTop, "str must not be nullptr.");
    }

    virtual ~SafeStringBase()
    {
    }

    virtual SafeStringBase& operator=(const SafeStringBase& rhs);

    iterator begin() const
    {
        return iterator(this, 0);
    }

    iterator end() const
    {
        return iterator(this, calcLength() + 1);
    }

    token_iterator tokenBegin(const SafeStringBase& delimiter) const
    {
        return token_iterator(this, 0, delimiter);
    }

    token_iterator tokenEnd(const SafeStringBase& delimiter = SafeStringBase()) const
    {
        return token_iterator(this, calcLength() + 1, delimiter);
    }

    const CharType* cstr() const
    {
        assureTerminationImpl_();
        return mStringTop;
    }

    inline const CharType& at(s32 idx) const;

    inline const CharType& operator[](s32 idx) const
    {
        return at(idx);
    }

    inline s32 calcLength() const;

    inline const SafeStringBase getPart(s32 at) const;
    inline const SafeStringBase getPart(const iterator& it) const;
    inline const SafeStringBase getPart(const token_iterator& it) const;

    inline bool include(const CharType& c) const;
    inline bool include(const SafeStringBase& str) const;

    inline bool isEqual(const SafeStringBase& rhs) const;

    friend bool operator==(const SafeStringBase& lhs, const SafeStringBase& rhs)
    {
        return lhs.isEqual(rhs);
    }

    friend bool operator!=(const SafeStringBase& lhs, const SafeStringBase& rhs)
    {
        return !lhs.isEqual(rhs);
    }

    inline s32 comparen(const SafeStringBase& rhs, s32 n) const;
    inline s32 compare(const SafeStringBase& rhs) const;

    friend bool operator>(const SafeStringBase& lhs, const SafeStringBase& rhs)
    {
        return lhs.compare(rhs) > 0;
    }

    friend bool operator<(const SafeStringBase& lhs, const SafeStringBase& rhs)
    {
        return lhs.compare(rhs) < 0;
    }

    inline iterator findIterator(const SafeStringBase& token) const;
    inline iterator findIterator(const SafeStringBase&, const iterator& startIt) const;
    s32 findIndex(const SafeStringBase& token) const;
    s32 findIndex(const SafeStringBase& token, s32 startPos) const;

    inline iterator rfindIterator(const SafeStringBase& token) const;
    s32 rfindIndex(const SafeStringBase& token) const;

    bool isEmpty() const
    {
        return unsafeAt_(0) == cNullChar;
    }

    bool startsWith(const SafeStringBase& token) const;
    bool endsWith(const SafeStringBase& token) const;

private:
    const CharType& unsafeAt_(s32 idx) const
    {
        return mStringTop[idx];
    }

protected:
    virtual void assureTerminationImpl_() const
    {
    }

protected:
    const CharType* mStringTop;
};

template <typename CharType>
class BufferedSafeStringBase : public SafeStringBase<CharType>
{
public:
    BufferedSafeStringBase(CharType* buffer, s32 size)
        : SafeStringBase<CharType>(buffer)
        , mBufferSize(size)
    {
        if (size <= 0)
        {
            SEAD_ASSERT_MSG(false, "Invalied buffer size(%d).\n", size);
            this->mStringTop = nullptr;
            mBufferSize = 0;
            return;
        }

        assureTerminationImpl_();
    }

    BufferedSafeStringBase(BufferedSafeStringBase* original, s32 pos)
        : SafeStringBase<CharType>()
        , mBufferSize(0)
    {
        if (!original)
        {
            SEAD_ASSERT_MSG(false, "original string must not be nullptr.");
            this->mStringTop = nullptr;
            return;
        }

        if (pos < 0 || pos >= original->getBufferSize())
        {
            SEAD_ASSERT_MSG(false, "pos(%d) out of bounds[0,%d)", pos, original->getBufferSize());
            this->mStringTop = nullptr;
            return;
        }

        this->mStringTop = original->getBuffer() + pos;
        mBufferSize = original->getBufferSize() - pos;

        assureTerminationImpl_();
    }

    BufferedSafeStringBase(const BufferedSafeStringBase&) = delete;

    ~BufferedSafeStringBase() override
    {
    }

    BufferedSafeStringBase& operator=(const BufferedSafeStringBase& rhs)
    {
        copy(rhs);
        return *this;
    }

    BufferedSafeStringBase& operator=(const SafeStringBase<CharType>& rhs) override;

    inline const CharType& operator[](s32 idx) const;

    CharType* getBuffer()
    {
        assureTerminationImpl_();
        return getMutableStringTop_();
    }

    s32 getBufferSize() const
    {
        return mBufferSize;
    }

    inline s32 copy(const SafeStringBase<CharType>& rhs, s32 size = -1);
    inline s32 copyAt(s32 at, const SafeStringBase<CharType>& src, s32 cpyLength = -1);

    inline s32 cutOffCopy(const SafeStringBase<CharType>& rhs, s32 size = -1);
    inline s32 cutOffCopyAt(s32 at, const SafeStringBase<CharType>& src, s32 cpyLength = -1);
    inline s32 copyAtWithTerminate(s32 at, const SafeStringBase<CharType>& src, s32 cpyLength = -1);

    s32 format(const CharType* formatString, ...);
    s32 formatV(const CharType* formatString, std::va_list varg);

    s32 appendWithFormat(const CharType* formatString, ...);
    s32 appendWithFormatV(const CharType* formatString, std::va_list varg);

    inline s32 append(const SafeStringBase<CharType>& src, s32 appendLength = -1);
    inline s32 append(CharType srcChr);
    inline s32 append(CharType srcChr, s32 num);

    inline s32 chop(s32 chopNum = 1);
    inline s32 chopMatchedChar(CharType chopChar);
    inline s32 chopMatchedChar(const SafeStringBase<CharType>& chopCharList);

    inline s32 chomp()
    {
        return chopMatchedChar(SafeStringBase<CharType>::cLineBreakChar);
    }

    inline s32 chopUnprintableAsciiChar();

    inline s32 rstrip(const SafeStringBase<CharType>& stripCharList);
    inline s32 rstripUnprintableAsciiChars();

    inline s32 trim(s32 trimLength);
    inline s32 trimMatchedString(const SafeStringBase<CharType>& trimString);

    // TODO
    s32 replaceChar(CharType, CharType);
    // TODO
    s32 replaceCharList(const SafeStringBase<CharType>&, const SafeStringBase<CharType>&);

    // TODO
    s32 setReplaceString(const SafeStringBase<CharType>&, const SafeStringBase<CharType>&, const SafeStringBase<CharType>&);
    // TODO
    s32 replaceString(const SafeStringBase<CharType>&, const SafeStringBase<CharType>&);

    // TODO
    s32 cutOffSetReplaceString(const SafeStringBase<CharType>&, const SafeStringBase<CharType>&, const SafeStringBase<CharType>&);
    // TODO
    s32 cutOffReplaceString(const SafeStringBase<CharType>&, const SafeStringBase<CharType>&);

    inline void clear()
    {
        getMutableStringTop_()[0] = SafeStringBase<CharType>::cNullChar;
    }

    s32 convertFromMultiByteString(const SafeStringBase<char>& src, s32 srcSize = -1)
    {
        return convertFromOtherType_(src, srcSize);
    }

    s32 convertFromWideCharString(const SafeStringBase<char16>& src, s32 srcSize = -1)
    {
        return convertFromOtherType_(src, srcSize);
    }

private:
    CharType* getMutableStringTop_()
    {
        return const_cast<CharType*>(this->mStringTop);
    }

    template <typename Other>
    inline s32 convertFromOtherType_(const SafeStringBase<Other>& src, s32 srcSize);

    inline bool isUnprintableChar_(CharType c) const;

    static s32 formatImpl_(CharType* dst, s32 dstSize, const CharType* formatString, std::va_list varg);

protected:
    void assureTerminationImpl_() const override;

private:
    s32 mBufferSize;
};

template <typename CharType, s32 N>
class FixedSafeStringBase : public BufferedSafeStringBase<CharType>
{
public:
    FixedSafeStringBase()
        : BufferedSafeStringBase<CharType>(mBuffer, N)
    {
        this->clear();
    }

    explicit FixedSafeStringBase(const SafeStringBase<CharType>& rhs)
        : BufferedSafeStringBase<CharType>(mBuffer, N)
    {
        this->copy(rhs);
    }

    FixedSafeStringBase(const FixedSafeStringBase& rhs)
        : BufferedSafeStringBase<CharType>(mBuffer, N)
    {
        this->copy(rhs);
    }

    ~FixedSafeStringBase() override
    {
    }

    FixedSafeStringBase& operator=(const FixedSafeStringBase& rhs)
    {
        this->copy(rhs);
        return *this;
    }

    FixedSafeStringBase& operator=(const SafeStringBase<CharType>& rhs) override
    {
        this->copy(rhs);
        return *this;
    }

private:
    CharType mBuffer[N];
};

template <typename CharType>
class HeapSafeStringBase : public BufferedSafeStringBase<CharType>
{
public:
    HeapSafeStringBase(Heap* heap, const SafeStringBase<CharType>& string, s32 alignment = alignof(void*))
        : BufferedSafeStringBase<CharType>(new(heap, alignment) CharType[string.calcLength() + 1](),
                                           string.calcLength() + 1)
    {
        this->copy(string);
    }

    HeapSafeStringBase(const HeapSafeStringBase&) = delete;
    HeapSafeStringBase& operator=(const HeapSafeStringBase&) = delete;

    ~HeapSafeStringBase() override
    {
        if (this->mStringTop)
            delete[] this->mStringTop;
    }

    HeapSafeStringBase& operator=(const SafeStringBase<CharType>& rhs) override;
};

using SafeString = SafeStringBase<char>;
using WSafeString = SafeStringBase<char16>;

using BufferedSafeString = BufferedSafeStringBase<char>;
using WBufferedSafeString = BufferedSafeStringBase<char16>;

using HeapSafeString = HeapSafeStringBase<char>;
using WHeapSafeString = HeapSafeStringBase<char16>;

template <s32 N>
class FixedSafeString : public FixedSafeStringBase<char, N>
{
public:
    FixedSafeString()
        : FixedSafeStringBase<char, N>()
    {
    }

    explicit FixedSafeString(const SafeString& rhs)
        : FixedSafeStringBase<char, N>(rhs)
    {
    }

    FixedSafeString(const FixedSafeString& rhs)
        : FixedSafeStringBase<char, N>(rhs)
    {
    }

    FixedSafeString& operator=(const FixedSafeString& rhs)
    {
        this->copy(rhs);
        return *this;
    }

    FixedSafeString& operator=(const SafeString& rhs) override
    {
        this->copy(rhs);
        return *this;
    }
};

template <s32 N>
class WFixedSafeString : public FixedSafeStringBase<char16, N>
{
public:
    WFixedSafeString()
        : FixedSafeStringBase<char16, N>()
    {
    }

    explicit WFixedSafeString(const WSafeString& rhs)
        : FixedSafeStringBase<char16, N>(rhs)
    {
    }

    WFixedSafeString(const WFixedSafeString& rhs)
        : FixedSafeStringBase<char16, N>(rhs)
    {
    }

    WFixedSafeString& operator=(const WFixedSafeString& rhs)
    {
        this->copy(rhs);
        return *this;
    }

    WFixedSafeString& operator=(const WSafeString& rhs) override
    {
        this->copy(rhs);
        return *this;
    }
};

template <s32 N>
class FormatFixedSafeString : public FixedSafeString<N>
{
public:
    explicit FormatFixedSafeString(const char* formatString, ...)
        : FixedSafeString<N>()
    {
        std::va_list args;
        va_start(args, formatString);
        this->formatV(formatString, args);
        va_end(args);
    }

    FormatFixedSafeString(const FormatFixedSafeString&) = delete;
    FormatFixedSafeString& operator=(const FormatFixedSafeString&) = delete;

    ~FormatFixedSafeString() override
    {
    }
};

template <s32 N>
class WFormatFixedSafeString : public WFixedSafeString<N>
{
public:
    explicit WFormatFixedSafeString(const char16* formatString, ...)
        : WFixedSafeString<N>()
    {
        std::va_list args;
        va_start(args, formatString);
        this->formatV(formatString, args);
        va_end(args);
    }

    WFormatFixedSafeString(const WFormatFixedSafeString&) = delete;
    WFormatFixedSafeString& operator=(const WFormatFixedSafeString&) = delete;

    ~WFormatFixedSafeString() override
    {
    }
};

template <s32 N>
class FormatVFixedSafeString : public FixedSafeString<N>
{
public:
    FormatVFixedSafeString(const char* formatString, std::va_list args)
        : FixedSafeString<N>()
    {
        this->formatV(formatString, args);
    }

    FormatVFixedSafeString(const FormatVFixedSafeString&) = delete;
    FormatVFixedSafeString& operator=(const FormatVFixedSafeString&) = delete;

    ~FormatVFixedSafeString() override
    {
    }
};

template <s32 N>
class WFormatVFixedSafeString : public WFixedSafeString<N>
{
public:
    WFormatVFixedSafeString(const char* formatString, std::va_list args)
        : WFixedSafeString<N>()
    {
        this->formatV(formatString, args);
    }

    WFormatVFixedSafeString(const WFormatVFixedSafeString&) = delete;
    WFormatVFixedSafeString& operator=(const WFormatVFixedSafeString&) = delete;

    ~WFormatVFixedSafeString() override
    {
    }
};

} // namespace sead

#include <prim/seadSafeString.hpp>
