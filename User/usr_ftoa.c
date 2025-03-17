#include "usr_ftoa.h"

static void convUltoa(uint32_t value, char *buffer);
static uint8_t countDigits(uint32_t value);
static float mathPow10(uint8_t exp);

void convUltoa(uint32_t value, char *buffer)
{
    char temp[15] = {0};
    uint8_t i = 0;

    do
    {
        temp[i++] = '0' + (value % 10); //数値を文字に変換
        value /= 10;                    //数値の桁を下げる
    } while (value > 0);

    for (uint8_t j = 0; j < i; j++)
    {
        buffer[j] = temp[i - j - 1]; //文字列を逆順にする
    }
    buffer[i] = '\0'; //終端文字を入れる
}

uint8_t countDigits(uint32_t value)
{
    if (value == 0)
    {
        return 1;
    }
    value = (value > 100000000) ? 100000000 : value;

    uint8_t count = 0;
    while (value > 0)
    {
        value /= 10;
        count++;
    }
    return count;
}

float mathPow10(uint8_t exp)
{
    float result = 1.0;
    if (exp > FLOAT_MAX_DIGITS)
    {
        return 0;
    }

    for (uint8_t i = 0; i < exp; i++)
    {
        result *= 10.0;
    }
    return result;
}

/**
 * 文字列を指定文字で指定文字数になるように埋める
 * @param ptr 変換する文字列
 * @param len 指定文字数
 * @return なし
 */
void fillChar(char *ptr, char c, uint8_t len)
{
    int8_t fillLen;
    len = (len > MAX_DIGITS) ? MAX_DIGITS : len;
    //指定桁数になるように空白を入れる
    fillLen = len - strlen(ptr);
    if (fillLen > 0)
    {
        memmove(ptr + fillLen, ptr, strlen(ptr) + 1);
        memset(ptr, c, fillLen);
    }
}

/**
 * 浮動小数点数を文字列に変換する
 * @param value 変換する値
 * @param ptr 変換結果を格納するポインタ
 * @param outputDigits 出力桁数
 * @param significantDigits 有効桁数
 * @return なし
 */
void usr_floa_sig(float value, char *ptr, uint8_t outputDigits, uint8_t significantDigits)
{
    char tmp[15] = {0};
    float fraction;
    uint8_t intLen;
    int8_t fracLen, fracFillLen;
    uint32_t intPart, fracPart;

    *ptr = '\0'; //出力文字バッファクリア
    if (value > 1e9)
    {
        strcpy(ptr, (const char *)"Inf");
        fillChar(ptr, ' ', outputDigits);
        return;
    }
    if (value < -1e9)
    {
        strcpy(ptr, (const char *)"-Inf");
        fillChar(ptr, ' ', outputDigits);
        return;
    }
    significantDigits = (significantDigits > FLOAT_MAX_DIGITS) ? FLOAT_MAX_DIGITS : significantDigits;
    outputDigits = (outputDigits > MAX_DIGITS) ? MAX_DIGITS : outputDigits;

    if (value < 0)
    {
        strcat(ptr, (const char *)"-"); //負数なら符号出力
        value = -value;
    }

    intPart = (uint32_t)value;     //整数部取り出し
    intLen = countDigits(intPart); //整数部桁取得
    convUltoa(intPart, tmp);
    strcat(ptr, tmp);
    intLen = (intPart == 0) ? 0 : intLen; //整数に数値がない時は整数桁はゼロとする(整数が1~9の時は整数桁は1)

    fracLen = significantDigits - intLen; //小数桁を算出
    if (fracLen > 0)                      //小数桁出力必要か？
    {
        fraction = (value - intPart) * mathPow10(fracLen); //小数部取り出し
        fraction += 0.5;                                   // 1桁下に0.5を加算して四捨五入する(floatは数値ピッタリにならないため)
        fracPart = (uint32_t)fraction;
        fracFillLen = fracLen - countDigits(fracPart);         //小数部不足桁数を算出
        strcat(ptr, (const char *)".");                        //小数点出力
        strncat(ptr, (const char *)"0000000000", fracFillLen); //不足桁を0埋め出力
        convUltoa(fracPart, tmp);                              //小数部を文字列変換
        strcat(ptr, tmp);                                      //小数部出力
    }
    fillChar(ptr, ' ', outputDigits);

    return;
}

void usr_floa_dec(float value, char *ptr, uint8_t intDigits, uint8_t decDigits)
{
    char tmp[15] = {0};
    float fraction;
    uint8_t intLen;
    int8_t fracLen, fracFillLen;
    uint32_t intPart, fracPart;

    *ptr = '\0'; //出力文字バッファクリア
    if (value > 1e9)
    {
        strcpy(ptr, (const char *)"Inf");
        fillChar(ptr, ' ', outputDigits);
        return;
    }
    if (value < -1e9)
    {
        strcpy(ptr, (const char *)"-Inf");
        fillChar(ptr, ' ', outputDigits);
        return;
    }
}
