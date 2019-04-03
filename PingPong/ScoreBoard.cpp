
#include "ScoreBoard.h"
vector<LPANIMATION> CScoreBoard::animations;

int CScoreBoard::score1;
int CScoreBoard::score2;
CScoreBoard::CScoreBoard()
{
	//Khởi tạo điểm 2 player =10
	score1 = 10;
	score2 = 10;
}
void CScoreBoard::Render(int x,int countdown)
{
	switch (x)
	{
	case -1: //vẽ background
		animations[11]->Render(0, 0);
		break;
	case 0: //vẽ scoreBoard
		animations[10]->Render(0, 0);
		break;
	case 1: //vẽ score 1
		if (score1 == 10)
		{
			animations[1]->Render(132, 90);
			animations[0]->Render(163, 90);
			return;
		}
		if (score1 == 1)
		{
			animations[1]->Render(157, 90);
			return;
		}
		animations[score1]->Render(146, 90);
		break;
	case 2: //vẽ score 2
		if (score2 == 10)
		{
			animations[1]->Render(132 + 498, 90);
			animations[0]->Render(163 + 498, 90);
			return;
		}
		if (score2 == 1)
		{
			animations[1]->Render(157 + 498, 90);
			return;
		}
		animations[score2]->Render(146 + 498, 90);
		break;
		
	case 3: //vẽ bảng player 1 win
		animations[12]->Render(SCREEN_WIDTH / 2-193, SCREEN_HEIGHT / 2-50);
		break;
	case 4://vẽ bảng player 2 win
		animations[13]->Render(SCREEN_WIDTH / 2-193, SCREEN_HEIGHT / 2-50);
		break;
	case 5:
		animations[countdown]->Render(SCREEN_WIDTH / 2 -25 , SCREEN_HEIGHT / 2 -20);
		break;
	default:
		animations[14]->Render(SCREEN_WIDTH / 2 -25, SCREEN_HEIGHT / 2-20);
		break;
	}
	

}
void CScoreBoard::AddAnimation(int aniId)
{
	LPANIMATION ani = CAnimations::GetInstance()->Get(aniId);
	animations.push_back(ani);
}
