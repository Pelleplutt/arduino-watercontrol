#include "greenhouse.h"


char *
relativeTimeString(unsigned long when) {
    when = now_s - when;
    return timeString(when);

}

char *
relativeTimeString(unsigned long when, unsigned char maxlen) {
    when = now_s - when;
    return timeString(when, maxlen);

}

char *
timeString(unsigned long seconds, unsigned char maxlen) {
    char *str = timeString(seconds);
    unsigned int len = strlen(str);
    if(len > maxlen) {
        if(maxlen > 2) {
                /* Convert to zero based */
            len = maxlen - 1;
                /* loop downwards until we find something not an integer */
            while(len && str[len] >= '0' && str[len] <= '9') {
                len--;
            }
            str[len + 1] = '\0';
        } else {
            if(len == 2) {
                str[len - 2] = '\0';
            } else {
                str[0] = '\0';
            }
        }
    }
    return str;
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
