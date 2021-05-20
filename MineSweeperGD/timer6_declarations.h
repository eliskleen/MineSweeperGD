#define TIM6_CR1 ((volatile unsigned short *) 0x40001000)
#define TIM6_DIER ((volatile unsigned short *) 0x4000100C)
#define TIM6_CNT ((volatile unsigned short *) 0x40001024)
#define TIM6_ARR ((volatile unsigned short *) 0x4000102C)
#define TIM6_PSC ((volatile unsigned short *) 0x40001028)
#define TIM6_SR ((volatile unsigned short *) 0x40001010)
#define UDIS (1<<1)
#define UIE (1<<0)
#define CEN (1<<0)
#define UIF (1<<0)

//set and clear interupt
#define NVIC_TIM6_IRQ_BPOS (1<<22)
#define NVIC_TIM6_ISER ((volatile unsigned int *) 0xE000E104)
#define NVIC_TIM6_ICER ((volatile unsigned int *) 0xE000E184)

#define    SCB_VTOR            ((volatile unsigned int *)        0xE000ED08)
#define    TIM6_IRQVEC         ((void (**)(void)) (*SCB_VTOR + 0x0118))
#define REALLOC 0x2001C000

void timer6_init();
void timer6_interrupt();

volatile int ticks;
volatile int seconds;
int timer_running;
