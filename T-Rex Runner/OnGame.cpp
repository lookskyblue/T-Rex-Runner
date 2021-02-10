#include <Windows.h>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include "resource.h"
#include "OnGame.h"

enum OBJ_COOR { START_X = 100, START_Y = 250, END_X = 700, END_Y = 250 }; // y원래 200씩
enum FLOOR_TEXTURE_INFO {OFFSET_Y=5, MAX_TEXTURE= 25};
enum OBS_INFO { MAX_OBS = 2, OBS_OFFSET_X = 20, OBS_OFFSET_Y = 20 };
enum DINO_COOR { DINO_START_X = 100, DINO_START_Y = 177, WIDTH  = 65, HEIGHT = 100 };

#define MOVE_SPEED 5
#define FRAME_SPEED 5
#define AGAIN 0

class GameObj {
private:
	int mGame_progress_speed;
	int mStart_coord_x;
	int mStart_coord_y;
	int mEnd_coord_x;
	int mEnd_coord_y;

public:
	GameObj() = default;

	GameObj(int startX, int startY)
		: mStart_coord_x(startX)
		, mStart_coord_y(startY)
		, mEnd_coord_x(0)
		, mEnd_coord_y(0)
	{}

	GameObj(int startX, int startY, int endX, int endY, int gameSpeed)
		: mStart_coord_x(startX)
		, mStart_coord_y(startY)
		, mEnd_coord_x(endX)
		, mEnd_coord_y(endY)
		, mGame_progress_speed(gameSpeed)
	{}

	~GameObj() = default;

	int GetGameSpeed() { return mGame_progress_speed; }
	int GetStartCoordX() { return mStart_coord_x; }
	int GetStartCoordY() { return mStart_coord_y; }
	int GetEndCoordX() { return mEnd_coord_x; }
	int GetEndCoordY() { return mEnd_coord_y; }
	void MoveLeft(int x) { mStart_coord_x -= x; };
	void MoveUp(int y) { mStart_coord_y -= y; };
	void MoveDown(int y) { mStart_coord_y += y; };
	void SetX(int x) { mStart_coord_x = x; }
};

class Floor : public GameObj{
private:

public:
	Floor() = default;
	Floor(int startX, int startY, int endX, int endY, int gameSpeed)
		: GameObj(startX, startY,endX,endY, gameSpeed) {}

	~Floor() = default;
};

class FloorTexture :public GameObj {
private: 
public:
	FloorTexture(int startX, int startY, int endX, int endY, int gameSpeed)
		: GameObj(startX, startY, endX, endY, gameSpeed) {}

	~FloorTexture() = default;
};

class TRex : public GameObj {
private:
	int m_width;
	int m_height;
	bool isJumping;
public:
	TRex() = default;
	TRex(int startX, int startY, int width, int height)
		: GameObj(startX, startY)
		, m_width(width)
		, m_height(height)
		, isJumping(false)
	{}
	~TRex() = default;

	bool GetisJumping() { return isJumping; }
	void SetisJumping(bool val) { isJumping = val; }
};

class Obstruction : public GameObj{
private:
public:
	Obstruction(int x, int y)
		: GameObj(x, y) {}

	~Obstruction() = default;
};

Floor g_floor;
TRex g_dino;
std::chrono::system_clock::time_point StartTime;
std::chrono::system_clock::time_point EndTime;
std::chrono::milliseconds elapsed_time;
TCHAR ElapsedTime[20];

std::vector<FloorTexture> g_floor_texture;
std::vector<Obstruction> g_obs;
std::random_device g_rd;
std::mt19937 g_gen(g_rd());

void OnCreate(HWND hWnd, int* coor)
{
	StartTime = std::chrono::system_clock::now();
	std::uniform_int_distribution<int> g_x_start_offset(0, OBJ_COOR::END_X - OBJ_COOR::START_X);
	std::uniform_int_distribution<int> g_x_end_offset(2, 6);
	std::uniform_int_distribution<int> g_y_offset(2, 7);

	g_floor = Floor(OBJ_COOR::START_X, OBJ_COOR::START_Y, OBJ_COOR::END_X, OBJ_COOR::END_Y, FRAME_SPEED);
	g_dino = TRex(DINO_COOR::DINO_START_X, DINO_COOR::DINO_START_Y, DINO_COOR::WIDTH, DINO_COOR::HEIGHT);
	g_floor_texture.reserve(FLOOR_TEXTURE_INFO::MAX_TEXTURE);
	g_obs.reserve(OBS_INFO::MAX_OBS);

	coor[0] = DINO_COOR::DINO_START_X;
	coor[1] = DINO_COOR::WIDTH;
	coor[2] = DINO_COOR::HEIGHT;

	for (size_t i = 0; i < g_floor_texture.capacity(); i++)
	{
		int start_offset_x = g_x_start_offset(g_gen);
		int end_offset_x = g_x_end_offset(g_gen);
		int startX = OBJ_COOR::START_X + start_offset_x;
		int y_offset = OBJ_COOR::START_Y + g_y_offset(g_gen);
		
		g_floor_texture.push_back(FloorTexture(startX, y_offset, end_offset_x, y_offset , FRAME_SPEED));
	}

	for (size_t i = 0; i < g_obs.capacity(); i++)
	{
		g_obs.push_back(Obstruction(OBJ_COOR::END_X - OBS_INFO::OBS_OFFSET_X, OBJ_COOR::END_Y- OBS_INFO::OBS_OFFSET_Y));
	}

	SetTimer(hWnd, 0, g_floor.GetGameSpeed(), NULL);
}

void InitSetting(HWND hWnd)
{
	g_floor_texture.erase(g_floor_texture.begin(), g_floor_texture.end());
	g_obs.erase(g_obs.begin(), g_obs.end());
}

int GetDinoCoorY()
{
	return g_dino.GetStartCoordY();
}

void OnTimer(HWND hWnd, HBITMAP* hBitMap, HINSTANCE* hInst)
{
	MoveObj();
	DrawObj(hWnd, hBitMap, hInst);
	InvalidateRect(hWnd, nullptr, false);
	CheckCollision(hWnd);
}

void SaveScore()
{

}

void CheckCollision(HWND hWnd)
{
	int nowX = DINO_COOR::DINO_START_X;
	int nowY = g_dino.GetStartCoordY();

	RECT dino_boundary = { nowX, nowY, nowX + DINO_COOR::WIDTH-30, nowY + DINO_COOR::HEIGHT-25 };

	for (size_t i = 0; i < g_obs.size(); i++)
	{
		int nowX = g_obs[i].GetStartCoordX();
		int nowY = g_obs[i].GetStartCoordY();

		RECT obs_boundary = { nowX, nowY, nowX + OBS_INFO::OBS_OFFSET_X, nowY + OBS_INFO::OBS_OFFSET_Y };
		RECT temp{};

		if (IntersectRect(&temp, &dino_boundary, &obs_boundary))
		{
			SaveScore();
			EndGame(hWnd);
		}
	}
}

void EndGame(HWND hWnd)
{
	KillTimer(hWnd, 0);

	if (MessageBox(hWnd, L"Again?", L"T-Rex Runner", MB_YESNO | MB_ICONHAND)
		== IDYES)
	{
		SendMessage(hWnd, WM_CREATE, AGAIN, NULL);
	}

	else
		SendMessage(hWnd, WM_DESTROY, 0, 0);

}

void OnKeyDown(HWND hWnd, WPARAM wParam, int* g_y)
{
	switch (wParam)
	{
	case VK_UP:
	{
		bool isJumping = g_dino.GetisJumping();

		if (isJumping == false)
		{
			g_dino.SetisJumping(true);

			Beep(250, 20);
			SetTimer(hWnd, 1, 5, NULL);
		}
	}

	default:
		break;
	}

}

void UpDino(HWND hWnd, WPARAM wParam)
{
	int coorY = g_dino.GetStartCoordY();

	if (coorY <= 80)
	{
		KillTimer(hWnd, wParam);
		SetTimer(hWnd, 2, 5, NULL);
	}

	else
	{
		if (coorY <= 90)
			g_dino.MoveUp(1);

		else
		g_dino.MoveUp(5);
	}
}

void DownDino(HWND hWnd, WPARAM wParam)
{
	int coorY = g_dino.GetStartCoordY();

	if (coorY >= 173)
	{
		KillTimer(hWnd, wParam);
		g_dino.SetisJumping(false);
	}

	else
	{
		g_dino.MoveDown(5);
	}
}

void MoveObj()
{
	std::uniform_int_distribution<int> send_obs_offset(200, 800);

	for (size_t i = 0; i < g_floor_texture.capacity(); i++)
	{
		int coorX = g_floor_texture[i].GetStartCoordX();

		if (coorX <= OBJ_COOR::START_X)
		{
			g_floor_texture[i].SetX(OBJ_COOR::END_X);
			
			continue;
		}

		g_floor_texture[i].MoveLeft(MOVE_SPEED);
	}

	for (size_t i = 0; i < g_obs.capacity(); i++)
	{
		int coorX = g_obs[i].GetStartCoordX();

		if (coorX <= OBJ_COOR::START_X)
		{
			
			int distance = send_obs_offset(g_gen);
			g_obs[i].SetX(OBJ_COOR::END_X+distance);

			continue;
		}

		g_obs[i].MoveLeft(MOVE_SPEED);
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
	
	// Floor
	MoveToEx(hMemDC, g_floor.GetStartCoordX(), g_floor.GetStartCoordY(), NULL);
	LineTo(hMemDC, g_floor.GetEndCoordX(), g_floor.GetEndCoordY());

	// Floor texture
	for (size_t i = 0; i < g_floor_texture.size(); i++)
	{
		int startX = g_floor_texture[i].GetStartCoordX();
		int startY = g_floor_texture[i].GetStartCoordY();
		int endX = g_floor_texture[i].GetEndCoordX();
		int endY = g_floor_texture[i].GetEndCoordY();

		MoveToEx(hMemDC, startX, startY, NULL);
		LineTo(hMemDC, startX + endX, endY);
	}

	// Obstructions
	for (size_t i = 0; i < g_obs.size(); i++) // Obstruction
	{
		HBRUSH myBrush = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
		HBRUSH oldBrush = (HBRUSH)SelectObject(hMemDC, myBrush);

		int startX = g_obs[i].GetStartCoordX();
		int startY = g_obs[i].GetStartCoordY();


		if (OBJ_COOR::START_X >= startX && startX + OBS_INFO::OBS_OFFSET_X >= OBJ_COOR::START_X)
			Rectangle(hMemDC, OBJ_COOR::START_X, startY, startX + OBS_INFO::OBS_OFFSET_Y, startY + OBS_INFO::OBS_OFFSET_Y);

		else if (startX <= OBJ_COOR::END_X)
			Rectangle(hMemDC, startX, startY, startX + OBS_INFO::OBS_OFFSET_X, startY + OBS_INFO::OBS_OFFSET_Y);

		DeleteObject(myBrush);
	}

	EndTime = std::chrono::system_clock::now();
	elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(EndTime - StartTime);
	wsprintf(ElapsedTime, L"%d", elapsed_time.count()/100);
	TextOut(hMemDC, OBJ_COOR::END_X - 50, OBJ_COOR::END_Y-120, ElapsedTime, lstrlen(ElapsedTime));

	SelectObject(hMemDC, OldBitMap);
	DeleteObject(OldBitMap);
	DeleteDC(hMemDC);
	ReleaseDC(hWnd, hdc);
}
