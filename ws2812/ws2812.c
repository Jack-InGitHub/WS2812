/**
 * @file    ws2812.c
 * @author  Frank (pq_liu@foxmail.com)
 * @brief   WS2812 Driver
 * @version 0.0.3
 * @date    2022-07-10
 * 
 * @section   History
 * <table>
 *     <tr><th>Version <th>Data        <th>Author   <th>Notes
 *     <tr><td>V0.0.1  <td>2021-01-16  <td>PQ       <td>First Version
 *     <tr><td>V0.0.2  <td>2022-07-07  <td>adadKKKX <td>fix WS2812_NUM
 *     <tr><td>V0.0.3  <td>2022-07-10  <td>PQ       <td>add comments & Remove some useless code
 * </table>
 */
/* Includes ------------------------------------------------------------------*/
#include "ws2812.h"


/* Define --------------------------------------------------------------------*/
// Please modify the code here if you port it.
// 移植的时候记得修改这些宏
#define delay_ms(x)                     HAL_Delay(x)
#define SPI_TRANSMIT(buf, size)         HAL_SPI_Transmit_DMA(&hspi1, (buf), (size));

/* Variables -----------------------------------------------------------------*/
extern SPI_HandleTypeDef                hspi1;

static ws2812_t ws2812={0};
/* Functions Prototypes ------------------------------------------------------*/
static __inline void WS2812_Show(void);


/* Functions -----------------------------------------------------------------*/


/**
 * @brief uint8 RGB 转 uint32
 * 
 * @param red    [0,255]
 * @param green  [0,255]
 * @param blue   [0,255]
 * @return uint32_t RGB value
 */
uint32_t WS2812_Color(uint8_t red, uint8_t green, uint8_t blue)
{
    return red << 16 | green << 8 | blue;
}

/**
 * @brief 设置某一个WS2812
 * 
 * @param num [0,WS2812_NUM]  WS2812_NUM<65536
 * @param RGB Uint32 RGB value
 */
void WS2812_OneSet( uint16_t num, uint32_t RGB )
{
    uint8_t i;
    // 这里存放的是真实的编码数据 只使用了 3bit*8=24bit
    uint32_t TempR = 0, TempG = 0, TempB = 0;

    //MSB First
    for( i = 0; i < 8; ++i,RGB>>=1 )
    {
        (RGB & 0x00010000) != 0 ? (TempR |= (WS2812_HIG<<(i*3))) : (TempR |= (WS2812_LOW<<(i*3)));
        (RGB & 0x00000100) != 0 ? (TempG |= (WS2812_HIG<<(i*3))) : (TempG |= (WS2812_LOW<<(i*3)));
        (RGB & 0x00000001) != 0 ? (TempB |= (WS2812_HIG<<(i*3))) : (TempB |= (WS2812_LOW<<(i*3)));
    }
    
    // TODO 这一块有待优化
    for (i = 0; i < 3; i++)
    {
        ws2812.Col[num].RGB.R[i] = TempR >> (16-8*i);
        ws2812.Col[num].RGB.G[i] = TempG >> (16-8*i);
        ws2812.Col[num].RGB.B[i] = TempB >> (16-8*i);
    }
}

/**
 * @brief   将数据通过DMA发送到WS2812
 * 
 */
static __inline void WS2812_Show(void)
{
    SPI_TRANSMIT( (uint8_t*)&ws2812.Col[0].Buff[0], 9*WS2812_NUM );
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
 * @param RGB Uint32 RGB value
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


/**
 * @brief  Fill the dots one after the other with a color
 * 
 * @param  RGB:Uint32 RGB value
 * @param  wait:wait time(ms)
 */
void WS2812_ColorWipe(uint32_t RGB, uint16_t wait)
{
    for (uint16_t i = 0; i < WS2812_NUM; i++)
    {
        WS2812_OneSet(i, RGB);
        WS2812_Show();
        delay_ms(wait);
    }
}

/**
 * @brief Input a value 0 to 255 to get a color value. The colours are a transition r - g - b - back to r.
 * @param wheelPos 
 * @return uint32
 * 
 */
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


// TODO 全彩色的 wheel

/**
 * @brief Input a value 0 to 255 to get a color value. The colours are a transition r - g - b - back to r.
 * @param wait  wait time(ms)
 * @return 
 * 
 */
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

/**
 * @brief this makes the rainbow equally distributed throughout
 * @param wait  wait time(ms)
 * @return 
 * 
 */
void  WS2812_RainbowRotate(uint16_t wait)
{
    uint16_t i, j;

    for (j = 0; j < 256 * 5; j++)
    {   
        // 5 cycles of all colors on wheel
        for (i = 0; i < WS2812_NUM; i++)
        {
            WS2812_OneSet(i, WS2812_Wheel(((i * 256 / WS2812_NUM) + j) & 255));
        }
        WS2812_Show();
        delay_ms(wait);
    }
}

/**
 * @brief  Fill the dots one after the other with a color
 * 
 * @param  RGB:Uint32 RGB value
 * @param  wait:wait time(ms)
 */
void WS2812_TheaterChase(uint32_t RGB, uint16_t wait)
{
    for (uint16_t j = 0; j < 10; j++)       //do 10 cycles of chasing
    { 
        for (uint16_t q = 0; q < 4; q++)
        {
            for (uint16_t i = 0; i < WS2812_NUM; i = i + 4)
            {
                WS2812_OneSet(i + q, RGB);  //turn x pixel on
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

/**
 * @brief  Theatre-style crawling lights with rainbow effect
 * 
 * @param  wait:wait time(ms)
 */
void WS2812_TheaterChaseRainbow(uint16_t wait)
{
    for (uint16_t j = 0; j < 256; j++)                              // cycle all 256 colors in the wheel
    { 
        for (uint16_t q = 0; q < 4; q++)
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

/**
 * @brief  set rand color of all ws2812
 * 
 * @param  wait:wait time(ms)
 * @param  times:flash times
 */
void WS2812_RandAll(uint16_t wait,uint16_t times)
{
    uint16_t i;
    
    for (i = 0; i < times; i++)
    {
        WS2812_SetAll(rand()%0x01000000);
        delay_ms(wait);
    }
}

/**
 * @brief  set rand color one by one
 * 
 * @param  wait:wait time(ms)
 */
void WS2812_RandColorWipe(uint16_t wait)
{
    uint16_t i,j;
    uint32_t temp32[WS2812_NUM]={0};
    
    for (i = 0; i < WS2812_NUM; i++)
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

