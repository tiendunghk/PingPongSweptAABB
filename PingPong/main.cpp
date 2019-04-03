/* =============================================================

================================================================ */

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <cmath>

#include "Game.h"
#include "GameObject.h"
#include "Textures.h"
#include "Paddle.h"
#include "Ball.h"
#include "ScoreBoard.h"

#define WINDOW_CLASS_NAME L"Ping Pong Game"
#define MAIN_WINDOW_TITLE L"Ping Pong"

#define BACKGROUND_COLOR D3DCOLOR_XRGB(200, 200, 255) //Màu để clear màn hình mỗi lần render lại
#define SCREEN_WIDTH	840 //Chiều rộng window
#define SCREEN_HEIGHT	680	//Chiều cao window
#define EDGE_ABOVE		170	//biên phía trên (=chiều cao score board)
#define MAX_FRAME_RATE	30	//dùng để set tỉ lệ số frame/giây

//Tất cả các ID của các ảnh được sử dụng trong game
//chính là chỉ số của phần tử đó trong mảng textures[]
#pragma region ID textures
#define ID_TEX_PADDLE1	0	//ảnh thanh paddle 1
#define	ID_TEX_PADDLE2	1	//ảnh thanh paddle 2
#define	ID_TEX_BALL		2	//ảnh trái banh
#define	ID_TEX_SCORE	3	//ảnh score từ số 0->9
#define ID_TEX_SB		4	//ảnh scoreboard ở phía trên
#define ID_TEX_BG		5	//ảnh background (để có 2 vạch đỏ)
#define ID_TEX_WIN		6	//ảnh "người chiến thắng"=>endgame
#define ID_TEX_WHITE	7
#pragma endregion

#pragma region objects and variables
//tất cả các đối tượng và biến được sử dụng trong game
CGame *game;
CPaddle *paddle1;
CPaddle *paddle2;
CBall *ball;
CScoreBoard *scoreboard;
bool endGame;// =true khi 1 trong 2 bên có điểm =0
int start;
#pragma endregion

#pragma region Lớp CSampleKeyHander để sử lý trạng thái bàn phím
class CSampleKeyHander : public CKeyEventHandler
{
	virtual void KeyState(BYTE *states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
};

CSampleKeyHander * keyHandler;

void CSampleKeyHander::OnKeyDown(int KeyCode)
{
}

void CSampleKeyHander::OnKeyUp(int KeyCode)
{

}

void CSampleKeyHander::KeyState(BYTE *states)
{
	if (game->IsKeyDown(DIK_DOWN))
		paddle1->SetState(PADDLE_STATE_MOVE_DOWN);
	else if (game->IsKeyDown(DIK_UP))
		paddle1->SetState(PADDLE_STATE_MOVE_UP);
	else paddle1->SetState(PADDLE_STATE_IDLE);
}
#pragma endregion

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

/*
Hàm loadResources để Load tất cả tài nguyên game gồm:
load textures, sprites, animations,.. và khởi tạo các biến
*/
void LoadResources()
{
	start = 0;
	endGame = false; //game chưa kết thúc
	//khởi tạo điểm 2 bên ban đầu=10

	CTextures * textures = CTextures::GetInstance(); //tạo ra 1 mảng các texture

	//mảng sprites để lưu từng sprite cho TẤT CẢ các texture dùng trong game
	CSprites * sprites = CSprites::GetInstance();	
	CAnimations * animations = CAnimations::GetInstance();

#pragma region paddle1
	//thêm 1 texture "paddle1.png" vào mảng textures (ở chỉ số thứ "ID_TEX_PADDLE1" của mảng)
	textures->Add(ID_TEX_PADDLE1, L"textures\\paddle1.png", D3DCOLOR_XRGB(176, 224, 248));
	//tạo 1 texture để get texture mới được thêm vào: textures[ID_TEX_PADDLE1]
	LPDIRECT3DTEXTURE9 texPaddle1 = textures->Get(ID_TEX_PADDLE1);
	//thêm 1 sprite vào mảng sprites(ở chỉ số thứ 0), vị trí cắt: góc trái trên là (0,0), phải dưới là (22,90) 
	sprites->Add(0, 0, 0, 22, 90, texPaddle1);

	LPANIMATION ani;
	ani = new CAnimation(100);
	ani->Add(0);
	animations->Add(0, ani);

	paddle1 = new CPaddle(0);
	CPaddle::AddAnimation(0);

	paddle1->SetPosition(50.0f, EDGE_ABOVE); //set vị trí bắt đầu game cho đối tượng
#pragma endregion

#pragma region paddle2
	textures->Add(ID_TEX_PADDLE2, L"textures\\paddle2.png", D3DCOLOR_XRGB(176, 224, 248));
	//CSprites * sprites = CSprites::GetInstance();
	//CAnimations * animations = CAnimations::GetInstance();

	LPDIRECT3DTEXTURE9 texPaddle2 = textures->Get(ID_TEX_PADDLE2);
	sprites->Add(1, 0, 0, 22,90, texPaddle2);


//	LPANIMATION ani;
	ani = new CAnimation(100);
	ani->Add(1);
	animations->Add(1, ani);

	paddle2 = new CPaddle(1);
	CPaddle::AddAnimation(1);

	paddle2->SetPosition(SCREEN_WIDTH-95.0f, EDGE_ABOVE);
#pragma endregion

#pragma region ball
	textures->Add(ID_TEX_BALL, L"textures\\ball.png", D3DCOLOR_XRGB(176, 224, 248));
	LPDIRECT3DTEXTURE9 texBall = textures->Get(ID_TEX_BALL);
	int x = 25;
	sprites->Add(2, 0, 0, x,x, texBall);
	sprites->Add(3, x, 0, x*2, x, texBall);
	sprites->Add(4, x, x, x*2, x*2, texBall);
	sprites->Add(5, 0, x, x, x*2, texBall);

	ani = new CAnimation(100);
	ani->Add(2);
	animations->Add(2, ani);

	ani = new CAnimation(100);
	ani->Add(3);
	animations->Add(3, ani);

	ani = new CAnimation(100);
	ani->Add(4);
	animations->Add(4, ani);

	ani = new CAnimation(100);
	ani->Add(5);
	animations->Add(5, ani);

	ball = new CBall();
	CBall::AddAnimation(2);
	CBall::AddAnimation(3);
	CBall::AddAnimation(4);
	CBall::AddAnimation(5);

	ball->SetPosition(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
#pragma endregion

#pragma region scoreboard

	textures->Add(ID_TEX_SCORE, L"textures\\score.png", D3DCOLOR_XRGB(176, 224, 248));
	LPDIRECT3DTEXTURE9 texScore = textures->Get(ID_TEX_SCORE);
	sprites->Add(6, 0, 0, 45,59, texScore); //số 0
	sprites->Add(7, 103,1, 128, 59, texScore); //số 1
	sprites->Add(8, 187, 1, 236, 59, texScore); //số 2
	sprites->Add(9, 293, 1, 343, 59, texScore); //số 3
	sprites->Add(10, 400, 1, 446, 59, texScore); //số 4
	sprites->Add(11, 505, 1, 552, 59, texScore); //số 5
	sprites->Add(12, 611, 1, 655, 59, texScore); //số 6
	sprites->Add(13, 711, 1, 758, 59, texScore); //số 7
	sprites->Add(14, 814, 1, 861, 59, texScore); //số 8
	sprites->Add(15, 920, 1, 967, 59, texScore); //số 9
	ani = new CAnimation(100);
	ani->Add(6);
	animations->Add(6, ani);

	ani = new CAnimation(100);
	ani->Add(7);
	animations->Add(7, ani);

	ani = new CAnimation(100);
	ani->Add(8);
	animations->Add(8, ani);

	ani = new CAnimation(100);
	ani->Add(9);
	animations->Add(9, ani);

	ani = new CAnimation(100);
	ani->Add(10);
	animations->Add(10, ani);

	ani = new CAnimation(100);
	ani->Add(11);
	animations->Add(11, ani);

	ani = new CAnimation(100);
	ani->Add(12);
	animations->Add(12, ani);

	ani = new CAnimation(100);
	ani->Add(13);
	animations->Add(13, ani);

	ani = new CAnimation(100);
	ani->Add(14);
	animations->Add(14, ani);

	ani = new CAnimation(100);
	ani->Add(15);
	animations->Add(15, ani);

	scoreboard = new CScoreBoard();
	CScoreBoard::AddAnimation(6);
	CScoreBoard::AddAnimation(7);
	CScoreBoard::AddAnimation(8);
	CScoreBoard::AddAnimation(9);
	CScoreBoard::AddAnimation(10);
	CScoreBoard::AddAnimation(11);
	CScoreBoard::AddAnimation(12);
	CScoreBoard::AddAnimation(13);
	CScoreBoard::AddAnimation(14);
	CScoreBoard::AddAnimation(15);
	//---------------------------

	textures->Add(ID_TEX_SB, L"textures\\scoreboard.png", D3DCOLOR_XRGB(176, 224, 248));
	LPDIRECT3DTEXTURE9 texSB = textures->Get(ID_TEX_SB);
	sprites->Add(16, 0, 0, 840, 170, texSB);
	ani = new CAnimation(100);
	ani->Add(16);
	animations->Add(16, ani);
	//scoreboard = new CScoreBoard();
	CScoreBoard::AddAnimation(16);

#pragma endregion

#pragma region background
	textures->Add(ID_TEX_BG, L"textures\\background.png", D3DCOLOR_XRGB(176, 224, 248));
	LPDIRECT3DTEXTURE9 texBG = textures->Get(ID_TEX_BG);
	sprites->Add(17, 0, 0, 1180, 697, texBG); 

	ani = new CAnimation(100);
	ani->Add(17);
	animations->Add(17, ani);
	CScoreBoard::AddAnimation(17);
#pragma endregion

#pragma region win
	textures->Add(ID_TEX_WIN, L"textures\\win.png", D3DCOLOR_XRGB(176, 224, 248));
	LPDIRECT3DTEXTURE9 texWIN = textures->Get(ID_TEX_WIN);
	sprites->Add(18, 0, 0,376,223, texWIN);
	sprites->Add(19, 376, 0, 773,223, texWIN);

	ani = new CAnimation(100);
	ani->Add(18);
	animations->Add(18, ani);

	ani = new CAnimation(100);
	ani->Add(19);
	animations->Add(19, ani);
	CScoreBoard::AddAnimation(18);
	CScoreBoard::AddAnimation(19);
#pragma endregion

#pragma region start game
	textures->Add(ID_TEX_WHITE, L"textures\\white.png", D3DCOLOR_XRGB(176, 224, 248));
	LPDIRECT3DTEXTURE9 texWHITE = textures->Get(ID_TEX_WHITE);
	sprites->Add(20, 0, 0, 155, 148, texWHITE);
	//---------khoảng trắng
	ani = new CAnimation(100);
	ani->Add(20);
	animations->Add(20, ani);
	CScoreBoard::AddAnimation(20); //ani[14]
#pragma endregion


}


/*
Update world status for this frame
dt: time period between beginning of last frame and beginning of this frame
*/
void Update(DWORD dt)
{
	//update lại vị trí mới cho 2 paddle và ball
	paddle1->Update(dt);
	paddle2->Update(dt);

	if (!(ball->processCollision(paddle1)) && !(ball->processCollision(paddle2)))
		ball->Update(dt);

	ball->UpdateFail(); //xử lý nếu ball không được đỡ
}

/*
Render a frame
*/
void Render()
{
	LPDIRECT3DDEVICE9 d3ddv = game->GetDirect3DDevice();
	LPDIRECT3DSURFACE9 bb = game->GetBackBuffer();
	LPD3DXSPRITE spriteHandler = game->GetSpriteHandler();

	if (start == 2)
	{
		int count = 3;
		while (count >= 0)
		{
			while (d3ddv->BeginScene())
			{
				//bắt đầu vẽ lên buffer
				spriteHandler->Begin(D3DXSPRITE_ALPHABLEND);
				scoreboard->Render(6, 0);
				scoreboard->Render(5, count);
		
				//Kết thúc vẽ
				spriteHandler->End();
				d3ddv->EndScene();
				d3ddv->Present(NULL, NULL, NULL, NULL);
				count--;
				Sleep(1000);
			}

		}
		//Sleep(3000);
	}
	else
	if (d3ddv->BeginScene())
	{
		// Nếu 2 bên chưa thua thì render từ đầu=>clear mọi thứ bằng colorfill
		if (CScoreBoard::score1 != 0 && CScoreBoard::score2 != 0)
			d3ddv->ColorFill(bb, NULL, BACKGROUND_COLOR);
		//bắt đầu vẽ lên buffer
		spriteHandler->Begin(D3DXSPRITE_ALPHABLEND);

			//khi chưa end game, kiểm tra xem có score nào =0 hay không
			if (!endGame)
			{
				//nếu 1 trong 2 score=0, thì vẽ lần cuối: số 0 lên màn hình và end game
				if (CScoreBoard::score1 == 0)
				{
					scoreboard->Render(1, 0); //vẽ số 0 bên player 1
					endGame = true;
				}
				if (CScoreBoard::score2 == 0)
				{
					scoreboard->Render(2, 0); //vẽ số 0 bên player 2
					endGame = true;
				}
			}

			//nếu player 1 thua, vẽ ảnh player 2 win
			if (CScoreBoard::score1 == 0)
				scoreboard->Render(4, 0);
			//nếu player 2 thua, vẽ ảnh player 1 win
			else if (CScoreBoard::score2 == 0)
				scoreboard->Render(3, 0);
			else //nếu không thì render bình thường
			{
				scoreboard->Render(-1, 0); //vẽ background
				paddle1->Render();
				paddle2->Render();
				ball->Render();


				scoreboard->Render(0, 0); //vẽ scoreBoard

				//if (CScoreBoard::score1 >= 0)
				scoreboard->Render(1, 0);
				//if (CScoreBoard::score2 >= 0)
				scoreboard->Render(2, 0);
			}

		
		//Kết thúc vẽ
		spriteHandler->End();
		d3ddv->EndScene();
	}

	// Display back buffer content to the screen
	if(start!=2)
	d3ddv->Present(NULL, NULL, NULL, NULL);

	start++;
}

HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;

	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	HWND hWnd =
		CreateWindow(
			WINDOW_CLASS_NAME,
			MAIN_WINDOW_TITLE,
			WS_OVERLAPPEDWINDOW, // WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			NULL,
			NULL,
			hInstance,
			NULL);

	if (!hWnd)
	{
		DWORD ErrCode = GetLastError();
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

int Run(HWND hWnd)
{
	MSG msg;
	int done = 0;
	DWORD frameStart = GetTickCount();
	DWORD tickPerFrame = 100 / MAX_FRAME_RATE;

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) done = 1;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		DWORD now = GetTickCount();

		// dt: the time between (beginning of last frame) and now
		// this frame: the frame we are about to render
		DWORD dt = now - frameStart;

		if (dt >= tickPerFrame)
		{
			frameStart = now;

			//xử lý keyboard và mouse trước khi sử dụng
			game->ProcessKeyboard(); 
			game->ProcessMouse();

			if (CScoreBoard::score1 != 0 && CScoreBoard::score2 != 0) //nếu cả 2 player đều chưa thua thì mới update game tiếp
				Update(dt);
			Render();

			
		}
		else
			Sleep(tickPerFrame - dt); //nếu chưa tới thời điểm được phép render thì chờ 1 khoảng time

		//Kiểm tra nhấn phím ESC=> thoát game, đóng window game
		if (game->IsKeyDown(DIK_ESCAPE))
			PostMessage(hWnd, WM_DESTROY, 0, 0);
		
	}
	

	return 1;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//set vài thuộc tính cho windowgame rồi tạo window game
	HWND hWnd = CreateGameWindow(hInstance, nCmdShow, SCREEN_WIDTH, SCREEN_HEIGHT);

	game = CGame::GetInstance();
	game->Init(hWnd); //khởi tạo game

	keyHandler = new CSampleKeyHander();
	game->InitKeyboard(keyHandler); //khởi tạo bàn phím
	game->InitMouse(hWnd);			//khởi tạo chuột

	LoadResources(); //load tài nguyên game
	Run(hWnd);	//run game

	return 0;
}