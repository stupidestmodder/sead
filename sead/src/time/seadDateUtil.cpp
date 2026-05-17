#include <time/seadDateUtil.h>

#include <prim/seadStringUtil.h>
#include <time/seadCalendarSpan.h>

// static bool parseW3CDTFSubString_(
//     bool* isSuccess, u32* value, sead::SafeString* str, s32* strLength, char* separatorOut, s32 parseLength,
//     const sead::SafeString& separatorChars, bool allowSeparateNull, u32 valueMin, u32 valueMax)
// {
//     if (*strLength < parseLength)
//     {
//         *isSuccess = false;
//         return true;
//     }

//     char separator = str->at(parseLength);
//     if (!separatorChars.include(separator) && (!allowSeparateNull || separator != sead::SafeString::cNullChar))
//     {
//         *isSuccess = false;
//         return true;
//     }

//     sead::FixedSafeString<8> buf;
//     buf.copy(*str, parseLength);

//     bool parseRet = sead::StringUtil::tryParseU32(value, buf, sead::StringUtil::CardinalNumber::eBase10);
//     if (!parseRet)
//     {
//         *isSuccess = false;
//         return true;
//     }

//     if (valueMin > *value || *value > valueMax)
//     {
//         *isSuccess = false;
//         return true;
//     }

//     if (separator == sead::SafeString::cNullChar)
//     {
//         *isSuccess = true;
//         return true;
//     }

//     *str = str->getPart(parseLength + 1);
//     *strLength -= parseLength + 1;
//     if (separatorOut)
//     {
//         *separatorOut = separator;
//     }

//     return false;
// }

// static bool parseW3CDTFStringImpl_(
//     u32* year, u32* month, u32* day, u32* hour, u32* minute, u32* second,
//     s32* tzHour, s32* tzMinute, const sead::SafeString& str)
// {
//     s32 length = str.calcLength();
//     sead::SafeString partStr(str);
//     bool isSuccess = true;
//     char separator;

//     if (parseW3CDTFSubString_(&isSuccess, year, &partStr, &length, &separator, 4, "-", true, 0, 0xFFFFFFFF))
//     {
//         return isSuccess;
//     }

//     if (parseW3CDTFSubString_(&isSuccess, month, &partStr, &length, &separator, 2, "-", true, 1, 12))
//     {
//         return isSuccess;
//     }

//     if (parseW3CDTFSubString_(&isSuccess, day, &partStr, &length, &separator, 2, "T", true, 1, 31))
//     {
//         return isSuccess;
//     }

//     if (parseW3CDTFSubString_(&isSuccess, hour, &partStr, &length, &separator, 2, ":", false, 0, 23))
//     {
//         return isSuccess;
//     }

//     if (parseW3CDTFSubString_(&isSuccess, minute, &partStr, &length, &separator, 2, ":+-Z", true, 0, 59))
//     {
//         return isSuccess;
//     }

//     if (separator == ':')
//     {
//         if (parseW3CDTFSubString_(&isSuccess, second, &partStr, &length, &separator, 2, ".+-Z", true, 0, 59))
//         {
//             return isSuccess;
//         }

//         if (separator == '.')
//         {
//             if (length == 0)
//             {
//                 return false;
//             }

//             sead::SafeString::token_iterator it = partStr.tokenBegin("+-Z");
//             ++it;
//             if (it == partStr.tokenEnd())
//             {
//                 return true;
//             }

//             separator = partStr.at(it.getIndex() - 1);
//             partStr = partStr.getPart(it.getIndex());
//             length -= it.getIndex();
//         }
//     }

//     if (separator == 'Z')
//     {
//         *tzHour = 0;
//         *tzMinute = 0;
//     }
//     else if (separator == '+' || separator == '-')
//     {
//         u32 tzHourTemp = 0;
//         bool isEnd = parseW3CDTFSubString_(&isSuccess, &tzHourTemp, &partStr, &length, nullptr, 2, ":", false, 0, 11);
//         if (isSuccess)
//         {
//             *tzHour = tzHourTemp;
//             if (separator == '-')
//             {
//                 *tzHour = -*tzHour;
//             }
//         }

//         if (isEnd)
//         {
//             return isSuccess;
//         }

//         u32 tzMinuteTemp = 0;
//         isEnd = parseW3CDTFSubString_(&isSuccess, &tzMinuteTemp, &partStr, &length, nullptr, 2, "", true, 0, 59);
//         if (isSuccess)
//         {
//             *tzMinute = tzMinuteTemp;
//             if (separator == '-')
//             {
//                 *tzMinute = -*tzMinute;
//             }
//         }

//         if (!isEnd || !isSuccess)
//         {
//             return false;
//         }

//         length -= 2;
//     }

//     return length == 0;
// }

namespace sead { namespace DateUtil {

bool isLeapYear(u32 year)
{
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

CalendarTime::Week calcWeekDay(const CalendarTime::Year& year, const CalendarTime::Month& mon, const CalendarTime::Day& day)
{
    u32 y = year.getValue();
    u32 m = mon.getValueOneOrigin();
    u32 d = day.getValue();
    if (m < 3)
    {
        y--;
        m += 12;
    }

    return CalendarTime::Week(((m * 26 + 16) / 10 + d + ((y + y / 4) - y / 100) + y / 400) % 7);
}

void DateUtil::calcSecondToCalendarSpan(CalendarSpan* dst, u64 sec)
{
    if (!dst)
    {
        return;
    }

    dst->setDays(static_cast<s32>(sec / cDaySeconds));
    dst->setHours((sec % cDaySeconds) / cHourSeconds);
    dst->setMinutes((sec % cHourSeconds) / cMinuteSeconds);
    dst->setSeconds(sec % cMinuteSeconds);
}

// bool DateUtil::parseW3CDTFString(CalendarTime* dst, CalendarSpan* timezone, const SafeString& str)
// {
//     u32 year = 1970;
//     u32 month = 1;
//     u32 day = 1;
//     u32 hour = 0;
//     u32 minute = 0;
//     u32 second = 0;

//     s32 tzHour = 0;
//     s32 tzMinute = 0;

//     bool success = parseW3CDTFStringImpl_(&year, &month, &day, &hour, &minute, &second, &tzHour, &tzMinute, str);
//     if (success)
//     {
//         dst->setDate(CalendarTime::Date(CalendarTime::Year(year), CalendarTime::Month::makeFromValueOneOrigin(month), CalendarTime::Day(day)));
//         dst->setTime(CalendarTime::Time(CalendarTime::Hour(hour), CalendarTime::Minute(minute), CalendarTime::Second(second)));

//         timezone->setDays(0);
//         timezone->setHours(tzHour);
//         timezone->setMinutes(tzMinute);
//         timezone->setSeconds(0);
//     }

//     return success;
// }

} } // namespace sead::DateUtil
