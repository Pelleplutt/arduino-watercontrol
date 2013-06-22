#include "greenhouse.h"


/*******************************************************************************/
void drawLog() {
    GLCD.CursorTo(0, 0);
    GLCD.print("Log");
}

/*******************************************************************************/
void handleLogInput(int button) {
  switch(button) {
    case 0: {
      draw();
      break;
    }
    case 2: {
      switchScreen(SCREEN_CONFIG);
    }
  }
}


