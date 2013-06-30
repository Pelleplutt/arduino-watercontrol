#include "greenhouse.h"

/*******************************************************************************/
void drawOverview() {

    GLCD.SelectFont(System5x7, BLACK);
    for(int i = 0; i < 8; i++) {

        if(i == monitor_selection) {
            GLCD.EraseTextLine(0);
            GLCD.print(monitors[i].name);

            if(monitors[i].water_state == WATER_OPEN) {
                GLCD.EraseTextLine(1);
                GLCD.CursorTo(0, 1);
                GLCD.print("OPEN");

                GLCD.EraseTextLine(2);
                GLCD.CursorTo(0, 2);
                GLCD.print("Close in ");

                    /* The port will be closed in main if we pass the trigger
                     * point, wrapping of millis() is also handled in main */
                GLCD.print(timeString((monitors[i].water_opened_at + monitors[i].water_open_for) - now_s));

            } else {
                if(monitors[i].water_interval == 0) {
                    drawSensorBar(monitors[i].current_value, monitors[i].calibrated_min, monitors[i].calibrated_max);
                } else {
                    unsigned long seconds_since;

                    GLCD.EraseTextLine(1);
                    GLCD.CursorTo(0, 1);
                    GLCD.print("Every ");
                    GLCD.print(timeString(monitors[i].water_interval));

                    GLCD.EraseTextLine(2);
                    GLCD.CursorTo(0, 2);
                    GLCD.print("Next in ");

                    seconds_since = now_s - monitors[i].last_water_open;

                    if(seconds_since >= monitors[i].water_interval) {
                        GLCD.print("NO TIME");
                    } else {
                        GLCD.print(timeString(monitors[i].water_interval - seconds_since));
                    }
                }
            }
            GLCD.SetFontColor(WHITE);
        }

        if(i < 4) {
            GLCD.CursorTo(0, DISPLAY_Y_MIDDLE + i);
        } else {
            GLCD.CursorTo(DISPLAY_X_MIDDLE, DISPLAY_Y_MIDDLE + i -4);
        }

        GLCD.print(monitors[i].name);
        for(int pad = strlen(monitors[i].name); pad < 10; pad++) {
            GLCD.PutChar(' ');
        }


        if(i == monitor_selection) {
            GLCD.SetFontColor(BLACK);
        }
    }
}


/*******************************************************************************/
void handleOverviewInput(int button) {
    switch(button) {
        case BUTTON_0: 
            {
                monitor_selection = (++monitor_selection) % 8;
                draw();
                break;
            }
        case BUTTON_1: 
            {
                if(monitor_selection) {
                    monitor_selection--;
                } else {
                    monitor_selection = 7;
                }
                draw();
                break;
            }
        case BUTTON_3: 
            {
                switchScreen(SCREEN_LOG);
            }
    }
}
