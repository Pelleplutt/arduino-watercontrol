#include "greenhouse.h"

void
drawSensorBar(int value, unsigned long calibrated_min, unsigned long calibrated_max) {
    GLCD.SelectFont(System5x7, BLACK);
    if(value >= 0) {

        int percent = getSensorCalibratedPercent(value, calibrated_min, calibrated_max);
        int pixels = (GLCD.Width - 1) * percent / 100;

        if(percent >= 100) {
            GLCD.CursorTo(17, 0);
        } else if(percent >= 10) {
            GLCD.CursorTo(18, 0);
        } else {
            GLCD.CursorTo(19, 0);
        }
        GLCD.print(percent, DEC);
        GLCD.PutChar('%');


        GLCD.FillRect(0, 20, GLCD.Width - 1, 10, WHITE);
        GLCD.DrawRect(0, 20, GLCD.Width - 1, 10, BLACK);
        GLCD.FillRect(0, 20, pixels, 10, BLACK);
    } else {
        GLCD.FillRect(0, 20, GLCD.Width - 1, 10, WHITE);
    }
}


