#include "terminal_task.h"
#include "cmsis_os.h"
#include "usart.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"

#define TASK_QUIT_SIGNAL 0x01

osThreadId TerminalRxTaskHandle;
osThreadId TerminalTxTaskHandle;
osThreadId TerminalExecTaskHandle = NULL;
osMailQId TerminalRxMail;
osMailQId TerminalTxMail;
osMailQDef(TerminalRxMail, TERMINAL_RECV_Q_SIZE, uint8_t);
osMailQDef(TerminalTxMail, TERMINAL_SEND_Q_SIZE, TerminalTxMsg_t);
uint8_t *pTerminalMsg;

static const char logo[][85] = {
    {"      OOOO\r\n"},
    {"     OOOOOO\r\n"},
    {"      OOOO   OOOOOOOO\r\n"},
    {"       OOOOOOOOOOOOOOOOOO\r\n"},
    {"       OOOOOO         OOOOOO\r\n"},
    {"     OOOO                 OOOO\r\n"},
    {"   OOOO                     OOOO\r\n"},
    {"  OOOO                       OOOO\r\n"},
    {" OOOO   OOOOOOOOOOOOOOOOOOO    OOO\r\n"},
    {" OOO    OOOOOOOOOOOOOOOOOOO    OOO\r\n"},
    {" OOO                           OOO\r\n"},
    {" OOO                           OOO\r\n"},
    {"\r\n"},
    {"   OOOOOOOOOO OOO  OOOOOO       OOO    OOO  OOOOOOOOO    OOOOOOO    OOOOOOOOOO\r\n"},
    {"  OOOOOOOOOO OOO  OOOOOOO      OOOO   OOO  OOO    OOO  OOO   OOOO  OOOOOOOOOO\r\n"},
    {"    OOOO    OOO  OOO  OOO     OOOOO  OOO  OOOOOOOOOO  OOO    OOOO    OOOO \r\n"},
    {"   OOOO    OOO  OOOOOOOOO    OOO OO OOO  OOOOOOOOOO  OOO    OOOO    OOOO\r\n"},
    {"  OOOO    OOO  OOOOOOOOOO   OOO  OOOOO  OOO    OOOO OOO    OOOO    OOOO \r\n"},
    {" OOOO    OOO  OOOO    OOO  OOO   OOOO  OOOOOOOOOO    OOOOOOOO    OOOO\r\n"},
    {"\r\n"},
    {"                                                                www.tianbot.com\r\n"}};

void tprintf(const char *format, ...)
{
  TerminalTxMsg_t *p;
  p = osMailAlloc(TerminalTxMail, osWaitForever);
  if (p != NULL)
  {
    va_list ap;
    va_start(ap, format);
    vsnprintf((char *)p->Msg, sizeof(p->Msg), format, ap);
    va_end(ap);
    p->MsgLen = strlen((char *)p->Msg);
    osMailPut(TerminalTxMail, p);
  }
  else
  {
    //err
  }
}

static int ParseLine(char *line, char *argv[])
{
  int nargs = 0;
  while (nargs < CFG_MAXARGS)
  {
    while ((*line == ' ') || (*line == '\t'))
    {
      ++line;
    }

    if (*line == '\0' || (*line == '\r') || (*line == '\n'))
    {
      *line = '\0';
      argv[nargs] = NULL;
      return nargs;
    }

    argv[nargs++] = line;

    while (*line && (*line != ' ') && (*line != '\t'))
    {
      ++line;
    }

    if ((*line == '\0') || (*line == '\r') || (*line == '\n'))
    {
      *line = '\0';
      argv[nargs] = NULL;
      return nargs;
    }

    *line++ = '\0';
  }

  return nargs;
}

static void DelCh(char *str, int index)
{
  int i;
  if (index < 0)
  {
    return;
  }
  if (index > strlen(str))
  {
    return;
  }
  for (i = index; i < strlen(str); i++)
  {
    str[i] = str[i + 1];
  }
}

static void InsCh(char *str, int index, char ch)
{
  int i;
  for (i = strlen(str) + 1; i > index; i--)
  {
    str[i] = str[i - 1];
  }
  str[i] = ch;
}

static void TerminalExecTaskEntry(void const *argument)
{
  TaskArgu_t *p = (TaskArgu_t *)argument;
  p->pFunc(p->argc, p->argv);
  TerminalExecTaskHandle = NULL;
  tprintf(TERMINAL_PROMPT);
  osThreadTerminate(osThreadGetId());
  while (1)
    ;
}

static void TerminalProc(uint8_t byte)
{
  uint32_t length;
  uint32_t base;
  uint16_t i;
  uint16_t cmdMatchCnt = 0;
  Cmd_t *pCmd;
  static uint8_t cmd[TERMINAL_CMD_BUFF_SIZE];
  static uint16_t index = 0;
  static uint16_t cursor = 0;
  static uint8_t ctrlKeyStatus = 0;
  static char *argv[CFG_MAXARGS];
  static int argc;

  if (byte == KEY_CTRL_C)
  {
    if (TerminalExecTaskHandle != NULL)
    {
      osThreadTerminate(TerminalExecTaskHandle);
      TerminalExecTaskHandle = NULL;
    }
    cursor = 0;
    index = 0;
    cmd[index] = '\0';
    tprintf("\r\n" TERMINAL_PROMPT);
    return;
  }
  else if (TerminalExecTaskHandle != NULL)
  {
    return;
  }
  switch (ctrlKeyStatus)
  {
  case 0:
    switch (byte)
    {
    case 0x1B:
      ctrlKeyStatus++;
      break;

    case KEY_BACKSPACE:
      if (cursor != 0 && index != 0)
      {
        cursor--;
        DelCh((char *)cmd, cursor);
        index--;
        tprintf("%c", byte);
        if (index != cursor)
        {
          tprintf("%s\x1B\x5B\x43%c", cmd + cursor, byte);
          i = index;
          while (i != cursor)
          {
            tprintf("\x1B\x5B\x44");
            osDelay(1);
            i--;
          }
        }
      }
      break;

    case KEY_ENTER:
      tprintf("\r\n");
      if (index != 0)
      {
        length = (uint32_t)(&CMD_SECTOR_Limit - &CMD_SECTOR_Base);
        base = (uint32_t)&CMD_SECTOR_Base;

        argc = ParseLine((char *)cmd, argv);
        pCmd = (Cmd_t *)base;
        for (i = 0; i < length / sizeof(Cmd_t); i++)
        {
          if (strcmp((char *)cmd, pCmd[i].CmdName) == 0)
          {
            //pCmd[i].pCmdEntry(argc, argv);
            static TaskArgu_t task;
            task.argc = argc;
            task.argv = argv;
            task.pFunc = pCmd[i].pCmdEntry;
            osThreadDef(TerminalExecTask, TerminalExecTaskEntry, osPriorityNormal, 0, 2048);
            TerminalExecTaskHandle = osThreadCreate(osThread(TerminalExecTask), &task);
            break;
          }
        }
        if (i == length / sizeof(Cmd_t))
        {
          tprintf(YELLOW "command \"" HIGHLIGHT "%s" CLEAR YELLOW "\" not found." CLEAR "\r\n" TERMINAL_PROMPT, argv[0]);
        }
      }
      else
      {
        tprintf(TERMINAL_PROMPT);
      }
      cursor = 0;
      index = 0;
      cmd[index] = '\0';
      break;

    case KEY_TAB:
      length = (uint32_t)(&CMD_SECTOR_Limit - &CMD_SECTOR_Base);
      base = (uint32_t)&CMD_SECTOR_Base;
      pCmd = (Cmd_t *)base;
      cmdMatchCnt = 0;
      for (i = 0; i < length / sizeof(Cmd_t); i++)
      {
        if (strncmp((char *)cmd, pCmd[i].CmdName, strlen((char *)cmd)) == 0)
        {
          cmdMatchCnt++;
        }
      }
      if (cmdMatchCnt > 1)
      {
        tprintf("\r\n");
        for (i = 0; i < length / sizeof(Cmd_t); i++)
        {
          if (strncmp((char *)cmd, pCmd[i].CmdName, strlen((char *)cmd)) == 0)
          {
            tprintf("%s\t", pCmd[i].CmdName);
            osDelay(strlen(pCmd[i].CmdName) / 10 + 1);
          }
        }
        tprintf("\r\n" TERMINAL_PROMPT "%s", cmd);
        if (index != cursor)
        {
          i = index;
          while (i != cursor)
          {
            tprintf("\x1B\x5B\x44");
            osDelay(1);
            i--;
          }
        }
      }
      else if (cmdMatchCnt == 1)
      {
        for (i = 0; i < length / sizeof(Cmd_t); i++)
        {
          if (strncmp((char *)cmd, pCmd[i].CmdName, strlen((char *)cmd)) == 0)
          {
            break;
          }
        }

        if (index != cursor)
        {
          uint16_t j;
          j = cursor;
          while (j != index)
          {
            tprintf("\x1B\x5B\x43");
            osDelay(1);
            j++;
          }
        }
        tprintf("%s ", pCmd[i].CmdName + strlen((char *)cmd));
        index = strlen(pCmd[i].CmdName) + 1;
        sprintf((char *)cmd, "%s ", pCmd[i].CmdName);
        cursor = index;
      }
      break;

    default:
      if (index < TERMINAL_CMD_BUFF_SIZE - 1)
      {
        tprintf("%c%s", byte, cmd + cursor);
        InsCh((char *)cmd, cursor, byte);
        index++;
        cursor++;
        i = index;
        while (i != cursor)
        {
          tprintf("\x1B\x5B\x44");
          osDelay(1);
          i--;
        }
      }
      else
      {
        tprintf("\r\n" YELLOW "Reach max command length\r\n" CLEAR);
        tprintf(TERMINAL_PROMPT "%s", cmd);
      }
      break;
    }
    break;

  case 1:
    switch (byte)
    {
    case 0x5B:
      ctrlKeyStatus++;
      break;

    default:
      ctrlKeyStatus = 0;
    }
    break;

  case 2:
    switch (byte)
    {
    case KEY_UP:
      ctrlKeyStatus = 0;
      break;

    case KEY_DOWN:
      ctrlKeyStatus = 0;
      break;

    case KEY_LEFT:
      if (cursor > 0)
      {
        cursor--;
        tprintf("\x1B\x5B\x44");
      }
      ctrlKeyStatus = 0;
      break;

    case KEY_RIGHT:
      if (cursor < index)
      {
        cursor++;
        tprintf("\x1B\x5B\x43");
      }
      ctrlKeyStatus = 0;
      break;

    case KEY_HOME:
      while (cursor != 0)
      {
        tprintf("\x1B\x5B\x44");
        osDelay(1);
        cursor--;
      }
      ctrlKeyStatus++;
      break;

    case KEY_END:
      while (cursor != index)
      {
        tprintf("\x1B\x5B\x43");
        osDelay(1);
        cursor++;
      }
      ctrlKeyStatus++;
      break;

    case KEY_DEL:
      if (cursor < index)
      {
        index--;
        DelCh((char *)cmd, cursor);
        tprintf("\x1B\x5B\x43\x7F");
        if (index != cursor)
        {
          tprintf("%s\x1B\x5B\x43\x7F", cmd + cursor);
          i = index;
          while (i != cursor)
          {
            tprintf("\x1B\x5B\x44");
            osDelay(1);
            i--;
          }
        }
      }
      ctrlKeyStatus++;
      break;

    default:
      ctrlKeyStatus = 0;
      break;
    }
    break;

  case 3:
    switch (byte)
    {
    case 0x7E:
      ctrlKeyStatus = 0;
      break;

    default:
      ctrlKeyStatus = 0;
      break;
    }
    break;

  default:
    ctrlKeyStatus = 0;
    break;
  }
}

static void TerminalRxTaskEntry(void const *argument)
{
  osEvent evt;
  uint8_t *p;
  int i;

  tprintf(CLEAR_SCREEN "\r\n");
  osDelay(10);
  for (i = 0; i < sizeof(logo) / sizeof(logo[0]); i++)
  {
    tprintf("%s", logo[i]);
    osDelay(10);
  }
  tprintf("\r\n" TERMINAL_PROMPT);

  pTerminalMsg = osMailAlloc(TerminalRxMail, osWaitForever);
  __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_TC);
  __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_RXNE);
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_TC);

  for (;;)
  {
    evt = osMailGet(TerminalRxMail, osWaitForever);
    if (evt.status == osEventMail)
    {
      p = evt.value.p;

      TerminalProc(*p);

      osMailFree(TerminalRxMail, p);
    }
  }
}

void TerminalTxTaskEntry(void const *argument)
{
  /* USER CODE BEGIN SerialTxTaskEntry */
  TerminalTxMsg_t *p;
  osEvent evt;
  /* Infinite loop */
  for (;;)
  {
    evt = osMailGet(TerminalTxMail, osWaitForever);
    if (evt.status == osEventMail)
    {
      p = evt.value.p;
      HAL_UART_Transmit_DMA(&huart2, p->Msg, p->MsgLen);
      osSignalWait(UART_TX_FINISH, osWaitForever);
      osMailFree(TerminalTxMail, p);
    }
  }
  /* USER CODE END SerialTxTaskEntry */
}

void TerminalTaskInit(void)
{
  TerminalRxMail = osMailCreate(osMailQ(TerminalRxMail), NULL);
  TerminalTxMail = osMailCreate(osMailQ(TerminalTxMail), NULL);
  osThreadDef(TerminalRxTask, TerminalRxTaskEntry, osPriorityAboveNormal, 0, 512);
  TerminalRxTaskHandle = osThreadCreate(osThread(TerminalRxTask), NULL);
  osThreadDef(TerminalTxTask, TerminalTxTaskEntry, osPriorityHigh, 0, 512);
  TerminalTxTaskHandle = osThreadCreate(osThread(TerminalTxTask), NULL);
}
