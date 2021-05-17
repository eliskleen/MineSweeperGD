/* Masker för kontrollbitar */
#define B_E 0x40 /* Enable-signal */
#define B_SELECT 4 /* Välj ASCII-display */
#define B_RW 2 /* 0=Write, 1=Read */
#define B_RS 1 /* 0=Control, 1=Data */

#define STATUS_FLAG 0x80

#define PORT_BASE    0x40021000
#define portModer    ((volatile unsigned int *)(PORT_BASE))
#define portOtyper   ((volatile unsigned short *) (PORT_BASE+0x4))
#define portPupdr    ((volatile unsigned int *) (PORT_BASE+0xC))
#define portOspeeder ((volatile unsigned int *)(PORT_BASE+0x08))
#define portIdrLow   ((volatile unsigned char *) (PORT_BASE+0x10))
#define portIdrHigh  ((volatile unsigned char *) (PORT_BASE+0x11))
#define portOdrLow   ((volatile unsigned char *) (PORT_BASE+0x14))
#define portOdrHigh  ((volatile unsigned char *) (PORT_BASE+0x15))


void ascii_ctrl_bit_set( char x );
void ascii_ctrl_bit_clear( char x );
void delay_250ns();
void delay_milli(unsigned int ms);
void delay_micro(unsigned int us);
void ascii_write_controller(char cmd);
void ascii_write_command(char cmd);
void ascii_write_data(char cmd);
unsigned char ascii_read_controller();
unsigned char ascii_read_status();
unsigned char ascii_read_data();
void ascii_command(char cmd);
void ascii_write_char(char ch);

void ascii_write_string(char * p);


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
