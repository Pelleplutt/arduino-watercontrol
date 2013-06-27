#include "greenhouse.h"


/*******************************************************************************/
void drawLog() {
    GLCD.CursorTo(0, 0);
    GLCD.print("Log");
}

/*******************************************************************************/
void handleLogInput(int button) {
    switch(button) {
        case BUTTON_0: 
            {
                draw();
                break;
            }
        case BUTTON_3: 
            {

                switchScreen(SCREEN_CONFIG);
            }
    }
}


