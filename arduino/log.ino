#include "greenhouse.h"


void
appendToOpenLog(unsigned char monitor, char mode, unsigned long time) {

    water_open_log *logentry = &monitors[monitor].open_log[monitors[monitor].current_open_log];

    logentry->time = time;
    logentry->open_mode = mode;

    if(++monitors[monitor].current_open_log == MONITOR_LOG_COUNT) {
        monitors[monitor].current_open_log = 0;
    }
}

void
appendToSenseLog(unsigned char monitor, int value, unsigned long time) {

    water_sense_log *logentry = &monitors[monitor].sense_log[monitors[monitor].current_sense_log];

    logentry->time = time;
    logentry->value = value;

    if(++monitors[monitor].current_sense_log == MONITOR_LOG_COUNT) {
        monitors[monitor].current_sense_log = 0;
    }
}
