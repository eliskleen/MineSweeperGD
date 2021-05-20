#ifndef GAME_LOGIC_DEC
	#define GAME_LOGIC_DEC 
	#include "game_logic_declarations.h"
#endif
void reset_game(int * game_started, int * n_same_key, struct Player * p)
{
	for(int i = 0; i<=7; i++)
		move_keys[i] = i < 4 ? i+1 : i+2;
	is_bomb_ptr = is_bomb;
	is_flag_ptr = is_flag;
	graphic_clear_screen();
	create_board(&board);
	*game_started = 0;	
	p->x = 0;
	p->y = 0;
	*n_same_key = 0;
	gameover = 0;
	curr_x = 0;
	curr_y = 0;
	running = 0;
	gameover = 0;	
	p->placed_flaggs = 0;
	gotoxy(0,0);
	print_to_gd(curr_x, curr_y, PLAYERCH);
	print_start_text_ascii();
	print_flags(BOMBS - p->placed_flaggs);
}

int check_win()
{
	int squares_to_open = SIZEX*SIZEY - BOMBS;
	int opened_squares = 0;
	for(int x = 0; x < SIZEX; x++)
		for(int y = 0; y < SIZEY; y++)
		{
			if(board[x][y].opened && board[x][y].bomb == 0)
				opened_squares++;
		}
	if(opened_squares == SIZEX*SIZEY - BOMBS)
		gameover = 1;
	return gameover;	
}
void place_bombs(struct Player p)
{
	 // du kan göra en räknarkrets som du kan läsa av värdet på!!!
	int placed_bombs = 0;
	while(placed_bombs < BOMBS)
	{
		int rx = *TIM6_CNT % SIZEX;
		int ry = *TIM6_CNT % SIZEY;
		if(board[rx][ry].bomb == 0 && (rx != p.x || ry != p.y))
		{
			board[rx][ry].bomb = 1;
			placed_bombs++;
		}
	}
	for(int i = 0; i<SIZEX; i++)
		for(int j = 0; j < SIZEY; j++)
			board[i][j].number_of_bombs = count_bomb_or_flag(i, j, is_bomb_ptr);
}
void make_first_open(struct Player p)
{
	place_bombs(p);
	open_pos(p.x, p.y);
}
void show_bombs()
{
	for(int i = 0; i<SIZEX; i++)
		for(int j = 0; j < SIZEY; j++)
			if(board[i][j].bomb)
				{
					gotoxy(STARTX + i, STARTY + j);
					print_to_gd(curr_x, curr_y, BOMBCH);
				}
}
void place_flag(struct Player * p)
{
	if(board[p->x][p->y].opened)
		return;
	board[p->x][p->y].flagged = (board[p->x][p->y].flagged + 1) % 2;
	p->placed_flaggs = board[p->x][p->y].flagged ? p->placed_flaggs + 1 : p->placed_flaggs -1;
	draw_pos(board[p->x][p->y]);
}
void game_over()
{
	show_bombs();
	gameover = 1;
}

void open_pos(int x, int y)
{
	if(board[x][y].flagged)
		return;
	
	if(board[x][y].opened && (count_bomb_or_flag(x,y, is_bomb_ptr) == count_bomb_or_flag(x,y,is_flag_ptr))) //open all connected positions
	{
		for(int i = -1; i<= 1; i++)
			for(int j = -1; j <= 1; j++)
				if(board[x+i][y+j].opened == 0 && valid_pos(x+i, y+j) && !(i == 0 && j == 0))
				{
					open_pos(x+i, y+j);
				}
		return;
	}
	int nBombs = count_bomb_or_flag(x, y,is_bomb_ptr);
	if(board[x][y].bomb)
	{
		game_over();
		return;
	}
	board[x][y].opened = 1;
	draw_pos(board[x][y]);
	if(nBombs == 0)
		for(int i = -1; i <= 1; i++)
			for(int j = -1; j <= 1; j++)
				if(valid_pos(x+i, y+j) && board[x+i][y+j].opened == 0 && board[x+i][y+j].bomb == 0)
				{
					//Sleep(10);
					open_pos(x+i, y+j);
				}

}
int count_bomb_or_flag(int x, int y, int (*func)(struct Positions))
{
	int found = 0;
	for(int i = -1; i <= 1; i++)
		for(int j = -1; j <= 1; j++)
			if(valid_pos(x+i, y+j))
				if(func(board[x+i][y+j]) == 1 && !(i == 0 && j == 0))
					found++;
	return found;
}
int is_bomb(struct Positions pos)
{
	return pos.bomb;
}
int is_flag(struct Positions pos)
{
	return pos.flagged;
}

void print_player(struct Player p)
{
	gotoxy(STARTX + p.x, STARTY + p.y);
	print_to_gd(curr_x, curr_y, PLAYERCH);
}
struct Player move_player(struct Player p, char moveCh)
{
	//TODO: diagonal movement, kan göra i två steg för att fixa valid move problemet
	if(moveCh == 0x1 || moveCh == 0x3 || moveCh == 0x7 || moveCh == 0x9)
		return move_player_diagonal(p, moveCh);
		
	draw_pos(board[p.x][p.y]);
	int *newXY;
	newXY = get_new_xy_from_ch(moveCh);
	//TODO:
	//check if valid move
	int nextX = p.x + *newXY;
	int nextY = p.y + *(newXY +1);
	if(valid_pos(nextX, nextY))
	{
		p.x = nextX;
		p.y = nextY;
	}
	return p;
}
struct Player move_player_diagonal(struct Player p, char moveCh)
{
	switch(moveCh)
	{
		case 0x1: // upp and left
		 p = move_player(p, 0x4); // left
		 p = move_player(p, 0x2); // upp
		 break;
		case 0x3: // upp and right
		 p = move_player(p, 0x6); // right
		 p = move_player(p, 0x2); // upp
		 break;
		case 0x7: // down and right
		 p = move_player(p, 0x8); // down
		 p = move_player(p, 0x4); // left
		 break;
		case 0x9: // down and right
		 p = move_player(p, 0x8); // down
		 p = move_player(p, 0x6); // right
		 break;
	}
	return p;
}


int valid_pos(int x, int y)
{
	if(x < 0 || x >= SIZEX)
		return 0;
	else if(y < 0 || y >= SIZEY)
		return 0;
	else
		return 1;
}
void draw_pos(struct Positions p)
{
	gotoxy(STARTX + p.x, STARTY + p.y);
	if(p.opened)
	{
		char chs[] = {'0','1','2','3','4','5','6','7','8','9'};
		print_to_gd(curr_x, curr_y, chs[p.number_of_bombs]);
	}
	else if(p.bomb && 0)
		{
			print_to_gd(curr_x, curr_y, BOMBCH);
		}
	else if(p.flagged)
	{
		print_to_gd(curr_x, curr_y, FLAGCH);
	}
	else
		print_to_gd(curr_x, curr_y, ' ');
		
}
int * get_new_xy_from_ch(char ch)
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

void create_board(struct Positions b[SIZEX][SIZEY])
{
	for(int x = 0; x < SIZEX; x++)
	{
		for(int y = 0; y < SIZEY; y++) 
		{
			b[x][y].x = x;
			b[x][y].y = y;
			b[x][y].number_of_bombs =  0;
			b[x][y].flagged = 0;
			b[x][y].opened = 0;
			b[x][y].bomb = 0;
		} 
	}
}


void gotoxy(int x, int y)
{
  curr_x = x;
  curr_y = y;
}

