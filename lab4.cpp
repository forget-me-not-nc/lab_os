#pragma warning(disable : 4996)

#define ID_RICHEDIT 9001
#define ID_DISENBUTTON 9002
#define ID_TEXTBUFFER 9003

#include <windows.h>
#include <windowsx.h>
#include <richedit.h>
#include <string>
#include <bits.h>
#include "atlstr.h"
#include <vector>
#include <sstream>

using std::string;
using std::strcpy;
using std::vector;

LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND CreateRichEdit(HWND hParent);
HWND CreateDisableEnableButton(HWND hParent);
HWND CreateText(HWND hParent);

void ValidateCursorPos(HWND hWnd, POINT CurrPos);
void MouseLeftClick();
void MouseRightClick();
void MouseTracker(HWND hWnd, LPARAM lParam);
void TextBufferOut(HWND hWnd, TCHAR keyName[], LONG Key);

//typedef struct tagPOINTS
//{
//	SHORT x;
//	SHORT y;
//
//} POINTS;

HINSTANCE _hInstance;
RECT _Rect = {};
BOOL RichEditEnable = true;
int PseudoTimer = 0;
string strBuffer = "";
LONG LastKey = NULL;

vector <HWND> ChildWindowsEnum;

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR szCmdLine, int nCmdShow)
{
	MSG			msg{};
	HWND		hwnd{};
	WNDCLASSEX	wc{ sizeof(WNDCLASSEX) };

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(GRAY_BRUSH));
	wc.hCursor = LoadCursor(nullptr, IDC_PERSON);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WinProc;
	wc.lpszClassName = L"MainWindow";
	wc.lpszMenuName = nullptr;
	wc.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wc))
	{
		return EXIT_FAILURE;
	}

	if (hwnd = CreateWindowEx(NULL, wc.lpszClassName, L"Window", WS_OVERLAPPEDWINDOW, 350, 100, 1000, 700, nullptr, nullptr, wc.hInstance, nullptr); hwnd == INVALID_HANDLE_VALUE)
	{
		return EXIT_FAILURE;
	}

	_hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hwnd, GWLP_HINSTANCE));

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
		{
			HWND hRichEdit = CreateRichEdit(hWnd);
			HWND hDisableEnableButton = CreateDisableEnableButton(hWnd);
			HWND hText = CreateText(hWnd);

			break;
		}

		case WM_KEYDOWN:
		{	
			POINT CurrPos;
			TCHAR textBuffer[32];

			GetCursorPos(&CurrPos);

			if (GetKeyNameText(lParam, textBuffer, 32))
			{
				TextBufferOut(hWnd, textBuffer, lParam);
			}

			if (wParam == 0x25)
			{
				SetCursorPos(CurrPos.x - 5, CurrPos.y);
				SendMessage(hWnd, WM_MOUSEMOVE, 0, 111);
			}
			else if (wParam == 0x26)
			{
				SetCursorPos(CurrPos.x, CurrPos.y - 5);
				SendMessage(hWnd, WM_MOUSEMOVE, 0, 111);
			}
			else if (wParam == 0x27)
			{
				SetCursorPos(CurrPos.x + 5, CurrPos.y);
				SendMessage(hWnd, WM_MOUSEMOVE, 0, 111);
			}
			else if (wParam == 0x28)
			{
				SetCursorPos(CurrPos.x, CurrPos.y + 5);
				SendMessage(hWnd, WM_MOUSEMOVE, 0, 111);
			}
			else if (wParam == 0x39)
			{
				MouseLeftClick();
			}
			else if (wParam == 0x30)
			{
				MouseRightClick();
			}

			break;
		}

		case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC)wParam;
			SetTextColor(hdcStatic, RGB(0, 0, 0));
			SetBkColor(hdcStatic, RGB(128, 128, 128));

			return (LRESULT)GetStockObject(NULL_BRUSH);
		}

		case WM_MOUSEMOVE:
		{
			if (lParam == 111)
			{
				POINT CurrPos;
				GetCursorPos(&CurrPos);

				ValidateCursorPos(hWnd, CurrPos);
				
				SetFocus(hWnd);
			}

			if (PseudoTimer % 10 == 0)
			{
				MouseTracker(hWnd, lParam);
				PseudoTimer = 0;
			}
			
			PseudoTimer++;

			break;
		}

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case ID_DISENBUTTON:
				{
					if (RichEditEnable)
					{
						EnableWindow((HWND)GetDlgItem(hWnd, ID_RICHEDIT), FALSE);
						RichEditEnable = false;
					}
					else
					{
						EnableWindow((HWND)GetDlgItem(hWnd, ID_RICHEDIT), TRUE);
						RichEditEnable = true;
					}

					SetFocus(hWnd);

					break;
				}

				break;
			}

			break;
		}

		case WM_DESTROY:
		{
			DestroyWindow(hWnd);
			PostQuitMessage(EXIT_SUCCESS);

			break;
		}

	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND CreateRichEdit(HWND hParent)
{
	LoadLibrary(L"riched20.dll");

	HWND hRichEdit;

	hRichEdit = CreateWindowEx(NULL, RICHEDIT_CLASS, NULL, 
			WS_VISIBLE | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_NOHIDESEL | ES_AUTOVSCROLL | ES_MULTILINE | ES_SAVESEL | ES_SUNKEN,
			_Rect.left + 40, _Rect.top + 300, 350, 40, hParent, reinterpret_cast<HMENU>(ID_RICHEDIT), _hInstance, NULL);

	return hRichEdit;
}

HWND CreateDisableEnableButton(HWND hParent)
{
	HWND hButton;

	hButton = CreateWindow(L"BUTTON", L"Dis/En RE", BS_FLAT | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, _Rect.left + 40, _Rect.top + 230, 125, 40,
				hParent, (HMENU)ID_DISENBUTTON, _hInstance, NULL);

	return hButton;
}

HWND CreateText(HWND hParent)
{
	GetClientRect(hParent, &_Rect);

	HWND hText;

	hText = CreateWindow(L"STATIC", L" LastPressedKey: ", WS_CHILD | WS_VISIBLE, _Rect.right - 325, _Rect.top + 100, 120, 20, hParent, NULL, _hInstance, NULL);

	return hText;
}

void ValidateCursorPos(HWND hWnd, POINT CurrPos)
{
	GetWindowRect(hWnd, &_Rect);

	if (CurrPos.x > _Rect.right)
	{
		SetCursorPos(_Rect.left, CurrPos.y);
	}
	else if (CurrPos.x < _Rect.left)
	{
		SetCursorPos(_Rect.right, CurrPos.y);
	}
	else if (CurrPos.y > _Rect.bottom)
	{
		SetCursorPos(CurrPos.x, _Rect.top);
	}
	else if (CurrPos.y < _Rect.top)
	{
		SetCursorPos(CurrPos.x, _Rect.bottom);
	}
}

void MouseLeftClick()
{
	INPUT Input = {sizeof(INPUT)}; 
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));

	::ZeroMemory(&Input, sizeof(INPUT));

	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	::SendInput(1, &Input, sizeof(INPUT));	
}

void MouseRightClick()
{
	INPUT Input = { sizeof(INPUT) };
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));

	::ZeroMemory(&Input, sizeof(INPUT));

	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	::SendInput(1, &Input, sizeof(INPUT));
}

void MouseTracker(HWND hWnd, LPARAM lParam)
{
	POINTS pt = MAKEPOINTS(lParam);
	std::stringstream temp;
	string TextToOut = "Current mouse position: X = ";

	temp << pt.x;

	TextToOut += temp.str();
	TextToOut += "; Y = ";

	temp.str("");
	temp << pt.y;

	TextToOut += temp.str();
	TextToOut += ";";

	std::wstring stemp = std::wstring(TextToOut.begin(), TextToOut.end());
	LPCWSTR Log = stemp.c_str();

	SetWindowText((HWND)GetDlgItem(hWnd, ID_RICHEDIT), Log);
}

void TextBufferOut(HWND hWnd, TCHAR keyName[], LONG Key)
{
	if (LastKey != Key)
	{
		for (int i = 0; i < ChildWindowsEnum.size(); i++)
		{
			EnableWindow(ChildWindowsEnum[i], FALSE);
			DestroyWindow(ChildWindowsEnum[i]);
		}

		LastKey = Key;
	}

	GetClientRect(hWnd, &_Rect);

	HWND hTextBuffer = CreateWindow(L"STATIC", keyName, WS_CHILD | WS_VISIBLE, _Rect.right - 200, _Rect.top + 100, 70, 20, hWnd, NULL, _hInstance, NULL);

	ChildWindowsEnum.push_back(hTextBuffer);
}
