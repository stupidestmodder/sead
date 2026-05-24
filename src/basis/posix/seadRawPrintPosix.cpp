#include <basis/seadRawPrint.h>

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

        std::printf("%s", str);

        FlushPrint();
    }
}

void FlushPrint()
{
    std::fflush(stdout);
}

} } // namespace sead::system
