#include "mainproc.hpp"

static VFD_Driver vfd; //クラスをローカルでオブジェクト化する(動的確保の必要がないため)
static usrTime ut;     //クラスをローカルでオブジェクト化する(動的確保の必要がないため)
volatile uint32_t AD_Val = 0;

typedef struct
{
    float temp;
    float press;
    float humi;
} EnvValue_t;
volatile static EnvValue_t EnvVal = {0.0, 0.0, 0.0};

/**
 * @brief  初期化処理
 */
void procInit(void)
{
    //初期化処理
    vfd.begin();
    ut.setEpochTime(0);
    ut.setEpochMilliseconds(0);
}

/**
 * @brief  メイン処理
 */
void procMain(void)
{
    usrTime::LocalTime_t LocalTime;
    usrTime::ltime1ms_t LocalTime1ms = 0;

    LocalTime = ut.getLocalTime();
    LocalTime1ms = ut.getEpochMilliseconds();
    // vfd.displayTime(true, &LocalTime, LocalTime1ms);
    //  VFD表示データを生成

    // EnvVal.press=AD_Val/1.7;
    // vfd.displayPress(EnvVal.press);
    // return;

    uint8_t sec = LocalTime.second;
    if (12 <= sec && sec <= 17)
    {
        //曜日表示
        vfd.displayWeek(&LocalTime);
    }
    else if (22 <= sec && sec <= 27)
    {
        //日付表示
        vfd.displayDate(&LocalTime);
    }
    else if (32 <= sec && sec <= 37)
    {
        //気温表示
        vfd.displayTemp(EnvVal.temp);
    }
    else if (42 <= sec && sec <= 47)
    {
        //気圧表示
        vfd.displayPress(EnvVal.press);
    }
    else if (52 <= sec && sec <= 57)
    {
        //湿度表示
        vfd.displayHumi(EnvVal.humi);
    }
    else
    {
        //時刻表示
        vfd.displayTime(true, &LocalTime, LocalTime1ms);
    }
}

/**
 * @brief  VFD表示データ更新処理
 */
void procEventCompare(void)
{
    uint8_t drv[4];
    uint32_t duty = 0;
    duty = vfd.getDriveData(drv);                         //桁データの取得と、その桁の表示デューティーを取得
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET); // SPIのCS
    HAL_SPI_Transmit(&hspi1, (uint8_t *)drv, 3, 1);       // SPIパケット送信
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);   // SPIのCS

    int32_t tmp;
    tmp = 4095 - (int32_t)AD_Val;
    tmp = (tmp < 380) ? 380 : tmp;
    tmp = (tmp > 4095) ? 4095 : tmp;

    duty = (duty * tmp) / 4096;// 輝度調整のためPWM duty計算

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, duty); // VFD点灯イネーブル(PWMで輝度調整)
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 380);  // VFD点灯イネーブルと同期したコンペアマッチ割り込みのカウント値設定
    //イネーブルdutyが380を超えたらVFD強制消灯させるため
    //強制消灯期間を設けないとVFDの隣の桁に前の桁がうっすら表示されてゴーストの原因になる
}

/**
 * @brief  1msイベント処理
 */
void procEvent1msec(void)
{
    ut.incrementEpochMilliseconds();
}

/**
 * @brief  AD変換完了イベント処理
 */
void procEventADC(void)
{
    static uint32_t ad = 0;
    ad += HAL_ADC_GetValue(&hadc1);
    ad /= 2;
    AD_Val = ad;
}

/**
 * @brief  UART受信イベント処理
 */
void procEventRcvUart(uint8_t *buf)
{
    if (
        (buf[0] == '$') &&
        (buf[33] == ';') &&
        (buf[34] == '\r') &&
        (buf[35] == '\n'))
    {
        //受信データは正しい
        typedef union
        {
            struct
            {
                // usrTime::Epoch_t epoch;
                uint32_t epoch;
                float temp;
                float press;
                float humi;
            } data;
            uint8_t bytes[30];
        } RcvData_t;
        RcvData_t rcv;
        uint8_t *a2b_h = &buf[1];                      // ascii2桁の上位を格納
        uint8_t *a2b_l = &buf[2];                      // ascii2桁の下位を格納
        for (uint8_t i = 0; i < sizeof(rcv.data); i++) // 1パケットのバイト数分ループ
        {
            uint8_t str[3];
            str[0] = *a2b_h;
            str[1] = *a2b_l;
            str[2] = 0;
            rcv.bytes[i] = strtoul((const char *)str, NULL, 16); // 16進数文字列を数値に変換
            a2b_h += 2;                                          //受信文字ポインタを2つ進める(2文字で1セットのため)
            a2b_l += 2;
        }

        ut.setEpochTime((usrTime::ltime_t)rcv.data.epoch); //エポック時間を格納
        ut.setEpochMilliseconds(0);                        //エポック時間を受信したときmsは0
        //各センサー値を格納
        EnvVal.temp = rcv.data.temp;
        EnvVal.press = rcv.data.press;
        EnvVal.humi = rcv.data.humi;
    }
}
