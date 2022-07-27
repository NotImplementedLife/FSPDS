#include "bottom_screen_power.h"
#include <nds.h>

int is_bottom_screen_on = 1;

void bottom_screen_power_off()
{
	fifoSendValue32(FIFO_USER_01,0x0001);
	is_bottom_screen_on = 0;
}

void bottom_screen_power_on()
{
	fifoSendValue32(FIFO_USER_01,0x0002);
	is_bottom_screen_on = 1;
}