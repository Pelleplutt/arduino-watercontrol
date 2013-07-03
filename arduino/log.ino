#include "greenhouse.h"


void
appendToLog(unsigned char monitor, char mode, unsigned long time) {

    water_log *logentry = &monitors[monitor].log[monitors[monitor].current_log];

    logentry->time = time;
    logentry->open_mode = mode;

    if(++monitors[monitor].current_log == MONITOR_LOG_COUNT) {
        monitors[monitor].current_log = 0;
    }
}
