/**
  ******************************************************************************
  * 文件名程: bsp_spi_flash.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 板载串行Flash芯片底层驱动程序
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
#include "bsp/spi_flash/bsp_spi_flash.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256
#define W25X_WriteEnable		            0x06 
#define W25X_WriteDisable		            0x04 
#define W25X_ReadStatusReg		          0x05 
#define W25X_WriteStatusReg		          0x01 
#define W25X_ReadData			              0x03 
#define W25X_FastReadData		            0x0B 
#define W25X_FastReadDual		            0x3B 
#define W25X_PageProgram		            0x02 
#define W25X_BlockErase			            0xD8 
#define W25X_SectorErase		            0x20 
#define W25X_ChipErase			            0xC7 
#define W25X_PowerDown			            0xB9 
#define W25X_ReleasePowerDown	          0xAB 
#define W25X_DeviceID			              0xAB 
#define W25X_ManufactDeviceID   	      0x90 
#define W25X_JedecDeviceID		          0x9F 

#define WIP_Flag                        0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte                      0xFF
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
 
/**
  * 函数功能: 串行FLASH初始化
  * 输入参数: 无
  * 返 回 值: uint32_t:返回串行Flash型号ID
  * 说    明：初始化串行Flash底层驱动GPIO和SPI外设
  */
void SPI_FLASH_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;	
  
  /* 使能GPIO和SPI时钟 */
	FLASH_SPI_APBxClock_FUN ( FLASH_SPI_CLK, ENABLE );
  FLASH_SPI_SCK_APBxClock_FUN ( FLASH_SPI_SCK_CLK, ENABLE );
 	FLASH_SPI_MISO_APBxClock_FUN ( FLASH_SPI_MISO_CLK, ENABLE );
  FLASH_SPI_MOSI_APBxClock_FUN ( FLASH_SPI_MOSI_CLK, ENABLE );
  FLASH_SPI_CS_APBxClock_FUN ( FLASH_SPI_CS_CLK, ENABLE );
  
  /* 配置SPI功能引脚：SCK 时钟引脚 */	
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(FLASH_SPI_SCK_PORT, &GPIO_InitStructure);

  /* 配置SPI功能引脚：MISO 主机输入从机输出引脚 */	
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MISO_PIN;
  GPIO_Init(FLASH_SPI_MISO_PORT, &GPIO_InitStructure);

  /* 配置SPI功能引脚：MISO 主机输出从机输入引脚 */	
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MOSI_PIN;
  GPIO_Init(FLASH_SPI_MOSI_PORT, &GPIO_InitStructure);
  
  /* 配置SPI功能引脚：CS 串行Flash片选引脚 */	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_CS_PIN;	
  GPIO_Init(FLASH_SPI_CS_PORT, &GPIO_InitStructure);

  /* 首先禁用串行Flash，等需要操作串行Flash时再使能即可 */
  FLASH_SPI_CS_DISABLE();

  /* SPI外设配置 */
  /* 
   * FLASH芯片：
	 * 在CLK上升沿时到DIO数据采样输入. 
   * 在CLK下降沿时在DIO进行数据输出。
	 * 据此设置CPOL CPHA 
   */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(FLASH_SPIx , &SPI_InitStructure);

  /* 使能SPI外设  */
  SPI_Cmd(FLASH_SPIx , ENABLE);	
}

/**
  * 函数功能: 擦除扇区
  * 输入参数: SectorAddr：待擦除扇区地址，要求为4096倍数
  * 返 回 值: 无
  * 说    明：串行Flash最小擦除块大小为4KB(4096字节)，即一个扇区大小，要求输入参数
  *           为4096倍数。在往串行Flash芯片写入数据之前要求先擦除空间。
  */
void SPI_FLASH_SectorErase(u32 SectorAddr)
{
  /* 发送FLASH写使能命令 */
  SPI_FLASH_WriteEnable();
  SPI_FLASH_WaitForWriteEnd();
  /* 擦除扇区 */
  /* 选择串行FLASH: CS低电平 */
  FLASH_SPI_CS_ENABLE();
  /* 发送扇区擦除指令*/
  SPI_FLASH_SendByte(W25X_SectorErase);
  /*发送擦除扇区地址的高位*/
  SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
  /* 发送擦除扇区地址的中位 */
  SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
  /* 发送擦除扇区地址的低位 */
  SPI_FLASH_SendByte(SectorAddr & 0xFF);
  /* 禁用串行FLASH: CS 高电平 */
  FLASH_SPI_CS_DISABLE();
  /* 等待擦除完毕*/
  SPI_FLASH_WaitForWriteEnd();
}

/**
  * 函数功能: 擦除整片
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：擦除串行Flash整片空间
  */
void SPI_FLASH_BulkErase(void)
{
 /* 发送FLASH写使能命令 */
  SPI_FLASH_WriteEnable();

  /* 整片擦除 Erase */
  /* 选择串行FLASH: CS低电平 */
  FLASH_SPI_CS_ENABLE();
  /* 发送整片擦除指令*/
  SPI_FLASH_SendByte(W25X_ChipErase);
  /* 禁用串行FLASH: CS高电平 */
  FLASH_SPI_CS_DISABLE();

  /* 等待擦除完毕*/
  SPI_FLASH_WaitForWriteEnd();
}

/**
  * 函数功能: 往串行FLASH按页写入数据，调用本函数写入数据前需要先擦除扇区
  * 输入参数: pBuffer：待写入数据的指针
  *           WriteAddr：写入地址
  *           NumByteToWrite：写入数据长度，必须小于等于SPI_FLASH_PerWritePageSize
  * 返 回 值: 无
  * 说    明：串行Flash每页大小为256个字节
  */
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  /* 发送FLASH写使能命令 */
  SPI_FLASH_WriteEnable();

   /* 寻找串行FLASH: CS低电平 */
  FLASH_SPI_CS_ENABLE();
  /* 写送写指令*/
  SPI_FLASH_SendByte(W25X_PageProgram);
  /*发送写地址的高位*/
  SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  /*发送写地址的中位*/
  SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  /*发送写地址的低位*/
  SPI_FLASH_SendByte(WriteAddr & 0xFF);

  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize;
     //printf("Err: SPI_FLASH_PageWrite too large!\n");
  }

  /* 写入数据*/
  while (NumByteToWrite--)
  {
     /* 发送当前要写入的字节数据 */
    SPI_FLASH_SendByte(*pBuffer);
     /* 指向下一字节数据 */
    pBuffer++;
  }

  /* 禁用串行FLASH: CS 高电平 */
  FLASH_SPI_CS_DISABLE();

  /* 等待写入完毕*/
  SPI_FLASH_WaitForWriteEnd();
}

/**
  * 函数功能: 往串行FLASH写入数据，调用本函数写入数据前需要先擦除扇区
  * 输入参数: pBuffer：待写入数据的指针
  *           WriteAddr：写入地址
  *           NumByteToWrite：写入数据长度
  * 返 回 值: 无
  * 说    明：该函数可以设置任意写入数据长度
  */
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % SPI_FLASH_PageSize;
  count = SPI_FLASH_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

  if (Addr == 0) /* 若地址与 SPI_FLASH_PageSize 对齐  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /* 若地址与 SPI_FLASH_PageSize 不对齐 */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
      {
        temp = NumOfSingle - count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

/**
  * 函数功能: 从串行Flash读取数据
  * 输入参数: pBuffer：存放读取到数据的指针
  *           ReadAddr：读取数据目标地址
  *           NumByteToRead：读取数据长度
  * 返 回 值: 无
  * 说    明：该函数可以设置任意读取数据长度
  */
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
  /* 选择串行FLASH: CS低电平 */
  FLASH_SPI_CS_ENABLE();

  /* 发送 读 指令 */
  SPI_FLASH_SendByte(W25X_ReadData);

  /* 发送 读 地址高位 */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* 发送 读 地址中位 */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* 发送 读 地址低位 */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /* 读取数据 */
  {
     /* 读取一个字节*/
    *pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
    /* 指向下一个字节缓冲区 */
    pBuffer++;
  }

  /* 禁用串行FLASH: CS 高电平 */
  FLASH_SPI_CS_DISABLE();
}

/**
  * 函数功能: 读取串行Flash型号的ID
  * 输入参数: 无
  * 返 回 值: u32：串行Flash的型号ID
  * 说    明：  FLASH_ID      IC型号      存储空间大小         
                0xEF3015      W25X16        2M byte
                0xEF4015	    W25Q16        4M byte
                0XEF4017      W25Q64        8M byte
                0XEF4018      W25Q128       16M byte  (YS-F1Pro开发板默认配置)
  */
u32 SPI_FLASH_ReadID(void)
{
  u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /* 选择串行FLASH: CS低电平 */
  FLASH_SPI_CS_ENABLE();

  /* 发送命令：读取芯片型号ID */
  SPI_FLASH_SendByte(W25X_JedecDeviceID);

  /* 从串行Flash读取一个字节数据 */
  Temp0 = SPI_FLASH_SendByte(Dummy_Byte);

  /* 从串行Flash读取一个字节数据 */
  Temp1 = SPI_FLASH_SendByte(Dummy_Byte);

  /* 从串行Flash读取一个字节数据 */
  Temp2 = SPI_FLASH_SendByte(Dummy_Byte);

  /* 禁用串行Flash：CS高电平 */
  FLASH_SPI_CS_DISABLE();
  
  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
  return Temp;
}

/**
  * 函数功能: 读取串行Flash设备ID
  * 输入参数: 无
  * 返 回 值: u32：串行Flash的设备ID
  * 说    明：
  */
u32 SPI_FLASH_ReadDeviceID(void)
{
  u32 Temp = 0;

  /* 选择串行FLASH: CS低电平 */
  FLASH_SPI_CS_ENABLE();

  /* 发送命令：读取芯片设备ID * */
  SPI_FLASH_SendByte(W25X_DeviceID);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  
  /* 从串行Flash读取一个字节数据 */
  Temp = SPI_FLASH_SendByte(Dummy_Byte);

  /* 禁用串行Flash：CS高电平 */
  FLASH_SPI_CS_DISABLE();

  return Temp;
}

/**
  * 函数功能: 启动连续读取数据串
  * 输入参数: ReadAddr：读取地址
  * 返 回 值: 无
  * 说    明：Initiates a read data byte (READ) sequence from the Flash.
  *           This is done by driving the /CS line low to select the device,
  *           then the READ instruction is transmitted followed by 3 bytes
  *           address. This function exit and keep the /CS line low, so the
  *           Flash still being selected. With this technique the whole
  *           content of the Flash is read with a single READ instruction.
  */
void SPI_FLASH_StartReadSequence(u32 ReadAddr)
{
  /* Select the FLASH: Chip Select low */
  FLASH_SPI_CS_ENABLE();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(W25X_ReadData);

  /* Send the 24-bit address of the address to read from -----------------------*/
  /* Send ReadAddr high nibble address byte */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
}

/**
  * 函数功能: 从串行Flash读取一个字节数据
  * 输入参数: 无
  * 返 回 值: u8：读取到的数据
  * 说    明：This function must be used only if the Start_Read_Sequence
  *           function has been previously called.
  */
u8 SPI_FLASH_ReadByte(void)
{
  return (SPI_FLASH_SendByte(Dummy_Byte));
}

/**
  * 函数功能: 往串行Flash读取写入一个字节数据并接收一个字节数据
  * 输入参数: byte：待发送数据
  * 返 回 值: u8：接收到的数据
  * 说    明：无
  */
u8 SPI_FLASH_SendByte(u8 byte)
{
  /* 循环等待直到SPI 数据寄存器DR为空，即当DR寄存器不为空时持续等待 */
  while (SPI_I2S_GetFlagStatus(FLASH_SPIx , SPI_I2S_FLAG_TXE) == RESET);

  /* 通过SPI外设发送一个字节数据 */
  SPI_I2S_SendData(FLASH_SPIx , byte);

  /* 等待接收到数据 */
  while (SPI_I2S_GetFlagStatus(FLASH_SPIx , SPI_I2S_FLAG_RXNE) == RESET);

  /* 读取SPI总线接收到一个字节数据并返回 */
  return SPI_I2S_ReceiveData(FLASH_SPIx );
}

/**
  * 函数功能: 往串行Flash读取写入半字(16bit)数据并接收半字数据
  * 输入参数: byte：待发送数据
  * 返 回 值: u16：接收到的数据
  * 说    明：无
  */
u16 SPI_FLASH_SendHalfWord(u16 HalfWord)
{
  /* 循环等待直到SPI 数据寄存器DR为空，即当DR寄存器不为空时持续等待 */
  while (SPI_I2S_GetFlagStatus(FLASH_SPIx , SPI_I2S_FLAG_TXE) == RESET);

  /* 通过SPI外设发送半字数据 */
  SPI_I2S_SendData(FLASH_SPIx , HalfWord);

  /* 等待接收到数据 */
  while (SPI_I2S_GetFlagStatus(FLASH_SPIx , SPI_I2S_FLAG_RXNE) == RESET);

  /* 读取SPI总线接收到半字数据并返回 */
  return SPI_I2S_ReceiveData(FLASH_SPIx );
}

/**
  * 函数功能: 使能串行Flash写操作
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void SPI_FLASH_WriteEnable(void)
{
  /* 选择串行FLASH: CS低电平 */
  FLASH_SPI_CS_ENABLE();

  /* 发送命令：写使能 */
  SPI_FLASH_SendByte(W25X_WriteEnable);

  /* 禁用串行Flash：CS高电平 */
  FLASH_SPI_CS_DISABLE();
}

/**
  * 函数功能: 等待数据写入完成
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：Polls the status of the Write In Progress (WIP) flag in the
  *           FLASH's status  register  and  loop  until write  opertaion
  *           has completed.
  */
void SPI_FLASH_WaitForWriteEnd(void)
{
  u8 FLASH_Status = 0;

  /* Select the FLASH: Chip Select low */
  FLASH_SPI_CS_ENABLE();

  /* Send "Read Status Register" instruction */
  SPI_FLASH_SendByte(W25X_ReadStatusReg);

  /* Loop as long as the memory is busy with a write cycle */
  do
  {
    /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);	 
  }
  while ((FLASH_Status & WIP_Flag) == SET); /* Write in progress */

  /* Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_DISABLE();
}


/**
  * 函数功能: 进入掉电模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void SPI_Flash_PowerDown(void)   
{ 
  /* Select the FLASH: Chip Select low */
  FLASH_SPI_CS_ENABLE();

  /* Send "Power Down" instruction */
  SPI_FLASH_SendByte(W25X_PowerDown);

  /* Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_DISABLE();
}   

/**
  * 函数功能: 唤醒串行Flash
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void SPI_Flash_WAKEUP(void)   
{
  /* Select the FLASH: Chip Select low */
  FLASH_SPI_CS_ENABLE();

  /* Send "Power Down" instruction */
  SPI_FLASH_SendByte(W25X_ReleasePowerDown);

  /* Deselect the FLASH: Chip Select high */
  FLASH_SPI_CS_DISABLE(); 
}   
   
/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
