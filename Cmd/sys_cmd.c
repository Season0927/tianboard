#include "terminal_task.h"
#include "version.h"

void example_cmd(int argc, char* argv[])
{
	tprintf("hello world!\r\n");
}

void help_cmd(int argc, char* argv[])
{
  uint32_t i;
  Cmd_t *pCmd = (Cmd_t *)&CMD_SECTOR_Base;
  uint32_t length = (uint32_t)(&CMD_SECTOR_Limit - &CMD_SECTOR_Base);
	if(argc == 1)
	{
    for(i = 0; i<length/sizeof(Cmd_t); i++)
    {
      tprintf("%s\t\t%s\r\n", pCmd[i].CmdName, pCmd[i].Desc);
      osDelay((strlen(pCmd[i].CmdName)+strlen(pCmd[i].Desc))/10);
    }
	}
	else if(argc == 2)
	{
    for(i = 0; i<length/sizeof(Cmd_t); i++)
    {
      if(strcmp(argv[1], pCmd[i].CmdName) == 0)
      {
        tprintf("%s\t\t%s\r\n", pCmd[i].CmdName, pCmd[i].Desc);
        break;
      }
    }
    if(i == length/sizeof(Cmd_t))
    {
      tprintf(YELLOW"command \""HIGHLIGHT"%s"CLEAR YELLOW"\" not found.\r\n"CLEAR, argv[1]);
    }
	}
  else
  {
    tprintf("usage:\n");
    tprintf("help [command]\r\n");
  }
}

void reset_cmd(int argc, char* argv[])
{
  HAL_NVIC_SystemReset();
}

void version_cmd(int argc, char* argv[])
{
  tprintf("Tianboard firmware version %s, build time %s %s.\n", __VERSION__, __DATE__, __TIME__);
}

ADD_CMD("example", example_cmd, "Example for terminal command.")
ADD_CMD("help", help_cmd, "List command and description.")
ADD_CMD("reset", reset_cmd, "Reset the system by software.")
ADD_CMD("version", version_cmd, "Get firmware version.")
