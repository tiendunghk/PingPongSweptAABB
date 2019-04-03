#pragma once
#include "GameObject.h"
#include "Game.h"
#include "ScoreBoard.h"
#include "Paddle.h"
#define BALL_MOVE_SPEED		8.0f

//Kích thước màn hình đã fix, do window set không đúng kích thước
#define SCREEN_WIDTH	(840-10)
#define SCREEN_HEIGHT	(680-35)
#define SIZE_BALL		25
class CBall :public CGameObject
{
	int numFrame;
public:
	CBall();
	void Update(DWORD dt);
	void Render();
	void UpdateFail();
	int getWidth() { return SIZE_BALL; }
	int getHeight() { return SIZE_BALL; }
	int processCollision(CPaddle* block);
};