#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>


#define SS_PIN D3  //connect SDA pin
#define RST_PIN D2



#define echoPin D1
#define trigPin D4
#define rst D8
#define yel D0

long duration; 
int distance; 


MFRC522 mfrc522(SS_PIN, RST_PIN);  

const char*NAME;
const char* ID;
const char* LINK;

String Event_Name = "Road";
String Key = "dN4SKFDMGCg7Vgtzj3Pd5kpTxm0sh5gaf5LPdEVyoeW";
String resource = "/trigger/" + Event_Name + "/with/key/" + Key;
const char* server = "maker.ifttt.com";

#define WLAN_SSID       "Nemo 4G"
#define WLAN_PASS       "unity@1234"

void setup()
{
  pinMode(rst, OUTPUT);
  pinMode(yel, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
     while (WiFi.status() != WL_CONNECTED) 
       {
        delay(500);
        Serial.print(".");
       }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());

  
  SPI.begin();         
  mfrc522.PCD_Init(); 
 
  Serial.println("Put your card to the reader...");
  Serial.println();
  
  digitalWrite(rst,LOW);
  digitalWrite(yel,LOW);
}

void loop()                   
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; 
  Serial.println(distance);
  
  Serial.print("UID tag :");
  String content= "";
  byte letter;
     for(byte i = 0; i < mfrc522.uid.size; i++) 
       {
         Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");//printing the card tag
         Serial.print(mfrc522.uid.uidByte[i], HEX);//also printing card tag
         content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
         content.concat(String(mfrc522.uid.uidByte[i], HEX));    
       }
  
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  Serial.println( content.substring(1));
    
//Conditional Actions for Unaurthorized Cars:
     if(distance<5)
      {
        delay(500);      
          if(content.substring(1) == "")
            {   
              digitalWrite(rst,HIGH);
              delay(3000);
              digitalWrite(rst,LOW);
           
              NAME = "UnAurthorized Car";
              LINK="DDD";
              ID = "24";
              makeIFTTTRequest();
              Serial.print("Attendace Marked for "); Serial.println(NAME);
              delay(1000);  
              ESP.restart();              
            }
     }
     
//Conditional Actions for Aurthorized car and Criminal car--------------------------------------------->>>>>>>>>>>>>>
  else if( distance<10 && distance>5)
   {     
      delay(500); 
         if( ! mfrc522.PICC_IsNewCardPresent()) 
           {               
            return;
           }                 
             if( ! mfrc522.PICC_ReadCardSerial()) 
               {
                return;
               }
  
  Serial.print("UID tag :");
  String content= "";
  byte letter;
    for(byte i = 0; i < mfrc522.uid.size; i++) 
     {
       Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");//printing the card tag
       Serial.print(mfrc522.uid.uidByte[i], HEX);//also printing card tag
       content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
       content.concat(String(mfrc522.uid.uidByte[i], HEX));     
     }
  
 Serial.println();
 Serial.print("Message : ");
 content.toUpperCase();
 Serial.println( content.substring(1));
 
//Conditional Actions for Aurthorized Cars:          
      if(content.substring(1) == "D2 05 96 31") 
          {        
            NAME = "Atiqur Rahman";
            ID="Dhaka Metro-B(25C525K)";
            LINK = "https://www.facebook.com/";
            makeIFTTTRequest();
            Serial.print("Attendace Marked for "); Serial.println(NAME);    
            ESP.restart();          
          }  
          
//Conditional Actions for Criminal Cars:          
      else if( content.substring(1) == "F2 04 DB 1B" )
           {
            digitalWrite(yel,HIGH);
            delay(3000);
            digitalWrite(yel,LOW);
            NAME = "Habibur Rahman";
            ID = "Dhaka Metro-N(28CF25K)";
            LINK = "https://www.facebook.com/";
            makeIFTTTRequest();
            Serial.print("Attendace Marked for "); Serial.println(NAME);   
            ESP.restart(); 
           }     
  }
//All conditional Action Over here...........  
 else  
   { 
    Serial.println(" Access denied");         
   }  
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

  // Temperature in Celsius
  String jsonObject = String("{\"value1\":\"") + NAME + "\",\"value2\":\"" + ID+ "\",\"value3\":\"" + LINK+ "\"}";

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
