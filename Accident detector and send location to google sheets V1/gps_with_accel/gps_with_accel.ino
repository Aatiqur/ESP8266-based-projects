#include <Wire.h>
#include <MPU6050.h>
#include <TinyGPS++.h>
#include <ESP8266WiFi.h>
#include "FirebaseESP8266.h"

MPU6050 mpu;
TinyGPSPlus gps;

#define period 10000

int count=0;
char okFlag=0;
float x,y,z;
const char*NAME;
const char* ID;
String link;

#define FIREBASE_HOST "map-demo-8286f.firebaseio.com"                          
#define FIREBASE_AUTH "7K0Fo7kmNushWssdnIB30yfxibC8hGvIgGQueuHJ"       
String path = "/Test";

FirebaseData firebaseData;
FirebaseJson json;
void printResult(FirebaseData &data);

String Event_Name = "H";
String Key = "dN4SKFDMGCg7Vgtzj3Pd5kpTxm0sh5gaf5LPdEVyoeW";
String resource = "/trigger/" + Event_Name + "/with/key/" + Key;
const char* server = "maker.ifttt.com";

#define WLAN_SSID       "Nemo 4G"
#define WLAN_PASS       "unity@1234"

void setup() 
{
  Serial.begin(9600);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
    while(WiFi.status() != WL_CONNECTED) 
      {
       delay(500);
       Serial.print(".");
      }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP()); 
  Serial.println("Initialize MPU6050");   
    while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
      {    
       Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
       delay(500);
      }
  count=0;
  mpu.calibrateGyro();
  mpu.setThreshold(3);   


  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true); 
  firebaseData.setBSSLBufferSize(1024, 1024);
  firebaseData.setResponseSize(1024);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");



  Serial.println("Setup Over");
}

void loop()
{
   while (Serial.available() > 0)
   {
     if( gps.encode(Serial.read()))
      {
       Serial.println("ok2");
       float lattitude=gps.location.lat();
       float longitude=gps.location.lng();
       link="http://maps.google.com/maps?q=" + String(lattitude,8)+","+String(longitude,8);
       accelShow();
       
         if(x<36.70 && 25<x ||y<36.70 && 25<y || y>3 && y<15 || x>3 && x<15)
              {
               Serial.println("Step 1");
               delay(3000);
               accelShow();
                   if(x<36.70 && 25<x ||y<36.70 && 25<y || y>3 && y<15 || x>3 && x<15)
                       {
                        Serial.println("Accident Occured");
                        Serial.println(link);
                        NAME = "Atiqur Rahman";
                        ID="Dhaka Metro-B(25C525K)";
                        makeIFTTTRequest();
                       }    
              }

               //new start
    if (Firebase.setFloat(firebaseData, path + "/lat", lattitude))
    {
      Serial.println("PASSED");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.println("TYPE: " + firebaseData.dataType());
      Serial.println("ETag: " + firebaseData.ETag());
      Serial.print("VALUE: ");
      printResult(firebaseData);
      Serial.println("------------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }

    if (Firebase.setFloat(firebaseData, path + "/lng", longitude))
    {
      Serial.println("PASSED");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.println("TYPE: " + firebaseData.dataType());
      Serial.println("ETag: " + firebaseData.ETag());
      Serial.print("VALUE: ");
      printResult(firebaseData);
      Serial.println("------------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("------------------------------------");
      Serial.println();
    }
               //new end
    }    
  }    
}

//Function for getting the X Y and Z Value from MPU6050 Sensor...............
void accelShow()
{  
  Vector rawAccel = mpu.readRawAccel();
  Vector normAccel = mpu.readNormalizeAccel();
  x = normAccel.XAxis;
  y = normAccel.YAxis;
  z = normAccel.ZAxis;
  
  Serial.print(" Xnorm = ");
  Serial.print(x);
  Serial.print(" Ynorm = ");
  Serial.print(y);
  Serial.print(" Znorm = ");
  Serial.println(z);
  delay(200);
}

//Function for Making the IFTTT request to print Datas on GoogleSheet.........
void makeIFTTTRequest() 
{
  Serial.print("Connecting to ");
  Serial.print(server);

  WiFiClient client;
  int retries = 5;
  while (!!!client.connect(server, 80) && (retries-- > 0)) 
  {
    Serial.print(".");
  }
  Serial.println();
  if (!!!client.connected()) 
  {
    Serial.println("Failed to connect...");
  }

  Serial.print("Request resource: ");
  Serial.println(resource);

 
  String jsonObject = String("{\"value1\":\"") + NAME + "\",\"value2\":\"" + ID+ "\",\"value3\":\"" + link+ "\"}";

  client.println(String("POST ") + resource + " HTTP/1.1");
  client.println(String("Host: ") + server);
  client.println("Connection: close\r\nContent-Type: application/json");
  client.print("Content-Length: ");
  client.println(jsonObject.length());
  client.println();
  client.println(jsonObject);

  int timeout = 5 * 10; // 5 seconds
  while (!!!client.available() && (timeout-- > 0)) 
  {
    delay(100);
  }
  if (!!!client.available()) 
  {
    Serial.println("No response...");
  }
  while (client.available()) 
  {
    Serial.write(client.read());
  }

  Serial.println("\nclosing connection");
  client.stop();
}

void printResult(FirebaseData &data)
{

    if (data.dataType() == "int")
        Serial.println(data.intData());
    else if (data.dataType() == "float")
        Serial.println(data.floatData(), 6);
    else if (data.dataType() == "double")
        printf("%.9lf\n", data.doubleData());
    else if (data.dataType() == "boolean")
        Serial.println(data.boolData() == 1 ? "true" : "false");
    else if (data.dataType() == "string")
        Serial.println(data.stringData());
    else if (data.dataType() == "json")
    {
        Serial.println();
        FirebaseJson &json = data.jsonObject();
        //Print all object data
        Serial.println("Pretty printed JSON data:");
        String jsonStr;
        json.toString(jsonStr,true);
        Serial.println(jsonStr);
        Serial.println();
        Serial.println("Iterate JSON data:");
        Serial.println();
        size_t len = json.iteratorBegin();
        String key, value = "";
        int type = 0;
        for (size_t i = 0; i < len; i++)
        {
            json.iteratorGet(i, type, key, value);
            Serial.print(i);
            Serial.print(", ");
            Serial.print("Type: ");
            Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
            if (type == FirebaseJson::JSON_OBJECT)
            {
                Serial.print(", Key: ");
                Serial.print(key);
            }
            Serial.print(", Value: ");
            Serial.println(value);
        }
        json.iteratorEnd();
    }
    else if (data.dataType() == "array")
    {
        Serial.println();
        //get array data from FirebaseData using FirebaseJsonArray object
        FirebaseJsonArray &arr = data.jsonArray();
        //Print all array values
        Serial.println("Pretty printed Array:");
        String arrStr;
        arr.toString(arrStr,true);
        Serial.println(arrStr);
        Serial.println();
        Serial.println("Iterate array values:");
        Serial.println();
        for (size_t i = 0; i < arr.size(); i++)
        {
            Serial.print(i);
            Serial.print(", Value: ");

            FirebaseJsonData &jsonData = data.jsonData();
            //Get the result data from FirebaseJsonArray object
            arr.get(jsonData, i);
            if (jsonData.typeNum == FirebaseJson::JSON_BOOL)
                Serial.println(jsonData.boolValue ? "true" : "false");
            else if (jsonData.typeNum == FirebaseJson::JSON_INT)
                Serial.println(jsonData.intValue);
            else if (jsonData.typeNum == FirebaseJson::JSON_DOUBLE)
                printf("%.9lf\n", jsonData.doubleValue);
            else if (jsonData.typeNum == FirebaseJson::JSON_STRING ||
                     jsonData.typeNum == FirebaseJson::JSON_NULL ||
                     jsonData.typeNum == FirebaseJson::JSON_OBJECT ||
                     jsonData.typeNum == FirebaseJson::JSON_ARRAY)
                Serial.println(jsonData.stringValue);
        }
    }
}
