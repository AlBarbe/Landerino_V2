#include <WiFi.h>
#include "My_Time_Config.h"         // MyClock class, used to easy get time from internal clock
#include "MY_Lander_Display.h"      // Display class used to create a graphic output on display
#include "My_Weather.h"             // Header of weatherData class, use to exctract data from raw
#include "My_Touchpad.h"            // used for touchpads functions
#include "noblock_led.h"            // Class that introduce smooth led transition with non blocking code

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
#define TOUCHPAD_1      32
#define TOUCHPAD_2      33

MyClock orologio;
TFT_eSPI tft = TFT_eSPI();          //  Invoke library, pins defined in User_Setup.h
Lander_Display Display (&tft);     
Adafruit_BME680 bme;                //  I2C
JsonDocument doc;                   //  https://registry.platformio.org/libraries/bblanchon/ArduinoJson/examples/StringExample/StringExample.ino
nb_led backlight (BACKLIGHT);
Touchpad pad1 (TOUCHPAD_1);
Touchpad pad2 (TOUCHPAD_2);

String openWeatherMapApiKey = "API-KEY";
String city = "YOUR_CITY";
String countryCode = "YOUR_COUNTRY_CODE";
String unitsCode = "metric";
String langCode = "en";
String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&units=" + unitsCode + "&lang=" + langCode + "&APPID=" + openWeatherMapApiKey;
String payload;                     // String to store rawdata got from weather API

const int Period1 = 100;            // TouchPad refresh rate
unsigned long StartMillis1 = 0;     
const int Period2 = 5000;           // Sensor refresh rate
unsigned long StartMillis2 = 0;     
const int Period3 = 1800000;        // Weather refresh rate [30m] 
long int StartMillis3 = 0;
int brightness = 4;


void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BACKLIGHT, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  //---------------------- Initialize sensor and Pads ------------------------
  bool status = bme.begin(0x77); 
  if (!status) {
  Serial.println("Could not find a valid BME680 sensor, check wiring!");
  //while (1);
  }

  pad1.initialise();
  pad2.initialise();

  //-------------------------- wifi connection -------------------------------
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {;}    //block code until wifi connected
  Serial.println("\nConnected to " WIFI_SSID "!" );

  orologio.ntpsync(NTP_SERVER);   // syncronizing internal clock to real time
  orologio.WriteTime();           // terminal debug
  orologio.TUpdate();             // uodate class to real time
  
  // ------------------------ Initialize screen ------------------------------
  Display.init();
  Display.timeup(orologio.exportStruct());

  digitalWrite(LED_BUILTIN, LOW);
  backlight.toggle();
}


void loop() {

  //------------------ Action repeated every code cycle ----------------------
  orologio.TUpdate();
  Display.timeup(orologio.exportStruct());
  backlight.run();

  //----------------- protection against wfi disconnection -------------------
  if (WiFi.status() != WL_CONNECTED) {
    while (WiFi.status() != WL_CONNECTED) {digitalWrite(LED_BUILTIN, HIGH);}
    digitalWrite(LED_BUILTIN, LOW);
  }
  
  //--------------- Protection against Millis() value reset ------------------
  if (millis() < StartMillis2 || millis() < StartMillis3){
    StartMillis2 = 0;
    StartMillis3 = 0;
    Serial.println("Millis() reset");
  }

  //----------------------- Touchpads Refresh --------------------------------
   if (millis () - StartMillis1 > Period1) {
    pad1.read();
    pad2.read();

    if(pad1.released() == true) {
      switch (brightness){
        case 4:
          brightness = 3;
          backlight.brightness(128);
          //Serial.println("PATATE - Brightness changed 3");
          break;
        case 3:
          brightness = 2;
          backlight.brightness(63);
          //Serial.println("PATATE - Brightness changed 2");
          break;
        case 2:
        brightness = 1;
          backlight.brightness(20);
          //Serial.println("PATATE - Brightness changed 1");
          break;
        case 1:
          brightness = 0;
          backlight.brightness(0);
          backlight.toggle();
          //Serial.println("PATATE - Brightness changed 0");
          break;
        case 0:
          brightness = 4;
          backlight.brightness(255);
          backlight.toggle();
          Serial.println("PATATE - Brightness changed 4");
          break;
        default:
          Serial.println("ERROR in Pad1 refresh action in MAIN");
          break;
      }
    }

    if(pad2.released() == true) {
      Display.nextColor();
    }
    StartMillis1 = millis();
  }

  //------------------------- Sensor Refresh ---------------------------------
  if (millis () - StartMillis2 > Period2) {
    Display.measureup(bme.readTemperature(), bme.readPressure());
    StartMillis2 = millis();
  }
  
  //-------------------------- Weather update --------------------------------
if (millis () - StartMillis3 > Period3 || StartMillis3 == 0) {
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
    StartMillis3 = millis();
  }
}