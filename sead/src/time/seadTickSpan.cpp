#include <time/seadTickSpan.h>

#include <basis/seadAssert.h>

#ifdef SEAD_PLATFORM_WINDOWS
#include <basis/win/seadWindows.h>
#endif // SEAD_PLATFORM_WINDOWS

static s64 getFrequency_()
{
#ifdef SEAD_PLATFORM_WINDOWS
    LARGE_INTEGER freq;

    bool ret = QueryPerformanceFrequency(&freq);
    if (!ret)
    {
        SEAD_ASSERT_MSG(false, "QueryPerformanceFrequency failed.\n");
        return 100000;
    }

    return freq.QuadPart;
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
}

namespace sead {

const s64 TickSpan::cFrequency = getFrequency_();

s64 TickSpan::toNanoSeconds() const
{
    s64 spanAbs = mSpan > 0 ? mSpan : -mSpan;

    if (spanAbs < LLONG_MAX / 1000 / 1000 / 1000)
        return mSpan * 1000 * 1000 * 1000 / cFrequency;
    else if (spanAbs < LLONG_MAX / 1000 / 1000)
        return mSpan * 1000 * 1000 / cFrequency * 1000;
    else if (spanAbs < LLONG_MAX / 1000)
        return mSpan * 1000 / cFrequency * 1000 * 1000;
    else
        return mSpan / cFrequency * 1000 * 1000 * 1000;
}

void TickSpan::setNanoSeconds(s64 nsec)
{
    s64 nsecAbs = nsec > 0 ? nsec : -nsec;

    if (nsecAbs <= LLONG_MAX / cFrequency)
        mSpan = cFrequency * nsec / 1000 / 1000 / 1000;
    else if (nsecAbs <= LLONG_MAX / cFrequency * 1000)
        mSpan = cFrequency * (nsec / 1000) / 1000 / 1000;
    else if (nsecAbs <= LLONG_MAX / cFrequency * 1000 * 1000)
        mSpan = cFrequency * (nsec / 1000 / 1000) / 1000;
    else
        mSpan = cFrequency * (nsec / 1000 / 1000 / 1000);
}

} // namespace sead
