#include "terminal_task.h"
#include "led.h"
#include "beep.h"
#include "stdlib.h"
#include "tim.h"
#include "gpio.h"
#include "can.h"
#include "misc.h"
#include "usart.h"
#include "ws2812.h"

void led_cmd(int argc, char* argv[])
{
	if(argc != 3)
  {
    tprintf("usage:\n");
    tprintf("led r/1/2/3/4/5/6/7/8 on/off\r\n");
  }
  else
  {
    if(strcmp(argv[1], "r") == 0)
    {
      if(strcmp(argv[2], "on") == 0)
      {
        LedROn();
      }
      else if(strcmp(argv[2], "off") == 0)
      {
        LedROff();
      }
      else
      {
        tprintf("usage:\n");
        tprintf("led r/1/2/3/4/5/6/7/8 on/off\r\n");
      }
    }
    else if(strcmp(argv[1], "1") == 0)
    {
      if(strcmp(argv[2], "on") == 0)
      {
        Led1On();
      }
      else if(strcmp(argv[2], "off") == 0)
      {
        Led1Off();
      }
      else
      {
        tprintf("usage:\n");
        tprintf("led r/1/2/3/4/5/6/7/8 on/off\r\n");
      }
    }
    else if(strcmp(argv[1], "2") == 0)
    {
      if(strcmp(argv[2], "on") == 0)
      {
        Led2On();
      }
      else if(strcmp(argv[2], "off") == 0)
      {
        Led2Off();
      }
      else
      {
        tprintf("usage:\n");
        tprintf("led r/1/2/3/4/5/6/7/8 on/off\r\n");
      }
    }
    else if(strcmp(argv[1], "3") == 0)
    {
      if(strcmp(argv[2], "on") == 0)
      {
        Led3On();
      }
      else if(strcmp(argv[2], "off") == 0)
      {
        Led3Off();
      }
      else
      {
        tprintf("usage:\n");
        tprintf("led r/1/2/3/4/5/6/7/8 on/off\r\n");
      }
    }
    else if(strcmp(argv[1], "4") == 0)
    {
      if(strcmp(argv[2], "on") == 0)
      {
        Led4On();
      }
      else if(strcmp(argv[2], "off") == 0)
      {
        Led4Off();
      }
      else
      {
        tprintf("usage:\n");
        tprintf("led r/1/2/3/4/5/6/7/8 on/off\r\n");
      }
    }
    else if(strcmp(argv[1], "5") == 0)
    {
      if(strcmp(argv[2], "on") == 0)
      {
        Led5On();
      }
      else if(strcmp(argv[2], "off") == 0)
      {
        Led5Off();
      }
      else
      {
        tprintf("usage:\n");
        tprintf("led r/1/2/3/4/5/6/7/8 on/off\r\n");
      }
    }
    else if(strcmp(argv[1], "6") == 0)
    {
      if(strcmp(argv[2], "on") == 0)
      {
        Led6On();
      }
      else if(strcmp(argv[2], "off") == 0)
      {
        Led6Off();
      }
      else
      {
        tprintf("usage:\n");
        tprintf("led r/1/2/3/4/5/6/7/8 on/off\r\n");
      }
    }
    else if(strcmp(argv[1], "7") == 0)
    {
      if(strcmp(argv[2], "on") == 0)
      {
        Led7On();
      }
      else if(strcmp(argv[2], "off") == 0)
      {
        Led7Off();
      }
      else
      {
        tprintf("usage:\n");
        tprintf("led r/1/2/3/4/5/6/7/8 on/off\r\n");
      }
    }
    else if(strcmp(argv[1], "8") == 0)
    {
      if(strcmp(argv[2], "on") == 0)
      {
        Led8On();
      }
      else if(strcmp(argv[2], "off") == 0)
      {
        Led8Off();
      }
      else
      {
        tprintf("usage:\n");
        tprintf("led r/1/2/3/4/5/6/7/8 on/off\r\n");
      }
    }
    else
    {
      tprintf("usage:\n");
      tprintf("led r/1/2/3/4/5/6/7/8 on/off\r\n");
    }
  }
}

void beep_cmd(int argc, char* argv[])
{
  if(argc != 3)
  {
    tprintf("usage:\n");
    tprintf("beep sound time\r\n");
  }
  else
  {
    BeepOn(atoi(argv[1]));
    osDelay(atoi(argv[2]));
    BeepOff();
  }
}

void pwm_cmd(int argc, char* argv[])
{
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
  
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);

  if(argc != 3)
  {
    tprintf("usage:\n");
    tprintf("pwm 1/2/3/4/5/6/7 us(0~20000)\r\n");
  }
  else
  {
    switch(atoi(argv[1]))
    {
      case 1:
        TIM4->CCR1 = atoi(argv[2]);
        break;
      
      case 2:
        TIM4->CCR2 = atoi(argv[2]);
        break;
      
      case 3:
        TIM4->CCR3 = atoi(argv[2]);
        break;
      
      case 4:
        TIM4->CCR4 = atoi(argv[2]);
        break;
      
      case 5:
        TIM8->CCR1 = atoi(argv[2]);
        break;
      
      case 6:
        TIM8->CCR2 = atoi(argv[2]);
        break;
      
      case 7:
        TIM8->CCR3 = atoi(argv[2]);
        break;
    }
  }
}

void key_cmd(int argc, char* argv[])
{
  while(1)
  {
    if(HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == GPIO_PIN_SET)
    {
      tprintf("KEY0 PRESS\r\n");
      osDelay(500);
    }
    if(HAL_GPIO_ReadPin(HUB_KEY1_GPIO_Port, HUB_KEY1_Pin) == GPIO_PIN_RESET)
    {
      tprintf("HUB_KEY1 PRESS\r\n");
      osDelay(500);
    }
    if(HAL_GPIO_ReadPin(HUB_KEY2_GPIO_Port, HUB_KEY2_Pin) == GPIO_PIN_RESET)
    {
      tprintf("HUB_KEY2 PRESS\r\n");
      osDelay(500);
    }
    if(HAL_GPIO_ReadPin(HUB_KEY3_GPIO_Port, HUB_KEY3_Pin) == GPIO_PIN_RESET)
    {
      tprintf("HUB_KEY3 PRESS\r\n");
      osDelay(500);
    }
  }
}

void uart3_cmd(int argc, char* argv[])
{
  HAL_UART_DeInit(&huart3);
  HAL_UART_Init(&huart3);
  if(argc == 2)
  {
    if(strcmp(argv[1], "recv")!=0)
    {
      tprintf("usage:\n");
      tprintf("uart3 send/recv [data]\r\n");
      return;
    }
    else
    {
      while(1)
      {
        uint8_t buff;
        HAL_StatusTypeDef result;
        result = HAL_UART_Receive(&huart3, &buff, 1, 1);
        if(result == HAL_OK)
        {
          tprintf("%c", buff);
        }
      }
    }
  }
  if(argc == 3)
  {
    if(strcmp(argv[1], "send")!=0)
    {
      tprintf("usage:\n");
      tprintf("uart3 send/recv [data]\r\n");
      return;
    }
    else
    {
      HAL_UART_Transmit(&huart3, (uint8_t *)argv[2], strlen(argv[2]), strlen(argv[2])/10+1);
      return;
    }
  }
  
  tprintf("usage:\n");
  tprintf("uart3 send/recv [data]\r\n");
}

void uart6_cmd(int argc, char* argv[])
{
  HAL_UART_DeInit(&huart6);
  HAL_UART_Init(&huart6);
  if(argc == 2)
  {
    if(strcmp(argv[1], "recv")!=0)
    {
      tprintf("usage:\n");
      tprintf("uart6 send/recv [data]\r\n");
      return;
    }
    else
    {
      while(1)
      {
        uint8_t buff;
        HAL_StatusTypeDef result;
        result = HAL_UART_Receive(&huart6, &buff, 1, 1);
        if(result == HAL_OK)
        {
          tprintf("%c", buff);
        }
      }
    }
  }
  if(argc == 3)
  {
    if(strcmp(argv[1], "send")!=0)
    {
      tprintf("usage:\n");
      tprintf("uart6 send/recv [data]\r\n");
      return;
    }
    else
    {
      HAL_UART_Transmit(&huart6, (uint8_t *)argv[2], strlen(argv[2]), strlen(argv[2])/10+1);
      return;
    }
  }
  
  tprintf("usage:\n");
  tprintf("uart6 send/recv [data]\r\n");
}

void uart7_cmd(int argc, char* argv[])
{
  HAL_UART_DeInit(&huart7);
  HAL_UART_Init(&huart7);
  if(argc == 2)
  {
    if(strcmp(argv[1], "recv")!=0)
    {
      tprintf("usage:\n");
      tprintf("uart7 send/recv [data]\r\n");
      return;
    }
    else
    {
      while(1)
      {
        uint8_t buff;
        HAL_StatusTypeDef result;
        result = HAL_UART_Receive(&huart7, &buff, 1, 1);
        if(result == HAL_OK)
        {
          tprintf("%c", buff);
        }
      }
    }
  }
  if(argc == 3)
  {
    if(strcmp(argv[1], "send")!=0)
    {
      tprintf("usage:\n");
      tprintf("uart7 send/recv [data]\r\n");
      return;
    }
    else
    {
      HAL_UART_Transmit(&huart7, (uint8_t *)argv[2], strlen(argv[2]), strlen(argv[2])/10+1);
      return;
    }
  }
  
  tprintf("usage:\n");
  tprintf("uart7 send/recv [data]\r\n");
}

void uart8_cmd(int argc, char* argv[])
{
  HAL_UART_DeInit(&huart8);
  HAL_UART_Init(&huart8);
  if(argc == 2)
  {
    if(strcmp(argv[1], "recv")!=0)
    {
      tprintf("usage:\n");
      tprintf("uart8 send/recv [data]\r\n");
      return;
    }
    else
    {
      while(1)
      {
        uint8_t buff;
        HAL_StatusTypeDef result;
        result = HAL_UART_Receive(&huart8, &buff, 1, 1);
        if(result == HAL_OK)
        {
          tprintf("%c", buff);
        }
      }
    }
  }
  if(argc == 3)
  {
    if(strcmp(argv[1], "send")!=0)
    {
      tprintf("usage:\n");
      tprintf("uart8 send/recv [data]\r\n");
      return;
    }
    else
    {
      HAL_UART_Transmit(&huart8, (uint8_t *)argv[2], strlen(argv[2]), strlen(argv[2])/10+1);
      return;
    }
  }
  
  tprintf("usage:\n");
  tprintf("uart8 send/recv [data]\r\n");
}

void can2_cmd(int argc, char* argv[])
{
  HAL_CAN_Stop(&hcan2);
  CanParamInit(&hcan2);
  if(argc == 2)
  {
    if(strcmp(argv[1], "recv")!=0)
    {
      tprintf("usage:\n");
      tprintf("can2 send/recv [data0/data1/data2...]\r\n");
      return;
    }
    else
    {
      while(1)
      {
        if(can2Flag == 1)
        {
          int i;
          can2Flag = 0;

          tprintf("id: 0x%x, length: %d, data: ", Can2RxHeader.StdId, Can2RxHeader.DLC);
          for(i=0;i<Can2RxHeader.DLC;i++)
          {
            tprintf("%02x", Can2RxData[i]);
            osDelay(1);
          }
          tprintf("\r\n");
          osDelay(1000);
        }
      }
    }
  }
  if(argc == 4)
  {
    if(strcmp(argv[1], "send")!=0)
    {
      tprintf("usage:\n");
      tprintf("can2 send/recv [id data]\r\n");
      return;
    }
    else
    {
      Can2Send(atoi(argv[2]), atoi(argv[3]));
      return;
    }
  }
  
  tprintf("usage:\n");
  tprintf("can2 send/recv [id data]\r\n");
}

void ws2812_cmd(int argc, char* argv[])
{
	Color_t color;
  if(argc == 4)
  {
		color.r = atoi(argv[1]);
		color.g = atoi(argv[2]);
		color.b = atoi(argv[3]);
    Ws2812Set(&color, 1);
  }
  else
	{
		tprintf("usage:\n");
		tprintf("ws2812 color_r color_g color_b\r\n");
	}
}

ADD_CMD("led", led_cmd, "Test led on/off.")
ADD_CMD("beep", beep_cmd, "Test beep on/off.")
ADD_CMD("pwm", pwm_cmd, "Test pwm output.")
ADD_CMD("key", key_cmd, "Test key state.")
ADD_CMD("uart3", uart3_cmd, "Test uart3 send/recv.")
ADD_CMD("uart6", uart6_cmd, "Test uart6 send/recv.")
ADD_CMD("uart7", uart7_cmd, "Test uart7 send/recv.")
ADD_CMD("uart8", uart8_cmd, "Test uart8 send/recv.")
ADD_CMD("can2", can2_cmd, "Test can2 send/recv.")
ADD_CMD("ws2812", ws2812_cmd, "Test ws2812.")
