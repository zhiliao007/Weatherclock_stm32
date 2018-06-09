#ifndef __RTC_H__
#define	__RTC_H__

#include "stm32f10x.h"
#include "bsp/rtc/bsp_calendar.h"
#include "bsp/rtc/bsp_date.h"

/* 如果定义了下面这个宏的话,PC13就会输出频率为RTC Clock/64的时钟 */   
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */

uint8_t RTC_CheckAndConfig(struct rtc_time *tm);

void Time_Display(uint32_t TimeVar,struct rtc_time *tm);

#endif /* __RTC_H__ */

