#include <Arduino.h>
#include <U8g2lib.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

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
#define MAX_CONTENT_SIZE  1000                 // max size of the HTTP response
#define _ssid "PandoraBox-2.4G"         // XXXXXX -- 使用时请修改为当前你的 wifi ssid
#define _password "PASS9999"         // XXXXXX -- 使用时请修改为当前你的 wifi 密码

const char* host = "api.seniverse.com";
const char* APIKEY = "SSzLfHYg1ahOPy9Zx";        //API KEY
const char* city = "zhuhai";
const char* language = "zh-Hans&unit=c";//zh-Hans 简体中文  会显示乱码 zh-Hans&unit=c?
char endOfHeaders[] = "\r\n\r\n";// 跳过 HTTP 头的标志
char response[MAX_CONTENT_SIZE];//存放服务器响应消息

WiFiClient client;//构造/类实例？C++内容

U8G2_ST7567_OS12864_F_4W_SW_SPI u8g2(U8G2_R2, /* clock=*/ 14, /* data=*/ 13, /* cs=*/ 15, /* dc=*/ 4, /* reset=*/ 5); //构造/类实例？C++内容 

DHT dht(DHTPIN, DHTTYPE);//构造/类实例？C++内容

bool sendRequest(const char* host, const char* city, const char* apiKey, const char* language);//发送请求消息
bool skipResponseHeaders();// 跳过 HTTP 头，使我们在响应正文的开头
void readReponseContent(char* content, size_t maxSize);//读取响应消息正文
void clrEsp8266ResponseBuffer(char* content);

void setup(void) {
  /****************wifi部分****************/
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
  /*发送请求*/
  if (sendRequest(host, city, APIKEY,language) && skipResponseHeaders()) {
    response[0]='2';
    Serial.println(response[0]);
    clrEsp8266ResponseBuffer(response);
    readReponseContent(response, sizeof(response));
  }
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

bool sendRequest(const char* host, const char* city, const char* apiKey, const char* language) {
  // We now create a URI for the request
  //心知天气
  String GetUrl = "/v3/weather/now.json?key=";
  GetUrl += apiKey;
  GetUrl += "&location=";
  GetUrl += city;
  GetUrl += "&language=";
  GetUrl += language;
  // This will send the request to the server
  client.print(String("GET ") + GetUrl + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

//  client.print("https://api.seniverse.com/v3/weather/now.json?key=smtq3n0ixdggurox&location=beijing&language=zh-Hans&unit=c");
  Serial.println("create a request:");
  Serial.println(String("GET ") + GetUrl + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(100);
  return true;
}

bool skipResponseHeaders() {
  // HTTP headers end with an empty line
  bool ok = client.find(endOfHeaders);
  if (!ok) {
    Serial.println("No response or invalid response!");
    Serial.println(ok);
  }
  return ok;
}

void readReponseContent(char* content,size_t maxSize) {
  while (client.available()) {
    char ch = (client.read());
    Serial.print(ch);
  }
//  size_t length = client.peekBytes(content, maxSize);
//  delay(100);
  Serial.println("Get the data from Internet!");
//  while(client.available())
//      {
//        String line=client.readStringUntil('\r');
//        Serial.print(line);
//        }
//  content[length] = 0;
//  Serial.println(content);
  Serial.println("Read data Over!");
//  client.flush();//这句代码需要加上  不然会发现每隔一次client.find会失败
}

//bool parseUserData(char* content, struct UserData* userData) {
////    -- 根据我们需要解析的数据来计算JSON缓冲区最佳大小
////   如果你使用StaticJsonBuffer时才需要
////    const size_t BUFFER_SIZE = 1024;
////   在堆栈上分配一个临时内存池
////    StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
////    -- 如果堆栈的内存池太大，使用 DynamicJsonBuffer jsonBuffer 代替
//
//  JsonObject& root = jsonBuffer.parseObject(content);
//
//  if (!root.success()) {
//    Serial.println("JSON parsing failed!");
//    return false;
//  }
// 
//
//  //复制我们感兴趣的字符串
//  strcpy(userData->city, root["results"][0]["location"]["name"]);
//  strcpy(userData->weather, root["results"][0]["now"]["text"]);
//  strcpy(userData->temp, root["results"][0]["now"]["temperature"]);
//  strcpy(userData->udate, root["results"][0]["last_update"]);
//  //  -- 这不是强制复制，你可以使用指针，因为他们是指向“内容”缓冲区内，所以你需要确保
//  //   当你读取字符串时它仍在内存中
//  return true;
//}
// 
//
//// 打印从JSON中提取的数据
//void printUserData( struct UserData* userData) {
//  Serial.println("Print parsed data :");
//  display.print("City: ",0,0);
//  display.print(userData->city,0,6);
//  display.print("Weat: ",2,0);
//  display.print(userData->weather,2,6);
//  display.print("Temp: ",4,0);
//  display.print(userData->temp,4,6);
//// display.print("Last Updata : ",7,2);
//display.print(userData->udate,6,0);
//// display.print("\r\n");
//delay(4*1000);
//}
// 
//
//// 关闭与HTTP服务器连接
//void stopConnect() {
//  Serial.println("Disconnect");
//  client.stop();
//}

void clrEsp8266ResponseBuffer(char* content){
    memset(content, 0, sizeof(content));      //清空
    Serial.println(content[0]);
}
 
