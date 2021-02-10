#include <Windows.h>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <string>
#include <iostream>
#include "resource.h"
#include "OnGame.h"

enum OBJ_COOR { START_X = 100, START_Y = 250, END_X = 700, END_Y = 250 }; // y원래 200씩
enum FLOOR_TEXTURE_INFO {OFFSET_Y=5, MAX_TEXTURE= 25};
enum OBS_INFO { MAX_OBS = 3, OBS_OFFSET_X = 20, OBS_OFFSET_Y = 20 };
enum DINO_COOR { DINO_START_X = 100, DINO_START_Y = 177, WIDTH  = 65, HEIGHT = 100 };

#define SPEED_UP_NUMBER 3
#define SPEED_UP_CALL_GAP 20000
#define FIRST_MOVE_SPEED 5
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

	int GetStartCoordX() { return mStart_coord_x; }
	int GetStartCoordY() { return mStart_coord_y; }
	int GetEndCoordX() { return mEnd_coord_x; }
	int GetEndCoordY() { return mEnd_coord_y; }
	int GetGameSpeed() { return mGame_progress_speed; }
	void MoveLeft(int x) { mStart_coord_x -= x; };
	void MoveUp(int y) { mStart_coord_y -= y; };
	void MoveDown(int y) { mStart_coord_y += y; };
	void SetX(int x) { mStart_coord_x = x; }
	void SetGameSpeed(int speed) { mGame_progress_speed = speed; }
	void SpeedUp(int speed) { mGame_progress_speed += speed; }
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
	int m_best_score;
	bool isJumping;
	bool isBending;
public:
	TRex() = default;
	TRex(int startX, int startY, int width, int height)
		: GameObj(startX, startY)
		, m_width(width)
		, m_height(height)
		, isJumping(false)
		, isBending(false)
		, m_best_score(0)
	{}
	~TRex() = default;

	bool GetisJumping() { return isJumping; }
	void SetisJumping(bool val) { isJumping = val; }
	bool GetisBending() { return isBending; }
	void SetisBending(bool val) { isBending = val; }
	int GetBestScore() { return m_best_score; }
	void SetBestScore(int score) { m_best_score = score; }
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
TCHAR BestScore[20];

std::vector<FloorTexture> g_floor_texture;
std::vector<Obstruction> g_obs_under;
std::random_device g_rd;
std::mt19937 g_gen(g_rd());

void OnCreate(HWND hWnd)
{
	StartTime = std::chrono::system_clock::now();
	std::uniform_int_distribution<int> obs_x_offset(0, 2000);
	std::uniform_int_distribution<int> texture_end_x_offset(2, 6);
	std::uniform_int_distribution<int> texture_start_x_offset(200,700);
	std::uniform_int_distribution<int> texture_y_offset(2, 7);

	g_floor = Floor(OBJ_COOR::START_X, OBJ_COOR::START_Y, OBJ_COOR::END_X, OBJ_COOR::END_Y, FRAME_SPEED);
	g_dino = TRex(DINO_COOR::DINO_START_X, DINO_COOR::DINO_START_Y, DINO_COOR::WIDTH, DINO_COOR::HEIGHT);
	g_floor_texture.reserve(FLOOR_TEXTURE_INFO::MAX_TEXTURE);
	g_obs_under.reserve(OBS_INFO::MAX_OBS);

	for (size_t i = 0; i < g_floor_texture.capacity(); i++) // Floor Texture
	{
		int start_offset_x = texture_start_x_offset(g_gen);
		int end_offset_x = texture_end_x_offset(g_gen);
		int startX = OBJ_COOR::START_X + start_offset_x;
		int y_offset = OBJ_COOR::START_Y + texture_y_offset(g_gen);
		
		g_floor_texture.push_back(FloorTexture(startX, y_offset, end_offset_x, y_offset , FRAME_SPEED));
	}

	int gap = 0;

	for (size_t i = 0; i < g_obs_under.capacity() - 1; i++) // Obstructions
	{
		g_obs_under.push_back(Obstruction(OBJ_COOR::END_X + gap, OBJ_COOR::END_Y- OBS_INFO::OBS_OFFSET_Y));
		gap = obs_x_offset(g_gen) + OBJ_COOR::END_X;
	}

	g_obs_under.push_back(Obstruction(OBJ_COOR::END_X + gap, OBJ_COOR::END_Y - OBS_INFO::OBS_OFFSET_Y - 60));

	LoadScore();
	SetTimer(hWnd, 0, g_floor.GetGameSpeed(), NULL);
	SetTimer(hWnd, SPEED_UP_NUMBER, SPEED_UP_CALL_GAP, NULL);

	g_dino.SetGameSpeed(FIRST_MOVE_SPEED);
}

void InitSetting(HWND hWnd)
{
	g_floor_texture.erase(g_floor_texture.begin(), g_floor_texture.end());
	g_obs_under.erase(g_obs_under.begin(), g_obs_under.end());
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
	std::ofstream writeFile;
	int nowScore = elapsed_time.count() / 100;
	int GetScore = g_dino.GetBestScore();

	if (nowScore <= GetScore)
		return;

	writeFile.open("score.txt");

	if (writeFile.is_open())
	{
		std::string str = std::to_string(nowScore);
		writeFile.write(str.c_str(), str.size());
		writeFile.close();
	}
}

void LoadScore()
{
	std::ifstream readFile;
	readFile.open("score.txt");

	if (readFile.is_open())
	{
		std::string tmp;
		std::getline(readFile, tmp);
		int bestScore = 0;

		try {
			bestScore = stoi(tmp);
		}
		catch (std::exception& e) {
			wsprintfW(BestScore, L"HI -");
			readFile.close();

			return;
		}

		g_dino.SetBestScore(bestScore);
		wsprintfW(BestScore, L"HI %d", bestScore);

		readFile.close();
	}
}

void CheckCollision(HWND hWnd)
{
	int nowX = DINO_COOR::DINO_START_X;
	int nowY = g_dino.GetStartCoordY();

	RECT dino_boundary = { nowX, nowY, nowX + DINO_COOR::WIDTH-30, nowY + DINO_COOR::HEIGHT-25 };
	RECT dino_bend_boundary = { nowX, nowY+20, nowX + DINO_COOR::WIDTH + 25, nowY + DINO_COOR::HEIGHT - 20 };

	for (size_t i = 0; i < g_obs_under.size(); i++)
	{
		int nowX = g_obs_under[i].GetStartCoordX();
		int nowY = g_obs_under[i].GetStartCoordY();

		RECT obs_boundary = { nowX, nowY, nowX + OBS_INFO::OBS_OFFSET_X, nowY + OBS_INFO::OBS_OFFSET_Y };
		RECT temp{};
		RECT* ptr_dino;

		if (g_dino.GetisBending())
			ptr_dino = &dino_bend_boundary;

		else
			ptr_dino = &dino_boundary;

		if (IntersectRect(&temp, ptr_dino, &obs_boundary))
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

bool isBending() { return g_dino.GetisBending(); }

void DrawDino(HDC* hdc, HDC* hMemDC, HBITMAP* BitMapDino, HBITMAP* BitMapDinoBend)
{
	if (g_dino.GetisBending()) // Bend
	{
		SelectObject(*hMemDC, *BitMapDinoBend);
		BitBlt(*hdc, DINO_COOR::DINO_START_X, g_dino.GetStartCoordY()+25, DINO_COOR::WIDTH + 25, DINO_COOR::HEIGHT-55
		, *hMemDC, 0, 40, SRCCOPY);
	}

	else // No Bend
	{
		SelectObject(*hMemDC, *BitMapDino);
		BitBlt(*hdc, DINO_COOR::DINO_START_X, g_dino.GetStartCoordY(), DINO_COOR::WIDTH, DINO_COOR::HEIGHT-30
			, *hMemDC, 0, 15, SRCCOPY);
	}
}

void OnKeyUp(HWND hWnd, WPARAM wParam)
{
	
	if (g_dino.GetisJumping() == false && g_dino.GetisBending() == true && wParam==VK_DOWN)
		g_dino.SetisBending(false);
}

void OnKeyDown(HWND hWnd, WPARAM wParam)
{
	switch (wParam)
	{
	case VK_UP:
	case VK_SPACE:
	{
		bool isJumping = g_dino.GetisJumping();
		bool isBending = g_dino.GetisBending();

		if (isJumping == false && isBending == false)
		{
			g_dino.SetisJumping(true);

			Beep(250, 20);
			SetTimer(hWnd, 1, 5, NULL);
		}

		break;
	}

	case VK_DOWN:
	{
		if (GetAsyncKeyState(VK_DOWN) && g_dino.GetisJumping() == false)
			g_dino.SetisBending(true);

		break;
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
		if (coorY <= 95)
			g_dino.MoveUp(1);

		else
		g_dino.MoveUp(7);
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
		g_dino.MoveDown(6);
	}
}

void MoveObj()
{
	std::uniform_int_distribution<int> send_obs_offset(100, 2500);

	for (size_t i = 0; i < g_floor_texture.capacity(); i++) // Move Floor Texture
	{
		int coorX = g_floor_texture[i].GetStartCoordX();

		if (coorX <= OBJ_COOR::START_X)
		{
			g_floor_texture[i].SetX(OBJ_COOR::END_X);
			
			continue;
		}
		g_floor_texture[i].MoveLeft(g_dino.GetGameSpeed());
	}

	for (size_t i = 0; i < g_obs_under.capacity(); i++) // Move Obstructions
	{
		int coorX = g_obs_under[i].GetStartCoordX();
		int delay_obs = send_obs_offset(g_gen);

		if (coorX <= OBJ_COOR::START_X)
		{
			int distance = send_obs_offset(g_gen);

			g_obs_under[i].SetX(OBJ_COOR::END_X + distance + delay_obs);
			delay_obs = distance + 800;

			continue;
		}

		g_obs_under[i].MoveLeft(g_dino.GetGameSpeed());
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
	for (size_t i = 0; i < g_obs_under.size(); i++) // Obstruction
	{
		HBRUSH myBrush = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
		HBRUSH oldBrush = (HBRUSH)SelectObject(hMemDC, myBrush);

		int startX = g_obs_under[i].GetStartCoordX();
		int startY = g_obs_under[i].GetStartCoordY();

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
	TextOut(hMemDC, OBJ_COOR::END_X - 150, OBJ_COOR::END_Y-120, BestScore, lstrlen(BestScore));

	SelectObject(hMemDC, OldBitMap);
	DeleteObject(OldBitMap);
	DeleteDC(hMemDC);
	ReleaseDC(hWnd, hdc);
}

void SpeedUp()
{
	g_dino.SpeedUp(1);
}