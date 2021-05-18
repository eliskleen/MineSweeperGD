
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
#define sameKeyMax 15


void gotoxy(int x, int y);
void createBoard(struct Positions b[sizeX][sizeY]);
void drawField();
struct Player movePlayer(struct Player p, char moveCh);
void drawPos(struct Positions p);
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
void reset_game();


int (*isBombPtr)(struct Positions);
int (*isFlagPtr)(struct Positions);
struct Positions board[sizeX][sizeY];
int currX;
int currY;
int running;
int gameover;
int placed_flaggs;