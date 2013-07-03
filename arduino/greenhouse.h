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

#define WATER_OPEN 1
#define WATER_CLOSED 0

#define MANUAL_WATER_OPEN_TIME (15 * 60)

#define MAX_OPEN_WATER_PORTS 3

#define OPEN_MODE_MANUAL 'M'
#define OPEN_MODE_SENSOR 'S'
#define OPEN_MODE_INTERVAL 'I'

    /* FIXME DO WE HAVE A MAX define somewhere ...? */
#define MILLIS_MAX 4294967295

typedef struct water_log {
        /* millis() / 1000 time of open */
    unsigned long time;
        /* OPEN_MODE_* */
    char open_mode; 
        /* Duration in seconds of openness */
    unsigned int duration;
};

typedef struct monitor {
    char  name[NAME_MAX_LEN + 1];

    /* Enabled or disabled */
    char  enabled;
    /* Connected to what port */
    char water_port;

    /* Calibrated wet sensor value */
    unsigned long   calibrated_max;
    unsigned long   calibrated_min;

    /* Duration in seconds of hose open at trigger*/
    unsigned int   water_duration;

    /* Trigger value for hose open, in percent, -1 == disable */
    unsigned char   trigger_value;
    /* Trigger watering at least this often, if used then the trigger value is
     * not used */
    unsigned long   water_interval;

    /* The last millis() / 1000 we opened the port */
    unsigned long   last_water_open;

    unsigned char current_log;
    /* Time of last waterings */
    water_log   log[17];

    /* Last/current sensor reading */
    int   current_value;

    /* Open/closed */  
    unsigned char  water_state;
    /* The millis()/1000 we opened the water control */
    unsigned long water_opened_at;
    /* The duration in seconds to keep the water open for */
    unsigned long water_open_for;
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
