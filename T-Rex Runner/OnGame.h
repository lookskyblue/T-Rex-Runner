#pragma once

void OnCreate(HWND, int*);
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