#include <stdio.h>
#include "MAX30100_PulseOximeter.h"
#include "stdlib.h"
#include "math.h"
float onBeatDetected;

PulseOximeterState state;
PulseOximeterDebuggingMode debuggingMode;
uint32_t tsFirstBeatDetected;
uint32_t tsLastBeatDetected;
uint32_t tsLastSample;
uint32_t tsLastBiasCheck;
uint32_t tsLastCurrentAdjustment;
uint8_t redLedPower;

DCRemover irDCRemover;
DCRemover redDCRemover;

u8  ID=0;
u8  TempINT=0;
u8  TempFRAC=0;

void SPO2_Init()
{
    ID=Read_One_Byte(MAX30100_REG_PART_ID);//??ID
    if(ID==17)
    {
        HAL_Delay(100);
        Write_One_Byte(MAX30100_REG_MODE_CONFIGURATION, MAX30100_MC_TEMP_EN);//??????
        TempINT=Read_One_Byte(MAX30100_REG_TEMPERATURE_DATA_INT);						 //??????
        TempFRAC=Read_One_Byte(MAX30100_REG_TEMPERATURE_DATA_FRAC);				   //??????
        HAL_Delay(300);
        setMode(MAX30100_MODE_SPO2_HR);																//SPO2 HR??
        setLedsPulseWidth(MAX30100_SPC_PW_1600US_16BITS);							//16bit AD
        setSamplingRate(MAX30100_SAMPRATE_100HZ);											//???? 100HZ
        setLedsCurrent( MAX30100_LED_CURR_24MA,MAX30100_LED_CURR_24MA);//24MA 24MA(??)
        setHighresModeEnabled(1);//????????
        BeatDetector();
        setOnBeatDetectedCallback(&onBeatDetected);
        begin(PULSEOXIMETER_DEBUGGINGMODE_PULSEDETECT);								//????
    }

}


void DisplayChar(unsigned char c)
{

    while((USART1->SR&0X40)==0);//等待上一次发送完毕
    USART1->DR=c;
}

void DisplayCurve(float ch0,float ch1)
{
    signed short temp;
    DisplayChar(0x03);
    DisplayChar(0xfc);

    temp=(signed short)ch0;
    DisplayChar(temp&0x00ff);
    DisplayChar(temp>>8);
    temp=(signed short)ch1;
    DisplayChar(temp&0x00ff);
    DisplayChar(temp>>8);


    DisplayChar(0xfc);
    DisplayChar(0x03);
}


void PulseOximeter()
{
    state=PULSEOXIMETER_STATE_INIT;
    tsFirstBeatDetected=0;
    tsLastBeatDetected=0;
    tsLastSample=0;
    tsLastBiasCheck=0;
    tsLastCurrentAdjustment=0;
    redLedPower=((uint8_t)RED_LED_CURRENT_START);
    onBeatDetected=0;
}

void begin(PulseOximeterDebuggingMode mode)
{
    debuggingMode = mode;

    PulseOximeter();
    //消除Ir  Red 直流分量
    //DCRemover1(DC_REMOVER_ALPHA,&irDCRemover.alpha,&irDCRemover.dcw);
    //DCRemover1(DC_REMOVER_ALPHA,&redDCRemover.alpha,&redDCRemover.dcw);

    state = PULSEOXIMETER_STATE_IDLE;//闲置状态
}

void POupdate()
{
    checkSample();
    // checkCurrentBias();
}

float getHeartRate()
{
    return getRate();
}

uint8_t POgetSpO2()
{
    return getSpO2();
}

uint8_t getRedLedCurrentBias()
{
    return redLedPower;
}

void setOnBeatDetectedCallback(float *cb)
{
    onBeatDetected = *cb;
}

//直流分量滤波
#define TRUE_RESISTANCE 93.16 // 校准时的电阻值(kOhm)
#define MEASURE_RESISTANCE 7.6 // MAX30100上的测量电阻(kOhm)

float calculateSpO2(uint32_t irValue, uint32_t redValue) {
    float ratio;
    float spo2;
    // 根据实际情况进行校准
    float correctionFactor;
    spo2 = spo2 * correctionFactor;
    ratio = (float)irValue / (float)redValue; // 红外和红光比值
    spo2 = 104.0 - 17.0 * ratio; // 估计血氧值
    correctionFactor = TRUE_RESISTANCE / MEASURE_RESISTANCE;
    // 限制血氧值范围在0-100%
    if (spo2 > 100.0) {
        spo2 = 100.0;
    } else if (spo2 < 0.0) {
        spo2 = 0.0;
    }

    return spo2;
}

static uint32_t prevIrValue = 0;
static uint32_t prevRedValue = 0;
/**
 * @brief 去除红外数据的直流分量
 * @param irValue 原始红外数据
 * @return 去除直流分量后的数据
 */
uint32_t removeIrDcComponent(uint32_t irValue) {
    uint32_t filtered;
    filtered = irValue - prevIrValue;
    prevIrValue = irValue;
    return filtered;
}

/**
 * @brief 去除红光数据的直流分量
 * @param redValue 原始红光数据
 * @return 去除直流分量后的数据
 */
uint32_t removeRedDcComponent(uint32_t redValue) {
    uint32_t filtered;
    filtered = redValue - prevRedValue;
    prevRedValue = redValue;
    return filtered;
}
void checkSample()
{

    float Sp;
    uint32_t filteredIrValue;
    uint32_t filteredRedValue;
    update();
    filteredIrValue = removeIrDcComponent(rawIRValue);
    filteredRedValue = removeRedDcComponent(rawRedValue);
    Sp = calculateSpO2(rawIRValue,rawRedValue);
    printf("SpO2:%d\r\n",filteredIrValue);
}



void onBeatDetected1(void)
{

}
void checkCurrentBias()
{


}


