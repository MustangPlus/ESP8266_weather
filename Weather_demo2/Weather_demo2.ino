#define BLINKER_WIFI

#include <ArduinoJson.h>

#include <Blinker.h>

 

char auth[] = "0adc1a68317c";

char ssid[] = "PandoraBox-2.4G";

char pswd[] = "PASS9999";

String adress = "guangdong-zhuhaishi";

 


void weatherData(const String & data)

{
    BLINKER_LOG("weather: ", data);

    StaticJsonDocument<400> doc;

    DeserializationError error = deserializeJson(doc, data);

    if (error)

    {
        Serial.print("ERROR IS:");

        Serial.println(error.c_str());

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

    const char* wind_dir = doc["wind_dir"]; //

    const char* wind_sc = doc["wind_sc"]; // "2"

    const char* wind_spd = doc["wind_spd"]; // "9"  

    Serial.print("风向：");

    Serial.println(wind_dir);

    Serial.print("天气：");

    Serial.println(cond_txt);

}

 

 

 

 


void setup()

{
    Serial.begin(115200);

    BLINKER_DEBUG.stream(Serial);

    pinMode(LED_BUILTIN, OUTPUT);
  
    digitalWrite(LED_BUILTIN, LOW);

    Blinker.begin(auth, ssid, pswd);

    // Blinker.attachData(dataRead);

    Blinker.attachWeather(weatherData);
    char weather1[]=Blinker.weather(adress);
}

 

void loop()

{
    Blinker.run();

    Serial.println(weather1);

    Blinker.delay(3000);

}
