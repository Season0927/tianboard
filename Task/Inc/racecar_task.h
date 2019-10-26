#ifndef _RACECAR_TASK_H_
#define _RACECAR_TASK_H_

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

#define LIMIT(x, a, b) (((x)<(a)) ? (a) : (((x)>(b)) ? (b) : (x)))

#define SERVO_MAX ((2500 - 500)*120/180+500)
#define SERVO_MIN ((2500 - 500)*60/180+500)
#define SERVO_CAL(X) ((2500 - 500)*(X)/180+500)

#define MOTOR_MAX 1800
#define MOTOR_MIN 1200

#define RACECAR_CTRL_TIMEOUT  100

#define RACECAR_SPEED_ZERO 1500
#define RACECAR_STEER_ANGLE_ZERO 90

void RacecarTaskInit(void);
#endif
