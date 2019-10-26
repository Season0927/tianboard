#ifndef _MECANUM_CTRL_
#define _MECANUM_CTRL_

#include "stm32f4xx_hal.h"

void MecanumMotoPower(uint16_t val);
void SetMecanumMoto(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4);

#endif
