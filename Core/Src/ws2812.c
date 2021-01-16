/**
 * @file    ws2812.c
 * @author  Frank (pq_liu@foxmail.com)
 * @brief   WS2812 Driver
 * @version 0.1
 * @date    2021-01-16
 * 
 * @copyright Copyright (c) 2021
 * 
 */
/* Includes ------------------------------------------------------------------*/
#include "ws2812.h"


/* Define --------------------------------------------------------------------*/
#define delay_ms(x)                 HAL_Delay(x)

/* Variables -----------------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_tx;

ws2812_t g_ws2812={0};
/* Functions Prototypes ------------------------------------------------------*/
void WS2812_Show(void);


/* Functions -----------------------------------------------------------------*/


/**
 * @brief uint8 RGB 转 uint32
 * 
 * @param red 
 * @param green 
 * @param blue 
 * @return uint32_t 
 */
uint32_t WS2812_Color(uint8_t red, uint8_t green, uint8_t blue)
{
    return red << 16 | green << 8 | blue;
}

/**
 * @brief 设置某一个WS2812
 * 
 * @param num 
 * @param RGB 
 */
void WS2812_OneSet( uint8_t num, uint32_t RGB )
{
    uint8_t i;
    uint32_t TempR = 0, TempG = 0, TempB = 0;

    //MSB First
    for( i = 0; i < 8; ++i,RGB>>=1 )
    {
        (RGB & 0x00010000) != 0 ? (TempR |= (WS2812_HIG<<(i*3))) : (TempR |= (WS2812_LOW<<(i*3)));
        (RGB & 0x00000100) != 0 ? (TempG |= (WS2812_HIG<<(i*3))) : (TempG |= (WS2812_LOW<<(i*3)));
        (RGB & 0x00000001) != 0 ? (TempB |= (WS2812_HIG<<(i*3))) : (TempB |= (WS2812_LOW<<(i*3)));
    }

    for (i = 0; i < 3; i++)
    {
        g_ws2812.Col[num].RGB.R[i] = TempR >> (16-8*i);
        g_ws2812.Col[num].RGB.G[i] = TempG >> (16-8*i);
        g_ws2812.Col[num].RGB.B[i] = TempB >> (16-8*i);
    }
}

/**
 * @brief   将数据通过DMA发送去WS2812
 * 
 */
void WS2812_Show(void)
{
    HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*)&g_ws2812.Col[0].Buff, 9*WS2812_NUM);
}

/**
 * @brief   关闭所有的WS2812
 * 
 */
void WS2812_CloseAll(void)
{
    uint16_t i;

    for (i = 0; i < WS2812_NUM; ++i)
    {
        WS2812_OneSet(i, 0);
    }
    WS2812_Show();
}

/**
 * @brief   给所有的WS2812设定某一个颜色
 * 
 * @param RGB 
 */
void WS2812_SetAll(uint32_t RGB)
{
    uint16_t i;

    for (i = 0; i < WS2812_NUM; ++i)
    {
        WS2812_OneSet(i, RGB);
    }
    WS2812_Show();
}

// Fill the dots one after the other with a color
void WS2812_ColorWipe(uint32_t c, uint16_t wait)
{
    for (uint16_t i = 0; i < WS2812_NUM; i++)
    {
        WS2812_OneSet(i, c);
        WS2812_Show();
        delay_ms(wait);
    }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t WS2812_Wheel(uint8_t wheelPos)
{
    wheelPos = 255 - wheelPos;
    if (wheelPos < 85)
    {
        return WS2812_Color(255 - wheelPos * 3, 0, wheelPos * 3);
    }
    if (wheelPos < 170)
    {
        wheelPos -= 85;
        return WS2812_Color(0, wheelPos * 3, 255 - wheelPos * 3);
    }
    wheelPos -= 170;
    return WS2812_Color(wheelPos * 3, 255 - wheelPos * 3, 0);
}

void WS2812_SingleBreatheRainbow(uint16_t wait)
{
    uint16_t i, j;

    for (j = 0; j < 256; j++)
    {
        for (i = 0; i < WS2812_NUM; i++)
        {
            WS2812_OneSet(i, WS2812_Wheel((i + j) & 255));
        }
        WS2812_Show();
        delay_ms(wait);
    }
}

// Slightly different, this makes the rainbow equally distributed throughout
void  WS2812_RainbowRotate(uint16_t wait)
{
    uint16_t i, j;

    for (j = 0; j < 256 * 5; j++)
    { // 5 cycles of all colors on wheel
        for (i = 0; i < WS2812_NUM; i++)
        {
            WS2812_OneSet(i, WS2812_Wheel(((i * 256 / WS2812_NUM) + j) & 255));
        }
        WS2812_Show();
        delay_ms(wait);
    }
}

//Theatre-style crawling lights.
void WS2812_TheaterChase(uint32_t c, uint16_t wait)
{
    for (int j = 0; j < 10; j++)            //do 10 cycles of chasing
    { 
        for (int q = 0; q < 4; q++)
        {
            for (uint16_t i = 0; i < WS2812_NUM; i = i + 4)
            {
                WS2812_OneSet(i + q, c);    //turn x pixel on
            }
            WS2812_Show();

            delay_ms(wait);

            for (uint16_t i = 0; i < WS2812_NUM; i = i + 4)
            {
                WS2812_OneSet(i + q, 0);    //turn x pixel off
            }
        }
    }
}

//Theatre-style crawling lights with rainbow effect
void WS2812_TheaterChaseRainbow(uint16_t wait)
{
    for (int j = 0; j < 256; j++)           // cycle all 256 colors in the wheel
    { 
        for (int q = 0; q < 4; q++)
        {
            for (uint16_t i = 0; i < WS2812_NUM; i = i + 4)
            {
                WS2812_OneSet(i + q, WS2812_Wheel((i + j) % 255));  //turn x pixel on
            }
            WS2812_Show();

            delay_ms(wait);

            for (uint16_t i = 0; i < WS2812_NUM; i = i + 4)
            {
                WS2812_OneSet(i + q, 0);                            //turn x pixel off
            }
        }
    }
}

void WS2812_RandAll(uint16_t wait)
{
    uint16_t i;
    
    for (i = 0; i < 125; i++)
    {
        WS2812_SetAll(rand()%0x01000000);
        delay_ms(wait);
    }
}

void WS2812_RandColorWipe(uint16_t wait)
{
    uint16_t i,j;
    uint32_t temp32[WS2812_NUM]={0};
    
    for (i = 0; i < 255; i++)
    {
        temp32[0] = rand()%0x01000000;
        for (j = WS2812_NUM-1; j > 0; --j)
        {
            temp32[j] = temp32[j-1];
        }
        
        for (j = 0; j < WS2812_NUM; j++)
        {
            WS2812_OneSet(j, temp32[j]);
        }
        WS2812_Show();
        delay_ms(wait);
    }
}

