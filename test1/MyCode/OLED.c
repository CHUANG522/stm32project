#include "OLED.h"
#include "OLED_Font.h"

// OLED显存（128*64/8=1024字节）
static uint8_t OLED_GRAM[128][8];

/********************* 内部函数 *********************/
// I2C起始信号
static void OLED_I2C_Start(void) {
  OLED_SDA_H;
  OLED_SCL_H;
  HAL_Delay(1);
  OLED_SDA_L;
  HAL_Delay(1);
  OLED_SCL_L;
}

// I2C停止信号
static void OLED_I2C_Stop(void) {
  OLED_SDA_L;
  OLED_SCL_H;
  HAL_Delay(1);
  OLED_SDA_H;
  HAL_Delay(1);
}

// I2C发送一个字节
static void OLED_I2C_SendByte(uint8_t byte) {
  uint8_t i;
  for (i = 0; i < 8; i++) {
    if (byte & 0x80)
      OLED_SDA_H;
    else
      OLED_SDA_L;
    byte <<= 1;
    OLED_SCL_H;
    HAL_Delay(1);
    OLED_SCL_L;
    HAL_Delay(1);
  }
  // 等待应答（省略）
  OLED_SCL_H;
  HAL_Delay(1);
  OLED_SCL_L;
  HAL_Delay(1);
}

/********************* 外部函数 *********************/
// 软件I2C引脚初始化
void OLED_I2C_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // 开启GPIOB时钟
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // 配置SCL/SDA引脚为开漏输出
  GPIO_InitStruct.Pin = OLED_SCL_PIN | OLED_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; // 开漏输出
  GPIO_InitStruct.Pull = GPIO_PULLUP;         // 上拉
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OLED_GPIO_PORT, &GPIO_InitStruct);

  // 初始化引脚为高电平
  OLED_SCL_H;
  OLED_SDA_H;
}

// 写命令
void OLED_WriteCmd(uint8_t cmd) {
  OLED_I2C_Start();
  OLED_I2C_SendByte(0x78); // OLED I2C地址（0x78）
  OLED_I2C_SendByte(0x00); // 写命令
  OLED_I2C_SendByte(cmd);
  OLED_I2C_Stop();
}

// 写数据
void OLED_WriteData(uint8_t dat) {
  OLED_I2C_Start();
  OLED_I2C_SendByte(0x78); // OLED I2C地址
  OLED_I2C_SendByte(0x40); // 写数据
  OLED_I2C_SendByte(dat);
  OLED_I2C_Stop();
}

// OLED初始化
void OLED_Init(void) {
  OLED_I2C_Init(); // 初始化I2C引脚

  HAL_Delay(100); // 上电延时

  // SSD1306初始化指令
  OLED_WriteCmd(0xAE); // 关闭显示
  OLED_WriteCmd(0xD5); // 设置时钟分频因子
  OLED_WriteCmd(0x80); // 默认值
  OLED_WriteCmd(0xA8); // 设置驱动路数
  OLED_WriteCmd(0x3F); // 1/64
  OLED_WriteCmd(0xD3); // 设置显示偏移
  OLED_WriteCmd(0x00); // 无偏移
  OLED_WriteCmd(0x40); // 设置显示开始行
  OLED_WriteCmd(0x8D); // 电荷泵设置
  OLED_WriteCmd(0x14); // 开启电荷泵
  OLED_WriteCmd(0x20); // 设置内存地址模式
  OLED_WriteCmd(0x02); // 页地址模式
  OLED_WriteCmd(0xA1); // 段重定义
  OLED_WriteCmd(0xC8); // COM扫描方向
  OLED_WriteCmd(0xDA); // COM硬件引脚配置
  OLED_WriteCmd(0x12);
  OLED_WriteCmd(0x81); // 对比度设置
  OLED_WriteCmd(0xCF); // 对比度值
  OLED_WriteCmd(0xD9); // 预充电周期
  OLED_WriteCmd(0xF1);
  OLED_WriteCmd(0xDB); // VCOMH电压倍率
  OLED_WriteCmd(0x30);
  OLED_WriteCmd(0xA4); // 全局显示开启
  OLED_WriteCmd(0xA6); // 正常显示
  OLED_WriteCmd(0xAF); // 开启显示

  OLED_Clear(); // 清屏
}

// 清屏
void OLED_Clear(void) {
  uint8_t i, j;
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 128; j++) {
      OLED_GRAM[j][i] = 0x00;
    }
  }
  OLED_Refresh();
}

// 刷新显存到屏幕
void OLED_Refresh(void) {
  uint8_t i, j;
  for (i = 0; i < 8; i++) {
    OLED_WriteCmd(0xB0 + i); // 设置页地址
    OLED_WriteCmd(0x00);     // 设置列低地址
    OLED_WriteCmd(0x10);     // 设置列高地址
    for (j = 0; j < 128; j++) {
      OLED_WriteData(OLED_GRAM[j][i]);
    }
  }
}

// 显示单个字符
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size) {
  uint8_t temp, t, t1;
  uint8_t y0 = y;
  chr -= ' '; // 偏移量修正

  // 12x6字体
  if (size == 12) {
    for (t = 0; t < 6; t++) {
      temp = OLED_Font12x6[chr * 6 + t];
      for (t1 = 0; t1 < 8; t1++) {
        if (temp & (0x01 << t1))
          OLED_GRAM[x][y] |= (1 << t1);
        else
          OLED_GRAM[x][y] &= ~(1 << t1);
        y++;
        if (y >= 64)
          return;
        if ((y0 + 12) <= y)
          break;
      }
      x++;
      if (x >= 128)
        return;
    }
  }
  // 16x8字体
  else if (size == 16) {
    for (t = 0; t < 8; t++) {
      temp = OLED_Font16x8[chr * 16 + t];
      for (t1 = 0; t1 < 8; t1++) {
        if (temp & (0x01 << t1))
          OLED_GRAM[x][y] |= (1 << t1);
        else
          OLED_GRAM[x][y] &= ~(1 << t1);
        y++;
        if (y >= 64)
          return;
        if ((y0 + 16) <= y)
          break;
      }
      x++;
      if (x >= 128)
        return;
    }
    for (t = 0; t < 8; t++) {
      temp = OLED_Font16x8[chr * 16 + t + 8];
      for (t1 = 0; t1 < 8; t1++) {
        if (temp & (0x01 << t1))
          OLED_GRAM[x][y] |= (1 << t1);
        else
          OLED_GRAM[x][y] &= ~(1 << t1);
        y++;
        if (y >= 64)
          return;
        if ((y0 + 16) <= y)
          break;
      }
      x++;
      if (x >= 128)
        return;
    }
  }
}

// 显示字符串
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *str, uint8_t size) {
  uint8_t j = 0;
  while (str[j] != '\0') {
    OLED_ShowChar(x, y, str[j], size);
    x += size / 2;
    if (x > 128 - size / 2) {
      x = 0;
      y += size;
    }
    j++;
  }
}

// 显示数字
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len,
                  uint8_t size) {
  uint8_t t, temp;
  uint8_t enshow = 0;
  for (t = 0; t < len; t++) {
    temp = (num / (uint32_t)pow(10, len - t - 1)) % 10;
    if (enshow == 0 && t < (len - 1)) {
      if (temp == 0) {
        OLED_ShowChar(x + (size / 2) * t, y, ' ', size);
        continue;
      } else
        enshow = 1;
    }
    OLED_ShowChar(x + (size / 2) * t, y, temp + '0', size);
  }
}