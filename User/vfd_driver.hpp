#ifndef __VFD_DRIVER_HPP
#define __VFD_DRIVER_HPP

#define VFD_DIGIT_NUM 10
#ifdef __cplusplus
extern "C"
{
	// C言語用定義
#include <string.h>
#include "usr_ftoa.h"
}
#endif //__cplusplus

#ifdef __cplusplus
#include <array>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cstdint>
#include <array>
#include <cstdio>
#include "usr_time.hpp"

// C++言語用定義
class VFD_Driver
{
public:
	typedef uint16_t SegmentData;
	typedef struct
	{
		uint8_t num;
		uint8_t bright;
	} DigitDisplayData_t;
	/// @brief セグメント表示データ
	enum Segment_e
	{
		SEG_0 = 0,
		SEG_1,
		SEG_2,
		SEG_3,
		SEG_4,
		SEG_5,
		SEG_6,
		SEG_7,
		SEG_8,
		SEG_9,
		SEG_A = 10,
		SEG_b,
		SEG_C,
		SEG_d,
		SEG_E,
		SEG_F,
		SEG_S = 16,
		SEG_n,
		SEG_M,
		SEG_T,
		SEG_u = 20,
		SEG_W,
		SEG_h,
		SEG_r,
		SEG_K,
		SEG_P = 25,
		SEG_o,
		SEG_i,
		SEG_H,
		SEG_MINUS,
		SEG_COLON = 30,
		SEG_SLASH,
		SEG_DP,
		SEG_COMMA,
		SEG_DASH,
		SEG_UNDER = 35,
		SEG_HALF,
		SEG_NONE = 37,
	};
	VFD_Driver();
	~VFD_Driver();
	void begin(void);
	uint32_t getDriveData(uint8_t *ptr);
	void setDigitDisplayData(uint8_t digit, SegmentData segment, uint8_t bright);
	void setDigitDisplayData(uint8_t digit, const DigitDisplayData_t *data);
	void setDigitDisplayData(uint8_t start_digit, char *str, uint8_t bright);
	SegmentData getDigitDisplayData(uint8_t digit);
	// void displayTime(bool changeFade, struct tm *time, uint8_t t100ms);
	void displayTime(bool changeFade, usrTime::LocalTime_t *lt, usrTime::ltime1ms_t lt1ms);
	// void displayDate(struct tm *time);
	void displayDate(usrTime::LocalTime_t *lt);
	void displayTemp(float temp);
	void displayPress(float press);
	void displayHumi(float humi);
	// void displayWeek(struct tm *time);
	void displayWeek(usrTime::LocalTime_t *lt);
	SegmentData getSegmentData(Segment_e disp);
	SegmentData getSegmentData(int num);
	SegmentData getSegmentData(char c);

private:
	typedef union
	{
		struct
		{
			uint32_t digit : 10;
			uint32_t segment : 12;
			uint32_t _ : 10;
		} VFD_DriveBits;
		uint8_t bytes[4];
		uint16_t words[2];
		uint32_t dword;
	} VFD_DriveBytes_t;
	/// @brief VFD点灯データ
	typedef union
	{
		struct
		{
			uint32_t segment : 16;
			uint32_t bright : 5;
		} bits;
		std::array<uint8_t, 4> bytes;
		std::array<uint16_t, 2> words;
		uint32_t dwords;
	} VFD_DigitData_t;

	typedef uint16_t DigitData;
	uint8_t _get_brigt_time_100ms(uint8_t t100ms);
	uint8_t _get_brigt_time_fade_out(uint8_t t100ms);
	uint8_t _get_brigt_time_fade_in(uint8_t t100ms);
	uint8_t _get_brigt_colon(uint8_t t100ms);
	static std::array<VFD_DigitData_t, VFD_DIGIT_NUM> VFD_DigitData;
	static const uint8_t VFD_DigitData_Size;
	// const DigitData DigitSelector[VFD_DIGIT_NUM];
	static const std::array<DigitData, VFD_DIGIT_NUM> DigitSelector;
	static const std::array<SegmentData, 38> SegmentSelector;
};

#endif //__cplusplus

#endif // __VFD_DRIVER_HPP
