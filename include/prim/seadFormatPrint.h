#pragma once

#include <basis/seadTypes.h>
#include <prim/seadSafeString.h>
#include <stream/seadBufferStream.h>
#include <stream/seadPrintStream.h>

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

class BufferingPrintOutput : public PrintOutput
{
public:
    BufferingPrintOutput(char* start, u32 size)
        : PrintOutput()
        , mStreamSrc(PrintStreamSrc::instance(), start, size)
    {
    }

    ~BufferingPrintOutput() override;

    void write(const char* str, s32 len) override;

protected:
    BufferMultiByteNullTerminatedTextWriteStreamSrc mStreamSrc;
};

class PrintFormatter
{
protected:
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

    PrintFormatter& operator,(const s8 obj)
    {
        return operator<<(obj);
    }

    PrintFormatter& operator,(const u8 obj)
    {
        return operator<<(obj);
    }

    PrintFormatter& operator,(const s16 obj)
    {
        return operator<<(obj);
    }

    PrintFormatter& operator,(const u16 obj)
    {
        return operator<<(obj);
    }

    PrintFormatter& operator,(const s32 obj)
    {
        return operator<<(obj);
    }

    PrintFormatter& operator,(const u32 obj)
    {
        return operator<<(obj);
    }

    template <typename T>
    PrintFormatter& operator,(const T& obj)
    {
        return operator<<(obj);
    }

    PrintFormatter& operator<<(char* str)
    {
        return operator<<(const_cast<const char*>(str));
    }

    PrintFormatter& operator<<(const char* str);

    template <typename T>
    PrintFormatter& operator<<(const T& obj)
    {
        char option[cOptionBufSize];

        bool end = proceedToFormatMark_(option);
        if (end)
            PrintFormatter::out<T>(obj, option[0] != '\0' ? option : nullptr, mPrintOutput);

        return *this;
    }

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

    static bool isQualification_(char c)
    {
        return c == '+' || c == '-' || c == ' ' || c == '#' || c == 'h' || c == 'l' || c == 'L' || c == '.' || c > '/' && c < ':';
    }

    template <typename T>
    static void outSimpleObject_(const char* defaultOption, const char* userOption, PrintOutput* output, const T& obj)
    {
        FixedSafeString<cOptionBufSize> str;

        s32 strLen = 0;
        if (userOption)
            strLen = str.format(userOption, obj);
        else
            strLen = str.format(defaultOption, obj);

        output->write(str.cstr(), strLen);
    }

protected:
    const char* mFormatStr;
    PrintOutput* mPrintOutput;
    s32 mPos;
    s32 mFormatStrLength;
    bool mIsFormatRestAll;
};

class BufferingPrintFormatter : public PrintFormatter
{
public:
    BufferingPrintFormatter();
    explicit BufferingPrintFormatter(const char* formatStr);

    static const u32 cBufferSize = 128;

protected:
    BufferingPrintOutput mOutput;
    char mBuffer[cBufferSize];
};

}
