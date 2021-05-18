#ifndef GAME_LOGIC_DEC
	#define GAME_LOGIC_DEC
	#include "game_logic_declarations.h"
#endif
#ifndef GRAPGIC_DISP_DEC
	#define GRAPGIC_DISP_DEC
	#include "graphic_disp_declarations.h"
#endif



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
					printToGD(currX, currY, bombCh);
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
	printToGD(currX, currY, playerCh);
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
		printToGD(currX, currY, chs[p.numberOfBombs]);
	}
	else if(p.bomb && 0)
		{
			printToGD(currX, currY, bombCh);
		}
	else if(p.flagged)
	{
		printToGD(currX, currY, flagCh);
	}
	else
		printToGD(currX, currY, " ");
		
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

void reset_game(int * gameStarted, int * nSameKey, struct Player * p)
{
	isBombPtr = isBomb;
	isFlagPtr = isFlag;
	graphic_clear_screen();
	createBoard(&board);
	*gameStarted = 0;	
	p->x = 0;
	p->y = 0;
	*nSameKey = 0;
	gotoxy(0,0);
	printToGD(currX, currY, playerCh);
	print_start_text_ascii();
	print_flags(Bombs - placed_flaggs);
	gameover = 0;
	currX = 0;
	currY = 0;
	running = 0;
	gameover = 0;	
	placed_flaggs = 0;
}