#ifndef __noblock_led__
#define __noblock_led__

#include <arduino.h>

class nb_led {
    public:
        nb_led(int led_pin) { PIN_LED = led_pin; }
        virtual ~nb_led() { ; };

        void run () {
            NowMil = millis();
            if (NowMil < StartMil) {                                //condition to check millis() reset
                StartMil = NowMil;
            }

            if ((led_state == false) && (led_value > 0 )) {         //if the led is set to be off -> turn it off bit by bit
                if (led_value <= 80) {
                    if ((NowMil-StartMil) > 3 ) { 
                        StartMil = NowMil;
                        led_value--;
                        analogWrite(PIN_LED, led_value);
                    }
                }
                else if (led_value > 80) {
                    if ((NowMil-StartMil) > 2 ) { 
                        StartMil = NowMil;
                        led_value--;
                        analogWrite(PIN_LED, led_value);
                    }
                }
            }

            if (led_state == true) {                                //  if led is set to be on
                
                if (led_value < led_target) {                       //  if the led is less bright than target brightness -> turn it on bit by bit
                    if (led_value <= 80) {
                        if ((NowMil-StartMil) > 3 ) { 
                            StartMil = NowMil;
                            led_value++;
                            analogWrite(PIN_LED, led_value);
                        }
                    }
                    else if (led_value > 80) {
                        if ((NowMil-StartMil) > 2 ) { 
                            StartMil = NowMil;
                            led_value++;
                            analogWrite(PIN_LED, led_value);
                        }
                    }   
                }

                if (led_value > led_target) {                       //  if the led is brighter than target brightness -> turn it off bit by bit
                    if (led_value <= 80) {
                        if ((NowMil-StartMil) > 3 ) { 
                            StartMil = NowMil;
                            led_value--;
                            analogWrite(PIN_LED, led_value);
                        }
                    }
                    else if (led_value > 80) {
                        if ((NowMil-StartMil) > 2 ) { 
                            StartMil = NowMil;
                            led_value--;
                            analogWrite(PIN_LED, led_value);
                        }
                    }
                }
            }
        }

        void toggle () {
            led_state = !led_state;
        }

        void brightness (int TARGET) {
            led_target = TARGET;
        }

    private:
        int led_target = 255;
        int led_value = 0;
        bool led_state = false;
        int PIN_LED;
        unsigned long StartMil = 0;
        unsigned long NowMil = 0;
};

#endif //__noblock_led__