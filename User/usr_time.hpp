#ifndef __USR_TIME_HPP
#define __USR_TIME_HPP

#ifdef __cplusplus
extern "C"
{
#endif
// C定義
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
// C++定義
#include <cstdint>
class usrTime
{
public:
    typedef struct
    {
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint8_t weekday;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
    } LocalTime_t;

    typedef uint32_t ltime_t;
    typedef uint16_t ltime1ms_t;
    typedef union
    {
        ltime_t epoch;
        uint8_t bytes[4];
    } Epoch_t;
    usrTime();
    ~usrTime();
    void setTimezoneOffset(int8_t offset);
    LocalTime_t getLocalTime(ltime_t epoch_seconds);
    LocalTime_t getLocalTime(void);
    ltime_t getEpochTime(void);
    ltime1ms_t getEpochMilliseconds(void);
    void setEpochTime(ltime_t epoch_seconds);
    void setEpochMilliseconds(ltime1ms_t epoch_milliseconds);
    void incrementEpochSecond(void);
    void incrementEpochMilliseconds(void);

private:
    uint16_t _is_leap_year(uint16_t year);
    static int8_t timezone_offset;
    static ltime_t epoch_seconds;
    static ltime1ms_t epoch_milliseconds;
};
#endif

#endif // __MAINPROC_HPP
