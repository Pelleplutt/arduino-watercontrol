#ifndef GREENHOUSE_H_INCLUDED
#define GREENHOUSE_H_INCLUDED 1

#include <glcd_Config.h>
#include <glcd.h>
#include <glcd_Buildinfo.h>

#include <fonts/SystemFont5x7.h>
#include <fonts/Arial_bold_14.h>

    /* Minium read time in ms between button repeats */
#define BUTTON_DELAY  150
    /* Sparsly used. Output information of doings onto the serial interface. */
#define SERIAL_DEBUG 1

#ifdef SERIAL_DEBUG
#   define DEBUG(a) Serial.print(a)
#   define DEBUGLN(a) Serial.println(a)
#else
#   define DEBUG(a)
#   define DEBUGLN(a)
#endif

    /* Button defines. Quite silly really, but this is more for the obvious
     * clearity of where we look at buttons and numbers. Makes it easier to
     * search if nothing else. */
#define BUTTON_0 0
#define BUTTON_1 1
#define BUTTON_2 2
#define BUTTON_3 3

    /* System5x7 font display positions */
#define DISPLAY_Y_MIDDLE  4
#define DISPLAY_X_MIDDLE  11

    /* GUI System display information. Controls screen switching, active
     * display etc. */
#define SCREEN_OVERVIEW  0
#define SCREEN_LOG_OPEN 1
#define SCREEN_LOG_SENSE 2
#define SCREEN_CONFIG1 3
#define SCREEN_SYSTEM_LOG 4

    /* Maximum length of sensor name */
#define NAME_MAX_LEN 10

    /* Maximum duration we can specify of open water port */
#define MAX_WATER_DURATION 1800
    /* Maxiumum specifyable interval */
#define MAX_WATER_INTERVAL (7 * 86400)

    /* If sensor current value is this, please ignore it */
#define SENSE_VALUE_INVALID -1

    /* Defines to control water flow */
#define WATER_OPEN 1
#define WATER_CLOSED 0

    /* When using manual open, set the port open for this long, then auto close
     * */
#define MANUAL_WATER_OPEN_TIME (15 * 60)

    /* The maximum number of open ports at the same time. We need this to
     * control the current we are using. Our power supply is rated at 2A, each
     * solenoid is using 250mA when open. I am keeping this conservative as is
     * does not really matter that much. But we need headroom for Arduino and
     * the sensors as well. */
#define MAX_OPEN_WATER_PORTS 3

    /* Open mode defines, used in open_log entries */
#define OPEN_MODE_MANUAL 'M'
#define OPEN_MODE_SENSOR 'S'
#define OPEN_MODE_INTERVAL 'I'

    /* Number of per monitor log entries we will keep for open log and sense
     * log */
#define MONITOR_LOG_COUNT 17

    /* Sense the reading of each sensor every this often */
#define SENSOR_MEASURE_INTERVAL (60 * 10)

    /* Do not allow opening the port based upon sensor input if we opened it no
     * longer then this time ago. This is a flood protection based upon
     * sensors. It might take some time before moist reaches the sensor after
     * we open it. So leave it alone for some time before going at it again. */
#define MIN_WATER_AUTO_INTERVAL (15 * 60)

    /* The value of millis() before it wraps. This should really be a LONG_MAX
     * value, but cannot find it... */
    /* FIXME DO WE HAVE A MAX define somewhere ...? */
#define MILLIS_MAX 4294967295

    /* Pad each entry in EEPROM by this much, this will give us some headroom
     * when changing configuration structure later so we do not cross read
     * entries. The Mega has 4k of this anyhow so we will be good */
#define EEPROMBYTES_PER_MONITOR 128

    /* Silly magic byte so we know if a valid sensor is saved in the slot */
#define EEPROM_SENSOR_MAGIC 'V'

    /* Number of global system log entries we will keep. */
#define SYSTEM_LOG_COUNT 128

    /* Log structure for individual per monitor port openings */
typedef struct water_open_log {
        /* millis() / 1000 time of open */
    unsigned long time;
        /* OPEN_MODE_* */
    char open_mode;
        /* Duration in seconds of openness */
    unsigned int duration;
};

    /* Log structure for individual per monitor sensor readings */
typedef struct water_sense_log {
        /* millis() / 1000 time of open */
    unsigned long time;
        /* Reading in number of triggers */
    int value;
};

    /* Structure per monitor. Keeps all the information about the port that we
     * might need */
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

    unsigned char current_open_log;
    /* Time of last waterings */
    water_open_log   open_log[MONITOR_LOG_COUNT];

    unsigned char current_sense_log;
    /* Values of last sensor readings */
    water_sense_log     sense_log[MONITOR_LOG_COUNT];

    /* millis()/1000 of last time we read the sensor */
    unsigned long last_sensor_read;
    /* Last/current sensor reading as the number of triggers triggered during
     * last read (during a 1s run)*/
    int   current_value;

    /* Open/closed */
    unsigned char  water_state;
    /* The millis()/1000 we opened the water control */
    unsigned long water_opened_at;
    /* The duration in seconds to keep the water open for */
    unsigned long water_open_for;
};

    /* Used for event_type in system_log_entry. Defines the action taken for
     * the log entry */

    /* Use 0 to mark invalid, structure will be memset to all zero when we boot
     * */
#define SYSTEM_LOG_EVENT_TYPE_INVALID 0
#define SYSTEM_LOG_EVENT_TYPE_BOOT 'b'
#define SYSTEM_LOG_EVENT_TYPE_SENSE 's'
#define SYSTEM_LOG_EVENT_TYPE_SENSE_FUNKY 'f'
#define SYSTEM_LOG_EVENT_TYPE_INTERVAL_OPEN 'i'
#define SYSTEM_LOG_EVENT_TYPE_MANUAL_OPEN 'm'
#define SYSTEM_LOG_EVENT_TYPE_SENSE_OPEN 't'
#define SYSTEM_LOG_EVENT_TYPE_AUTO_CLOSE 'c'
#define SYSTEM_LOG_EVENT_TYPE_MANUAL_CLOSE 'C'

typedef struct system_log_entry {
        /* millis()/1000 at time of event */
    unsigned long time;

        /* See defined above */
    char event_type;

        /* Sensor, -1 if not relevant */
    char monitor;

        /* Current value at this point. For sense/sense_funk and open this will
         * be the new value */
    int current_value;
};

extern system_log_entry system_log[SYSTEM_LOG_COUNT];
extern int last_system_log;

    /* Currently selected entry from main screen */
extern int monitor_selection;

    /* We cache these from the main loop as some timing routines will depend
     * upon time being consistent throughout our iteration */
extern unsigned long now;
extern unsigned long now_s;

  /* Currently displaying screen in the application */
extern int current_screen;
extern monitor  monitors[8];

    /* Flag revealing if the current monitor in configuration has changed. If
     * so save it to eeprom when we exit configuration */
extern char config_monitor_changed;

    /* Flag if we are currently probing a sensor value */
extern char measuring;

    /* ARDUINO */
void
draw();

void
loadMonitor(unsigned char monitornum);

void
saveMonitor(unsigned char monitornum);

void
loadMonitors();

void
saveMonitors();

    /* GUI_OVERVIEW */
void
drawOverview();
void
handleOverviewInput(int button);

    /* GUI_LOG */
void
drawOpenLog();
void
handleOpenLogInput(int button);
void
drawSenseLog();
void
handleSenseLogInput(int button);
void
handleSystemLogInput(int button);
void
drawSystemLog();

    /* GUI_CONFIG1 */
void
drawConf();
void
handleConfInput(int button);

void
drawSensorBar(int value, unsigned long calibrated_min, unsigned long calibrated_max);

    /* STRINGS */
char
*timeString(int seconds);
char
*timeString(int seconds, unsigned char maxlen);
char *
relativeTimeString(unsigned long when);
char *
relativeTimeString(unsigned long when, unsigned char maxlen);

    /* WATER */
void
changeWaterPort(unsigned char monitor, unsigned char state);

    /* MEASURE */
unsigned int
measure(unsigned char monitor);

int
getSensorCalibratedPercent(int value, unsigned long calibrated_min, unsigned long calibrated_max);

    /* LOG */
void
appendToOpenLog(unsigned char monitor, char mode, unsigned long time);
void
appendToSenseLog(unsigned char monitor, int value, unsigned long time);
void
appendToSystemLog(char monitor, char event_type, int value, unsigned long time);


#endif
