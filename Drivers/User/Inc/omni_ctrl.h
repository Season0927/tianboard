#ifndef _OMNI_CTRL_
#define _OMNI_CTRL_

#include "stm32f4xx_hal.h"

void OmniMotoPower(uint16_t val);
void SetOmniMoto(int16_t iq1, int16_t iq2, int16_t iq3);

#endif
