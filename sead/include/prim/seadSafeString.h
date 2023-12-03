#pragma once

#include <basis/seadAssert.h>
#include <basis/seadTypes.h>

// TODO

namespace sead {

template <typename CharType>
class SafeStringBase
{
public:
    static const CharType cNullChar;
    static const CharType cLineBreakChar;
    static const SafeStringBase cEmptyString;
    static const s32 cMaximumLength = 256*1024;

public:
    SafeStringBase()
        : mStringTop(&cNullChar)
    {
    }

    SafeStringBase(const CharType* str)
        : mStringTop(str)
    {
        SEAD_ASSERT_MSG(mStringTop, "str must not be nullptr.");
    }

    virtual ~SafeStringBase()
    {
    }

    const CharType* cstr() const
    {
        assureTerminationImpl_();
        return mStringTop;
    }

    bool isEqual(const SafeStringBase& rhs) const;

    friend bool operator==(const SafeStringBase& lhs, const SafeStringBase& rhs)
    {
        return lhs.isEqual(rhs);
    }

    friend bool operator!=(const SafeStringBase& lhs, const SafeStringBase& rhs)
    {
        return !lhs.isEqual(rhs);
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
};

using SafeString = SafeStringBase<char>;
using WSafeString = SafeStringBase<char16>;

} // namespace sead
