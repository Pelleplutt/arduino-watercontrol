#include "greenhouse.h"

/*******************************************************************************/
void
drawLog() {
    water_log *log;

    GLCD.ClearScreen();
    GLCD.SelectFont(System5x7, BLACK);
    char entry_number  = monitors[monitor_selection].current_log;


        /* We will have no inbetween invalid entries, either they are set or
         * they are empty so simply draw the ones we can, skip the rest */
    for(char i = 0; i < 17; i++) {
        entry_number--;
        if(entry_number == -1) {
            entry_number = MONITOR_LOG_COUNT - 1;
        }

        log = &monitors[monitor_selection].log[entry_number];
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
    if(monitors[monitor_selection].current_log == 0 &&
        monitors[monitor_selection].log[0].time == 0) {

        GLCD.CursorTo(0, 0);
        GLCD.print("No log");
    }
}

/*******************************************************************************/
void
handleLogInput(int button) {
    switch(button) {
        case BUTTON_0:
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


