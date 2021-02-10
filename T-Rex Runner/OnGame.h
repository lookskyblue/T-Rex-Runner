#pragma once

void OnCreate(HWND);
void OnTimer(HWND, HBITMAP*, HINSTANCE*);
void MoveObj();
void DrawObj(HWND, HBITMAP*, HINSTANCE*);
void OnKeyDown(HWND, WPARAM);
void UpDino(HWND, WPARAM);
void DownDino(HWND, WPARAM);
void CheckCollision(HWND);
int GetDinoCoorY();
void EndGame(HWND);
void InitSetting(HWND);
void SaveScore();
void LoadScore();
bool isBending();
void DrawDino(HDC*, HDC*, HBITMAP*, HBITMAP*);
void OnKeyUp(HWND, WPARAM);
void SpeedUp();