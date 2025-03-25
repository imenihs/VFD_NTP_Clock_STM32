#ifndef __USR_FTOA_H
#define __USR_FTOA_H

#ifdef __cplusplus
extern "C"
{
#endif
// C定義
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <float.h>

#define FLOAT_MAX_DIGITS (9)
#define MAX_DIGITS (11)

    void fillChar(char *ptr, char c, int8_t len);
    void usr_ftoa_sig(float value, char *ptr, uint8_t outputDigits, uint8_t significantDigits, uint8_t addThousandSeparat);
    void usr_ftoa_dec(float value, char *ptr, uint8_t outputDigits, uint8_t intDigits, uint8_t decDigits, uint8_t addThousandSeparat);
    void addThousandSeparator(char *ptr, char sep, uint8_t sepDigits);
    float formatSI_Prefix(float value, char *prefix);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
// C++定義

#endif

#endif //__USR_FTOA_H