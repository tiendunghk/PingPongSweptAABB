#pragma once
#include "GameObject.h"
#include "Game.h"
#define PADDLE_MOVE_SPEED		0.5f //Vận tốc paddle

//các trạng thái paddle
#define PADDLE_STATE_IDLE		0	
#define PADDLE_STATE_MOVE_UP	1
#define PADDLE_STATE_MOVE_DOWN	2

#define PADDLE_ANI			0
#define SCREEN_WIDTH		840
#define SCREEN_HEIGHT		(680-35)
#define PADDLE_SIZE_WIDTH	22
#define PADDLE_SIZE_HEIGHT	90

class CPaddle :public CGameObject
{
	bool controlByMouse; //=true nếu đối tượng được điều khiển = mouse
	int paddle_ani;
public:

	CPaddle(int mouse);
	void Update(DWORD dt);
	void Render();
	void SetState(int state);
	int getWidth() { return PADDLE_SIZE_WIDTH; }
	int getHeight() { return PADDLE_SIZE_HEIGHT; }
};