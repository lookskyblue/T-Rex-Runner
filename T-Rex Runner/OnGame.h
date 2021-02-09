#pragma once

void OnCreate(HWND hWnd);
void OnTimer(HWND hWnd, HBITMAP* hBitMap, HINSTANCE* hInst);
void MoveObj();
void DrawObj(HWND hWnd, HBITMAP* hBitMap, HINSTANCE* hInst);
void OnKeyDown(HWND hWnd, WPARAM wParam, int* g_y);
void UpDino(HWND hWnd, WPARAM wParam, int* g_y);
void DownDino(HWND hWnd, WPARAM wParam, int* g_y);
