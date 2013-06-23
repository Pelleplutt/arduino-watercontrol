#include <glcd.h>
#include <glcd_Buildinfo.h>
#include <glcd_watercontrol.h>

#include "greenhouse.h"
#include "ports.h"
#include "fonts/SystemFont5x7.h"


unsigned long last_button_read = 0;

  /* Currently selected monitor from first screen */
int monitor_selection = 0;

int current_screen = SCREEN_OVERVIEW;
monitor  monitors[8];

/*******************************************************************************/
char *timeString(unsigned long seconds) {
  static char buf[32];
  int len = 0;

  *buf = 0;
  if(seconds > 86400) {
    len += snprintf(buf + len, 32 - len, "%dd", seconds / 86400);
    seconds = seconds % 86400; 
  }
  
  if(seconds > 3600) {
    len += snprintf(buf + len, 32 - len, "%dh", seconds / 3600);
    seconds = seconds % 3600; 
  }

  if(seconds > 60) {
    len += snprintf(buf + len, 32 - len, "%dm", seconds / 60);
    seconds = seconds % 60; 
  }
  
  if(seconds % 60) {
    len += snprintf(buf + len, 32 - len, "%dm", seconds / 60);
  }
    
  
  return buf;
}

void changeWaterPort(unsigned char monitor, unsigned char state) {
  monitors[monitor].water_state = state;
  if(state) {
    digitalWrite(monitors[monitor].water_port, HIGH);
  } else {
    digitalWrite(monitors[monitor].water_port, LOW);
  }
}

/*******************************************************************************/
int getSensorValue(int value, unsigned long calibrated_min, unsigned long calibrated_max) {
  if(value >= 0) {
    value -= calibrated_min;
    calibrated_max -= calibrated_min;
    unsigned long percent = value * 100 / calibrated_max;

    return percent;
  }
  
  return -1;
}

/*******************************************************************************/
void drawSensorBar(int value, unsigned long calibrated_min, unsigned long calibrated_max) {
  if(value >= 0) {
    
    int percent = getSensorValue(value, calibrated_min, calibrated_max);
    int pixels = (GLCD.Width - 1) * percent / 100;
    
    if(percent == 100) {
      GLCD.CursorTo(17, 0);
    } else if(percent >= 10) {
      GLCD.CursorTo(18, 0);
    } else {
      GLCD.CursorTo(19, 0);
    }
    GLCD.print(percent, DEC);
    GLCD.PutChar('%');

    
    GLCD.FillRect(0, 10, GLCD.Width - 1, 10, WHITE);
    GLCD.DrawRect(0, 10, GLCD.Width - 1, 10, BLACK);
    GLCD.FillRect(0, 10, pixels, 10, BLACK);
  } else {
    GLCD.FillRect(0, 10, GLCD.Width - 1, 10, WHITE);
  }
}


/*******************************************************************************/
void setup() {

  Serial.begin(9600);

  pinMode(13, OUTPUT);

    /* Setup ports for water control */
  pinMode(PORT_RELAY0, OUTPUT);
  pinMode(PORT_RELAY1, OUTPUT);
  pinMode(PORT_RELAY2, OUTPUT);
  pinMode(PORT_RELAY3, OUTPUT);
  pinMode(PORT_RELAY4, OUTPUT);
  pinMode(PORT_RELAY5, OUTPUT);
  pinMode(PORT_RELAY6, OUTPUT);
  pinMode(PORT_RELAY7, OUTPUT);

    /* Setup ports for pushbuttons */
  pinMode(PORT_BTN0, INPUT);
  pinMode(PORT_BTN1, INPUT);
  pinMode(PORT_BTN2, INPUT);
  pinMode(PORT_BTN3, INPUT);

    /* Setup port for sensors, Adress lines and enablers */
  pinMode(PORT_SENSE0_A0, OUTPUT);
  pinMode(PORT_SENSE0_A1, OUTPUT);
  pinMode(PORT_SENSE0_E, OUTPUT);

  pinMode(PORT_SENSE1_A0, OUTPUT);
  pinMode(PORT_SENSE1_A1, OUTPUT);
  pinMode(PORT_SENSE1_E, OUTPUT);

    /* FIXME Setup interrupts for sensors */


    /*  Initialize monitor structure, this way we know it
        contains nothing wrong at least */
  memset(monitors, sizeof(monitors), 0);

  strcpy(monitors[0].name, "Port a0");
  strcpy(monitors[1].name, "Port a1");
  strcpy(monitors[2].name, "Port b2");
  strcpy(monitors[3].name, "Port b3");
  strcpy(monitors[4].name, "Port c4");
  strcpy(monitors[5].name, "Port c5");
  strcpy(monitors[6].name, "Port d6");
  strcpy(monitors[7].name, "Port d7");

  for(int i = 0; i < 8; i++) {
      /* Given the current hardware and limitations we will
         max out at around 7000 when the sensors are shorted */
    monitors[i].calibrated_max = 7000;
      /* The sensors will idle at around 48hz when in open
         air */
    monitors[i].calibrated_min = 48;

    monitors[i].enabled = 1;
  }

    /* At some point we might want to disconnect the 1-1
       matching between the given input signal sensor
       number and the output water flow valve number */
  monitors[0].water_port = PORT_RELAY0;
  monitors[1].water_port = PORT_RELAY1;
  monitors[2].water_port = PORT_RELAY2;
  monitors[3].water_port = PORT_RELAY3;
  monitors[4].water_port = PORT_RELAY4;
  monitors[5].water_port = PORT_RELAY5;
  monitors[6].water_port = PORT_RELAY6;
  monitors[7].water_port = PORT_RELAY7;

  GLCD.Init();
  GLCD.ClearScreen();
  draw();
}


/*******************************************************************************/
void draw() {
  switch(current_screen) {
    case SCREEN_OVERVIEW: {
      drawOverview();
      break;
    }
    case SCREEN_LOG: {
      drawLog();
      break;
    }
    case SCREEN_CONFIG: {
      drawConf();
      break;
    }
      
  }
}

/*******************************************************************************/
void switchScreen(int screen) {
  GLCD.ClearScreen();
  current_screen = screen;
  draw();
}



/*******************************************************************************/
void handleInput(int button) {
  switch(current_screen) {
    case SCREEN_OVERVIEW: {
      handleOverviewInput(button);
      break;
    }
    case SCREEN_LOG: {
      handleLogInput(button);
      break;
    }
    case SCREEN_CONFIG: {
      handleConfInput(button);
      break;
    }

  }
}

/*******************************************************************************/
void loop() {



    /* Failsafe */
  if(millis() < last_button_read) {
    last_button_read = 0;
  }

  if(millis() - last_button_read > BUTTON_DELAY) {
    if(digitalRead(PORT_BTN0) == HIGH) {
      handleInput(0);
      last_button_read = millis();
    }else if(digitalRead(PORT_BTN1) == HIGH) {
      handleInput(1);
      last_button_read = millis();
    }else if(digitalRead(PORT_BTN2) == HIGH) {
      handleInput(2);
      last_button_read = millis();
    }else if(digitalRead(PORT_BTN3) == HIGH) {
      handleInput(3);
      last_button_read = millis();
    }
  }

}
