/**
  ******************************************************************************
  * 文件名称: main.c 
  * 作    者: 李文晴、乔启鸣
  * 邮    箱: 
  * 版    本: V2.2.2
  * 编写日期: 2018-05-22
  * 功    能: stm32网络查询天气
  ******************************************************************************
  */

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f10x.h"
#include "bsp/usart/bsp_debug_usart.h"
#include "bsp/systick/bsp_SysTick.h"
#include "bsp/ESP8266/bsp_esp8266.h"
#include "bsp/syn6288/bsp_syn6288.h"
#include "bsp/sram/bsp_sram.h"	
#include "bsp/malloc/bsp_malloc.h"	 

#include <string.h>
#include "cjson/cJSON.h"
#include "gzip/zipmem.h"

/* 私有类型定义 --------------------------------------------------------------*/
typedef struct _CityWeather {
    char *date;
    char *city;
    char *high_tem;
    char *low_tem;
	char *wind_scale;
}CityWeather;

/* 私有宏定义 ----------------------------------------------------------------*/
#define XINZHI_WEATHER            0  //心知天气
#define JISU_WEATHER              0  //极速数据
#define CHINA_WEATHER             1  //中国天气


#define User_ESP8266_ApSsid                       "Xiaomi_LAB"              //要连接的热点的名称
#define User_ESP8266_ApPwd                        "lab141516"           //要连接的热点的密钥

#define User_ESP8266_TcpServer_Port               "80"                 //要连接的服务器的端口

/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
extern __IO uint8_t ucTcpClosedFlag;
extern int SwitchToGbk(const unsigned char* pszBufIn, int nBufInLen, unsigned char* pszBufOut, int* pnBufOutLen);

/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

//添加你的key替换yourkey

#if JISU_WEATHER

#define User_ESP8266_TcpServer_IP                 "api.jisuapi.com"
char cStr [100] = {"GET http://api.jisuapi.com/weather/query?appkey=yourkey&citycode=101280601\r\n"}; 

void parsingJSON(const char* str, CityWeather* weather)
{
    cJSON* root=cJSON_Parse(str);
    cJSON* results=cJSON_GetObjectItem(root,"results");
	cJSON* block=cJSON_GetArrayItem(results,0);
	
    /* 城市 */	
	cJSON* location=cJSON_GetObjectItem(block,"location");
	cJSON* name=cJSON_GetObjectItem(location,"name");
    weather->city = name->valuestring;

	/* 日期 */	
	cJSON* daily=cJSON_GetObjectItem(block,"daily");
	cJSON* today=cJSON_GetArrayItem(daily,0);
	cJSON* date=cJSON_GetObjectItem(today,"date");
    weather->date = date->valuestring;
	
	/* 最高温度 */
	cJSON* high=cJSON_GetObjectItem(today,"high");
    weather->high_tem = high->valuestring;

	/* 最低温度 */
	cJSON* low=cJSON_GetObjectItem(today,"low");
    weather->low_tem = low->valuestring;
	
	/* 风力等级 */
	cJSON* wind_scale=cJSON_GetObjectItem(today,"wind_scale");
    weather->wind_scale = wind_scale->valuestring;
	
    cJSON_Delete(root);

}

#endif

#if CHINA_WEATHER

#define User_ESP8266_TcpServer_IP                 "wthrcdn.etouch.cn"
char cStr [80] = {"GET http://wthrcdn.etouch.cn/weather_mini?citykey=101250201\r\n"};
char aucOut[400];

void parsingJSON(const char* str, CityWeather* weather)
{
	cJSON* root=cJSON_Parse(str);
	cJSON* data=cJSON_GetObjectItem(root,"data");
	
	/* 城市 */	
	cJSON* city=cJSON_GetObjectItem(data,"city");
    weather->city = city->valuestring;
	
	/* 日期 */ 
	cJSON* forecast=cJSON_GetObjectItem(data,"forecast");
	cJSON* today=cJSON_GetArrayItem(forecast,0);
	cJSON* date=cJSON_GetObjectItem(today,"date");
    weather->date = date->valuestring;
		
	/* 最高温度 */
	cJSON* high=cJSON_GetObjectItem(today,"high");
    weather->high_tem = high->valuestring;

	/* 最低温度 */
	cJSON* low=cJSON_GetObjectItem(today,"low");
    weather->low_tem = low->valuestring;
	
	/* 风力等级 */
	cJSON* fengli=cJSON_GetObjectItem(today,"fengli");
    weather->wind_scale = fengli->valuestring;
	
	cJSON_Delete(root);
}

#endif

#if XINZHI_WEATHER

#define User_ESP8266_TcpServer_IP                 "api.seniverse.com"     
char cStr [131] = {"GET https://api.seniverse.com/v3/weather/daily.json?key=yourkey&location=xiangtan&language=zh-Hans&unit=c&start=0&days=5\r\n"};

void parsingJSON(const char* str, CityWeather* weather)
{
	cJSON* root=cJSON_Parse(str);
    cJSON* result=cJSON_GetObjectItem(root,"result");
	
	/* 城市 */	
	cJSON* city=cJSON_GetObjectItem(result,"city");
    weather->city = city->valuestring;
	    
	cJSON_Delete(root);
}

#endif

/**
  * 函数功能: 主函数.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
int main(void)
{ 
	uint8_t ucStatus;  
	CityWeather ctweather;
	
	/* 调试串口初始化配置，115200-N-8-1.使能串口发送和接受 */
	DEBUG_USART_Init();  
	
	FSMC_SRAM_Init();			//初始化外部SRAM  
	my_mem_init(SRAMIN);		//初始化内部内存池
	my_mem_init(SRAMEX);		//初始化外部内存池
	
	SysTick_Init();

	SYN6288_Init();
		
	ESP8266_Init();
	SYN6288_Play("[v8]系统初始化成功，正在配置ESP8266");
	printf("\r\n姝ｅ湪閰嶇疆 ESP8266 ......\r\n" );

	if(ESP8266_AT_Test())
	{
		printf("AT test OK\r\n");
	}
	printf("\r\n< 1 >\r\n");
	if(ESP8266_Net_Mode_Choose(STA))
	{
		printf("ESP8266_Net_Mode_Choose OK\r\n");
	}  
	printf("\r\n< 2 >\r\n");
	while(!ESP8266_JoinAP(User_ESP8266_ApSsid,User_ESP8266_ApPwd));		
	printf("\r\n< 3 >\r\n");
	ESP8266_Enable_MultipleId(DISABLE);	
	while(!ESP8266_Link_Server(enumTCP,User_ESP8266_TcpServer_IP,User_ESP8266_TcpServer_Port,Single_ID_0));	
	printf("\r\n< 4 >\r\n");
	while(!ESP8266_UnvarnishSend());	
	printf("閰嶇疆 ESP8266 瀹屾瘯\r\n");

	SYN6288_Play("[v8]ESP8266配置成功，正在查询天气");
	/* 无限循环 */
	while (1)
	{
		/* 发送GET请求 */
		while (!ESP8266_SendString(ENABLE,cStr,131,Single_ID_0)); 
		/* 获取天气数据包 */
		char *httpData = ESP8266_ReceiveString(ENABLE);
		
		
	#if CHINA_WEATHER	  		
		/* gzip解压原始数据 */
		unzipmem(httpData, strEsp8266_Fram_Record .InfBit .FramLength, aucOut); 
		/* 解包JSON数据获取天气数据 */
		parsingJSON((const char*)aucOut, &ctweather);
	#else
		/* 解包JSON数据获取天气数据 */
		parsingJSON((const char*)httpData, &ctweather);
	#endif
		
		printf("city = %s\r\n",ctweather.city);
		printf("date = %s\r\n",ctweather.date);
		printf("high_tem = %s\r\n",ctweather.high_tem);
		printf("low_tem = %s\r\n",ctweather.low_tem);
		printf("wind_scale = %s\r\n",ctweather.wind_scale);
		
		Delay(8000);
		
		uint8_t GBK_city[20];
		uint8_t GBK_date[20];
		uint8_t GBK_high[20];
		uint8_t GBK_low[20];
		uint8_t GBK_wind[20];
		int num;
		char strl[300];
		SwitchToGbk((const unsigned char *)ctweather.city, strlen(ctweather.city),GBK_city,&num);
		SwitchToGbk((const unsigned char *)ctweather.date, strlen(ctweather.date),GBK_date,&num);
		SwitchToGbk((const unsigned char *)ctweather.high_tem, strlen(ctweather.high_tem),GBK_high,&num);
		SwitchToGbk((const unsigned char *)ctweather.low_tem, strlen(ctweather.low_tem),GBK_low,&num);
		SwitchToGbk((const unsigned char *)ctweather.wind_scale, strlen(ctweather.wind_scale),GBK_wind,&num);
		sprintf(strl,"[v8]%s，%s今天最高温度%s，最低温度%s，风力指数%s",GBK_date,GBK_city,GBK_high,GBK_low,GBK_wind);
		SYN6288_Play(strl);
		
		while(1);
		
		#pragma diag_suppress 111
		if(ucTcpClosedFlag)                                             //检测是否失去连接
		{
			ESP8266_ExitUnvarnishSend();                                    //退出透传模式			
			do ucStatus = ESP8266_Get_LinkStatus();                         //获取连接状态
			while(!ucStatus);			
			if(ucStatus==4)                                             //确认失去连接后重连
			{
				printf("姝ｅ湪閲嶈繛鐑偣鍜屾湇鍔″櫒 ......\r\n");				
				while(!ESP8266_JoinAP(User_ESP8266_ApSsid,User_ESP8266_ApPwd));				
				while(!ESP8266_Link_Server(enumTCP,User_ESP8266_TcpServer_IP,User_ESP8266_TcpServer_Port,Single_ID_0));				
				printf("閲嶈繛鐑偣鍜屾湇鍔″櫒鎴愬姛!!!\r\n");
			}			
			while(!ESP8266_UnvarnishSend());					
		}
	}
}

/******************* (C) COPYRIGHT 2014-2019 学生开放实验室 *****END OF FILE****/
