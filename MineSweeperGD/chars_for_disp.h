#define CHAR_X  8
#define CHAR_Y 12
#define MAX_X 14
#define MAX_Y 10

typedef struct dispChar {
	char val;
	char pattern[CHAR_Y][CHAR_X];
} DISPCHAR;

DISPCHAR zero = {'0',{{0,0,0,0,0,0,0,0}
					 ,{0,0,1,1,1,1,0,0}
					 ,{0,1,1,0,0,1,1,0}
					 ,{0,1,0,0,0,0,1,0}
					 ,{0,1,0,0,0,1,1,0}
					 ,{0,1,0,0,1,0,1,0}
					 ,{0,1,0,1,0,0,1,0}
					 ,{0,1,1,0,0,0,1,0}
					 ,{0,1,0,0,0,0,1,0}
					 ,{0,1,1,0,0,1,1,0}
					 ,{0,0,1,1,1,1,0,0}
					 ,{0,0,0,0,0,0,0,0}}};
DISPCHAR one = {'1', {{0,0,0,0,0,0,0,0}
					 ,{0,0,0,1,1,0,0,0}
					 ,{0,0,1,0,1,0,0,0}
					 ,{0,1,0,0,1,0,0,0}
					 ,{0,0,0,0,1,0,0,0}
					 ,{0,0,0,0,1,0,0,0}
					 ,{0,0,0,0,1,0,0,0}
					 ,{0,0,0,0,1,0,0,0}
					 ,{0,0,0,0,1,0,0,0}
					 ,{0,0,0,0,1,0,0,0}
					 ,{0,1,1,1,1,1,1,0}
					 ,{0,0,0,0,0,0,0,0}}};
DISPCHAR two = {'2', {{0,0,0,0,0,0,0,0}
					 ,{0,0,0,1,1,1,0,0}
					 ,{0,0,1,0,0,1,1,0}
					 ,{0,1,0,0,0,0,1,0}
					 ,{0,0,0,0,0,0,1,0}
					 ,{0,0,0,0,0,1,0,0}
					 ,{0,0,0,0,1,0,0,0}
					 ,{0,0,0,1,0,0,0,0}
					 ,{0,0,1,0,0,0,0,0}
					 ,{0,1,1,0,0,0,0,0}
					 ,{0,1,1,1,1,1,1,0}
					 ,{0,0,0,0,0,0,0,0}}};
DISPCHAR three ={'3',{{0,0,0,0,0,0,0,0}
					 ,{0,1,1,1,1,1,0,0}
					 ,{0,0,0,0,0,1,1,0}
					 ,{0,0,0,0,0,0,1,0}
					 ,{0,0,0,0,0,1,1,0}
					 ,{0,0,1,1,1,1,0,0}
					 ,{0,0,0,0,0,1,1,0}
					 ,{0,0,0,0,0,0,1,0}
					 ,{0,0,0,0,0,0,1,0}
					 ,{0,0,0,0,0,0,1,0}
					 ,{0,1,1,1,1,1,0,0}
					 ,{0,0,0,0,0,0,0,0}}};
DISPCHAR four = {'4',{{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,1,1,0}
					 ,{0,0,0,0,1,0,1,0}
					 ,{0,0,0,1,0,0,1,0}
					 ,{0,0,1,0,0,0,1,0}
					 ,{0,1,0,0,0,0,1,0}
					 ,{0,1,1,1,1,1,1,1}
					 ,{0,0,0,0,0,0,1,0}
					 ,{0,0,0,0,0,0,1,0}
					 ,{0,0,0,0,0,0,1,0}
					 ,{0,0,0,0,0,0,1,0}
					 ,{0,0,0,0,0,0,0,0}}};
DISPCHAR doll = {'$',{{0,0,0,0,1,0,0,0}
					 ,{0,0,0,0,1,0,0,0}
					 ,{0,0,1,1,1,1,1,0}
					 ,{0,1,1,0,1,0,0,0}
					 ,{0,1,1,0,1,0,0,0}
					 ,{0,0,1,1,1,0,0,0}
					 ,{0,0,0,1,1,1,0,0}
					 ,{0,0,0,1,0,1,1,0}
					 ,{0,0,0,1,0,1,1,0}
					 ,{0,1,0,1,1,1,0,0}
					 ,{0,0,1,1,1,0,0,0}
					 ,{0,0,0,1,0,0,0,0}}};
DISPCHAR star = {'*',{{0,0,0,1,0,0,0,0}
					 ,{0,1,0,1,0,1,0,0}
					 ,{0,0,1,1,1,0,0,0}
					 ,{0,0,1,1,1,0,0,0}
					 ,{0,1,0,1,0,1,0,0}
					 ,{0,0,0,1,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}}};
DISPCHAR arrow = {'^', {{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,1,0,0,0}
					 ,{0,0,0,1,0,1,0,0}
					 ,{0,0,1,0,0,0,1,0}
					 ,{0,1,0,0,0,0,0,1}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}}};
DISPCHAR space = {' ', {{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}
					 ,{0,0,0,0,0,0,0,0}}};