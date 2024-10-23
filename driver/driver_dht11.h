
#ifndef _DRIVER_DHT11_H
#define _DRIVER_DHT11_H


 /**
  * @brief DHT11的初始化函数
  */
void DHT11_Init(void);

 /**
  * @brief 读取DHT11的温度/湿度
  * @param hum  - 用于保存湿度值       temp - 用于保存温度值
  * @return  0 - 成功, (-1) - 失败
  */
int DHT11_Read(int *hum, int *temp);


 /**
  * @brief DHT11测试程序
  */
void DHT11_Test(void);


#endif /* _DRIVER_DHT11_H */

