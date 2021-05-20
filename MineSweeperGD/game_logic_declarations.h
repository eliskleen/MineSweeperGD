
#ifndef GRAPGIC_DISP_DEC
	#define GRAPGIC_DISP_DEC
	#include "graphic_disp_declarations.h"
#endif
#ifndef TIMER6INC
	#define TIMER6INC
	#include "timer6_declarations.h"
#endif

struct Positions 
{
	int x,y, number_of_bombs;
	int flagged, bomb, opened;
}position;
struct Player
{
	int x,y;
	int placed_flaggs;
};
#define STARTX 0
#define STARTY 0
#define SIZEX 16
#define SIZEY 5
#define PLAYERCH '$'
#define BOMBCH '*'
#define FLAGCH '^'
#define BOMBS 10

#define SIMULATED
#ifdef SIMULATED
	#define SAMEKEYMAX 20 //denna behöver justeras beroende på hur snabb din dator är...
#else
	#define SAMEKEYMAX 20*1000 //detta borde ju funka för riktig hårdvara
#endif

void gotoxy(int x, int y);
void create_board(struct Positions b[SIZEX][SIZEY]);
void draw_field();
struct Player move_player(struct Player p, char moveCh);
struct Player move_player_diagonal(struct Player p, char moveCh);
void print_player(struct Player p);
void draw_pos(struct Positions p);
int * get_new_xy_from_ch(char ch);
int valid_pos(int x, int y);
void place_bombs(struct Player p);
void make_first_open(struct Player p);
void open_pos(int x, int y);
int count_bombs(int x, int y);
void show_bombs();
void place_flag(p);
void game_over();
int check_win();
int is_bomb(struct Positions pos);
int is_flag(struct Positions pos);
void reset_game();


int (*is_bomb_ptr)(struct Positions);
int (*is_flag_ptr)(struct Positions);
struct Positions board[SIZEX][SIZEY];
int curr_x;
int curr_y;
int running;
int gameover;
//int placed_flaggs;
char move_keys[8];