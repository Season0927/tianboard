#include "beep_task.h"
#include "cmsis_os.h"
#include "tim.h"
#include "beep.h"
#include "stdarg.h"

osThreadId BeepTaskHandle;
osMailQId BeepMail;

void Beep(uint8_t sound, uint8_t time)
{
  BeepMsg_t *p;
  p = osMailAlloc(BeepMail, 0);
  if(p != NULL)
  {
    p->sound = sound;
    p->time = time;
    osMailPut(BeepMail, p);
  }
}

static void BeepTaskEntry(void const * argument)
{
  osEvent evt;
  BeepMsg_t *p;
  /* Infinite loop */
  BeepOff();
  for(;;)
  {
    evt = osMailGet(BeepMail, osWaitForever);
    if (evt.status == osEventMail) 
    {       
      p = evt.value.p;
      BeepOn(p->sound);
      osDelay(p->time);
      osMailFree(BeepMail, p);
      BeepOff();
    }
  }
}

void BeepTaskInit(void)
{
  osMailQDef(BeepMail, BEEP_MSG_QUENE_SIZE, BeepMsg_t);
  BeepMail = osMailCreate(osMailQ(BeepMail), NULL);
  
  osThreadDef(BeepTask, BeepTaskEntry, osPriorityAboveNormal, 0, 128);
  BeepTaskHandle = osThreadCreate(osThread(BeepTask), NULL);
}
