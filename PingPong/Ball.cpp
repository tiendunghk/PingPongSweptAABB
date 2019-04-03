#include "Ball.h"
#include <cstdlib>
#include <ctime>

CBall::CBall()
{
	vx = BALL_MOVE_SPEED;
	vy = BALL_MOVE_SPEED;
	//Frame của ball là 2->3->4->5, bắt đầu từ 2
	numFrame = 2;
}

void CBall::Update(DWORD dt)
{
	//cộng cho vận tốc
	x += vx;
	y += vy;
#pragma region Xử lý chạm 4 biên
	if (x > SCREEN_WIDTH - SIZE_BALL)
	{
		x = SCREEN_WIDTH- SIZE_BALL;
		vx *= -1;

	}
	else if (x < 0)
	{
		x = 0;
		vx *= -1;
	}
	if (y >SCREEN_HEIGHT - SIZE_BALL)
	{
		y = SCREEN_HEIGHT - SIZE_BALL;
		vy *= -1;
	}
	else if (y < EDGE_ABOVE)
	{
		y = EDGE_ABOVE;
		vy *= -1;
	}
#pragma endregion

}

float rand(int x, int y)
{
	//srand(time(NULL));
	float a = rand() % (y - x) + x;
	return a;
}

void CBall::UpdateFail()
{
	
	//nếu x chạm cạnh trái
	if (x <= 0)
	{
		//update score 1
		CScoreBoard::score1 -= 1; 
		if (CScoreBoard::score1 < 0) CScoreBoard::score1 = 0;
		//đặt lại ball ở giữa sân bóng
		y = rand(SCREEN_HEIGHT / 2 - 200, SCREEN_HEIGHT / 2 + 200);
		x = SCREEN_WIDTH / 2;

		return;
	}
	//xử lý score 2 tương tự score 1
	if(x+SIZE_BALL>SCREEN_WIDTH-5) 
	{
		CScoreBoard::score2 -= 1;
		if (CScoreBoard::score2 < 0) CScoreBoard::score2 = 0;
		y = rand(SCREEN_HEIGHT / 2 - 200, SCREEN_HEIGHT / 2 + 200);
		x = SCREEN_WIDTH / 2;
		return;
	}
	
}

void CBall::Render()
{
	//render từ fram từ 2->5
	animations[numFrame]->Render(x, y);
	numFrame++;
	if (numFrame > 5) numFrame = 2;
}


#pragma region Xử lý va chạm SWEPT AABB
float SweptAABB(CBall b1, CPaddle b2, float& normalx, float& normaly)
{
#pragma region Tìm khoảng cách gần nhất, xa nhất để va chạm giữa 2 đối tượng
	float xInvEntry, yInvEntry; //khoảng cách gần nhất giữa 2 đối tượng
	float xInvExit, yInvExit; //khoảng cách xa nhất giữa 2 đối tượng
							  // tìm khoảng cách giữa 2 vật thể ở cạnh gần và cạnh xa
	if (b1.getVelocityX() > 0.0f)
	{
		xInvEntry = b2.getX() - (b1.getX() + b1.getWidth());
		xInvExit = (b2.getX() + b2.getWidth()) - b1.getX();
	}
	else
	{
		xInvEntry = (b2.getX() + b2.getWidth()) - b1.getX();
		xInvExit = b2.getX() - (b1.getX() + b1.getWidth());
	}
	if (b1.getVelocityY() > 0.0f)
	{
		yInvEntry = b2.getY() - (b1.getY() + b1.getHeight());
		yInvExit = (b2.getY() + b2.getHeight()) - b1.getY();
	}
	else
	{
		yInvEntry = (b2.getY() + b2.getHeight()) - b1.getY();
		yInvExit = b2.getY() - (b1.getY() + b1.getHeight());
	}
#pragma endregion

#pragma region Tìm thời gian va chạm
	float xEntry, yEntry; //Thời gian còn lại trước khi va chạm
	float xExit, yExit;	//Thời gian đến khi va chạm xong (rời khỏi nhau)
	if (b1.getVelocityX() == 0.0f)
	{
		xEntry = -std::numeric_limits<float>::infinity();
		xExit = std::numeric_limits<float>::infinity();
	}
	else
	{
		//thời gian=quãng đường/vận tốc
		xEntry = xInvEntry / b1.getVelocityX();
		xExit = xInvExit / b1.getVelocityX();
	}
	if (b1.getVelocityY() == 0.0f)
	{
		yEntry = -std::numeric_limits<float>::infinity();
		yExit = std::numeric_limits<float>::infinity();
	}
	else
	{
		yEntry = yInvEntry / b1.getVelocityY();
		yExit = yInvExit / b1.getVelocityY();
	}
#pragma endregion
	//Tìm thời gian đến khi va chạm và kết thúc va chạm
	float entryTime = max(xEntry, yEntry);
	float exitTime = min(xExit, yExit);
	//Kiểm tra va chạm liệu có xảy ra?
	if (entryTime > exitTime || xEntry < 0.0f && yEntry <0.0f || xEntry > 1.0f || yEntry > 1.0f)
	{
		normalx = 0.0f;
		normaly = 0.0f;
		return 1.0f;
	}
	else // if there was a collision
	{
		// tính vector pháp tuyến của bề mặt va chạm
		if (xEntry > yEntry)
		{
			if (xInvEntry < 0.0f)
			{
				normalx = 1.0f;
				normaly = 0.0f;
			}
			else
			{
				normalx = -1.0f;
				normaly = 0.0f;
			}
		}
		else
		{
			if (yInvEntry < 0.0f)
			{
				normalx = 0.0f;
				normaly = 1.0f;
			}
			else
			{
				normalx = 0.0f;
				normaly = -1.0f;
			}
		}
		// return the time of collision
		return entryTime;
	}
}

int CBall::processCollision(CPaddle* block)
{
	float normalx, normaly;
	float collisiontime = SweptAABB(*this, *block, normalx, normaly);
	if (collisiontime == 1) return 0;
	this->x += this->vx * collisiontime;
	this->y += this->vy* collisiontime;
	float remainingtime = 1.0f - collisiontime;

	// deflect
	if (abs(normalx) > 0.0001f)
		this->vx *= -1;
	if (abs(normaly) > 0.0001f)
		this->vy *= -1;

	this->x += this->vx * remainingtime;
	this->y += this->vy * remainingtime;

	return 1;
}
#pragma endregion



