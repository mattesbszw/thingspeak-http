// #define PRINT_DEBUG_MESSAGES // not necessary but useful for debugging TS library

#include "tscert.h"

#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <ThingSpeak.h> // unbedingt nach der WiFi-Bibliothek einbinden


#define DHT_PIN 4
#define ONE_WIRE_BUS 16
#define DHTTYPE DHT11

DHT dht(DHT_PIN, DHTTYPE);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// WLAN-Zugangsdaten
const char* ssid = "CPSLABOR";
const char* password = "A1234567890";

WiFiClient client;

// auf den jew. ThingSpeak-Kanal anpassen, hier MAT
unsigned long tsChannelNumber = 0;
const char* tsWriteAPIKey = "YOURKEYHERE";


void setup(void){
  
  Serial.begin(115200);
  dht.begin(); // init DHT sensor
  sensors.begin(); // init DS18B20 sensors on OneWire bus
  
  connectWiFi();
     
  // ThingSpeak API wird initialisiert
  ThingSpeak.begin(client);
  
}

void connectWiFi() {
  
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to " + String(ssid));
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    WiFi.begin(ssid, password); 
    delay(5000);     
  } 
  Serial.println("\nConnected.");  
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}


void loop(void){ 
  
  sensors.requestTemperatures(); // Send the command to get temperatures
  String tmpDS = String(sensors.getTempCByIndex(0));
  Serial.print("Sensor 1(*C): ");
  Serial.println(tmpDS); 
   
  float tmp = dht.readTemperature();
  String tmpDHT = "0";
  if (!isnan(tmp)) {
    tmpDHT = tmp;
  }
  Serial.print("Sensor 2(*C): "); 
  Serial.println(tmpDHT);
 
  ThingSpeak.setField(1, tmpDS);
  ThingSpeak.setField(2, tmpDHT);
  int ret = ThingSpeak.writeFields(tsChannelNumber, tsWriteAPIKey);
   
   if(ret == 200){
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(ret));
  } 
  
  delay(30000);
}
