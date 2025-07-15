#ifndef __My_Touchpad__
#define __My_Touchpad__

#include <Arduino.h>

class Touchpad {
    public:
        Touchpad(int Touchpad_Pin) {         //initializer
                PIN = Touchpad_Pin;   
            }
            
        virtual ~Touchpad() { ; };       //destructor

        void initialise () {
            int mean = 0;
            for (int i = 0; i<10 ; i++) {
                mean += touchRead(PIN);
            }
            trigger = (mean / 10) -20;
            //debug Serial.printf("TouchPad on GPIO %d setted trigger level: %d\n", PIN, trigger);
        }

        void read () {
            if (trigger == -1) {
                Serial.println ("error in Touchpad class, trigger not set");
                return;
            }

            value = touchRead(PIN);
            if ( value < trigger) {
                last_state = state;
                state = true;
            }
            if (value > trigger) {
                last_state = state;
                state = false;
            }
            //Serial.printf("PATATE - PIN: %d - state: %d - last_state %d \n", PIN, state, last_state);
        }

        bool touched () {
            if (trigger == -1) {
                Serial.println ("error in Touchpad class, trigger not set");
                return false;
            }

            if (state == true && last_state ==  false) {
                return true;
            }
            return false;
        }

        bool released () {
            if (trigger == -1) {
                Serial.println ("error in Touchpad class, trigger not set");
                return false;
            }

            if (state == false && last_state ==  true) {
                return true;
            }
            return false;
        }

        bool kept () {
            if (trigger == -1) {
                Serial.println ("error in Touchpad class, trigger not set");
                return false;
            }

            if (state == true && last_state ==  true) {
                return true;
            }
            return false;
        }

    private:
        int startmillis=0;      //millis when touched
        int PIN;
        bool state = false;
        bool last_state = false;
        int trigger = 0;
        int value = 0;
};

#endif //__My_Touchpad__