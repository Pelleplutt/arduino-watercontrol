#include "greenhouse.h"

/*******************************************************************************/
void _printUntilNext(char monitor) {
    unsigned long seconds_since = now_s - monitors[monitor].last_water_open;
    GLCD.SelectFont(System5x7, BLACK);

    char *tstr;
    if(seconds_since >= monitors[monitor].water_interval) {
        tstr = "ASAP";
    } else {
        tstr = timeString(monitors[monitor].water_interval - seconds_since);
    }
    GLCD.CursorTo(21 - strlen(tstr), 1);
    GLCD.print(tstr);
}

void
drawOverview() {
    GLCD.ClearScreen();

    GLCD.SelectFont(System5x7, BLACK);
    for(int i = 0; i < 8; i++) {

        if(i == monitor_selection) {
            GLCD.CursorTo(0, 0);

            if(monitors[i].water_state == WATER_OPEN) {
                GLCD.SelectFont(Arial_bold_14, BLACK);
                GLCD.print("OPEN");

                GLCD.CursorTo(0, 1);
                GLCD.SelectFont(System5x7, BLACK);
                GLCD.print("Close in ");

                    /* The port will be closed in main if we pass the trigger
                     * point, wrapping of millis() is also handled in main */
                GLCD.print(timeString((monitors[i].water_opened_at + monitors[i].water_open_for) - now_s));

            } else {
                if(!monitors[i].enabled || (monitors[i].trigger_value == 0 && monitors[i].water_interval == 0)) {
                    GLCD.SelectFont(Arial_bold_14, BLACK);
                    GLCD.print("Disabled");
                    GLCD.SelectFont(System5x7, BLACK);
                } else if(monitors[i].trigger_value > 0) {

                    GLCD.SelectFont(Arial_bold_14, BLACK);
                    GLCD.print(monitors[i].trigger_value);
                    GLCD.print("%");
                    if(monitors[i].water_interval > 0) {
                        GLCD.print(" / ");
                        GLCD.print(timeString(monitors[i].water_interval));
                    }
                    _printUntilNext(i);

                    drawSensorBar(monitors[i].current_value, monitors[i].calibrated_min, monitors[i].calibrated_max);
                } else if(monitors[i].water_interval > 0) {
                    unsigned long seconds_since;

                    GLCD.SelectFont(Arial_bold_14, BLACK);
                    GLCD.print("Every ");
                    GLCD.print(timeString(monitors[i].water_interval));

                    _printUntilNext(i);
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
void
handleOverviewInput(int button) {
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
