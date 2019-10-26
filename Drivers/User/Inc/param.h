#ifndef _PARAM_H_
#define _PARAM_H_

#include "stm32f4xx_hal.h"

#pragma pack(1)

#define PARAM_HEAD 0xaa5555aa
#define PARAM_TAIL 0x55aaaa55

#define BASE_TYPE_MECANUM 0
#define BASE_TYPE_OMNI 1
#define BASE_TYPE_RACECAR 2

#define PI 3.1415926f

#define SIN_60 0.8660254f
#define COS_60 0.5f

//              ^^
//              ^^
//              ||
//      
//         m1 | -- | m2
//            |    |
//         m3 | -- | m4
//
//
//           \    /|
//            \  / |
//          A---   |B
//            /  \
//           /    \

typedef struct{
  float base_a;//width/2 left to center
  float base_b;//length/2 front to center
}MecanumBaseParam_t;

//            ^^
//            ^^
//            ||
//
//             m1
//             |
//            r|
//             /\
//            /  \
//           m3   m2

typedef struct{
  float base_r;
}OmniBaseParam_t;

typedef struct{
  float p;
  float i;
  float d;
  float max_out;
  float i_limit;
}PidParam_t;

typedef struct{
  uint32_t param_head;
  float wheel_r;
  float moto_reduction_ratio;
  float max_w;
  float max_speed;
  uint32_t base_type;
  struct{
    MecanumBaseParam_t mecanum;
    OmniBaseParam_t omni;
  }base;
  PidParam_t pid;
  int ctrl_period;
  int feedback_period;
  int pose_calc_period;
  char desc[32];
  uint32_t param_tail;
}Param_t;

extern Param_t param;
extern const Param_t DefaultParam;
void InitParam(void);
void SaveParam(void *p);
#endif
