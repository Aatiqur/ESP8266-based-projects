#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
BlynkTimer timer;
char auth[] = "39qoDEmZy2atJf08BpG-a8EoGrXQLzmD";
char ssid[] = "Nemo 4G";
char pass[] = "unity@1234";

int n;

void setup()
{

  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);

  pinMode(16,OUTPUT);
  pinMode(5,OUTPUT);
  timer.setInterval(1000L, sendUptime);
}


void sendUptime()
{
  Blynk.virtualWrite(V1, n);
}

void loop()
{
  Blynk.run();
  timer.run();
  
  n=analogRead(A0);
  Serial.println(n);
  
  if(n>190)
  {
     digitalWrite(16,LOW);
    digitalWrite(5,HIGH);   
  }
  
  if(n<190)
  {
      digitalWrite(16,HIGH);
      digitalWrite(5,LOW);
  }

}
