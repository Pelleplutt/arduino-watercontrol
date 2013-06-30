#ifndef GREENHOUSE_H_INCLUDED
#define GREENHOUSE_H_INCLUDED 1

#include <glcd_Config.h>
#include <glcd.h>
#include <glcd_Buildinfo.h>

#include <fonts/SystemFont5x7.h>
#include <fonts/Arial_bold_14.h>

#define BUTTON_DELAY  150

#define SERIAL_DEBUG 1

#ifdef SERIAL_DEBUG
#   define DEBUG(a) Serial.print(a)
#   define DEBUGLN(a) Serial.println(a)
#else
#   define DEBUG(a) 
#   define DEBUGLN(a) 
#endif

#define BUTTON_0 0
#define BUTTON_1 1
#define BUTTON_2 2
#define BUTTON_3 3

#define DISPLAY_Y_MIDDLE  4
#define DISPLAY_X_MIDDLE  11

#define SCREEN_OVERVIEW  0
#define SCREEN_LOG 1
#define SCREEN_CONFIG 2

#define NAME_MAX_LEN 10
#define MAX_WATER_DURATION 1800
#define MAX_WATER_INTERVAL (7 * 86400)

#define SENSE_VALUE_INVALID -1

typedef struct monitor {
    char  name[NAME_MAX_LEN + 1];

    /* Enabled or disabled */
    char  enabled;
    /* Open/closed */  
    unsigned char  water_state;
    /* Connected to what port */
    char water_port;

    /* Calibrated wet sensor value */
    unsigned long   calibrated_max;
    unsigned long   calibrated_min;
    /* Trigger value for hose open, in percent, -1 == disable */
    unsigned char   trigger_value;
    /* Duration in seconds of hose open */
    unsigned int   water_duration;
    /* Trigger watering at least this often */
    unsigned long   water_interval;
    /* The last time the interval timer triggered watering */
    unsigned long   last_water_interval_trigger;
    /* Time of last watering */
    unsigned long   last_triggered[64];
    /* Last/current sensor reading */
    int   current_value;
};


extern int monitor_selection;

    /* We cache these from the main loop as some timing routines will depend
     * upon time being consistent throughout our iteration */
extern unsigned long now;
extern unsigned long now_s;

  /* Currently displaying screen in the application */
extern int current_screen;
extern monitor  monitors[8];

void draw();

void drawOverview();
void handleOverviewInput(int button);

void drawLog();
void handleLogInput(int button);

void drawConf();
void handleConfInput(int button);

char *timeString(int seconds);
void drawSensorBar(int value, unsigned long calibrated_min, unsigned long calibrated_max);
int getSensorValue(int value, unsigned long calibrated_min, unsigned long calibrated_max);

void changeWaterPort(unsigned char monitor, unsigned char state);

unsigned int measure(unsigned char monitor);


#endif
