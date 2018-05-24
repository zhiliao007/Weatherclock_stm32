#ifndef  __BSP_ESP8266_H__
#define	 __BSP_ESP8266_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include <stm32f10x.h>
#include <stdio.h>
#include <stdbool.h>

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif
/* 类型定义 ------------------------------------------------------------------*/
/******************************* ESP8266 数据类型定义 ***************************/
typedef enum{
	STA,
  AP,
  STA_AP  
} ENUM_Net_ModeTypeDef;

typedef enum{
	 enumTCP,
	 enumUDP,
} ENUM_NetPro_TypeDef;

typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
} ENUM_ID_NO_TypeDef;
	
typedef enum{
	OPEN = 0,
	WEP = 1,
	WPA_PSK = 2,
	WPA2_PSK = 3,
	WPA_WPA2_PSK = 4,
} ENUM_AP_PsdMode_TypeDef;

/* 宏定义 --------------------------------------------------------------------*/
#define RX_BUF_MAX_LEN     (7*1024)                                     //最大接收缓存字节数

/******************************** ESP8266 连接引脚定义 ***********************************/
#define ESP8266_RST_CLK                               RCC_APB2Periph_GPIOD
#define ESP8266_RST_PORT                              GPIOD
#define ESP8266_RST_PIN                               GPIO_Pin_6

#define ESP8266_USART_BAUD_RATE                       115200
#define ESP8266_USARTx                                USART2

#define ESP8266_USART_APBxClock_FUN                   RCC_APB1PeriphClockCmd
#define ESP8266_USART_CLK                             RCC_APB1Periph_USART2
#define ESP8266_USART_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define ESP8266_USART_GPIO_CLK                        RCC_APB2Periph_GPIOA     
#define ESP8266_USART_TX_PORT                         GPIOA   
#define ESP8266_USART_TX_PIN                          GPIO_Pin_2
#define ESP8266_USART_RX_PORT                         GPIOA
#define ESP8266_USART_RX_PIN                          GPIO_Pin_3
#define ESP8266_USART_IRQ                             USART2_IRQn
#define ESP8266_USART_INT_FUN                         USART2_IRQHandler

/*********************************************** ESP8266 函数宏定义 *******************************************/
#define ESP8266_Usart( fmt, ... )           USART_printf (ESP8266_USARTx, fmt, ##__VA_ARGS__ ) 
#define PC_Usart( fmt, ... )                printf ( fmt, ##__VA_ARGS__ )
//#define PC_Usart( fmt, ... )                

#define ESP8266_RST_HIGH_LEVEL()            GPIO_SetBits(ESP8266_RST_PORT,ESP8266_RST_PIN )
#define ESP8266_RST_LOW_LEVEL()             GPIO_ResetBits(ESP8266_RST_PORT,ESP8266_RST_PIN )

/* 扩展变量 ------------------------------------------------------------------*/
/******************************* ESP8266 外部全局变量声明 ***************************/
extern struct  STRUCT_USARTx_Fram                                  //串口数据帧的处理结构体
{
	char  Data_RX_BUF [ RX_BUF_MAX_LEN ];
  union {
    __IO u16 InfAll;
    struct {
		  __IO u16 FramLength       :15;                               // 14:0 
		  __IO u16 FramFinishFlag   :1;                                // 15 
	  } InfBit;
  }; 	
} strEsp8266_Fram_Record;


/* 函数声明 ------------------------------------------------------------------*/
void  ESP8266_Init( void );
void  ESP8266_stop( void );
void  ESP8266_Rst( void );
bool  ESP8266_Cmd( char * cmd, char * reply1, char * reply2, u32 waittime );
bool  ESP8266_AT_Test( void );
bool  ESP8266_Net_Mode_Choose( ENUM_Net_ModeTypeDef enumMode );
bool  ESP8266_JoinAP( char * pSSID, char * pPassWord );
bool  ESP8266_BuildAP( char * pSSID, char * pPassWord, ENUM_AP_PsdMode_TypeDef enunPsdMode );
bool  ESP8266_Enable_MultipleId( FunctionalState enumEnUnvarnishTx );
bool  ESP8266_Link_Server( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id);
bool  ESP8266_StartOrShutServer( FunctionalState enumMode, char * pPortNum, char * pTimeOver );
uint8_t ESP8266_Get_LinkStatus( void );
uint8_t ESP8266_Get_IdLinkStatus( void );
uint8_t ESP8266_Inquire_ApIp( char * pApIp, uint8_t ucArrayLength );
bool ESP8266_UnvarnishSend( void );
void ESP8266_ExitUnvarnishSend( void );
bool ESP8266_SendString( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId );
char * ESP8266_ReceiveString( FunctionalState enumEnUnvarnishTx );

void  USART_printf(USART_TypeDef * USARTx, char * Data, ... );

#endif

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
