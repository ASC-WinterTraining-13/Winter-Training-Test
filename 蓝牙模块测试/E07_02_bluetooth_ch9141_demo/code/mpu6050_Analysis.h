#ifndef __MPU6050_ANALYSIS_H
#define __MPU6050_ANALYSIS_H

#include "zf_common_typedef.h"

// 校准相关宏定义
#define CALIB_TARGET_SAMPLES    400  // 校准目标样本数

// MPU6050滤波器配置结构体
typedef struct {
    float comp_filter_alpha;    // 互补滤波系数 (0-1)
    float lpf_alpha;            // 低通滤波系数 (0-1)
    float sample_time;          // 采样时间 (秒)
    float gyro_threshold;       // 陀螺仪死区阈值 (dps)
} MPU6050_FilterConfig_t;

// MPU6050姿态角结构体
typedef struct {
    float roll;         // 横滚角 (度)
    float pitch;        // 俯仰角 (度)
    float yaw;          // 偏航角 (度)
    
    float roll_acc;     // 加速度计计算的横滚角
    float pitch_acc;    // 加速度计计算的俯仰角
    float roll_gyro;    // 陀螺仪计算的横滚角
    float pitch_gyro;   // 陀螺仪计算的俯仰角
    float yaw_gyro;     // 陀螺仪计算的偏航角
} MPU6050_Attitude_t;

// MPU6050校准结构体
typedef struct {
    float gyro_offset_x;    // 陀螺仪X轴偏移
    float gyro_offset_y;    // 陀螺仪Y轴偏移
    float gyro_offset_z;    // 陀螺仪Z轴偏移
    uint16_t calib_samples; // 校准样本数
    uint8_t is_calibrated;  // 是否已校准
} MPU6050_Calib_t;

// 外部变量声明
extern MPU6050_Attitude_t mpu6050_attitude;
extern MPU6050_FilterConfig_t mpu6050_filter_config;
extern MPU6050_Calib_t mpu6050_calib;

// 校准相关变量（提供给Mode_1.c使用）
extern uint16_t calib_count;  // 校准计数

// 旧API全局变量（保持兼容性）
extern float Roll_Result;
extern float Pitch_Result;
extern float Yaw_Result;

// 函数声明
void MPU6050_Analysis_Init(void);
void MPU6050_Analysis_SetFilter(const MPU6050_FilterConfig_t *config);
void MPU6050_Analysis_Update(void);
void MPU6050_Calibration_Start(void);
uint8_t MPU6050_Calibration_Check(void);
const MPU6050_Attitude_t* MPU6050_GetAttitude(void);

// 旧API函数（保持兼容性）
void MPU6050_Analysis(void);

#endif /* __MPU6050_ANALYSIS_H */