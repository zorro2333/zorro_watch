/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 *
 * 文件名称：beep.h
 * 摘要：
 *
 *--------------------------------------------------
*/

#ifndef BEEP_H_
#define BEEP_H_

/**********************************************************************
 * 函数名称： buzzer_init
 * 功能描述： 初始化蜂鸣器并创建定时器(用于停止声音)
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
int buzzer_init(void);

/**********************************************************************
 * 函数名称： buzzer_buzz
 * 功能描述： 启动蜂鸣器发出一段声音
 * 输入参数： freq - 声音频率
 *            time_ms - 持续时间
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
void buzzer_buzz(int freq, int time_ms);


#endif /* BEEP_H_ */