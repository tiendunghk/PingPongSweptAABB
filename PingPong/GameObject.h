#pragma once

#include <Windows.h>
#include <d3dx9.h>
#include <vector>
#include "Sprites.h"

#define EDGE_ABOVE	170

using namespace std;

class CGameObject
{
protected:
	//Tọa độ object
	float x;
	float y;

	//vận tốc
	float vx;
	float vy;

	int state; //trạng thái của object

	//chứa 1 loạt các animation của các texture, dùng chung cho tất cả các đối tượng thuộc GameObject
	static vector<LPANIMATION> animations; 

public:
	void SetPosition(float x, float y) { this->x = x, this->y = y; } //set vị trí cho object
	float getX() { return this->x; }	//lấy tọa độ x
	float getY() { return this->y; }	//lấy tọa độ y
	void setX(float xx) { x = xx; }
	void setY(float yy) { y = yy; }
	void SetSpeed(float vx, float vy) { this->vx = vx, this->vy = vy; } //set vận tốc

	void SetState(int state) { this->state = state; } //set trạng thái
	int GetState() { return this->state; } //lấy trạng thái

	static void AddAnimation(int aniId); //thêm 1 animation vào mảng static animations ở trên
	float getVelocityX() { return vx; }
	float getVelocityY() { return vy; }
	CGameObject();

	void Update(DWORD dt);
	void Render(); 
	~CGameObject();
};