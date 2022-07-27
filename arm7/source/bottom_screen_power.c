#include "bottom_screen_power.h"
#include <nds.h>

// with help from moonshell source code

u8 PM_GetRegister(int reg)
{
	SerialWaitBusy();
 
	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER |SPI_BAUD_1MHz | SPI_CONTINUOUS;
	REG_SPIDATA = reg | 0x80;
 
	SerialWaitBusy();
 
	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER |SPI_BAUD_1MHz ;
	REG_SPIDATA = 0;
 
	SerialWaitBusy();

	return REG_SPIDATA & 0xff;
}

void PM_SetRegister(int reg, int control)
{
	SerialWaitBusy();
 
	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER |SPI_BAUD_1MHz | SPI_CONTINUOUS;
	REG_SPIDATA = reg;
 
	SerialWaitBusy();
 
	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER |SPI_BAUD_1MHz;
	REG_SPIDATA = control;
}

void a7led(int sw)
{
	int control = PM_LED_CONTROL(3) | BIT(7);
	int sc = sw << 4;
	control = PM_GetRegister(0) & ~control;
	PM_SetRegister(0, (control|sc)&255);
}

void a7lcd_select(int control)
{
	control |= PM_GetRegister(0) & ~(PM_BACKLIGHT_BOTTOM | PM_BACKLIGHT_TOP);
	PM_SetRegister(0, control&255);
}

void bottom_screen_power_off()
{
	a7led(0);
    a7lcd_select(PM_BACKLIGHT_TOP);	
}

void bottom_screen_power_on()
{
	a7led(0);
    a7lcd_select(PM_BACKLIGHT_TOP | PM_BACKLIGHT_BOTTOM);
}