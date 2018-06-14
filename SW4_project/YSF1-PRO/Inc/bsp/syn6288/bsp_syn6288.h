#ifndef __BSP_SYN6288_H__
#define __BSP_SYN6288_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include <stm32f10x.h>

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define SYN6288_USART_BAUDRATE                      9600

#define SYN6288_USART_ClockCmd                      RCC_APB1PeriphClockCmd
#define SYN6288_USART_CLK                           RCC_APB1Periph_USART3

#define SYN6288_USART_GPIO_ClockCmd                 RCC_APB2PeriphClockCmd    
#define SYN6288_USART_TX_PORT                       GPIOB  
#define SYN6288_USART_TX_PIN                        GPIO_Pin_10
#define SYN6288_USART_TX_CLK                        RCC_APB2Periph_GPIOB
#define SYN6288_USART_RX_PORT                       GPIOB
#define SYN6288_USART_RX_PIN                        GPIO_Pin_11
#define SYN6288_USART_RX_CLK                        RCC_APB2Periph_GPIOB

#define SYN6288_USART_IRQHANDLER                    USART3_IRQHandler
#define SYN6288_USART_IRQn                          USART3_IRQn
#define SYN6288_USART                               USART3

/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
void SYN6288_Init(void);
void SYN6288_Play(const char *str);
void SYN6288_USART_Config(void);
void SYN6288_Usart_SendByte(uint8_t ch);
void SYN6288_Usart_SendStr_length(uint8_t *str,uint32_t strlen);
void SYN6288_Usart_SendString(uint8_t *str);

#endif  // __BSP_SYN6288_H__

/******************* (C) COPYRIGHT 2014-2019 学生开放实验室 *****END OF FILE****/
