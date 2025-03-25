#include "usr_ftoa.h"

static void convUltoa(uint32_t value, char *buffer);
static uint8_t countDigits(uint32_t value);
static float mathPow10(int8_t exp);

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

float mathPow10(int8_t exp)
{
    float result = 1.0;
    float base = 1.0;
    if (exp < 0)
    {
        exp = -exp;
        base = 0.1;
    }
    else
    {
        base = 10.0;
    }

    exp = (exp > FLOAT_MAX_DIGITS) ? FLOAT_MAX_DIGITS : exp;

    for (uint8_t i = 0; i < exp; i++)
    {
        result *= base;
    }

    return result;
}

/**
 * 文字列を指定文字で指定文字数になるように埋める
 * @param ptr 変換する文字列
 * @param len 指定文字数
 * @return なし
 */
void fillChar(char *ptr, char c, int8_t len)
{
    int8_t fillLen;
    if (len <= 0)
        return;
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
void usr_ftoa_sig(float value, char *ptr, uint8_t outputDigits, uint8_t significantDigits, uint8_t addThousandSeparat)
{
    char intStr[10] = {0};
    char fracStr[10] = {0};
    float fraction, ftmp;
    uint8_t intLen;
    int8_t fracLen, fracFillLen;
    uint32_t intPart, fracPart;

    //*ptr = '\0'; //出力文字バッファクリア
    if (value > 1e9)
    {
        // strcpy(ptr, (const char *)"Inf");
        strcat(intStr, (const char *)"Inf");
        fillChar(intStr, ' ', outputDigits);
        strcat(ptr, intStr);
        return;
    }
    if (value < -1e9)
    {
        // strcpy(ptr, (const char *)"-Inf");
        strcat(intStr, (const char *)"-Inf");
        fillChar(intStr, ' ', outputDigits);
        strcat(ptr, intStr);
        return;
    }
    significantDigits = (significantDigits > FLOAT_MAX_DIGITS) ? FLOAT_MAX_DIGITS : significantDigits;
    outputDigits = (outputDigits > MAX_DIGITS) ? MAX_DIGITS : outputDigits;

    if (value < 0)
    {
        strcat(ptr, (const char *)"-"); //負数なら符号出力
        value = -value;
    }

    intPart = (uint32_t)value;             //整数部取り出し
    intLen = countDigits(intPart);         //整数部桁取得
    fracLen = significantDigits - intLen;  //小数桁を算出
    fracLen = (fracLen < 0) ? 0 : fracLen; //小数桁が負はありえないのでクリップさせる

    ftmp = mathPow10(fracLen); //小数桁分桁移動
    value *= ftmp;
    value += 0.5; //四捨五入
    value /= ftmp;

    //四捨五入した後、もう一回有効数値判定する
    intPart = (uint32_t)value;             //整数部取り出し
    intLen = countDigits(intPart);         //整数部桁取得
    fracLen = significantDigits - intLen;  //小数桁を算出
    fracLen = (fracLen < 0) ? 0 : fracLen; //小数桁が負はありえないのでクリップさせる

    intPart = (uint32_t)value; //四捨五入後の整数部取り出し
    convUltoa(intPart, intStr);
    strcat(ptr, intStr);

    if (fracLen > 0) //小数桁出力必要か？
    {
        strcat(ptr, (const char *)".");      //小数点出力
        fraction = (value - intPart) * ftmp; //小数部取り出し                          // 1桁下に0.5を加算して四捨五入する(floatは数値ピッタリにならないため)
        fracPart = (uint32_t)fraction;
        fracFillLen = fracLen - countDigits(fracPart);             //小数部不足桁数を算出
        strncat(fracStr, (const char *)"0000000000", fracFillLen); //不足桁を0埋め出力
        convUltoa(fracPart, fracStr);                              //小数部を文字列変換
        strcat(ptr, fracStr);                                      //小数部出力
    }
    if (addThousandSeparat != 0)
    {
        addThousandSeparator(ptr, ',', 3);
    }
    fillChar(ptr, ' ', outputDigits);

    return;
}

/**
 * @brief  文字列に浮動小数点数を出力する
 * @param  value  出力する値
 * @param  ptr    出力先の文字列
 * @param  outputDigits 出力桁数
 * @param  intDigits 整数部桁数
 * @param  decDigits 小数部桁数
 * @retval なし
 */
void usr_ftoa_dec(float value, char *ptr, uint8_t outputDigits, uint8_t intDigits, uint8_t decDigits, uint8_t addThousandSeparat)
{
    char intStr[10] = {0};
    char fracStr[10] = {0};
    float fraction, ftmp;
    uint32_t intPart;

    //*ptr = '\0'; //出力文字バッファクリア
    if (value > 1e9)
    {
        // strcpy(ptr, (const char *)"Inf");
        strcat(intStr, (const char *)"Inf");
        fillChar(intStr, ' ', outputDigits);
        strcat(ptr, intStr);
        return;
    }
    if (value < -1e9)
    {
        // strcpy(ptr, (const char *)"-Inf");
        strcat(intStr, (const char *)"-Inf");
        fillChar(intStr, ' ', outputDigits);
        strcat(ptr, intStr);
        return;
    }

    decDigits = ((intDigits + decDigits) > FLOAT_MAX_DIGITS) ? (FLOAT_MAX_DIGITS - intDigits) : decDigits;
    outputDigits = (outputDigits > MAX_DIGITS) ? MAX_DIGITS : outputDigits;

    if (value < 0)
    {
        strcat(ptr, (const char *)"-"); //負数なら符号出力
        value = -value;
    }
    ftmp = mathPow10(decDigits);
    value *= ftmp;
    value += 0.5; //四捨五入
    value /= ftmp;

    intPart = (uint32_t)value; //整数部取り出し
    convUltoa(intPart, intStr);
    fillChar(intStr, ' ', intDigits);
    strcat(ptr, intStr);

    if (decDigits > 0)
    {
        //小数部の出力あり
        strcat(ptr, (const char *)".");         //小数点出力
        fraction = (value - intPart) * ftmp;    //小数部取り出し
        convUltoa((uint32_t)fraction, fracStr); //小数部を文字列変換
        fillChar(fracStr, '0', decDigits);
        strcat(ptr, fracStr); //小数部出力
    }
    if (addThousandSeparat != 0)
    {
        addThousandSeparator(ptr, ',', 3);
    }
    fillChar(ptr, ' ', outputDigits);

    return;
}

/**
 * @brief  文字列に指定桁ごとに区切り文字を挿入する
 * @param  ptr    出力先の文字列ポインタ
 * @param  sep    区切り文字
 * @param  sepDigits  区切り桁数
 * @retval なし
 */
void addThousandSeparator(char *ptr, char sep, uint8_t sepDigits)
{
    char *p = ptr;
    uint8_t count = 0;
    while (*p != '\0')
    {
        if (*p == '.') //小数点のある数値は、そこで桁カウントを打ち切る
        {
            break;
        }
        if (*p == ' ') //空白は数値があるとカウントしない
        {
            p++;
            ptr++;
            continue;
        }
        p++;
        count++;
    }
    if (count <= sepDigits)
    {
        return;
    }
    p = ptr + count - sepDigits;
    while (p > ptr)
    {
        memmove(p + 1, p, strlen(p) + 1);
        *p = sep;
        p--;
    }
}

/**
 * @brief  浮動小数点数をSI接頭辞付きの文字列に変換する
 * @param  value  変換する値
 * @param  prefix SI接頭辞を格納する文字列
 * @retval 変換後の値
 */
float formatSI_Prefix(float value, char *prefix)
{
    if (prefix == NULL)
        return value;

    const char *prefixes[] = {"Y", "Z", "E", "P", "T", "G", "M", "k", "", "m", "u", "n", "p", "f", "a", "z", "y"};
    const float factors[] = {1e24, 1e21, 1e18, 1e15, 1e12, 1e9, 1e6, 1e3, 1, 1e-3, 1e-6, 1e-9, 1e-12, 1e-15, 1e-18, 1e-21, 1e-24};

    float absValue = (value < 0) ? -value : value; // 絶対値を計算

    if ((value == 0.0f) || (value >= FLT_MAX) || (value <= -FLT_MAX))
    {
        //ゼロや過剰に大きな値はリジェクト
        return value;
    }
    else if (absValue >= 1.0f) // 絶対値で比較
    {
        for (uint8_t i = 0; i < sizeof(factors) / sizeof(factors[0]); i++)
        {
            if (absValue >= factors[i]) // 絶対値で比較
            {
                strcpy(prefix, prefixes[i]);
                return value / factors[i];
            }
        }
    }
    else
    {
        for (int8_t i = sizeof(factors) / sizeof(factors[0]) - 1; i >= 0; i--)
        {
            if (absValue <= factors[i]) // 絶対値で比較
            {
                strcpy(prefix, prefixes[i]);
                return value / factors[i];
            }
        }
    }

    return value;
}