/**
 * @file    ws2812.c
 * @author  Frank (pq_liu@foxmail.com)
 * @brief   WS2812 Driver
 * @version 0.0.3
 * @date    2022-07-10
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WS2812_H
#define __WS2812_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <stdlib.h>

/* Define --------------------------------------------------------------------*/
/*
 * 硬件spi模拟ws2811时序（用spi的3位数据模拟ws281x的一位数据）
 * 要将系统时钟设置为72M，分频数设置为32div，则SPI的通信频率为2.25M，传输一位数据的时间约为444纳秒（ns）
 * 444ns   888ns  符合WS281X芯片的通信时序。
 * 这里是将WS2812的DI输入脚接到STM32 SPI的MOSI脚
 *  __
 * |  |_|   0b110  high level
 *  _   
 * | |__|   0b100  low level
 */
#define WS2812_HIG                      (0x06)
#define WS2812_LOW                      (0x04)

//WS2812的数量
#define WS2812_NUM                      (8u)

/* typedef -------------------------------------------------------------------*/
/**
 * @brief WS2812 date obj
 * @attention 需要注意字节序问题，适用于小端序
 */
// TODO 做好大小端兼容
typedef struct
{
    uint32_t Info;                      //reserve
    union 
    {
        uint8_t Buff[9];
        struct 
        {
            uint8_t G[3];               //G First
            uint8_t R[3];
            uint8_t B[3];               //
        }RGB;
    }Col[WS2812_NUM];
} ws2812_t;

/* types ---------------------------------------------------------------------*/

/* macro ---------------------------------------------------------------------*/


/* functions -----------------------------------------------------------------*/
uint32_t WS2812_Color(uint8_t red, uint8_t green, uint8_t blue);
void WS2812_OneSet(uint16_t num, uint32_t RGB);

void WS2812_CloseAll(void);
void WS2812_SetAll(uint32_t RGB);

void WS2812_ColorWipe(uint32_t RGB, uint16_t wait);

void WS2812_SingleBreatheRainbow(uint16_t wait);
void WS2812_RainbowRotate(uint16_t wait);

void WS2812_TheaterChase(uint32_t RGB, uint16_t wait);
void WS2812_TheaterChaseRainbow(uint16_t wait);

void WS2812_RandAll(uint16_t wait, uint16_t times);
void WS2812_RandColorWipe(uint16_t wait);

#ifdef __cplusplus
}
#endif

#endif /* __WS2812_H */

/******************************** END *****************************************/
