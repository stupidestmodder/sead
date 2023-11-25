#include <basis/seadAssert.h>

#include <basis/seadRawPrint.h>
#include <devenv/seadAssertConfig.h>
#include <prim/seadMemUtil.h>
#include <prim/seadStringUtil.h>

#include <windows.h>

#include <cstdlib>
#include <cstring>

#ifdef SEAD_COMPILER_MSVC
    #define SEAD_BREAKPOINT() __debugbreak()
#elif defined(SEAD_COMPILER_CLANG)
    #define SEAD_BREAKPOINT() __builtin_debugtrap()
#elif defined(SEAD_COMPILER_GCC)
    // TODO: Better solution for GCC ?
    #define SEAD_BREAKPOINT() __builtin_trap()
#else
    #error "Unsupported compiler"
#endif // SEAD_COMPILER_MSVC

static const size_t cStrBufSize = 0x180;

static bool sEnableExceptionOnHalt = false;
static char sExceptionStrBuf[cStrBufSize];

namespace sead { namespace system {

void DebugBreak()
{
    if (!IsDebuggerPresent())
        return;

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
    char strBuf[cStrBufSize];

    MemUtil::fillZero(strBuf, cStrBufSize);

    s32 strBufSize = StringUtil::snprintf(strBuf, cStrBufSize,
        "\n//================= PROGRAM HALT ==================//\nSource File: %s\nLine  Number: %d\nDescription: ",
        pos, line
    );

    if (strBufSize >= 0)
    {
        std::va_list list;
        va_start(list, format);

        s32 substrSize = StringUtil::vsnprintf(strBuf + strBufSize, cStrBufSize - strBufSize, format, list);

        va_end(list);

        if (substrSize >= 0)
        {
            strBufSize += substrSize;
            substrSize = StringUtil::snprintf(strBuf + strBufSize, cStrBufSize - strBufSize, "\n//=================================================//");

            if (substrSize > 0)
            {
                strBufSize += substrSize;
                
                if (strBufSize < cStrBufSize - 2)
                {
                    strBuf[strBufSize] = '\n';
                    strBuf[strBufSize + 1] = '\0';
                    strBufSize++;
                }
                else
                {
                    strBufSize = cStrBufSize - 1;
                }
            }
        }
        else
        {
            strBufSize = -1;
        }
    }

    strBuf[cStrBufSize - 1] = '\0';

    if (strBufSize >= 0)
        PrintString(strBuf, strBufSize);
    else
        PrintString(strBuf, static_cast<s32>(std::strlen(strBuf)));

    AssertConfig::execCallbacks(strBuf);

    strncpy_s(sExceptionStrBuf, cStrBufSize, strBuf, cStrBufSize);
    sExceptionStrBuf[cStrBufSize - 1] = '\0';

    Halt();
}

void SetEnableExceptionOnHalt(bool enable)
{
    sEnableExceptionOnHalt = enable;
}

} } // namespace sead::system
