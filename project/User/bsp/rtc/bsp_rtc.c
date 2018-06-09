/**
  ******************************************************************************
  * 文件名程: bsp_rtc.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 实时时钟底层驱动程序
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
#include "bsp/usart/bsp_debug_usart.h"
#include  "bsp/rtc/bsp_rtc.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/*星期，生肖用文字ASCII码*/
uint8_t const *WEEK_STR[] = {"日", "一", "二", "三", "四", "五", "六"};
uint8_t const *zodiac_sign[] = {"猪", "鼠", "牛", "虎", "兔", "龙", "蛇", "马", "羊", "猴", "鸡", "狗"};

/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
void Time_Adjust(struct rtc_time *tm);
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 配置RTC秒中断的主中断优先级为1，次优先级为0
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void RTC_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * 函数功能: 简单的延时函数
  * 输入参数: time：延时时间
  * 返 回 值: 无
  * 说    明：无
  */
static void Delay(uint16_t time)
{
  uint16_t i;
  while(--time)
  {
    for(i=0;i<2000;i++);
  }
}

/**
  * 函数功能: 配置RTC
  * 输入参数: 无
  * 返 回 值: 1：RTC配置成功或运行正常，0：RTC无法正常使用
  * 说    明：无
  */
static uint8_t RTC_Configuration(void)
{
  uint32_t count=0;
  
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);
	
	/* Reset Backup Domain */
	BKP_DeInit();
	
	/* Enable LSE */
	RCC_LSEConfig(RCC_LSE_ON);
	
	/* Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
     Delay(100);
     count++;
     if(count>4000)return 0;
  }
	
	/* Select LSE as RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);
	
	/* Wait for RTC registers synchronization 
	 * 因为RTC时钟是低速的，内环时钟是高速的，所以要同步
	 */
	RTC_WaitForSynchro();
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Enable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) = 1HZ */
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
  
  return 1;
}

/**
  * 函数功能: 检查并配置RTC
  * 输入参数: tm:用于读取RTC时间的结构体指针
  * 返 回 值: 无
  * 说    明：无
  */
uint8_t RTC_CheckAndConfig(struct rtc_time *tm)
{
  /* 配置RTC秒中断的主中断优先级 */
  RTC_NVIC_Config();
  
  /*在启动时检查备份寄存器BKP_DR1，如果内容不是0xA5A5,
	  则需重新配置时间并询问用户调整时间*/
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		printf("RTC not yet configured....\r\n");

		/* RTC Configuration */
		if(RTC_Configuration()!=1)
      return 0;
		
		printf("RTC configured....\r\n");

		/* Adjust time by users typed on the hyperterminal */
		Time_Adjust(tm);

		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}
	else
	{
	  /*启动无需设置新时钟*/
		/*检查是否掉电重启*/
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
		    printf("Power On Reset occurred....\r\n");
		}
		/*检查是否Reset复位*/
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
			printf("External Reset occurred....\r\n");
		}
	
		printf("No need to configure RTC....\r\n");
		
		/*等待寄存器同步*/
		RTC_WaitForSynchro();
		
		/*允许RTC秒中断*/
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		
		/*等待上次RTC寄存器写操作完成*/
		RTC_WaitForLastTask();
	}
	   /*定义了时钟输出宏，则配置校正时钟输出到PC13*/
	#ifdef RTCClockOutput_Enable
	  /* Enable PWR and BKP clocks */
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	  /* Allow access to BKP Domain */
	  PWR_BackupAccessCmd(ENABLE);
	
	  /* Disable the Tamper Pin */
	  BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
	                                 functionality must be disabled */
	
	  /* Enable RTC Clock Output on Tamper Pin */
	  BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
	#endif
	
	  /* Clear reset flags */
	  RCC_ClearFlag();
  
  return 1;
}

/**
  * 函数功能: 从串口调试助手获取数字值(把ASCII码转换为数字)
  * 输入参数: value 用户在超级终端中输入的数值
  * 返 回 值: 输入字符的ASCII码对应的数值
  * 说    明：本函数专用于RTC获取时间，若进行其它输入应用，要修改一下
  */
static uint8_t USART_Scanf(uint32_t value)
{
  uint32_t index = 0;
  uint32_t tmp[2] = {0, 0};

  while (index < 2)
  {
    /* Loop until RXNE = 1 */
    while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET)
    {
    }  
    tmp[index++] = (USART_ReceiveData(DEBUG_USARTx));
    if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))   /*数字0到9的ASCII码为0x30至0x39*/
    { 		  
      printf("Please enter valid number between 0 and 9 -->:  \r\n");
      index--; 		 
    }
  }
  
  /* 计算输入字符的ASCII码转换为数字*/
  index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
  
  /* Checks */
  if (index > value)
  {
    printf("Please enter valid number between 0 and %d\r\n", value);
    return 0xFF;
  }
  return index;
}

/**
  * 函数功能: 时间调整，保存在RTC时钟结构体
  * 输入参数: tm：用于读取RTC时间的结构体指针
  * 返 回 值: 无
  * 说    明：无
  */
static void Time_Regulate(struct rtc_time *tm)
{
#if 0
  tm->tm_year = 2015;
  tm->tm_mon  =   10;
  tm->tm_mday =    4;	
  tm->tm_hour =   10;	 
  tm->tm_min  =   15;	 
  tm->tm_sec  =   46;
#else
  u32 Tmp_YY = 0xFF, Tmp_MM = 0xFF, Tmp_DD = 0xFF, Tmp_HH = 0xFF, Tmp_MI = 0xFF, Tmp_SS = 0xFF;

  printf("=========================Time Settings==================\r\n");

  printf("请输入年份(Please Set Years):  20\r\n");

  while (Tmp_YY == 0xFF)
  {
    Tmp_YY = USART_Scanf(99);
  }

  printf("年份被设置为:  20%0.2d\r\n", Tmp_YY);

  tm->tm_year = Tmp_YY+2000;

  Tmp_MM = 0xFF;

  printf("请输入月份(Please Set Months):  \r\n");

  while (Tmp_MM == 0xFF)
  {
    Tmp_MM = USART_Scanf(12);
  }

  printf("月份被设置为:  %d\r\n", Tmp_MM);

  tm->tm_mon= Tmp_MM;

  Tmp_DD = 0xFF;

  printf("请输入日期(Please Set Dates):  \r\n");

  while (Tmp_DD == 0xFF)
  {
    Tmp_DD = USART_Scanf(31);
  }

  printf("日期被设置为:  %d\r\n", Tmp_DD);

  tm->tm_mday= Tmp_DD;

  Tmp_HH  = 0xFF;

  printf("请输入时钟(Please Set Hours):  \r\n");

  while (Tmp_HH == 0xFF)
  {
    Tmp_HH = USART_Scanf(23);
  }

  printf("时钟被设置为:  %d\r\n", Tmp_HH );

  tm->tm_hour= Tmp_HH;
    
  Tmp_MI = 0xFF;

  printf("请输入分钟(Please Set Minutes):  \r\n");

  while (Tmp_MI == 0xFF)
  {
    Tmp_MI = USART_Scanf(59);
  }

  printf("分钟被设置为:  %d\r\n", Tmp_MI);

  tm->tm_min= Tmp_MI;

  Tmp_SS = 0xFF;

  printf("请输入秒钟(Please Set Seconds):  \r\n");

  while (Tmp_SS == 0xFF)
  {
    Tmp_SS = USART_Scanf(59);
  }

  printf("秒钟被设置为:  %d\r\n", Tmp_SS);

  tm->tm_sec= Tmp_SS;  
#endif
}

/**
  * 函数功能: 时间调节实现
  * 输入参数: tm：用于读取RTC时间的结构体指针
  * 返 回 值: 无
  * 说    明：无
  */
void Time_Adjust(struct rtc_time *tm)
{
	  /* Wait until last write operation on RTC registers has finished */
	  RTC_WaitForLastTask();
	
	  /* Get time entred by the user on the hyperterminal */
	  Time_Regulate(tm);
	  
	  /* Get wday */
	  GregorianDay(tm);

	  /* 修改当前RTC计数寄存器内容 */
	  RTC_SetCounter(mktimev(tm));

	  /* Wait until last write operation on RTC registers has finished */
	  RTC_WaitForLastTask();
}

/**
  * 函数功能: 显示当前时间值
  * 输入参数: TimeVar RTC计数值，单位为 s，tm：用于读取RTC时间的结构体指针
  * 返 回 值: 无
  * 说    明：无
  */
void Time_Display(uint32_t TimeVar,struct rtc_time *tm)
{
   static uint8_t FirstDisplay = 1;
   uint32_t BJ_TimeVar;
   uint8_t str[15]; // 字符串暂存  	

   /*  把标准时间转换为北京时间*/
   BJ_TimeVar =TimeVar + 8*60*60;

   to_tm(BJ_TimeVar, tm);/*把定时器的值转换为北京时间*/	

  if((!tm->tm_hour && !tm->tm_min && !tm->tm_sec)  || (FirstDisplay))
  {
      
    GetChinaCalendar((u16)tm->tm_year, (u8)tm->tm_mon, (u8)tm->tm_mday, str);	
      printf("今天新历：%0.2d%0.2d,%0.2d,%0.2d\r\n", str[0], str[1], str[2],  str[3]);

    GetChinaCalendarStr((u16)tm->tm_year,(u8)tm->tm_mon,(u8)tm->tm_mday,str);
      printf("今天农历：%s\r\n", str);

   if(GetJieQiStr((u16)tm->tm_year, (u8)tm->tm_mon, (u8)tm->tm_mday, str))
      printf("今天农历：%s\r\n", str);

    FirstDisplay = 0;
  }	 	  	
  /* 输出时间戳，公历时间 */
  printf(" UNIX时间戳 = %d 当前时间为: %d年(%s年) %d月 %d日 (星期%s)  %0.2d:%0.2d:%0.2d\r\r\n",TimeVar,
                    tm->tm_year, zodiac_sign[(tm->tm_year-3)%12], tm->tm_mon, tm->tm_mday, 
                    WEEK_STR[tm->tm_wday], tm->tm_hour, 
                    tm->tm_min, tm->tm_sec);
}

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
