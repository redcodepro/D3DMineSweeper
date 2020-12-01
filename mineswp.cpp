#include "main.h"

MineSweeper::MineSweeper(short x, short y, short m)
	: size_x(x), size_y(y), mines(m)
{
	if (x >= MAX_SIZE_X)
		size_x = MAX_SIZE_X;
	if (y >= MAX_SIZE_Y)
		size_y = MAX_SIZE_Y;
	if (m >= size_x * size_y)
		mines = size_x * size_y - 1;
	StartGame();
}

void MineSweeper::FieldClear()
{
	for (short x = 0; x < size_x; ++x)
		for (short y = 0; y < size_y; ++y)
			pole[x][y] = NULL;
}

void MineSweeper::FieldFill()
{
	/* Минирование поля */
	for (int i = 0; i < mines; ++i)
	{
		srand(time(0));
		while (1)
		{
			int x = rand() % size_x;
			int y = rand() % size_y;
			if (pole[x][y] == stNone)
			{
				pole[x][y] = stMine;
				break;
			}
		}
	}
	/* Заполнение клеток цифрами */
	for (int x = 0; x < size_x; ++x)
		for (int y = 0; y < size_y; ++y)
			if (pole[x][y] == stNone)
				pole[x][y] = CheckNear(stMine, x, y);
}

BYTE MineSweeper::CheckNear(short type, short x, short y)
{
	BYTE n = 0;
	if (Check(type, x - 1, y - 1))
		n++;
	if (Check(type, x - 1, y))
		n++;
	if (Check(type, x - 1, y + 1))
		n++;
	if (Check(type, x, y - 1))
		n++;
	if (Check(type, x, y + 1))
		n++;
	if (Check(type, x + 1, y - 1))
		n++;
	if (Check(type, x + 1, y))
		n++;
	if (Check(type, x + 1, y + 1))
		n++;
	return n;
}

void MineSweeper::StartGame()
{
	FieldClear();
	FieldFill();
	nGameState = gstReady;
	flags = mines;
	start_time = 0;
	timer = 0;
}

void MineSweeper::MouseClick(unsigned int prm)
{
	if (gPosX < 0 || gPosX >= size_x) return;
	if (gPosY < 0 || gPosY >= size_y) return;
	if (nGameState == gstReady)
	{
		nGameState = gstActive;
		start_time = GetTickCount();
	}
	if (nGameState != gstActive) return;
	switch (prm)
	{
	case WM_LBUTTONUP:
		Clean(gPosX, gPosY);
		break;
	case WM_RBUTTONUP:
		if (!(pole[gPosX][gPosY] & STATE_OPEN))
		{
			pole[gPosX][gPosY] ^= STATE_FLAG;
			if (pole[gPosX][gPosY] & STATE_FLAG)
				flags--;
			else
				flags++;
		}
		break;
	case WM_LBUTTONDBLCLK:
		if ((pole[gPosX][gPosY] & STATE_OPEN) && LOBYTE(pole[gPosX][gPosY]) > 0 && LOBYTE(pole[gPosX][gPosY]) < 9)
			CheckNOpenNear(gPosX, gPosY);
		break;
	default:
		break;
	}
}

void MineSweeper::Clean(int x, int y)
{
	if (x < 0 || x >= size_x) return;
	if (y < 0 || y >= size_y) return;
	if (!(pole[x][y] & STATE_OPEN))
	{
		pole[x][y] |= STATE_OPEN;
		if (pole[x][y] & STATE_FLAG)
		{
			pole[x][y] &= ~STATE_FLAG;
			flags++;
		}
		if (Check(stNone, x, y))
		{
			Clean(x - 1, y - 1);
			Clean(x - 1, y);
			Clean(x - 1, y + 1);
			Clean(x, y - 1);
			Clean(x, y + 1);
			Clean(x + 1, y - 1);
			Clean(x + 1, y);
			Clean(x + 1, y + 1);
		}
		else
		{
			if (Check(stNone, x - 1, y - 1))
				Clean(x - 1, y - 1);
			if (Check(stNone, x - 1, y))
				Clean(x - 1, y);
			if (Check(stNone, x - 1, y + 1))
				Clean(x - 1, y + 1);
			if (Check(stNone, x, y - 1))
				Clean(x, y - 1);
			if (Check(stNone, x, y + 1))
				Clean(x, y + 1);
			if (Check(stNone, x + 1, y - 1))
				Clean(x + 1, y - 1);
			if (Check(stNone, x + 1, y))
				Clean(x + 1, y);
			if (Check(stNone, x + 1, y + 1))
				Clean(x + 1, y + 1);
		}
		/* Проверка для полу-автоматического копания */
		if (Check(stMine, x, y))
			GameOver(x, y);
		else
			if (CheckWin())
				Win();
	}

}

// бывает
void MineSweeper::SetPos(int x, int y)
{
	gPosX = x;
	gPosY = y;
}

bool MineSweeper::CheckWin()
{
	for (short x = 0; x < size_x; ++x)
		for (short y = 0; y < size_y; ++y)
			if (!(pole[x][y] & STATE_OPEN) && (LOBYTE(pole[x][y]) != stMine))
				return false;
	return true;
}

void MineSweeper::CheckNOpenNear(short x, short y)
{
	if (CheckNear(STATE_FLAG, x, y) == LOBYTE(pole[x][y]))
	{
		if (!Check(STATE_FLAG, x - 1, y - 1))
			Clean(x - 1, y - 1);
		if (!Check(STATE_FLAG, x - 1, y))
			Clean(x - 1, y);
		if (!Check(STATE_FLAG, x - 1, y + 1))
			Clean(x - 1, y + 1);
		if (!Check(STATE_FLAG, x, y - 1))
			Clean(x, y - 1);
		if (!Check(STATE_FLAG, x, y + 1))
			Clean(x, y + 1);
		if (!Check(STATE_FLAG, x + 1, y - 1))
			Clean(x + 1, y - 1);
		if (!Check(STATE_FLAG, x + 1, y))
			Clean(x + 1, y);
		if (!Check(STATE_FLAG, x + 1, y + 1))
			Clean(x + 1, y + 1);
	}
}

void MineSweeper::GameOver(short x, short y)
{
	nGameState = gstLose;
	pole[x][y] |= STATE_BOOM;
	/* Открытие мин */
	for (short x = 0; x < size_x; ++x)
		for (short y = 0; y < size_y; ++y)
			if (LOBYTE(pole[x][y]) == stMine)
				pole[x][y] |= STATE_OPEN;
}

void MineSweeper::Win()
{
	nGameState = gstWin;
	for (short x = 0; x < size_x; ++x)
		for (short y = 0; y < size_y; ++y)
			if (!(pole[x][y] & STATE_OPEN) && (LOBYTE(pole[x][y]) == stMine))
				pole[x][y] |= STATE_FLAG;
	flags = 0;
};

bool MineSweeper::Check(short state, short x, short y)
{
	/* Проверка на выход за пределы */
	if (x < 0 || y < 0 || x >= size_x || y >= size_y)
		return false;
	/* Проверка флагов */
	if (state >= STATE_OPEN)
		return (pole[x][y] & state);
	/* Проверка содержимого */
	return (LOBYTE(pole[x][y]) == state);
}