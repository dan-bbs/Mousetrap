/* MusheCommunication for Mushelovka v1.0a
This tiny library allows to communicate between MusheLovka server device and 
clients through RS-485 with short commands from server and events/answers from
clients.
Example of parsing procedure see below all useful code in comment.

Incoming messages BUF_SIZE defined in "mushecommunication.h"
Incoming message format:
  $a:b=c;d=e;*f
where a   - device_id or address
      b,d - param 1,2
      c,e - value 1,2
      f   - cyclic redundancy check, just an XOR of all the bytes between the $ and the * (not including the delimiters themselves), and written in hexadecimal.
All commands are case insensitive, all fields can be strings or numbers. Here is online CRC calculator - https://nmeachecksum.eqth.net/

Incoming messages example:
  12345                       // error, replying nothing
  $osajdfkahsfkl*xz           // error, replying nothing
  $7:1=on;2=off;*99           // error, replying    $7.31:CRC ERROR*59
  $8:lock=on;*0E              // ok, replying nothing because mismatch of dev_id
  $7:1=on;2=off;*60           // ok, replying:      $7.21:LOCK ON*0A
  $7:xz=on;mmz=off;*1B        // ok, doing nothing
  $7:lock=off;light=off;*78   // ok, replying       $7.62:LOCK OFF*43
  $7:lock=on;lock=off;*63     // ok, replying_1     $7.28:LOCK ON*03
                                     replying_2     $7.29:LOCK OFF*4C

All internal events related to button or switch induce outgoing messages like below.
Outgoing message format:
  $a.i:RESULT*f
where i - outgoing message sequential number

Outgoing message examples:  
  $7.15:LOCK ON*0D
  $7.16:LOCK OFF*40 */
  
#include <Arduino.h>
#include "mushecommunication.h"

char    receivedMessage[BUF_SIZE];
char    messageToSend[BUF_SIZE];
bool    dataLineBusy    = false;  // flag to delay outcoming message
bool    receiveComplete = false;  // flag to proceed incoming message
bool    wantToSend      = false;  // flag to send from main loop if network was busy
long    dataLineTimeout = 0;      // timer to abort waiting end of incoming message
long    messageCount    = 0;
byte    messageTries    = 10;
byte    messageTriesMax = 10;
bool    packetBegin;
bool    packetEnd;
char    receivedCrc[3];

/*SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.*/
void serialEvent() {
  static int msgIdx, crcIdx; //character indexes
  char inChar;

  if (!dataLineBusy) // first receive
    {
    dataLineTimeout = millis();
    dataLineBusy = true;
    receivedMessage[0] = 0;
    msgIdx = 0;
    crcIdx = 0;
    receivedCrc[0] = 0;
    packetBegin = false;
    packetEnd = false;
    }

  
  while (Serial.available()) 
    {                                                                               // Must not rearrange sequence of checking of tokens!
    inChar = (char) Serial.read();                                                  // get the new byte (0)

    if ((inChar == '\n')||(inChar == '\r'))     { dataLineBusy = false;             // end of transmit (1)
                                                  receiveComplete = true;
                                                  dataLineTimeout  = 0;
                                                  packetBegin = false;
                                                  packetEnd = false;
                                                  receivedCrc[0] = 0; }
    if (packetBegin && packetEnd && (crcIdx<2)) { receivedCrc[crcIdx] = inChar;     // stores crc after * before \n\r (2)
                                                  receivedCrc[crcIdx+1] = 0;
                                                  crcIdx++; }
    if (inChar == '*')                          { packetEnd = true; }               // found end of packet (3)
    if (packetBegin && (msgIdx < BUF_SIZE))     { receivedMessage[msgIdx] = inChar; // stores chars in receivedMessages (4)
                                                  receivedMessage[msgIdx+1] = 0;
                                                  msgIdx++; }
    if (inChar == '$')                          { packetBegin = true;               // found start of packet (5)
                                                  receivedMessage[msgIdx] = inChar;
                                                  receivedMessage[msgIdx+1] = 0;
                                                  msgIdx++;}
    }
}

bool sendMsg(const char *dev_id, const char *out_message) {
  char crc[3];
  char tmp_crc[6];
  char tmp_str[BUF_SIZE];

  messageCount++;
  wantToSend=true;
  strcpy(messageToSend,out_message);
  if (!dataLineBusy)
    {
    wantToSend = false;
    dataLineTimeout = millis();
    strcpy(tmp_str, "$");
    strcat(tmp_str, dev_id);
    strcat(tmp_str, ".");
    itoa(messageCount, tmp_crc, 10);
    strcat(tmp_str, tmp_crc);
    strcat(tmp_str, ":");
    strcat(tmp_str, messageToSend);
    strcat(tmp_str, "*");
    sprintf(crc, "%02X", computeChecksum(tmp_str));
    Serial.print(tmp_str);
    Serial.println(crc);
    Serial.flush();
    }
  return true;
}

void rcvTimeout(){
  receiveComplete = false;
  dataLineBusy    = false;//assuming that another device finishes transmit
  dataLineTimeout = 0;
  packetBegin = false;
  packetEnd = false;
  //packetCrc = 0;
  receivedCrc[0] = 0;
  receivedMessage[0] = 0;
}

char computeChecksum(const char * sentence)
{
    unsigned char calculated_checksum = 0;
    for (int i = 1; i != strlen(sentence) && sentence[i] != '*'; ++i) calculated_checksum ^= (unsigned char) sentence[i];//skipping $ and * from calculating
    return(calculated_checksum);
}
