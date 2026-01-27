#include "mpu6050_Analysis.h"
#include "zf_device_mpu6050.h"
#include "timer_flag.h"  // 添加这一行
#include <math.h>
#include <string.h>  // 添加memset函数需要

// 常数定义
#define PI              3.14159265358979323846f
#define RAD_TO_DEG      (180.0f / PI)
#define DEG_TO_RAD      (PI / 180.0f)

// MPU6050默认配置
#define DEFAULT_COMP_FILTER_ALPHA    0.005f     // 互补滤波系数
#define DEFAULT_LPF_ALPHA            0.3f       // 低通滤波系数
#define DEFAULT_SAMPLE_TIME          0.005f     // 5ms采样周期
#define DEFAULT_GYRO_THRESHOLD       0.5f       // 陀螺仪死区阈值(dps)
#define DEFAULT_CALIB_SAMPLES        400        // 校准样本数(2秒@5ms)

// 全局变量定义
MPU6050_Attitude_t mpu6050_attitude = {0};
MPU6050_FilterConfig_t mpu6050_filter_config = {
    .comp_filter_alpha = DEFAULT_COMP_FILTER_ALPHA,
    .lpf_alpha = DEFAULT_LPF_ALPHA,
    .sample_time = DEFAULT_SAMPLE_TIME,
    .gyro_threshold = DEFAULT_GYRO_THRESHOLD
};

MPU6050_Calib_t mpu6050_calib = {
    .gyro_offset_x = 0.0f,
    .gyro_offset_y = 0.0f,
    .gyro_offset_z = 0.0f,
    .calib_samples = DEFAULT_CALIB_SAMPLES,
    .is_calibrated = 0
};

// 私有变量 - 现在改为全局变量，在头文件中声明
uint16_t calib_count = 0;  // 去掉static，改为全局
int32_t sum_gx = 0, sum_gy = 0, sum_gz = 0;
static uint8_t calib_state = 0; // 0:空闲, 1:校准中, 2:校准完成

// 私有函数声明
static void apply_deadzone(float *value, float threshold);
static void apply_gyro_calibration(void);
static void calculate_attitude_from_accel(void);
static void integrate_gyro_data(void);
static void apply_complementary_filter(void);
static void apply_low_pass_filter(void);

/**
  * @brief  MPU6050姿态解算初始化
  * @retval 无
  */
void MPU6050_Analysis_Init(void)
{
    // 初始化变量
    memset(&mpu6050_attitude, 0, sizeof(MPU6050_Attitude_t));
    
    // 使用默认滤波器配置
    mpu6050_filter_config.comp_filter_alpha = DEFAULT_COMP_FILTER_ALPHA;
    mpu6050_filter_config.lpf_alpha = DEFAULT_LPF_ALPHA;
    mpu6050_filter_config.sample_time = DEFAULT_SAMPLE_TIME;
    mpu6050_filter_config.gyro_threshold = DEFAULT_GYRO_THRESHOLD;
    
    // 初始化校准结构
    mpu6050_calib.gyro_offset_x = 0.0f;
    mpu6050_calib.gyro_offset_y = 0.0f;
    mpu6050_calib.gyro_offset_z = 0.0f;
    mpu6050_calib.is_calibrated = 0;
    mpu6050_calib.calib_samples = DEFAULT_CALIB_SAMPLES;
    
    calib_state = 0;
    calib_count = 0;
    sum_gx = sum_gy = sum_gz = 0;
}

/**
  * @brief  设置滤波器配置参数
  * @param  config: 滤波器配置结构体指针
  * @retval 无
  */
void MPU6050_Analysis_SetFilter(const MPU6050_FilterConfig_t *config)
{
    if (config == NULL) return;
    
    // 参数有效性检查
    mpu6050_filter_config.comp_filter_alpha = 
        (config->comp_filter_alpha < 0) ? 0 : 
        (config->comp_filter_alpha > 1) ? 1 : config->comp_filter_alpha;
    
    mpu6050_filter_config.lpf_alpha = 
        (config->lpf_alpha < 0) ? 0 : 
        (config->lpf_alpha > 1) ? 1 : config->lpf_alpha;
    
    mpu6050_filter_config.sample_time = 
        (config->sample_time <= 0) ? DEFAULT_SAMPLE_TIME : config->sample_time;
    
    mpu6050_filter_config.gyro_threshold = config->gyro_threshold;
}

/**
  * @brief  开始MPU6050陀螺仪校准
  * @retval 无
  */
void MPU6050_Calibration_Start(void)
{
    calib_state = 1;  // 开始校准
    calib_count = 0;
    sum_gx = 0;
    sum_gy = 0;
    sum_gz = 0;
    mpu6050_calib.is_calibrated = 0;
}

/**
  * @brief  检查校准状态并处理校准数据
  * @retval 校准状态: 0-校准完成, 1-校准中
  */
uint8_t MPU6050_Calibration_Check(void)
{
    if (calib_state != 1) {
        return 0;  // 不在校准状态
    }
    
    // 检查是否有新数据
    if (mpu6050_analysis_enable) {  // 这个变量现在在timer_flag.h中声明
        // 读取原始数据
        mpu6050_get_data();
        mpu6050_analysis_enable = 0;
        
        // 累加陀螺仪数据
        sum_gx += mpu6050_gyro_x;
        sum_gy += mpu6050_gyro_y;
        sum_gz += mpu6050_gyro_z;
        calib_count++;
        
        // 检查是否达到目标样本数
        if (calib_count >= mpu6050_calib.calib_samples) {
            // 计算平均值作为零偏
            mpu6050_calib.gyro_offset_x = (float)sum_gx / mpu6050_calib.calib_samples;
            mpu6050_calib.gyro_offset_y = (float)sum_gy / mpu6050_calib.calib_samples;
            mpu6050_calib.gyro_offset_z = (float)sum_gz / mpu6050_calib.calib_samples;
            
            mpu6050_calib.is_calibrated = 1;
            calib_state = 2;  // 校准完成
        }
    }
    
    return (calib_state == 1);  // 返回1表示校准仍在进行中
}

/**
  * @brief  更新MPU6050姿态解算
  * @retval 无
  */
void MPU6050_Analysis_Update(void)
{
    // 1. 应用陀螺仪校准
    apply_gyro_calibration();
    
    // 2. 从加速度计计算姿态角
    calculate_attitude_from_accel();
    
    // 3. 陀螺仪数据积分
    integrate_gyro_data();
    
    // 4. 应用互补滤波融合数据
    apply_complementary_filter();
    
    // 5. 应用低通滤波平滑输出
    apply_low_pass_filter();
}

/**
  * @brief  获取当前姿态角
  * @retval 姿态角结构体指针
  */
const MPU6050_Attitude_t* MPU6050_GetAttitude(void)
{
    return &mpu6050_attitude;
}

// ==================== 私有函数实现 ====================

/**
  * @brief  应用死区处理
  * @param  value: 输入输出值
  * @param  threshold: 死区阈值
  * @retval 无
  */
static void apply_deadzone(float *value, float threshold)
{
    if (fabs(*value) < threshold) {
        *value = 0.0f;
    }
}

/**
  * @brief  应用陀螺仪校准
  * @retval 无
  */
static void apply_gyro_calibration(void)
{
    if (mpu6050_calib.is_calibrated) {
        // 减去零偏
        mpu6050_gyro_x -= (int16_t)mpu6050_calib.gyro_offset_x;
        mpu6050_gyro_y -= (int16_t)mpu6050_calib.gyro_offset_y;
        mpu6050_gyro_z -= (int16_t)mpu6050_calib.gyro_offset_z;
    } else {
        // 未校准时使用默认补偿值
        mpu6050_gyro_x += 12;
        mpu6050_gyro_y -= 6;
        mpu6050_gyro_z += 38;
    }
    
    // 转换为物理单位(dps)
    float gyro_x_dps = mpu6050_gyro_transition(mpu6050_gyro_x);
    float gyro_y_dps = mpu6050_gyro_transition(mpu6050_gyro_y);
    float gyro_z_dps = mpu6050_gyro_transition(mpu6050_gyro_z);
    
    // 应用死区
    apply_deadzone(&gyro_x_dps, mpu6050_filter_config.gyro_threshold);
    apply_deadzone(&gyro_y_dps, mpu6050_filter_config.gyro_threshold);
    apply_deadzone(&gyro_z_dps, mpu6050_filter_config.gyro_threshold);
    
    // 保存处理后的值（转换回原始单位）
    mpu6050_gyro_x = (int16_t)(gyro_x_dps * 131.0f);  // 根据量程转换
    mpu6050_gyro_y = (int16_t)(gyro_y_dps * 131.0f);
    mpu6050_gyro_z = (int16_t)(gyro_z_dps * 131.0f);
}

/**
  * @brief  从加速度计计算姿态角
  * @retval 无
  */
static void calculate_attitude_from_accel(void)
{
    // 转换为物理单位(g)
    float acc_x_g = mpu6050_acc_transition(mpu6050_acc_x);
    float acc_y_g = mpu6050_acc_transition(mpu6050_acc_y);
    float acc_z_g = mpu6050_acc_transition(mpu6050_acc_z);
    
    // 计算横滚角 (绕X轴旋转)
    mpu6050_attitude.roll_acc = atan2(acc_y_g, acc_z_g) * RAD_TO_DEG;
    
    // 计算俯仰角 (绕Y轴旋转)
    mpu6050_attitude.pitch_acc = -atan2(acc_x_g, acc_z_g) * RAD_TO_DEG;
    
    // 限制角度范围
    if (mpu6050_attitude.roll_acc > 180.0f) mpu6050_attitude.roll_acc -= 360.0f;
    if (mpu6050_attitude.roll_acc < -180.0f) mpu6050_attitude.roll_acc += 360.0f;
    if (mpu6050_attitude.pitch_acc > 180.0f) mpu6050_attitude.pitch_acc -= 360.0f;
    if (mpu6050_attitude.pitch_acc < -180.0f) mpu6050_attitude.pitch_acc += 360.0f;
}

/**
  * @brief  陀螺仪数据积分
  * @retval 无
  */
static void integrate_gyro_data(void)
{
    // 转换为dps
    float gyro_x_dps = mpu6050_gyro_transition(mpu6050_gyro_x);
    float gyro_y_dps = mpu6050_gyro_transition(mpu6050_gyro_y);
    float gyro_z_dps = mpu6050_gyro_transition(mpu6050_gyro_z);
    
    // 积分计算角度
    mpu6050_attitude.roll_gyro = mpu6050_attitude.roll + 
                                 gyro_x_dps * mpu6050_filter_config.sample_time;
    
    mpu6050_attitude.pitch_gyro = mpu6050_attitude.pitch + 
                                  gyro_y_dps * mpu6050_filter_config.sample_time;
    
    mpu6050_attitude.yaw_gyro = mpu6050_attitude.yaw + 
                                gyro_z_dps * mpu6050_filter_config.sample_time;
}

/**
  * @brief  应用互补滤波融合数据
  * @retval 无
  */
static void apply_complementary_filter(void)
{
    float alpha = mpu6050_filter_config.comp_filter_alpha;
    
    // 横滚角融合
    mpu6050_attitude.roll = alpha * mpu6050_attitude.roll_acc + 
                          (1 - alpha) * mpu6050_attitude.roll_gyro;
    
    // 俯仰角融合
    mpu6050_attitude.pitch = alpha * mpu6050_attitude.pitch_acc + 
                           (1 - alpha) * mpu6050_attitude.pitch_gyro;
    
    // 偏航角只使用陀螺仪积分（会漂移）
    mpu6050_attitude.yaw = mpu6050_attitude.yaw_gyro;
    
    // 限制角度范围
    if (mpu6050_attitude.roll > 180.0f) mpu6050_attitude.roll -= 360.0f;
    if (mpu6050_attitude.roll < -180.0f) mpu6050_attitude.roll += 360.0f;
    if (mpu6050_attitude.pitch > 180.0f) mpu6050_attitude.pitch -= 360.0f;
    if (mpu6050_attitude.pitch < -180.0f) mpu6050_attitude.pitch += 360.0f;
    if (mpu6050_attitude.yaw > 180.0f) mpu6050_attitude.yaw -= 360.0f;
    if (mpu6050_attitude.yaw < -180.0f) mpu6050_attitude.yaw += 360.0f;
}

/**
  * @brief  应用低通滤波平滑输出
  * @retval 无
  */
static void apply_low_pass_filter(void)
{
    static float roll_lpf = 0, pitch_lpf = 0, yaw_lpf = 0;
    
    float alpha = mpu6050_filter_config.lpf_alpha;
    
    // 低通滤波
    roll_lpf = alpha * mpu6050_attitude.roll + (1 - alpha) * roll_lpf;
    pitch_lpf = alpha * mpu6050_attitude.pitch + (1 - alpha) * pitch_lpf;
    yaw_lpf = alpha * mpu6050_attitude.yaw + (1 - alpha) * yaw_lpf;
    
    // 更新最终结果
    mpu6050_attitude.roll = roll_lpf;
    mpu6050_attitude.pitch = pitch_lpf;
    mpu6050_attitude.yaw = yaw_lpf;
}

// ==================== 兼容旧API的函数 ====================

// 旧API函数，保持向后兼容
void MPU6050_Analysis(void)
{
    MPU6050_Analysis_Update();
}

// 旧API全局变量
float Roll_Result = 0.0f;
float Pitch_Result = 0.0f;
float Yaw_Result = 0.0f;