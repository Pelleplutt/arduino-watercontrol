#include "greenhouse.h"


char *
relativeTimeString(unsigned long when) {
    when = now_s - when;
    return timeString(when);

}

char *
timeString(unsigned long seconds) {
    static char buf[32];
    int len = 0;

    *buf = 0;
    if(seconds >= 86400) {
        len += snprintf(buf + len, 32 - len, "%dd", seconds / 86400);
        seconds = seconds % 86400;
    }

    if(seconds >= 3600) {
        len += snprintf(buf + len, 32 - len, "%dh", seconds / 3600);
        seconds = seconds % 3600;
    }

    if(seconds >= 60) {
        len += snprintf(buf + len, 32 - len, "%dm", seconds / 60);
        seconds = seconds % 60;
    }

    if(seconds % 60) {
        len += snprintf(buf + len, 32 - len, "%ds", seconds % 60);
    }


    return buf;
}
