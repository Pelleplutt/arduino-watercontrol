#include <glcd_Config.h>
#include <glcd.h>
#include <glcd_Buildinfo.h>

#include "greenhouse.h"
#include "ports.h"
#include <fonts/SystemFont5x7.h>


unsigned long last_button_read = 0;

    /* Current time cached to keep it consistent in the iteration */
unsigned long now;
unsigned long now_s;

    /* The last millis() we redrawed the overview screen. Use this for auto
     * refreshing the overview screen */
unsigned long last_overview_redraw = 0;

  /* Currently selected monitor from first screen */
int monitor_selection = 0;

  /* Currently displaying screen */
int current_screen = SCREEN_OVERVIEW;
monitor  monitors[8];

/*******************************************************************************/
void
setup() {

#ifdef SERIAL_DEBUG
    Serial.begin(9600);
#endif

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
    strcpy(monitors[2].name, "Port a2");
    strcpy(monitors[3].name, "Port a3");
    strcpy(monitors[4].name, "Port b0");
    strcpy(monitors[5].name, "Port b1");
    strcpy(monitors[6].name, "Port b2");
    strcpy(monitors[7].name, "Port b3");

    for(int i = 0; i < 8; i++) {

        /* Given the current hardware and limitations we will
           max out at around 7000 when the sensors are shorted */
        monitors[i].calibrated_max = 7000;

        /* The sensors will idle at around 48hz when in open
           air */
        monitors[i].calibrated_min = 48;

        /* Default to water for 3s once we turn the switch on */
        monitors[i].water_duration = 30;

        /* Set current value to invalid */
        monitors[i].current_value = SENSE_VALUE_INVALID;

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
void
draw() {
    switch(current_screen) {
        case SCREEN_OVERVIEW:
            {
                drawOverview();
                last_overview_redraw = millis();
                break;
            }
        case SCREEN_LOG:
            {
                drawLog();
                break;
            }
        case SCREEN_CONFIG1:
            {
                drawConfig1();
                break;
            }
    }
}

/*******************************************************************************/
void
switchScreen(int screen) {
    GLCD.ClearScreen();
    current_screen = screen;
    draw();
}



/*******************************************************************************/
void
handleInput(int button) {

    DEBUG("Input: ");
    DEBUGLN(button);

    switch(current_screen) {
        case SCREEN_OVERVIEW:
            {
                handleOverviewInput(button);
                break;
            }
        case SCREEN_LOG:
            {
                handleLogInput(button);
                break;
            }
        case SCREEN_CONFIG1:
            {
                handleConfig1Input(button);
                break;
            }

    }
}

/*******************************************************************************/
void
loop() {

    now = millis();
    now_s = now / 1000;
    unsigned char i;

    /* Failsafe */
    if(now < last_button_read) {
        last_button_read = now;
    }

    if(now - last_button_read > BUTTON_DELAY) {
        if(digitalRead(PORT_BTN0) == HIGH) {
            handleInput(0);
            last_button_read = now;
        }else if(digitalRead(PORT_BTN1) == HIGH) {
            handleInput(1);
            last_button_read = now;
        }else if(digitalRead(PORT_BTN2) == HIGH) {
            handleInput(2);
            last_button_read = now;
        }else if(digitalRead(PORT_BTN3) == HIGH) {
            handleInput(3);
            last_button_read = now;
        }
    }

    unsigned char open_ports = 0;

    for(i = 0; i < 8; i++) {
        if(monitors[i].water_state == WATER_CLOSED) {
            if(monitors[i].water_interval > 0) {
                unsigned long seconds_since = now_s - monitors[i].last_water_open;

                if(open_ports < MAX_OPEN_WATER_PORTS) {
                    if(seconds_since > monitors[i].water_interval) {
                        changeWaterPort(i, WATER_OPEN, monitors[i].water_duration, OPEN_MODE_INTERVAL);
                        open_ports++;
                    }
                }
            }
        }else {
            open_ports++;

                /* Did the timer wrap ? attempt to recover */
            if(monitors[i].water_opened_at > now_s) {
                    /* First, adjust how much "millis" we had left when opened */
                unsigned long time_before_wrap = MILLIS_MAX / 1000 - monitors[i].water_opened_at;
                    /* Then reset open until now and reduce open time with time until wrap */
                monitors[i].water_opened_at = 0;
                    /* Failsafe this ... */
                if(monitors[i].water_open_for < time_before_wrap) {
                    monitors[i].water_open_for = 0;
                } else {
                    monitors[i].water_open_for = monitors[i].water_open_for - time_before_wrap;
                }
            }

            if(now_s >= (monitors[i].water_opened_at + monitors[i].water_open_for)) {
                changeWaterPort(i, WATER_CLOSED, 0, 0);
                open_ports--;
            }
        }
    }

    if(current_screen == SCREEN_OVERVIEW) {
            /* Redraw the screen every second */
        if(now - last_overview_redraw >= 1000) {
            draw();
        }
    }
}
