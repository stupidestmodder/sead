#include <basis/seadRawPrint.h>

#include <basis/win/seadWindows.h>
#include <prim/seadScopedLock.h>
#include <thread/seadCriticalSection.h>

#include <cstdio>

static sead::CriticalSection& GetPrintCriticalSection()
{
    static sead::CriticalSection sPrintCriticalSection;
    return sPrintCriticalSection;
}

namespace sead { namespace system {

void PrintStringImpl(const char* str, s32 len)
{
    if (len > 0)
    {
        ScopedLock<CriticalSection> lock(&GetPrintCriticalSection());

        OutputDebugStringA(str);
        std::printf("%s", str);

        FlushPrint();
    }
}

void FlushPrint()
{
    std::fflush(stdout);
}

} } // namespace sead::system
