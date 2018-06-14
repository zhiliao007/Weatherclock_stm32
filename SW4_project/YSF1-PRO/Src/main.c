/**
  ******************************************************************************
  * 文件名称: main.c
  * 作    者: 李文晴、乔启鸣
  * 邮    箱:
  * 版    本: V2.3.1
  * 编写日期: 2018-05-22
  * 功    能: stm32网络查询天气
  ******************************************************************************
  */

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f10x.h"
#include "usart/bsp_debug_usart.h"
#include "systick/bsp_SysTick.h"
#include "ESP8266/bsp_esp8266.h"
#include "syn6288/bsp_syn6288.h"
#include "spi_flash/bsp_spi_flash.h"
#include "stmflash/stm_flash.h"
#include "sram/bsp_sram.h"
#include "malloc/bsp_malloc.h"
#include "config.h"

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

/* 私有变量 ------------------------------------------------------------------*/
CityWeather ctweather;
const char g_ssid[]={User_ESP8266_ApSsid};
const char g_pwd[]={User_ESP8266_ApPwd};

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
char aucOut[1024] __attribute__ ((section (".exramtext")));

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

void setESP8266STAMode(char *ssid, char *pwd)
{
	char espPN[] = {"1234"};
	char espTO[] = {"1000"};

	printf("\r\n姝ｅ湪閰嶇疆 ESP8266 ......\r\n" );

	if(ESP8266_AT_Test())
	{
		printf("AT test OK\r\n");
	}
	printf("\r\n< 1 >\r\n");
	Delay(200);
	if(ESP8266_Net_Mode_Choose(STA))
	{
		printf("ESP8266_Net_Mode_Choose OK\r\n");
	}
	printf("\r\n< 2 >\r\n");
	while(!ESP8266_JoinAP(ssid,pwd));
	printf("\r\n< 3 >\r\n");

#if 1
	ESP8266_Enable_MultipleId(DISABLE);
	while(!ESP8266_Link_Server(enumTCP,User_ESP8266_TcpServer_IP,User_ESP8266_TcpServer_Port,Single_ID_0));
	printf("\r\n< 4 >\r\n");
	while(!ESP8266_UnvarnishSend());
	printf("閰嶇疆 ESP8266 瀹屾瘯\r\n");
#else
	ESP8266_Enable_MultipleId(ENABLE);
	while(!ESP8266_StartOrShutServer(ENABLE,espPN,espTO));
	printf("閰嶇疆 ESP8266 瀹屾瘯\r\n");
	while(1);
#endif
}

void setESP8266APMode(void)
{
	char espSSID[] = {"esp8266"};
	char espPWD[] = {"esp82666"};
	char espPN[] = {"1234"};
	char espTO[] = {"1000"};
	printf("\r\n姝ｅ湪閰嶇疆 ESP8266 ......\r\n" );

	if(ESP8266_AT_Test())
	{
		printf("AT test OK\r\n");
	}
	printf("\r\n< 1 >\r\n");
	if(ESP8266_Net_Mode_Choose(AP))
	{
		printf("ESP8266_Net_Mode_Choose OK\r\n");
	}
	printf("\r\n< 2 >\r\n");
	while(!ESP8266_BuildAP(espSSID,espPWD,WPA_WPA2_PSK));
	printf("\r\n< 3 >\r\n");
	ESP8266_Enable_MultipleId(ENABLE);
	while(!ESP8266_StartOrShutServer(ENABLE,espPN,espTO));
	printf("閰嶇疆 ESP8266 瀹屾瘯\r\n");
}

void dataToGBK(CityWeather *weather,char * buff)
{
	uint8_t GBK_city[20];
	uint8_t GBK_date[20];
	uint8_t GBK_high[20];
	uint8_t GBK_low[20];
	uint8_t GBK_wind[20];
	int num;

	SwitchToGbk((const unsigned char *)weather->city, strlen(weather->city),GBK_city,&num);
	SwitchToGbk((const unsigned char *)weather->date, strlen(weather->date),GBK_date,&num);
	SwitchToGbk((const unsigned char *)weather->high_tem, strlen(weather->high_tem),GBK_high,&num);
	SwitchToGbk((const unsigned char *)weather->low_tem, strlen(weather->low_tem),GBK_low,&num);
	GBK_high[7] = '\0';
	GBK_low[7] = '\0';
	GBK_wind[0] = (weather->wind_scale)[9];
	GBK_wind[1] = (weather->wind_scale)[10];
	GBK_wind[2] = (weather->wind_scale)[11];
	GBK_wind[3] = '\0';
	sprintf(buff,"6月%s，%s今天%s度，%s度，风力指数%s级",GBK_date,GBK_city,GBK_high,GBK_low,GBK_wind);

}


#define FLASHFLAG 0xCCCC
uint16_t g_flashFlag = FLASHFLAG;

/**
  * 函数功能: 主函数.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
int main(void)
{
	uint8_t ucStatus;
	char buff[300];
	uint16_t flashFlag;
	uint8_t ssidBuff[50];
	uint8_t pwdBuff[50];

	/* 初始化调试串口，115200-N-8-1. */
	DEBUG_USART_Init();
	printf("hello ");
	/* 初始化系统滴答定时器 */
	SysTick_Init();

	/* 初始化内存管理 */
	//FSMC_SRAM_Init();
	my_mem_init(SRAMIN);
	my_mem_init(SRAMEX);

	/* 初始化SYN6288语音播报模块 */
	SYN6288_Init();

	/* 初始化ESP8266网络模块 */
	ESP8266_Init();


	SYN6288_Play("[d][o0][v10][t5]");
	Delay(200);
	SYN6288_Play("系统初始化成功，正在配置ESP8266");
	Delay(8000);
	/* 从FLASH中读取密码 */
	STMFLASH_Read(FLASH_flagAddress,&flashFlag,1);
	if(flashFlag == FLASHFLAG)
	{
		SYN6288_Play("正在读取保存的歪滥账号密码");
		/* 读取WIFI账号密码 */
		STMFLASH_Read(FLASH_ssidAddress,(uint16_t *)ssidBuff,25);
		STMFLASH_Read(FLASH_pwdAddress,(uint16_t *)pwdBuff,25);
	}
	else
	{
		SYN6288_Play("初次使用正在设置账号密码");
		setESP8266APMode();
		SYN6288_Play("配置AP模式成功,请连接到esp8266并输入账号密码");
		ESP8266_ReceiveString(ENABLE);
		SYN6288_Play("连接成功");
		sprintf((char *)ssidBuff,"%s",ESP8266_ReceiveString(ENABLE));
		SYN6288_Play("账号为");
		Delay(3000);
		SYN6288_Play((const char *)(ssidBuff+12));         //12 and 11 is offest address
		sprintf((char *)pwdBuff,"%s",ESP8266_ReceiveString(ENABLE));
		SYN6288_Play("密码为");
		Delay(3000);
		SYN6288_Play((const char *)(pwdBuff+11));
		Delay(5000);
		/* 向内部Flash写入初始密码 */
		STMFLASH_Write(FLASH_ssidAddress,(uint16_t *)(ssidBuff+12),(strlen((const char *)ssidBuff)-12)/2+1);
		STMFLASH_Write(FLASH_pwdAddress,(uint16_t *)(pwdBuff+11),(strlen((const char *)pwdBuff)-11)/2+1);
		STMFLASH_Write(FLASH_flagAddress,&g_flashFlag,1);
		SYN6288_Play("配置完成，请复位重启系统");
		while(1);
	}

	/* 配置ESP8266入网 */
	setESP8266STAMode((char *)ssidBuff,(char *)pwdBuff);
	SYN6288_Play("ESP8266配置成功，正在查询天气");


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

	/*printf("city = %s\r\n",ctweather.city);
	printf("date = %s\r\n",ctweather.date);
	printf("high_tem = %s\r\n",ctweather.high_tem);
	printf("low_tem = %s\r\n",ctweather.low_tem);
	printf("wind_scale = %s\r\n",ctweather.wind_scale);*/
	Delay(8000);

	/* 将天气数据编码转为GBK */
	dataToGBK(&ctweather,buff);

	/* 播放天气 */
	SYN6288_Play(buff);

	Delay(10000);
	while(1)
	{
		/* 断线重连 */
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
