#include "Head.h" 
int counter=0;
uint8_t  LED_Mode=0;

void LED_Show(uint8_t LED,uint8_t mode)
{ //实现任意LED开灭
	 
	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);//开🔒将指定LED点亮或熄灭	
	
	
	if(mode ==1)
	HAL_GPIO_WritePin( GPIOC,GPIO_PIN_8  <<(LED -1),GPIO_PIN_RESET);
	else 
	HAL_GPIO_WritePin( GPIOC,GPIO_PIN_8 <<(LED -1),GPIO_PIN_SET);	
	
		
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);//关🔒
}

uint8_t Key_B1 ;
uint8_t Key_B1_Last ;
uint8_t Key_B2 ;
uint8_t Key_B2_Last ;
void Key_Scan(void)
{
  Key_B1 =  HAL_GPIO_ReadPin (GPIOB,GPIO_PIN_0);
	Key_B2 =  HAL_GPIO_ReadPin (GPIOB,GPIO_PIN_1);
	//if判断条件的原理：按键按下则此时是地电平，其上一次一定是高电平
  if(Key_B1 == 0&& Key_B1_Last ==1)  counter++;
	if(Key_B2 == 0&& Key_B2_Last ==1) counter--;
	
	
	
	
	Key_B1_Last =Key_B1; //得到key的上一个状态
	Key_B2_Last =Key_B2;
}

char string[20];

void LCD_Show(void)
{ 
	sprintf(string, "        text       ");
	LCD_DisplayStringLine ( Line0 , (uint8_t *)string);
	
	sprintf(string ,"     counter:%d    ",counter);
	LCD_DisplayStringLine ( Line1 , (uint8_t *)string);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim -> Instance == TIM2)
  {
  LED_Mode ++;
  LED_Mode %=2;  
  
  }
 



}



