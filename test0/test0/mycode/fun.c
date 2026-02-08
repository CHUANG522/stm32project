#include "headfile.h"


Key_TypeDef keys[] = 
{
    {GPIOB, GPIO_PIN_0, 1, 1, 1},  // B0
    {GPIOB, GPIO_PIN_1, 1, 1, 2},  // B1
    {GPIOB, GPIO_PIN_2, 1, 1, 3},  // B2
    {GPIOA, GPIO_PIN_0, 1, 1, 4}   // A0
};

#define KEY_NUM (sizeof(keys) / sizeof(Key_TypeDef))
uint8_t key_flags[KEY_NUM] = {0};  
int count=0;
uint8_t led8_flag=0;
uint8_t lcd_high;
uint32_t tim17_cnt,fre1;               //tim17 use for PWM_fre celiang
uint32_t tim16_cnt,fre2;
uint32_t tim8_cnt,fre3;
/*********************************************** LED************************************************/
void led_show(uint8_t led ,uint8_t mode)
{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);	
	if(mode)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8<<(led-1),GPIO_PIN_RESET);	
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8<<(led-1),GPIO_PIN_SET);	
	}	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}

void key_init() 
{
    for (uint8_t i = 0; i < KEY_NUM; i++) 
	{
        keys[i].last_state = HAL_GPIO_ReadPin(keys[i].port, keys[i].pin);
        keys[i].current_state = keys[i].last_state;
    }
}
/***********************************************摁键************************************************/
void key_scan() 
{
    for (uint8_t i = 0; i < KEY_NUM; i++) 
	{
        keys[i].current_state = HAL_GPIO_ReadPin(keys[i].port, keys[i].pin);
 
        if (keys[i].current_state == 0 && keys[i].last_state == 1) 
		{
            led_show(keys[i].key_id, 1);
 
            key_flags[i] = !key_flags[i];  
			//摁键执行	
			if(i==0){count+=20;}
			else if(i==1){count--;}
			else if(i==2){count+=10;}
			else if(i==3){count-=10;}
			
        }

        else if(keys[i].current_state == 1 && keys[i].last_state == 0)
        {
            led_show(keys[i].key_id, 0); 
        }
        keys[i].last_state = keys[i].current_state;
		
		
    }
}
/***********************************************长短摁键----TIM3************************************************/
void key_scan2() 
{
    for (uint8_t i = 0; i < KEY_NUM; i++) 
	{
        keys[i].current_state = HAL_GPIO_ReadPin(keys[i].port, keys[i].pin);
 
        if (keys[i].current_state == 0 && keys[i].last_state == 1) 
		{
			TIM3->CNT=0;
			
            led_show(keys[i].key_id, 1);
            key_flags[i] = !key_flags[i];  
						
        }
		else if(keys[i].current_state == 0 && keys[i].last_state == 0)
		{
			if(TIM3->CNT>=10000)
			{
				if(i==0){count+=3;}
				else if(i==1){count-=3;}
				else if(i==2){count+=30;}
				else if(i==3){count-=30;}
			
			}
						
		}

        else if(keys[i].current_state == 1 && keys[i].last_state == 0)
        {
            led_show(keys[i].key_id, 0); 
			if(TIM3->CNT<10000)
			{
				if(i==0){lcd_high=(lcd_high+1)%6;}
				else if(i==1){count-=1;}
				else if(i==2){count+=10;}
				else if(i==3){count-=10;}
			
			}
        }
        keys[i].last_state = keys[i].current_state;
		
		
    }
}

/***********************************************ADC************************************************/


double get_vol(ADC_HandleTypeDef *hadc)
{
    HAL_ADC_Start(hadc);
    uint32_t adc_value = HAL_ADC_GetValue(hadc);
    return 3.3*adc_value/4095.0;
}



/***********************************************LCD显示（tim3）************************************************/
char text[20];
void lcd_show()
{

    uint16_t line_bg[] = {Black, Black, Black, Black, Black,Black};
    if (lcd_high < sizeof(line_bg)/sizeof(line_bg[0]))
    {
        line_bg[lcd_high] = Yellow;
    }
    LCD_SetBackColor(line_bg[0]);
    sprintf(text, "    SBszm-test       ");
    LCD_DisplayStringLine(Line0, (uint8_t *)text);
	
    LCD_SetBackColor(line_bg[1]);
    sprintf(text, "    count: %d    ", count);
    LCD_DisplayStringLine(Line1, (uint8_t *)text);
    LCD_SetBackColor(line_bg[2]);
	
    sprintf(text, "    cnt:   %d    ", TIM3->CNT);
    LCD_DisplayStringLine(Line2, (uint8_t *)text);    
	
	LCD_SetBackColor(line_bg[3]);
	sprintf(text, "ad1:%.2fV f1:%d", get_vol(&hadc1),fre2);
    LCD_DisplayStringLine(Line3, (uint8_t *)text);  
	
	LCD_SetBackColor(line_bg[4]);
	sprintf(text, "ad2:%.2fV f2:%d", get_vol(&hadc2),fre3);
    LCD_DisplayStringLine(Line4, (uint8_t *)text);

	LCD_SetBackColor(line_bg[5]);
	sprintf(text, "    Frequency:%dHZ    ", fre1);
    LCD_DisplayStringLine(Line5, (uint8_t *)text);
	
 
}

/***********************************************定时器中断2+17************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM2)
	{
		count++;
		led8_flag=!led8_flag;

	}
}




/***********************************************串口接收（定时器4）************************************************/
uint8_t rec_flag;
uint8_t rec_count;
uint8_t rec_buf[20];
char send_buf[20];
uint8_t receive_data;

int fputc(int ch,FILE *f)
{
	HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,0xffff);
	return ch;
}
int fgetc(FILE *f)
{
	uint8_t ch=0;
	HAL_UART_Receive(&huart1,&ch,1,0xffff);
	return ch;


}


//void Serial_sendstring()	
//{
//	char test0[20];
//	sprintf(test0 ,"hello world\r\n");
//	HAL_UART_Transmit(&huart1,(uint8_t *)test0,sizeof(test0),50);
//	

//}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)
	{
//		HAL_UART_Transmit(&huart1,&receive_data,1,50);
		TIM4->CNT=0;
		rec_flag=1;
		rec_buf[rec_count]=receive_data;
		rec_count++;
		HAL_UART_Receive_IT(huart,&receive_data,1);
	
	}


}

void uart_data_rec()     //测试函数
{
	if(rec_flag)
	{
		if(TIM4->CNT > 15)
		{

			if (rec_buf[0] == 'l' && rec_buf[1] == 'a' && rec_buf[2] == 'n') 
			{ 
				sprintf(send_buf, "lan\r\n");
				HAL_UART_Transmit(&huart1, (uint8_t *)send_buf, sizeof(send_buf), 50);
			} 

			else if (rec_buf[0] == 'q' && rec_buf[1] == 'i' && rec_buf[2] == 'a' && rec_buf[3] == 'o') 
			{ 
				sprintf(send_buf, "qiao\r\n");
				HAL_UART_Transmit(&huart1, (uint8_t *)send_buf, sizeof(send_buf), 50);
			}
			else
			{
				sprintf(send_buf, "ERROR\r\n");
				HAL_UART_Transmit(&huart1, (uint8_t *)send_buf, sizeof(send_buf), 50);
			
			}
			rec_flag=0;
			for(int i=0;i<rec_count;i++) rec_buf[i]=0;
			rec_count=0;
		
		}
	
	
	}


}

/***********************************************串口控制-DMA************************************************/

uint8_t receiveData[50];
uint8_t rxBuffer[50];
uint16_t rxSize = 0;
uint8_t OK_ALL = 0;

// 用于存储解析出的x和y值
int x_value, y_value;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(huart == &huart1)
    {
        memcpy(rxBuffer, receiveData, Size);
        rxSize = Size;
        OK_ALL = 1;
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, receiveData, sizeof(receiveData));
    }
}

void re_fun()
{
    if(OK_ALL == 1)
    {

        HAL_UART_Transmit(&huart1, rxBuffer, rxSize, HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart1, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);	
       
        if(rxSize < sizeof(rxBuffer) - 1)
        {
            rxBuffer[rxSize] = '\0';                                                                             // 确保字符串以null结尾
            if(sscanf((char*)rxBuffer, "#x:%d,y:%d#", &x_value, &y_value) == 2)                                  // 解析成功，打印
            {
               
                char print_buf[50];
                sprintf((char*)print_buf, "解析成功: x=%d, y=%d\r\n", x_value, y_value);
                HAL_UART_Transmit(&huart1, (uint8_t*)print_buf, strlen((char*)print_buf), HAL_MAX_DELAY);
            }
            else
            {               
                HAL_UART_Transmit(&huart1, (uint8_t*)"格式不匹配，无法解析\r\n", 23, HAL_MAX_DELAY);              // 解析失败
            }
        }
        else
        {
            HAL_UART_Transmit(&huart1, (uint8_t*)"接收数据过长\r\n", 15, HAL_MAX_DELAY);
        }
        
        OK_ALL = 0;
    }
}



/***********************************************PWM************************************************/

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM17)
	{
		tim17_cnt= HAL_TIM_ReadCapturedValue(&htim17,TIM_CHANNEL_1);  // TIM17->CNT;
		TIM17->CNT=0;
		fre1=80000000/(tim17_cnt*80)-2;
		
	}
	if(htim->Instance==TIM16)
	{
		tim16_cnt= HAL_TIM_ReadCapturedValue(&htim16,TIM_CHANNEL_1);  // TIM16->CNT;
		TIM16->CNT=0;
		fre2=80000000/(tim16_cnt*80)-2;
		
	}
	if(htim->Instance==TIM8)
	{
		tim8_cnt= HAL_TIM_ReadCapturedValue(&htim8,TIM_CHANNEL_1);    // TIM16->CNT;
		TIM8->CNT=0;
		fre3=80000000/(tim8_cnt*80)-2;
		
	}
}


/***********************************************555TIM16、8************************************************/




/***********************************************RTC************************************************/


RTC_TimeTypeDef sTime = {0};
RTC_DateTypeDef sDate = {0};
char Text[20];
uint8_t led_mode=0;
void RTC_GET(void)
{
	HAL_RTC_GetDate(&hrtc,&sDate,RTC_FORMAT_BIN);
	HAL_RTC_GetTime(&hrtc,&sTime,RTC_FORMAT_BIN);
	sprintf(Text, "%d:%d:%d;%2d;%2d;%2d",sDate.Year,sDate.Month,sDate.Date,sTime.Hours,sTime.Minutes,sTime.Seconds);
    LCD_DisplayStringLine(Line6, (uint8_t *)Text);
	led_show(1,led_mode);

}
void  HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	led_mode=1;


}
