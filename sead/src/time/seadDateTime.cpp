#include <time/seadDateTime.h>

#include <time/seadDateUtil.h>

#if defined(SEAD_PLATFORM_WINDOWS)
#include <ctime>
#endif // SEAD_PLATFORM_WINDOWS

static const s32 sDaysOfMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static const s32 sDaysOfNewYearsDay[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

static s32 calcDateToDays(const sead::CalendarTime::Date& date)
{
    u32 dayNum = date.mDay.getValue() - 1;
    u32 mon = date.mMonth.getValueOneOrigin();
    SEAD_ASSERT_MSG(1 <= mon && mon <= 12, "wrong month. correct range is [1, 12]. your param %d", mon);
    SEAD_ASSERT_MSG(date.mYear.getValue() >= 1970, "wrong year. your param %d, must after 1970.", date.mYear.getValue());

    dayNum += sDaysOfNewYearsDay[mon - 1];
    if (mon > 2 && sead::DateUtil::isLeapYear(date.mYear.getValue()))
    {
        dayNum++;
    }

    dayNum += (date.mYear.getValue() - 1970) * 365;
    for (u32 year = 1970; year < date.mYear.getValue(); year++)
    {
        if (sead::DateUtil::isLeapYear(year))
        {
            dayNum++;
        }
    }

    return dayNum;
}

static u64 calcTimeToSeconds(const sead::CalendarTime::Time& time)
{
    return (time.mHour.getValue() * sead::DateUtil::cMinuteSeconds + time.mMinute.getValue()) * sead::DateUtil::cMinuteSeconds + time.mSecond.getValue();
}

static u64 calcDateTimeToSeconds(const sead::CalendarTime::Date& date, const sead::CalendarTime::Time& time)
{
#if defined(SEAD_TARGET_DEBUG)
    u32 year = date.mYear.getValue();
    u32 mon = date.mMonth.getValueOneOrigin();
    u32 daysOfMonth;
    SEAD_ASSERT_MSG(1 <= mon && mon <= 12, "wrong month. correct range is [1, 12]. your param %d", mon);

    if (mon == 2 && sead::DateUtil::isLeapYear(year))
    {
        daysOfMonth = sDaysOfMonth[mon - 1] + 1;
    }
    else
    {
        daysOfMonth = sDaysOfMonth[mon - 1];
    }

    SEAD_ASSERT_MSG(date.mDay.getValue() <= daysOfMonth, "wrong day, correct range is [1, %d] (when year %4d month %2d).",
                    daysOfMonth, year, mon);
#endif // SEAD_TARGET_DEBUG

    return (calcDateToDays(date) * sead::DateUtil::cDaySeconds) + calcTimeToSeconds(time);
}

static u32 calcDaysToYear(u32* days)
{
    SEAD_ASSERT(days);

    u32 year = 1970;
    u32 dayCnt = 0;
    u32 dayMem = 0;
    while (dayCnt <= *days)
    {
        dayMem = dayCnt;
        if (sead::DateUtil::isLeapYear(year))
        {
            dayCnt += 366;
        }
        else
        {
            dayCnt += 365;
        }

        year++;
    }

    *days -= dayMem;
    return year - 1;
}

static s32 calcDaysToMonth(u32* days, u32 year)
{
    SEAD_ASSERT(days);
    SEAD_ASSERT_MSG(*days <= 365, "wrong days. correct range is [0, 365]. your param %d", *days);

    u32 month;
    u32 dayMem = 0;
    u32 dayCnt = 0;
    for (month = 0; month < 12; month++)
    {
        dayMem = dayCnt;
        if (month == 1 && sead::DateUtil::isLeapYear(year))
        {
            dayCnt += sDaysOfMonth[month] + 1;
        }
        else
        {
            dayCnt += sDaysOfMonth[month];
        }

        if (*days < dayCnt)
        {
            break;
        }
    }

    *days -= dayMem;
    return month + 1;
}

namespace sead {

DateTime::DateTime(u64 timeCount)
    : mRealTimeClock(timeCount)
{
}

DateTime::DateTime(const CalendarTime::Year& year, const CalendarTime::Month& mon, const CalendarTime::Day& day,
                   const CalendarTime::Hour& hour, const CalendarTime::Minute& min, const CalendarTime::Second& sec)
{
    setUnixTime(year, mon, day, hour, min, sec);
}

DateTime::DateTime(const CalendarTime& calendar)
{
    setUnixTime(calendar);
}

u64 DateTime::setNow()
{
#if defined(SEAD_PLATFORM_WINDOWS)
    _tzset();

    time_t now = ::time(nullptr);
    SEAD_ASSERT_MSG(now >= 0, "failed get current time.");

    long timezone;
    ::_get_timezone(&timezone);
    int daylight;
    ::_get_daylight(&daylight);

    if (daylight != 0)
    {
        timezone -= DateUtil::cHourSeconds;
    }

    if (now >= 0)
    {
        mRealTimeClock = now - timezone;
    }
    else
    {
        mRealTimeClock = 0;
    }

    return mRealTimeClock;
#elif defined(SEAD_PLATFORM_SDL)
    // TODO
    SEAD_ASSERT(false);
    return 0;
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
}

DateSpan DateTime::diff(DateTime t) const
{
    DateSpan ds(mRealTimeClock - t.mRealTimeClock);
    return ds;
}

DateSpan DateTime::diffToNow() const
{
    DateTime now(0);
    now.setNow();
    return now.diff(*this);
}

u64 DateTime::setUnixTime(const CalendarTime& src)
{
    mRealTimeClock = calcDateTimeToSeconds(src.getDate(), src.getTime());
    return mRealTimeClock;
}

u64 DateTime::setUnixTime(const CalendarTime::Year& year, const CalendarTime::Month& mon, const CalendarTime::Day& day,
                          const CalendarTime::Hour& hour, const CalendarTime::Minute& min, const CalendarTime::Second& sec)
{
    CalendarTime::Date date(year, mon, day);
    CalendarTime::Time time(hour, min, sec);
    mRealTimeClock = calcDateTimeToSeconds(date, time);
    return mRealTimeClock;
}

void DateTime::getCalendarTime(CalendarTime* calendar) const
{
    u32 day = static_cast<u32>(mRealTimeClock / DateUtil::cDaySeconds);
    u32 y = calcDaysToYear(&day);
    u32 m = calcDaysToMonth(&day, y);
    u32 d = day + 1;

    CalendarTime::Time time;
    s64 sec = mRealTimeClock % DateUtil::cDaySeconds;
    time.mHour.setValue((sec % DateUtil::cDaySeconds) / DateUtil::cHourSeconds);
    time.mMinute.setValue((sec % DateUtil::cHourSeconds) / DateUtil::cMinuteSeconds);
    time.mSecond.setValue(sec % DateUtil::cMinuteSeconds);

    if (calendar)
    {
        calendar->setDate(CalendarTime::Date(CalendarTime::Year(y), CalendarTime::Month::makeFromValueOneOrigin(m), CalendarTime::Day(d)));
        calendar->setTime(time);
    }
}

const DateSpan operator-(DateTime lhs, DateTime rhs)
{
    DateSpan ds(lhs.getUnixTime() - rhs.getUnixTime());
    return ds;
}

const DateTime operator-(DateTime lhs, DateSpan rhs)
{
    DateTime dt(lhs.getUnixTime() - rhs.getSeconds());
    return dt;
}

const DateTime operator+(DateTime lhs, DateSpan rhs)
{
    DateTime dt(lhs.getUnixTime() + rhs.getSeconds());
    return dt;
}

} // namespace sead
