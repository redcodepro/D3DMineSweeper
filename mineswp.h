#include "main.h"

#define MAX_SIZE_X		127
#define MAX_SIZE_Y		127

#define STATE_OPEN		0x100
#define STATE_FLAG		0x200
#define STATE_BOOM		0x400

using namespace std;

enum blockStates
{
	stNone = 0,
	stAround1,
	stAround2,
	stAround3,
	stAround4,
	stAround5,
	stAround6,
	stAround7,
	stAround8,
	stMine,
};

enum gameStates
{
	gstActive = 0,
	gstReady,
	gstLose,
	gstWin,
};

class MineSweeper
{
	short pole[128][128]; // 256 -> 128 | Cause: EXCEPTION_STACK_OVERFLOW
	short size_x;
	short size_y;

	short mines;
	short flags;

	BYTE nGameState;

	bool CheckWin();
	void Clean(int, int);
	void FieldFill();
	void FieldClear();
	BYTE CheckNear(short, short, short);
	void CheckNOpenNear(short, short);
	void GameOver(short, short);
	void Win();
	bool Check(short, short, short);

	DWORD start_time;
	int timer;

	int gPosX;
	int gPosY;
public:
	int GetTime()
	{
		if (nGameState == gstActive)
			timer = (GetTickCount() - start_time) / 1000;
		return timer;
	}

	MineSweeper(short size_x, short size_y, short mines);
	void StartGame();
	void MouseClick(unsigned int);
	void SetPos(int x, int y);
	
	int GetXPos() { return gPosX; }
	int GetYPos() { return gPosY; }
	int GetXSize() { return size_x; }
	int GetYSize() { return size_y; }
	int GetFlagNum() { return flags; }
	BYTE GetGameState() { return nGameState; }

	short getBlock(short x, short y)
	{
		return pole[x][y];
	}
};