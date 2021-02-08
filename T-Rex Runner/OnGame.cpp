#include <Windows.h>
#include "OnGame.h"

enum MAP_COOR { START_X = 50, START_Y = 200, END_X = 100, END_Y = 200 };
#define GAME_SPEED 300

class GameMap {
private:
	int mGame_progress_speed;
	int mStart_coord_x;
	int mStart_coord_y;
	int mEnd_coord_x;
	int mEnd_coord_y;

public:
	GameMap(int startX, int startY, int endX, int endY, int gameSpeed)
		: mStart_coord_x(startX)
		, mStart_coord_y(startY)
		, mEnd_coord_x(endX)
		, mEnd_coord_y(endY)
		, mGame_progress_speed(gameSpeed)
	{}

	int GetGameSpeed() { return mGame_progress_speed; }
};

void OnCreate(HWND hWnd)
{
	GameMap gm(MAP_COOR::START_X, MAP_COOR::START_Y, MAP_COOR::END_X, MAP_COOR::END_Y, GAME_SPEED);

	SetTimer(hWnd, 0, gm.GetGameSpeed(), NULL);
}