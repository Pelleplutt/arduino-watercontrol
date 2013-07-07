#include "greenhouse.h"

char configmenu[8][12] = {"Name", "Calibrate", "Water dur", "Trigger", "Manual", "Enabled", "Measure", "Interval"};
int config_selection = 0;
int active_config = -1;

/*******************************************************************************/
char config_edit_name[NAME_MAX_LEN + 1];
char config_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVXYZabcdefghijklmnopqrstuvxyz0123456789., \x7e\x7f";
char config_alphabet_pos;

char config_monitor_changed = 0;

void
drawConfigEditName() {
    GLCD.ClearScreen();
    GLCD.SelectFont(System5x7, BLACK);
    GLCD.print(config_edit_name);
    GLCD.CursorTo(strlen(config_edit_name), 0);
    GLCD.SetFontColor(WHITE);
    GLCD.PutChar(' ');


    GLCD.SetFontColor(BLACK);
    GLCD.CursorTo(0, 1);

    int alen = strlen(config_alphabet);
    for(int i = config_alphabet_pos - DISPLAY_X_MIDDLE; i < config_alphabet_pos + DISPLAY_X_MIDDLE - 1; i++) {
        int ch = i;
        if(ch < 0) {
            ch = alen + ch;
        } else if (ch >= alen) {
            ch = ch - alen;
        }
        GLCD.PutChar(config_alphabet[ch]);
    }
    GLCD.SetFontColor(WHITE);
    GLCD.CursorTo(DISPLAY_X_MIDDLE, 1);
    GLCD.PutChar(config_alphabet[config_alphabet_pos]);

}

void
initConfigName() {
    strcpy(config_edit_name, monitors[monitor_selection].name);
    config_alphabet_pos = strlen(config_alphabet) - 1;
    drawConfigEditName();
}

void
drawConfigName() {
    GLCD.SelectFont(Arial_bold_14, BLACK);
    GLCD.CursorTo(0, 0);
    GLCD.print(monitors[monitor_selection].name);
}

void
handleConfigNameInput(int button) {
    int slen = strlen(config_edit_name);
    switch(button) {
        case BUTTON_0:
            {
                config_alphabet_pos = (++config_alphabet_pos) % strlen(config_alphabet);
                drawConfigEditName();
                break;
            }
        case BUTTON_1:
            {
                if(config_alphabet_pos) {
                    config_alphabet_pos--;
                } else {
                    config_alphabet_pos = strlen(config_alphabet) - 1;

                }
                drawConfigEditName();
                break;
            }
        case BUTTON_2:
            {
                if(config_alphabet[config_alphabet_pos] == 0x7f) {
                    if(slen > 0) {
                        config_edit_name[slen - 1] = '\0';
                    }
                } else if(config_alphabet[config_alphabet_pos] == 0x7e) {
                    strcpy(monitors[monitor_selection].name, config_edit_name);
                    config_monitor_changed = 1;
                    active_config = -1;
                    draw();
                } else if(slen < NAME_MAX_LEN) {
                    config_edit_name[slen] = config_alphabet[config_alphabet_pos];
                    config_edit_name[slen + 1] = '\0';
                }
                drawConfigEditName();
                break;
            }
        case BUTTON_3:
            {
                active_config = -1;
                draw();
                break;
            }
    }
}

/*******************************************************************************/
unsigned int config_edit_water_duration;

void
drawConfigEditWaterDuration() {
    GLCD.ClearScreen();
    GLCD.SelectFont(Arial_bold_14);
    GLCD.print(timeString((unsigned long)config_edit_water_duration));
}

void
initConfigWaterDuration() {
    config_edit_water_duration = monitors[monitor_selection].water_duration;
    drawConfigEditWaterDuration();
}

void
drawConfigWaterDuration() {
    GLCD.SelectFont(Arial_bold_14);
    GLCD.print(timeString((unsigned long)monitors[monitor_selection].water_duration));
}

void
handleConfigWaterDurationInput(int button) {
    switch(button) {
        case BUTTON_0:
            {
                if(config_edit_water_duration < MAX_WATER_DURATION) {
                    if(config_edit_water_duration >= 180) {
                        config_edit_water_duration += 60;
                    } else {
                        config_edit_water_duration += 10;
                    }
                } else {
                    config_edit_water_duration = 0;
                }
                drawConfigEditWaterDuration();
                break;
            }
        case BUTTON_1:
            {
                if(config_edit_water_duration > 0) {
                    if(config_edit_water_duration >= 180) {
                        config_edit_water_duration -= 60;
                    } else {
                        config_edit_water_duration -= 10;
                    }
                } else {
                    config_edit_water_duration = MAX_WATER_DURATION;
                }
                drawConfigEditWaterDuration();
                break;
            }
        case BUTTON_2:
            {
                monitors[monitor_selection].water_duration = config_edit_water_duration;
                config_monitor_changed = 1;
                /* No break */
            }
        case BUTTON_3:
            {
                active_config = -1;
                draw();
                break;
            }
    }
}

/*******************************************************************************/
unsigned long  config_edit_water_interval;

void
drawConfigEditInterval() {
    GLCD.ClearScreen();
    GLCD.SelectFont(Arial_bold_14);
    if(config_edit_water_interval == 0) {
        GLCD.print("OFF");
    } else {
        GLCD.print(timeString(config_edit_water_interval));
    }
}

void
initConfigInterval() {
    config_edit_water_interval = monitors[monitor_selection].water_interval;
    drawConfigEditInterval();
}

void
drawConfigInterval() {
    GLCD.SelectFont(Arial_bold_14);
    if(monitors[monitor_selection].water_interval == 0) {
        GLCD.print("OFF");
    } else {
        GLCD.print(timeString(monitors[monitor_selection].water_interval));
    }
}

void
handleConfigIntervalInput(int button) {
    switch(button) {
        case BUTTON_0:
            {
                if(config_edit_water_interval < MAX_WATER_INTERVAL) {
                    if(config_edit_water_interval >= 4 * 3600) {
                        config_edit_water_interval += 20 * 60;
                    } else {
                        config_edit_water_interval += 10 * 60;
                    }
                } else {
                    config_edit_water_interval = 0;
                }
                drawConfigEditInterval();
                break;
            }
        case BUTTON_1:
            {
                if(config_edit_water_interval < 10 * 60) {
                    if(config_edit_water_interval > 0) {
                        config_edit_water_interval = 0;
                    } else {
                        config_edit_water_interval = MAX_WATER_INTERVAL;
                    }
                }else {
                    if(config_edit_water_interval > 4 * 3600) {
                        config_edit_water_interval -= 20 * 60;
                    } else {
                        config_edit_water_interval -= 10 * 60;
                    }
                }
                drawConfigEditInterval();
                break;
            }
        case BUTTON_2:
            {
                monitors[monitor_selection].water_interval = config_edit_water_interval;
                config_monitor_changed = 1;
                /* No break */
            }
        case BUTTON_3:
            {
                active_config = -1;
                draw();
                break;
            }
    }
}

/*******************************************************************************/
int  config_edit_trigger;

void
_drawConfigEditTrigger(int value) {
    GLCD.SelectFont(Arial_bold_14, BLACK);
    if(value == 0) {
        GLCD.print("OFF");
    } else {
        drawSensorBar(value, 0, 100);
    }
}

void
drawConfigEditTrigger() {
    GLCD.ClearScreen();
    _drawConfigEditTrigger(config_edit_trigger);
}

void
initConfigTrigger() {
    config_edit_trigger = monitors[monitor_selection].trigger_value;
    drawConfigEditTrigger();
}

void
drawConfigTrigger() {
    _drawConfigEditTrigger(monitors[monitor_selection].trigger_value);
}

void
handleConfigTriggerInput(int button) {
    switch(button) {
        case BUTTON_0:
            {
                config_edit_trigger = (++config_edit_trigger) % 100;
                drawConfigEditTrigger();
                break;
            }
        case BUTTON_1:
            {
                if(config_edit_trigger == 0) {
                    config_edit_trigger = 99;
                } else {
                    config_edit_trigger--;
                }
                drawConfigEditTrigger();
                break;
            }
        case BUTTON_2:
            {
                monitors[monitor_selection].trigger_value = config_edit_trigger;
                config_monitor_changed = 1;
                /* No break */
            }
        case BUTTON_3:
            {
                active_config = -1;
                draw();
                break;
            }
    }
}

/*******************************************************************************/
char  config_edit_enabled;

void
_drawConfigEditEnabled(char value) {
    GLCD.SelectFont(Arial_bold_14, BLACK);
    if(value == 0) {
        GLCD.print("Disabled");
    } else {
        GLCD.print("Enabled");
    }
}

void
drawConfigEditEnabled() {
    GLCD.ClearScreen();
    _drawConfigEditEnabled(config_edit_enabled);
}

void
initConfigEnabled() {
    config_edit_enabled = monitors[monitor_selection].enabled;
    drawConfigEditEnabled();
}

void
drawConfigEnabled() {
    _drawConfigEditEnabled(monitors[monitor_selection].enabled);
}

void
handleConfigEnabledInput(int button) {
    switch(button) {
        case BUTTON_0:
        case BUTTON_1:
            {
                config_edit_enabled ^= 1;
                drawConfigEditEnabled();
                break;
            }
        case BUTTON_2:
            {
                monitors[monitor_selection].enabled = config_edit_enabled;
                config_monitor_changed = 1;
                /* No break */
            }
        case BUTTON_3:
            {
                active_config = -1;
                draw();
                break;
            }
    }
}

/*******************************************************************************/
void
_drawConfigManual() {
    GLCD.SelectFont(Arial_bold_14, BLACK);
    if(monitors[monitor_selection].water_state == WATER_OPEN) {
        GLCD.print("Open, close in ");
        GLCD.CursorTo(0, 1);
        GLCD.print(timeString((monitors[monitor_selection].water_opened_at + monitors[monitor_selection].water_open_for) - now_s));
    } else {
        GLCD.print("Closed");
    }
}

void
drawConfigEditManual() {
    GLCD.ClearScreen();
    _drawConfigManual();
}

void
initConfigManual() {
    drawConfigEditManual();
}

void
drawConfigManual() {
    _drawConfigManual();
}

void
handleConfigManualInput(int button) {
    switch(button) {
        case BUTTON_0:
        case BUTTON_1:

            {
                if(monitors[monitor_selection].water_state == WATER_OPEN) {
                    changeWaterPort(monitor_selection, WATER_CLOSED, MANUAL_WATER_OPEN_TIME, OPEN_MODE_MANUAL);
                    appendToSystemLog(monitor_selection, SYSTEM_LOG_EVENT_TYPE_MANUAL_CLOSE, monitors[monitor_selection].current_value, now_s);
                } else {
                    changeWaterPort(monitor_selection, WATER_OPEN, MANUAL_WATER_OPEN_TIME, OPEN_MODE_MANUAL);
                    appendToSystemLog(monitor_selection, SYSTEM_LOG_EVENT_TYPE_MANUAL_OPEN, monitors[monitor_selection].current_value, now_s);
                }
                drawConfigEditManual();
                break;
            }
        case BUTTON_3:
            {
                active_config = -1;
                draw();
                break;
            }
    }
}

/*******************************************************************************/

void
drawConfigEditMeasure(bool measuring) {
    GLCD.SelectFont(Arial_bold_14, BLACK);
    GLCD.ClearScreen();
    if(measuring) {
        GLCD.print("Sensing...");
    } else {
        GLCD.print("Triggers: ");
        GLCD.print(monitors[monitor_selection].current_value);
    }

    drawSensorBar(monitors[monitor_selection].current_value,
            monitors[monitor_selection].calibrated_min,
            monitors[monitor_selection].calibrated_max);
}

void
drawConfigMeasure() {
}

void
initConfigMeasure() {
    if(!measuring) {
        drawConfigEditMeasure(true);
        measure(monitor_selection, false);
        drawConfigEditMeasure(false);
    } else {
        active_config = -1;
        draw();
    }
}

void
handleConfigMeasureInput(int button) {
    switch(button) {
        case BUTTON_0:
        case BUTTON_1:
        case BUTTON_2:

            {
                break;
            }
        case BUTTON_3:
            {
                active_config = -1;
                draw();
                break;
            }
    }
}

/*******************************************************************************/
/* Current state of ongoing calibration */
unsigned int config_calibrate_low = 0;
unsigned int config_calibrate_high = 0;
int config_calibrate_state = -1;

void
_drawConfigCalibrateHighLow(unsigned int high, unsigned int low) {
    GLCD.print("Low: ");
    if(low) {
        GLCD.print(low);
    } else {
        GLCD.print("NOT SET");
    }
    GLCD.CursorTo(0, 1);

    GLCD.print("High: ");
    if(high) {
        GLCD.print(high);
    } else {
        GLCD.print("NOT SET");
    }
}

void
drawConfigCalibrate() {
    GLCD.SelectFont(Arial_bold_14, BLACK);
    _drawConfigCalibrateHighLow(monitors[monitor_selection].calibrated_max,
            monitors[monitor_selection].calibrated_min);
}

void
drawConfigEditCalibrate() {
    GLCD.ClearScreen();
    GLCD.SelectFont(Arial_bold_14, BLACK);

    switch(config_calibrate_state) {
        case 0:
            GLCD.print("Hold in AIR");
            GLCD.CursorTo(0, 1);
            GLCD.print("Then press 1");

            break;

        case 1:
            GLCD.print("Sensing...");

            break;

        case 2:
            _drawConfigCalibrateHighLow(config_calibrate_high,
                    config_calibrate_low);
            break;

        case 3:
            GLCD.print("Put in WATER");
            GLCD.CursorTo(0, 1);
            GLCD.print("Then press 1");

            break;

        case 4:
            GLCD.print("Sensing...");

            break;

        case 5:
            _drawConfigCalibrateHighLow(config_calibrate_high,
                    config_calibrate_low);
            break;

        default:
            _drawConfigCalibrateHighLow(monitors[monitor_selection].calibrated_max,
                    monitors[monitor_selection].calibrated_min);
    }
}

void
initConfigCalibrate() {
    config_calibrate_state = 0;
    config_calibrate_low = 0;
    config_calibrate_high = 0;

    drawConfigEditCalibrate();
}

void
handleConfigCalibrateInput(int button) {
        /* A bit crude, but dont mess up the existing measurements */
    if(measuring) {
        return;
    }
    switch(button) {
        case BUTTON_1:
        case BUTTON_0:
            switch(config_calibrate_state) {
                case 0:
                    config_calibrate_state++; //1
                    drawConfigEditCalibrate();

                    config_calibrate_low = measure(monitor_selection, true);

                    config_calibrate_state++; //2
                    drawConfigEditCalibrate();


                    break;
                case 2:
                    config_calibrate_state++; //3
                    drawConfigEditCalibrate();

                    break;

                case 3:
                    config_calibrate_state++; //4
                    drawConfigEditCalibrate();
                    config_calibrate_high = measure(monitor_selection, true);

                    config_calibrate_state++; //5
                    drawConfigEditCalibrate();

                    config_calibrate_state++; //6
                    break;

            }

            break;
        case BUTTON_2:
            switch(config_calibrate_state) {
                case 6:
                    monitors[monitor_selection].calibrated_min = config_calibrate_low;
                    monitors[monitor_selection].calibrated_max = config_calibrate_high;

                    config_calibrate_state = -1;
                    config_monitor_changed = 1;
                    active_config = -1;
                    draw();

                    break;

                default:
                    config_calibrate_state = 0;
                    drawConfigEditCalibrate();

                    break;
            }
            break;
        case BUTTON_3:
            {
                config_calibrate_state = -1;
                active_config = -1;
                draw();
                break;
            }
    }
}

/*******************************************************************************/
void
drawConfig1() {
    GLCD.SetFontColor(BLACK);
    GLCD.ClearScreen();

    if(active_config == -1) {
        switch(config_selection) {
            case 0:
                drawConfigName();
                break;
            case 1:
                drawConfigCalibrate();
                break;
            case 2:
                drawConfigWaterDuration();
                break;
            case 3:
                drawConfigTrigger();
                break;
            case 4:
                drawConfigManual();
                break;
            case 5:
                drawConfigEnabled();
                break;
            case 6:
                drawConfigMeasure();
                break;
            case 7:
                drawConfigInterval();
                break;
        }
    } else {
        switch(active_config) {
            case 0:
                drawConfigEditName();
                break;
            case 1:
                drawConfigEditCalibrate();
                break;
            case 2:
                drawConfigEditWaterDuration();
                break;
            case 3:
                drawConfigEditTrigger();
                break;
            case 4:
                drawConfigEditManual();
                break;
            case 5:
                drawConfigEditEnabled();
                break;
            case 6:
                drawConfigEditMeasure(false);
                break;
            case 7:
                drawConfigEditInterval();
                break;
        }
    }
    GLCD.SelectFont(System5x7, BLACK);


    for(int i = 0; i < 8; i++) {
        if(i < 4) {
            GLCD.CursorTo(0, DISPLAY_Y_MIDDLE + i);
        } else {
            GLCD.CursorTo(DISPLAY_X_MIDDLE, DISPLAY_Y_MIDDLE + i -4);
        }

        if(i == config_selection) {
            GLCD.SetFontColor(WHITE);
        }

        GLCD.print(configmenu[i]);
        for(int pad = strlen(configmenu[i]); pad < 10; pad++) {
            GLCD.PutChar(' ');
        }

        if(i == config_selection) {
            GLCD.SetFontColor(BLACK);
        }
    }
}

/*******************************************************************************/
void
handleConfig1Input(int button) {

    if(active_config == -1) {
        switch(button) {
            case BUTTON_0:
                {
                    config_selection = (++config_selection) % 8;
                    draw();
                    break;
                }
            case BUTTON_1:
                {
                    if(config_selection) {
                        config_selection--;
                    } else {
                        config_selection = 7;
                    }
                    draw();
                    break;
                }
            case BUTTON_2:
                {
                    switch(config_selection) {
                        case 0:
                            initConfigName();
                            break;
                        case 1:
                            initConfigCalibrate();
                            break;
                        case 2:
                            initConfigWaterDuration();
                            break;
                        case 3:
                            initConfigTrigger();
                            break;
                        case 4:
                            initConfigManual();
                            break;
                        case 5:
                            initConfigEnabled();
                            break;
                        case 6:
                            initConfigMeasure();
                            break;
                        case 7:
                            initConfigInterval();
                            break;

                    }
                    active_config = config_selection;

                    break;
                }
            case BUTTON_3:
                {
                    if(config_monitor_changed) {
                        GLCD.ClearScreen();
                        GLCD.print("Saving....");
                        saveMonitor(monitor_selection);
                        config_monitor_changed = 0;
                    }

                    switchScreen(SCREEN_SYSTEM_LOG);
                    break;
                }
        }
    }else {
        switch(active_config)
        {
            case 0:
                {
                    handleConfigNameInput(button);
                    break;
                }
            case 1:
                {
                    handleConfigCalibrateInput(button);
                    break;
                }
            case 2:
                {
                    handleConfigWaterDurationInput(button);
                    break;
                }
            case 3:
                {
                    handleConfigTriggerInput(button);
                    break;
                }
            case 4:
                {
                    handleConfigManualInput(button);
                    break;
                }
            case 5:
                {
                    handleConfigEnabledInput(button);
                    break;
                }
            case 6:
                {
                    handleConfigMeasureInput(button);
                    break;
                }
            case 7:
                {
                    handleConfigIntervalInput(button);
                    break;
                }
        }
    }
}
