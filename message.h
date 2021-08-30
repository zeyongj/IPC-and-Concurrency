#ifndef _MESSAGE_H_
#define _MESSAGE_H_
#define DATA_BUF_LEN 1024

enum MSG_TYPE{
    CHAT_MSG = 0,
    SHUTDOWN = 1,
};

typedef unsigned short msg_header;
typedef struct 
{
    //uint16_t port; //send to port
    unsigned short cmd;
    char chat[DATA_BUF_LEN];
	
} chat_msg;


#endif
//_MESSAGE_H_
