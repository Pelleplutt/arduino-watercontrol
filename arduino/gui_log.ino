#include "greenhouse.h"

/*******************************************************************************/
void
drawOpenLog() {
    water_open_log *log;

    GLCD.ClearScreen();
    GLCD.SelectFont(System5x7, BLACK);
    char entry_number  = monitors[monitor_selection].current_open_log;


        /* We will have no inbetween invalid entries, either they are set or
         * they are empty so simply draw the ones we can, skip the rest */
    for(char i = 0; i < 17; i++) {
        entry_number--;
        if(entry_number == -1) {
            entry_number = MONITOR_LOG_COUNT - 1;
        }

        log = &monitors[monitor_selection].open_log[entry_number];
        if(log->time) {
            GLCD.CursorTo(
                i < 8?0:DISPLAY_X_MIDDLE,
                i < 8?i:i-8);

            
            GLCD.print(log->open_mode);
            GLCD.print(" ");
            GLCD.print(relativeTimeString(log->time));
        }
    }

        /* No valid enties? */
    if(monitors[monitor_selection].current_open_log == 0 &&
        monitors[monitor_selection].open_log[0].time == 0) {

        GLCD.print("No log");
    }
}

void
drawSenseLog() {
    water_sense_log *log;

    GLCD.ClearScreen();
    GLCD.SelectFont(System5x7, BLACK);
    char entry_number  = monitors[monitor_selection].current_sense_log;


        /* We will have no inbetween invalid entries, either they are set or
         * they are empty so simply draw the ones we can, skip the rest */
    for(char i = 0; i < 17; i++) {
        entry_number--;
        if(entry_number == -1) {
            entry_number = MONITOR_LOG_COUNT - 1;
        }

        log = &monitors[monitor_selection].sense_log[entry_number];
        if(log->time) {
            char reading[10];
            GLCD.CursorTo(
                i < 8?0:DISPLAY_X_MIDDLE,
                i < 8?i:i-8);

           
            sprintf(reading, "%2d%% ", getSensorCalibratedPercent(log->value,
                        monitors[monitor_selection].calibrated_min,
                        monitors[monitor_selection].calibrated_max));
            GLCD.print(reading);
            GLCD.print(relativeTimeString(log->time, 7));
        }
    }

        /* No valid enties? */
    if(monitors[monitor_selection].current_sense_log == 0 &&
        monitors[monitor_selection].sense_log[0].time == 0) {

        GLCD.print("No log");
    }
}

/*******************************************************************************/
void
handleOpenLogInput(int button) {
    switch(button) {
        case BUTTON_0:
        case BUTTON_1:
        case BUTTON_2:
            {
                draw();
                break;
            }
        case BUTTON_3:
            {
                if(monitors[monitor_selection].trigger_value) {
                    switchScreen(SCREEN_LOG_SENSE);
                } else {
                    switchScreen(SCREEN_CONFIG1);
                }
            }
    }
}


void
handleSenseLogInput(int button) {
    switch(button) {
        case BUTTON_0:
        case BUTTON_1:
        case BUTTON_2:
            {
                draw();
                break;
            }
        case BUTTON_3:
            {
                switchScreen(SCREEN_CONFIG1);
            }
    }
}


