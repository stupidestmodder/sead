#pragma once

#include <time/seadCalendarSpan.h>

namespace sead {

class DateSpan
{
public:
    explicit DateSpan(s64 timeSpan);
    DateSpan(const CalendarSpan::Day& day, const CalendarSpan::Hour& hour, const CalendarSpan::Minute& min, const CalendarSpan::Second& sec);
    DateSpan(const CalendarSpan& src);

    void getCalendarSpan(CalendarSpan* dst) const;

    void setSeconds(s64 sec) { mSpan = sec; }
    s64 set(const CalendarSpan& src);
    s64 set(const CalendarSpan::Day& day, const CalendarSpan::Hour& hour, const CalendarSpan::Minute& min, const CalendarSpan::Second& sec);

    s64 getDays() const;
    s64 getHours() const;
    s64 getMinutes() const;
    s64 getSeconds() const { return mSpan; }

    const DateSpan& operator+=(DateSpan);
    const DateSpan& operator-=(DateSpan);
    const DateSpan& operator*=(f32);
    const DateSpan& operator/=(f32);

private:
    s64 setTimeImpl_(s32 d, s32 h, s32 m, s32 s);

private:
    s64 mSpan;
};

} // namespace sead

