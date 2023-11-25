#pragma once

#include <basis/seadTypes.h>

namespace sead {

// TODO: Implement all functions
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
    s64 toMicroSeconds() const;
    s64 toMilliSeconds() const;
    s64 toSeconds() const;
    f32 toFrameRatio(f32) const;

    void setTicks(s64 tick)
    {
        mSpan = tick;
    }

    void setNanoSeconds(s64);
    void setMicroSeconds(s64);
    void setMilliSeconds(s64);
    void setSeconds(s64);
    void setFrameRatio(f32, f32);

    const TickSpan& operator+=(TickSpan);
    const TickSpan& operator-=(TickSpan);
    const TickSpan& operator*=(f32);
    const TickSpan& operator/=(f32);

    static TickSpan makeFromNanoSeconds(s64);
    static TickSpan makeFromMicroSeconds(s64);
    static TickSpan makeFromMilliSeconds(s64);
    static TickSpan makeFromSeconds(s64);
    static TickSpan makeFromFrameRatio(f32, f32);

private:
    static const s64 cFrequency;

    s64 mSpan;
};

} // namespace sead
