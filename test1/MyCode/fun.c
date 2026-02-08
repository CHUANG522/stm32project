#include "headfile.h"





void led_show(uint8_t led ,uint8_t mode )
{
	uint16_t pin ;//数组映射法
	const uint16_t pin_map[] = {	GPIO_PIN_5 ,GPIO_PIN_6,GPIO_PIN_7 	};
	
	if(led >=5 && led<=7)   pin = pin_map[led -5];
	else  return ;
	
	if(mode)
    HAL_GPIO_WritePin(GPIOA,pin,GPIO_PIN_SET);
	else
	HAL_GPIO_WritePin(GPIOA,pin,GPIO_PIN_RESET);
	
	/*位运算实现任意led亮灭
	if(mode)
    HAL_GPIO_WritePin(GPIOA,(1 << led),GPIO_PIN_SET);
	else
	HAL_GPIO_WritePin(GPIOA,(1 << led),GPIO_PIN_RESET);*/
	
}

uint16_t B1_state ;//按键1当前状态
uint16_t B1_last_state =1; //按键1上一次的状态
uint16_t B2_state ;
uint16_t B2_last_state =1;
void key_scan(void)
{
	B1_state = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2) ;
	
	if( B1_state == 0 && B1_last_state == 1)  led_show(5,1);
	//条件，按键按下时当前为0，那么上一次为1
	B1_last_state = B1_state ; 
	//使用结束将当前状态保留
	B2_state = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4 ) ;
	
	if( B2_state == 1 && B2_last_state == 0)  led_show(5,0);
	
	B2_last_state = B2_state ;
}

