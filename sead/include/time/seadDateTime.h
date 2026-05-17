#pragma once

#include <time/seadCalendarTime.h>
#include <time/seadDateSpan.h>

namespace sead {

class DateTime
{
public:
    explicit DateTime(u64 timeCount);
    DateTime(const CalendarTime::Year& year, const CalendarTime::Month& mon, const CalendarTime::Day& day,
             const CalendarTime::Hour& hour, const CalendarTime::Minute& min, const CalendarTime::Second& sec);
    DateTime(const CalendarTime& calendar);

    u64 setNow();

    DateSpan diff(DateTime t) const;
    DateSpan diffToNow() const;

    u64 setUnixTime(const CalendarTime& src);
    u64 setUnixTime(const CalendarTime::Year& year, const CalendarTime::Month& mon, const CalendarTime::Day& day,
                    const CalendarTime::Hour& hour, const CalendarTime::Minute& min, const CalendarTime::Second& sec);

    void getCalendarTime(CalendarTime* calendar) const;

    u64 getUnixTime() const { return mRealTimeClock; }
    void setUnixTime(u64 time) { mRealTimeClock = time; }

    DateTime& operator+=(DateSpan rhs)
    {
        mRealTimeClock += rhs.getSeconds();
        return *this;
    }

    DateTime& operator-=(DateSpan rhs)
    {
        mRealTimeClock -= rhs.getSeconds();
        return *this;
    }

protected:
    u64 mRealTimeClock;
};

const DateSpan operator-(DateTime lhs, DateTime rhs);
const DateTime operator-(DateTime lhs, DateSpan rhs);
const DateTime operator+(DateTime lhs, DateSpan rhs);

} // namespace sead
