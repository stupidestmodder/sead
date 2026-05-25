#include <basis/seadAssert.h>

#include <basis/seadRawPrint.h>
#include <devenv/seadAssertConfig.h>
#include <prim/seadMemUtil.h>
#include <prim/seadStringUtil.h>

#include <cstdlib>
#include <cstring>

#if defined(SEAD_PLATFORM_LINUX)
#include <cstdio>
#elif defined(SEAD_PLATFORM_MACOSX)
#include <sys/types.h>
#include <sys/sysctl.h>
#include <unistd.h>
#endif // SEAD_PLATFORM

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

static const s32 cFormatBufSize = 0x180;

namespace sead { namespace system {

static bool IsDebuggerPresent()
{
#if defined(SEAD_PLATFORM_LINUX)
    std::FILE* file = std::fopen("/proc/self/status", "r");
    if (!file)
    {
        return false;
    }

    char line[256];
    while (std::fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "TracerPid:", 10) == 0)
        {
            s32 tracerPid = 0;
            std::sscanf(line + 10, "%d", &tracerPid);

            std::fclose(file);
            return tracerPid != 0;
        }
    }

    std::fclose(file);
    return false;
#elif defined(SEAD_PLATFORM_MACOSX)
    s32 mib[4];
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PID;
    mib[3] = getpid();

    struct kinfo_proc info;

    size_t size = sizeof(info);
    sead::MemUtil::fillZero(&info, size);

    if (sysctl(mib, 4, &info, &size, nullptr, 0) != 0)
    {
        return false;
    }

    return (info.kp_proc.p_flag & P_TRACED) != 0;
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM
}

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

    if (!IsDebuggerPresent())
    {
        std::exit(255);
        return;
    }

    SEAD_BREAKPOINT();
}

void HaltWithDetail(const char* pos, s32 line, const char* format, ...)
{
    char tmp[cFormatBufSize];
    MemUtil::fillZero(tmp, cFormatBufSize);

    s32 msgLen = StringUtil::snprintf(
        tmp, cFormatBufSize,
        "\n//======================= PROGRAM HALT =======================//\nSource File: %s\nLine Number: %d\nDescription: ",
        pos, line
    );

    if (msgLen > -1)
    {
        s32 formatLen;

        std::va_list list;
        va_start(list, format);
        formatLen = StringUtil::vsnprintf(tmp + msgLen, cFormatBufSize - msgLen, format, list);
        va_end(list);

        if (formatLen > -1)
        {
            msgLen += formatLen;

            s32 closeFormatLen = StringUtil::snprintf(
                tmp + msgLen, cFormatBufSize - msgLen,
                "\n//============================================================//"
            );

            if (closeFormatLen > 0)
            {
                msgLen += closeFormatLen;

                if (msgLen < cFormatBufSize - 2)
                {
                    tmp[msgLen] = '\n';
                    tmp[msgLen + 1] = '\0';
                    msgLen++;
                }
                else
                {
                    msgLen = cFormatBufSize - 1;
                }
            }
        }
        else
        {
            msgLen = -1;
        }
    }

    tmp[cFormatBufSize - 1] = '\0';

    if (msgLen > -1)
    {
        PrintString(tmp, msgLen);
    }
    else
    {
        PrintString(tmp, static_cast<s32>(std::strlen(tmp)));
    }

    AssertConfig::execCallbacks(tmp);
    Halt();
}

} } // namespace sead::system
