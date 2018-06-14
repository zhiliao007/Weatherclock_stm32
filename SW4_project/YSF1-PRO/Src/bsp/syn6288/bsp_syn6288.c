/**
  ******************************************************************************
  * 文件名程: bsp_syn6288.c 
  * 作    者: 乔启鸣
  * 版    本: V1.0
  * 编写日期: 2018-05-24
  * 功    能: syn6288硬件驱动
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "syn6288/bsp_syn6288.h"
#include <string.h>

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

void SYN6288_Init(void)
{
	SYN6288_USART_Config();
}

void SYN6288_Play(const char *str)
{
	uint8_t headOfFrame[5];  
	uint8_t ecc  = 0;  			//定义校验字节
	uint8_t length;
	uint32_t i=0; 
	
	length =strlen(str); 			//需要发送文本的长度
	
	headOfFrame[0] = 0xFD ; 			//构造帧头FD
	headOfFrame[1] = 0x00 ; 			//构造数据区长度的高字节
	headOfFrame[2] = length + 3; 		//构造数据区长度的低字节
	headOfFrame[3] = 0x01 ; 			//构造命令字：合成播放命令
	headOfFrame[4] = 0x01 ;
	
	for(i = 0; i<5; i++)   				//依次发送构造好的5个帧头字节
	{  
		ecc=ecc^(headOfFrame[i]); 		//对发送的字节进行异或校验	
		SYN6288_Usart_SendByte(headOfFrame[i]);
	}

	for(i = 0; i<length; i++)   		//依次发送待合成的文本数据
	{  
		ecc=ecc^(str[i]); 				//对发送的字节进行异或校验	
		SYN6288_Usart_SendByte(str[i]);		
	}

	SYN6288_Usart_SendByte(ecc);
}

/**
  * 函数功能: 配置NVIC，设定USART接收中断优先级.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void SYN6288_USART_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	 /* 嵌套向量中断控制器组选择 */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
   /* 配置USART为中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = SYN6288_USART_IRQn;	 
	 /* 抢断优先级为0 */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	 /* 子优先级为1 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	 /* 使能中断 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 /* 初始化配置NVIC */
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * 函数功能: 串口参数配置.
  * 输入参数: 无
  * 返 回 值: 无
  */
void SYN6288_USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
  
    SYN6288_USART_NVIC_Configuration();
	
    SYN6288_USART_ClockCmd(SYN6288_USART_CLK,ENABLE);
    SYN6288_USART_GPIO_ClockCmd(SYN6288_USART_TX_CLK | SYN6288_USART_RX_CLK | RCC_APB2Periph_AFIO,ENABLE);
  
	/* 调试USART功能GPIO初始化 */
	GPIO_InitStructure.GPIO_Pin =  SYN6288_USART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SYN6288_USART_TX_PORT, &GPIO_InitStructure);    
  
	GPIO_InitStructure.GPIO_Pin = SYN6288_USART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SYN6288_USART_RX_PORT, &GPIO_InitStructure);	
			
	/* USART工作环境配置 */
	USART_InitStructure.USART_BaudRate = SYN6288_USART_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(SYN6288_USART, &USART_InitStructure);
  
  //USART_ITConfig(SYN6288_USART, USART_IT_RXNE, ENABLE);	
  
	USART_Cmd(SYN6288_USART, ENABLE);
	USART_ClearFlag(SYN6288_USART, USART_FLAG_TC|USART_FLAG_TXE|USART_FLAG_RXNE);
}

/**
  * 函数功能: 串口发送一个字节数据 
  * 输入参数: ch：待发送字符
  * 返 回 值: 无
  * 说    明：无
  */
void SYN6288_Usart_SendByte(uint8_t ch)
{
  /* 发送一个字节数据到USART1 */
  USART_SendData(SYN6288_USART,ch);

  /* 等待发送完毕 */
  while (USART_GetFlagStatus(SYN6288_USART, USART_FLAG_TXE) == RESET);	
}

/**
  * 函数功能: 串口发送指定长度的字符串
  * 输入参数: str：待发送字符串缓冲器
  *           strlen:指定字符串长度
  * 返 回 值: 无
  * 说    明：无
  */
void SYN6288_Usart_SendStr_length(uint8_t *str,uint32_t strlen)
{
	unsigned int k=0;
	do 
	{
		SYN6288_Usart_SendByte(*(str + k));
		k++;
	} while(k < strlen);
}

/**
  * 函数功能: 串口发送字符串，直到遇到字符串结束符
  * 输入参数: str：待发送字符串缓冲器
  * 返 回 值: 无
  * 说    明：无
  */
void SYN6288_Usart_SendString(uint8_t *str)
{
	unsigned int k=0;
	do 
	{
		SYN6288_Usart_SendByte(*(str + k));
		k++;
	} while(*(str + k)!='\0');
}

/******************* (C) COPYRIGHT 2014-2019 学生开放实验室 *****END OF FILE****/
