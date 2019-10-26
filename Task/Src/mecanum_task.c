#include "mecanum_task.h"
#include "cmsis_os.h"
#include "dbus.h"
#include "misc.h"
#include "can.h"
#include "dbus_task.h"
#include "mecanum_ctrl.h"
#include "pid.h"
#include "param.h"
#include "stdlib.h"
#include "protocol_task.h"
#include "string.h"
#include "imu.h"
#include "math.h"

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

osThreadId MecanumCtrlTaskHandle;
osThreadId MecanumFeedbackTaskHandle;
osThreadId MecanumPoseTaskHandle;

static void MecanumCtrlTaskEntry(void const * argument)
{
  osEvent evt;
  MotionCtrl_t *p;
  Pid_t wheelPid[4];
  float w,vx,vy;
  int timeout = 0;
  int16_t motoCurrent[4] = {0,0,0,0};
  int16_t v[4] = {0,0,0,0};
  
  osDelay(1000);
  
  MecanumMotoPower(1);
  CanParamInit(&hcan1);
  
  PidInit(&wheelPid[0], POSITION_PID, param.pid.max_out, param.pid.i_limit, param.pid.p, param.pid.i, param.pid.d);
  PidInit(&wheelPid[1], POSITION_PID, param.pid.max_out, param.pid.i_limit, param.pid.p, param.pid.i, param.pid.d);
  PidInit(&wheelPid[2], POSITION_PID, param.pid.max_out, param.pid.i_limit, param.pid.p, param.pid.i, param.pid.d);
  PidInit(&wheelPid[3], POSITION_PID, param.pid.max_out, param.pid.i_limit, param.pid.p, param.pid.i, param.pid.d);
  
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

      v[0] = (vx+vy - w*(param.base.mecanum.base_a+param.base.mecanum.base_b))*param.moto_reduction_ratio/(2*PI*param.wheel_r)*60.0f;
      v[1] = (-(vx-vy + w*(param.base.mecanum.base_a+param.base.mecanum.base_b)))*param.moto_reduction_ratio/(2*PI*param.wheel_r)*60.0f;
      v[2] = (vx-vy - w*(param.base.mecanum.base_a+param.base.mecanum.base_b))*param.moto_reduction_ratio/(2*PI*param.wheel_r)*60.0f;
      v[3] = (-(vx+vy + w*(param.base.mecanum.base_a+param.base.mecanum.base_b)))*param.moto_reduction_ratio/(2*PI*param.wheel_r)*60.0f;
      
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
        v[3] = 0;
        timeout = 0;
      }
    }
    motoCurrent[0] = PidCalc(&wheelPid[0], motoInfo[0].w, v[0]);
    motoCurrent[1] = PidCalc(&wheelPid[1], motoInfo[1].w, v[1]);
    motoCurrent[2] = PidCalc(&wheelPid[2], motoInfo[2].w, v[2]);
    motoCurrent[3] = PidCalc(&wheelPid[3], motoInfo[3].w, v[3]);
    SetMecanumMoto(motoCurrent[0], motoCurrent[1], motoCurrent[2], motoCurrent[3]);
  }
}

static void MecanumFeedbackTaskEntry(void const * argument)
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

static void MecanumPoseTaskEntry(void const * argument)
{
  int i;
  int DeltaTick[4];
  float delta_angle = 0.0;
  float delta_pos[4];
  float delta_x_mm, delta_y_mm;
  float angle;
  memset(&pose, 0, sizeof(pose));
  for(;;)
  {
    for(i=0; i<4; i++)
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
      delta_pos[i] = DeltaTick[i]/8192.0f*2*PI*param.wheel_r/param.moto_reduction_ratio;
    }
    
    delta_pos[0] = -delta_pos[0];
    delta_pos[2] = -delta_pos[2];
    
    angle = imu.yaw / RADIAN_COEF;
    
    delta_x_mm = (delta_pos[0] + delta_pos[1] + delta_pos[2] + delta_pos[3])/4.0f;
    delta_y_mm = (-delta_pos[0] + delta_pos[1] + delta_pos[2] - delta_pos[3])/4.0f;
    
    pose.position_x_mm += delta_x_mm * cos(angle) - delta_y_mm * sin(angle);
    pose.position_y_mm += delta_x_mm * sin(angle) + delta_y_mm * cos(angle);
    
    delta_angle = (-delta_pos[0]+delta_pos[1]-delta_pos[2]+delta_pos[3])/4/(param.base.mecanum.base_a+param.base.mecanum.base_b)*RADIAN_COEF;
    pose.angle_deg += delta_angle;
    
    pose.angle_deg = ((int)pose.angle_deg%360) + pose.angle_deg - (int)pose.angle_deg;
    
    pose.rate_deg = (-motoInfo[0].w - motoInfo[1].w - motoInfo[2].w - motoInfo[3].w)/param.moto_reduction_ratio*2*PI*param.wheel_r/(param.base.mecanum.base_a+param.base.mecanum.base_b)/4/60*RADIAN_COEF;
    pose.v_x_mm = (motoInfo[0].w - motoInfo[1].w + motoInfo[2].w - motoInfo[3].w)/param.moto_reduction_ratio*2*PI*param.wheel_r/4/60;
    pose.v_y_mm = (-motoInfo[0].w - motoInfo[1].w + motoInfo[2].w + motoInfo[3].w)/param.moto_reduction_ratio*2*PI*param.wheel_r/4/60;
    
    osDelay(param.pose_calc_period);
  }
}

void MecanumTaskInit(void)
{
  osThreadDef(MecanumCtrlTask, MecanumCtrlTaskEntry, osPriorityAboveNormal, 0, 512);
  MecanumCtrlTaskHandle = osThreadCreate(osThread(MecanumCtrlTask), NULL);
  
  osThreadDef(MecanumFeedbackTask, MecanumFeedbackTaskEntry, osPriorityAboveNormal, 0, 512);
  MecanumFeedbackTaskHandle = osThreadCreate(osThread(MecanumFeedbackTask), NULL);
  
  osThreadDef(MecanumPoseTask, MecanumPoseTaskEntry, osPriorityAboveNormal, 0, 512);
  MecanumPoseTaskHandle = osThreadCreate(osThread(MecanumPoseTask), NULL);
}
