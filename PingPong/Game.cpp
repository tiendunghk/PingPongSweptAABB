#include "Game.h"
#define BUTTON_DOWN(obj, button) (obj.rgbButtons[button] & 0x80)
CGame * CGame::__instance = NULL;

/*
khởi tạo DirectX, tạo ra Direct3D device để render window, khởi tạo Sprite library để
render ảnh 2D
- hInst: Application instance handle
- hWnd: Application window handle
*/
void CGame::Init(HWND hWnd)
{
	LPDIRECT3D9 d3d = Direct3DCreate9(D3D_SDK_VERSION);

	this->hWnd = hWnd;

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;

	RECT r;
	GetClientRect(hWnd, &r);	// retrieve Window width & height 

	d3dpp.BackBufferHeight = r.bottom + 1;
	d3dpp.BackBufferWidth = r.right + 1;

	d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddv);

	if (d3ddv == NULL)
	{
		return;
	}

	d3ddv->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);

	// Initialize sprite helper from Direct3DX helper library
	D3DXCreateSprite(d3ddv, &spriteHandler);

}

/*
Utility function to wrap LPD3DXSPRITE::Draw
viết lại hàm để đóng gói chức năng cho hàm Draw
*/
void CGame::Draw(float x, float y, LPDIRECT3DTEXTURE9 texture, int left, int top, int right, int bottom)
{
	D3DXVECTOR3 p(x, y, 0);
	RECT r;
	r.left = left;
	r.top = top;
	r.right = right;
	r.bottom = bottom;
	spriteHandler->Draw(texture, &r, NULL, &p, D3DCOLOR_XRGB(255, 255, 255)); //tham số 3 là vị trí xoay, ko cần

}

int CGame::IsKeyDown(int KeyCode)
{
	return (keyStates[KeyCode] & 0x80) > 0;
}


void CGame::InitKeyboard(LPKEYEVENTHANDLER handler)
{
	//tạo giá trị(khởi tạo) cho "di", sử dụng "di" chung cho bàn phím và chuột
	//vì initKeyBoard được gọi trước (trong main) nên câu lệnh khởi tạo "di" nằm trong hàm này
	HRESULT result = DirectInput8Create(
		GetModuleHandle(NULL),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&di,
		NULL);

	if (result != DI_OK)
	{
		return;
	}

	//Tạo giá trị cho "dikeyboard"
	result = di->CreateDevice(GUID_SysKeyboard, &dikeyboard, NULL);

	if (result != DI_OK)
	{
		return;
	}

	// Set the data format to "keyboard format" - một data format được xác định trước
	//
	// data format dùng để xác định rõ những controls nào trên thiết bị mà chúng ta muốn sử dụng
	//và how they should be reported
	//
	// và cho DirectInput biết rằng ta sẽ dùng 1 mảng 256 bytes để làm việc với IDirectInputDevice::GetDeviceState

	result = dikeyboard->SetDataFormat(&c_dfDIKeyboard);

	result = dikeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); //set mức độ điều khiển


	// IMPORTANT STEP TO USE BUFFERED DEVICE DATA!
	//
	// DirectInput uses unbuffered I/O (buffer size = 0) by default.
	// If you want to read buffered data, you need to set a nonzero
	// buffer size.
	//
	// Set the buffer size to DINPUT_BUFFERSIZE (defined above) elements.
	//
	// The buffer size is a DWORD property associated with the device.
	//Đoạn setProperty cho keyboard bên dưới này không cần dùng cho mouse
	DIPROPDWORD dipdw;
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = KEYBOARD_BUFFER_SIZE; // Arbitary buffer size

	result = dikeyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

	result = dikeyboard->Acquire(); //Acquire bàn phím
	if (result != DI_OK)
	{
		MessageBox(hWnd, L"accquire keyboard fail", L"Lỗi", MB_OK);
		return;
	}

	this->keyHandler = handler;
}
void CGame::InitMouse(HWND hwnd)
{
	//tạo giá trị cho "dimouse"
	HRESULT result = di->CreateDevice(GUID_SysMouse, &dimouse, NULL);
	//set dataFormat như bên keyboard
	result = dimouse->SetDataFormat(&c_dfDIMouse);
	if (result != DI_OK)
		return;
	//set the cooperative level và ẩn con trỏ chuột
	result = dimouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND);
	//result = dimouse->SetCooperativeLevel(hwnd,DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	if (result != DI_OK)
		return;

	//acquire the mouse
	result = dimouse->Acquire();

}
void CGame::ProcessMouse()
{
	HRESULT result;
	//lấy trạng thái của mouse lưu vào mouse_state để sử dụng
	result = dimouse->GetDeviceState(sizeof(mouse_state), (LPVOID)&mouse_state);
	if (FAILED(result))
	{
		// nếu mouse bị mất focus thì acquire mouse lại 1 lần nữa
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			HRESULT h = dimouse->Acquire();
			if (h != DI_OK)
			{
				return;
			}
		}
	}
	
}
void CGame::ProcessKeyboard()
{
	HRESULT hr;

	//get trạng thái keyboard và lưu vào mảng keyStates
	hr = dikeyboard->GetDeviceState(sizeof(keyStates), keyStates);
	if (FAILED(hr))
	{
		//nếu keyboard bị mất focus thì acquire nó lại 1 lần nữa
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			HRESULT h = dikeyboard->Acquire();
			if (h != DI_OK)
			{
				return;
			}
		}
		else
		{
			return;
		}
	}

	keyHandler->KeyState((BYTE *)&keyStates);

#pragma region Đoạn này khó quá bỏ qua đi :V
	// Collect all buffered events
	DWORD dwElements = KEYBOARD_BUFFER_SIZE;
	hr = dikeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), keyEvents, &dwElements, 0);
	if (FAILED(hr))
	{
		return;
	}

	// Scan through all buffered events, check if the key is pressed or released
	for (DWORD i = 0; i < dwElements; i++)
	{
		int KeyCode = keyEvents[i].dwOfs;
		int KeyState = keyEvents[i].dwData;
		if ((KeyState & 0x80) > 0)
			keyHandler->OnKeyDown(KeyCode);
		else
			keyHandler->OnKeyUp(KeyCode);
	}
#pragma endregion
	
}

//Lấy vị trí mouse
int CGame::Mouse_X()
{
	return mouse_state.lX;
}
int CGame::Mouse_Y()
{
	return mouse_state.lY;
}
//Xác định nút nhấn của mouse : trong project này ko sử dụng tới
int CGame::Mouse_Button(int button)
{
	return BUTTON_DOWN(mouse_state, button);
}

CGame::~CGame()
{
	if (spriteHandler != NULL) spriteHandler->Release();
	if (backBuffer != NULL) backBuffer->Release();
	if (d3ddv != NULL) d3ddv->Release();
	if (d3d != NULL) d3d->Release();
	//kill keyboard
	if (dikeyboard != NULL)
	{
		dikeyboard->Unacquire();
		dikeyboard->Release();
		dikeyboard = NULL;
	}
	//kill mouse
	if (dimouse != NULL)
	{
		dimouse->Unacquire();
		dimouse->Release();
		dimouse = NULL;
	}
	if (di != NULL) di->Release();
}


CGame *CGame::GetInstance()
{
	if (__instance == NULL) __instance = new CGame();
	return __instance;
}
