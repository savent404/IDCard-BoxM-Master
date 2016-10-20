#include "Box_BSP.h"

void BSP_Box_Init(void) {
	GPIO_InitTypeDef GPIOx;
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	
	GPIOx.Pin = GPIO_PIN_All;
	GPIOx.Mode = GPIO_MODE_INPUT;
	GPIOx.Pull = GPIO_PULLUP;
	GPIOx.Speed = GPIO_SPEED_FREQ_LOW;
	
	/* L_STA_channel */
	HAL_GPIO_Init(GPIO_Port_L_L, &GPIOx);
	HAL_GPIO_Init(GPIO_Port_L_H, &GPIOx);
	
	/* IF_STA_channel */
	HAL_GPIO_Init(GPIO_Port_IF_L, &GPIOx);
	HAL_GPIO_Init(GPIO_Port_IF_H, &GPIOx);
	
	/* CTR init */
	GPIOx.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_WritePin(GPIO_Port_CTL_L, GPIO_PIN_All, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIO_Port_CTL_H, GPIO_PIN_All, GPIO_PIN_RESET);

	HAL_GPIO_Init(GPIO_Port_CTL_L, &GPIOx);
	HAL_GPIO_Init(GPIO_Port_CTL_H, &GPIOx);
	
	/* Box_Num init */
	GPIOx.Mode = GPIO_MODE_INPUT;
	GPIOx.Pull =  GPIO_PULLUP;
	GPIOx.Pin = (GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6);
	HAL_GPIO_Init(GPIOA, &GPIOx);
}

/**
  * @Etc: BSP_Box_Ctl(Box_ID(channel)); channel 0~31
  * @Retvl: @1 - "有箱子未关闭"
 */
uint8_t BSP_Box_Ctl(uint32_t box_id) {
	
	uint32_t buff = box_id;
	
	/* channel 0~15 */
	HAL_GPIO_WritePin(GPIO_Port_CTL_L, box_id, GPIO_PIN_SET);
	/* channel 16~31*/
	HAL_GPIO_WritePin(GPIO_Port_CTL_H, box_id>>16, GPIO_PIN_SET);
	
	
	HAL_Delay(500);
	
	HAL_GPIO_WritePin(GPIO_Port_CTL_L, GPIO_PIN_All, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIO_Port_CTL_H, GPIO_PIN_All, GPIO_PIN_RESET);
	
	
	if (buff & BSP_Box_Check_L()) {
		return 1;
	}
	return 0;
}

/**
  * @Retvl: \box is opened @0
            \box is closed @1
  */
uint32_t BSP_Box_Check_L(void) {
	
	uint32_t buff = 0;
	
//	buff = HAL_GPIO_ReadPin(GPIO_Port_L_L, GPIO_PIN_All);
//	buff |= HAL_GPIO_ReadPin(GPIO_Port_L_H, GPIO_PIN_All) << 16;
	buff = GPIO_Port_L_L->IDR;
	buff |= GPIO_Port_L_H->IDR << 16;
	
	//取反
	//buff = ~buff;
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET) {
		buff = ~buff;
	}
	return buff;
}

/**
  * @Retvl: \box is empty @1
            \box is full  @0
  */
uint32_t BSP_Box_Check_IF(void) {
	uint32_t buff = 0;
	
	HAL_GPIO_WritePin(IF_EN_GPIO_Port, IF_EN_Pin, GPIO_PIN_SET);
	
	HAL_Delay(1);
	
//	buff = HAL_GPIO_ReadPin(GPIO_Port_IF_L, GPIO_PIN_All);
//	buff |= HAL_GPIO_ReadPin(GPIO_Port_IF_H, GPIO_PIN_All);
	buff = GPIO_Port_IF_L->IDR;
	buff |= GPIO_Port_IF_H->IDR << 16;
	
	HAL_GPIO_WritePin(IF_EN_GPIO_Port, IF_EN_Pin, GPIO_PIN_RESET);
	
	return buff;
}
