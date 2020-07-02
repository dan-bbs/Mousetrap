#ifndef __THRONE_MUSHE__
#define __THRONE_MUSHE__

#include "mushecommunication.h"

// Pinout parameters
#define pinLockRelay  10 // output to relay driving electromagnet lock
#define pinOperButt   11 // override operator's button
#define pinSwordLimSw 12 // sword's endswitch

// Communication parameters
const char __DEV_ID__[]="7";

typedef enum {off=0,on} onoff_t;
bool lock_on = false;           // lock state
bool remote_open = false;       // lock remote control by rs485
bool clear_remote_open = false; // clear remote control by pressing switch

void throne_lock(onoff_t state);
rcv_errors parseMsg(char *in_message);

#endif
