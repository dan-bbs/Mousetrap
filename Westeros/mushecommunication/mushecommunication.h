#ifndef __MUSHECOMMUNICATION_H__
#define __MUSHECOMMUNICATION_H__

#define BUF_SIZE 100  //100 bytes of message

extern char receivedMessage[BUF_SIZE];
extern char messageToSend[BUF_SIZE];
extern bool dataLineBusy;
extern long dataLineTimeout;  //timer to abort waiting end of incoming message
extern bool receiveComplete;  //flag to proceed incoming message
extern bool wantToSend;
extern long messageCount;
extern byte messageTries;
extern byte messageTriesMax;

typedef enum {_format_error, _crc_error, _general_error, _parse_ok} rcv_errors;

void serialEvent();                                        // Interrupt handler to receive data from Serial
bool sendMsg(const char *dev_id, const char *out_message); // Prepare and send message 
void rcvTimeout();                                         // Reset comm flags
char computeChecksum(const char *sentence);                // Calculate XOR CRC

#endif
