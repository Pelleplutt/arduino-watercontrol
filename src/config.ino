#include "greenhouse.h"

char configmenu[8][12] = {"Name", "Calibrate", "Water", "Trigger", "Manual", "Enabled", "Measure", "Interval"};
int config_selection = 0;

/*******************************************************************************/
char config_edit_name[NAME_MAX_LEN + 1];
char config_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVXYZabcdefghijklmnopqrstuvxyz0123456789., \x7e\x7f";
char config_alphabet_pos;

void drawConfigEditName() {
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
  GLCD.CursorTo(0, 0);
  GLCD.print(monitors[monitor_selection].name);
}

void handleConfigNameInput(int button) {
  int slen = strlen(config_edit_name);
  switch(button) {
    case 0: {
      if(config_alphabet[config_alphabet_pos] == 0x7f) {
        if(slen > 0) {
          config_edit_name[slen - 1] = '\0';
        }
      } else if(config_alphabet[config_alphabet_pos] == 0x7e) {
        strcpy(monitors[monitor_selection].name, config_edit_name);
        config_selection = 0;
        draw();
      } else if(slen < NAME_MAX_LEN) {
        config_edit_name[slen] = config_alphabet[config_alphabet_pos];
        config_edit_name[slen + 1] = '\0'; 
      }
      drawConfigEditName();
      break;
    }
    case 1: {
      config_alphabet_pos = (++config_alphabet_pos) % strlen(config_alphabet);
      drawConfigEditName();
      break;
    }
    case 2: {
      config_selection = 0;
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
    case 0: {
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
    case 1: {
      monitors[monitor_selection].water_duration = config_edit_water_duration;
      /* No break */  
    }
    case 2: {
      config_selection = 2;
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
    case 0: {
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
    case 1: {
      monitors[monitor_selection].water_interval = config_edit_water_interval;
      /* No break */  
    }
    case 2: {
      config_selection = 7;
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
    case 0: {
      if(config_edit_trigger == 99) {
        config_edit_trigger = -1;
      } else {
        config_edit_trigger = (++config_edit_trigger) % 100;
      }
      drawConfigEditTrigger();
      break;
    }
    case 1: {
      monitors[monitor_selection].trigger_value = config_edit_trigger;
      /* No break */  
    }
    case 2: {
      config_selection = 3;
      draw();
      break;
    }
  }
}

/*******************************************************************************/
char  config_edit_enabled;

void _drawConfigEditEnabled(char value) {
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
    case 0: {
      config_edit_enabled ^= 1;
      drawConfigEditEnabled();
      break;
    }
    case 1: {
      monitors[monitor_selection].enabled = config_edit_enabled;
      /* No break */  
    }
    case 2: {
      config_selection = 5;
      draw();
      break;
    }
  }
}

/*******************************************************************************/
void _drawConfigManual(unsigned char value) {
  GLCD.EraseTextLine(0);
  if(value) {
    GLCD.print("Close");
  } else {
    GLCD.print("Open");
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
    case 0: {
      monitors[monitor_selection].water_state ^= 1;
      drawConfigManual();
      changeWaterPort(monitor_selection, monitors[monitor_selection].water_state);
      break;
    }
    case 1: {
    }
    case 2: {
      config_selection = 5;
      draw();
      break;
    }
  }
}


/*******************************************************************************/
void drawConf() {
  GLCD.SelectFont(System5x7, BLACK);
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
  
  if(config_selection >= 0) {
     switch(button) {
      case 0: {
        config_selection = (++config_selection) % 8;
        draw();
        break;
      }
      case 1: {
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
          case 7:
            initConfigInterval();
            break;
            
        }
        config_selection = -1 - config_selection;

        break;
      }
      case 2: {
        switchScreen(SCREEN_OVERVIEW);
        break;
      }
    }
  }else {
    switch(config_selection) {
      case -1: {
        handleConfigNameInput(button);
        break;
      }
      case -3: {
        handleConfigWaterInput(button);
        break;
      }
      case -4: {
        handleConfigTriggerInput(button);
        break;
      }
      case -5: {
        handleConfigManualInput(button);
        break;
      }
      case -6: {
        handleConfigEnabledInput(button);
        break;
      }
      case -8: {
        handleConfigIntervalInput(button);
        break;
      }
    }
  }
}


