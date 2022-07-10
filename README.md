# WS2812

# History

| version | date       | Author   | Note                                                         |
| ------- | ---------- | -------- | ------------------------------------------------------------ |
| V0.0.1  | 2021-01-16 | PQ       | First Version                                                |
| V0.0.2  | 2022-07-07 | adadKKKX | fix WS2812_NUM                                               |
| V0.0.3  | 2022-07-10 | PQ       | add comments <br>Remove some useless code <br>modify README to add Preface<br>Modify the directory of the project |





# 简体中文

## Preface

如果你有任何问题可以提issues, 非常欢迎提交PR一起改进这个项目, 如果你有其他问题你可以联系我Frank (pq_liu@foxmail.com)

## Detail

本例程是WS2812的驱动底层，MCU是STM32F103C8，CubeMX+HAL库。本例程MCU主频72M，且一个WS2812只需9Byte的RAM空间。**本例程优势是RAM占用小，且不需要降频。**

* SPI模拟WS2812时序
* SPI的3bit数据模拟WS2812的1bit数据
* 一个WS2812只占用9Byte，RAM占用属于较小
* STM32 时钟设置为72M，SPI分频数设置为32，则SPI的通信频率为2.25M，传输一位数据的时间约为444纳秒（ns）444ns   888ns  符合WS281X芯片的通信时序。
  ```c
  //  __
  // |  |_|   0b110  high level
  //  _   
  // | |__|   0b100  low level
  ```

## Question

### 如何接线

将WS2812的D(I输入脚), 接到SPI的MOSI(输出脚)即可



# English

## Preface

If you have any questions, you can submit issues, and you are very welcome to submit PR to improve this project. If you have other questions, you can contact me Frank (pq_liu@foxmail.com).

## Detail

This routine is WS2812 driver base, MCU is STM32F103C8, CUBEMX +HAL library.This routine MCU main frequency is 72M, and a WS2812 only needs 9Byte RAM space.** The advantage of this routine is that it takes up little RAM and does not need to drop frequency.* *

* SPI simulation WS2812 timing sequence
* SPI's 3bit data simulates WS2812's 1bit data
* A WS2812 only takes up 9 bytes, and the RAM footprint is small
* The STM32 clock is set to 72M, and the SPI sub-frequency is set to 32, so the communication frequency of SPI is 2.25M, and the transmission time of one bit is about 444 nanoseconds (ns). 444ns 888ns is in line with the communication timing sequence of WS281X chip.
  ```c
  //  __
  // |  |_|   0b110  high level
  //  _   
  // | |__|   0b100  low level
  ```

## Question

### How to wire

Connect D (I input pin) of WS2812 to MOSI (output pin) of SPI.