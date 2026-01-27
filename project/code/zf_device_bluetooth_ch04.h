/*********************************************************************************************************************
* MM32F327X-G8P Opensourec Library 即（MM32F327X-G8P 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
* 
* 本文件是 MM32F327X-G8P 开源库的一部分
* 
* MM32F327X-G8P 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
* 
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的保证。详见 GPL。
* 
* 您应该在收到本开源库时获得了 GPL 的副本。
* 如果没有，请参阅<http://www.gnu.org/licenses/>
* 
* 文件名称          zf_device_bluetooth_ch04
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          IAR 8.32.4 or MDK 5.37
* 适用平台          MM32F327X_G8P
* 店铺链接          https://seekfree.taobao.com/
* 
* 修改记录
* 日期              作者                备注
* 2026-01-27        YUNYYING            CH-04 蓝牙模块驱动移植
********************************************************************************************************************/
/*********************************************************************************************************************
* 接线定义：
*                   ------------------------------------
*                   模块管脚            单片机管脚
*                   RX                  查看 zf_device_bluetooth_ch04.h 中 CH04_RX_PINx 宏定义
*                   TX                  查看 zf_device_bluetooth_ch04.h 中 CH04_TX_PINx 宏定义
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   ------------------------------------
********************************************************************************************************************/
#ifndef _zf_device_bluetooth_ch04_h_
#define _zf_device_bluetooth_ch04_h_

#include "zf_common_typedef.h"

// ========== UART配置 ==========
#define BLUETOOTH_CH04_INDEX                   ( UART_6 )                      // 蓝牙模块对应使用的串口号（需要根据实际硬件修改）
#define BLUETOOTH_CH04_BUAD_RATE               ( 9600 )                        // 蓝牙模块对应使用的串口波特率（CH-04默认9600）
#define BLUETOOTH_CH04_TX_PIN                  ( UART6_RX_C7 )                 // 蓝牙模块对应模块的 TX 要接到单片机的 RX
#define BLUETOOTH_CH04_RX_PIN                  ( UART6_TX_C6 )                // 蓝牙模块对应模块的 RX 要接到单片机的 TX

// ========== 缓冲区配置 ==========
#define BLUETOOTH_CH04_BUFFER_SIZE             ( 100 )                         // 默认的蓝牙模块使用的接收缓冲区大小
#define BLUETOOTH_CH04_TIMEOUT_COUNT           ( 500 )                         // 发送超时计数

// ========== 协议格式定义 ==========
#define BLUETOOTH_CH04_PACKET_HEADER           ( '[' )                         // 数据包头
#define BLUETOOTH_CH04_PACKET_TAIL             ( ']' )                         // 数据包尾
#define BLUETOOTH_CH04_PACKET_MAX_DATA_LEN     ( 96 )                          // 最大数据长度（缓冲区大小 - 头 - 尾 - 1字符串结束符）

// ========== 函数声明 ==========
uint32      bluetooth_ch04_send_byte           (const uint8 data);
uint32      bluetooth_ch04_send_buffer         (const uint8 *buff, uint32 len);
uint32      bluetooth_ch04_send_string         (const char *str);
uint32      bluetooth_ch04_read_buffer         (uint8 *buff, uint32 len);
uint8       bluetooth_ch04_get_rx_flag         (void);
void        bluetooth_ch04_clear_rx_flag       (void);
void        bluetooth_ch04_uart_callback       (void);
uint8       bluetooth_ch04_init                (void);

#endif
