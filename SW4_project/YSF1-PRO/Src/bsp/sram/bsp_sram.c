/**
  ******************************************************************************
  * 文件名程: bsp_sram.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 外部扩展SRAM底层驱动实现
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-F1Pro使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "sram/bsp_sram.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 初始化外部SRAM
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void FSMC_SRAM_Init(void)
{	
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
	GPIO_InitTypeDef  GPIO_InitStructure;
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);
  
	GPIO_InitStructure.GPIO_Pin = 0xFF33; 			 	//PORTD复用推挽输出 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure);

 
	GPIO_InitStructure.GPIO_Pin = 0xFF83; 			 	//PORTE复用推挽输出 
 	GPIO_Init(GPIOE, &GPIO_InitStructure);

 	GPIO_InitStructure.GPIO_Pin = 0xF03F; 			 	//PORTD复用推挽输出 
 	GPIO_Init(GPIOF, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = 0x043F; 			 	//PORTD复用推挽输出 
 	GPIO_Init(GPIOG, &GPIO_InitStructure);
 
	 			  	  
	readWriteTiming.FSMC_AddressSetupTime = 0x00;	 //地址建立时间（ADDSET）为1个HCLK 1/36M=27ns
	readWriteTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（ADDHLD）模式A未用到	
	readWriteTiming.FSMC_DataSetupTime = 0x03;		 //数据保持时间（DATAST）为3个HCLK 4/72M=55ns(对EM的SRAM芯片)	 
	readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	readWriteTiming.FSMC_CLKDivision = 0x00;
	readWriteTiming.FSMC_DataLatency = 0x00;
	readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 
 
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;//  这里我们使用NE3 ，也就对应BTCR[4],[5]。
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; 
	FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM   
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//存储器数据宽度为16bit  
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//存储器写使能 
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;  
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable; // 读写使用相同的时序
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;  
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &readWriteTiming; //读写同样时序

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC配置

	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  // 使能BANK3										  
											
}
	  		
/**
  * 函数功能: 在指定地址开始,连续写入n个字节.
  * 输入参数: pBuffer:字节指针
  *           WriteAddr:要写入的地址
  *           n:要写入的字节数
  * 返 回 值: 无
  * 说    明：无
  */
void FSMC_SRAM_WriteBuffer(uint8_t* pBuffer,uint32_t WriteAddr,uint32_t n)
{
	for(;n!=0;n--)  
	{										    
		*(vu8*)(Bank1_SRAM3_ADDR+WriteAddr)=*pBuffer;	  
		WriteAddr++; 
		pBuffer++;
	}   
}									

/**
  * 函数功能: 在指定地址开始,连续读出n个字节.
  * 输入参数: pBuffer:字节指针
  *           ReadAddr:要读出的起始地址
  *           n:要写入的字节数
  * 返 回 值: 无
  * 说    明：无
  */
void FSMC_SRAM_ReadBuffer(uint8_t* pBuffer,uint32_t ReadAddr,uint32_t n)
{
	for(;n!=0;n--)  
	{											    
		*pBuffer++=*(vu8*)(Bank1_SRAM3_ADDR+ReadAddr);    
		ReadAddr++; 
	}  
} 

/**
  * 函数功能: 测试函数，在指定地址写入1个字节
  * 输入参数: addr:地址
  *           data:要写入的数据
  * 返 回 值: 无
  * 说    明：无
  */
void fsmc_sram_test_write(uint8_t data,uint32_t addr)
{			   
	FSMC_SRAM_WriteBuffer(&data,addr,1);//写入1个字节
}

/**
  * 函数功能: 测试函数，读取1个字节
  * 输入参数: addr:要读取的地址
  *           data:要写入的数据
  * 返 回 值: uint8_t:读取到的数据
  * 说    明：无
  */
uint8_t fsmc_sram_test_read(uint32_t addr)
{
	uint8_t data;
	FSMC_SRAM_ReadBuffer(&data,addr,1);
	return data;
}	
/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
