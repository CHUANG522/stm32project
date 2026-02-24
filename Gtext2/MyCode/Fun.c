#include "Head.h" 

void LED_Show(uint8_t LED,uint8_t mode)
{
	
	if(mode ==1)
	HAL_GPIO_WritePin( GPIOC,GPIO_PIN_8  <<(LED -1),GPIO_PIN_RESET);
	else 
	HAL_GPIO_WritePin( GPIOC,GPIO_PIN_8 <<(LED -1),GPIO_PIN_SET);	
}


