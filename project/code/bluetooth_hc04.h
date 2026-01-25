/*********************************************************************************************************************
* HC-04 蓝牙串口模块驱动（带数据包解析）
* 数据包格式：[tag,param1,param2,...]
* 适配手机蓝牙调试APP
* 
* 使用方法：
* 1. 将本文件和 bluetooth_hc04.c 添加到工程
* 2. 在 main.c 中 #include "bluetooth_hc04.h"
* 3. 在 main.c 中调用 hc04_init() 初始化
* 4. 在 isr.c 的 UART6_IRQHandler() 中调用 hc04_uart_callback()
********************************************************************************************************************/
#ifndef _BLUETOOTH_HC04_H_
#define _BLUETOOTH_HC04_H_

#include "zf_common_typedef.h"
#include <stdio.h>

// ==================== 配置参数 ====================
#define HC04_UART_INDEX         ( UART_6 )          // 使用的串口号
#define HC04_BAUDRATE           ( 9600 )            // HC-04默认9600
#define HC04_TX_PIN             ( UART6_TX_C6 )     // 单片机TX连接模块RX
#define HC04_RX_PIN             ( UART6_RX_C7 )     // 单片机RX连接模块TX

#define HC04_RX_PACKET_SIZE     ( 100 )             // 接收数据包缓冲区大小

// ==================== 外部变量 ====================
extern char     hc04_rx_packet[HC04_RX_PACKET_SIZE];    // 接收数据包缓冲区
extern uint8    hc04_rx_flag;                            // 接收完成标志位（1=收到完整数据包）

// ==================== 函数声明 ====================
// 初始化
uint8   hc04_init               (void);

// 发送函数
void    hc04_send_byte          (const uint8 data);
void    hc04_send_buffer        (const uint8 *buff, uint32 len);
void    hc04_send_string        (const char *str);
void    hc04_printf             (const char *format, ...);      // 格式化输出（推荐）

// 中断回调（在ISR中调用）
void    hc04_uart_callback      (void);

#endif
