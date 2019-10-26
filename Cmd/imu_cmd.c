#include "terminal_task.h"
#include "imu.h"

void imu_cmd(int argc, char* argv[])
{
  while(1)
  {
    tprintf("Pitch: %6.2f\tRoll: %6.2f\tYaw: %6.2f", imu.pit, imu.rol, imu.yaw);
    osDelay(100);
    tprintf("\r");
  }
}

ADD_CMD("imu", imu_cmd, "Display imu information.")
