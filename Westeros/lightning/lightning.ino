/* Mushelovka Lightning v1.0a
Settings for communication and pin settings are in "lightning.h"
Required "mushecommunication.cpp & .h"
Description of MusheCommunication library see in "mushecommunication.cpp"

Remote commands for this puzzle:
$5:drawer=on;spark=on;*63    opens drawer and ignites spark
$5:1=1;2=1;*0C               same */

#include "lightning.h"

void setup() {
  Serial.begin(38400);
  pinMode(LED_BUILTIN,      OUTPUT);
  pinMode(pinDrawerLock,    OUTPUT); // Drawer lock
  pinMode(pinLightningHV,   OUTPUT); // High Voltage module
  pinMode(pinLightningLed,  OUTPUT); // Lightning led
  pinMode(pinPictureReedIn, INPUT);  // Picture reed switches chain
  pinMode(pinHandleSpinIn,  INPUT);  // Handle reed switch
  delay(5);                          // delay for serial to init
}

void loop() {
  // Operating with buttons or endswitches

  if ((digitalRead(pinPictureReedIn) == LOW) && (!drawer_state))  // setting all magnets on chain of reed switches correctly
    {
    drawer_open();
    }
  if (digitalRead(pinPictureReedIn) == HIGH) drawer_state=false; // reset flag if pictures are not in place

    handle_turning_counter = turncount(pinHandleSpinIn);
    if (handle_turning_counter >= max_handle_turns)
      {
      lightning_on();
      handle_turning_counter = 0;
      }

  // Communication handling below
  if (receiveComplete)
    {
    switch (parseMsg(receivedMessage))
      {
      case _format_error:  sendMsg(__DEV_ID__,"PARSE ERROR"); break;
      case _crc_error:     sendMsg(__DEV_ID__,"CRC ERROR");   break;
      case _general_error:
      default:             break;
      }
    receivedMessage[0] = 0;
    receiveComplete = false;
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

  if (dataLineBusy&&(millis()-dataLineTimeout>100))//100ms incoming data timeout estimately 96 bytes of data on 9600bps
    {
    rcvTimeout(); 
    }
}

int turncount(int pin) {
  //static long int start_oborota = 0;
  static int oborotov = 0;
  static bool wait_next_turn = false;
  bool reed_sw = digitalRead(pin);
  if (oborotov >= max_handle_turns) oborotov = 0;
  if (!reed_sw  && !wait_next_turn) // handle magnet short out reed
    {
    wait_next_turn=true;
    oborotov++;
    }
  if (reed_sw) wait_next_turn=false; // handle continue turning
        
    /*if (!reed_sw && !start_oborota)//first turn
      {
      start_oborota=millis();
      wait_next_turn=true;
      oborotov++;
      }
    if ((millis()-start_oborota)>500)//wait 500ms for debounce
      {
      start_oborota=0;
      oborotov=0;
      }
    */
    return(oborotov);
}

void drawer_open()
    {
    sendMsg(__DEV_ID__,"DRAWER OPEN");
    drawer_state = true;
    digitalWrite(LED_BUILTIN,HIGH);
    digitalWrite(pinDrawerLock,HIGH);   //unlock
    delay(300);
    digitalWrite(LED_BUILTIN,LOW);
    digitalWrite(pinDrawerLock,LOW);   //lock
    //delay(300);
    }

void lightning_on()
    {
    sendMsg(__DEV_ID__,"SPARK ON");
    digitalWrite(pinLightningHV,HIGH);
    delay(100);
    digitalWrite(pinLightningLed,HIGH);
    delay(200);
    digitalWrite(pinLightningHV,LOW);
    digitalWrite(pinLightningLed,LOW);
    delay(300);
    digitalWrite(pinLightningLed,HIGH);
    delay(100);
    digitalWrite(pinLightningHV,HIGH);
    digitalWrite(pinLightningLed,LOW);
    delay(300);
    digitalWrite(pinLightningLed,HIGH);
    digitalWrite(pinLightningHV,LOW);
    delay(200);
    digitalWrite(pinLightningHV,HIGH);
    delay(200);
    digitalWrite(pinLightningHV,LOW);
    digitalWrite(pinLightningLed,LOW);
    delay(300);
    digitalWrite(pinLightningLed,HIGH);
    digitalWrite(pinLightningHV,HIGH);
    delay(200);
    digitalWrite(pinLightningLed,LOW);
    delay(300);
    digitalWrite(pinLightningHV,LOW);
    digitalWrite(pinLightningLed,HIGH);
    delay(100);
    digitalWrite(pinLightningLed,LOW);
    delay(200);
    digitalWrite(pinLightningHV,HIGH);
    digitalWrite(pinLightningLed,HIGH);
    delay(200);
    digitalWrite(pinLightningLed,LOW);
    delay(200);
    digitalWrite(pinLightningHV,LOW);
    digitalWrite(pinLightningLed,HIGH);
    delay(300);
    digitalWrite(pinLightningLed,LOW);
    digitalWrite(pinLightningHV,HIGH);
    delay(300);
    digitalWrite(pinLightningLed,HIGH);
    delay(100);
    digitalWrite(pinLightningHV,LOW);
    digitalWrite(pinLightningLed,LOW);
    delay(200);
    digitalWrite(pinLightningLed,HIGH);
    delay(100);
    digitalWrite(pinLightningHV,HIGH);
    delay(200);
    digitalWrite(pinLightningHV,LOW);
    delay(300);
    digitalWrite(pinLightningLed,LOW);
    }

rcv_errors parseMsg(char *in_message) {
  // Calculating CRC from entire message
  char calc_crc[3];
  sprintf(calc_crc, "%02X", computeChecksum(in_message));

  // Reading device ID
  char *dev_id= strtok(in_message, "$:*");
  if (dev_id==NULL) return _format_error;                         // error in message format

  // Reading payload
  char *msg_payload=strtok(NULL, ":*");
  if (msg_payload==NULL) return _format_error;                    // error in message format

  // Comparing received and calculated CRC
  char *msg_crc = strtok(NULL, ":*");
  if (msg_crc==NULL) return _format_error;                        // error in message format
  if (strcmp(msg_crc,calc_crc)) return _crc_error;                // error in message CRC
  
  if (!strcmp(__DEV_ID__,dev_id))                                 // Compare dev id with own
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
        if (!strcasecmp(command,"1") || !strcasecmp(command,"drawer"))    // operating drawer
          {
          ++separator;
          if (!strcasecmp(separator,"1") || !strcasecmp(separator,"on"))  // operator switched ON
            {
            drawer_open();
            }
          }
        if (!strcasecmp(command,"1") || !strcasecmp(command,"spark"))    // operating drawer
          {
          ++separator;
          if (!strcasecmp(separator,"1") || !strcasecmp(separator,"on"))  // operator switched ON
            {
            lightning_on();
            }
          }
        //----------------------------------------------------------------// Commands processing ^^^
        } //if separator
      // Find the next command in input string
      command = strtok(NULL, ";");
      }// while command
    } //if !strcasecmp

  return _parse_ok;
}
