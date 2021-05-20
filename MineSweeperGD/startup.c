/*
 * 	startup.c
 *
 *///__asm__ volatile(" LDR R0,=0x2001C000\n")
//#include <ctype.h>

#ifndef GAME_LOGIC_DEC
	#define GAME_LOGIC_DEC
	#include "game_logic_declarations.h"
#endif
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
int contains(char * arr, char ch);


int main(void)
{   
	//Ett ganska stort problem är att ibland fryser spelet, jag kan verkligen inte förstå varför
	//om jag skulle gissa så är det simulatorn som hänger sig av någon anledning
	init_app();
	
	running = 1;
	int game_started = 0;	
	struct Player p;
	char hit_ch;
	char last_key = 0;
	int n_same_key = 0;
	int won = 0;
	
	
	while(running)	// loop som körs hela tiden programmet är igånh
	{
		set_running(1);
		reset_game(&game_started, &n_same_key, &p);
		while(!gameover) // loop för varje spel
		{
			if(game_started)
				print_timer(seconds, ticks);
			won = check_win();
			hit_ch = keyb_enhanced();
			if( hit_ch != NOKEYRETURN)
			{
				if(n_same_key >= SAMEKEYMAX) //ska vi ge samma input igen?
				{
					hit_ch = last_key;
					n_same_key = SAMEKEYMAX / 3;
				}
				
				if(hit_ch != SAMEKEYRETURN)// vi har en ny knapp, spara i lastkey 
				{
					last_key = hit_ch; 
					n_same_key = 0;
				}
				switch(hit_ch)
				{
					case 0xA: // open
					if(!game_started)
					{
						make_first_open(p);
						game_started = 1;
						reset_timer(); // eftersom vi använder timer6 för att få randomtal när vi sätter ut bomberna 
					}				   // måste vi starta om den när vi öppnar
					else
						open_pos(p.x, p.y);
					break;
					case 0xB: // place flag
					place_flag(&p);
					print_flags(BOMBS - p.placed_flaggs);
					break;
					default:	//move
					if(contains(move_keys, hit_ch))
					{
						p = move_player(p, hit_ch);
						print_player(p);
					}
					break;
				}
			}
			if(hit_ch == NOKEYRETURN || !contains(move_keys, last_key)) // ingen knapp nedtryckt, samma i rad = 0
					n_same_key = 0;
			if(hit_ch == SAMEKEYRETURN && contains(move_keys, last_key))
				n_same_key++; 											// vi har samma knapp igen, och det är en "move" knapp, öka samma i rad
		}
		print_win_loose_msg(won);
		toggle_timer_running();
		reset_timer();
		delay_milli(1000); // detta kan också behöva justeras för den riktiga hårdvaran...
		running = play_again();
		p.placed_flaggs = 0;
		gameover = 0;
	}
	ascii_gotoxy(1,1);
	ascii_write_string("Well played! Bye :)           :");
	return 0;
}

void init_app(void)
{
	init_keypad(&GPIO_D);
	*GPIO_E_MODER = 0x55555555; // setup för GD
	*SCB_VTOR = REALLOC;		
	graphic_initialize();
	sysTick_init();
	timer6_init();
	ascii_init();
}

int play_again()
{
	char upper[] = "Play again? yes -> D";
	char lower[] = "yes -> D, no -> anything";
	ascii_gotoxy(1,1);
	ascii_write_string(upper);
	char hitch = 0xFF;
	while(1)
	{
		hitch = keyb_enhanced();
		if(hitch != SAMEKEYRETURN && hitch != NOKEYRETURN) // vänta på att man släppt upp efter man vann/förlorade
			break;
	}
	return hitch == 0xD;
	
}

int contains(char * arr, char ch)
{
	while(*arr != 0)
		if(*arr++ == ch)
			return 1;
	return 0;
}