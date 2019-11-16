#include "protocol_task.h"
#include "cmsis_os.h"
#include "dbus_task.h"
#include "beep_task.h"
#include "param.h"
#include "usart.h"
#include "dbus.h"
#include "crc.h"
#include "stdlib.h"
#include "string.h"
#include "terminal_task.h"
osMailQId ProtocolRxMail;
osMailQId ProtocolTxMail;
osMailQDef(ProtocolRxMail, PROTOCOL_MSG_QUENE_SIZE, ProtocolMsg_t);
osMailQDef(ProtocolTxMail, PROTOCOL_MSG_QUENE_SIZE, ProtocolMsg_t);
ProtocolMsg_t *pProtocolMsg;

osThreadId ProtocolSendTaskHandle;
osThreadId ProtocolRecvTaskHandle;

uint8_t ConnectStatus = DISCONNECTED;

static void BeepConnect(void)
{
  Beep(0, 150);
  Beep(1, 150);
  Beep(2, 150);
}

static void BeepDisconnect(void)
{
  Beep(2, 150);
  Beep(1, 150);
  Beep(0, 150);
}

static int ParseLine(char *line, char *argv[])
{
  int nargs = 0;
  while (nargs < 10)
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

    while (*line && (*line != ' ') && (*line != '\t') && (*line != '\n') && (*line != '\r'))
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

void ProtocolSend(struct cmd_chassis_info info)
{
  ProtocolMsg_t *p;
  p = osMailAlloc(ProtocolTxMail, osWaitForever);
  if (p != NULL)
  {
    static uint16_t seq_num = 0;
    uint8_t frame_size = sizeof(struct cmd_chassis_info) + HEAD_LEN + CMD_SIZE + CRC32_SIZE;
    protocol_pack_desc_t *pBuff = (protocol_pack_desc_t *)(p->Msg);
    ProtocolCmd_t *pCmd = (ProtocolCmd_t *)pBuff->pdata;
    memset(pBuff, 0, frame_size);
    pBuff->sof = PROCOTOL_HEAD;
    pBuff->VL_u.VL_s.data_len = frame_size;
    pBuff->VL_u.VL_s.version = 0;
    pBuff->SAR_u.SAR_s.pack_type = 0;
    pBuff->SAR_u.SAR_s.session = 0;
    pBuff->sender = 1;
    pBuff->reciver = 0;
    pBuff->seq_num = seq_num++;
    pCmd->cmd = CMD_CHASSIS_INFO;
    memcpy(pCmd->pdata, &info, sizeof(struct cmd_chassis_info));
    append_crc16(p->Msg, HEAD_LEN);
    append_crc32(p->Msg, frame_size);
    p->MsgLen = frame_size;
    osMailPut(ProtocolTxMail, p);
  }
}

static void ProtocolProcess(uint8_t *Buf, uint8_t Len)
{
  if (param.base_type == BASE_TYPE_RACECAR)
  {
    int argc;
    char *argv[10];
    if (Len >= PROTOCOL_MSG_LEN)
    {
      Len = PROTOCOL_MSG_LEN - 1;
    }
    Buf[Len] = '\0';
    argc = ParseLine((char *)Buf, argv);
    if (strncmp(argv[0], "s", 2) == 0)
    {
      if (argc == 3)
      {
        if (ConnectStatus == DISCONNECTED)
        {
          ConnectStatus = CONNECTETED;
          BeepConnect();
        }
        MotionCtrl_t *pMotionData = osMailAlloc(CtrlMail, osWaitForever);
        if (pMotionData != NULL)
        {
          pMotionData->vy = atoi(argv[1]);
          pMotionData->steer_angle = atoi(argv[2]);
          osMailPut(CtrlMail, pMotionData);
        }
        else
        {
          //err
        }
      }
    }
  }
  else //icra protocol
  {
    protocol_pack_desc_t *p = (protocol_pack_desc_t *)Buf;
    CmdSpd_t *pSpd;
    CmdSpdAcc_t *pSpdAcc;
    MotionCtrl_t *pMotionData;
    if ((p->sof == PROCOTOL_HEAD) && (p->VL_u.VL_s.data_len == Len))
    {
      if (verify_crc16((uint8_t *)p, HEAD_LEN))
      {
        if (verify_crc32((uint8_t *)p, Len))
        {
          if (ConnectStatus == DISCONNECTED)
          {
            ConnectStatus = CONNECTETED;
            BeepConnect();
          }
          if (p->SAR_u.SAR_s.pack_type != PROTOCOL_PACK_ACK)
          {
            ProtocolCmd_t *pCmd = (ProtocolCmd_t *)p->pdata;
            switch (pCmd->cmd)
            {
            case CMD_SET_CHASSIS_SPD:
              pSpd = (CmdSpd_t *)pCmd->pdata;
              pMotionData = osMailAlloc(CtrlMail, osWaitForever);
              if (pMotionData != NULL)
              {
                pMotionData->vx = pSpd->vx;
                pMotionData->vy = -pSpd->vy;
                pMotionData->w = pSpd->vw / 10 / RADIAN_COEF;
                osMailPut(CtrlMail, pMotionData);
              }
              break;

            case CMD_SET_CHASSIS_SPD_ACC:
              pSpdAcc = (CmdSpdAcc_t *)pCmd->pdata;
              pMotionData = osMailAlloc(CtrlMail, osWaitForever);
              if (pMotionData != NULL)
              {
                pMotionData->vx = pSpdAcc->vx + pSpdAcc->ax / 1000 * param.ctrl_period;
                pMotionData->vy = -(pSpdAcc->vy + pSpdAcc->ay / 1000 * param.ctrl_period);
                pMotionData->w = (pSpdAcc->vw + pSpdAcc->wz / 1000 * param.ctrl_period) / 10 / RADIAN_COEF;
                osMailPut(CtrlMail, pMotionData);
              }
              break;

            case CMD_PC_HEART:
              break;

            default:
              break;
            }
            if (p->SAR_u.SAR_s.session != 0)
            {
              //send ack
            }
          }
        }
      }
    }
  }
}

int message_process = 0;

static void ProtocolRecvTaskEntry(void const *argument)
{
  osEvent evt;
  ProtocolMsg_t *p;
  osDelay(5000);
  pProtocolMsg = osMailAlloc(ProtocolRxMail, osWaitForever);
  HAL_UART_Receive_DMA(&huart7, pProtocolMsg->Msg, PROTOCOL_MSG_LEN);
  __HAL_UART_CLEAR_IDLEFLAG(&huart7);
  __HAL_UART_ENABLE_IT(&huart7, UART_IT_IDLE);
  /* Infinite loop */
  for (;;)
  {
    evt = osMailGet(ProtocolRxMail, PROTOCOL_TIMEOUT);
    if (evt.status == osEventMail)
    {
      p = evt.value.p;
      if (CtrlFlag == CTRL_TYPE_PC)
      {
        ProtocolProcess(p->Msg, p->MsgLen);
      }
      osMailFree(ProtocolRxMail, p);
      message_process++;
    }
    else if (evt.status == osEventTimeout)
    {
      if (CtrlFlag == CTRL_TYPE_PC)
      {
        if (ConnectStatus == CONNECTETED)
        {
          ConnectStatus = DISCONNECTED;
          BeepDisconnect();
        }
        //disconnect
        MotionCtrl_t *pMotionData = osMailAlloc(CtrlMail, osWaitForever);
        if (pMotionData != NULL)
        {
          if (param.base_type == BASE_TYPE_RACECAR)
          {
            pMotionData->vy = 1500;
            pMotionData->steer_angle = 90;
          }
          else
          {
            pMotionData->vx = 0;
            pMotionData->vy = 0;
            pMotionData->w = 0;
          }
          osMailPut(CtrlMail, pMotionData);
        }
      }
    }
  }
}

static void ProtocolSendTaskEntry(void const *argument)
{
  ProtocolMsg_t *p;
  osEvent evt;
  /* Infinite loop */
  for (;;)
  {
    evt = osMailGet(ProtocolTxMail, osWaitForever);
    if (evt.status == osEventMail)
    {
      p = evt.value.p;
      HAL_UART_Transmit_DMA(&huart7, p->Msg, p->MsgLen);
      osSignalWait(UART7_TX_FINISH, osWaitForever);
      osMailFree(ProtocolTxMail, p);
    }
  }
}
osThreadDef(ProtocolRecvTask, ProtocolRecvTaskEntry, osPriorityRealtime, 0, 512);
osThreadDef(ProtocolSendTask, ProtocolSendTaskEntry, osPriorityAboveNormal, 0, 512);
void ProtocolTaskInit(void)
{
  ProtocolRxMail = osMailCreate(osMailQ(ProtocolRxMail), NULL);
  ProtocolTxMail = osMailCreate(osMailQ(ProtocolTxMail), NULL);

  ProtocolRecvTaskHandle = osThreadCreate(osThread(ProtocolRecvTask), NULL);
  ProtocolSendTaskHandle = osThreadCreate(osThread(ProtocolSendTask), NULL);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == UART7)
  {
    osSignalSet(ProtocolSendTaskHandle, UART7_TX_FINISH);
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == UART7)
  {
    pProtocolMsg->MsgLen = PROTOCOL_MSG_LEN - huart7.hdmarx->Instance->NDTR;

    osMailPut(ProtocolRxMail, pProtocolMsg);

    pProtocolMsg = osMailAlloc(ProtocolRxMail, 0);
    if (pProtocolMsg == NULL)
    {
      //error
    }
    else
    {
      HAL_UART_Receive_DMA(&huart7, pProtocolMsg->Msg, PROTOCOL_MSG_LEN);
    }
  }
}
