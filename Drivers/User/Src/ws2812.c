#include "ws2812.h"
#include "tim.h"

static volatile int bit_count;
static volatile int count;
static volatile uint8_t Ws2812Data[MAX_2812_COUNT*24];
int irq_count;
void Ws2812Set(Color_t *p, int len)
{
  bit_count = 0;
  count = len * 24;
  if(len > MAX_2812_COUNT)
  {
    return;
  }
  for(bit_count = 0; bit_count<len*24; bit_count++)
  {
    if(((uint8_t *)p)[bit_count/8] & (1<<(7-bit_count%8)))
    {
      Ws2812Data[bit_count] = 14;
    }
    else
    {
      Ws2812Data[bit_count] = 7;
    }
  }
  irq_count = 0;
  bit_count = 0;
  //TIM9->CCR2 = Ws2812Data[bit_count ++];
  HAL_TIM_PWM_Start_IT(&htim9, TIM_CHANNEL_2);
}


void TIM1_BRK_TIM9_IRQHandler(void)
{
  if (__HAL_TIM_GET_FLAG(&htim9, TIM_FLAG_CC2) != RESET)
  {
    __HAL_TIM_CLEAR_IT(&htim9, TIM_IT_CC2);
    TIM9->CCR2 = Ws2812Data[bit_count ++];
    if(bit_count > count)
    {
      TIM9->CCR2 = 0;
      __HAL_TIM_DISABLE(&htim9);
      HAL_TIM_PWM_Stop_IT(&htim9, TIM_CHANNEL_2);
    }
  }
}
