#include "dbus_task.h"
#include "dbus.h"
#include "cmsis_os.h"
#include "usart.h"
#include "stdlib.h"
#include "param.h"

osMailQId DbusMail;
osMailQId CtrlMail;
osMailQDef(DbusMail, DBUS_MSG_QUENE_SIZE, DbusMsg_t);
osMailQDef(CtrlMail, CTRL_MSG_QUENE_SIZE, MotionCtrl_t);
DbusMsg_t *pDbusMsg;

osThreadId DbusTaskHandle;

volatile uint32_t CtrlFlag = CTRL_TYPE_PC;

static void DbusTaskEntry(void const *argument)
{
  osEvent evt;
  DbusMsg_t *p;

  osDelay(1000);

  pDbusMsg = osMailAlloc(DbusMail, osWaitForever);
  HAL_UART_Receive_DMA(&huart1, pDbusMsg->Msg, DBUS_MSG_LEN);
  __HAL_UART_CLEAR_IDLEFLAG(&huart1);
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
  /* Infinite loop */
  for (;;)
  {
    evt = osMailGet(DbusMail, DBUS_TIMEOUT);
    if (evt.status == osEventMail)
    {
      p = evt.value.p;

      if (p->MsgLen == RC_FRAME_LENGTH)
      {
        RC_Ctl_t CtrlData;
        RemoteDataProcess(p->Msg, &CtrlData);
        if (CtrlData.rc.s1 == RC_SW_UP)
        {
          CtrlFlag = CTRL_TYPE_PC;
        }
        else
        {
          MotionCtrl_t *pMotionData = osMailAlloc(CtrlMail, osWaitForever);
          CtrlFlag = CTRL_TYPE_DBUS_RC;
          if (abs(CtrlData.rc.ch2 - RC_CH_VALUE_OFFSET) < RC_MIN_CTRL)
          {
            CtrlData.rc.ch2 = RC_CH_VALUE_OFFSET;
          }

          if (abs(CtrlData.rc.ch3 - RC_CH_VALUE_OFFSET) < RC_MIN_CTRL)
          {
            CtrlData.rc.ch3 = RC_CH_VALUE_OFFSET;
          }

          if (abs(CtrlData.rc.ch4 - RC_CH_VALUE_OFFSET) < RC_MIN_CTRL)
          {
            CtrlData.rc.ch4 = RC_CH_VALUE_OFFSET;
          }

          if (abs(CtrlData.rc.ch0 - RC_CH_VALUE_OFFSET) < RC_MIN_CTRL)
          {
            CtrlData.rc.ch0 = RC_CH_VALUE_OFFSET;
          }

          pMotionData->vx = (CtrlData.rc.ch3 - RC_CH_VALUE_OFFSET) * param.max_speed / (RC_CH_VALUE_MAX - RC_CH_VALUE_MIN);
          pMotionData->vy = (CtrlData.rc.ch2 - RC_CH_VALUE_OFFSET) * param.max_speed / (RC_CH_VALUE_MAX - RC_CH_VALUE_MIN);
          pMotionData->w = (CtrlData.rc.ch4 - RC_CH_VALUE_OFFSET) * param.max_w / (RC_CH_VALUE_MAX - RC_CH_VALUE_MIN);
          if (param.base_type == BASE_TYPE_RACECAR)
          {
            pMotionData->vx = (CtrlData.rc.ch3 - RC_CH_VALUE_OFFSET) * (1800 - 1200) / (RC_CH_VALUE_MAX - RC_CH_VALUE_MIN) + 1500;
            pMotionData->steer_angle = (CtrlData.rc.ch0 - RC_CH_VALUE_OFFSET) * (120 - 60) / (RC_CH_VALUE_MAX - RC_CH_VALUE_MIN) + 90;
          }
          osMailPut(CtrlMail, pMotionData);
        }
      }

      osMailFree(DbusMail, p);
    }
    else if (evt.status == osEventTimeout)
    {
      CtrlFlag = CTRL_TYPE_PC;
    }
  }
}
osThreadDef(DbusTask, DbusTaskEntry, osPriorityRealtime, 0, 512);
void DbusTaskInit(void)
{
  DbusMail = osMailCreate(osMailQ(DbusMail), NULL);
  CtrlMail = osMailCreate(osMailQ(CtrlMail), NULL);

  DbusTaskHandle = osThreadCreate(osThread(DbusTask), NULL);
}
