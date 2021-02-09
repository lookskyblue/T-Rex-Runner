#include <Windows.h>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include "resource.h"
#include "OnGame.h"

enum OBJ_COOR { START_X = 100, START_Y = 00, END_X = 700, END_Y = 0 }; // y원래 200씩
enum FLOOR_TEXTURE_INFO {OFFSET_Y=5, MAX_TEXTURE= 25};
#define MOVE_SPEED 5
#define FRAME_SPEED 5

class GameObj {
private:
	int mGame_progress_speed;
	int mStart_coord_x;
	int mStart_coord_y;
	int mEnd_coord_x;
	int mEnd_coord_y;

public:
	GameObj() = default;

	GameObj(int startX, int startY, int endX, int endY, int gameSpeed)
		: mStart_coord_x(startX)
		, mStart_coord_y(startY)
		, mEnd_coord_x(endX)
		, mEnd_coord_y(endY)
		, mGame_progress_speed(gameSpeed)
	{}

	int GetGameSpeed() { return mGame_progress_speed; }
	int GetStartCoordX() { return mStart_coord_x; }
	int GetStartCoordY() { return mStart_coord_y; }
	int GetEndCoordX() { return mEnd_coord_x; }
	int GetEndCoordY() { return mEnd_coord_y; }
	void MoveX(int x) { mStart_coord_x -= x; };
	void SetX(int x) { mStart_coord_x = x; }
};

class Floor : public GameObj{
private:

public:
	Floor() = default;
	Floor(int startX, int startY, int endX, int endY, int gameSpeed)
		: GameObj(startX, startY,endX,endY, gameSpeed) {}
};

class FloorTexture :public GameObj {
private: 
public:
	FloorTexture(int startX, int startY, int endX, int endY, int gameSpeed)
		: GameObj(startX, startY, endX, endY, gameSpeed) {}
};

class TRex {
private:
public:

};

Floor g_floor;
std::vector<FloorTexture> g_floor_texture;

std::random_device g_rd;
std::mt19937 g_gen(g_rd());
std::uniform_int_distribution<int> g_x_start_offset(0, OBJ_COOR::END_X - OBJ_COOR::START_X);
std::uniform_int_distribution<int> g_x_end_offset(2, 6);
std::uniform_int_distribution<int> g_y_offset(2, 7);

void OnCreate(HWND hWnd)
{
	g_floor = Floor(OBJ_COOR::START_X, OBJ_COOR::START_Y, OBJ_COOR::END_X, OBJ_COOR::END_Y, FRAME_SPEED);
	g_floor_texture.reserve(FLOOR_TEXTURE_INFO::MAX_TEXTURE);

	for (size_t i = 0; i < g_floor_texture.capacity(); i++)
	{
		int start_offset_x = g_x_start_offset(g_gen);
		int end_offset_x = g_x_end_offset(g_gen);
		int startX = OBJ_COOR::START_X + start_offset_x;
		int y_offset = OBJ_COOR::START_Y + g_y_offset(g_gen);
		
		g_floor_texture.push_back(FloorTexture(startX, y_offset, end_offset_x, y_offset , FRAME_SPEED));
	}

	SetTimer(hWnd, 0, g_floor.GetGameSpeed(), NULL);
}

void OnTimer(HWND hWnd, HBITMAP* hBitMap, HINSTANCE* hInst)
{
	MoveObj();
	DrawObj(hWnd, hBitMap, hInst);
	InvalidateRect(hWnd, nullptr, false);
}

void OnKeyDown(HWND hWnd, WPARAM wParam, int* g_y)
{
	switch (wParam)
	{
	case VK_UP:
	{
		SetTimer(hWnd, 1, 5, NULL);
	}

	default:
		break;
	}

}

void UpDino(HWND hWnd, WPARAM wParam, int* g_y)
{
	if (*g_y <= 50)
	{
		KillTimer(hWnd, wParam);

		/*std::thread t([&hWnd]() { 
			std::this_thread::sleep_for(std::chrono::nanoseconds(100));
			SetTimer(hWnd, 2, 5, NULL);
			});


		t.detach();*/

		SetTimer(hWnd, 2, 5, NULL);
	}

	else
	{
		if (*g_y <= 65)
			*g_y -= 1;

		else
		*g_y -= 5;

	}
}

void DownDino(HWND hWnd, WPARAM wParam, int* g_y)
{
	if (*g_y >= 135)
		KillTimer(hWnd, wParam);

	else
	{
		*g_y += 5;
	}
}

void MoveObj()
{
	for (size_t i = 0; i < g_floor_texture.capacity(); i++)
	{
		int coorX = g_floor_texture[i].GetStartCoordX();

		if (coorX <= OBJ_COOR::START_X)
		{
			g_floor_texture[i].SetX(OBJ_COOR::END_X);
			
			continue;
		}

		g_floor_texture[i].MoveX(MOVE_SPEED);
	}
}

void DrawObj(HWND hWnd, HBITMAP* hBitMap, HINSTANCE* hInst)
{
	HBITMAP OldBitMap;
	HDC hdc, hMemDC;
	RECT crt;

	hdc = GetDC(hWnd);
	GetClientRect(hWnd, &crt);
	
	if (*hBitMap == nullptr) // or 역참조
		*hBitMap = CreateCompatibleBitmap(hdc, crt.right, crt.bottom);

	hMemDC = CreateCompatibleDC(hdc);
	OldBitMap = (HBITMAP)SelectObject(hMemDC, *hBitMap);

	FillRect(hMemDC, &crt, GetSysColorBrush(COLOR_WINDOW));
	MoveToEx(hMemDC, g_floor.GetStartCoordX(), g_floor.GetStartCoordY(), NULL);
	LineTo(hMemDC, g_floor.GetEndCoordX(), g_floor.GetEndCoordY());

	//DinoBitMap = LoadBitmap(*hInst, MAKEINTRESOURCE(Dino));
	//OldBitMap = (HBITMAP)SelectObject(hMemDC, DinoBitMap);

	for (size_t i = 0; i < g_floor_texture.capacity(); i++)
	{
		int startX = g_floor_texture[i].GetStartCoordX();
		int startY = g_floor_texture[i].GetStartCoordY();
		int endX = g_floor_texture[i].GetEndCoordX();
		int endY = g_floor_texture[i].GetEndCoordY();

		MoveToEx(hMemDC, startX, startY, NULL);
		LineTo(hMemDC, startX + endX, endY);
	}


	SelectObject(hMemDC, OldBitMap);
	DeleteObject(OldBitMap);
	DeleteDC(hMemDC);
	ReleaseDC(hWnd, hdc);
}
