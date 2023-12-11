#pragma once

#include <basis/seadAssert.h>
#include <basis/seadTypes.h>

#include <climits>

namespace sead {

class TickSpan
{
public:
    TickSpan()
        : mSpan(0)
    {
    }

    TickSpan(s64 tick)
        : mSpan(tick)
    {
    }

    s64 toS64() const
    {
        return mSpan;
    }

    s64 toNanoSeconds() const;

    // TODO
    s64 toMicroSeconds() const;

    s64 toMilliSeconds() const
    {
        if (LLONG_MIN / 1000 <= mSpan && mSpan < LLONG_MAX / 1000)
            return 1000 * mSpan / cFrequency;

        return 1000 * (mSpan / cFrequency);
    }

    // TODO
    s64 toSeconds() const;

    // TODO
    f32 toFrameRatio(f32) const;

    void setTicks(s64 tick)
    {
        mSpan = tick;
    }

    void setNanoSeconds(s64 nsec);

    void setMicroSeconds(s64 usec)
    {
        SEAD_ASSERT(LLONG_MIN / cFrequency <= usec && usec < LLONG_MAX / cFrequency);
        mSpan = usec * cFrequency / 1000 / 1000;
    }

    void setMilliSeconds(s64 msec)
    {
        SEAD_ASSERT(LLONG_MIN / cFrequency <= msec && msec < LLONG_MAX / cFrequency);
        mSpan = msec * cFrequency / 1000;
    }

    void setSeconds(s64 sec)
    {
        SEAD_ASSERT(LLONG_MIN / cFrequency <= sec && sec < LLONG_MAX / cFrequency);
        mSpan = sec * cFrequency;
    }

    // TODO
    void setFrameRatio(f32, f32);

    const TickSpan& operator+=(TickSpan rhs)
    {
        mSpan += rhs.mSpan;
        return *this;
    }

    const TickSpan& operator-=(TickSpan rhs)
    {
        mSpan -= rhs.mSpan;
        return *this;
    }

    const TickSpan& operator*=(f32 s)
    {
        mSpan = static_cast<s64>(mSpan * s);
        return *this;
    }

    const TickSpan& operator/=(f32 s)
    {
        mSpan = static_cast<s64>(mSpan / s);
        return *this;
    }

    static TickSpan makeFromNanoSeconds(s64 nsec)
    {
        TickSpan span;
        span.setNanoSeconds(nsec);
        return span;
    }

    static TickSpan makeFromMicroSeconds(s64 usec)
    {
        TickSpan span;
        span.setMicroSeconds(usec);
        return span;
    }

    static TickSpan makeFromMilliSeconds(s64 msec)
    {
        TickSpan span;
        span.setMilliSeconds(msec);
        return span;
    }

    static TickSpan makeFromSeconds(s64 sec)
    {
        TickSpan span;
        span.setSeconds(sec);
        return span;
    }

    // TODO
    static TickSpan makeFromFrameRatio(f32, f32);

private:
    static const s64 cFrequency;

private:
    s64 mSpan;
};

} // namespace sead
