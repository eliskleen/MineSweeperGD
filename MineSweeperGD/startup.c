/*
 * 	startup.c
 *
 *///__asm__ volatile(" LDR R0,=0x2001C000\n")
//#include <ctype.h>
#include "chars_for_disp.h"
#include "systick.h"
#include "timer6.h"
#include "ASCIIDisp.h"
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
#define GPIO_D 0x40020C00
#define GPIO_MODER ((volatile unsigned int *) (GPIO_D))
#define GPIO_OTYPER ((volatile unsigned short *) (GPIO_D+0x4))
#define GPIO_PUPDR ((volatile unsigned int *) (GPIO_D+0xC))
#define GPIO_IDR_LOW ((volatile unsigned char *) (GPIO_D+0x10))
#define GPIO_IDR_HIGH ((volatile unsigned char *) (GPIO_D+0x11))
#define GPIO_ODR_LOW ((volatile unsigned char *) (GPIO_D+0x14))
#define GPIO_ODR_HIGH ((volatile unsigned char *) (GPIO_D+0x15))

#define    SCB_VTOR            ((volatile unsigned int *)        0xE000ED08)


//for ascii
#define FLAGSOFFSET 12
#define TIMEROFFSET 8

void init_app();
unsigned char readKey(void);
unsigned char keyb_enhanced(void);
void activateRow(int row);
int readColumn(void);



int currX = 0;
int currY = 0;


int placed_flaggs = 0;


struct Positions 
{
	int x,y, numberOfBombs;
	int flagged, bomb, opened;
}position;
struct Player
{
	int x,y;
	int placedFlaggs;
};
#define startX 0
#define startY 0
#define sizeX 14
#define sizeY 5
#define playerCh '$'
#define bombCh '*'
#define flagCh '^'
#define Bombs 10
#define sameKeyMax 30

#define sameKeyReturn 0xFE
#define noKeyReturn   0xFF 

struct Positions board[sizeX][sizeY];
int running = 0;
int gameover = 0;

void gotoxy(int x, int y);
void createBoard(struct Positions b[sizeX][sizeY]);
void drawField();
struct Player movePlayer(struct Player p, char moveCh);
void drawPos(struct Positions p);
void printToGD(char ch);
void setCol(int col);
int getColFromBombs(int nBombs);
int * getNewXYFromCh(char ch);
int validPos(int x, int y);
void placeBombs(struct Player p);
void makeFirstOpen(struct Player p);
void openPos(int x, int y);
int countBombs(int x, int y);
void showBombs();
void placeFlag(p);
void gameOver();
void checkWin();
int isBomb(struct Positions pos);
int isFlag(struct Positions pos);
void print_start_text_ascii();
void print_flags(int f);
void print_timer(int secs, int tenths);
int play_again();



int (*isBombPtr)(struct Positions);
int (*isFlagPtr)(struct Positions);


int main(void)
{   
	init_app();
	
	isBombPtr = isBomb;
	isFlagPtr = isFlag;
	running = 1;

	//TODO: kolla vad som faktiskt behöver städas upp...
	//kolla varför den hänger sig ibland, hallå?? vin?!
	while(running)
	{
		graphic_clear_screen();
		createBoard(&board);
		int gameStarted = 0;	
		struct Player p;
		p.x = 0;
		p.y = 0;
		char hitCh;
		char lastKey;
		int nSameKey = 0;
	
		gotoxy(0,0);
		printToGD(playerCh);
		
		print_start_text_ascii();
		print_flags(Bombs - placed_flaggs);
		gameover = 0;
		
		while(!gameover)
		{
			
			print_timer(seconds, ticks);
			//TODO: lägg till att skriva ut tiden på ASCII!!
			// skapa headerfil?
			
			
			checkWin();
			hitCh = keyb_enhanced();
			if( hitCh != noKeyReturn)
			{
				if(nSameKey >= sameKeyMax)
				{
					hitCh = lastKey;
					nSameKey = 0;
				}
				
				if(hitCh != sameKeyReturn)
					lastKey = hitCh;
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
					//TODO: skapa metod för att skrva ut hur många dlaggor är kvar att placera ut
					// ta hand om negativa tal 
					//borde kunna lösa det med NBCD kod om vi hat tvåsiffriga tal?
					break;
					default:	//move
					if(hitCh == 0x2 || hitCh == 0x4 || hitCh == 0x8 || hitCh == 0x6)
						p = movePlayer(p, hitCh);
					break;
				}
				
			}
			if(hitCh == noKeyReturn)
					nSameKey = 0;
			if(hitCh == sameKeyReturn && (lastKey == 0x2 || lastKey == 0x4 || lastKey == 0x8 || lastKey == 0x6))
				nSameKey++;
		}
		
		toggle_timer_running();
		reset_timer();
		running = play_again();
		placed_flaggs = 0;
		gameover = 0;
	}
	return 0;
}
void checkWin()
{
	int squaresToOpen = 60; //sizeX*sizeY - Bombs;
	int openedSquares = 0;
	for(int x = 0; x < sizeX; x++)
		for(int y = 0; y < sizeY; y++)
		{
			if(board[x][y].opened && board[x][y].bomb == 0)
				openedSquares++;
		}
	if(openedSquares == sizeX*sizeY - Bombs)
		gameover = 1;
}
void placeBombs(struct Player p)
{
	 // du kan göra en räknarkrets som du kan läsa av värdet på!!!
	int placedBombs = 0;
	start_rand();
	while(placedBombs < Bombs)
	{
		int tst = get_rand_val();
		int rx = get_rand_val() % sizeX;
		int ry = get_rand_val() % sizeY;
		if(board[rx][ry].bomb == 0 && (rx != p.x && ry != p.y))
		{
			board[rx][ry].bomb = 1;
			placedBombs++;
		}
	}
	end_rand();
	for(int i = 0; i<sizeX; i++)
		for(int j = 0; j < sizeY; j++)
			board[i][j].numberOfBombs = countBombOrFlag(i, j, isBombPtr);
}
void makeFirstOpen(struct Player p)
{
	placeBombs(p);
	openPos(p.x, p.y);
}
void showBombs()
{
	for(int i = 0; i<sizeX; i++)
		for(int j = 0; j < sizeY; j++)
			if(board[i][j].bomb)
				{
					gotoxy(startX + i, startY + j);
					printToGD(bombCh);
				}
}
void placeFlag(struct Player p)
{
	if(board[p.x][p.y].opened)
		return;
	board[p.x][p.y].flagged = (board[p.x][p.y].flagged + 1) % 2;
	p.placedFlaggs = board[p.x][p.y].flagged ? p.placedFlaggs + 1 : p.placedFlaggs -1;
	placed_flaggs = p.placedFlaggs == 1 ? placed_flaggs + 1 : placed_flaggs - 1;
	drawPos(board[p.x][p.y]);
}
void gameOver()
{
	showBombs();
	gameover = 1;
}

void openPos(int x, int y)
{
	if(board[x][y].flagged)
		return;
	
	if(board[x][y].opened && (countBombOrFlag(x,y, isBombPtr) == countBombOrFlag(x,y,isFlagPtr))) //open all connected positions
	{
		for(int i = -1; i<= 1; i++)
			for(int j = -1; j <= 1; j++)
				if(board[x+i][y+j].opened == 0 && validPos(x+i, y+j) && !(i == 0 && j == 0))
				{
					openPos(x+i, y+j);
				}
		return;
	}
	int nBombs = countBombOrFlag(x, y,isBombPtr);
	if(board[x][y].bomb)
	{
		gameOver();
		return;
	}
	board[x][y].opened = 1;
	drawPos(board[x][y]);
	if(nBombs == 0)
		for(int i = -1; i <= 1; i++)
			for(int j = -1; j <= 1; j++)
				if(validPos(x+i, y+j) && board[x+i][y+j].opened == 0 && board[x+i][y+j].bomb == 0)
				{
					//Sleep(10);
					openPos(x+i, y+j);
				}

}
int countBombOrFlag(int x, int y, int (*func)(struct Positions))
{
	int found = 0;
	for(int i = -1; i <= 1; i++)
		for(int j = -1; j <= 1; j++)
			if(validPos(x+i, y+j))
				if(func(board[x+i][y+j]) == 1 && !(i == 0 && j == 0))
					found++;
	return found;
}
int isBomb(struct Positions pos)
{
	return pos.bomb;
}
int isFlag(struct Positions pos)
{
	return pos.flagged;
}


struct Player movePlayer(struct Player p, char moveCh)
{
	
	drawPos(board[p.x][p.y]);
	int *newXY;
	newXY = getNewXYFromCh(moveCh);
	//TODO:
	//check if valid move
	int nextX = p.x + *newXY;
	int nextY = p.y + *(newXY +1);
	if(validPos(nextX, nextY))
	{
		p.x = nextX;
		p.y = nextY;
	}
	gotoxy(startX + p.x, startY + p.y);
	printToGD(playerCh);
	return p;
}

int validPos(int x, int y)
{
	if(x < 0 || x >= sizeX)
		return 0;
	else if(y < 0 || y >= sizeY)
		return 0;
	else
		return 1;
}
void drawPos(struct Positions p)
{
	gotoxy(startX + p.x, startY + p.y);
	if(p.opened)
	{
		char chs[] = {'0','1','2','3','4','5','6','7','8','9'};
		printToGD(chs[p.numberOfBombs]);
	}
	else if(p.bomb && 0)
		{
			printToGD(bombCh);
		}
	else if(p.flagged)
	{
		printToGD(flagCh);
	}
	else
		printToGD(" ");
		
}
int * getNewXYFromCh(char ch)
{
	ch = tolower(ch);
	static int r[2];
	switch(ch)
	{
		case 0x2:
		 r[0] = 0;
		 r[1] = -1;
		 break;
		case 0x8:
		 r[0] = 0;
		 r[1] = 1;
		 break;
		case 0x4:
		 r[0] = -1;
		 r[1] = 0;
		 break;
		case 0x6:
		 r[0] = 1;
		 r[1] = 0;
		 break;
	}
	return r;
}

void createBoard(struct Positions b[sizeX][sizeY])
{
	for(int x = 0; x < sizeX; x++)
	{
		for(int y = 0; y < sizeY; y++) 
		{
			b[x][y].x = x;
			b[x][y].y = y;
			b[x][y].numberOfBombs =  0;
			b[x][y].flagged = 0;
			b[x][y].opened = 0;
			b[x][y].bomb = 0;
		} 
	}
}


void gotoxy(int x, int y)
{
  currX = x;
  currY = y;
}

void printToGD(char ch)
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
	printNumAt(currX*CHAR_X, currY*CHAR_Y, print_ch);
}

void printNumAt(int x, int y, DISPCHAR ch)
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

void init_app(void)
{
	*GPIO_MODER = 0x55000000;
	*GPIO_OTYPER = 0xFFAA0000;
	*GPIO_PUPDR = 0x0F0000;
	*GPIO_E_MODER = 0x55555555;
	*SCB_VTOR = REALLOC;
	graphic_initialize();
	sysTick_init();
	timer6_init();
	toggle_timer_running();
	ascii_init();
}

#define waitState 1
#define initState 0
unsigned char current_state = initState;


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
		if(*GPIO_IDR_HIGH == 0)
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
		case 1: *GPIO_ODR_HIGH = 0x10; break;
		case 2: *GPIO_ODR_HIGH = 0x20; break;
		case 3: *GPIO_ODR_HIGH = 0x40; break;
		case 4: *GPIO_ODR_HIGH = 0x80; break;
		case 5: *GPIO_ODR_HIGH = 0xF0; break; 
		case 0: *GPIO_ODR_HIGH = 0x00; break;
	}
}
int readColumn(void)
{
	/* Om någon tangent (i aktiverad rad)
	* är nedtryckt, returnera dess kolumnnummer,
	* annars, returnera 0 */
	unsigned char c;
	c = *GPIO_IDR_HIGH;
	if (c & 0x8) return 4;
	if (c & 0x4) return 3;
	if (c & 0x2) return 2;
	if (c & 0x1) return 1;
	return 0;
}

//ascii
void print_start_text_ascii()
{
	ascii_gotoxy(1,1);
	char upper[] = "Flags left: ";
	char lower[] = "Timer: 		";
	ascii_write_string(upper);
	ascii_gotoxy(1,2);
	ascii_write_string(lower);
}

void print_flags(int f)
{
	if(f > 0)
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