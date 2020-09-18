#include "Ubidots.h"

/****************************************
* Define Instances and Constants
****************************************/   
 
const char* UBIDOTS_TOKEN = "BBFF-xyG2eSU2WCQjkpqH5Pxbw1FulvSxCf";  // Put here your Ubidots TOKEN
const char* WIFI_SSID = "Nemo 4G";      // Put here your Wi-Fi SSID
const char* WIFI_PASS = "unity@1234";      // Put here your Wi-Fi password 

Ubidots ubidots(UBIDOTS_TOKEN, UBI_HTTP);

/****************************************
* Auxiliar Functions
****************************************/

// Put here your auxiliar functions

/****************************************
* Main Functions
****************************************/  
int sensor =A0;
float value;

void setup() {                       

  Serial.begin(115200);
  ubidots.wifiConnect(WIFI_SSID, WIFI_PASS);
  // ubidots.setDebug(true);  // Uncomment this line for printing debug  messages   
  pinMode(sensor,INPUT);                  
}
void loop(){
  value =analogRead(sensor);
  ubidots.add("Value", value);
   bool bufferSent = false;
  bufferSent = ubidots.send("Value");// Will send data to a device label that matches the device Id

  if (bufferSent) {
  // Do something if values were sent properly
   Serial.println("Value sent");
   Serial.println(value);
  }
  delay(5000);
}
