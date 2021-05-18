#include "gpio.h"

/*
#define GPIO_D 0x40020C00
#define GPIO_MODER ((volatile unsigned int *) (GPIO_D))
#define GPIO_OTYPER ((volatile unsigned short *) (GPIO_D+0x4))
#define GPIO_PUPDR ((volatile unsigned int *) (GPIO_D+0xC))
#define GPIO_IDR_LOW ((volatile unsigned char *) (GPIO_D+0x10))
#define GPIO_IDR_HIGH ((volatile unsigned char *) (GPIO_D+0x11))
#define GPIO_ODR_LOW ((volatile unsigned char *) (GPIO_D+0x14))
#define GPIO_ODR_HIGH ((volatile unsigned char *) (GPIO_D+0x15))
*/

#define sameKeyReturn 0xFE
#define noKeyReturn   0xFF 

#define waitState 1
#define initState 0

unsigned char readKey(void);
unsigned char keyb_enhanced(void);
void activateRow(int row);
int readColumn(void);

unsigned char current_state = initState;

static volatile GPIO * port;

void init_keypad(GPIO * p)
{
	port = p;
	port->moder = 0x55000000; //*GPIO_MODER = 0x55000000;
	port->otyper = 0xFFAA0000; // *GPIO_OTYPER = 0xFFAA0000;
	port->pupdr = 0x0F0000; //	*GPIO_PUPDR = 0x0F0000;
}

unsigned char readKey(void)
{
	unsigned char keyValue[] = {1,2,3,0xA,4,5,6,0xB,7,8,9,0xC,0xE,0,0xF,0xD};
	for(int row = 0; row< 4; row++)
	{
		activateRow(row+1);
		int col = readColumn();
		if(col != 0)
		{
			activateRow(0);
			return keyValue[row*4 + (col-1)];
		}
	}
	activateRow(0);
	return noKeyReturn;
}

unsigned char keyb_enhanced(void)
{
	if(current_state == waitState)
	{
		activateRow(5);
		if(port->idrHigh == 0)//if(*GPIO_IDR_HIGH == 0)
		{ current_state = initState; }
		return sameKeyReturn;
	}
	if(current_state == initState)
	{
		unsigned char c = readKey();
		if(c != noKeyReturn)
			current_state = waitState;
		return c;
	}

}

void activateRow(int row)
{ /* Aktivera angiven rad hos tangentbordet, eller
* deaktivera samtliga */
	switch( row )
	{
		//port.odrHigh
		case 1: port->odrHigh = 0x10; break;
		case 2: port->odrHigh = 0x20; break;
		case 3: port->odrHigh = 0x40; break;
		case 4: port->odrHigh = 0x80; break;
		case 5: port->odrHigh = 0xF0; break; 
		case 0: port->odrHigh = 0x00; break;
	}
}
int readColumn(void)
{
	/* Om någon tangent (i aktiverad rad)
	* är nedtryckt, returnera dess kolumnnummer,
	* annars, returnera 0 */
	unsigned char c;
	c = port->idrHigh; //*GPIO_IDR_HIGH;
	if (c & 0x8) return 4;
	if (c & 0x4) return 3;
	if (c & 0x2) return 2;
	if (c & 0x1) return 1;
	return 0;
}