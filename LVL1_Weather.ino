//  Brian Wagner
//  2/20/17
//  Tiny Weather Station
//  Uses Wemos D1 Mini


//wifi manager stuff
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library wifimanager
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

//needed for time
#include "TimeClient.h"
// TimeClient settings
const float UTC_OFFSET = 7;  //5 is eastern time zone
TimeClient timeClient(UTC_OFFSET);

#include "WundergroundClient.h"


//needed for screen and sensor
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WEMOS_SHT3X.h>

#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);
 
SHT3X sht30(0x45);

// Wunderground Settings
const boolean IS_METRIC = false;
const String WUNDERGRROUND_API_KEY = "e5db567c4067ded8";
const String WUNDERGRROUND_LANGUAGE = "EN";
const String WUNDERGROUND_COUNTRY = "US";
const String WUNDERGROUND_CITY = "Louisville";

// Set to false, if you prefere imperial/inches, Fahrenheit
WundergroundClient wunderground(IS_METRIC);


static const unsigned char PROGMEM temperature_icon16x16[] =
{
  0b00000001, 0b11000000, //        ###      
  0b00000011, 0b11100000, //       #####     
  0b00000111, 0b00100000, //      ###  #     
  0b00000111, 0b11100000, //      ######     
  0b00000111, 0b00100000, //      ###  #     
  0b00000111, 0b11100000, //      ######     
  0b00000111, 0b00100000, //      ###  #     
  0b00000111, 0b11100000, //      ######     
  0b00000111, 0b00100000, //      ###  #     
  0b00001111, 0b11110000, //     ########    
  0b00011111, 0b11111000, //    ##########   
  0b00011111, 0b11111000, //    ##########   
  0b00011111, 0b11111000, //    ##########   
  0b00011111, 0b11111000, //    ##########   
  0b00001111, 0b11110000, //     ########    
  0b00000111, 0b11100000, //      ######     
};

static const unsigned char PROGMEM water_tap_icon16x16[] =
{
  0b00000001, 0b10000000, //        ##       
  0b00000111, 0b11100000, //      ######     
  0b00000001, 0b10000000, //        ##       
  0b00001111, 0b11110000, //     ########    
  0b11111111, 0b11111110, // ############### 
  0b11111111, 0b11111111, // ################
  0b11111111, 0b11111111, // ################
  0b11111111, 0b11111111, // ################
  0b00000000, 0b00001111, //             ####
  0b00000000, 0b00001111, //             ####
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00001100, //             ##  
  0b00000000, 0b00001100, //             ##  
  0b00000000, 0b00000000, //                 
  0b00000000, 0b00001100, //             ##  
  0b00000000, 0b00001100, //             ##  
};


void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextColor(WHITE); 
  display.setTextSize(1);
  display.println("Please");
  display.println("Connect to");
  display.println("Access pt:");
  display.println(myWiFiManager->getConfigPortalSSID());
  display.println("3m timeout");
  display.display();
  delay(1000);
  
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void sensor_page()   
{
  //display the sensor
  sht30.get();
  display.clearDisplay();
  display.setCursor(0,0);
  
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.setTextSize(1);
  display.println("  Sensor  ");

  display.setCursor(0,12);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("    Temp");
  display.print("    ");
  display.print(sht30.fTemp);
  display.println("F");
  display.setCursor(0,30);
  display.println("    Humid");
  display.print("    ");
  display.print(sht30.humidity);
  display.println("%");
  display.drawBitmap(0, 10, temperature_icon16x16, 16, 16, 1);
  display.drawBitmap(0, 30, water_tap_icon16x16, 16, 16, 1);
  display.display();
}


void wifi_page()   
{
  //display the sensor
  display.clearDisplay();
  display.setCursor(0,0);
  
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.setTextSize(1);
  display.println("   WiFi   ");

  display.setCursor(0,12);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  if (WiFi.status() != WL_CONNECTED) {
    display.println("None");
  } else {
    display.println(WiFi.localIP());
  }
  display.display();
}

void time_page()   
{
  //display the sensor
  display.clearDisplay();
  display.setCursor(0,0);
  
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.setTextSize(1);
  display.println("   Time   ");

  display.setCursor(0,12);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  if (WiFi.status() != WL_CONNECTED) {
    display.println("No Wifi...");
    display.println("No Time!");
  } else {
    //wunderground.updateForecast(WUNDERGRROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
    //wunderground.updateConditions(WUNDERGRROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
    timeClient.updateTime();
    display.println(timeClient.getFormattedTime());
    display.println();
    //display.println(wunderground.getDate());
    //Serial.println(wunderground.getWeatherText());
  }
  display.display();
}

 
void setup()   
{
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextColor(WHITE); 
  display.setTextSize(1);
  display.println("LVL1");
  display.println("Tiny");
  display.println("Weather");
  display.display();
  delay(2000);
  display.display();
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(180);
  //wifiManager.setTimeout(5);

  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextColor(WHITE); 
  display.setTextSize(1);
  display.println("Connecting");
  display.println("to WiFi...");
  display.display();
  delay(1000);
  
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect()) {
      Serial.println("failed to connect and hit timeout");
      display.clearDisplay();
      display.setCursor(0,0);
      display.setTextColor(WHITE); 
      display.setTextSize(1);
      display.println("Failed to Connect");
      display.println("No WiFi");
      display.display();
      delay(1000);
  } else {
    //if you get here you have connected to the WiFi
      Serial.println("connected...yeey :)");
      
      display.clearDisplay();
      display.setCursor(0,0);
      display.setTextColor(WHITE); 
      display.setTextSize(1);
      display.println("Connected to WiFi");
      display.println(WiFi.localIP());
      display.display();
      delay(3000);
  }
}

 
 
void loop() 
{
  sensor_page();
  delay(5000);
  time_page();
  delay(5000);
  wifi_page();
  delay(5000);
}
