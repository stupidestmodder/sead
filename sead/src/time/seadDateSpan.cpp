#include <time/seadDateSpan.h>

#include <time/seadDateUtil.h>

namespace sead {

DateSpan::DateSpan(s64 timeSpan)
    : mSpan(timeSpan)
{
}

DateSpan::DateSpan(const CalendarSpan::Day& day, const CalendarSpan::Hour& hour, const CalendarSpan::Minute& min, const CalendarSpan::Second& sec)
{
    setTimeImpl_(day.getValue(), hour.getValue(), min.getValue(), sec.getValue());
}

DateSpan::DateSpan(const CalendarSpan& src)
{
    setTimeImpl_(src.getDays(), src.getHours(), src.getMinutes(), src.getSeconds());
}

void DateSpan::getCalendarSpan(CalendarSpan* dst) const
{
    DateUtil::calcSecondToCalendarSpan(dst, mSpan);
}

s64 DateSpan::set(const CalendarSpan& src)
{
    return setTimeImpl_(src.getDays(), src.getHours(), src.getMinutes(), src.getSeconds());
}

s64 DateSpan::set(const CalendarSpan::Day& day, const CalendarSpan::Hour& hour, const CalendarSpan::Minute& min, const CalendarSpan::Second& sec)
{
    return setTimeImpl_(day.getValue(), hour.getValue(), min.getValue(), sec.getValue());
}

s64 DateSpan::setTimeImpl_(s32 d, s32 h, s32 m, s32 s)
{
    mSpan  = static_cast<s64>(d) * DateUtil::cDaySeconds;
    mSpan += static_cast<s64>(h) * DateUtil::cHourSeconds;
    mSpan += static_cast<s64>(m) * DateUtil::cMinuteSeconds;
    mSpan += static_cast<s64>(s);
    return mSpan;
}

} // namespace sead
