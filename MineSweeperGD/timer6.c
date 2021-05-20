#ifndef TIMER6INC
	#define TIMER6INC
	#include "timer6_declarations.h"
#endif
void timer6_init()
{
	ticks = 0; 
	seconds = 0;
	*TIM6_CR1 &= ~CEN; //stoppa räknare
	//*SCB_VTOR = REALLOC;
	*TIM6_IRQVEC = timer6_interrupt;
	*NVIC_TIM6_ISER |= NVIC_TIM6_IRQ_BPOS;
	
	//100ms tidbas
	*TIM6_PSC = 839;
    *TIM6_ARR = 9999;
	*TIM6_DIER |= UIE;
	*TIM6_CR1 |= CEN; //starta räknare
	timer_running = 1;
}


void timer6_interrupt()
{
	*TIM6_SR &= ~UIF;
	ticks++;
	if(ticks > 9)
	{
		ticks = 0;
		seconds ++;
	}
}

void toggle_timer_running()
{
	timer_running = !timer_running;
	set_running(timer_running);
}

void set_running(int r)
{
	if(r)
		*TIM6_CR1 |= CEN; //starta räknare
	else
		*TIM6_CR1 &= ~CEN; //stoppa räknare
	timer_running = r;
}
void reset_timer()
{
	ticks = 0;
	seconds = 0;
}