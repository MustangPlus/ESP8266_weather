#include <Arduino.h>
#include <U8g2lib.h>
#include <DHT.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define DHTPIN 0 
#define DHTTYPE    DHT11

U8G2_ST7567_OS12864_F_4W_SW_SPI u8g2(U8G2_R2, /* clock=*/ 14, /* data=*/ 13, /* cs=*/ 15, /* dc=*/ 4, /* reset=*/ 5);  
DHT dht(DHTPIN, DHTTYPE);

void setup(void) {
  u8g2.begin();
  u8g2.setContrast(0);//设置对比度
  dht.begin();
  u8g2.enableUTF8Print();//使arduino支持UTF8输出
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
