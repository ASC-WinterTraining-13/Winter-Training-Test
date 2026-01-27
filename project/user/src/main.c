/*********************************************************************************************************************
* MM32F327X-G8P 蓝牙CH-04 收发测试 - 回显模式
********************************************************************************************************************/

#include "zf_common_headfile.h"
#include "zf_device_bluetooth_ch04.h"

int main(void)
{
    clock_init(SYSTEM_CLOCK_120M);
    debug_init();
    bluetooth_ch04_init();
    
    printf("\r\n========== Bluetooth CH-04 Echo Test ==========\r\n");
    printf("Waiting for data...\r\n");
    printf("Send any data in format: [data]\r\n\r\n");
    
    while(1)
    {
        if(bluetooth_ch04_get_rx_flag())
        {
            uint8 rx_buffer[100];
            uint32 rx_length = bluetooth_ch04_read_buffer(rx_buffer, 100);
            
            if(rx_length > 0)
            {
                rx_buffer[rx_length] = '\0';
                
                // 显示接收到的数据
                printf("RX: [");
                for(uint32 i = 0; i < rx_length; i++)
                {
                    printf("%c", rx_buffer[i]);
                }
                printf("]\r\n");
                
                // ★★★ 重点改动：直接返回接收到的数据 ★★★
                printf("TX: [");
                for(uint32 i = 0; i < rx_length; i++)
                {
                    printf("%c", rx_buffer[i]);
                }
                printf("]\r\n");
                
                // 发送原数据给蓝牙
                bluetooth_ch04_send_string("[");
                bluetooth_ch04_send_buffer(rx_buffer, rx_length);
                bluetooth_ch04_send_string("]");
            }
        }
        
        system_delay_ms(100);
    }
    
    return 0;
}
