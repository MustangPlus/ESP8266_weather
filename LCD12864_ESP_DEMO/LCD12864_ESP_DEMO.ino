#include <Arduino.h>
#include <U8g2lib.h>
#include <DHT.h>
#include <ESP8266WiFi.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define DHTPIN 0 
#define DHTTYPE    DHT11
#define BAUD_RATE 115200                   // serial connection speed
#define HTTP_TIMEOUT  5000           // max respone time from server
#define _ssid "PandoraBox-2.4G"         // XXXXXX -- 使用时请修改为当前你的 wifi ssid
#define _password "PASS9999"         // XXXXXX -- 使用时请修改为当前你的 wifi 密码
#define GETUrl  "GET "\
                "/v3/weather/daily.json?"\
                "key=SSzLfHYg1ahOPy9Zx"\
                "&location=zhuhai"\
                "&language=zh-Hans&unit=c"\
                "&start=0&days=1"\
                " HTTP/1.1\r\n"\
                "Host: ""api.seniverse.com""\r\n"\
                "Connection: close\r\n\r\n"
                ///v3/weather/daily.json?,请求网站地址
                //key=，密钥
                //&location=，地址
                //&language=，语言
                //&start=0&days=1，预报时间
                //api.seniverse.com，心知天气网站
                
char* host = "api.seniverse.com";

WiFiClient client;//构造/类实例？C++内容

U8G2_ST7567_OS12864_F_4W_SW_SPI u8g2(U8G2_R2, /* clock=*/ 14, /* data=*/ 13, /* cs=*/ 15, /* dc=*/ 4, /* reset=*/ 5); //构造/类实例？C++内容 

DHT dht(DHTPIN, DHTTYPE);//构造/类实例？C++内容

bool sendRequest(char* host, char* city, char* apiKey, char* language);//发送请求消息
void readReponseContent();//读取响应消息的字符串数组指针
//提取正文

void setup(void) {
  /****************Wifi连接部分****************/
  WiFi.mode(WIFI_STA);
  Serial.begin(BAUD_RATE);
  WiFi.begin(_ssid, _password);
  while (WiFi.status() != WL_CONNECTED) {
    //这个函数是wifi连接状态，返回wifi链接状态
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  delay(100);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());//WiFi.localIP()返回8266获得的ip地址
  client.setTimeout(HTTP_TIMEOUT);
  /*连接host地址*/
  while (!client.connected()){
     if (!client.connect(host, 80)){
         Serial.println("host connection....");
         delay(100);
     }
  }
  /*发送请求消息*/
  client.print(GETUrl);
  /*读取响应消息*/
  delay(100);
  int number = client.available();
  char Reponse[client.available()+1];//创建字符数组,加1是补'\0'转换成字符串数组
  memset(Reponse,0,client.available()+1);//清除数组内容
  readReponseContent(Reponse);
  Serial.print("a:");
  Serial.println(Reponse[number-1]);
  /****************液晶显示部分****************/
  u8g2.begin();
  u8g2.setContrast(0);//设置对比度
  u8g2.enableUTF8Print();//使arduino支持UTF8输出
  /****************温湿度获取部分****************/
  dht.begin();
}

void loop(void) {
  u8g2.clearBuffer();
  u8g2.setFontDirection(0);//设置字体方向
  u8g2.clearBuffer();//清空缓存区
  float h = dht.readHumidity();//获取湿度
  float t = dht.readTemperature();//获取温度
  u8g2.setCursor(0, 20);
  u8g2.setFont(u8g2_font_wqy16_t_gb2312b);
  u8g2.print("温度:");
  u8g2.setFont(u8g2_font_crox5tb_tf);
  u8g2.print(((String)t+"°C"));
  u8g2.setCursor(0, 50);
  u8g2.setFont(u8g2_font_wqy16_t_gb2312b);
  u8g2.print("湿度:");
  u8g2.setFont(u8g2_font_crox5tb_tf);
  u8g2.print(((String)h+"%"));
  u8g2.sendBuffer();//写入缓存区
  delay(1000);//显示1秒
}

void readReponseContent(char Reponse[]) {
  int idex = 0;
  Serial.println("Get the data from Internet!");
  while(client.available()){
    Reponse[idex] = client.read();
    idex++;
  }
  Reponse[idex] = '\0';//字符数组转字符串数组
  Serial.println(Reponse);  
  Serial.println("Read data Over!");
}
