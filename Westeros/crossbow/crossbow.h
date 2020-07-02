#ifndef __CROSSBOW_MUSHE__
#define __CROSSBOW_MUSHE__

#include "mushecommunication.h"

// Pinout parameters
#define pinLockOut 3     // lock output
#define pinAudioIn 4     // audio sensor input

// Communication parameters
const char __DEV_ID__[]="2";

void lock_open(void);
rcv_errors parseMsg(char *in_message);

#endif
