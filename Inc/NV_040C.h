#ifndef _NV_040C_H_
#define _NV_040C_H_

#include "stm32f1xx_hal.h"

#define DATA_PIN_RESET() HAL_GPIO_WritePin(V_Data_GPIO_Port, V_Data_Pin, GPIO_PIN_RESET)
#define DATA_PIN_SET()   HAL_GPIO_WritePin(V_Data_GPIO_Port, V_Data_Pin, GPIO_PIN_SET)

//Test Data 
//#define DATA_PIN_RESET() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_RESET)
//#define DATA_PIN_SET() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_SET)
void SoundPlay(uint32_t voice_addr);

#endif
