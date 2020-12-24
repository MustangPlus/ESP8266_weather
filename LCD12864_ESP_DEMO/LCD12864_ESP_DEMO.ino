#define BLINKER_WIFI
#include <Arduino.h>
#include <U8g2lib.h>
#include <DHT.h>
#include <Blinker.h>



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

char auth[] = "0adc1a68317c";
char ssid[] = "PandoraBox-2.4G";
char pswd[] = "PASS9999";

void weatherData(const String & data)
{
  BLINKER_LOG("weather: ", data);
}

void weatherForecastData(const String & data)
{
  BLINKER_LOG("weather: ", data);
}

void dataRead(const String & data)
{
  BLINKER_LOG("Blinker readString: ", data);
  
  uint32_t BlinkerTime = millis();
  
  Blinker.vibrate();        
  Blinker.print("millis", BlinkerTime);
  
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  Blinker.weather();
  Blinker.weatherForecast();
  
  Blinker.delay(60000);
}

void setup() {
  Serial.begin(115200);
  BLINKER_DEBUG.stream(Serial);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  Blinker.begin(auth, ssid, pswd);
  Blinker.attachData(dataRead);
  Blinker.attachWeather(weatherData);
  Blinker.attachWeatherForecast(weatherForecastData);
  
  u8g2.begin();
  u8g2.setContrast(0);//设置对比度
  dht.begin();
  u8g2.enableUTF8Print();//使arduino支持UTF8输出
}

void loop() {
  Blinker.run();
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
