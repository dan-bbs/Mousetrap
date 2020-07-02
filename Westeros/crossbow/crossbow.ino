/* Mushelovka Crossbow v1.0a
Settings for communication and pin settings are in "crossbow.h"
Required "mushecommunication.cpp & .h"
Description of MusheCommunication library see in "mushecommunication.cpp"

Remote commands for this puzzle:
$2:lock=off;*6A    turns lock off
$2:1=0;*0F         same */

#include "crossbow.h"

void setup() {
  Serial.begin (38400);
  pinMode(LED_BUILTIN, OUTPUT); // initialize digital pin LED_BUILTIN as an output.
  pinMode(pinLockOut,  OUTPUT);
  pinMode(pinAudioIn,  INPUT);
  digitalWrite(pinLockOut,LOW);   //lock
  delay(5);                     // delay for serial to init
}

void loop() {
  if (digitalRead(pinAudioIn)) lock_open(); // yes, that's all

  // Communication handling below
  if (receiveComplete)
    {
    switch (parseMsg(receivedMessage))
      {
      case _format_error:  sendMsg(__DEV_ID__,"PARSE ERROR");break;
      case _crc_error:     sendMsg(__DEV_ID__,"CRC ERROR");  break;
      case _general_error:
      default:break;
      }
    receivedMessage[0]=0;
    receiveComplete=false;
    }
    
  if (wantToSend&&!dataLineBusy)
    {
    if (sendMsg(__DEV_ID__,messageToSend)==true) messageTries=messageTriesMax; // Successfull send
    else
      {
      messageTries--;
      if (messageTries<=0) 
        {
        wantToSend=false;
        messageTries=messageTriesMax;
        }
      }
    }

  if (dataLineBusy&&(millis()-dataLineTimeout>100)) // 100ms incoming data timeout estimately 96 bytes of data on 9600bps
    {
    rcvTimeout(); 
    }
}

void lock_open() {
    sendMsg(__DEV_ID__,"CROSSBOW SHOT");
    digitalWrite(LED_BUILTIN,HIGH);
    digitalWrite(pinLockOut,HIGH);  //unlock
    delay(300);
    digitalWrite(LED_BUILTIN,LOW);
    digitalWrite(pinLockOut,LOW);   //lock
    delay(600);
    digitalWrite(LED_BUILTIN,HIGH);
    digitalWrite(pinLockOut,HIGH);  //unlock
    delay(300);
    digitalWrite(LED_BUILTIN,LOW);
    digitalWrite(pinLockOut,LOW);   //lock
}

rcv_errors parseMsg(char *in_message) {
  // Calculating CRC from entire message
  char calc_crc[3];
  sprintf(calc_crc, "%02X", computeChecksum(in_message));

  // Reading device ID
  char *dev_id= strtok(in_message, "$:*");
  if (dev_id==NULL) return _general_error;                         // error in message format, suppressing output

  // Reading payload
  char *msg_payload=strtok(NULL, ":*");
  if (msg_payload==NULL) return _format_error;                    // error in message format

  // Comparing received and calculated CRC
  char *msg_crc = strtok(NULL, ":*");
  if (msg_crc==NULL) return _format_error;                        // error in message format
  if (strcasecmp(msg_crc,calc_crc)) return _crc_error;            // error in message CRC
  
  if (!strcasecmp(__DEV_ID__,dev_id))                             // Compare dev id with own
    {
    char *command = strtok(msg_payload, ";");
    if (command==NULL) return _format_error;                      // error in message format
    
    while (command != 0)                                          // Split the commands and get the values
      {
      char* separator = strchr(command, '=');
      if (separator != 0)
        {
        // Actually split the string in 2: replace '=' with 0
        *separator = 0;
        //----------------------------------------------------------------// Commands processing vvv
        if (!strcasecmp(command,"1") || !strcasecmp(command,"lock"))      // operating lock
          {
          ++separator;
          if (!strcasecmp(separator,"0") || !strcasecmp(separator,"off")) // operator switched OFF
            {
            lock_open();
            }
          }
        //----------------------------------------------------------------// Commands processing ^^^

        } //if separator
      command = strtok(NULL, ";"); // Find the next command in input string
      }// while command
    } //if !strcasecmp

  return _parse_ok;
}
