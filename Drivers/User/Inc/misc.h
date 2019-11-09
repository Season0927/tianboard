#ifndef _MISC_H_
#define _MISC_H_

#include "stm32f4xx_hal.h"

typedef struct
{
  uint16_t position;
  int16_t w;
  int16_t current;
  int8_t temperature;
  int16_t prePosition;
} __PACKED MotoInfo_t;

extern MotoInfo_t motoInfo[4];
extern int can2Flag;
extern CAN_RxHeaderTypeDef Can2RxHeader;
extern uint8_t Can2RxData[8];

void CanParamInit(CAN_HandleTypeDef *hcan);
void Can2Send(uint16_t id, int16_t data);
#endif
