#include <WiFi.h>
#include "My_Time_Config.h"         // MyClock class, used to easy get time from internal clock
#include "MY_Lander_Display.h"      // Display class used to create a graphic output on display
#include "My_Weather.h"             // Header of weatherData class, use to exctract data from raw
#include "noblock_led.h"

#include <TFT_eSPI.h>               // Graphics and font library for ST7735 driver chip
#include <SPI.h>                    // https://github.com/Bodmer/TFT_eSPI/tree/master

#include <Wire.h>                   // BMP280 library group
#include <Adafruit_Sensor.h>        // https://randomnerdtutorials.com/esp32-bme280-arduino-ide-pressure-temperature-humidity/
#include <Adafruit_BME680.h>      

#include <HTTPClient.h>             // https://randomnerdtutorials.com/esp32-http-get-open-weather-map-thingspeak-arduino/

#include <ArduinoJson.h>            // Used to read data from raw String got from API

#define NTP_SERVER      "it.pool.ntp.org"
#define WIFI_SSID       "WIFI_NAME"
#define WIFI_PASSWORD   "WIFI_PASSWORD"
#define LED_BUILTIN     2
#define BACKLIGHT       16

MyClock orologio;

TFT_eSPI tft = TFT_eSPI();          // Invoke library, pins defined in User_Setup.h
Lander_Display Display (&tft);     
Adafruit_BME680 bme;                // I2C
JsonDocument doc;                   // https://registry.platformio.org/libraries/bblanchon/ArduinoJson/examples/StringExample/StringExample.ino
nb_led backlight (BACKLIGHT);

String openWeatherMapApiKey = "API-KEY";
String city = "YOUR_CITY";
String countryCode = "YOUR_COUNTRY_CODE";
String unitsCode = "metric";
String langCode = "en";
String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&units=" + unitsCode + "&lang=" + langCode + "&APPID=" + openWeatherMapApiKey;
String payload;                     // String to store rawdata got from weather API

uint32_t color = 0xfba0;            // color of graphic and lines on DIsplay

const int Period1 = 5000;           // Setting refresh sensor rate
unsigned long StartMillis1 = 0;     
const int Period2 = 1800000;        // Weather refresh rate [30m] 
long int StartMillis2 = 0;


void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BACKLIGHT, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  //------------------------- Initialize sensor ---------------------------
  bool status = bme.begin(0x77); 
  if (!status) {
  Serial.println("Could not find a valid BMP280 sensor, check wiring!");
  //while (1);
  }

  //-------------------------- wifi connection -----------------------------
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {;}    //block code until wifi connected
  Serial.println("\nConnected to " WIFI_SSID "!" );

  orologio.ntpsync(NTP_SERVER);   // syncronizing internal clock to real time
  orologio.WriteTime();           // terminal debug
  orologio.TUpdate();             // uodate class to real time
  
  // ------------------------ Initialize screen -------------------------------
  Display.init();
  Display.timeup(orologio.exportStruct());

  digitalWrite(LED_BUILTIN, LOW);
  backlight.toggle( true );
}

void loop() {

  //------------------ Action repeated every code cycle -----------------------
  orologio.TUpdate();
  Display.timeup(orologio.exportStruct());
  backlight.run();

  //----------------- protection against wfi disconnection --------------------
  if (WiFi.status() != WL_CONNECTED) {
    while (WiFi.status() != WL_CONNECTED) {digitalWrite(LED_BUILTIN, HIGH);}
    digitalWrite(LED_BUILTIN, LOW);
  }
  
  //--------------- Protection against Millis() value reset -------------------
  if (millis() < StartMillis1 || millis() < StartMillis2){    //protection against Millis() value reset
    StartMillis1 = 0;
    StartMillis2 = 0;
    Serial.println("Millis() reset");
  }

  //------------------------- Sensor Refresh ---------------------------------
  if (millis () - StartMillis1 > Period1) {
    Display.measureup(bme.readTemperature(), bme.readPressure());
    StartMillis1 = millis();
  }
  
  //-------------------------- Weather update --------------------------------
if (millis () - StartMillis2 > Period2 || StartMillis2 == 0) {
    Serial.print("Weather update:");
    orologio.WriteTime();

    HTTPClient http;
    http.begin(serverPath);
    int httpCode = http.GET();

    // httpCode will be negative on error
    if(httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if(httpCode == HTTP_CODE_OK) {
        payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();

    Display.weatherUp(payload);
    StartMillis2 = millis();
  }
}