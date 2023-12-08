#include <prim/seadFormatPrint.h>

#include <prim/seadSafeString.h>

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
    }
    else
    {
        char option[cOptionBufSize];

        bool end = proceedToFormatMark_(option);
        if (end)
        {
            const char* optionStr;

            if (option[0] != '\0')
                optionStr = option;
            else
                optionStr = nullptr;

            PrintFormatter::outputString_(optionStr, mPrintOutput, str, -1);
        }
    }

    return *this;
}

void PrintFormatter::outputPtr_(const char* option, PrintOutput* output, uintptr_t ptr)
{
    FixedSafeString<cOptionBufSize> str;

    if (!option)
        option = "0x%p";

    s32 strLen = str.format(option, ptr);

    output->write(str.cstr(), strLen);
}

} // namespace sead
