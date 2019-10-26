#ifndef _PROTOCOL_TASK_H_
#define _PROTOCOL_TASK_H_

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

#define HEAD_LEN       12
#define CMD_SIZE       2
#define CRC32_SIZE     4
#define PROCOTOL_HEAD  0xAA

#define PROTOCOL_PACK_ACK 1

#define CMD_PC_HEART                      (0x0001u)
#define CMD_CHASSIS_INFO                  (0x0201u)
#define CMD_SET_CHASSIS_SPD               (0x0203u)
#define CMD_SET_CHASSIS_SPD_ACC           (0x0205u)

#define PROTOCOL_MSG_LEN  128
#define PROTOCOL_MSG_QUENE_SIZE 4
#define PROTOCOL_TIMEOUT 500

#define UART7_TX_FINISH  0x01

#define RADIAN_COEF 57.29578f

#define CONNECTETED 0
#define DISCONNECTED 1

#pragma pack(1)

typedef struct{
  uint8_t sof; /*!< Identify Of A Package */
  union {
    struct
    {
      uint16_t data_len : 10; /*!< Data Length, Include Header And Crc */
      uint16_t version : 6;   /*!< Protocol Version */
    }VL_s;
    uint16_t ver_data_len;
  }VL_u;
  union {
    struct
    {
      uint8_t session : 5;   /*!< Need(0~1) Or Not Need(2~63) Ack */
      uint8_t pack_type : 1; /*!< Ack Package Or Normal Package */
      uint8_t res : 2;       /*!< Reserve */
    }SAR_s;
    uint8_t S_A_R_c;
  }SAR_u;
  uint8_t sender;   /*!< Sender Module Information */
  uint8_t reciver;  /*!< Receiver Module Information */
  uint16_t res1;    /*!< Reserve 1 */
  uint16_t seq_num; /*!< Sequence Number */
  uint16_t crc_16;  /*!< CRC16 */
  uint8_t pdata[];
} protocol_pack_desc_t;

typedef struct{
  uint16_t cmd;
  uint8_t pdata[];
}ProtocolCmd_t;

typedef struct cmd_chassis_speed
{
  int16_t vx; // forward/back
  int16_t vy; // left/right
  int16_t vw; // anticlockwise/clockwise
  int16_t rotate_x_offset;
  int16_t rotate_y_offset;
}CmdSpd_t;

typedef struct cmd_chassis_spd_acc
{
  int16_t   vx; 
  int16_t   vy;
  int16_t   vw; 

  int16_t   ax; 
  int16_t   ay; 
  int16_t   wz; 
  
  int16_t rotate_x_offset;
  int16_t rotate_y_offset;
}CmdSpdAcc_t;

struct cmd_chassis_info
{
  int16_t gyro_angle;
  int16_t gyro_palstance;
  int32_t position_x_mm;
  int32_t position_y_mm;
  int16_t angle_deg;
  int16_t v_x_mm;
  int16_t v_y_mm;
};

typedef struct{
	uint8_t Msg[PROTOCOL_MSG_LEN];
	uint16_t MsgLen;
}ProtocolMsg_t;

extern ProtocolMsg_t *pProtocolMsg;

extern osMailQId ProtocolRxMail;
extern osMailQId ProtocolTxMail;

extern osThreadId ProtocolSendTaskHandle;

void ProtocolTaskInit(void);
void ProtocolSend(struct cmd_chassis_info info);
#endif
