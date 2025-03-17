/// @file
/// @brief VFDドライバ
/// @details

#include "vfd_driver.hpp"

const std::array<VFD_Driver::DigitData, VFD_DIGIT_NUM> VFD_Driver::DigitSelector = {
	0b0000000001, // 1桁目(左側)
	0b0000000010, // 2桁目
	0b0000000100, // 3桁目
	0b0000001000, //	4桁目
	0b0000010000, //	5桁目
	0b0000100000, //	6桁目
	0b0001000000, //	7桁目
	0b0010000000, //	8桁目
	0b0100000000, //	9桁目
	0b1000000000, //	10桁目(右側)
};

const std::array<VFD_Driver::SegmentData, 38> VFD_Driver::SegmentSelector = {
	// 0bxxxxxgfedcba
	0b000000111111, // 0
	0b000000000110, // 1
	0b000001011011, // 2
	0b000001001111, // 3
	0b000001100110, // 4
	0b000001101101, // 5
	0b000001111101, // 6
	0b000000100111, // 7
	0b000001111111, // 8
	0b000001101111, // 9
	0b000001110111, // A    10
	0b000001111100, // b    11
	0b000000111001, // C    12
	0b000001011110, // d    13
	0b000001111001, // E    14
	0b000001110001, // F    15
	0b000001101101, // S    16
	0b000001010100, // n    17
	0b000000110111, // П(M) 18
	0b000000110001, // Γ(T) 19
	0b000000011100, // u    20
	0b000001111110, // ∀(W) 21
	0b000001110100, // h    22
	0b000001010000, // r    23
	0b000001111010, // K	24
	0b000001110011, // P	25
	0b000001011100, // o	26
	0b000000000100, // i	27
	0b000001110110, // H	28
	0b000001000000, // -    29
	0b000001001000, // :    30
	0b000001010010, // /    31
	0b000010000000, // dp . 32
	0b000100000000, // conma , 33
	0b001000000000, // dash ' 34
	0b010000000000, // digit v 35
	0b100000000000, // harf 1/2 36
	0b000000000000, // none	37;
};
std::array<VFD_Driver::VFD_DigitData_t, VFD_DIGIT_NUM> VFD_Driver::VFD_DigitData;
const uint8_t VFD_Driver::VFD_DigitData_Size = sizeof(VFD_Driver::VFD_DigitData);

VFD_Driver::VFD_Driver()
{
	// コンストラクタ
}

VFD_Driver::~VFD_Driver()
{
	// デストラクタ
}

void VFD_Driver::begin(void)
{
	// VFDドライバの初期化
	for (uint8_t i = 0; i < VFD_DIGIT_NUM; i++)
	{
		VFD_Driver::VFD_DigitData[i].bits.segment = 0;
		VFD_Driver::VFD_DigitData[i].bits.bright = 0;
	}
}

/// @brief VFD点灯データ取得
/// この関数を10usごとに呼ぶと、各桁に設定した表示内容と輝度に合わせた制御信号を取得する
/// @return 3バイトの制御信号(桁・セグメント用24bit情報)
uint32_t VFD_Driver::getDriveData(uint8_t *ptr)
{
	VFD_DriveBytes_t *vdb = (VFD_DriveBytes_t *)ptr;
	static uint8_t digit_ctrl = 0;
	// static uint8_t bright_ctrl = 0;

	if (digit_ctrl >= VFD_DIGIT_NUM)
	{
		digit_ctrl = 0;
	}

	// //桁の輝度制御0～20 が明るさ brightが20のときは最も明るい
	// if (VFD_DigitData[digit_ctrl].bits.bright > bright_ctrl)
	// {
	// 	// VFDを点灯させる
	// 	vdb->VFD_DriveBits.digit = VFD_Driver::DigitSelector[digit_ctrl];
	// 	vdb->VFD_DriveBits.segment = VFD_Driver::VFD_DigitData[digit_ctrl].bits.segment;
	// }
	// else // brightがbright_ctrlより小さい場合
	// {
	// 	// VFDを消灯させる
	// 	vdb->VFD_DriveBits.digit = 0;
	// 	vdb->VFD_DriveBits.segment = 0;
	// }

	// bright_ctrl++;
	// if (bright_ctrl > 20)
	// {
	// 	bright_ctrl = 0;
	// 	digit_ctrl++;
	// }

	// VFDの輝度制御はPWMとコンペアマッチで行う PWMとコンペアマッチのタイマは共通にし、同期動作させる
	// brightの値はPWMのデューティーに設定、20=おおよそ90%となるように調整する
	// コンペアマッチはPWMのおよそ90%に固定設定しておく
	// 使用するタイマはおよそ400usecの割り込み周期にするとちらつきも少ない。

	// VFDを点灯させる
	vdb->VFD_DriveBits.digit = VFD_Driver::DigitSelector[digit_ctrl];
	vdb->VFD_DriveBits.segment = VFD_Driver::VFD_DigitData[digit_ctrl].bits.segment;

	uint32_t duty = 0;
	duty = VFD_DigitData[digit_ctrl].bits.bright * 19; // 0-20を0-380に変換

	digit_ctrl++;

	return duty;
}

/// @brief 各桁の表示データと輝度をセットする
/// @param digit 桁番号(0～9)
/// @param segment セグメント表示データ
/// @param bright 輝度(0～20)
void VFD_Driver::setDigitDisplayData(uint8_t digit, VFD_Driver::SegmentData segment, uint8_t bright)
{
	static VFD_DigitData_t vdd;					 // VFD点灯データ
	vdd.bits.segment = segment & 0b111111111111; //表示データの上限設定
	vdd.bits.bright = bright > 20 ? 20 : bright; //輝度の上限設定

	if (digit < sizeof(VFD_DigitData) / sizeof(VFD_DigitData[0]))
	{
		VFD_DigitData[digit] = vdd; // VFD点灯データの設定
	}
}

void VFD_Driver::setDigitDisplayData(uint8_t digit, const VFD_Driver::DigitDisplayData_t *data)
{
	this->setDigitDisplayData(digit, this->getSegmentData(data->num), data->bright);
}

void VFD_Driver::setDigitDisplayData(uint8_t start_digit, char *str, uint8_t bright)
{
	uint8_t i = 0;
	while (str[i] != '\0')
	{
		if ((start_digit + i) >= VFD_DIGIT_NUM)
		{
			return;
		}
		this->setDigitDisplayData(start_digit + i, this->getSegmentData(str[i]), bright);
		i++;
	}
}

void VFD_Driver::displayTime(bool changeFade, usrTime::LocalTime_t *lt, usrTime::ltime1ms_t lt1ms)
{
	typedef struct
	{
		VFD_Driver::DigitDisplayData_t t_100ms;
		VFD_Driver::DigitDisplayData_t sec_1;
		VFD_Driver::DigitDisplayData_t sec_10;
		VFD_Driver::DigitDisplayData_t min_1;
		VFD_Driver::DigitDisplayData_t min_10;
		VFD_Driver::DigitDisplayData_t hour_1;
		VFD_Driver::DigitDisplayData_t hour_10;
	} digitTime_t;
	digitTime_t digitTime;

	uint8_t t100ms = lt1ms / 100;
	// 100ms桁の表示
	digitTime.t_100ms.num = t100ms;
	digitTime.t_100ms.bright = 20;
	// 1秒桁の表示
	digitTime.sec_1.num = lt->second % 10;
	digitTime.sec_1.bright = (changeFade) ? _get_brigt_time_100ms(t100ms) : 20;
	// 10秒桁の表示
	digitTime.sec_10.num = lt->second / 10;
	if (digitTime.sec_1.num == 9)
	{
		digitTime.sec_10.bright = (changeFade) ? _get_brigt_time_fade_out(t100ms) : 20;
	}
	else if (digitTime.sec_1.num == 0)
	{
		digitTime.sec_10.bright = (changeFade) ? _get_brigt_time_fade_in(t100ms) : 20;
	}
	else
	{
		digitTime.sec_10.bright = 20;
	}
	// 1分桁の表示
	digitTime.min_1.num = lt->minute % 10;
	if (digitTime.sec_10.num == 5 && digitTime.sec_1.num == 9)
	{
		digitTime.min_1.bright = (changeFade) ? _get_brigt_time_fade_out(t100ms) : 20;
	}
	else if (digitTime.sec_10.num == 0 && digitTime.sec_1.num == 0)
	{
		digitTime.min_1.bright = (changeFade) ? _get_brigt_time_fade_in(t100ms) : 20;
	}
	else
	{
		digitTime.min_1.bright = 20;
	}
	// 10分桁の表示
	digitTime.min_10.num = lt->minute / 10;
	if (digitTime.min_1.num == 9 && digitTime.sec_10.num == 5 && digitTime.sec_1.num == 9)
	{
		digitTime.min_10.bright = (changeFade) ? _get_brigt_time_fade_out(t100ms) : 20;
	}
	else if (digitTime.min_1.num == 0 && digitTime.sec_10.num == 0 && digitTime.sec_1.num == 0)
	{
		digitTime.min_10.bright = (changeFade) ? _get_brigt_time_fade_in(t100ms) : 20;
	}
	else
	{
		digitTime.min_10.bright = 20;
	}
	// 1時間桁の表示
	digitTime.hour_1.num = lt->hour % 10;
	if (digitTime.min_10.num == 5 && digitTime.min_1.num == 9 && digitTime.sec_10.num == 5 && digitTime.sec_1.num == 9)
	{
		digitTime.hour_1.bright = (changeFade) ? _get_brigt_time_fade_out(t100ms) : 20;
	}
	else if (digitTime.min_10.num == 0 && digitTime.min_1.num == 0 && digitTime.sec_10.num == 0 && digitTime.sec_1.num == 0)
	{
		digitTime.hour_1.bright = (changeFade) ? _get_brigt_time_fade_in(t100ms) : 20;
	}
	else
	{
		digitTime.hour_1.bright = 20;
	}
	// 10時間桁の表示
	digitTime.hour_10.num = lt->hour / 10;
	if (digitTime.hour_1.num == 9 && digitTime.min_10.num == 5 && digitTime.min_1.num == 9 && digitTime.sec_10.num == 5 && digitTime.sec_1.num == 9)
	{
		digitTime.hour_10.bright = (changeFade) ? _get_brigt_time_fade_out(t100ms) : 20;
	}
	else if (digitTime.hour_1.num == 0 && digitTime.min_10.num == 0 && digitTime.min_1.num == 0 && digitTime.sec_10.num == 0 && digitTime.sec_1.num == 0)
	{
		digitTime.hour_10.bright = (changeFade) ? _get_brigt_time_fade_in(t100ms) : 20;
	}
	else
	{
		digitTime.hour_10.bright = 20;
	}

	this->setDigitDisplayData(9, &digitTime.t_100ms);
	this->setDigitDisplayData(8, this->getSegmentData(':'), _get_brigt_colon(t100ms));
	this->setDigitDisplayData(7, &digitTime.sec_1);
	this->setDigitDisplayData(6, &digitTime.sec_10);
	this->setDigitDisplayData(5, this->getSegmentData(':'), _get_brigt_colon(t100ms));
	this->setDigitDisplayData(4, &digitTime.min_1);
	this->setDigitDisplayData(3, &digitTime.min_10);
	this->setDigitDisplayData(2, this->getSegmentData(':'), _get_brigt_colon(t100ms));
	this->setDigitDisplayData(1, &digitTime.hour_1);
	this->setDigitDisplayData(0, &digitTime.hour_10);
}

uint8_t VFD_Driver::_get_brigt_time_100ms(uint8_t t100ms)
{
	switch (t100ms)
	{
	case 0:
		return 0;
		break;
	case 1:
	case 9:
		return 5;
		break;
	case 2:
	case 8:
		return 10;
		break;
	case 3:
	case 7:
		return 15;
		break;
	default:
		return 20;
		break;
	}
}
uint8_t VFD_Driver::_get_brigt_time_fade_out(uint8_t t100ms)
{
	switch (t100ms)
	{
	case 9:
		return 5;
		break;
	case 8:
		return 10;
		break;
	case 7:
		return 15;
		break;
	default:
		return 20;
		break;
	}
}
uint8_t VFD_Driver::_get_brigt_time_fade_in(uint8_t t100ms)
{
	switch (t100ms)
	{
	case 0:
		return 0;
		break;
	case 1:
		return 5;
		break;
	case 2:
		return 10;
		break;
	case 3:
		return 15;
		break;
	default:
		return 20;
		break;
	}
}
uint8_t VFD_Driver::_get_brigt_colon(uint8_t t100ms)
{
	switch (t100ms)
	{
	case 0:
	case 7:
		return 5;
		break;
	case 1:
	case 6:
		return 10;
		break;
	case 2:
	case 5:
		return 15;
		break;
	case 3:
	case 4:
		return 20;
		break;
	case 8:
	case 9:
		return 0;
		break;
	default:
		return 20;
		break;
	}
}

void VFD_Driver::displayDate(usrTime::LocalTime_t *lt)
{
	char date_str[20];
	// strftime(date_str, sizeof(date_str), "%Y-%m-%d", time);
	sprintf(date_str, "%04d-%02d-%02d", lt->year, lt->month, lt->day);
	this->setDigitDisplayData(0, date_str, 20);
}

void VFD_Driver::displayTemp(float temp)
{
	char str[10];
	// sprintf(str, "%4.1f", temp);
	usr_floa_sig(temp, str, 4, 3);
	this->setDigitDisplayData(0, (char *)"TM: ", 20);
	this->setDigitDisplayData(4, this->getSegmentData(str[0]), 20);
	this->setDigitDisplayData(5, this->getSegmentData(str[1]) | this->getSegmentData('.'), 20);
	this->setDigitDisplayData(6, this->getSegmentData(str[3]), 20);
	this->setDigitDisplayData(7, this->getSegmentData('C') | this->getSegmentData('\''), 20);
	this->setDigitDisplayData(8, (char *)"  ", 20); // 8～9桁目
}
void VFD_Driver::displayPress(float press)
{
	char str[10];
	// sprintf(str, "%4.0f", press);
	usr_floa_sig(press, str, 4, 4);
	this->setDigitDisplayData(0, this->getSegmentData('P'), 20);
	this->setDigitDisplayData(1, this->getSegmentData('r'), 20);
	this->setDigitDisplayData(2, this->getSegmentData(':'), 20);
	this->setDigitDisplayData(3, str, 20); // 3～6桁目
	this->setDigitDisplayData(7, this->getSegmentData('h'), 20);
	this->setDigitDisplayData(8, this->getSegmentData('P'), 20);
	this->setDigitDisplayData(9, this->getSegmentData('A'), 20);

	if (str[0] != ' ')
	{
		this->setDigitDisplayData(3, this->getDigitDisplayData(3) | this->getSegmentData(',') | this->getSegmentData('.'), 20);
	}
}
void VFD_Driver::displayHumi(float humi)
{
	char str[10];
	// sprintf(str, "%4.1f", humi);
	usr_floa_sig(humi, str, 4, 3);
	this->setDigitDisplayData(0, (char *)"Hu: ", 20);
	this->setDigitDisplayData(4, this->getSegmentData(str[0]), 20);
	this->setDigitDisplayData(5, this->getSegmentData(str[1]) | this->getSegmentData('.'), 20);
	this->setDigitDisplayData(6, this->getSegmentData(str[3]), 20);
	this->setDigitDisplayData(7, this->getSegmentData('/') | this->getSegmentData('\'') | this->getSegmentData('.'), 20);
	this->setDigitDisplayData(8, (char *)"  ", 20); // 8～9桁目
}
void VFD_Driver::displayWeek(usrTime::LocalTime_t *lt)
{
	const char week_str[][4] =
		{
			"Sun",
			"Mon",
			"TuE",
			"WEd",
			"Thu",
			"Fri",
			"SAT",
		};
	this->setDigitDisplayData(0, (char *)" WEEK: ", 20);
	this->setDigitDisplayData(7, (char *)week_str[lt->weekday], 20);
}

/// @brief
/// @param disp
/// @return
VFD_Driver::SegmentData VFD_Driver::getSegmentData(Segment_e disp)
{
	volatile const Segment_e sd_disp = (Segment_e)disp;
	volatile const Segment_e sd_slash = SEG_SLASH;
	volatile const Segment_e sd_half = SEG_HALF;
	if ((sd_slash <= sd_disp) && (sd_disp <= sd_half))
	{
		return SegmentSelector[sd_disp];
	}
	return SegmentSelector[VFD_Driver::SEG_NONE];
}

VFD_Driver::SegmentData VFD_Driver::getSegmentData(int num)
{
	const uint8_t sd_slash = (uint8_t)SEG_SLASH;
	if ((uint8_t)num <= sd_slash)
	{
		return SegmentSelector[num];
	}
	return SegmentSelector[VFD_Driver::SEG_NONE];
}

VFD_Driver::SegmentData VFD_Driver::getSegmentData(char c)
{
	volatile SegmentData ret = 0;
	if (isdigit(c))
	{
		ret = SegmentSelector[c - 0x30];
	}
	else
	{
		switch (c)
		{
		case 'A':
			ret = SegmentSelector[VFD_Driver::SEG_A];
			break;
		case 'b':
			ret = SegmentSelector[VFD_Driver::SEG_b];
			break;
		case 'C':
			ret = SegmentSelector[VFD_Driver::SEG_C];
			break;
		case 'd':
			ret = SegmentSelector[VFD_Driver::SEG_d];
			break;
		case 'E':
			ret = SegmentSelector[VFD_Driver::SEG_E];
			break;
		case 'F':
			ret = SegmentSelector[VFD_Driver::SEG_F];
			break;
		case 'S':
			ret = SegmentSelector[VFD_Driver::SEG_S];
			break;
		case 'n':
			ret = SegmentSelector[VFD_Driver::SEG_n];
			break;
		case 'M':
			ret = SegmentSelector[VFD_Driver::SEG_M];
			break;
		case 'T':
			ret = SegmentSelector[VFD_Driver::SEG_T];
			break;
		case 'u':
			ret = SegmentSelector[VFD_Driver::SEG_u];
			break;
		case 'W':
			ret = SegmentSelector[VFD_Driver::SEG_W];
			break;
		case 'h':
			ret = SegmentSelector[VFD_Driver::SEG_h];
			break;
		case 'r':
			ret = SegmentSelector[VFD_Driver::SEG_r];
			break;
		case 'K':
			ret = SegmentSelector[VFD_Driver::SEG_K];
			break;
		case 'P':
			ret = SegmentSelector[VFD_Driver::SEG_P];
			break;
		case 'o':
			ret = SegmentSelector[VFD_Driver::SEG_o];
			break;
		case 'i':
			ret = SegmentSelector[VFD_Driver::SEG_i];
			break;
		case 'H':
			ret = SegmentSelector[VFD_Driver::SEG_H];
			break;
		case '-':
			ret = SegmentSelector[VFD_Driver::SEG_MINUS];
			break;
		case ':':
		case '=':
			ret = SegmentSelector[VFD_Driver::SEG_COLON];
			break;
		case '/':
			ret = SegmentSelector[VFD_Driver::SEG_SLASH];
			break;
		case '.':
			ret = SegmentSelector[VFD_Driver::SEG_DP];
			break;
		case ',':
			ret = SegmentSelector[VFD_Driver::SEG_COMMA];
			break;
		case '\'':
			ret = SegmentSelector[VFD_Driver::SEG_DASH];
			break;
		case 'v':
			ret = SegmentSelector[VFD_Driver::SEG_UNDER];
			break;
		default:
			ret = SegmentSelector[VFD_Driver::SEG_NONE];
			break;
		}
	}
	return ret;
}

VFD_Driver::SegmentData VFD_Driver::getDigitDisplayData(uint8_t digit)
{
	if (digit < sizeof(VFD_DigitData) / sizeof(VFD_DigitData[0]))
	{
		return VFD_DigitData[digit].bits.segment;
	}
	return 0;
}
