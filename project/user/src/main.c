/*********************************************************************************************************************
* HC-04 蓝牙模块测试程序（修正OLED坐标）
********************************************************************************************************************/

#include "zf_common_headfile.h"
#include "bluetooth_hc04.h"
#include "zf_device_oled.h"
#include "zf_device_key.h"
#include <string.h>
#include <stdlib.h>

// **************************** 代码区域 ****************************
int main(void)
{
    clock_init(SYSTEM_CLOCK_120M);
    debug_init();

    // ========== OLED初始化 ==========
    oled_init();
    oled_set_font(OLED_8X16_FONT);    // 8x16字体，占2页高度
    oled_clear();
    
    // ========== 按键初始化 ==========
    key_init(10);
    pit_ms_init(TIM6_PIT, 10);
    
    // ========== 蓝牙初始化 ==========
    hc04_init();
    
    // ========== 启动信息 ==========
    printf("\r\n========================================\r\n");
    printf("HC-04 Bluetooth Test Program\r\n");
    printf("Debug UART:  UART1 @ 115200 (DAP)\r\n");
    printf("BT UART:     UART6 @ 9600   (HC-04)\r\n");
    printf("========================================\r\n");
    
    // 📱 发送启动信息到手机
    hc04_printf("=== HC-04 Test ===\r\n");
    hc04_printf("System Ready!\r\n");
    
    // 📺 OLED显示（注意：y是页坐标0-7，8x16字体占2页）
    oled_show_string(0, 0, "BT Test");      // 第1行：页0
    oled_show_string(0, 2, "Key: ---");     // 第2行：页2（8x16字体占2页，所以+2）
    oled_show_string(0, 4, "RX: ---");      // 第3行：页4
    oled_show_string(0, 6, "Ready");        // 第4行：页6

    while(1)
    {
        // ==================== 按键1测试 ====================
        if (KEY_SHORT_PRESS == key_get_state(KEY_1))
        {
            key_clear_state(KEY_1);
            
            printf("[KEY1] Pressed\r\n");
            hc04_printf("KEY1 Pressed\r\n");
            
            // 📺 OLED显示（清空后显示新内容）
            oled_show_string(0, 2, "Key: 1     ");    // 页2
            oled_show_string(0, 4, "TX: KEY1   ");    // 页4
        }
        
        // ==================== 按键2测试 ====================
        if (KEY_SHORT_PRESS == key_get_state(KEY_2))
        {
            key_clear_state(KEY_2);
            
            printf("[KEY2] Pressed\r\n");
            hc04_printf("KEY2 Pressed\r\n");
            
            oled_show_string(0, 2, "Key: 2     ");
            oled_show_string(0, 4, "TX: KEY2   ");
        }
        
        // ==================== 按键3测试 ====================
        if (KEY_SHORT_PRESS == key_get_state(KEY_3))
        {
            key_clear_state(KEY_3);
            
            printf("[KEY3] Pressed\r\n");
            hc04_printf("KEY3 Pressed\r\n");
            
            oled_show_string(0, 2, "Key: 3     ");
            oled_show_string(0, 4, "TX: KEY3   ");
        }
        
        // ==================== 按键4测试 ====================
        if (KEY_SHORT_PRESS == key_get_state(KEY_4))
        {
            key_clear_state(KEY_4);
            
            printf("[KEY4] Pressed\r\n");
            hc04_printf("KEY4 Pressed\r\n");
            
            oled_show_string(0, 2, "Key: 4     ");
            oled_show_string(0, 4, "TX: KEY4   ");
        }
        
        // ==================== 接收蓝牙数据测试 ====================
        if (hc04_rx_flag == 1)
        {
            printf("[BT_RX] %s\r\n", hc04_rx_packet);
            hc04_printf("Echo: %s\r\n", hc04_rx_packet);
            
            // 📺 OLED显示收到的数据（最多显示16个字符，超出会被截断）
            oled_show_string(0, 4, "RX:            ");  // 先清空
            oled_show_string(24, 4, hc04_rx_packet);    // x=24 是"RX:"后面的位置
            
            hc04_rx_flag = 0;
        }
    }
}
// **************************** 代码区域 ****************************
