// #define PRINT_DEBUG_MESSAGES // not necessary but useful for debugging TS library

#include "tscert.h"

#include <Wire.h>
#include <Adafruit_SSD1306.h>

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

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// WLAN-Zugangsdaten
const char* ssid = "CPSLABOR-Z";
const char* password = "Z123456789Z";

WiFiClient client;

// auf den jew. ThingSpeak-Kanal anpassen, hier MAT
unsigned long tsChannelNumber = 0;
const char* tsWriteAPIKey = "YOURKEYHERE";


void setup(void){
  
  Serial.begin(115200);
  dht.begin(); // init DHT sensor
  sensors.begin(); // init DS18B20 sensors on OneWire bus
  if(!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // init OLED display
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  } 

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
 
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.println("DS: " + tmpDS);
  oled.println("DHT: " + tmpDHT);
  oled.display(); 

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
