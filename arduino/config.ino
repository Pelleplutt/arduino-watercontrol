#include "greenhouse.h"

char configmenu[8][12] = {"Name", "Calibrate", "Water dur", "Trigger", "Manual", "Enabled", "Measure", "Interval"};
int config_selection = 0;
int active_config = -1;

/*******************************************************************************/
char config_edit_name[NAME_MAX_LEN + 1];
char config_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVXYZabcdefghijklmnopqrstuvxyz0123456789., \x7e\x7f";
char config_alphabet_pos;

void drawConfigEditName() {
    GLCD.SelectFont(System5x7, BLACK);
    GLCD.SetFontColor(BLACK);
    GLCD.EraseTextLine(0);
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

void initConfigName() {
    strcpy(config_edit_name, monitors[monitor_selection].name);
    config_alphabet_pos = strlen(config_alphabet) - 1;
    drawConfigEditName(); 
}

void drawConfigName() {
    GLCD.SelectFont(Arial_bold_14, BLACK);
    GLCD.CursorTo(0, 0);
    GLCD.print(monitors[monitor_selection].name);
}

void handleConfigNameInput(int button) {
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

void drawConfigEditWater() {
    GLCD.EraseTextLine(0);
    GLCD.print(timeString((unsigned long)config_edit_water_duration));
}

void initConfigWater() {
    config_edit_water_duration = monitors[monitor_selection].water_duration;
    drawConfigEditWater(); 
}

void drawConfigWater() {
    GLCD.EraseTextLine(0);
    GLCD.print(timeString((unsigned long)monitors[monitor_selection].water_duration));
}

void handleConfigWaterInput(int button) {
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
                drawConfigEditWater();
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
                drawConfigEditWater();
                break;
            }
        case BUTTON_2: 
            {
                monitors[monitor_selection].water_duration = config_edit_water_duration;
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

void drawConfigEditInterval() {
    GLCD.EraseTextLine(0);
    if(config_edit_water_interval == 0) {
        GLCD.print("OFF");
    } else {
        GLCD.print(timeString(config_edit_water_interval));
    }
}

void initConfigInterval() {
    config_edit_water_interval = monitors[monitor_selection].water_interval;
    drawConfigEditInterval(); 
}

void drawConfigInterval() {
    GLCD.EraseTextLine(0);
    if(monitors[monitor_selection].water_interval == 0) {
        GLCD.print("OFF");
    } else {
        GLCD.print(timeString(monitors[monitor_selection].water_interval));
    }
}

void handleConfigIntervalInput(int button) {
    switch(button) {
        case 0: 
            {
                if(config_edit_water_interval < MAX_WATER_INTERVAL) {
                    if(config_edit_water_interval >= 6 * 3600) {
                        config_edit_water_interval += 6 * 3600;
                    } else {
                        config_edit_water_interval += 15 * 60;
                    }
                } else {
                    config_edit_water_interval = 0;
                }
                drawConfigEditInterval();
                break;
            }
        case 1: 
            {
                monitors[monitor_selection].water_interval = config_edit_water_interval;
                /* No break */  
            }
        case 2: 
            {
                active_config = -1;
                draw();
                break;
            }
    }
}

/*******************************************************************************/
int  config_edit_trigger;

void _drawConfigEditTrigger(int value) {
    GLCD.EraseTextLine(0);
    if(value == -1) {
        GLCD.print("OFF");
        GLCD.EraseTextLine(1);
    } else {
        drawSensorBar(value, 0, 100);
    }
}

void drawConfigEditTrigger() {
    _drawConfigEditTrigger(config_edit_trigger);
}

void initConfigTrigger() {
    config_edit_trigger = monitors[monitor_selection].trigger_value;
    drawConfigEditTrigger(); 
}

void drawConfigTrigger() {
    _drawConfigEditTrigger(monitors[monitor_selection].trigger_value);
}

void handleConfigTriggerInput(int button) {
    switch(button) {
        case BUTTON_0: 
            {
                if(config_edit_trigger == 99) {
                    config_edit_trigger = -1;
                } else {
                    config_edit_trigger = (++config_edit_trigger) % 100;
                }
                drawConfigEditTrigger();
                break;
            }
        case BUTTON_1: 
            {
                if(config_edit_trigger == 1) {
                    config_edit_trigger = 100;
                } else {
                    config_edit_trigger = (++config_edit_trigger) % 100;
                }
                drawConfigEditTrigger();
                break;
            }
        case BUTTON_2: 
            {
                monitors[monitor_selection].trigger_value = config_edit_trigger;
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

void _drawConfigEditEnabled(char value) {
    GLCD.SelectFont(Arial_bold_14, BLACK);
    GLCD.EraseTextLine(0);
    if(value == 0) {
        GLCD.print("Disabled");
    } else {
        GLCD.print("Enabled");
    }
}

void drawConfigEditEnabled() {
    _drawConfigEditEnabled(config_edit_enabled);
}

void initConfigEnabled() {
    config_edit_enabled = monitors[monitor_selection].enabled;
    drawConfigEditEnabled(); 
}

void drawConfigEnabled() {
    _drawConfigEditEnabled(monitors[monitor_selection].enabled);
}

void handleConfigEnabledInput(int button) {
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
void _drawConfigManual(unsigned char value) {
    GLCD.SelectFont(Arial_bold_14, BLACK);
    GLCD.EraseTextLine(0);
    if(value) {
        GLCD.print("Opened");
    } else {
        GLCD.print("Closed");
    }
}

void initConfigManual() {
    drawConfigManual(); 
}

void drawConfigManual() {
    _drawConfigManual(monitors[monitor_selection].water_state);
}

void handleConfigManualInput(int button) {
    switch(button) {
        case BUTTON_0: 
        case BUTTON_1:

            {
                monitors[monitor_selection].water_state ^= 1;
                _drawConfigManual(monitors[monitor_selection].water_state);
                changeWaterPort(monitor_selection, monitors[monitor_selection].water_state);
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
drawConfigMeasure(bool measuring) {
    GLCD.SelectFont(Arial_bold_14, BLACK);
    GLCD.EraseTextLine(0);
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
initConfigMeasure() {
    drawConfigMeasure(true);
    measure(monitor_selection);
    drawConfigMeasure(false);
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
void drawConf() {
    GLCD.SetFontColor(BLACK);
    GLCD.EraseTextLine(0);
    GLCD.EraseTextLine(1);
    GLCD.EraseTextLine(2);
    GLCD.EraseTextLine(3);
  
    switch(config_selection) {
        case 0:
            drawConfigName();
            break;
        case 2:
            drawConfigWater();
            break;
        case 3:
            drawConfigTrigger();
            break;
        case 5:
            drawConfigEnabled();
            break;
        case 7:
            drawConfigInterval();
            break;
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
void handleConfInput(int button) {

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
                        case 2:
                            initConfigWater();
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
                    switchScreen(SCREEN_OVERVIEW);
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
                active_config = -1;
                break;
            case 2: 
                {
                    handleConfigWaterInput(button);
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


