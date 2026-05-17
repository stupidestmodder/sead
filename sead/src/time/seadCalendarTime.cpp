#include <time/seadCalendarTime.h>

#include <basis/seadAssert.h>
#include <container/seadSafeArray.h>
#include <hostio/seadHostIOContext.h>
#include <time/seadDateUtil.h>

static const s32 sDaysFromNewYearsDay[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

static u32 calcDaysFromNewYearsDay(const sead::CalendarTime::Date& date)
{
    u32 mon = date.mMonth.getValueOneOrigin();
    SEAD_ASSERT_MSG(1 <= mon && mon <= 12, "wrong month. correct range is [1, 12]. your param %d", mon);

    u32 day = date.mDay.getValue() + sDaysFromNewYearsDay[mon - 1];
    if (mon > 2 && sead::DateUtil::isLeapYear(date.mYear.getValue()))
    {
        day++;
    }

    return day;
}

namespace sead {

const CalendarTime::Month CalendarTime::cMonth_Jan(1);
const CalendarTime::Month CalendarTime::cMonth_Feb(2);
const CalendarTime::Month CalendarTime::cMonth_Mar(3);
const CalendarTime::Month CalendarTime::cMonth_Apr(4);
const CalendarTime::Month CalendarTime::cMonth_May(5);
const CalendarTime::Month CalendarTime::cMonth_Jun(6);
const CalendarTime::Month CalendarTime::cMonth_Jul(7);
const CalendarTime::Month CalendarTime::cMonth_Aug(8);
const CalendarTime::Month CalendarTime::cMonth_Sep(9);
const CalendarTime::Month CalendarTime::cMonth_Oct(10);
const CalendarTime::Month CalendarTime::cMonth_Nov(11);
const CalendarTime::Month CalendarTime::cMonth_Dec(12);

const CalendarTime::Year CalendarTime::cDefaultYear(cStandardYear);
const CalendarTime::Month CalendarTime::cDefaultMonth = CalendarTime::cMonth_Jan;
const CalendarTime::Day CalendarTime::cDefaultDay(cStandardDay);
const CalendarTime::Hour CalendarTime::cDefaultHour(cStandardHour);
const CalendarTime::Minute CalendarTime::cDefaultMinute(cStandardMinute);
const CalendarTime::Second CalendarTime::cDefaultSecond(cStandardSecond);

void CalendarTime::Year::setValue(u32 val)
{
    mValue = val;
}

void CalendarTime::Day::setValue(u32 val)
{
    SEAD_ASSERT_MSG(1 <= val && val <= 31, "wrong day. correct range is [1, 31]. your param %d", val);
    mValue = val;
}

void CalendarTime::Hour::setValue(u32 val)
{
    SEAD_ASSERT_MSG(val <= 23, "wrong hour. correct range is [0, 23]. your param %d", val);
    mValue = val;
}

void CalendarTime::Minute::setValue(u32 val)
{
    SEAD_ASSERT_MSG(val <= 59, "wrong minute. correct range is [0, 59]. your param %d", val);
    mValue = val;
}

void CalendarTime::Second::setValue(u32 val)
{
    SEAD_ASSERT_MSG(val <= 61, "wrong second. correct range is [0, 61]. your param %d", val);
    mValue = val;
}

CalendarTime::Month::Month(u32 m)
{
    setValueOneOrigin(m);
}

void CalendarTime::Month::setValueOneOrigin(u32 v)
{
    SEAD_ASSERT_MSG(1 <= v && v <= 12, "wrong month. correct range is [1, 12]. your param %d", v);
    mValue = v;
}

s32 CalendarTime::Month::addSelf(u32 rhs)
{
    mValue = mValue - 1 + rhs;
    s32 ret = mValue / 12;
    mValue = mValue % 12 + 1;
    SEAD_ASSERT(1 <= mValue && mValue <= 12);
    return ret;
}

s32 CalendarTime::Month::subSelf(u32 rhs)
{
    s32 val = mValue - 1 - rhs;
    s32 ret = (val - 12) / 12;
    val = mValue + 11;
    mValue = (val - rhs % 12) % 12 + 1;
    SEAD_ASSERT(1 <= mValue && mValue <= 12);
    return ret;
}

s32 CalendarTime::Month::sub(Month rhs) const
{
    return getValueOneOrigin() - rhs.getValueOneOrigin();
}

CalendarTime::Month CalendarTime::Month::makeFromValueOneOrigin(u32 m)
{
    SEAD_ASSERT(1 <= m && m <= 12);
    return Month(m);
}

SafeString CalendarTime::Month::makeStringOneOrigin(u32 idx)
{
    SEAD_ASSERT_MSG(1 <= idx && idx <= 12, "wrong month. correct range is [1, 12]. your param %d", idx);

    switch (idx)
    {
        case 1:
            return "Jan";
        case 2:
            return "Feb";
        case 3:
            return "Mar";
        case 4:
            return "Apr";
        case 5:
            return "May";
        case 6:
            return "Jun";
        case 7:
            return "Jul";
        case 8:
            return "Aug";
        case 9:
            return "Sep";
        case 10:
            return "Oct";
        case 11:
            return "Nov";
        case 12:
        default:
            return "Dec";
    }
}

CalendarTime::Date::Date(const Year& y, const Month& m, const Day& d)
    : mYear(y)
    , mMonth(m)
    , mDay(d)
    , mWeek(DateUtil::calcWeekDay(y, m, d))
{
}

void CalendarTime::Date::calcWeek()
{
    mWeek = DateUtil::calcWeekDay(mYear, mMonth, mDay);
}

CalendarTime::Time::Time(const Hour& h, const Minute& m, const Second& s)
    : mHour(h)
    , mMinute(m)
    , mSecond(s)
{
}

CalendarTime::CalendarTime(const Date& date, const Time& time)
    : mDate(date)
    , mTime(time)
{
    mDate.calcWeek();
}

CalendarTime::CalendarTime(const Year& year, const Month& mon, const Day& day, const Hour& hour, const Minute& min, const Second& sec)
    : mDate(year, mon, day)
    , mTime(hour, min, sec)
{
}

void CalendarTime::setDate(const Date& date)
{
    mDate = date;
    mDate.calcWeek();
}

u32 CalendarTime::getYearDays() const
{
    return calcDaysFromNewYearsDay(mDate);
}

#if defined(SEAD_TARGET_DEBUG)
void CalendarTime::genHostIOMessage(hostio::Context* context) const
{
    context->startLayout("Layout = Wrap");
    context->genLabel("year", 0, "");
}
#endif // SEAD_TARGET_DEBUG

void CalendarTime::makeWeekDayNameLabel_(BufferedSafeString* str, Week week)
{
#if defined(SEAD_USE_JAPANESE)
    static const SafeArray<const char*, 7> cWeekDayNames = { "日", "月", "火", "水", "木", "金", "土" };
    str->format("曜日:%s", cWeekDayNames[static_cast<s32>(week)]);
#else
    static const SafeArray<const char*, 7> cWeekDayNames = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    str->format("weekday:%s", cWeekDayNames[static_cast<s32>(week)]);
#endif // SEAD_USE_JAPANESE
}

} // namespace sead
