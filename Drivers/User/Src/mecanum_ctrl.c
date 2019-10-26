#include "mecanum_ctrl.h"
#include "can.h"
#include "gpio.h"

void MecanumMotoPower(uint16_t val)
{
  if(val)
  {
    HAL_GPIO_WritePin(POWER_EN1_GPIO_Port, POWER_EN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(POWER_EN2_GPIO_Port, POWER_EN2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(POWER_EN3_GPIO_Port, POWER_EN3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(POWER_EN4_GPIO_Port, POWER_EN4_Pin, GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(POWER_EN1_GPIO_Port, POWER_EN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(POWER_EN2_GPIO_Port, POWER_EN2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(POWER_EN3_GPIO_Port, POWER_EN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(POWER_EN4_GPIO_Port, POWER_EN4_Pin, GPIO_PIN_RESET);
  }
}

void SetMecanumMoto(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4)
{
  CAN_TxHeaderTypeDef   TxHeader;
  uint32_t TxMailBox;
  uint8_t Data[8];
  TxHeader.StdId = 0x200;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.DLC = 0x08;
  TxHeader.TransmitGlobalTime = DISABLE;
  Data[0] = (iq1 >> 8);
  Data[1] = iq1;
  Data[2] = (iq2 >> 8);
  Data[3] = iq2;
  Data[4] = iq3 >> 8;
  Data[5] = iq3;
  Data[6] = iq4 >> 8;
  Data[7] = iq4;
  HAL_CAN_AddTxMessage(&hcan1, &TxHeader, Data, &TxMailBox);
}
