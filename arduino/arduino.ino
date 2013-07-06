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
        offset = _eeprom_read_bytes(monitornum, offset, (char*)&mon->water_duration,   (unsigned int)sizeof(mon->water_duration));
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
    offset = _eeprom_write_bytes(monitornum, offset, (char*)&mon->water_duration,   (unsigned int)sizeof(mon->water_duration));
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
    last_auto_redraw = millis();
    switch(current_screen) {
        case SCREEN_OVERVIEW:
            {
                drawOverview();
                break;
            }
        case SCREEN_LOG_OPEN:
            {
                drawOpenLog();
                break;
            }
        case SCREEN_LOG_SENSE:
            {
                drawSenseLog();
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
        case SCREEN_LOG_OPEN:
            {
                handleOpenLogInput(button);
                break;
            }
        case SCREEN_LOG_SENSE:
            {
                handleSenseLogInput(button);
                break;
            }
        case SCREEN_CONFIG1:
            {
                handleConfig1Input(button);
                break;
            }

    }
}

void
showMonitorAction(char monitor, char *action) {
    GLCD.ClearScreen();
    GLCD.DrawRect(0, 0, GLCD.Width - 1, GLCD.Height - 1, BLACK);
    GLCD.SelectFont(Arial_bold_14, BLACK);
    GLCD.CursorTo(1, 1);
    GLCD.print(monitors[monitor].name);
    GLCD.CursorTo(1, 2);
    GLCD.print(action);
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

    char needs_redraw = 0;

    if(now_s > 0) {
        for(i = 0; i < 8; i++) {
            monitor *mon = &monitors[i];
            if(mon->water_state == WATER_CLOSED) {
                if(mon->enabled && open_ports < MAX_OPEN_WATER_PORTS) {
                    if(mon->water_interval > 0) {
                        unsigned long seconds_since = now_s - mon->last_water_open;

                        if(seconds_since > mon->water_interval) {
                            showMonitorAction(i, "Open Interval");
                            needs_redraw = 1;
                            changeWaterPort(i, WATER_OPEN, mon->water_duration, OPEN_MODE_INTERVAL);
                            delay(2000);
                            open_ports++;
                        }
                            /* Only do a sensor reading if we have proper
                             * calibrated values and a trigger value */
                    }

                        /* Re-check the water_state here as we might very well
                         * have opened the port before */
                    if(!measuring && mon->water_state == WATER_CLOSED &&
                        mon->trigger_value && mon->calibrated_min &&
                        mon->calibrated_max) {

                        unsigned long seconds_since = now_s - mon->last_sensor_read;

                        if(seconds_since > SENSOR_MEASURE_INTERVAL || mon->last_sensor_read == 0) {
                            char status_str[20];
                            showMonitorAction(i, "Sensing...");
                            needs_redraw = 1;
                            measure(i, false);
                            mon->last_sensor_read = now_s;

                            int value = getSensorCalibratedPercent(mon->current_value, mon->calibrated_min, mon->calibrated_max);
                            if(value > mon->trigger_value) {
                                sprintf(status_str, "Sensed %d%%, open!", value);
                                showMonitorAction(i, status_str);
                                changeWaterPort(i, WATER_OPEN, mon->water_duration, OPEN_MODE_SENSOR);
                                open_ports++;
                                delay(2000);
                            } else {
                                sprintf(status_str, "Sensed %d%%", value);
                                showMonitorAction(i, status_str);
                                delay(1000);
                            }
                        }
                    }
                }
            }else {
                open_ports++;

                    /* Did the timer wrap ? attempt to recover */
                if(mon->water_opened_at > now_s) {
                        /* First, adjust how much "millis" we had left when opened */
                    unsigned long time_before_wrap = MILLIS_MAX / 1000 - mon->water_opened_at;
                        /* Then reset open until now and reduce open time with time until wrap */
                    mon->water_opened_at = 0;
                        /* Failsafe this ... */
                    if(mon->water_open_for < time_before_wrap) {
                        mon->water_open_for = 0;
                    } else {
                        mon->water_open_for = mon->water_open_for - time_before_wrap;
                    }
                }

                if(now_s >= (mon->water_opened_at + mon->water_open_for)) {
                    showMonitorAction(i, "Close");
                    needs_redraw = 1;
                    changeWaterPort(i, WATER_CLOSED, 0, 0);
                    delay(2000);
                    open_ports--;
                }
            }
        }
    }

    if(current_screen == SCREEN_OVERVIEW || current_screen == SCREEN_LOG_OPEN || current_screen == SCREEN_LOG_SENSE) {
            /* Redraw the screen every second */
        if(now - last_auto_redraw >= 1000) {
            needs_redraw = 1;
        }
    }

    if(needs_redraw) {
        draw();
    }
}
