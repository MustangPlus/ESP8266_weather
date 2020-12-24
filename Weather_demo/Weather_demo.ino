#define BLINKER_WIFI
#include <Blinker.h>
#include <Arduino.h>
#include <Arduino_JSON.h>
#include <SPI.h>
#include <U8g2lib.h>
char auth[] = "0adc1a68317c";
char ssid[] = "PandoraBox-2.4G";
char pswd[] = "PASS9999";

char home_add[]="jilin"; //地址代码获取天气用
char* icon_index[3]={"A","C","E"};//图标在天气字库里分别代表 云，雨，晴
int sel_icon=0;
float humi_read, temp_read;
int wind_wifi,weather_wifi,wind_ang_wifi;
int temp_wifi=18;
bool first_weadher=true;

StaticJsonDocument<400> doc;

int row_i=15-1;//汉字坐标向下偏移一像素 坐标为左下角减一 高度=16(0-15)
//以此推算，12864可显示(128/16)*(64/16)=8列4行共32个汉字
//U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R0, /* CS=*/ 15, /* reset=*/ 16); // Feather HUZZAH ESP8266, E=clock=14, RW=data=13, RS=CS
//U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 14, /* data=*/ 13, /* CS=*/ 15, /* reset=*/ 16); // Feather HUZZAH ESP8266, E=clock=14, RW=data=13, RS=CS
U8G2_ST7567_OS12864_F_4W_SW_SPI u8g2(U8G2_R2, /* clock=*/ 14, /* data=*/ 13, /* cs=*/ 15, /* dc=*/ 4, /* reset=*/ 5);  
void dis_Time(){
  char str_week[]="";
  char str_date[]="";
  char str_big_Time[]="";
  char str_small_second[]=""; 
  int num_temp=0;
  char str_temp[6]; 
  if(Blinker.second()>=0){
    do {
//      system_soft_wdt_feed();
      u8g2.clearBuffer();
      num_temp=Blinker.year()%100;
      if(num_temp<10){
        strcat(str_date,"0");
      }
      itoa(num_temp,str_temp,10);   //日期
      strcat(str_date,str_temp);
      num_temp=Blinker.month();               
      itoa(num_temp,str_temp,10);
      strcat(str_date,"-");
      if(Blinker.month()<10){
        strcat(str_date,"0");
      }
      strcat(str_date,str_temp);
      num_temp=Blinker.mday();
      itoa(num_temp,str_temp,10);
      strcat(str_date,"-");
      strcat(str_date,str_temp);
      num_temp=Blinker.wday();
      char* week_str[8]={"日","一","二","三","四","五","六","日"};
      strcat(str_date,"星期");
      strcat(str_date,week_str[Blinker.wday()]);
      Serial.print("日期:");
      Serial.print(str_date);
      u8g2.setFont(u8g2_font_wqy12_t_gb2312a); // 汉字 14*26
      u8g2.setCursor(0, 54);
      u8g2.print(str_date);
      num_temp=Blinker.wday();    
      itoa(num_temp,str_temp,10);   //星期
      strcat(str_week,str_temp);
      num_temp=Blinker.hour();
      itoa(num_temp,str_temp,10);   //时分
      if(Blinker.hour()<10){
        strcat(str_big_Time,"0");
      }
      strcat(str_big_Time,str_temp);       
      num_temp=Blinker.minute();
      itoa(num_temp,str_temp,10);
      strcat(str_big_Time,":");
      if(Blinker.minute()<10){
        strcat(str_big_Time,"0");
      }
      strcat(str_big_Time,str_temp);
      Serial.print("时分:");
      Serial.print(str_big_Time);
      u8g2.setFont(u8g2_font_logisoso28_tf ); // 大字 14*26
      u8g2.setCursor(0, 34);
      u8g2.print(str_big_Time);
      num_temp=Blinker.second();
      itoa(num_temp,str_temp,10); //秒
      if(Blinker.second()<10){
        strcat(str_small_second,"0");
      }
      strcat(str_small_second,str_temp);
      Serial.print("秒:");
      Serial.println(str_small_second);
      u8g2.drawLine(88, 2, 88, 61);    //分割线
      int8_t sec = Blinker.second();
      int8_t minute = Blinker.minute();
      if((sec%10) > 5)// 每5秒更新一次参考地址：https://www.arduino.cn/forum.php ... B%E5%A4%A9%E6%B0%94
        {
          if((minute > 58) or (first_weadher=true))//每一小时获取一次天气信息，参考https://diandeng.tech/doc/weather
          {
            Blinker.weather();//默认查询设备ip所属地区的当前时刻的天气情况  
            first_weadher=false ; //开机第一次更新默认查询设备ip所属地区的当前时刻的天气情况 
          }
          if(weather_wifi == 0)
          {
            sel_icon=0;
          }
          else if (weather_wifi < 101)
          {
            sel_icon=2;
          }
          else if (weather_wifi < 201)
          {
            sel_icon=0;
          }
          else if (weather_wifi < 501)
          {
            sel_icon=1;
          }
        }
      //判断天气情况
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t ); // 天气图标 32*32
      u8g2.setCursor(96, 34);
      u8g2.print(icon_index[sel_icon]);
      u8g2.setFont(u8g2_font_wqy16_t_gb2312b ); // 大字 16*16
      u8g2.setCursor(96, 55);
      char b_b_tmp[25];
      itoa(temp_wifi, b_b_tmp, 10);
      strcat(b_b_tmp,"℃");
      u8g2.print(b_b_tmp);
     } while ( u8g2.nextPage() );
  }
}
void weatherData(const String & data)
{
    BLINKER_LOG("weather: ", data);
    DeserializationError error = deserializeJson(doc, data);
    if(error)
    {
     //显示获取失败
      return;
    }
    const char* cloud = doc["cloud"]; // "0"
    const char* cond_code = doc["cond_code"]; // "101"
    const char* cond_txt = doc["cond_txt"]; // ""
    const char* fl = doc["fl"]; // "31"
    const char* hum = doc["hum"]; // "81"
    const char* pcpn = doc["pcpn"]; // "0.0"
    const char* pres = doc["pres"]; // "997"
    const char* tmp = doc["tmp"]; // "28"
    const char* vis = doc["vis"]; // "16"
    const char* wind_deg = doc["wind_deg"]; // "159"
    const char* wind_dir = doc["wind_dir"]; // 风向
    const char* wind_sc = doc["wind_sc"]; // "2"
    const char* wind_spd = doc["wind_spd"]; // "9"  
    temp_wifi = atoi(tmp);//atoi是将字符型转化为数字 详见菜鸟教程：https://www.runoob.com/cprogramming/c-function-atoi.html
    wind_wifi = atoi(wind_spd);
    weather_wifi = atoi(cond_code);
    wind_ang_wifi = atoi(wind_deg);
}
void dataRead(const String & data)
{
    BLINKER_LOG("Blinker readString: ", data);
    uint32_t BlinkerTime = millis();
    Blinker.vibrate();        
    Blinker.print("millis", BlinkerTime);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}
void setup()
{
    ESP.wdtDisable();
    Serial.begin(9600);
    BLINKER_DEBUG.stream(Serial);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    Blinker.begin(auth, ssid, pswd);
    Blinker.attachData(dataRead);
    Blinker.attachWeather(weatherData);
    Blinker.setTimezone(8.0);
    u8g2.begin();   //选择U8G2模式，或者U8X8模式
    u8g2.setContrast(0);//设置对比度
    u8g2.enableUTF8Print(); //  UTF8 输出（汉字显示不是很好，勉强够用）
    Blinker.delay(3000);//此处预留开机动画位置
}
void loop()
{   
    Blinker.run();
    dis_Time();
    Blinker.delay(1000);
}
