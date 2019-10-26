#include "param.h"
#include "stm32f4xx_hal_flash.h"
#include "string.h"

#define PARAM_SAVE_ADDR 0x080E0000

const Param_t DefaultParam = {
  PARAM_HEAD,
  38.1,//wheel_r
  36.0,//moto_reduction_ratio
  PI,//max_w 
  1500.0,//max_speed
  BASE_TYPE_MECANUM,//base_type
  {{130.0, 140.0},{10.0}},//{base_a, base_b} {base_r}
  {15, 0.2, 0.3, 16384.0, 16384.0},//p i d max_output i_limit
  2,//ctrl periodsss
  10,//feedback period
  2,//pose calc period
  "default",
  PARAM_TAIL
};

Param_t param;

void InitParam(void)
{
  int i;
  Param_t *p = (Param_t *)PARAM_SAVE_ADDR;

  if((p->param_head != PARAM_HEAD) || (p->param_tail != PARAM_TAIL))
  {
    HAL_FLASH_Unlock();
    FLASH_Erase_Sector(FLASH_SECTOR_11, FLASH_VOLTAGE_RANGE_3);
    for(i=0;i<sizeof(Param_t);i++)
    {
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, PARAM_SAVE_ADDR+i, *(((uint8_t *)&DefaultParam)+i));
    }
    HAL_FLASH_Lock();
  }
  
  memcpy(&param, (void *)PARAM_SAVE_ADDR, sizeof(Param_t));
}

void SaveParam(void *p)
{
  int i;
  HAL_FLASH_Unlock();
  FLASH_Erase_Sector(FLASH_SECTOR_11, FLASH_VOLTAGE_RANGE_3);
  for(i=0;i<sizeof(Param_t);i++)
  {
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, PARAM_SAVE_ADDR+i, *(((uint8_t *)p)+i));
  }
  HAL_FLASH_Lock();
}
