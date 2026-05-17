#pragma once

#include <basis/seadTypes.h>

namespace sead {

class CalendarSpan
{
public:
    class Day
    {
    public:
        explicit Day(s32 val)
        {
            setValue(val);
        }

        s32 getValue() const { return mValue; }
        void setValue(s32 val) { mValue = val; }

    private:
        s32 mValue;
    };

    class Hour
    {
    public:
        explicit Hour(s32 val)
        {
            setValue(val);
        }

        s32 getValue() const { return mValue; }
        void setValue(s32 val) { mValue = val; }

    private:
        s32 mValue;
    };

    class Minute
    {
    public:
        explicit Minute(s32 val)
        {
            setValue(val);
        }

        s32 getValue() const { return mValue; }
        void setValue(s32 val) { mValue = val; }

    private:
        s32 mValue;
    };

    class Second
    {
    public:
        explicit Second(s32 val)
        {
            setValue(val);
        }

        s32 getValue() const { return mValue; }
        void setValue(s32 val) { mValue = val; }

    private:
        s32 mValue;
    };

public:
    CalendarSpan()
        : mDay(0), mHour(0), mMinute(0), mSecond(0)
    {
    }

    CalendarSpan(const Day& day, const Hour& hour, const Minute& minute, const Second& second)
        : mDay(day), mHour(hour), mMinute(minute), mSecond(second)
    {
    }

    s32 getDays() const { return mDay.getValue(); }
    s32 getHours() const { return mHour.getValue(); }
    s32 getMinutes() const { return mMinute.getValue(); }
    s32 getSeconds() const { return mSecond.getValue(); }

    void setDays(s32 v) { mDay.setValue(v); }
    void setHours(s32 v) { mHour.setValue(v); }
    void setMinutes(s32 v) { mMinute.setValue(v); }
    void setSeconds(s32 v) { mSecond.setValue(v); }

private:
    Day mDay;
    Hour mHour;
    Minute mMinute;
    Second mSecond;
};

} // namespace sead
