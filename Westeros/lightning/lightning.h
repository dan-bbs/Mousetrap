#ifndef __LIGHTNING_MUSHE__
#define __LIGHTNING_MUSHE__

#include "mushecommunication.h"

// Pinout parameters
#define pinDrawerLock     6   // output drawer lock
#define pinLightningHV    7   // output high voltage
#define pinLightningLed   8   // output blinking
#define pinPictureReedIn  4   // input picture reed switches chain
#define pinHandleSpinIn   9   // input handle spinning

// Communication parameters
const char __DEV_ID__[] = "5";

int handle_turning_counter = 0;
const byte max_handle_turns = 4;  // amount of turns needed to light sparks
bool drawer_state = false;        // suppress multiple opens of drawer

int turncount(int pin);
void drawer_open(void);
void lightning_on(void);
rcv_errors parseMsg(char *in_message);

#endif
