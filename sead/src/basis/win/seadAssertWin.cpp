#include <basis/seadAssert.h>

#include <basis/seadRawPrint.h>
#include <basis/win/seadWindows.h>
#include <devenv/seadAssertConfig.h>
#include <prim/seadMemUtil.h>
#include <prim/seadStringUtil.h>

#include <cstdlib>
#include <cstring>

#if defined(SEAD_COMPILER_MSVC)
    #define SEAD_BREAKPOINT() __debugbreak()
#elif defined(SEAD_COMPILER_CLANG)
    #define SEAD_BREAKPOINT() __builtin_debugtrap()
#elif defined(SEAD_COMPILER_GCC)
    // TODO: Better solution for GCC ?
    #define SEAD_BREAKPOINT() __builtin_trap()
#else
    #error "Unsupported compiler"
#endif // SEAD_COMPILER_MSVC

static const size_t sExceptionStrBufSize = 0x180;
static char sExceptionStrBuf[sExceptionStrBufSize];

static bool sEnableExceptionOnHalt = false;

namespace sead { namespace system {

void DebugBreak()
{
    if (!IsDebuggerPresent())
    {
        return;
    }

    SEAD_BREAKPOINT();
}

void Halt()
{
    FlushPrint();

    if (sEnableExceptionOnHalt)
    {
        throw sExceptionStrBuf;
    }
    else
    {
        if (IsDebuggerPresent())
        {
            SEAD_BREAKPOINT();
            return;
        }

        std::exit(255);
    }
}

void HaltWithDetail(const char* pos, s32 line, const char* format, ...)
{
    char tmp[sExceptionStrBufSize];
    MemUtil::fillZero(tmp, sExceptionStrBufSize);

    s32 msgLen = StringUtil::snprintf(
        tmp, sExceptionStrBufSize,
        "\n//======================= PROGRAM HALT =======================//\nSource File: %s\nLine Number: %d\nDescription: ",
        pos, line
    );

    if (msgLen > -1)
    {
        s32 formatLen;

        std::va_list list;
        va_start(list, format);
        formatLen = StringUtil::vsnprintf(tmp + msgLen, sExceptionStrBufSize - msgLen, format, list);
        va_end(list);

        if (formatLen > -1)
        {
            msgLen += formatLen;

            s32 closeFormatLen = StringUtil::snprintf(
                tmp + msgLen, sExceptionStrBufSize - msgLen,
                "\n//============================================================//"
            );

            if (closeFormatLen > 0)
            {
                msgLen += closeFormatLen;

                if (msgLen < sExceptionStrBufSize - 2)
                {
                    tmp[msgLen] = '\n';
                    tmp[msgLen + 1] = '\0';
                    msgLen++;
                }
                else
                {
                    msgLen = sExceptionStrBufSize - 1;
                }
            }
        }
        else
        {
            msgLen = -1;
        }
    }

    tmp[sExceptionStrBufSize - 1] = '\0';

    if (msgLen > -1)
    {
        PrintString(tmp, msgLen);
    }
    else
    {
        PrintString(tmp, static_cast<s32>(std::strlen(tmp)));
    }

    AssertConfig::execCallbacks(tmp);

    ::strncpy_s(sExceptionStrBuf, sExceptionStrBufSize, tmp, sExceptionStrBufSize);
    sExceptionStrBuf[sExceptionStrBufSize - 1] = '\0';

    Halt();
}

void SetEnableExceptionOnHalt(bool enable)
{
    sEnableExceptionOnHalt = enable;
}

} } // namespace sead::system
