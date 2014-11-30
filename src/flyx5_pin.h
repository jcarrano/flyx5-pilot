/**
 * Pin definitions for flyx5 board.
 *
 * @author  Juan I Carrano, Andrés Calcabrini.
 *
 * @copyright Copyright 2014 Carrano, Calcabrini, Ubeira, Venturo.
 */

#ifndef FLYX5_PIN_H
#define FLYX5_PIN_H

#include <stdbool.h>

struct pin_cfg {
	uint32_t function;

};


JTAG		PC3
BUTTON_1
JTAG            PC2
LED_A
JTAG            PC1
LED_B
JTAG            PC0
Unusable
DEBUG_RX        PA0
DEBUG_TX        PA1
BUTTON_2        PE2

BATT_SENSE0     PE0
BATT_SENSE1     PE1

SENSORS_SCL     PA6
SENSORS_SDA     PA7
DMU_SCL         PB2
DMU_SDA         PB3
DMU_INT         PE3
MAG_INT         PE4
ALT_INT12       PE5
ULTRA_PROBE     PF2
ULTRA_ECHO      PF3

ESC_SCK         PB4
ESC_MISO        PB6
ESC_MOSI        PB7
ESC_SS0         PD0
ESC_SS1         PD1
ESC_SS2         PD2
ESC_SS3         PD3
ESC_SS4         PD4
ESC_SS5         PD5
ESC_SS6         PD6
ESC_SS7         PD7

AUX_SCK         PA2
SDCARD_SS       PA3
AUX_MISO        PA4
AUX_MOSI        PA5
NRF_CE          PB0
NRF_SS          PB1
NRF_INT         PB5
BUZZER          PF4

RC_ROLL         PC4
RC_PITCH        PC5
RC_YAW          PC6
RC_ALTITUDE     PC7
RC_AUX1         PF0
RC_AUX2         PF1

#endif /* FLYX5_PIN_H */
