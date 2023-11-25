#pragma once

#include <time/seadTickSpan.h>

#ifdef SEAD_PLATFORM_WINDOWS
#include <windows.h>
#endif // SEAD_PLATFORM_WINDOWS

namespace sead {

class TickTime
{
public:
    TickTime()
    {
        setNow();
    }

#ifdef SEAD_PLATFORM_WINDOWS
    void setNow()
    {
        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);

        mTick = li.QuadPart;
    }
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS

    TickSpan diff(const TickTime& t) const
    {
        return mTick - t.mTick;
    }

    TickSpan diffToNow() const
    {
        return TickTime().diff(*this);
    }

    TickTime& operator+=(const TickSpan& rhs)
    {
        mTick += rhs.toS64();
        return *this;
    }

    TickTime& operator-=(const TickSpan& rhs)
    {
        mTick -= rhs.toS64();
        return *this;
    }

    u64 toU64() const
    {
        return mTick;
    }

private:
    u64 mTick;
};

} // namespace sead
