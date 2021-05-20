#ifndef GRAPGIC_DISP_DEC
	#define GRAPGIC_DISP_DEC
	#include "graphic_disp_declarations.h"
#endif
#ifndef CHARS_FOR_DISP
	#define CHARS_FOR_DISP
	#include "chars_for_disp.h"
#endif

#include "systick.h"
void init_disp(int use_ascii)
{
	if(use_ascii)
		ascii_init();
	else
		*portModer = 0x55555555;
}

void print_to_gd(int x, int y,char ch)
{
	DISPCHAR print_ch;
	switch(ch)
	{
		case '0':
		print_ch = zero;
		break;
		case '1':
		print_ch = one;
		break;
		case '2':
		print_ch = two;
		break;
		case '3':
		print_ch = three;
		break;
		case '4':
		print_ch = four;
		break;
		case '$':
		print_ch = doll;
		break;
		case bombCh:
		print_ch = star;
		break;
		case flagCh:
		print_ch = flag;
		break;
		default:
		print_ch = space;
		break;
	}
	print_num_at(x*CHAR_X, y*CHAR_Y, print_ch);
}

void print_num_at(int x, int y, DISPCHAR ch)
{
	for(int i = 0; i < CHAR_Y; i++)
		for(int j = 0; j < CHAR_X; j++)
		if(ch.pattern[i][j])
			graphic_pixel_set(j+x, i+y);
		else
			graphic_pixel_clear(j+x, i+y);
}


__attribute__((naked))
void graphic_initialize(void)
{
	__asm volatile(" .HWORD 0xDFF0\n");
	__asm volatile(" BX LR\n");
}
__attribute__((naked))
void graphic_clear_screen(void)
{
	__asm volatile(" .HWORD 0xDFF1\n");
	__asm volatile(" BX LR\n");
}
__attribute__((naked))
void graphic_pixel_set(int x, int y)
{
	__asm volatile(" .HWORD 0xDFF2\n");
	__asm volatile(" BX LR\n");
}
__attribute__((naked))
void graphic_pixel_clear(int x, int y)
{
	__asm volatile(" .HWORD 0xDFF3\n");
	__asm volatile(" BX LR\n");
}


//ascii
void print_start_text_ascii()
{
	ascii_gotoxy(1,1);
	char upper[] = "Flags left: ";
	char lower[] = "Time: 		";
	ascii_write_string(upper);
	ascii_gotoxy(1,2);
	ascii_write_string(lower);
}

void print_win_loose_msg(int won)
{
	ascii_gotoxy(1,1);
	char * str = won != 0 ? "Congrats!! Well played <3" : "Thats to bad...";
	ascii_write_string(str);
}

void print_flags(int f)
{
	if(f >= 0)
	{
		ascii_gotoxy(FLAGSOFFSET, 1);
		char clear[] = "  ";
		ascii_write_string(clear);
	
		ascii_gotoxy(FLAGSOFFSET, 1);
		ascii_write_number(f);	
	}
	else {
		ascii_gotoxy(FLAGSOFFSET, 1);
		ascii_write_char(45); // - = 45
		f *= (-1);
		ascii_write_number(f);
	}
	
}

void print_timer(int secs, int tenths)
{
	ascii_gotoxy(TIMEROFFSET, 2);
	ascii_write_number(secs);
	ascii_write_char(46); // . = 46
	ascii_write_number(tenths);
}


void ascii_init()
{
	*portModer = 0x55555555;
	ascii_command(1); //clear display
	ascii_command(0xF); //display på, markör: på, blinkande markör: på
	ascii_gotoxy(1,1);
}
void ascii_gotoxy(int x, int y)
{
	int address = x-1;
	address = y == 2 ? address + 0x40 : address;
	ascii_write_command(STATUS_FLAG | address);
}
void ascii_write_string(char * p)
{
	while(* p!= 0)
		ascii_write_char(*p++);
}
void ascii_write_number(int num)
{
	if(num < 10)
		ascii_write_char(48 + num);
	else {
		int tens = num / 10;
		int ones = num % 10;
		ascii_write_char(48 + tens);
		ascii_write_char(48 + ones);
	}
}
void ascii_write_char(char ch)
{
	while((ascii_read_status() & STATUS_FLAG) == STATUS_FLAG);
	delay_micro(8);
	ascii_write_data(ch);
	delay_micro(45);
}
void ascii_command(char cmd)
{
	while((ascii_read_status() & STATUS_FLAG) == STATUS_FLAG);
	delay_micro(8);
	ascii_write_command(cmd);
	delay_milli(2);
}

unsigned char ascii_read_data()
{
	*portModer = 0x00005555;
	ascii_ctrl_bit_set(B_RW);
	ascii_ctrl_bit_set(B_RS);
	unsigned char rv = ascii_read_controller();
	*portModer = 0x55555555;
	return rv;
}

unsigned char ascii_read_status()
{
	*portModer = 0x00005555;
	ascii_ctrl_bit_set(B_RW);
	ascii_ctrl_bit_clear(B_RS);
	unsigned char rv = ascii_read_controller();
	*portModer = 0x55555555;
	return rv;
}
unsigned char ascii_read_controller()
{
	ascii_ctrl_bit_set(B_E);
	delay_250ns();
	delay_250ns();
	unsigned char rv = *portIdrHigh;
	ascii_ctrl_bit_clear(B_E);
	return rv;
}
void ascii_write_data(char data)
{
	ascii_ctrl_bit_set(B_RS);
	ascii_ctrl_bit_clear(B_RW);
	ascii_write_controller(data);
}
void ascii_write_command(char cmd)
{
	ascii_ctrl_bit_clear(B_RW);
	ascii_ctrl_bit_clear(B_RS);
	ascii_write_controller(cmd);
}
void ascii_write_controller(char cmd)
{
	ascii_ctrl_bit_set(B_E);
	*portOdrHigh = cmd;
	delay_250ns();
	ascii_ctrl_bit_clear(B_E);
}
void ascii_ctrl_bit_set( char x )
{ /* x: bitmask bits are 1 to set */
	char c;
	c = *portOdrLow;
	*portOdrLow = B_SELECT | x | c;
}
void ascii_ctrl_bit_clear( char x )
{ /* x: bitmask bits are 1 to clear */
	char c;
	c = *portOdrLow;
	c = c & ~x;
	*portOdrLow = B_SELECT | c;
}


void delay_250ns()
{
	//ett tick från klockan var 6:e nanosekund 
	//för att få 250 ns behöver vi vänta 6*42 ns
	*STK_CTRL = 0;
	*STK_LOAD = 41; // 250/6 =ca 42, -1 för att klockan är ett tick försenat att märka att val = 0 
	*STK_VAL = 0;
	*STK_CTRL = 5;	// 101 systemklocka, inget avbrott, starta räknare
	while((*STK_CTRL & 0x10000) == 0); // vänta på bit 16 = 1
	*STK_CTRL = 0; // stoppa räknaren
}

#define SIMULATED
void delay_micro(unsigned int us)
{
#ifdef SIMULATED
	us /= 1000;
	us++;
#endif
	while(us > 0)
	{
		delay_250ns();
		delay_250ns();
		delay_250ns();
		delay_250ns();
		us--;
	}
}

void delay_milli(unsigned int ms)
{
	delay_micro(ms*1000);
}
