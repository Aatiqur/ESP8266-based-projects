#include <ESP8266HTTPClient.h>
#include <LiquidCrystal.h>
#include "json_parser.h"
#include "WifiConnect.h"

#define s2ms(second) (second*1000)
unsigned long long prev_millis(0);
 float p;

#define country_code "Bangladesh"

LiquidCrystal lcd(D0, D1, D2, D3, D4, D5);

int interval = s2ms(1);
unsigned long long PreviousMillis = 0;
unsigned long long CurrentMillis = interval;
bool bFirstKickMillis = false;

extern bool bGotIpFlag;

static String build_url_from_country(String country)
{
  String url = "http://coronavirus-19-api.herokuapp.com/countries/";
  url = url + country;
  return url;
}

void setup(void)
{
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Covid-19 BD");
#if defined JSON_DEBUG
  Serial.begin(9600);
#endif

  JSON_LOG("Connecting...");

  vConnWifiNetworkViaSdk();
}

void loop()
{
  
  
  if (bGotIpFlag) bGotIp();

  if (bFirstKickMillis) CurrentMillis = millis();

  if (!bGotIpFlag && CurrentMillis - PreviousMillis >= interval)
  {
    if (!bFirstKickMillis) CurrentMillis = 0;

    bFirstKickMillis = true;

    PreviousMillis = CurrentMillis;

    HTTPClient http;
    http.begin(build_url_from_country(country_code));

    int httpCode = http.GET();

    if (httpCode > 0)
    {
      String payload = http.getString();

      char* JsonArray = (char *)malloc(payload.length() + 1);
      if (!JsonArray) JSON_LOG("upssss fuck");

      payload.toCharArray(JsonArray, payload.length() + 1);

      JSON_LOG(JsonArray);

      if (json_validate(JsonArray))
      {
        int confirmed = (int)get_json_value(JsonArray, "cases", INT);
        int deaths = (int)get_json_value(JsonArray, "deaths", INT);
        int recovered = (int)get_json_value(JsonArray, "recovered", INT);
        int td = (int)get_json_value(JsonArray, "todayDeaths", INT);////////////////////////////////////////////////////////
        int tc = (int)get_json_value(JsonArray, "todayCases", INT);
        int tt = (int)get_json_value(JsonArray, "totalTests", INT); 
        int crc = (int)get_json_value(JsonArray, "critical", INT);


        
        JSON_LOG(crc);
        JSON_LOG(confirmed);/////
        JSON_LOG(deaths);/////////
        JSON_LOG(recovered);
        JSON_LOG(td);////////////////
        JSON_LOG(tc);/////
        JSON_LOG(tt);//////////////
        p=(deaths*100)/confirmed;/////////////////////
      
      lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Total Tests:");
        lcd.setCursor(0,1);
        lcd.print(tt);
        delay(5000);
      lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Total Cases:");
        lcd.setCursor(0,1);
        lcd.print(confirmed);
        delay(5000);
      lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Today Cases:");
        lcd.setCursor(0,1);
        lcd.print(tc);
        delay(5000);
      lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Total Deaths:");
        lcd.setCursor(0,1);
        lcd.print(deaths);
        delay(5000);
      lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Today Deaths:");
        lcd.setCursor(0,1);
        lcd.print(td);
        delay(5000);
      lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Total Recovered:");
        lcd.setCursor(0,1);
        lcd.print(recovered);
        delay(5000);
      lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Critical Patient");
        lcd.setCursor(0,1);
        lcd.print(crc);
        delay(5000);
      lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Deaths rate");
        lcd.setCursor(0,1);
        lcd.print(p);
         lcd.print("%");
        delay(5000);

      }

      free(JsonArray);
    }

    http.end();
  }
  delay(100);
}
