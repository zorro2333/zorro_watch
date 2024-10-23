#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "main.h"
#include "i2c.h"




/* MPU6050宏定义 */
#define MPU6050_ADDR        0xD0    // MPU6050地址
#define MPU6050_SMPLRT_DIV  0x19    // 采样率分频器寄存器地址
#define MPU6050_CONFIG      0x1A    // 配置寄存器地址，用来配置陀螺仪和加速度计的数字低通滤波器截止频率
#define MPU6050_GYRO_CONFIG 0x1B    // 陀螺仪配置寄存器地址
#define MPU6050_ACCEL_CONFIG 0x1C   // 加速度计配置寄存器地址
#define MPU6050_ACCEL_XOUT_H 0x3B   // X轴加速度值高位寄存器地址
#define MPU6050_ACCEL_XOUT_L 0x3C   // X轴加速度值低位寄存器地址
#define MPU6050_ACCEL_YOUT_H 0x3D   // Y轴加速度值高位寄存器地址
#define MPU6050_ACCEL_YOUT_L 0x3E   // Y轴加速度值低位寄存器地址
#define MPU6050_ACCEL_ZOUT_H 0x3F   // Z轴加速度值高位寄存器地址
#define MPU6050_ACCEL_ZOUT_L 0x40   // Z轴加速度值低位寄存器地址
#define MPU6050_TEMP_OUT_H  0x41    // 温度值高位寄存器地址
#define MPU6050_TEMP_OUT_L  0x42    // 温度值低位寄存器地址
#define MPU6050_GYRO_XOUT_H  0x43   // X轴陀螺仪角速度值高位寄存器地址
#define MPU6050_GYRO_XOUT_L  0x44   // X轴陀螺仪角速度值低位寄存器地址
#define MPU6050_GYRO_YOUT_H  0x45   // Y轴陀螺仪角速度值高位寄存器地址
#define MPU6050_GYRO_YOUT_L  0x46   // Y轴陀螺仪角速度值低位寄存器地址
#define MPU6050_GYRO_ZOUT_H  0x47   // Z轴陀螺仪角速度值高位寄存器地址
#define MPU6050_GYRO_ZOUT_L  0x48   // Z轴陀螺仪角速度值低位寄存器地址
#define MPU6050_PWR_MGMT_1   0x6B    // 电源管理寄存器1地址
#define MPU6050_WHO_AM_I     0x75    // ID寄存器地址


/* 数字低通滤波器截止频率配置 */
typedef enum {
    LPF_CUTOFF_FREQ_5HZ = 0,    //截止频率5HZ   初始采样率1KHZ
    LPF_CUTOFF_FREQ_10HZ,       //截止频率10HZ   初始采样率1KHZ
    LPF_CUTOFF_FREQ_21HZ,       //截止频率21HZ   初始采样率1KHZ
    LPF_CUTOFF_FREQ_44HZ,       //截止频率44HZ   初始采样率1KHZ
    LPF_CUTOFF_FREQ_94HZ,       //截止频率94HZ   初始采样率1KHZ
    LPF_CUTOFF_FREQ_184HZ,      //截止频率184HZ   初始采样率1KHZ
    LPF_CUTOFF_FREQ_260HZ,      //截止频率260HZ   初始采样率8KHZ
    LPF_CUTOFF_FREQ_3600HZ      //禁用低通滤波器   初始采样率8KHZ
} MPU6050_LPF_CUTOFF_FREQ_t;

/* 采样率配置 */
typedef enum {
    SAMPLE_RATE_DIV0 = 0,       //0分频
    SAMPLE_RATE_DIV2,           //2分频
    SAMPLE_RATE_DIV4,           //4分频
    SAMPLE_RATE_DIV8,           //8分频
    SAMPLE_RATE_DIV16,          //16分频
    SAMPLE_RATE_DIV32,          //32分频
    SAMPLE_RATE_DIV64,          //64分频
    SAMPLE_RATE_DIV128          //128分频
} MPU6050_SAMPLE_RATE_t;

/* 加速度计灵敏度配置 */
typedef enum {
    ACCEL_FS_SEL_2G = 0,    // ±2g
    ACCEL_FS_SEL_4G,        // ±4g
    ACCEL_FS_SEL_8G,        // ±8g
    ACCEL_FS_SEL_16G        // ±16g
} MPU6050_ACCEL_FS_SEL_t;

/* 陀螺仪灵敏度配置 */
typedef enum {
    GYRO_FS_SEL_250DPS = 0,  // ±250dps
    GYRO_FS_SEL_500DPS,      // ±500dps
    GYRO_FS_SEL_1000DPS,     // ±1000dps
    GYRO_FS_SEL_2000DPS      // ±2000dps
} MPU6050_GYRO_FS_SEL_t;

/* MPU6050设备结构体 */
typedef struct {
    I2C_HandleTypeDef *hi2c;                    // I2C句柄
    uint8_t devAddr;                            // 设备地址
    MPU6050_LPF_CUTOFF_FREQ_t lpf_cutoff_freq;  // 数字低通滤波器截止频率
    MPU6050_SAMPLE_RATE_t sample_rate;          // 采样率
    MPU6050_ACCEL_FS_SEL_t accel_fs_sel;        // 加速度计灵敏度
    MPU6050_GYRO_FS_SEL_t gyro_fs_sel;          // 陀螺仪灵敏度
    uint8_t initialized;                        // 设备是否已初始化
} MPU6050_t;
/* 函数原型 */
void MPU6050_Init(MPU6050_t* mpu6050, I2C_HandleTypeDef* hi2c);
void MPU6050_SetLPF(MPU6050_t* mpu6050, MPU6050_LPF_CUTOFF_FREQ_t lpf_cutoff_freq);
void MPU6050_SetSampleRate(MPU6050_t* mpu6050, MPU6050_SAMPLE_RATE_t sample_rate);
void MPU6050_SetAccelFS(MPU6050_t* mpu6050, MPU6050_ACCEL_FS_SEL_t accel_fs_sel);
void MPU6050_SetGyroFS(MPU6050_t* mpu6050, MPU6050_GYRO_FS_SEL_t gyro_fs_sel);
void MPU6050_ReadAccel(MPU6050_t* mpu6050, float* ax, float* ay, float* az);
void MPU6050_ReadGyro(MPU6050_t* mpu6050, float* gx, float* gy, float* gz);
float MPU6050_ReadTemp(MPU6050_t* mpu6050);
#endif /* MPU6050_H */
