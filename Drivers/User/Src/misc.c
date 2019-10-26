#include "misc.h"
#include "can.h"

MotoInfo_t motoInfo[4];

int can2Flag = 0;
CAN_RxHeaderTypeDef Can2RxHeader;
uint8_t Can2RxData[8];

void CanParamInit(CAN_HandleTypeDef *hcan)
{
  CAN_FilterTypeDef CAN_FilterConfigStructure;

	CAN_FilterConfigStructure.FilterMode = CAN_FILTERMODE_IDMASK;
	CAN_FilterConfigStructure.FilterScale = CAN_FILTERSCALE_32BIT;
	CAN_FilterConfigStructure.FilterIdHigh = 0x0000;
	CAN_FilterConfigStructure.FilterIdLow = 0x0000;
	CAN_FilterConfigStructure.FilterMaskIdHigh = 0x0000;
	CAN_FilterConfigStructure.FilterMaskIdLow = 0x0000;
	CAN_FilterConfigStructure.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  CAN_FilterConfigStructure.SlaveStartFilterBank = 14;

  if(hcan->Instance == CAN1)
  {
    CAN_FilterConfigStructure.FilterBank = 0;//can1(0-13)?can2(14-27)???????filter
  }
  else
  {
    CAN_FilterConfigStructure.FilterBank = 14;//can1(0-13)?can2(14-27)???????filter
  }
	CAN_FilterConfigStructure.FilterActivation = ENABLE;

	if(HAL_CAN_ConfigFilter(hcan, &CAN_FilterConfigStructure) != HAL_OK)
	{
		while(1);
	}
  
  if(HAL_CAN_Start(hcan) != HAL_OK)
  {
    while(1);
  }
  
  if(HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    while(1);
  }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  if(hcan->Instance == CAN1)
  {
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];
    
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
    {
      /* Reception Error */
      Error_Handler();
    }
    if (((RxHeader.StdId>0x200) && (RxHeader.StdId<0x205)) && (RxHeader.IDE == CAN_ID_STD) && (RxHeader.DLC == 8))
    {
      motoInfo[RxHeader.StdId-0x200-1].position = (RxData[0]<<8) | RxData[1];
      motoInfo[RxHeader.StdId-0x200-1].w = ((RxData[2]<<8) | RxData[3]);
      motoInfo[RxHeader.StdId-0x200-1].current = ((RxData[4]<<8) | RxData[5]);
      motoInfo[RxHeader.StdId-0x200-1].temperature = RxData[6];
    }
  }
  else
  {
    can2Flag = 1;
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &Can2RxHeader, Can2RxData) != HAL_OK)
    {
      /* Reception Error */
      Error_Handler();
    }
  }
}

void Can2Send(uint16_t id, int16_t data)
{
  CAN_TxHeaderTypeDef   TxHeader;
  uint32_t TxMailBox;
  uint8_t Data[8];
  TxHeader.StdId = id;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.DLC = 0x08;
  TxHeader.TransmitGlobalTime = DISABLE;
  Data[0] = (data >> 8);
  Data[1] = data;
  Data[2] = 0;
  Data[3] = 0;
  Data[4] = 0;
  Data[5] = 0;
  Data[6] = 0;
  Data[7] = 0;
  HAL_CAN_AddTxMessage(&hcan2, &TxHeader, Data, &TxMailBox);
}
