#include "racecar_task.h"
#include "cmsis_os.h"
#include "dbus.h"
#include "dbus_task.h"
#include "param.h"
#include "tim.h"
#include "protocol_task.h"
#include "imu.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

osThreadId RacecarCtrlTaskHandle;
osThreadId RacecarFeedbackTaskHandle;

static void RacecarCtrlTaskEntry(void const *argument)
{
  osEvent evt;
  MotionCtrl_t *p;

  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);

  TIM8->CCR3 = LIMIT(RACECAR_SPEED_ZERO, MOTOR_MIN, MOTOR_MAX);
  TIM8->CCR2 = LIMIT(SERVO_CAL(RACECAR_STEER_ANGLE_ZERO), SERVO_MIN, SERVO_MAX);

  for (;;)
  {
    evt = osMailGet(CtrlMail, RACECAR_CTRL_TIMEOUT);

    if (evt.status == osEventMail)
    {
      p = evt.value.p;

      TIM8->CCR3 = LIMIT(p->vy, MOTOR_MIN, MOTOR_MAX);
      TIM8->CCR2 = LIMIT(SERVO_CAL(p->steer_angle), SERVO_MIN, SERVO_MAX);
      ;
      osMailFree(CtrlMail, p);
    }
    else
    {
      TIM8->CCR3 = LIMIT(RACECAR_SPEED_ZERO, MOTOR_MIN, MOTOR_MAX);
      TIM8->CCR2 = LIMIT(SERVO_CAL(RACECAR_STEER_ANGLE_ZERO), SERVO_MIN, SERVO_MAX);
    }
  }
}

static void RacecarFeedbackTaskEntry(void const *argument)
{
  while (1)
  {
    ProtocolMsg_t *p;
    p = osMailAlloc(ProtocolTxMail, osWaitForever);
    if (p != NULL)
    {
      sprintf((char *)p->Msg, "i\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", imu.q[1] / 65535.0f / 16384.0f,
              imu.q[2] / 65535.0f / 16384.0f,
              imu.q[3] / 65535.0f / 16384.0f,
              imu.q[0] / 65535.0f / 16384.0f,
              imu.wx,
              imu.wy,
              imu.wz,
              imu.ax / 4096.0f * 9.8f,
              imu.ay / 4096.0f * 9.8f,
              imu.az / 4096.0f * 9.8f);
      p->MsgLen = strlen((char *)p->Msg);
      osMailPut(ProtocolTxMail, p);
    }
    osDelay(param.feedback_period);
  }
}
osThreadDef(RacecarCtrlTask, RacecarCtrlTaskEntry, osPriorityAboveNormal, 0, 512);
osThreadDef(RacecarFeedbackTask, RacecarFeedbackTaskEntry, osPriorityAboveNormal, 0, 512);
void RacecarTaskInit(void)
{
  //for racecar protocol baudrate 115200, for icra protocol baudrate 921600
  huart7.Instance->BRR = UART_BRR_SAMPLING16(HAL_RCC_GetPCLK1Freq(), 115200);

  RacecarCtrlTaskHandle = osThreadCreate(osThread(RacecarCtrlTask), NULL);
  RacecarFeedbackTaskHandle = osThreadCreate(osThread(RacecarFeedbackTask), NULL);
}
