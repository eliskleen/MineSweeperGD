
#define STK 0xE000E010
#define STK_CTRL  ((volatile int *)(STK))
#define STK_LOAD  ((volatile int *)(STK+0x04))
#define STK_VAL   ((volatile int *)(STK+0x08))
#define STK_CALIB ((volatile int *)(STK+0x0C))
#define LOAD_MAX 0xFF


#define SCB_VTOR ((volatile unsigned long *)0xE000ED08)
#define REALLOC 0x2001C000

static volatile int systick_flag;
static volatile int delay_count;

static volatile int counting;
static volatile int used_for_rand;


void systick_irq_handler( void );

void start_rand()
{
	if(counting) return; // kan inte göra båda samtidigt
	used_for_rand = 1;
	*STK_CTRL = 0;
	*STK_LOAD = LOAD_MAX;
	*STK_VAL = 0;
	*STK_CTRL = 7;
}
int get_rand_val()
{
	if(used_for_rand)
		return *STK_VAL;
	else return 0;
}
void end_rand()
{
	*STK_CTRL = 0;
	used_for_rand = 0;
}

void sysTick_init()
{
	//*SCB_VTOR = REALLOC;
	*((void (**)(void) ) 0x2001C03C) = &systick_irq_handler;
}

void delay_1mikro( void )
{
	*STK_CTRL = 0;
	*STK_LOAD = (168 - 1);
	*STK_VAL = 0;
	*STK_CTRL = 7;
}

void systick_irq_handler( void )
{
	if(used_for_rand) systick_irq_handler_rand();
	else systick_irq_handler_counting();
}

void systick_irq_handler_counting( void )
{
	*STK_CTRL = 0;
	delay_count -- ;
	if( delay_count > 0 ) delay_1mikro();
	else 
		{
			systick_flag = 1;
			counting = 0;
		}
}
void systick_irq_handler_rand( void )
{
	*STK_CTRL = 0;
	*STK_LOAD = LOAD_MAX;
	*STK_VAL = 0;
	*STK_CTRL = 7;
}
void delay( unsigned int count )
{
	if(used_for_rand) return; // kan inte göra båda samtidigt
	counting = 1;
	if( count == 0 ) return;
	delay_count = count;
	systick_flag = 0;
	delay_1mikro();
}

