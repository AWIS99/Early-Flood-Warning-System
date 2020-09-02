#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

char ssid[] = "AOSP EXTENDED"; 
char pass[] = "9853183968";

WiFiClient  client;

unsigned long myChanne2Field = 1008034; 
const char * myWriteAPIKey2 = "0QV960AQ9CDWJJ8D"; 

#define SENSOR  4

const int trigPin = 2;  
const int echoPin = 0; 
long duration;
int distance; 

unsigned long currentMillis = 0;
long previousMillis = 0;
unsigned long m1 = 0;
unsigned long m2 = 0;
int interval = 1000;
//boolean ledState = LOW;
float calibrationFactor = 7.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

void setup()
{
  Serial.begin(115200);
  
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  pinMode(SENSOR, INPUT_PULLUP);

  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;

  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }  
  
  currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    
    pulse1Sec = pulseCount;
    pulseCount = 0;
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;
    
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  
    Serial.print("L/min");
    Serial.print("\t");      

    Serial.print("Output Liquid Quantity: ");
    Serial.print(totalMilliLitres);
    Serial.print("mL / ");
    Serial.print(totalMilliLitres / 1000);
    Serial.println("L");
  }
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  distance= duration*0.034/2;
  
  Serial.print("Distance: ");
  Serial.println(distance);

  m1=millis();
  if(m1-m2>15000)
  {
    ThingSpeak.setField( 1, (float) flowRate);
    ThingSpeak.setField( 2, (long) totalMilliLitres);
    ThingSpeak.setField( 3, (int) distance);
    ThingSpeak.writeFields(myChanne2Field, myWriteAPIKey2);
    Serial.println("****************************Data Uploaded****************************");
    m2=millis();
  }   
  delay(1000);

}
