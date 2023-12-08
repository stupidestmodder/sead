#pragma once

#include <basis/seadTypes.h>

namespace sead {

class PrintFormatter;

class PrintOutput
{
public:
    virtual ~PrintOutput()
    {
    }

    virtual void write(const char* str, s32 len) = 0;
    void writeLineBreak();

    PrintFormatter& operator<<(PrintFormatter& f);
};

// TODO
class PrintFormatter
{
public:
    template <typename T, template <typename> typename TClass>
    class OutImpl
    {
    public:
        static void out(const TClass<T>& obj, const char* option, PrintOutput* output);
    };

public:
    PrintFormatter(const char* formatStr, PrintOutput* output);

    void setPrintOutput(PrintOutput* output)
    {
        mPrintOutput = output;
    }

    void flush();
    void flushWithLineBreak();

    PrintFormatter& operator,(const s8);
    PrintFormatter& operator,(const u8);
    PrintFormatter& operator,(const s16);
    PrintFormatter& operator,(const u16);
    PrintFormatter& operator,(const s32);
    PrintFormatter& operator,(const u32);

    //template <typename T>
    //PrintFormatter& operator,(const T&);

    PrintFormatter& operator<<(char* str)
    {
        operator<<(const_cast<const char*>(str));
    }

    PrintFormatter& operator<<(const char* str);

    template <typename T>
    static void out(const T& obj, const char* option, PrintOutput* output);

    template <typename T, template <typename> typename TClass>
    static void out(const TClass<T>& obj, const char* option, PrintOutput* output)
    {
        OutImpl<T, TClass>::out(obj, option, output);
    }

    static const u32 cOptionLengthMax = 31;
    static const u32 cOptionBufSize = cOptionLengthMax + 1;

protected:
    bool proceedToFormatMark_(char* option);

    static void outputString_(const char* option, PrintOutput* output, const char* str, s32 strLen);
    static void outputPtr_(const char* option, PrintOutput* output, uintptr_t ptr);
    static bool isQualification_(char c);

protected:
    const char* mFormatStr;
    PrintOutput* mPrintOutput;
    s32 mPos;
    s32 mFormatStrLength;
    bool mIsFormatRestAll;
};

}
