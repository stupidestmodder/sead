#include <basis/seadRawPrint.h>

#include <prim/seadScopedLock.h>
#include <thread/seadCriticalSection.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdio>

static sead::CriticalSection sPrintCriticalSection;

namespace sead { namespace system {

void PrintStringImpl(const char* str, s32 len)
{
    if (len <= 0)
        return;

    ScopedLock<CriticalSection> lock(&sPrintCriticalSection);

    OutputDebugStringA(str);
    std::printf("%s", str);

    FlushPrint();
}

void FlushPrint()
{
    std::fflush(stdout);
}

} } // namespace sead::system
