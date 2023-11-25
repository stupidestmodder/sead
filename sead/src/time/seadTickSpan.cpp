#include <time/seadTickSpan.h>

#include <basis/seadAssert.h>

#ifdef SEAD_PLATFORM_WINDOWS
#include <windows.h>
#endif // SEAD_PLATFORM_WINDOWS

static s64 getFrequency_()
{
#ifdef SEAD_PLATFORM_WINDOWS
    LARGE_INTEGER li;

    if (!QueryPerformanceFrequency(&li))
    {
        SEAD_ASSERT_MSG(false, "QueryPerformanceFrequency failed.\n");
        return 100000;
    }

    return li.QuadPart;
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
}

namespace sead {

const s64 TickSpan::cFrequency = getFrequency_();

} // namespace sead
