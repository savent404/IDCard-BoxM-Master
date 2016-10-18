#ifndef _BOX_BSP_H_
#define _BOX_BSP_H_

#include "stm32f1xx_hal.h"

#define Box_ID(x) (0x0001 << x)

#define GPIO_Port_CTL_L GPIOD
#define GPIO_Port_CTL_H GPIOE
#define GPIO_Port_L_L   GPIOB
#define GPIO_Port_L_H   GPIOC
#define GPIO_Port_IF_L  GPIOF
#define GPIO_Port_IF_H  GPIOG

void BSP_Box_Init(void);
uint8_t  BSP_Box_Ctl(uint32_t box_id);
uint32_t BSP_Box_Check_L(void);
uint32_t BSP_Box_Check_IF(void);

#endif

