#include "terminal_task.h"
#include "param.h"
#include "stdlib.h"

static void SetParam(int argc, char* argv[])
{
  if(argc != 3)
  {
    tprintf("usage:\n");
    tprintf("param get/set/save/reset [name] [value]\r\n");
    return;
  }
  
  if(strcmp("desc", argv[1]) == 0)
  {
    memset(param.desc, 0, sizeof(param.desc));
    strncpy(param.desc, argv[2], sizeof(param.desc));
  }
  else if(strcmp("base_type", argv[1]) == 0)
  {
    if(strcmp("omni", argv[2]) == 0)
    {
      param.base_type = BASE_TYPE_OMNI;
    }
    else if(strcmp("mecanum", argv[2]) == 0)
    {
      param.base_type = BASE_TYPE_MECANUM;
    }
    else if(strcmp("mecanum", argv[2]) == 0)
    {
      param.base_type = BASE_TYPE_RACECAR;
    }
    else
    {
      tprintf("support base type: "YELLOW"mecanum/omni"CLEAR"\r\n");
    }
  }
  else if(strcmp("base_a", argv[1]) == 0)
  {
    param.base.mecanum.base_a = atof(argv[2]);
  }
  else if(strcmp("base_b", argv[1]) == 0)
  {
    param.base.mecanum.base_b = atof(argv[2]);
  }
  else if(strcmp("base_r", argv[1]) == 0)
  {
    param.base.omni.base_r = atof(argv[2]);
  }
  else if(strcmp("wheel_r", argv[1]) == 0)
  {
    param.wheel_r = atof(argv[2]);
  }
  else if(strcmp("moto_reduction", argv[1]) == 0)
  {
    param.moto_reduction_ratio = atof(argv[2]);
  }
  else if(strcmp("max_w", argv[1]) == 0)
  {
    param.max_w = atof(argv[2]);
  }
  else if(strcmp("max_speed", argv[1]) == 0)
  {
    param.max_speed = atof(argv[2]);
  }
  else if(strcmp("pid_p", argv[1]) == 0)
  {
    param.pid.p = atof(argv[2]);
  }
  else if(strcmp("pid_i", argv[1]) == 0)
  {
    param.pid.i = atof(argv[2]);
  }
  else if(strcmp("pid_d", argv[1]) == 0)
  {
    param.pid.d = atof(argv[2]);
  }
  else if(strcmp("pid_max_out", argv[1]) == 0)
  {
    param.pid.max_out = atof(argv[2]);
  }
  else if(strcmp("pid_i_limit", argv[1]) == 0)
  {
    param.pid.i_limit = atof(argv[2]);
  }
  else if(strcmp("ctrl_period", argv[1]) == 0)
  {
    param.ctrl_period = atoi(argv[2]);
  }
  else if(strcmp("feedback_period", argv[1]) == 0)
  {
    param.feedback_period = atoi(argv[2]);
  }
  else if(strcmp("pose_calc_period", argv[1]) == 0)
  {
    param.pose_calc_period = atoi(argv[2]);
  }
  else
  {
    tprintf("no param "YELLOW"%s"CLEAR"found\r\n", argv[1]);
  }
}

static void GetParam(int argc, char* argv[])
{
  tprintf(YELLOW"desc:"CLEAR" %s\r\n", param.desc);
  
  if(param.base_type == BASE_TYPE_MECANUM)
  {
    tprintf(YELLOW"base_type:"CLEAR" mecanum\r\n");
    tprintf(YELLOW"base_a:"CLEAR" %f\r\n", param.base.mecanum.base_a);
    tprintf(YELLOW"base_b:"CLEAR" %f\r\n", param.base.mecanum.base_b);
  }
  else if(param.base_type == BASE_TYPE_OMNI)
  {
    tprintf(YELLOW"base_type:"CLEAR" omni\r\n");
    tprintf(YELLOW"base_r:"CLEAR" %f\r\n", param.base.omni.base_r);
  }
  else
  {
    tprintf(YELLOW"base_type:"CLEAR" unknown\r\n");
  }
  
  tprintf(YELLOW"wheel_r:"CLEAR" %f\r\n", param.wheel_r);
  tprintf(YELLOW"moto_reduction:"CLEAR" %f\r\n", param.moto_reduction_ratio);
  tprintf(YELLOW"max_w:"CLEAR" %f\r\n", param.max_w);
  tprintf(YELLOW"max_speed:"CLEAR" %f\r\n", param.max_speed);
  tprintf(YELLOW"pid_p:"CLEAR" %f\r\n", param.pid.p);
  tprintf(YELLOW"pid_i:"CLEAR" %f\r\n", param.pid.i);
  tprintf(YELLOW"pid_d:"CLEAR" %f\r\n", param.pid.d);
  tprintf(YELLOW"pid_max_out:"CLEAR" %f\r\n", param.pid.max_out);
  tprintf(YELLOW"pid_i_limit:"CLEAR" %f\r\n", param.pid.i_limit);
  tprintf(YELLOW"ctrl_period:"CLEAR" %d\r\n", param.ctrl_period);
  tprintf(YELLOW"feedback_period:"CLEAR" %d\r\n", param.feedback_period);
  tprintf(YELLOW"pose_calc_period:"CLEAR" %d\r\n", param.pose_calc_period);
}

void param_cmd(int argc, char* argv[])
{
  if(argc == 2)
  {
    if(strcmp("save", argv[1]) == 0)
    {
      SaveParam(&param);
    }
    else if(strcmp("get", argv[1]) == 0)
    {
      GetParam(argc-1, &argv[1]);
    }
    else if(strcmp("reset", argv[1]) == 0)
    {
      SaveParam((void *)&DefaultParam);
      InitParam();
    }
    else 
    {
      tprintf("usage:\n");
      tprintf("param get/set/save/reset [name] [value]\r\n");
    }
  }
  else if(argc == 4)
  {
    if(strcmp("set", argv[1]) == 0)
    {
      SetParam(argc-1, &argv[1]);
    }
    else
    {
      tprintf("usage:\n");
      tprintf("param get/set/save/reset [name] [value]\r\n");
    }
  }
  else
  {
    tprintf("usage:\n");
    tprintf("param get/set/save/reset [name] [value]\r\n");
  }
}

ADD_CMD("param", param_cmd, "Print or modify base param.")
