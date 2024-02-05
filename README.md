# WS2812 V0.1.1

# History

| version | date       | Author   | Note                                                         |
| ------- | ---------- | -------- | ------------------------------------------------------------ |
| V0.0.1  | 2021-01-16 | PQ       | First Version                                                |
| V0.0.2  | 2022-07-07 | adadKKKX | fix WS2812_NUM                                               |
| V0.0.3  | 2022-07-10 | PQ       | add comments <br>Remove some useless code <br>modify README to add Preface<br>Modify the directory of the project |
| V0.1.0  | 2022-07-11 | PQ       | Modifying version Information                                |
| V0.1.1  | 2024-02-05 | PQ       | Update README                                                |






# 简体中文

## Preface

如果你有任何问题可以提issues, 非常欢迎提交PR一起改进这个项目, 如果你有其他问题你可以联系我(pq_liu@foxmail.com)

## Detail

本例程是WS2812的驱动底层，MCU是STM32F103C8，CubeMX+HAL库。

本例程使用SPI+DMA方式驱动，在所有方式里面**性能最好**，在SPI+DMA方式里面**资源占用最优**。

### 主流驱动方式分析：

我们来分析一下现在主流的三种驱动方式：

1.   使用 GPIO+延时 处理。
     *   这种方案一般不推荐，CPU被完全占用了性能最差的方案，除非实在无法使用另外两种，或者外设资源不足(8位51核单片机等)的情况下才考虑使用。
2.   使用 PWM+DMA 设定特定占空比来实现。
     *   这种方式需要DMA中断频繁参与，有一个优势就是兼容性好，不管什么样占空比的WS2812波形都能较好还原。
     *   [CSDN HZ1213825](https://blog.csdn.net/m0_57585228/article/details/127157823) 实现了此方法[代码](https://github.com/HZ1213825/HAL_STM32F4_WS2812)，问题在于每个WS2812数据发送完成都会产生中断，**中断过于频繁**，由CSDN的逻辑分析图可以看出0.1ms内中断了3次，<u>频繁的中断对系统来说也是致命且忌讳的</u>。也正是因为这个频繁的中断，使得他可以使每个WS2812使用较少的RAM来存储缓存数值，代码中每个WS2812需要4Byte(最低可以优化成3Byte)。
3.   使用 SPI+DMA 来实现。
     1.   这个实现方式需要占用一个SPI Unit，如果外部加上一个使能电路，即可挂载在总线上实现一主多从驱动多个SPI设备。
     2.   这个方式实现发送过程中不占用CPU时间，不触发中断，效率很高，优化后RAM占用也很低。

### 特征：

* SPI模拟WS2812时序
* SPI的3bit数据模拟WS2812的1bit数据。（常规方式使用0xFE、0xE0作为10发送，每个WS2812需要占用24Byte的空间）
* 一个WS2812只占用9Byte，RAM占用较小
* STM32 时钟设置为72M，SPI分频数设置为32，则SPI的通信频率为**2.25M**，传输一位数据的时间约为444纳秒（ns）444ns   888ns  符合WS281X芯片的通信时序。(部分SPI+DMA实现需要将STM32的时钟降频)
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
