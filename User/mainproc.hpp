#ifndef __MAINPROC_HPP
#define __MAINPROC_HPP

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include "stm32f1xx_hal.h"
#include "../Core/Inc/spi.h"
#include "../Core/Inc/adc.h"
#include "../Core/Inc/tim.h"
#include "../Core/Inc/gpio.h"
#include "../Core/Inc/usart.h"
#include "../Core/Inc/dma.h"
#include "../Core/Inc/main.h"

    void procMain(void);
    void procInit(void);
    void procEnd(void);
    void procEvent1msec(void);
    void procEventCompare(void);
    void procEventADC(void);
    void procEventRcvUart(uint8_t *buf);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include "vfd_driver.hpp"
#include "usr_time.hpp"
#include <new>
#include <cstdint>
#include <cstdlib>

#endif

#endif // __MAINPROC_HPP
