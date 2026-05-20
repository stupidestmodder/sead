#pragma once

#include <basis/seadTypes.h>
#include <prim/seadSafeString.h>
#include <time/seadCalendarTime.h>

namespace sead {

class CalendarSpan;

namespace DateUtil {

const u32 cMinuteSeconds = 60;
const u32 cHourSeconds = 60 * cMinuteSeconds;
const u32 cDaySeconds = 24 * cHourSeconds;

bool isLeapYear(u32 year);
CalendarTime::Week calcWeekDay(const CalendarTime::Year& year, const CalendarTime::Month& mon, const CalendarTime::Day& day);
void calcSecondToCalendarSpan(CalendarSpan* dst, u64 sec);
bool parseW3CDTFString(CalendarTime* dst, CalendarSpan* timezone, const SafeString& str);

} } // namespace sead::DateUtil
