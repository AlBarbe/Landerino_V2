#ifndef __My_Lander_Display__
#define __My_Lander_Display__

#include <TFT_eSPI.h>
#include <My_Weather.h>

class Lander_Display {
    public:
        Lander_Display(TFT_eSPI *tft_pointer ) { tft = tft_pointer; }
        virtual ~Lander_Display() { ; };

        void init () {
            tft->init();
            tft->setRotation(2);
            tft->fillScreen(TFT_BLACK);
            graphicUp();
        }
        
        void tmstructup (struct tm buffer) {
            PresentTime = buffer;
        }

        void measureup (float temp, float pres) {
            tft->setTextColor(TFT_BLACK);
            tft->drawRightString(roomTemperature,64, 38 ,4);
            tft->drawRightString(roomPressure,125,55,2);
            roomTemperature = String ( ((round (temp*10.f))/10.f) );
                if (roomTemperature.length() == 5) {
                    roomTemperature.remove(4, 1);
                } else if ( roomTemperature.length() == 4 ) {
                    roomTemperature.remove(3, 1);
                }
                roomTemperature += "\u00b0";
            roomPressure = String((pres)/100000.f);
                if (roomPressure.length() == 9) {
                    //roomPressure.remove (6,3);
                }
                roomPressure += " bar";
            tft->setTextColor(TFT_WHITE);
            tft->drawRightString(roomTemperature,64,38,4);
            tft->drawCircle(61, 41, 2, TFT_WHITE);
            tft->drawRightString(roomPressure,125,55,2);
        }

        void timeup (struct tm buffer) {
            PresentTime = buffer;
            //-----clock-----//
            if (minute != PresentTime.tm_min) {
                tft->setTextColor(color);
                tft->drawString(clock,2,0,4);
                
                if (PresentTime.tm_hour < 10) {                      //inserting hours in buffer
                    clock = "0" + String(PresentTime.tm_hour);
                } else {
                    clock = "" + String(PresentTime.tm_hour);
                }
                if (PresentTime.tm_min < 10) {                    //inserting minute in buffer
                    clock += ":0" + String(PresentTime.tm_min);
                } else {
                    clock += ":" + String(PresentTime.tm_min);
                }
                tft->setTextColor(clock_color);
                tft->drawString(clock,2,0,4);
                minute = PresentTime.tm_min;
            }
            //-----date------//
            if (day != PresentTime.tm_mday) {
                tft->setTextColor(TFT_BLACK);
                tft->drawString (year,87,5,2);
                tft->drawString(date,77,22,2);
                if (PresentTime.tm_mday < 10) {
                    date = " " + String(PresentTime.tm_mday);
                } else {
                    date = String(PresentTime.tm_mday);
                }
                date += "-" + month_name(PresentTime.tm_mon);
                year = 1900 + PresentTime.tm_year;
                tft->setTextColor(TFT_WHITE);
                tft->drawString (year,87,5,2);
                tft->drawString(date,77,22,2);
                day = PresentTime.tm_mday;
            }
        }

        void weatherUp(String rawData) {
            tft->setTextColor(TFT_BLACK);
            tft->drawRightString(cityTemperature, 47, 79, 2);
            tft->drawCircle(51, 83, 2, TFT_BLACK);
            tft->drawRightString("Naters",118, 85, 2);
            tft->drawCentreString(description, 64, 106, 1);
            tft->drawString(sunrise, 10, 123, 1);
            tft->drawString(sunset, 49, 123, 1);
            tft->drawRightString(wind, 113, 123, 1);
            tft->drawString("m",115,120,1);
            tft->drawString("s",118,128,1);
            wt.extractData(rawData);
            getWeather ();
            tft->setTextColor(TFT_WHITE);
            tft->drawRightString(cityTemperature, 47, 79, 2);
            tft->drawCircle(51, 83, 2, TFT_WHITE);
            tft->drawRightString("Naters",118, 85, 2);
            tft->drawCentreString(description, 64, 106, 1);
            tft->drawString(sunrise, 10, 123, 1);
            tft->drawString(sunset, 49, 123, 1);
            tft->drawRightString(wind, 113, 123, 1);
            tft->drawString("m",115,120,1);
            tft->drawString("s",118,128,1);
        }

        void graphicUp() {
            //clock
            tft->drawWideLine(60, 24, 84, 48, 2, color, TFT_BLACK);
            tft->fillRect(0, 0, 60, 25, color);
            tft->fillRect(84, 47, 44, 2, color);
            tft->fillTriangle(60, 0, 60, 24, 84, 0, color);

            //roomPressure and roomTemperature
            tft->drawWideLine(58, 67, 72, 81, 2, color, TFT_BLACK);
            tft->fillRect(0, 67, 58, 2, color);
            tft->fillRect(72, 81, 58, 2.5, color);

            //sunrise
            tft->drawArc(20, 153, 9, 7,90, 270, color, TFT_BLACK, true);
            tft->fillRect(8, 154, 25, 2, color);

            tft->drawLine(20, 140, 20, 134, color);
            tft->drawLine(21, 140, 21, 134, color);

            tft->drawLine(20,134,17,137,color);
            tft->drawLine(20,133,17,136,color);

            tft->drawLine(21,134,24,137,color);
            tft->drawLine(21,133,24,136,color);

            //sunset
            tft->drawArc(63, 153, 9, 7,90, 270, color, TFT_BLACK, true);
            tft->fillRect(51, 154, 25, 2, color);

            tft->drawLine(63, 140, 63, 134, color);
            tft->drawLine(64, 140, 64, 134, color);

            tft->drawLine(63,140,60,137,color);
            tft->drawLine(63,139,60,136,color);

            tft->drawLine(64,140,67,137,color);
            tft->drawLine(64,139,67,136,color);

            //wind
            tft->fillRect(99, 144, 9, 2, color);
            tft->drawArc(108,142, 3, 2,180,360,color,TFT_BLACK,true); 
            tft->fillRect(96, 149, 16, 2, color);
            tft->drawArc(112,152, 3, 2,180,360,color,TFT_BLACK,true); 
            tft->fillRect(99, 154, 9, 2, color);

            //restore clock
            tft->setTextColor(clock_color);
            tft->drawString(clock,2,0,4);
        }

        void getWeather() {
            wind = String(wt.get_wind());
            description = wt.get_description();
            sunrise = wt.get_sunrise();
            sunset = wt.get_sunset();
            cityTemperature = String(wt.get_temperature());
            cityHumidity = String(wt.get_humidity()) + " %";
        }

        void setColor (uint32_t new_color) {
            color = new_color;
        }

        void nextColor () {
            switch (color_index)
            {
                case 0:
                    color_index = 1;
                    color = 0xc800;
                    graphicUp();               
                    break;
                case 1:
                    color_index = 2;
                    color = TFT_VIOLET;
                    graphicUp();
                    break;
                case 2:
                    color_index = 4;
                    color = TFT_SKYBLUE;
                    graphicUp();
                    break;
                case 4:
                    color_index = 5;
                    color = 0x3fc8;
                    graphicUp();
                    break;
                case 5:
                    color_index = 6;
                    color = TFT_YELLOW;
                    graphicUp();
                    break;
                case 6:
                    color_index = 0;
                    color = 0xfcc0;
                    graphicUp();
                    break;
                default:
                break;
            }
        }

    private:
    
            //-----Pointers----//
        TFT_eSPI* tft;
            //----Variables----//
        weatherData wt;
        struct tm PresentTime;
        int day;
        int minute;
        uint32_t color = TFT_RED;
        uint32_t clock_color = TFT_BLACK;
        int color_index = 1;
        //-----clock-------//
        String clock = "00:00";
        String date = "00/00";
        String year = "0000";
            //-----Sensor------//
        String roomTemperature = "00.0";
        String roomPressure = "0.00 bar";
            //-----Weather-----//
        String city = "Naters";
        String wind;
        String description;
        String sunrise = "0:00";
        String sunset = "0:00";
        String cityTemperature = "00.0";
        String cityHumidity = "00%";
};

#endif //__My_Lander_Display__