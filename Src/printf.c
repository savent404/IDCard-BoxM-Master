#include "stm32f1xx_hal.h"

extern UART_HandleTypeDef huart1;

int stdin_getchar (void) {
	uint8_t buf = 0;
	
	HAL_GPIO_WritePin(RS485_RD_GPIO_Port, RS485_RD_Pin, GPIO_PIN_RESET);
	
	while (HAL_UART_Receive(&huart1, &buf, 1, 10) != HAL_OK);
	
	return buf;
}
int stdout_putchar (int ch) {
	while (HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 10) != HAL_OK);
	
	return ch;
}
