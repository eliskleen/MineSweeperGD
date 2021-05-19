/*
 * 	startup.c
 *
 *///__asm__ volatile(" LDR R0,=0x2001C000\n")
//#include <ctype.h>

#ifndef GAME_LOGIC_DEC
	#define GAME_LOGIC_DEC
	#include "game_logic_declarations.h"
#endif

#include "timer6.h"
#include "keypad.h"


__attribute__((naked)) __attribute__((section (".start_section")) )
void startup ( void )
{
__asm__ volatile(" LDR R0,=0x2001C000\n");    
__asm__ volatile(" MOV SP,R0\n");
//__asm__ volatile(" BL _crt_init\n");            
__asm__ volatile(" BL main\n");                    
//__asm__ volatile(" BL _crt_deinit\n");            
__asm__ volatile(" B .\n");               
__asm__ volatile(" .LTORG\n");                
}

#define GPIO_E 0x40021000
#define GPIO_E_MODER ((volatile unsigned int *) (GPIO_E))

//port for keypad
#define GPIO_D (*((volatile GPIO*) 0x40020c00))

#define    SCB_VTOR            ((volatile unsigned int *)        0xE000ED08)
void init_app();
int main(void)
{   
	init_app();
	
	running = 1;
	int gameStarted = 0;	
	struct Player p;
	char hitCh;
	char lastKey = 0;
	int nSameKey = 0;

	//TODO: kolla vad som faktiskt behöver städas upp...
	//kolla varför den hänger sig ibland, hallå?? vin?!
	while(running)
	{
		reset_game(&gameStarted, &nSameKey, &p);
		while(!gameover)
		{
			print_timer(seconds, ticks);
			checkWin();
			hitCh = keyb_enhanced();
			if( hitCh != noKeyReturn)
			{
				if(nSameKey >= sameKeyMax) //ska vi ge samma input igen?
				{
					hitCh = lastKey;
					nSameKey = 0;
				}
				
				if(hitCh != sameKeyReturn)// vi har en ny knapp, spara i lastkey ska inte detta bara göras
					lastKey = hitCh; // när vi har en move??
				switch(hitCh)
				{
					case 0xA: // open
					if(!gameStarted)
					{
						makeFirstOpen(p);
						gameStarted = 1;
						toggle_timer_running();
					}
					else
						openPos(p.x, p.y);
					break;
					case 0xB: // place flag
					placeFlag(p);
					print_flags(Bombs - placed_flaggs);
					break;
					default:	//move
					//if(hitCh == 0x2 || hitCh == 0x4 || hitCh == 0x8 || hitCh == 0x6)// TODO: skapa lista och contains typ? samt diagonal movement
					p = movePlayer(p, hitCh);
					break;
				}
			}
			if(hitCh == noKeyReturn) // ingen knapp nedtryckt, samma irad = 0
					nSameKey = 0;
			if(hitCh == sameKeyReturn && (lastKey == 0x2 || lastKey == 0x4 || lastKey == 0x8 || lastKey == 0x6))
				nSameKey++; //vi har samma knapp ige, och det är en "move" knapp, öka samma irad
		}
		
		toggle_timer_running();
		reset_timer();
		running = play_again();
		placed_flaggs = 0;
		gameover = 0;
	}
	return 0;
}

void init_app(void)
{
	init_keypad(&GPIO_D);
	/*
	GPIO_D.moder = 0x55000000; //*GPIO_MODER = 0x55000000;
	GPIO_D.otyper = 0xFFAA0000; // *GPIO_OTYPER = 0xFFAA0000;
	GPIO_D.pupdr = 0x0F0000; //	*GPIO_PUPDR = 0x0F0000;
	GPIO_D.odrHigh = 0xF;
	*/
	
	*GPIO_E_MODER = 0x55555555;
	*SCB_VTOR = REALLOC;
	graphic_initialize();
	sysTick_init();
	timer6_init();
	toggle_timer_running();
	ascii_init();
}

int play_again()
{
	char upper[] = "Play again? yes -> D";
	char lower[] = "yes -> D, no -> anything";
	ascii_gotoxy(1,1);
	ascii_write_string(upper);
	//ascii_gotoxy(1,2);
	//ascii_write_string(lower);
	char hitch = 0xFF;
	while(1)
	{
		hitch = keyb_enhanced();
		if(hitch != sameKeyReturn && hitch != noKeyReturn)
			break;
	}
	return hitch == 0xD;
	
}
