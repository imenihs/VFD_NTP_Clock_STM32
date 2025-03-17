#ifndef __USR_FTOA_H
#define __USR_FTOA_H

#ifdef __cplusplus
extern "C"
{
#endif
//C定義
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define FLOAT_MAX_DIGITS (9)
#define MAX_DIGITS (11)

void fillChar(char *ptr, char c, uint8_t len);
void usr_floa_sig(float value, char *ptr, uint8_t outputDigits, uint8_t significantDigits);
void usr_floa_dec(float value, char *ptr, uint8_t intDigits, uint8_t decDigits);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
//C++定義

#endif

#endif //__USR_FTOA_H