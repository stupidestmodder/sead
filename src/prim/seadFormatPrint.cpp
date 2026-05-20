#include <prim/seadFormatPrint.h>

#include <prim/seadBitFlag.h>
#include <prim/seadSafeString.h>
#include <prim/seadStringUtil.h>
#include <time/seadTickSpan.h>

#include <cstring>

namespace sead {

void PrintOutput::writeLineBreak()
{
    write(&SafeString::cLineBreakChar, 1);
}

PrintFormatter& PrintOutput::operator<<(PrintFormatter& f)
{
    f.setPrintOutput(this);
    return f;
}

BufferingPrintOutput::~BufferingPrintOutput()
{
    mStreamSrc.write(&SafeString::cNullChar, 1);
    mStreamSrc.flush();
}

void BufferingPrintOutput::write(const char* str, s32 len)
{
    mStreamSrc.write(str, len);
}

PrintFormatter::PrintFormatter(const char* formatStr, PrintOutput* output)
    : mFormatStr(formatStr)
    , mPrintOutput(output)
    , mPos(0)
    , mFormatStrLength(0)
    , mIsFormatRestAll(false)
{
    if (mFormatStr)
        mFormatStrLength = static_cast<s32>(std::strlen(mFormatStr));
}

void PrintFormatter::flush()
{
    if (!mFormatStr)
        return;

    mIsFormatRestAll = false;

    char option[cOptionBufSize];

    while (mPos < mFormatStrLength)
    {
        bool end = proceedToFormatMark_(option);
        SEAD_ASSERT_MSG(!end, "too much format specifier: %s", mFormatStr);
    }
}

void PrintFormatter::flushWithLineBreak()
{
    flush();

    if (mFormatStr)
        mPrintOutput->writeLineBreak();
}

PrintFormatter& PrintFormatter::operator<<(const char* str)
{
    if (!mFormatStr)
    {
        mFormatStr = str;
        mFormatStrLength = static_cast<s32>(std::strlen(str));
        return *this;
    }

    char option[cOptionBufSize];

    bool end = proceedToFormatMark_(option);
    if (end)
        PrintFormatter::outputString_(option[0] != '\0' ? option : nullptr, mPrintOutput, str, -1);

    return *this;
}

bool PrintFormatter::proceedToFormatMark_(char* option)
{
    option[0] = '\0';

    if (!mFormatStr)
    {
        SEAD_ASSERT_MSG(false, "format string is not set. please check argument order.");
        return false;
    }

    if (mIsFormatRestAll)
        return true;

    if (mPos >= mFormatStrLength)
        return false;

    const char* str = mFormatStr + mPos;
    s32 idx = 0;

    while (str[idx] != '\0')
    {
        if (str[idx] == '%')
        {
            if (str[idx + 1] != '%')
            {
                if (idx > 0)
                {
                    mPrintOutput->write(str, idx);

                    str += idx;
                    mPos += idx;
                    idx = 0;
                }

                s32 idxNext = idx + 1;
                if (str[idxNext] == '@')
                {
                    idx += 2;
                }
                else if (str[idxNext] == '<')
                {
                    idx += 2;
                    mIsFormatRestAll = true;
                }
                else
                {
                    option[0] = '%';

                    while (PrintFormatter::isQualification_(str[idxNext]))
                    {
                        if (idxNext >= cOptionLengthMax)
                        {
                            SEAD_ASSERT_MSG(false, "option string too long: %s", mFormatStr);
                            option[0] = '\0';
                            mPos = mFormatStrLength;
                            return true;
                        }

                        option[idxNext] = str[idxNext];
                        idxNext++;
                    }

                    if (str[idxNext] == '\0')
                    {
                        SEAD_ASSERT_MSG(false, "illegal format: %s", mFormatStr);
                        option[0] = '\0';
                        mPos = mFormatStrLength;
                        return true;
                    }

                    option[idxNext] = str[idxNext];
                    idx = idxNext + 1;
                    option[idx] = '\0';
                }

                mPos += idx;
                return true;
            }

            mPrintOutput->write(str, idx + 1);

            str += idx + 2;
            mPos += idx + 2;
            idx = 0;
        }
        else
        {
            idx++;
        }
    }

    if (idx > 0)
    {
        mPrintOutput->write(str, idx);
        mPos += idx;
    }

    return false;
}

void PrintFormatter::outputString_(const char* option, PrintOutput* output, const char* str, s32 strLen)
{
    if (!option || option[1] == 's')
    {
        if (strLen == -1)
            strLen = static_cast<s32>(std::strlen(str));

        output->write(str, strLen);

        return;
    }

    if (option[std::strlen(option) - 1] == 's')
    {
        FixedSafeString<128> tempStr;

        s32 tempStrLen = StringUtil::convertUtf8ToSjis(&tempStr, str, -1);

        if (option[1] == '-')
        {
            if (strLen == -1)
                strLen = static_cast<s32>(std::strlen(str));

            output->write(str, strLen);

            s32 pad = StringUtil::parseS32(option + 2, StringUtil::CardinalNumber::eBase10);

            for (s32 i = 0; i < pad - tempStrLen; i++)
            {
                output->write(" ", 1);
            }
        }
        else
        {
            s32 pad = StringUtil::parseS32(option + 1, StringUtil::CardinalNumber::eBase10);

            for (s32 i = 0; i < pad - tempStrLen; i++)
            {
                output->write(" ", 1);
            }

            if (strLen == -1)
                strLen = static_cast<s32>(std::strlen(str));

            output->write(str, strLen);
        }
    }
    else
    {
        FixedSafeString<128> tempStr;
        strLen = tempStr.format(option, str);

        output->write(tempStr.cstr(), strLen);
    }
}

void PrintFormatter::outputPtr_(const char* option, PrintOutput* output, uintptr_t ptr)
{
    FixedSafeString<cOptionBufSize> str;

    if (!option)
        option = "0x%p";

    s32 strLen = str.format(option, ptr);

    output->write(str.cstr(), strLen);
}

BufferingPrintFormatter::BufferingPrintFormatter()
    : PrintFormatter(nullptr, &mOutput)
    , mOutput(mBuffer, cBufferSize)
{
}

BufferingPrintFormatter::BufferingPrintFormatter(const char* formatStr)
    : PrintFormatter(formatStr, &mOutput)
    , mOutput(mBuffer, cBufferSize)
{
}

template <>
void PrintFormatter::out<u8>(const u8& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("%u", option, output, obj);
}

template <>
void PrintFormatter::out<u16>(const u16& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("%u", option, output, obj);
}

template <>
void PrintFormatter::out<u32>(const u32& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("%u", option, output, obj);
}

template <>
void PrintFormatter::out<u64>(const u64& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("%llu", option, output, obj);
}

template <>
void PrintFormatter::out<s8>(const s8& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("%d", option, output, obj);
}

template <>
void PrintFormatter::out<s16>(const s16& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("%d", option, output, obj);
}

template <>
void PrintFormatter::out<s32>(const s32& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("%d", option, output, obj);
}

template <>
void PrintFormatter::out<s64>(const s64& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("%lld", option, output, obj);
}

template <>
void PrintFormatter::out<f32>(const f32& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("%f", option, output, obj);
}

template <>
void PrintFormatter::out<f64>(const f64& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("%f", option, output, obj);
}

template <>
void PrintFormatter::out<bool>(const bool& obj, const char* option, PrintOutput* output)
{
    if (!option)
    {
        if (obj)
            output->write("true", 4);
        else
            output->write("false", 5);
    }
    else
    {
        PrintFormatter::out<s32>(obj, option, output);
    }
}
/*
template <>
void PrintFormatter::out<char>(const char* obj, const char* option, PrintOutput* output)
{
}

template <>
void PrintFormatter::out<char16>(const char16* obj, const char* option, PrintOutput* output)
{
}
*/
template <>
void PrintFormatter::out<TickSpan>(const TickSpan& obj, const char*, PrintOutput* output)
{
    PrintFormatter::out(obj.toMicroSeconds(), "%lld", output);
    output->write("us", 2);
}

template <>
void PrintFormatter::OutImpl<char, SafeStringBase>::out(const SafeStringBase<char>& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outputString_(option, output, obj.cstr(), obj.calcLength());
}

template <>
void PrintFormatter::OutImpl<char, BufferedSafeStringBase>::out(const BufferedSafeStringBase<char>& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outputString_(option, output, obj.cstr(), obj.calcLength());
}

// TODO: StringBuilder
/*
template <>
void PrintFormatter::OutImpl<char, StringBuilderBase>::out(const StringBuilderBase<char>& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outputString_(option, output, obj.cstr(), obj.getLength());
}
*/

template <>
void PrintFormatter::OutImpl<u8, BitFlag>::out(const BitFlag<u8>& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("0x%x", option, output, obj.getDirect());
}

template <>
void PrintFormatter::OutImpl<u16, BitFlag>::out(const BitFlag<u16>& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("0x%x", option, output, obj.getDirect());
}

template <>
void PrintFormatter::OutImpl<u32, BitFlag>::out(const BitFlag<u32>& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("0x%x", option, output, obj.getDirect());
}

template <>
void PrintFormatter::OutImpl<u64, BitFlag>::out(const BitFlag<u64>& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("0x%llx", option, output, obj.getDirect());
}

} // namespace sead
