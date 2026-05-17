#pragma once

#include <basis/seadTypes.h>
#include <prim/seadSafeString.h>

namespace sead { namespace hostio {

class Context;
struct PropertyEvent;
class Reflexible;

} // namespace hostio

class CalendarTime
{
public:
    class Year
    {
    public:
        explicit Year(u32 val)
        {
            setValue(val);
        }

        u32 getValue() const { return mValue; }
        void setValue(u32 val);

    private:
        u32 mValue;
    };

    class Day
    {
    public:
        explicit Day(u32 val)
        {
            setValue(val);
        }

        u32 getValue() const { return mValue; }
        void setValue(u32 val);

    private:
        u32 mValue;
    };

    class Hour
    {
    public:
        explicit Hour(u32 val)
        {
            setValue(val);
        }

        u32 getValue() const { return mValue; }
        void setValue(u32 val);

    private:
        u32 mValue;
    };

    class Minute
    {
    public:
        explicit Minute(u32 val)
        {
            setValue(val);
        }

        u32 getValue() const { return mValue; }
        void setValue(u32 val);

    private:
        u32 mValue;
    };

    class Second
    {
    public:
        explicit Second(u32 val)
        {
            setValue(val);
        }

        u32 getValue() const { return mValue; }
        void setValue(u32 val);

    private:
        u32 mValue;
    };

    class Month
    {
    public:
        explicit Month(u32 m);

        void setValueOneOrigin(u32 v);
        s32 getValueOneOrigin() const { return mValue; }

        s32 addSelf(u32 rhs);
        s32 subSelf(u32 rhs);
        s32 sub(Month rhs) const;

        const SafeString getString() const { return makeStringOneOrigin(getValueOneOrigin()); }

        static Month makeFromValueOneOrigin(u32 m);
        static SafeString makeStringOneOrigin(u32 idx);

    private:
        s32 mValue;
    };

    enum class Week
    {
        eSunday = 0,
        eMonday,
        eTuesday,
        eWednesday,
        eThursday,
        eFriday,
        eSaturday,
        eMax
    };

    class Date
    {
    public:
        Date(const Year& y = cDefaultYear, const Month& m = cDefaultMonth, const Day& d = cDefaultDay);

        void calcWeek();

        Year mYear;
        Month mMonth;
        Day mDay;
        Week mWeek;
    };

    class Time
    {
    public:
        Time(const Hour& h = cDefaultHour, const Minute& m = cDefaultMinute, const Second& s = cDefaultSecond);

        Hour mHour;
        Minute mMinute;
        Second mSecond;
    };

    static const u32 cStandardYear = 1970;
    static const u32 cStandardDay = 1;
    static const u32 cStandardHour = 0;
    static const u32 cStandardMinute = 0;
    static const u32 cStandardSecond = 0;

    static const Month cMonth_Jan;
    static const Month cMonth_Feb;
    static const Month cMonth_Mar;
    static const Month cMonth_Apr;
    static const Month cMonth_May;
    static const Month cMonth_Jun;
    static const Month cMonth_Jul;
    static const Month cMonth_Aug;
    static const Month cMonth_Sep;
    static const Month cMonth_Oct;
    static const Month cMonth_Nov;
    static const Month cMonth_Dec;

    static const Year cDefaultYear;
    static const Month cDefaultMonth;
    static const Day cDefaultDay;
    static const Hour cDefaultHour;
    static const Minute cDefaultMinute;
    static const Second cDefaultSecond;

public:
    CalendarTime(const Date& date, const Time& time);
    CalendarTime(const Year& year = cDefaultYear, const Month& mon = cDefaultMonth, const Day& day = cDefaultDay,
                 const Hour& hour = cDefaultHour, const Minute& min = cDefaultMinute, const Second& sec = cDefaultSecond);

    Time* getTimePtr() { return &mTime; }

    const Date& getDate() const { return mDate; }
    const Time& getTime() const { return mTime; }
    void setDate(const Date& date);
    void setTime(const Time& time) { mTime = time; }

    u32 getYear() const { return mDate.mYear.getValue(); }
    const Month& getMonth() const { return mDate.mMonth; }
    u32 getDay() const { return mDate.mDay.getValue(); }
    Week getWeekDay() const { return mDate.mWeek; }
    u32 getHour() const { return mTime.mHour.getValue(); }
    u32 getMinute() const { return mTime.mMinute.getValue(); }
    u32 getSecond() const { return mTime.mSecond.getValue(); }
    u32 getYearDays() const;

#if defined(SEAD_TARGET_DEBUG)
    void genHostIOMessage(hostio::Context* context) const;
    void listenHostIOPropertyEvent(const hostio::PropertyEvent* event, hostio::Reflexible*);
#endif // SEAD_TARGET_DEBUG

private:
    static void makeWeekDayNameLabel_(BufferedSafeString* str, Week week);

private:
    Date mDate;
    Time mTime;
};

} // namespace sead
