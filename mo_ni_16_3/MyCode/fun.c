#include "Header.h"

void led_show(uint8_t led, uint8_t mode)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);

    if (mode == 1)
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 << (led - 1), GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 << (led - 1), GPIO_PIN_SET);

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

uint8_t B1_state = 1, B1_last = 1, B2_state = 1, B2_last = 1, B3_state = 1, B3_last = 1, B4_state = 1, B4_last = 1;
uint8_t led_falg = 1;
void key_scan(void)
{

    B1_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
    B2_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
    B3_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
    B4_state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
    // B1
    if (!B1_state & B1_last)
    {
        led_falg ^= 1; // 异或运算符，两个二进制位相同则为 0，不同则为 1。
    }
    // B2
    if (!B2_state & B2_last)
    {
        counter++;
    }
    // B3
    if (!B3_state & B3_last)
    {
    }
    // B4
    if (!B4_state & B4_last)
    {
    }

    B1_last = B1_state;
    B2_last = B2_state;
    B3_last = B3_state;
    B4_last = B4_state;
}
uint8_t string[20];
void lcd_show(void)
{
    sprintf((char *)string, "        Test       ");
    LCD_DisplayStringLine(Line0, string);

    sprintf((char *)string, "      H E A R T    ");
    LCD_DisplayStringLine(Line1, string);

    sprintf((char *)string, "    R a t e :%.2f  ", adc_read(37));
    LCD_DisplayStringLine(Line3, string);

    sprintf((char *)string, "      C o n :%d     ", counter);
    LCD_DisplayStringLine(Line4, string);
}
// 读取ADC的电压
float adc_read(uint8_t Rx)
{ // 如果形参ADC_HandleTypeDef *hadc 不能使用就这样写
    float value;
    if (Rx == 37)
    {
        HAL_ADC_Start(&hadc2);
        value = 3.3 * HAL_ADC_GetValue(&hadc2) / 4096;
    }
    //  else
    // {
    //    HAL_ADC_Start(&hadc1);
    //    value = 3.3 * HAL_ADC_GetValue(&hadc1) / 4096;
    // }
    return value;
}
int counter;
// main循环函数
void main_pors(void)
{
    lcd_show();
    key_scan();

    led_show(8, led_falg);
}
