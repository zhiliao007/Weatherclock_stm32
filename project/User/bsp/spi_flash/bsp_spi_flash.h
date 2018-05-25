#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include <stm32f10x.h>

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
//#define  SPI_FLASH_ID                       0xEF3015     //W25X16
//#define  SPI_FLASH_ID                       0xEF4015	    //W25Q16
//#define  SPI_FLASH_ID                       0XEF4017     //W25Q64
#define  SPI_FLASH_ID                       0XEF4018     //W25Q128  YS-F1Pro开发默认使用

/************************** SPI Flash 连接引脚定义********************************/
#define FLASH_SPIx                        SPI1
#define FLASH_SPI_APBxClock_FUN           RCC_APB2PeriphClockCmd
#define FLASH_SPI_CLK                     RCC_APB2Periph_SPI1

#define FLASH_SPI_SCK_APBxClock_FUN       RCC_APB2PeriphClockCmd
#define FLASH_SPI_SCK_CLK                 RCC_APB2Periph_GPIOA   
#define FLASH_SPI_SCK_PORT                GPIOA   
#define FLASH_SPI_SCK_PIN                 GPIO_Pin_5

#define FLASH_SPI_MISO_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define FLASH_SPI_MISO_CLK                RCC_APB2Periph_GPIOA    
#define FLASH_SPI_MISO_PORT               GPIOA 
#define FLASH_SPI_MISO_PIN                GPIO_Pin_6

#define FLASH_SPI_MOSI_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define FLASH_SPI_MOSI_CLK                RCC_APB2Periph_GPIOA    
#define FLASH_SPI_MOSI_PORT               GPIOA 
#define FLASH_SPI_MOSI_PIN                GPIO_Pin_7

#define FLASH_SPI_CS_APBxClock_FUN        RCC_APB2PeriphClockCmd
#define FLASH_SPI_CS_CLK                  RCC_APB2Periph_GPIOA    
#define FLASH_SPI_CS_PORT                 GPIOA
#define FLASH_SPI_CS_PIN                  GPIO_Pin_4

#define FLASH_SPI_CS_ENABLE()             GPIO_ResetBits(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN)
#define FLASH_SPI_CS_DISABLE()            GPIO_SetBits(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN)


#define CALIBRATE_DATA_ADDR               2*4096

/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
void SPI_FLASH_Init(void);
void SPI_FLASH_SectorErase(uint32_t SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t SPI_FLASH_ReadID(void);
uint32_t SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);

uint8_t SPI_FLASH_ReadByte(void);
uint8_t SPI_FLASH_SendByte(uint8_t byte);
uint16_t SPI_FLASH_SendHalfWord(uint16_t HalfWord);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

#endif /* __SPI_FLASH_H__ */
/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
