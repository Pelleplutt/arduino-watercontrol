#include "greenhouse.h"


void
changeWaterPort(unsigned char monitor, unsigned char state, unsigned long open_time, char mode) {
    if(state == WATER_OPEN) {
        digitalWrite(monitors[monitor].water_port, HIGH);

        monitors[monitor].water_state = state;
        monitors[monitor].water_opened_at = millis() / 1000;
        monitors[monitor].water_open_for = open_time;
        monitors[monitor].last_water_open = monitors[monitor].water_opened_at;




            /* FIXME Write to log */
    } else if(state == WATER_CLOSED) {
        digitalWrite(monitors[monitor].water_port, LOW);
        monitors[monitor].water_state = state;
        monitors[monitor].water_opened_at = 0;
        monitors[monitor].water_open_for = 0;
    }
}

