/*
 * ks0108_Manual_Config.h - User specific configuration for Arduino GLCD library
 *
 * Use this file to set io pins and LCD panel parameters
 * This version is for a standard ks0108 display
 * connected using the default Arduino wiring
 *
*/

#ifndef GLCD_PANEL_CONFIG_HH
#define GLCD_PANEL_CONFIG_H

/*
 * define name for panel configuration
 */
#define glcd_PanelConfigName "ks0108-watercontrol"

/*********************************************************/
/*  Configuration for LCD panel specific configuration   */
/*********************************************************/
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

// panel controller chips
#define CHIP_WIDTH     64  // pixels per chip
#define CHIP_HEIGHT    64  // pixels per chip

/*********************************************************/
/*  Configuration for assigning LCD bits to Arduino Pins */
/*********************************************************/

/*
 * define name for pin configuration
 */
#define glcd_PinConfigName "ks0108-watercontrol"

/*
 * Pins can be assigned using Arduino pin numbers 0-n
 * Pins can also be assigned using PIN_Pb
 *   where P is port A-L and b is bit 0-7
 *   Example: port D pin 3 is PIN_D3
 *
 */


/* Data pin definitions
 */
#define glcdData0Pin        22
#define glcdData1Pin        24
#define glcdData2Pin        26
#define glcdData3Pin        28
#define glcdData4Pin        30
#define glcdData5Pin        32
#define glcdData6Pin        34
#define glcdData7Pin        36

/* Arduino pins used for Commands
 * default assignment uses the first five analog pins
 */
#define glcdCSEL1        38
#define glcdCSEL2        40

#define glcdRW            44
#define glcdDI            46
#define glcdEN            48
// Reset Bit
//#define glcdRES           42    // Reset Bit

/*
 * the following is the calculation of the number of chips - do not change
 */
#define glcd_CHIP_COUNT (((DISPLAY_WIDTH + CHIP_WIDTH - 1)  / CHIP_WIDTH) * ((DISPLAY_HEIGHT + CHIP_HEIGHT -1) / CHIP_HEIGHT))

/*********************************************************/
/*  Chip Select Configuration                            */
/*********************************************************/

/*
 * Change the following define to match the number of Chip Select pins for this panel
 * Most panels use two pins for chip select,
 * but check your datasheet to see if a different number is required
 */
#define NBR_CHIP_SELECT_PINS   2 // the number of chip select pins required for this panel

/*
 * Defines for Panels using two Chip Select pins
 */

/*
 * Two Chip panels using two select pins (the most common panel type)
 */
#define glcd_CHIP0 glcdCSEL1,HIGH,   glcdCSEL2,LOW
#define glcd_CHIP1 glcdCSEL1,LOW,    glcdCSEL2,HIGH

/*********************************************************/
/*  End of Chip Select Configuration                     */
/*********************************************************/

/*
 * The following defines are for panel specific low level timing.
 *
 * See your data sheet for the exact timing and waveforms.
 * All defines below are in nanoseconds.
 */

#define GLCD_tDDR   320    /* Data Delay time (E high to valid read data)        */
#define GLCD_tAS    140    /* Address setup time (ctrl line changes to E high)   */
#define GLCD_tDSW   200    /* Data setup time (data lines setup to dropping E)   */
#define GLCD_tWH    450    /* E hi level width (minimum E hi pulse width)        */
#define GLCD_tWL    450    /* E lo level width (minimum E lo pulse width)        */

#include "device/ks0108_Device.h"
#endif //GLCD_PANEL_CONFIG_H
