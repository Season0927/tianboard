#ifndef _TERMINAL_TASK_H_
#define _TERMINAL_TASK_H_

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "string.h"

#define CLEAR_SCREEN "\33[2J"

#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RED "\033[31m"

#define HIGHLIGHT "\33[1m"

#define CLEAR "\033[0m"

#define TERMINAL_PROMPT GREEN"Tianbot> "CLEAR

#define TERMINAL_RECV_Q_SIZE 64
#define TERMINAL_SEND_Q_SIZE 16
#define TERMINAL_SEND_BUFF_SIZE 256
#define TERMINAL_CMD_BUFF_SIZE 64

#define UART_TX_FINISH 1

#define CMD_NAME_MAX_LEN 24
#define CMD_DESC_MAX_LEN 256

#define CFG_MAXARGS 16

typedef struct{
	uint8_t Msg[TERMINAL_SEND_BUFF_SIZE];
	uint16_t MsgLen;
}TerminalTxMsg_t;

typedef struct{
  char CmdName[CMD_NAME_MAX_LEN];
  void (*pCmdEntry)(int argc, char* argv[]);
  char Desc[CMD_DESC_MAX_LEN];
}Cmd_t;

#define ADD_CMD(cmd, func, desc) const Cmd_t Struct_##func __attribute__((section(".CMD_SECTOR"))) = {cmd, func, desc};

extern char CMD_SECTOR_Limit;
extern char CMD_SECTOR_Base;

enum{
  KEY_CTRL_C = 0x03,
  KEY_TAB = 0x09, 
	KEY_ENTER = 0x0D,
	KEY_HOME = 0x31,
	KEY_DEL = 0x33,
	KEY_END = 0x34,
	KEY_UP = 0x41,
  KEY_DOWN = 0x42,
	KEY_RIGHT = 0x43,
	KEY_LEFT = 0x44,
  KEY_BACKSPACE = 0x7F,
};

typedef struct{
  int argc;
  char **argv;
  void (*pFunc)(int argc, char *argv[]);
}TaskArgu_t;

extern uint8_t *pTerminalMsg;
extern osMailQId TerminalRxMail;
extern osThreadId TerminalRxTaskHandle;
extern osThreadId TerminalTxTaskHandle;
void TerminalTaskInit(void);

void tprintf(const char *format,...);
#endif
