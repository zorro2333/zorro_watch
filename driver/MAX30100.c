/*
Arduino-MAX30100 oximetry / heart rate integrated sensor library
Copyright (C) 2016  OXullo Intersecans <x@brainrapers.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//#include <Wire.h>

#include "MAX30100.h"
#include "MAX30100_Filters.h"
uint16_t rawIRValue;		//红外FIFO数据
uint16_t rawRedValue;		//红光FIFO数据
uint8_t PartID;					//设备ID

void MAX30100Init(void)
{

    Write_One_Byte(MAX30100_REG_MODE_CONFIGURATION, MAX30100_MC_RESET);


}

void max_begin()
{

    setMode(DEFAULT_MODE);								// HR_ONLY
    setLedsPulseWidth(DEFAULT_PULSE_WIDTH);//16bits
    setSamplingRate(DEFAULT_SAMPLING_RATE);//100HZ
    setLedsCurrent(DEFAULT_IR_LED_CURRENT, DEFAULT_RED_LED_CURRENT);//Ir=Red=50MA
    setHighresModeEnabled(1);
}

void setMode(Mode mode)
{

    Write_One_Byte(MAX30100_REG_MODE_CONFIGURATION, mode);
//	printf("%d\r\n",Write_One_Byte(MAX30100_REG_MODE_CONFIGURATION, mode));
}

void setLedsPulseWidth(LEDPulseWidth ledPulseWidth)
{


    uint8_t previous;  previous=Read_One_Byte(MAX30100_REG_SPO2_CONFIGURATION);
    Write_One_Byte(MAX30100_REG_SPO2_CONFIGURATION, (previous & 0xfc) | ledPulseWidth);
}

void setSamplingRate(SamplingRate samplingRate)
{

    uint8_t previous; previous=Read_One_Byte(MAX30100_REG_SPO2_CONFIGURATION);
    Write_One_Byte(MAX30100_REG_SPO2_CONFIGURATION, (previous & 0xe3) | (samplingRate << 2));

}

void setLedsCurrent(LEDCurrent irLedCurrent, LEDCurrent redLedCurrent)
{

    Write_One_Byte(MAX30100_REG_LED_CONFIGURATION, redLedCurrent << 4 | irLedCurrent);
}

void setHighresModeEnabled(u8 enabled)
{

    uint8_t previous;
    previous=Read_One_Byte(MAX30100_REG_SPO2_CONFIGURATION);
    if (enabled) {
        Write_One_Byte(MAX30100_REG_SPO2_CONFIGURATION, previous | MAX30100_SPC_SPO2_HI_RES_EN);
    } else {
        Write_One_Byte(MAX30100_REG_SPO2_CONFIGURATION, previous & ~MAX30100_SPC_SPO2_HI_RES_EN);
    }
}

uint8_t buffer[4];

void update()
{
    readFifoData();
}

void readFifoData()
{


    Buff_Read(MAX30100_REG_FIFO_DATA,buffer, 4);
    //printf("buff:%d %d\r\n",rawIRValue,rawRedValue);
}

u8 Write_One_Byte(u8 addr,u8 data)
{
    return HAL_I2C_Mem_Write(&hi2c2,0xAE,addr,I2C_MEMADD_SIZE_8BIT,&data, 1, 1000);
}
u8 Read_One_Byte(u8 addr)
{
    u8 res;
    HAL_I2C_Mem_Read(&hi2c2,0xAF,addr,I2C_MEMADD_SIZE_8BIT,&res, 1, 1000);
    return res;
}

u8 Buff_Read(u8 address, u8 *buf, u8 len)
{
    HAL_StatusTypeDef status;
    // 发送设备地址和寄存器地址
    status = HAL_I2C_Mem_Write(&hi2c2, 0xAE, address, I2C_MEMADD_SIZE_8BIT, &address, 1, HAL_MAX_DELAY);
    if (status != HAL_OK)
        return 1; // 写入失败

    // 从寄存器读取数据
    status = HAL_I2C_Mem_Read(&hi2c2, 0xAF, address, I2C_MEMADD_SIZE_8BIT, buf, len, HAL_MAX_DELAY);
    //printf("buff:%d %d\r\n",(buffer[0] << 8) | buffer[1],(buffer[2] << 8) | buffer[3]);
    rawIRValue = (buf[0] << 8) | buf[1];
    rawRedValue = (buf[2] << 8) | buf[3];
    if (status != HAL_OK)
        return 1; // 读取失败

    return 0; // 读取成功
}
