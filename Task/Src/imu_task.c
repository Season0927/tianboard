#include "imu_task.h"
#include "imu.h"
#include "cmsis_os.h"
#include "beep_task.h"

#define IMU_DATA_READY 0x01

osThreadId ImuTaskHandle;

void ImuTaskEntry(void const *argument)
{
  /* USER CODE BEGIN ImuTaskEntry */
  HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);

  mpu_device_init();
  Beep(6, 100);
  osDelay(400);
  Beep(6, 100);
  init_quaternion();

  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  /* Infinite loop */
  for (;;)
  {
    osSignalWait(IMU_DATA_READY, osWaitForever);
    mpu_get_data();
    imu_ahrs_update();
    imu_attitude_update();
  }
  /* USER CODE END ImuTaskEntry */
}
osThreadDef(ImuTask, ImuTaskEntry, osPriorityAboveNormal, 0, 1024);
void ImuTaskInit(void)
{
  ImuTaskHandle = osThreadCreate(osThread(ImuTask), NULL);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (ImuTaskHandle != NULL)
  {
    osSignalSet(ImuTaskHandle, IMU_DATA_READY);
  }
}
