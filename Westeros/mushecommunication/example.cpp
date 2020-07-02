// Example of using this library

/*
void loop() {
  // Operating with buttons or endswitches
  if (clear_remote_open) { clear_remote_open=false; remote_open=false; };
  
  if ((digitalRead(pinSwordLimSw) == LOW) || (digitalRead(pinOperButt) == LOW))     //pressing button or endswitch
    {
    if (!lock_on&&remote_open) clear_remote_open=true;
    if (lock_on&&!remote_open) throne_lock(off);
    }
  if (((digitalRead(pinSwordLimSw) == HIGH) && (digitalRead(pinOperButt) == HIGH))) // no pressing switches or buttons
    {
    if (lock_on&&remote_open) clear_remote_open=true;
    if (!lock_on&&!remote_open) throne_lock(on);
    }

  // Communication handling below
  if (receiveComplete)
    {
    switch (parseMsg(receivedMessage))
      {
      case _format_error:sendMsg(__DEV_ID__,"PARSE ERROR");break;
      case _crc_error:sendMsg(__DEV_ID__,"CRC ERROR");break;
      default:break;
      }
    receivedMessage[0]=0;
    receiveComplete=false;
    }
    
  if (wantToSend&&!dataLineBusy)
    {
    if (sendMsg(__DEV_ID__,messageToSend)==true) messageTries=messageTriesMax; // Succsessfull send
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
          if (!strcasecmp(separator,"1") || !strcasecmp(separator,"on"))  // operator switched ON
            {
            throne_lock(on);
            remote_open=true;
            }
          if (!strcasecmp(separator,"0") || !strcasecmp(separator,"off")) // operator switched OFF
            {
            throne_lock(off);
            remote_open=true;
            }
          }
        //----------------------------------------------------------------// Commands processing ^^^

        } //if separator
      command = strtok(NULL, ";"); // Find the next command in input string
      }// while command
    } //if !strcasecmp

  return _parse_ok;
}
*/
