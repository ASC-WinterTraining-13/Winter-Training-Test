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
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
* 
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
* 
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
* 
* 文件名称          main
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          IAR 8.32.4 or MDK 5.37
* 适用平台          MM32F327X_G8P
* 店铺链接          https://seekfree.taobao.com/
* 
* 修改记录
* 日期              作者                备注
* 2022-08-10        Teternal            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"

#include "zf_device_oled.h"
#include "zf_driver_timer.h"


// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完

// 本例程是开源库移植用空工程

#define KEY1                    (E2 )
#define KEY2                    (E3 )
#define KEY3                    (E4 )
#define KEY4                    (E5 )

#define SWITCH1                 (D3 )
#define SWITCH2                 (D4 )

// **************************** 代码区域 ****************************
int main(void)       
{
	clock_init(SYSTEM_CLOCK_120M);                                              // 初始化芯片时钟 工作频率为 120MHz
    debug_init();                                                               // 初始化默认 Debug UART

	//OLED初始化
	oled_init();
	//另一个可以使用的字体为8X16,但是坐标仍然是基于6X8标定的
	oled_set_font(OLED_6X8_FONT);	
	
	//按键初始化
	gpio_init(KEY1, GPI, GPIO_HIGH, GPI_PULL_UP);                               // 初始化 KEY1 输入 默认高电平 上拉输入
	gpio_init(KEY2, GPI, GPIO_HIGH, GPI_PULL_UP);                               // 初始化 KEY2 输入 默认高电平 上拉输入
	gpio_init(KEY3, GPI, GPIO_HIGH, GPI_PULL_UP);                               // 初始化 KEY3 输入 默认高电平 上拉输入
	gpio_init(KEY4, GPI, GPIO_HIGH, GPI_PULL_UP);                               // 初始化 KEY4 输入 默认高电平 上拉输入

	//拨码开关初始化
	gpio_init(SWITCH1, GPI, GPIO_HIGH, GPI_FLOATING_IN);                        // 初始化 SWITCH1 输入 默认高电平 浮空输入
	gpio_init(SWITCH2, GPI, GPIO_HIGH, GPI_FLOATING_IN);                        // 初始化 SWITCH2 输入 默认高电平 浮空输入
	
	
	uint16_t i = 0, j = 0;
	
    while(1)
    {
			
		//按键按下电平为0
		i = 0, j = 0;
		if (gpio_get_level(KEY4))
		{
			i = 1;
		}
		if (!gpio_get_level(KEY4))
		{
			j = 1;
		}
			
			
		//拨码开关上0，下一		
//		if(gpio_get_level(SWITCH1))
//		{
//			i = 1;
//		}
//		if(!gpio_get_level(SWITCH1))
//		{
//			j = 1;
//		}
			
			
		oled_show_uint(0, 0, i, 4);
		oled_show_uint(0, 1, j, 4);
				
    }
}
// **************************** 代码区域 ****************************
