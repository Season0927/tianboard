#include "led.h"
#include "gpio.h"

inline void LedROn(void)
{
  HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);
}

inline void LedROff(void)
{
  HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET);
}

inline void LedRToggle(void)
{
  HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
}

inline void LedGOn(void)
{
  HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET);
}

inline void LedGOff(void)
{
  HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
}

inline void LedGToggle(void)
{
  HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
}

inline void Led1On(void)
{
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
}

inline void Led1Off(void)
{
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
}

inline void Led1Toggle(void)
{
  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
}

inline void Led2On(void)
{
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
}

inline void Led2Off(void)
{
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
}

inline void Led2Toggle(void)
{
  HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
}

inline void Led3On(void)
{
  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
}

inline void Led3Off(void)
{
  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
}

inline void Led3Toggle(void)
{
  HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
}

inline void Led4On(void)
{
  HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);
}

inline void Led4Off(void)
{
  HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);
}

inline void Led4Toggle(void)
{
  HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
}

inline void Led5On(void)
{
  HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, GPIO_PIN_RESET);
}

inline void Led5Off(void)
{
  HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, GPIO_PIN_SET);
}

inline void Led5Toggle(void)
{
  HAL_GPIO_TogglePin(LED5_GPIO_Port, LED5_Pin);
}

inline void Led6On(void)
{
  HAL_GPIO_WritePin(LED6_GPIO_Port, LED6_Pin, GPIO_PIN_RESET);
}

inline void Led6Off(void)
{
  HAL_GPIO_WritePin(LED6_GPIO_Port, LED6_Pin, GPIO_PIN_SET);
}

inline void Led6Toggle(void)
{
  HAL_GPIO_TogglePin(LED6_GPIO_Port, LED6_Pin);
}

inline void Led7On(void)
{
  HAL_GPIO_WritePin(LED7_GPIO_Port, LED7_Pin, GPIO_PIN_RESET);
}

inline void Led7Off(void)
{
  HAL_GPIO_WritePin(LED7_GPIO_Port, LED7_Pin, GPIO_PIN_SET);
}

inline void Led7Toggle(void)
{
  HAL_GPIO_TogglePin(LED7_GPIO_Port, LED7_Pin);
}

inline void Led8On(void)
{
  HAL_GPIO_WritePin(LED8_GPIO_Port, LED8_Pin, GPIO_PIN_RESET);
}

inline void Led8Off(void)
{
  HAL_GPIO_WritePin(LED8_GPIO_Port, LED8_Pin, GPIO_PIN_SET);
}

inline void Led8Toggle(void)
{
  HAL_GPIO_TogglePin(LED8_GPIO_Port, LED8_Pin);
}
