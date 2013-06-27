#include "greenhouse.h"

/*******************************************************************************/
void drawOverview() {

    GLCD.SelectFont(System5x7, BLACK);
    for(int i = 0; i < 8; i++) {

        if(i == monitor_selection) {
            GLCD.EraseTextLine(0);
            GLCD.print(monitors[i].name);


            drawSensorBar(monitors[i].current_value, monitors[i].calibrated_min, monitors[i].calibrated_max);
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
