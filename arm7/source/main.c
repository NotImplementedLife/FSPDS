/*---------------------------------------------------------------------------------

	default ARM7 core

		Copyright (C) 2005 - 2010
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/
/// Removed wifi
#include <nds.h>
#include "bottom_screen_power.h"

void VblankHandler(void) { }

void VcountHandler()
{
	inputGetAndSend();
}

volatile bool exitflag = false;

void powerButtonCB()
{
	exitflag = true;
}

int main()
{
	// clear sound registers
	dmaFillWords(0, (void*)0x04000400, 0x100);

	REG_SOUNDCNT |= SOUND_ENABLE;
	writePowerManagement(PM_CONTROL_REG, ( readPowerManagement(PM_CONTROL_REG) & ~PM_SOUND_MUTE ) | PM_SOUND_AMP );
	powerOn(POWER_SOUND);

	readUserSettings();
	ledBlink(0);

	irqInit();
	initClockIRQ();
	fifoInit();
	touchInit();

	SetYtrigger(80);

	installSoundFIFO();
	installSystemFIFO();

	irqSet(IRQ_VCOUNT, VcountHandler);
	irqSet(IRQ_VBLANK, VblankHandler);

	irqEnable( IRQ_VBLANK | IRQ_VCOUNT);

	setPowerButtonCB(powerButtonCB);

	while (!exitflag) {
		if ( 0 == (REG_KEYINPUT & (KEY_SELECT | KEY_START | KEY_L | KEY_R)))
        {
			exitflag = true;
		}
		
		if(fifoCheckValue32(FIFO_USER_01))
		{			
			int command = fifoGetValue32(FIFO_USER_01);			
			switch(command)
			{
			case 0x0001:
				{
					bottom_screen_power_off();
					break;
				}
			case 0x0002:
				{
					bottom_screen_power_on();
					break;
				}
			}
		}
		
		swiIntrWait(1,IRQ_FIFO_NOT_EMPTY | IRQ_VBLANK);
	}
	return 0;
}
