#include "omni_task.h"
#include "cmsis_os.h"
#include "dbus.h"
#include "misc.h"
#include "can.h"
#include "dbus_task.h"
#include "omni_ctrl.h"
#include "pid.h"
#include "param.h"
#include "stdlib.h"
#include "protocol_task.h"
#include "imu.h"
#include "math.h"

osThreadId OmniCtrlTaskHandle;
osThreadId OmniFeedbackTaskHandle;
osThreadId OmniPoseTaskHandle;

typedef struct Position
{
  float v_x_mm;
  float v_y_mm;
  float rate_deg;
  float position_x_mm;
  float position_y_mm;
  float angle_deg;
}Pos_t;

static Pos_t pose;


static void OmniCtrlTaskEntry(void const * argument)
{
  osEvent evt;
  MotionCtrl_t *p;
  Pid_t wheelPid[3];
  float w,vx,vy;
  
  int timeout = 0;
  int16_t motoCurrent[3] = {0,0,0};
  int16_t v[3] = {0,0,0};
  
  osDelay(1000);
  
  OmniMotoPower(1);
  CanParamInit(&hcan1);
  
  PidInit(&wheelPid[0], POSITION_PID, param.pid.max_out, param.pid.i_limit, param.pid.p, param.pid.i, param.pid.d);
  PidInit(&wheelPid[1], POSITION_PID, param.pid.max_out, param.pid.i_limit, param.pid.p, param.pid.i, param.pid.d);
  PidInit(&wheelPid[2], POSITION_PID, param.pid.max_out, param.pid.i_limit, param.pid.p, param.pid.i, param.pid.d);
  
  for(;;)
  {
    evt = osMailGet(CtrlMail, param.ctrl_period);
    if (evt.status == osEventMail) 
    {
      timeout = 0;
      p = evt.value.p;
      
      vx = p->vx;
      vy = p->vy;
      w = p->w;

      v[0] = (vx+w*param.base.omni.base_r)/(2*PI*param.wheel_r)*60.0f;
      v[1] = (-vx*COS_60+vy*SIN_60+w*param.base.omni.base_r)/(2*PI*param.wheel_r)*60.0f;
      v[2] = (-vx*COS_60-vy*SIN_60+w*param.base.omni.base_r)/(2*PI*param.wheel_r)*60.0f;
      
      osMailFree(CtrlMail, p);
    }
    else
    {
      timeout++;
      if(timeout >= 100)
      {
        v[0] = 0;
        v[1] = 0;
        v[2] = 0;
        timeout = 0;
      }
    }
    motoCurrent[0] = PidCalc(&wheelPid[0], motoInfo[0].w, v[0]);
    motoCurrent[1] = PidCalc(&wheelPid[1], motoInfo[1].w, v[1]);
    motoCurrent[2] = PidCalc(&wheelPid[2], motoInfo[2].w, v[2]);
    SetOmniMoto(motoCurrent[0], motoCurrent[1], motoCurrent[2]);
  }
}

static void OmniFeedbackTaskEntry(void const * argument)
{
  while(1)
  {
    float tmp;
    struct cmd_chassis_info info;

    tmp= ((int)imu.yaw%360) + imu.yaw - (int)imu.yaw;
    if(tmp > 180)
    {
      tmp -= 360;
    }
    info.gyro_angle = tmp*10;
    info.gyro_palstance = imu.wz*RADIAN_COEF*10;
    info.angle_deg = pose.angle_deg*10;
    info.position_x_mm = pose.position_x_mm;
    info.position_y_mm = pose.position_y_mm;
    info.v_x_mm = pose.v_x_mm;
    info.v_y_mm = pose.v_y_mm;
    ProtocolSend(info);
    osDelay(param.feedback_period);
  }
}

static void OmniPoseTaskEntry(void const * argument)
{
  int i;
  int DeltaTick[3];
  float delta_angle = 0.0;
  float delta_pos[3];
  float delta_x_mm, delta_y_mm;
  float angle;
  memset(&pose, 0, sizeof(pose));
  for(;;)
  {
    for(i=0; i<3; i++)
    {
      if((motoInfo[i].position -motoInfo[i].prePosition) > 4096)//down overflow
      {
        DeltaTick[i] = ((motoInfo[i].prePosition + 8192 - motoInfo[i].position)%8192);
      }
      else if((motoInfo[i].position -motoInfo[i].prePosition) < -4096)//up overflow
      {
        DeltaTick[i] = -((motoInfo[i].position + 8192 - motoInfo[i].prePosition)%8192);
      }
      else if(motoInfo[i].position > motoInfo[i].prePosition)
      {
        DeltaTick[i] = -(motoInfo[i].position - motoInfo[i].prePosition);
      }
      else
      {
        DeltaTick[i] = (motoInfo[i].prePosition - motoInfo[i].position);
      }
      motoInfo[i].prePosition = motoInfo[i].position;
      delta_pos[i] = DeltaTick[i]/8192.0f*2*PI*param.wheel_r;
    }
    
    angle = imu.yaw / RADIAN_COEF;
    
    delta_x_mm = (2*delta_pos[0] - delta_pos[1] - delta_pos[2])/3.0f;
    delta_y_mm = (delta_pos[1] - delta_pos[2])/2/SIN_60;
    
    pose.position_x_mm += delta_x_mm * cos(angle) - delta_y_mm * sin(angle);
    pose.position_y_mm += delta_x_mm * sin(angle) + delta_y_mm * cos(angle);
    
    delta_angle = (delta_pos[0]+delta_pos[1]+delta_pos[2])/3/param.base.omni.base_r*RADIAN_COEF;
    pose.angle_deg += delta_angle;
    
    pose.rate_deg = (motoInfo[0].w + motoInfo[1].w + motoInfo[2].w)/param.moto_reduction_ratio*2*PI*param.wheel_r/param.base.omni.base_r/3/60*RADIAN_COEF;
    pose.v_x_mm = (motoInfo[0].w*2 - motoInfo[1].w - motoInfo[2].w)/param.moto_reduction_ratio*2*PI*param.wheel_r/3/60;
    pose.v_y_mm = (motoInfo[1].w - motoInfo[2].w)/param.moto_reduction_ratio*2*PI*param.wheel_r/2/SIN_60/60;
    osDelay(param.pose_calc_period);
  }
}


void OmniTaskInit(void)
{
  osThreadDef(OmniCtrlTask, OmniCtrlTaskEntry, osPriorityAboveNormal, 0, 512);
  OmniCtrlTaskHandle = osThreadCreate(osThread(OmniCtrlTask), NULL);
  
  osThreadDef(OmniFeedbackTask, OmniFeedbackTaskEntry, osPriorityAboveNormal, 0, 512);
  OmniFeedbackTaskHandle = osThreadCreate(osThread(OmniFeedbackTask), NULL);
  
  osThreadDef(OmniPoseTask, OmniPoseTaskEntry, osPriorityAboveNormal, 0, 512);
  OmniPoseTaskHandle = osThreadCreate(osThread(OmniPoseTask), NULL);
}
