
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#define SS_PIN D3  //connect SDA pin
#define RST_PIN D2


#define SERVO_PIN D4
Servo myservo;

#define green D0

#define red D1


MFRC522 mfrc522(SS_PIN, RST_PIN);  

const char*NAME;
const char* ID;
String Event_Name = "Attendance";
String Key = "dN4SKFDMGCg7Vgtzj3Pd5kpTxm0sh5gaf5LPdEVyoeW";
String resource = "/trigger/" + Event_Name + "/with/key/" + Key;
const char* server = "maker.ifttt.com";

#define WLAN_SSID       "Nemo 4G"
#define WLAN_PASS       "unity@1234"



void setup()
{
 
 Serial.begin(115200);
 Serial.println();
 Serial.print("Connecting to ");
 Serial.println(WLAN_SSID);

 WiFi.begin(WLAN_SSID, WLAN_PASS);
 while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  
SPI.begin();         
  mfrc522.PCD_Init(); 
 
  Serial.println("Put your card to the reader...");
  Serial.println();
////////////////////////////////////////////
    myservo.write(0);
    myservo.attach(SERVO_PIN);
    myservo.write(180);
    delay(8000);
    myservo.write(0);
////////////////////////////////////////////
pinMode(green,OUTPUT);
pinMode(red,OUTPUT);
digitalWrite(green,LOW);
digitalWrite(red,LOW);
 
}

void loop()                   
{
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
 
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  
  Serial.print("UID tag :");
  String content= "";
  byte letter;
for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
    Serial.println();
    Serial.print("Message : ");
    content.toUpperCase();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
if (content.substring(1) == "D2 05 96 31") 
  {

   
    NAME = "Atiqur Rahman";
    ID = "1";
    makeIFTTTRequest();
    Serial.print("Attendace Marked for "); Serial.println(NAME);
    ///////////////////
      digitalWrite(green,HIGH);
      delay(2000);
       digitalWrite(green,LOW); 
      digitalWrite(red,LOW);
    myservo.write(180);
    delay(8000);
    myservo.write(0);
    //////////////////    
      

    
  }

else if (content.substring(1) == "F2 04 DB 1B" )
  {
    
    NAME = "Habibur Rahman";
    ID = "2";
     makeIFTTTRequest();
         Serial.print("Attendace Marked for "); Serial.println(NAME);
    ///////////////////
      digitalWrite(green,HIGH);
      delay(2000);
       digitalWrite(green,LOW); 
      digitalWrite(red,LOW);
    myservo.write(180);
    delay(8000);
    myservo.write(0);
    //////////////////    
    
     
 }
 
else  
 {
    Serial.println(" Access denied");
        digitalWrite(red,HIGH);
            delay(2000);
           digitalWrite(red,LOW); 
    digitalWrite(green,LOW);
      
  
 }
     }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void makeIFTTTRequest() {
  Serial.print("Connecting to ");
  Serial.print(server);

  WiFiClient client;
  int retries = 5;
  while (!!!client.connect(server, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if (!!!client.connected()) {
    Serial.println("Failed to connect...");
  }

  Serial.print("Request resource: ");
  Serial.println(resource);

  // Temperature in Celsius
  String jsonObject = String("{\"value1\":\"") + NAME + "\",\"value2\":\""+ID+"\"}";

  client.println(String("POST ") + resource + " HTTP/1.1");
  client.println(String("Host: ") + server);
  client.println("Connection: close\r\nContent-Type: application/json");
  client.print("Content-Length: ");
  client.println(jsonObject.length());
  client.println();
  client.println(jsonObject);

  int timeout = 5 * 10; // 5 seconds
  while (!!!client.available() && (timeout-- > 0)) {
    delay(100);
  }
  if (!!!client.available()) {
    Serial.println("No response...");
  }
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("\nclosing connection");
  client.stop();
}





















      
      
