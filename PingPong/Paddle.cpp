#include "Paddle.h"
#include <dinput.h>

CPaddle::CPaddle(int mouse):CGameObject::CGameObject()
{
	if (mouse == 0)
	{
		controlByMouse = false;
		paddle_ani = 0;
	}
	else
	{
		controlByMouse = true;
		paddle_ani = 1;
	}
}

void CPaddle::Update(DWORD dt)
{
	if (!controlByMouse)
		//điều khiển = keyboard thì cộng vận tốc như bình thường
		CGameObject::Update(dt);
	else
	{
		y += (CGame::GetInstance()->Mouse_Y());
	}
	//kiểm tra va chạm 
	//biên trên
	if (y < EDGE_ABOVE) 
		y = EDGE_ABOVE;
	//biên dưới
	if (y > SCREEN_HEIGHT-PADDLE_SIZE_HEIGHT) 
		y = SCREEN_HEIGHT - PADDLE_SIZE_HEIGHT;
}

void CPaddle::Render()
{
	animations[paddle_ani]->Render(x, y);
}

//chỉ dùng cho paddle điều khiển =keyboard
void CPaddle::SetState(int state)
{
	switch (state)
	{
	case PADDLE_STATE_MOVE_DOWN:
		this->vy = PADDLE_MOVE_SPEED;
		break;
	case PADDLE_STATE_MOVE_UP:
		this->vy = -PADDLE_MOVE_SPEED;
		break;
	default:
		this->vy = 0;
		break;
	}
}