#include "usr_time.hpp"
#include <stdint.h>

int8_t usrTime::timezone_offset = 0;
usrTime::ltime_t usrTime::epoch_seconds = 0;
usrTime::ltime1ms_t usrTime::epoch_milliseconds = 0;

// 閏年判定関数
uint16_t usrTime::_is_leap_year(uint16_t year)
{
    return (year % 4 == 0) && (year % 100 != 0 || year % 400 == 0);
}

usrTime::usrTime()
{
    timezone_offset = 9;
    epoch_seconds = 0;
}

usrTime::~usrTime()
{
}

void usrTime::setTimezoneOffset(int8_t offset)
{
    if (offset < -12 || offset > 14)
    {
        return;
    }
    usrTime::timezone_offset = offset;
}

usrTime::LocalTime_t usrTime::getLocalTime(usrTime::ltime_t epoch_seconds)
{
    // 各月の日数（平年）
    const uint8_t days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    usrTime::LocalTime_t lt;
    uint16_t year = 1970;
    int32_t days;

    // タイムゾーン補正（秒単位）
    epoch_seconds += static_cast<int64_t>(usrTime::timezone_offset) * 3600;

    // 秒から日数に変換
    days = epoch_seconds / 86400;
    uint32_t remaining_seconds = epoch_seconds % 86400;

    // **曜日の計算（1970/1/1 は木曜日=3）**
    lt.weekday = ((epoch_seconds / 86400) + 4) % 7; // 0: 日, 1: 月, ..., 6: 土

    // 年を計算
    while (days >= (usrTime::_is_leap_year(year) ? 366 : 365))
    {
        days -= usrTime::_is_leap_year(year) ? 366 : 365;
        year++;
    }
    lt.year = year;

    // 月を計算
    uint8_t month = 0;
    while (days >= (month == 1 && usrTime::_is_leap_year(year) ? 29 : days_in_month[month]))
    {
        days -= (month == 1 && usrTime::_is_leap_year(year)) ? 29 : days_in_month[month];
        month++;
    }
    lt.month = month + 1; // 1月=1, 2月=2, ...

    // 日を計算
    lt.day = days + 1; // 1日からカウント

    // 時・分・秒を計算
    lt.hour = remaining_seconds / 3600;
    remaining_seconds %= 3600;
    lt.minute = remaining_seconds / 60;
    lt.second = remaining_seconds % 60;

    return lt;
}

usrTime::LocalTime_t usrTime::getLocalTime(void)
{
    return usrTime::getLocalTime(usrTime::epoch_seconds);
}

usrTime::ltime_t usrTime::getEpochTime(void)
{
    return usrTime::epoch_seconds;
}

usrTime::ltime1ms_t usrTime::getEpochMilliseconds(void)
{
    return usrTime::epoch_milliseconds;
}

void usrTime::setEpochTime(usrTime::ltime_t epoch_seconds)
{
    usrTime::epoch_seconds = epoch_seconds;
}

void usrTime::setEpochMilliseconds(usrTime::ltime1ms_t epoch_milliseconds)
{
    usrTime::epoch_milliseconds = epoch_milliseconds;
}

void usrTime::incrementEpochSecond(void)
{
    usrTime::epoch_seconds++;
}

void usrTime::incrementEpochMilliseconds(void)
{
    epoch_milliseconds++;
    if (epoch_milliseconds >= 1000)
    {
        epoch_milliseconds = 0;
        epoch_seconds++;
    }
}
