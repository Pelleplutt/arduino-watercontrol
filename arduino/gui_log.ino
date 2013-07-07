#include "greenhouse.h"

int system_log_offset = 0;
int system_log_page = 0;

/*******************************************************************************/
void
drawOpenLog() {
    water_open_log *log;

    GLCD.ClearScreen();
    GLCD.SelectFont(System5x7, BLACK);
    char entry_number  = monitors[monitor_selection].current_open_log;


        /* We will have no inbetween invalid entries, either they are set or
         * they are empty so simply draw the ones we can, skip the rest */
    for(char i = 0; i < 16; i++) {
        entry_number--;
        if(entry_number == -1) {
            entry_number = MONITOR_LOG_COUNT - 1;
        }

        log = &monitors[monitor_selection].open_log[entry_number];
        if(log->time) {
            GLCD.CursorTo(
                i < 8?0:DISPLAY_X_MIDDLE,
                i < 8?i:i-8);


            GLCD.print(log->open_mode);
            GLCD.print(" ");
            GLCD.print(relativeTimeString(log->time));
        }
    }

        /* No valid enties? */
    if(monitors[monitor_selection].current_open_log == 0 &&
        monitors[monitor_selection].open_log[0].time == 0) {

        GLCD.print("No log");
    }
}

void
drawSenseLog() {
    water_sense_log *log;

    GLCD.ClearScreen();
    GLCD.SelectFont(System5x7, BLACK);
    char entry_number  = monitors[monitor_selection].current_sense_log;


        /* We will have no inbetween invalid entries, either they are set or
         * they are empty so simply draw the ones we can, skip the rest */
    for(char i = 0; i < 16; i++) {
        entry_number--;
        if(entry_number == -1) {
            entry_number = MONITOR_LOG_COUNT - 1;
        }

        log = &monitors[monitor_selection].sense_log[entry_number];
        if(log->time) {
            char reading[10];
            GLCD.CursorTo(
                i < 8?0:DISPLAY_X_MIDDLE,
                i < 8?i:i-8);


            sprintf(reading, "%3d%% ", getSensorCalibratedPercent(log->value,
                        monitors[monitor_selection].calibrated_min,
                        monitors[monitor_selection].calibrated_max));
            GLCD.print(reading);
            GLCD.print(relativeTimeString(log->time, 6));
        }
    }

        /* No valid enties? */
    if(monitors[monitor_selection].current_sense_log == 0 &&
        monitors[monitor_selection].sense_log[0].time == 0) {

        GLCD.print("No log");
    }
}

void
drawSystemLog() {
    system_log_entry *log;

    GLCD.ClearScreen();
    GLCD.SelectFont(System5x7, BLACK);

        /* Start at the current entry, then go backwards in entries (and in
         * time) foreach each row we are showing */
    int entry_number  = last_system_log;

        /* We will never set system_log_offset to a value greater then
         * last_system_log unless we have wrapped entries. */
    if(last_system_log < system_log_offset) {
        entry_number = SYSTEM_LOG_COUNT - (system_log_offset - last_system_log);
    } else {
        entry_number -= system_log_offset;
    }

    for(char i = 0; i < 8; i++) {
        entry_number--;
        if(entry_number == -1) {
            entry_number = SYSTEM_LOG_COUNT - 1;
                /* If we wrapped and the last entry is invalid, stop displaying
                 * right now please */
            if(system_log[entry_number].event_type !=
                SYSTEM_LOG_EVENT_TYPE_INVALID) {
                break;
            }
        }
        log = &system_log[entry_number];

        if(log->event_type != SYSTEM_LOG_EVENT_TYPE_INVALID) {
            char reading[25];
            char const *action_str;

            switch(log->event_type) {
                case SYSTEM_LOG_EVENT_TYPE_BOOT:
                    action_str = "Boot";
                    break;
                case SYSTEM_LOG_EVENT_TYPE_SENSE :
                    action_str = "Sense";
                    break;
                case SYSTEM_LOG_EVENT_TYPE_SENSE_FUNKY :
                    action_str = "Funky";
                    break;
                case SYSTEM_LOG_EVENT_TYPE_INTERVAL_OPEN :
                    action_str = "IOpen";
                    break;
                case SYSTEM_LOG_EVENT_TYPE_MANUAL_OPEN :
                    action_str = "MOpen";
                    break;
                case SYSTEM_LOG_EVENT_TYPE_SENSE_OPEN :
                    action_str = "SOpen";
                    break;
                case SYSTEM_LOG_EVENT_TYPE_AUTO_CLOSE :
                    action_str = "AClos";
                    break;
                case SYSTEM_LOG_EVENT_TYPE_MANUAL_CLOSE :
                    action_str = "MClos";
                    break;
            }

            if(log->monitor == -1) {
                sprintf(reading, "%5.5s %5s",
                        relativeTimeString(log->time, 5),
                        action_str);
            } else if(log->current_value < 0) {
                sprintf(reading, "%5.5s %5s %.10s",
                        relativeTimeString(log->time, 5),
                        action_str,
                        monitors[log->monitor].name);
            } else {
                if(system_log_page == 0) {
                    sprintf(reading, "%5.5s %5s %.10s",
                            relativeTimeString(log->time, 5),
                            action_str,
                            monitors[log->monitor].name);
                } else {
                    sprintf(reading, "%5.5s %3d%% %.11s",
                            relativeTimeString(log->time, 5),
                            log->current_value,
                            monitors[log->monitor].name);
                }
            }
            GLCD.CursorTo(0, i);
            GLCD.print(reading);
        }
    }

        /* No valid enties? */
    if(last_system_log == 0 && system_log[0].event_type == SYSTEM_LOG_EVENT_TYPE_INVALID) {

        GLCD.print("No log");
    }
}

void
clearSenseLog() {
    monitors[monitor_selection].current_sense_log = 0;
    memset(monitors[monitor_selection].sense_log, sizeof(monitors[monitor_selection].sense_log), 0);
}

void
clearOpenLog() {
    monitors[monitor_selection].current_open_log = 0;
    memset(monitors[monitor_selection].open_log, sizeof(monitors[monitor_selection].open_log), 0);
}

/*******************************************************************************/
void
handleOpenLogInput(int button) {
    switch(button) {
        case BUTTON_0:
        case BUTTON_1:
            draw();
            break;
        case BUTTON_2:
            clearOpenLog();
            draw();
            break;
        case BUTTON_3:
            if(monitors[monitor_selection].trigger_value) {
                switchScreen(SCREEN_LOG_SENSE);
            } else {
                switchScreen(SCREEN_CONFIG1);
            }
            break;
    }
}


void
handleSenseLogInput(int button) {
    switch(button) {
        case BUTTON_0:
        case BUTTON_1:
            draw();
            break;
        case BUTTON_2:
            clearSenseLog();
            draw();
            break;
        case BUTTON_3:
            switchScreen(SCREEN_CONFIG1);
            break;
    }
}

void
handleSystemLogInput(int button) {
    switch(button) {
        case BUTTON_0:
                /* Current entry will be valid only if we have written to it
                 * before */
            if(system_log[last_system_log].event_type != SYSTEM_LOG_EVENT_TYPE_INVALID ||
                (system_log_offset + 8 < last_system_log)) {

                system_log_offset = (system_log_offset + 8) % SYSTEM_LOG_COUNT;
            }

            draw();
            break;
        case BUTTON_1:
            if(system_log_offset >= 8) {
                /* If we have set a larger value then this, then all
                 * values below are good */
                system_log_offset -= 8;
            } else {
                /* Easy check here, if the last value in the circular log
                 * is good, then we can wrap safely. Otherwise stay put */
                if(system_log[SYSTEM_LOG_COUNT - 1].event_type != SYSTEM_LOG_EVENT_TYPE_INVALID) {
                    system_log_offset = SYSTEM_LOG_COUNT - 8;
                } else {
                        /* Otherwise pick the highest top offset we can do that
                         * is an even offset */
                    system_log_offset = (int)(last_system_log / 8) * 8;
                }
            }
            draw();
            break;
        case BUTTON_2:
            system_log_page ^= 1;
            draw();
            break;
        case BUTTON_3:
            switchScreen(SCREEN_OVERVIEW);
            break;
    }
}
