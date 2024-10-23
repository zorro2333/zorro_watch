#include "MPU6050.h"

/*
@brief 写入MPU6050寄存器数据
@param mpu6050: MPU6050设备结构体指针
@param regAddr: 要写入的寄存器地址
@param data: 要写入的数据
@retval 返回HAL库操作状态
*/
static HAL_StatusTypeDef MPU6050_WriteReg(MPU6050_t* mpu6050, uint8_t regAddr, uint8_t data)
{
    return HAL_I2C_Mem_Write(mpu6050->hi2c, mpu6050->devAddr, regAddr, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000);
}
/*
@brief 读取MPU6050寄存器数据
@param mpu6050: MPU6050设备结构体指针
@param regAddr: 要读取的寄存器地址
@param pData: 存储读取数据的缓冲区指针
@param size: 要读取的数据长度（字节）
@retval 返回HAL库操作状态
*/
static HAL_StatusTypeDef MPU6050_ReadReg(MPU6050_t* mpu6050, uint8_t regAddr, uint8_t* pData, uint16_t size)
{
    return HAL_I2C_Mem_Read(mpu6050->hi2c, mpu6050->devAddr, regAddr, I2C_MEMADD_SIZE_8BIT, pData, size, 1000);
}
/*
@brief  初始化MPU6050设备

@param  mpu6050: MPU6050设备结构体指针

@param  hi2c: I2C句柄
*/
void MPU6050_Init(MPU6050_t* mpu6050, I2C_HandleTypeDef *hi2c)
{
    uint8_t whoami = 0;

    /* 复位设备 */
    MPU6050_WriteReg(mpu6050, MPU6050_PWR_MGMT_1, 0x80);
    HAL_Delay(100);     // 等待100ms

    /* 唤醒设备 */
    MPU6050_WriteReg(mpu6050, MPU6050_PWR_MGMT_1, 0x00);

    /* 设置采样率 */
    MPU6050_WriteReg(mpu6050, MPU6050_SMPLRT_DIV, 0x00); // 1kHz采样率

    /* 设置数字低通滤波器截止频率为44Hz */
    MPU6050_WriteReg(mpu6050, MPU6050_CONFIG, 0x03);

    /* 设置加速度计灵敏度 */
    MPU6050_SetAccelFS(mpu6050, ACCEL_FS_SEL_2G);

    /* 设置陀螺仪灵敏度 */
    MPU6050_SetGyroFS(mpu6050, GYRO_FS_SEL_250DPS);

    /* 开启FIFO缓存 */
    MPU6050_WriteReg(mpu6050, 0x23, 0x00);

    /* 关闭自检 */
    MPU6050_WriteReg(mpu6050, 0x6B, 0x00);

    /* 检查设备ID */
    MPU6050_ReadReg(mpu6050,MPU6050_WHO_AM_I, &whoami, 1);
    if (whoami != 0x68) {
        mpu6050->initialized = 0;
        return;
    }

    /* 设备初始化完成 */
    mpu6050->initialized = 1;
}
/*
@brief  设置数字低通滤波器

@param  mpu6050: MPU6050设备结构体指针
        默认禁用外部同步信号
@param  lpf_cutoff_freq: 数字低通滤波器截止频率配置
*/
void MPU6050_SetLPF(MPU6050_t* mpu6050, MPU6050_LPF_CUTOFF_FREQ_t lpf_cutoff_freq)
{
    uint8_t data;

    switch (lpf_cutoff_freq)
    {
        case LPF_CUTOFF_FREQ_5HZ:
            data = 0x06;
            break;

        case LPF_CUTOFF_FREQ_10HZ:
            data = 0x05;
            break;

        case LPF_CUTOFF_FREQ_21HZ:
            data = 0x04;
            break;

        case LPF_CUTOFF_FREQ_44HZ:
            data = 0x03;
            break;

        case LPF_CUTOFF_FREQ_94HZ:
            data = 0x02;
            break;

        case LPF_CUTOFF_FREQ_184HZ:
            data = 0x01;
            break;

        case LPF_CUTOFF_FREQ_260HZ:
            data = 0x00;
            break;

        case LPF_CUTOFF_FREQ_3600HZ:
            data = 0x07;
            break;

        default:
            return;
    }

    MPU6050_WriteReg(mpu6050, MPU6050_CONFIG, data);
    mpu6050->lpf_cutoff_freq = lpf_cutoff_freq;
}

/*
@brief  设置采样率

@param  mpu6050: MPU6050设备结构体指针

@param  sample_rate: 采样率配置
*/
void MPU6050_SetSampleRate(MPU6050_t* mpu6050, MPU6050_SAMPLE_RATE_t sample_rate)
{
    uint8_t data;

    switch (sample_rate)
    {
        case SAMPLE_RATE_DIV0:  //0
            data = 0x00;
            break;

        case SAMPLE_RATE_DIV2:  //2
            data = 0x01;
            break;

        case SAMPLE_RATE_DIV4:  //4
            data = 0x03;
            break;

        case SAMPLE_RATE_DIV8:  //8
            data = 0x07;
            break;

        case SAMPLE_RATE_DIV16: //16
            data = 0x0F;
            break;

        case SAMPLE_RATE_DIV32: //32
            data = 0x1F;
            break;

        case SAMPLE_RATE_DIV64: //64
            data = 0x3F;
            break;

        case SAMPLE_RATE_DIV128: //128
            data = 0x7F;
            break;

        default:
            return;
    }

    MPU6050_WriteReg(mpu6050, MPU6050_SMPLRT_DIV, data);
    mpu6050->sample_rate = sample_rate;
}
/*
@brief  设置加速度计灵敏度和高通滤波器
        默认关闭高通滤波器和自检

@param  mpu6050: MPU6050设备结构体指针

@param  accel_fs_sel: 加速度计量程配置
*/
void MPU6050_SetAccelFS(MPU6050_t* mpu6050, MPU6050_ACCEL_FS_SEL_t accel_fs_sel)
{
    uint8_t data;

    switch (accel_fs_sel)
    {
        case ACCEL_FS_SEL_2G:
            data = 0x00;
            break;

        case ACCEL_FS_SEL_4G:
            data = 0x08;
            break;

        case ACCEL_FS_SEL_8G:
            data = 0x10;
            break;

        case ACCEL_FS_SEL_16G:
            data = 0x18;
            break;

        default:
            return;
    }

    MPU6050_WriteReg(mpu6050, MPU6050_ACCEL_CONFIG, data);
    mpu6050->accel_fs_sel = accel_fs_sel;
}

/*

@brief  设置陀螺仪灵敏度

@param  mpu6050: MPU6050设备结构体指针

@param  gyro_fs_sel: 陀螺仪量程配置
*/
void MPU6050_SetGyroFS(MPU6050_t* mpu6050, MPU6050_GYRO_FS_SEL_t gyro_fs_sel)
{
    uint8_t data;

    switch (gyro_fs_sel)
    {
        case GYRO_FS_SEL_250DPS:
            data = 0x00;
            break;

        case GYRO_FS_SEL_500DPS:
            data = 0x08;
            break;

        case GYRO_FS_SEL_1000DPS:
            data = 0x10;
            break;

        case GYRO_FS_SEL_2000DPS:
            data = 0x18;
            break;

        default:
            return;
    }

    MPU6050_WriteReg(mpu6050, MPU6050_GYRO_CONFIG, data);
    mpu6050->gyro_fs_sel = gyro_fs_sel;
}

/*

@brief  读取加速度计值

@param  mpu6050: MPU6050设备结构体指针

@param  ax: 存储X轴加速度值的指针

@param  ay: 存储Y轴加速度值的指针

@param  az: 存储Z轴加速度值的指针
*/
void MPU6050_ReadAccel(MPU6050_t* mpu6050, float* ax, float* ay, float* az)
{
    uint8_t buf[6];
    int16_t rawAx, rawAy, rawAz;

    MPU6050_ReadReg(mpu6050, MPU6050_ACCEL_XOUT_H, buf, 6);

    /* 将原始数据转换为g值 */
    rawAx = ((int16_t)buf[0] << 8) | buf[1];
    rawAy = ((int16_t)buf[2] << 8) | buf[3];
    rawAz = ((int16_t)buf[4] << 8) | buf[5];

    switch (mpu6050->accel_fs_sel)
    {
        case ACCEL_FS_SEL_2G:
            *ax = (float)rawAx / 16384.0;
            *ay = (float)rawAy / 16384.0;
            *az = (float)rawAz / 16384.0;
            break;

        case ACCEL_FS_SEL_4G:
            *ax = (float)rawAx / 8192.0;
            *ay = (float)rawAy / 8192.0;
            *az = (float)rawAz / 8192.0;
            break;

        case ACCEL_FS_SEL_8G:
            *ax = (float)rawAx / 4096.0;
            *ay = (float)rawAy / 4096.0;
            *az = (float)rawAz / 4096.0;
            break;

        case ACCEL_FS_SEL_16G:
            *ax = (float)rawAx / 2048.0;
            *ay = (float)rawAy / 2048.0;
            *az = (float)rawAz / 2048.0;
            break;

        default:
            break;
    }
}

/*

@brief  读取陀螺仪值

@param  mpu6050: MPU6050设备结构体指针

@param  gx: 存储X轴角速度值的指针

@param  gy: 存储Y轴角速度值的指针

@param  gz: 存储Z轴角速度值的指针
*/
void MPU6050_ReadGyro(MPU6050_t* mpu6050, float* gx, float* gy, float* gz)
{
    uint8_t buf[6];
    int16_t rawGx, rawGy, rawGz;

    MPU6050_ReadReg(mpu6050, MPU6050_GYRO_XOUT_H, buf, 6);

    /* 将原始数据转换为度每秒 */
    rawGx = ((int16_t)buf[0] << 8) | buf[1];
    rawGy = ((int16_t)buf[2] << 8) | buf[3];
    rawGz = ((int16_t)buf[4] << 8) | buf[5];

    switch (mpu6050->gyro_fs_sel)
    {
        case GYRO_FS_SEL_250DPS:
            *gx = (float)rawGx / 131.0;
            *gy = (float)rawGy / 131.0;
            *gz = (float)rawGz / 131.0;
            break;

        case GYRO_FS_SEL_500DPS:
            *gx = (float)rawGx / 65.5;
            *gy = (float)rawGy / 65.5;
            *gz = (float)rawGz / 65.5;
            break;

        case GYRO_FS_SEL_1000DPS:
            *gx = (float)rawGx / 32.8;
            *gy = (float)rawGy / 32.8;
            *gz = (float)rawGz / 32.8;
            break;

        case GYRO_FS_SEL_2000DPS:
            *gx = (float)rawGx / 16.4;
            *gy = (float)rawGy / 16.4;
            *gz = (float)rawGz / 16.4;
            break;

        default:
            break;
    }
}
/*

@brief  读取温度值

@param  mpu6050: MPU6050设备结构体指针

@retval 返回温度值（摄氏度）
*/
float MPU6050_ReadTemp(MPU6050_t* mpu6050)
{
    uint8_t buf[2];
    int16_t rawTemp;

    MPU6050_ReadReg(mpu6050, MPU6050_TEMP_OUT_H, buf, 2);

    /* 将原始数据转换为摄氏度 */
    rawTemp = ((int16_t)buf[0] << 8) | buf[1];
    return (float)rawTemp / 340.0 + 36.53;
}