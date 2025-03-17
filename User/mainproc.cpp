#include "mainproc.hpp"

static VFD_Driver vfd; //クラスをローカルでオブジェクト化する(動的確保の必要がないため)
static usrTime ut;     //クラスをローカルでオブジェクト化する(動的確保の必要がないため)

volatile uint32_t testadc;

volatile static float temp = 0;
volatile static float press = 0;
volatile static float humi = 0;

void procInit(void)
{
    //初期化処理
    vfd.begin();
    ut.setEpochTime(0);
    ut.setEpochMilliseconds(0);
}

void procMain(void)
{
    usrTime::LocalTime_t LocalTime;
    usrTime::ltime1ms_t LocalTime1ms = 0;

    LocalTime = ut.getLocalTime();
    LocalTime1ms = ut.getEpochMilliseconds();
    // vfd.displayTime(true, &LocalTime, LocalTime1ms);
    //  VFD表示データを生成

    //vfd.displayTemp(temp);
    //return;

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
        vfd.displayTemp(temp);
    }
    else if (42 <= sec && sec <= 47)
    {
        //気圧表示
        vfd.displayPress(press);
    }
    else if (52 <= sec && sec <= 57)
    {
        //湿度表示
        vfd.displayHumi(humi);
    }
    else
    {
        //時刻表示
        vfd.displayTime(true, &LocalTime, LocalTime1ms);
    }
}

void procEventCompare(void)
{
    static uint8_t drv[4];
    static uint32_t duty = 0;
    duty = vfd.getDriveData(drv);
    // 割り込み内処理のため最速化のためレジスタ直叩き
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
    // GPIOA->BSRR = GPIO_PIN_3 << 16; // GPIO_PIN_3をLOWにする
    HAL_SPI_Transmit(&hspi1, (uint8_t *)drv, 3, 1);
    // SPI1->CR1 |= SPI_CR1_SPE; // SPIを有効化
    // while (!(SPI1->SR & SPI_SR_TXE))
    //     ;              // TXバッファが空になるのを待つ
    // SPI1->DR = drv[0]; // 送信データをセット
    // while (!(SPI1->SR & SPI_SR_TXE))
    //     ;              // TXバッファが空になるのを待つ
    // SPI1->DR = drv[1]; // 送信データをセット
    // while (!(SPI1->SR & SPI_SR_TXE))
    //     ;                         // TXバッファが空になるのを待つ
    // SPI1->DR = drv[2];            // 送信データをセット
    // while (SPI1->SR & SPI_SR_BSY) // 最後のデータが送信完了するのを待つ
    //     ;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
    // GPIOA->BSRR = GPIO_PIN_3; // GPIO_PIN_3をHIGHにする

    duty = (duty * testadc) / 4096;

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, duty);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 380);
}

void procEvent1msec(void)
{
    ut.incrementEpochMilliseconds();
}

void procEventADC(void)
{
    static uint32_t ad = 0;
    ad += HAL_ADC_GetValue(&hadc1);
    ad /= 2;
    testadc = ad;
}

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
        uint8_t *a2b_h = &buf[1];
        uint8_t *a2b_l = &buf[2];
        for (uint8_t i = 0; i < sizeof(rcv.data); i++)
        {
            uint8_t str[3];
            str[0] = *a2b_h;
            str[1] = *a2b_l;
            str[2] = 0;
            rcv.bytes[i] = strtoul((const char *)str, NULL, 16);
            a2b_h += 2;
            a2b_l += 2;
        }

        ut.setEpochTime((usrTime::ltime_t)rcv.data.epoch);
        ut.setEpochMilliseconds(0); //エポック時間を受信したときmsは0
        temp = rcv.data.temp;
        press = rcv.data.press;
        humi = rcv.data.humi;
    }
}
