/*********************************************************************************************************************
* MM32F327X-G8P 蓝牙CH-04 收发测试 - 回显模式
********************************************************************************************************************/

#include "zf_common_headfile.h"

#include "motor.h"

int main(void)
{
    clock_init(SYSTEM_CLOCK_120M);
    debug_init();
    
	/* （七针脚）OLED初始化*/
    oled_init();
    // 可以使用的字体为6X8和8X16,点阵坐标等效是基于6X8标定的
    oled_set_font(OLED_6X8_FONT);    
	oled_clear();
	
	/* 按键初始化（10ms扫描周期）*/
    key_init(10);
	
	pit_ms_init(TIM6_PIT, 10);
	
	int16_t PWM_L = 0;
	int16_t PWM_R = 0;
	
	oled_show_string(0, 0, "PWM_L:");
	oled_show_string(0, 1, "PWM_R:");
	
	motor_Init();
	
    while(1)
    {
        if (KEY_SHORT_PRESS == key_get_state(KEY_1))
        {
            key_clear_state(KEY_1);
			PWM_L += 100;
			motor_SetPWM(1, PWM_L);
        }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_2))
        {
            key_clear_state(KEY_2);
			PWM_L -= 100;
			motor_SetPWM(1, PWM_L);
        }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_3))
        {
            key_clear_state(KEY_3);
			PWM_R += 100;
			motor_SetPWM(2, PWM_R);
        }
        else if (KEY_SHORT_PRESS == key_get_state(KEY_4))
        {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
            key_clear_state(KEY_4);
			PWM_R -= 100;
			motor_SetPWM(2, PWM_R);
        }
		oled_show_int(36, 0, PWM_L, 6);
		oled_show_int(36, 1, PWM_R, 6);
		
		
    }
    
    return 0;
}
