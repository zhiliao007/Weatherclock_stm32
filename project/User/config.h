#ifndef _CONFIG_H_
#define _CONFIG_H_


#define XINZHI_WEATHER                           0               //心知天气
#define JISU_WEATHER                             0               //极速数据
#define CHINA_WEATHER                            1               //中国天气


#define User_ESP8266_ApSsid                      "Xiaomi_LAB"    //要连接的热点的名称
#define User_ESP8266_ApPwd                       "lab141516"     //要连接的热点的密钥

#define FLASH_ssidAddress                        0x807F000       // 用来保存wifi的ssid
#define FLASH_pwdAddress                         0x807F700       // 用来保存wifi的pwd
#define FLASH_flagAddress                        0x807FF00       // FLASH使用标志

#define User_ESP8266_TcpServer_Port              "80"            //要连接的服务器的端口

#endif /* _CONFIG_H_ */
