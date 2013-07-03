#include <glcd_Config.h>
#include <glcd.h>
#include <glcd_Buildinfo.h>

#include "greenhouse.h"
#include "ports.h"
#include <fonts/SystemFont5x7.h>
#include <EEPROM.h>


unsigned long last_button_read = 0;

    /* Current time cached to keep it consistent in the iteration */
unsigned long now;
unsigned long now_s;

    /* The last millis() we redrawed the log or overview screen. Use this for auto
     * refreshing the overview screen */
unsigned long last_auto_redraw = 0;

  /* Currently selected monitor from first screen */
int monitor_selection = 0;

  /* Currently displaying screen */
int current_screen = SCREEN_OVERVIEW;
monitor  monitors[8];

unsigned int
_eeprom_write_bytes(unsigned char monitor, unsigned int offset, char *data, unsigned int count) {

    offset = monitor * EEPROMBYTES_PER_MONITOR + offset;
    while(count--) {
        EEPROM.write(offset++, *data);
        data++;
    }

    return offset;
}

unsigned int
_eeprom_read_bytes(unsigned char monitor, unsigned int offset, char *data, unsigned int count) {

    offset = monitor * EEPROMBYTES_PER_MONITOR + offset;
    while(count--) {
        *data = EEPROM.read(offset++);
        data++;
    }

    return offset;
}

void
loadMonitor(unsigned char monitornum) {
    unsigned int offset = 0;

    monitor *mon = &monitors[monitornum];
    unsigned char magic = EEPROM_SENSOR_MAGIC;


    /*  Initialize monitor structure, this way we know it
        contains nothing wrong at least */
    memset(mon, sizeof(monitor), 0);

    offset = _eeprom_read_bytes(monitornum, offset, (char*)&magic,                 (unsigned int)sizeof(magic));
    if(magic == EEPROM_SENSOR_MAGIC) {
        offset = _eeprom_read_bytes(monitornum, offset, (char*)mon->name,              (unsigned int)sizeof(mon->name));
        offset = _eeprom_read_bytes(monitornum, offset, (char*)&mon->enabled,          (unsigned int)sizeof(mon->enabled));
        offset = _eeprom_read_bytes(monitornum, offset, (char*)&mon->water_port,       (unsigned int)sizeof(mon->water_port));
        offset = _eeprom_read_bytes(monitornum, offset, (char*)&mon->calibrated_min,   (unsigned int)sizeof(mon->calibrated_min));
        offset = _eeprom_read_bytes(monitornum, offset, (char*)&mon->calibrated_max,   (unsigned int)sizeof(mon->calibrated_max));
        offset = _eeprom_read_bytes(monitornum, offset, (char*)&mon->trigger_value,    (unsigned int)sizeof(mon->trigger_value));
        offset = _eeprom_read_bytes(monitornum, offset, (char*)&mon->water_interval,   (unsigned int)sizeof(mon->water_interval));
    } else {

        /* At some point we might want to disconnect the 1-1
           matching between the given input signal sensor
           number and the output water flow valve number */
        switch(monitornum) {
            case 0:
                strcpy(mon->name, "Port a0");
                mon->water_port = PORT_RELAY0;
                break;
            case 1:
                strcpy(mon->name, "Port a1");
                mon->water_port = PORT_RELAY1;
                break;
            case 2:
                strcpy(mon->name, "Port a2");
                mon->water_port = PORT_RELAY2;
                break;
            case 3:
                strcpy(mon->name, "Port a3");
                mon->water_port = PORT_RELAY3;
                break;
            case 4:
                strcpy(mon->name, "Port b0");
                mon->water_port = PORT_RELAY4;
                break;
            case 5:
                strcpy(mon->name, "Port b1");
                mon->water_port = PORT_RELAY5;
                break;
            case 6:
                strcpy(mon->name, "Port b2");
                mon->water_port = PORT_RELAY6;
                break;
            case 7:
                strcpy(mon->name, "Port b3");
                mon->water_port = PORT_RELAY7;
                break;
        }

        /* Given the current hardware and limitations we will
           max out at around 7000 when the sensors are shorted */
        mon->calibrated_max = 7000;

        /* The sensors will idle at around 48hz when in open
           air */
        mon->calibrated_min = 48;

        /* Default to water for 3s once we turn the switch on */
        mon->water_duration = 30;

        mon->enabled = 1;
    }

    /* Set current value to invalid */
    mon->current_value = SENSE_VALUE_INVALID;
}

void
saveMonitor(unsigned char monitornum) {
    unsigned int offset = 0;

    monitor *mon = &monitors[monitornum];

    unsigned char magic = EEPROM_SENSOR_MAGIC;
    offset = _eeprom_write_bytes(monitornum, offset, (char*)&magic,                 (unsigned int)sizeof(magic));
    offset = _eeprom_write_bytes(monitornum, offset, (char*)mon->name,              (unsigned int)sizeof(mon->name));
    offset = _eeprom_write_bytes(monitornum, offset, (char*)&mon->enabled,          (unsigned int)sizeof(mon->enabled));
    offset = _eeprom_write_bytes(monitornum, offset, (char*)&mon->water_port,       (unsigned int)sizeof(mon->water_port));
    offset = _eeprom_write_bytes(monitornum, offset, (char*)&mon->calibrated_min,   (unsigned int)sizeof(mon->calibrated_min));
    offset = _eeprom_write_bytes(monitornum, offset, (char*)&mon->calibrated_max,   (unsigned int)sizeof(mon->calibrated_max));
    offset = _eeprom_write_bytes(monitornum, offset, (char*)&mon->trigger_value,    (unsigned int)sizeof(mon->trigger_value));
    offset = _eeprom_write_bytes(monitornum, offset, (char*)&mon->water_interval,   (unsigned int)sizeof(mon->water_interval));
}

void
loadMonitors() {
    for(char i = 0; i < 8; i++) {
        loadMonitor(i);
    }
}

void
saveMonitors() {
    for(char i = 0; i < 8; i++) {
        saveMonitor(i);
    }
}


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

    loadMonitors();

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
                last_auto_redraw = millis();
                break;
            }
        case SCREEN_LOG:
            {
                drawLog();
                last_auto_redraw = millis();
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
        /* For the configuration, keep a flag of wether or not the current
         * monitor has been changed. We need to know if we are to save it to
         * eeprom */
    if(current_screen == SCREEN_CONFIG1) {
        config_monitor_changed = 0;
    }
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

    if(current_screen == SCREEN_OVERVIEW || current_screen == SCREEN_LOG) {
            /* Redraw the screen every second */
        if(now - last_auto_redraw >= 1000) {
            draw();
        }
    }
}
