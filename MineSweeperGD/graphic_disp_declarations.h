

#define playerCh '$'
#define bombCh '*'
#define flagCh '^'
//for ascii
#define FLAGSOFFSET 12
#define TIMEROFFSET 8



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



void init_disp();
void printToGD(int x, int y,char ch);
void print_start_text_ascii();
void print_flags(int f);
void print_timer(int secs, int tenths);
void graphic_initialize(void);
void graphic_clear_screen(void); 
void graphic_pixel_set(int x, int y);
void graphic_pixel_clear(int x, int y);