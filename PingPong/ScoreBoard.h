#pragma once

#include <Windows.h>
#include <d3dx9.h>
#include <vector>
#include "Sprites.h"

#define SCORE_BOARD_ANI 0
#define SCREEN_WIDTH	840
#define SCREEN_HEIGHT	680
using namespace std;
class CScoreBoard 
{
protected:
	static vector<LPANIMATION> animations; //mảng animation cho scoreboard
	
public:
	//Điểm 2 player
	static int score1;
	static int score2;
	CScoreBoard();
	void Render(int x, int countdown);
	static void AddAnimation(int aniId); 
};
