/**
  ******************************************************************************
  * 文件名程: bsp_led.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 板载LED灯底层驱动实现
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-F1Pro使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */
/**
  * 很多初学者可能会疑惑bsp_led.c和bsp_led.h这两个文件哪里来的？？是不是官方下载
  * 的？？实际上，这两个文件是要求我们自己创建的，内容也是我们自己敲代码的。
  * 这个已经属于应用层次的代码ST官方是不提供的，需要我们根据自己的开发板硬件来
  * 编写我们的程序，所以需要我们自己新建文件编写。
  * 这里我们使用#include "bsp/led/bsp_led.h"而不是#include "bsp_led.h"是因为我们
  * 使用相对路径方法，可以省去在魔术棒(工程设置选项)的C/C++项目中添加文件的路径，
  * 使得移植程序更加方便。当然这样使用的前提是我们已经把"User"目录已经包含在工程
  * 中。
  */

/* 包含头文件 ----------------------------------------------------------------*/
#include "led/bsp_led.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: 板载LED灯IO引脚初始化.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：使用宏定义方法代替具体引脚号，方便程序移植，只要简单修改bsp_led.h
  *           文件相关宏定义就可以方便修改引脚。
  */
void LED_GPIO_Init(void)
{
   /* 定义IO硬件初始化结构体变量 */
  GPIO_InitTypeDef GPIO_InitStructure;
	
	/* 使能(开启)LED1引脚对应IO端口时钟 */  
  RCC_APB2PeriphClockCmd(LED1_RCC_CLOCKGPIO|LED2_RCC_CLOCKGPIO |LED3_RCC_CLOCKGPIO , ENABLE);
  
  /* 设定LED1对应引脚IO编号 */
  GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;  
  /* 设定LED1对应引脚IO最大操作速度 ：GPIO_Speed_50MHz */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  /* 设定LED1对应引脚IO为输出模式 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  /* 初始化LED1对应引脚IO */
  GPIO_Init(LED1_GPIO, &GPIO_InitStructure);
 
  /* 设定LED2对应引脚IO编号 */
  GPIO_InitStructure.GPIO_Pin = LED2_GPIO_PIN;  
  /* 初始化LED2对应引脚IO */
  GPIO_Init(LED2_GPIO, &GPIO_InitStructure);

  /* 设定LED3对应引脚IO编号 */
  GPIO_InitStructure.GPIO_Pin = LED3_GPIO_PIN;  
  /* 初始化LED3对应引脚IO */
  GPIO_Init(LED3_GPIO, &GPIO_InitStructure);
  
  /* 设置引脚输出为低电平，此时LED1灭 */
  GPIO_ResetBits(LED1_GPIO,LED1_GPIO_PIN);  
  /* 设置引脚输出为低电平，此时LED2灭 */
  GPIO_ResetBits(LED2_GPIO,LED2_GPIO_PIN);  
  /* 设置引脚输出为低电平，此时LED3灭 */
  GPIO_ResetBits(LED3_GPIO,LED3_GPIO_PIN);
}

/**
  * 函数功能: 设置板载LED灯的状态
  * 输入参数: LEDx:其中x可甚至为(1,2,3)用来选择对应的LED灯
  * 输入参数：state:设置LED灯的输出状态。
  *             可选值：LED_OFF：LED灯灭；
  *             可选值：LED_ON： LED灯亮。
  * 返 回 值: 无
  * 说    明：该函数使用类似标准库函数的编程方法，方便理解标准库函数编程思想。
  */
void LEDx_StateSet(uint8_t LEDx,LEDState_TypeDef state)
{
  /* 检查输入参数是否合法 */
  assert_param(IS_LED_TYPEDEF(LEDx));
  assert_param(IS_LED_STATE(state));
  
  /* 判断设置的LED灯状态，如果设置为LED灯灭 */
  if(state==LED_OFF)
  {
    if(LEDx & LED1)            
      GPIO_ResetBits(LED1_GPIO,LED1_GPIO_PIN);/* 设置引脚输出为低电平，此时LED1灭 */
    if(LEDx & LED2)
      GPIO_ResetBits(LED2_GPIO,LED2_GPIO_PIN);/* 设置引脚输出为低电平，此时LED2灭 */
    if(LEDx & LED3)
      GPIO_ResetBits(LED3_GPIO,LED3_GPIO_PIN);/* 设置引脚输出为低电平，此时LED3灭 */    
  }
  else  /* state=LED_ON ：否则，设置LED灯为亮 */
  {
    if(LEDx & LED1)
      GPIO_SetBits(LED1_GPIO,LED1_GPIO_PIN);/* 设置引脚输出为高电平，此时LED1亮 */
    if(LEDx & LED2)
      GPIO_SetBits(LED2_GPIO,LED2_GPIO_PIN);/* 设置引脚输出为高电平，此时LED2亮 */
    if(LEDx & LED3)
      GPIO_SetBits(LED3_GPIO,LED3_GPIO_PIN);/* 设置引脚输出为高电平，此时LED3亮 */ 
  }
}

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
