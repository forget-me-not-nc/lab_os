#pragma warning(disable : 4996)

#define ID_RICHEDIT		11
#define ID_BUTTON2		22
#define ID_BUTTON3		33
#define ID_BUTTON4		44
#define ID_TREEVIEW		55
#define ID_IPADRESS		66
#define ID_IPINFO		77
#define ID_UPDATEBUTTON	88
#define ID_COMBOBOX		99
#define ID_PROGRESS_BAR				20
#define ID_STATIC					10
#define DELETEW			999
#define IDB_TEST				101
#define IDT_TIMER				102
#define IDT_PROGRESS_TIMER		103
#define IDM_NEW		11122
#define IDM_SAVE	11133
#define IDM_OPEN	11144

#include <Windows.h>
#include "atlstr.h"
#include <richedit.h>
#include <commctrl.h>
#include <vector>
#include <ctime>
#include <string>

std::vector <HWND> ChildWindowEnum;

int r, g, b;
bool button3Created = false;
bool button4Created = false;
bool button5Created = false;

RECT _Rect = {};
HINSTANCE _hInstance;

LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND CreateRichEdit(HWND hParent, HINSTANCE hInst);
HWND CreateIpAdressControl(HWND hParent, HINSTANCE hInst);
HWND UpdateIPInfo(HWND hWnd);
HWND CreateToolTip(int IDFor, HWND hParent, LPWSTR TipText, HINSTANCE hInst);
HWND CreateCustomProgressBar(HWND hParent, HINSTANCE hInst);
HWND CreateComboBox(HWND hParent, HINSTANCE hInst);
HWND CreateToolbar(HWND hWndParent, HINSTANCE hInst);
HFONT CreateCustomFont(HWND hParent, HINSTANCE hInst);


int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR szCmdLine, int nCmdShow)
{
	MSG			msg{};
	HWND		hwnd{};
	WNDCLASSEX	wc{ sizeof(WNDCLASSEX) };

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(GRAY_BRUSH));
	wc.hCursor = LoadCursor(nullptr, IDC_PERSON);
	wc.hIcon = reinterpret_cast<HICON>(LoadImage(hInstance, L"Caesar.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	wc.hIconSm = reinterpret_cast<HICON>(LoadImage(hInstance, L"Caesar.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WinProc;
	wc.lpszClassName = L"MainWindow";
	wc.lpszMenuName = nullptr;
	wc.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wc))
	{
		return EXIT_FAILURE;
	}

	if (hwnd = CreateWindowEx(NULL, wc.lpszClassName, L"Window", WS_OVERLAPPEDWINDOW, 350, 350, 800, 500, nullptr, nullptr, wc.hInstance, nullptr); hwnd == INVALID_HANDLE_VALUE)
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
			if (LOWORD(wParam) == 111)
			{
				HWND hButton3;
				GetClientRect(hWnd, &_Rect);

				hButton3 = CreateWindowEx(NULL, L"BUTTON", L"STAGE 2", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_FLAT,
					_Rect.right - 130, _Rect.bottom - 90, 120, 30, hWnd, (HMENU)ID_BUTTON3, _hInstance, NULL);
				
				EnableWindow(hButton3, FALSE);

				ChildWindowEnum.push_back(hButton3);

				break;
			}
			else if (LOWORD(wParam) == 2020)
			{

				HWND hWndProgressBar = CreateCustomProgressBar(hWnd, _hInstance);
			
				break;
			}
			else if(LOWORD(wParam) == 444)
			{
				HWND hButton4;

				GetClientRect(hWnd, &_Rect);

				hButton4 = CreateWindowEx(NULL, L"BUTTON", L"STAGE 3", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_FLAT,
					_Rect.right - 130, _Rect.bottom - 150, 120, 30, hWnd, (HMENU)ID_BUTTON4, _hInstance, NULL);

				ChildWindowEnum.push_back(hButton4);

				EnableWindow(hButton4, FALSE);

				break;
			}
			else if(LOWORD(wParam) == 3)
			{
				
				HWND			hIPAdress = CreateIpAdressControl(hWnd, _hInstance);
				HWND			hInfoWindow = UpdateIPInfo(hWnd);
				HWND			hUpdateButton;

				hUpdateButton = CreateWindowEx(NULL, L"BUTTON", L"UPDATE", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_FLAT,
					_Rect.left + 100, _Rect.top + 200, 100, 45, hWnd, (HMENU)ID_UPDATEBUTTON, _hInstance, NULL);

				ChildWindowEnum.push_back(hUpdateButton);

				break;
			}
			else if (LOWORD(wParam) == 4)
			{
				HWND hComboBox = CreateComboBox(hWnd, _hInstance);
				HWND hToolBar = CreateToolbar(hWnd, _hInstance);

				break;
			}
			else if (LOWORD(wParam) == 8)
			{
				HWND hInfo = UpdateIPInfo(hWnd);

				break;
			}
			else
			{

				HWND		hRichEdit = CreateRichEdit(hWnd, _hInstance);
				HWND		hDeleteButton;
				HFONT		hFont = CreateCustomFont(hWnd, _hInstance);
				HWND		hButton2;
				HWND		hStatic;

				SendMessage(hRichEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

				hDeleteButton = CreateWindowEx(NULL, L"BUTTON", L"DELETE", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_FLAT,
					_Rect.right - 130, _Rect.bottom - 40, 120, 30, hWnd, (HMENU)DELETEW, _hInstance, NULL);

				char text1[] = "PRESS ME! PRESS ME! PRESS ME!";
				wchar_t wtext1[30];
				mbstowcs(wtext1, text1, strlen(text1) + 1);
				LPWSTR result = wtext1;

				HWND		hToolTip = CreateToolTip(999, hWnd, result, _hInstance);

				hButton2 = CreateWindowEx(NULL, L"BUTTON", L"PRESS", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_FLAT,
					_Rect.left + 350, _Rect.top + 10, 100, 45, hWnd, (HMENU)ID_BUTTON2, _hInstance, NULL);

				char text2[] = "I CAN CHANGE COLOR!";
				wchar_t wtext2[30];
				mbstowcs(wtext2, text2, strlen(text2) + 1);
				result = wtext2;

				hToolTip = CreateToolTip(2, hWnd, result, _hInstance);

				ChildWindowEnum.push_back(hButton2);

				hStatic = CreateWindow(L"STATIC", L"Text Editor:", WS_VISIBLE | WS_CHILD, _Rect.left + 45, _Rect.top + 45, 100, 20, hWnd, (HMENU)ID_STATIC, _hInstance, NULL);

				ChildWindowEnum.push_back(hStatic);
				

				break;
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

		case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO min_max = (LPMINMAXINFO)lParam;
			min_max->ptMinTrackSize.x = 800;
			min_max->ptMinTrackSize.y = 500;

			min_max->ptMaxTrackSize.x = 800;
			min_max->ptMaxTrackSize.y = 500;

			break;
		}

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case 999:
				{
					for (int i = 0; i < ChildWindowEnum.size(); i++)
					{
						EnableWindow(ChildWindowEnum[i], FALSE);
						DestroyWindow(ChildWindowEnum[i]);
					}

					ChildWindowEnum.clear();
					
					switch (HIWORD(wParam))
					{
						case BN_CLICKED:
						{
							if (SetTimer(hWnd, IDT_TIMER, 5000, (TIMERPROC)NULL))
							{
								SetTimer(hWnd, IDT_PROGRESS_TIMER, 500, (TIMERPROC)NULL);
								MessageBox(NULL, L"Timer set for 5 seconds", L"Success", MB_OK | MB_ICONINFORMATION);
							}
							else
							{
								MessageBox(NULL, L"Timer failed", L"Error", MB_OK | MB_ICONERROR);
							}

							break;
						}

						break;
					}

					SendMessage(hWnd, WM_CREATE, 2020, NULL);

					if (!button3Created)
					{
						SendMessage(hWnd, WM_CREATE, 111, NULL);

						button3Created = true;
					}
					else if (!button4Created && button3Created)
					{
						SendMessage(hWnd, WM_CREATE, 444, NULL);
						
						button4Created = true;
					}
					else if(button4Created && button3Created)
					{
						SendMessage(hWnd, WM_DESTROY, 0, 0);
					}

					EnableWindow((HWND)GetDlgItem(hWnd, 999), FALSE);

					break;
				}

				case 22:
				{

					srand(time(nullptr));

					r = rand() % 255;
					g = rand() % 255;
					b = rand() % 255;

					HDC			wmf = CreateMetaFile(L"button2.wmf");
					HBRUSH		hBrush = CreateSolidBrush(RGB(r, g, b));
					RECT		CurRect;
					
					GetClientRect(GetDlgItem(hWnd, 22), &CurRect);

					SelectObject(wmf, hBrush);
					FillRect(wmf, &CurRect, hBrush);

					CloseMetaFile(wmf);

					HDC hdc = GetDC(GetDlgItem(hWnd, 22));

					PlayMetaFile(hdc, GetMetaFile(L"button2.wmf"));

					ReleaseDC(hWnd, hdc);

					break;
				}

				case 33:
				{
					EnableWindow((HWND)GetDlgItem(hWnd, 999), TRUE);
					SendMessage(hWnd, WM_CREATE, 3, NULL);
					EnableWindow((HWND)GetDlgItem(hWnd, 33), FALSE);

					break;
				}

				case 44:
				{
					EnableWindow((HWND)GetDlgItem(hWnd, 999), TRUE);
					SendMessage(hWnd, WM_CREATE, 4, NULL);
					EnableWindow((HWND)GetDlgItem(hWnd, 44), FALSE);

					break;
				}

				case 88:
				{
					DestroyWindow((HWND)GetDlgItem(hWnd,77));
					SendMessage(hWnd, WM_CREATE, 8, NULL);

					break;
				}

				case ID_COMBOBOX:
				{
					if (CBN_DBLCLK)
					{
						int ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL,
							(WPARAM)0, (LPARAM)0);
						TCHAR  ListItem[256];
						(TCHAR)SendMessage((HWND)lParam, (UINT)CB_GETLBTEXT,
							(WPARAM)ItemIndex, (LPARAM)ListItem);
						MessageBox(hWnd, (LPCWSTR)ListItem, TEXT("Item Selected"), MB_OK);

						break;
					}

					break;
				}

				break;
			}

			break;
		}

		case WM_TIMER:
		{

			switch ((UINT)wParam)
			{

				case IDT_TIMER:
				{

					KillTimer(hWnd, IDT_TIMER);
					KillTimer(hWnd, IDT_PROGRESS_TIMER);
					DestroyWindow((HWND)GetDlgItem(hWnd, 20));

					MessageBox(NULL, L"Deleted successfully", L"Success", MB_OK | MB_ICONINFORMATION);

					EnableWindow((HWND)GetDlgItem(hWnd, ID_BUTTON3), TRUE);
					EnableWindow((HWND)GetDlgItem(hWnd, ID_BUTTON4), TRUE);

					break;
				}
				case IDT_PROGRESS_TIMER:
				{
					SendMessage((HWND)GetDlgItem(hWnd, 20), PBM_STEPIT, 0, 0);

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

		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND CreateRichEdit(HWND hParent, HINSTANCE hInst)
{
	LoadLibrary(L"riched20.dll");

	HWND		hRichEdit;

	GetClientRect(hParent, &_Rect);

	hRichEdit = CreateWindowEx(NULL, RICHEDIT_CLASS, NULL, WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_CHILD | WS_BORDER | ES_NOHIDESEL | ES_MULTILINE | ES_SAVESEL | ES_SUNKEN,
		_Rect.left + 40, _Rect.top + 60, 700, 300, hParent, reinterpret_cast<HMENU>(ID_RICHEDIT), hInst, NULL);

	ChildWindowEnum.push_back(hRichEdit);

	return hRichEdit;
}

HFONT CreateCustomFont(HWND hParent, HINSTANCE hInst)
{
	HFONT hFont;

	hFont = CreateFont(-20, 0, 0, 0, FW_BOLD, TRUE, FALSE, TRUE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, NULL);

	return hFont;
}

HWND CreateCustomProgressBar(HWND hParent, HINSTANCE hInst)
{
	HWND hWndProgressBar;

	GetClientRect(hParent, &_Rect);

	hWndProgressBar = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_VISIBLE | WS_CHILD, 250, 190, 300, 40,
		hParent, (HMENU)ID_PROGRESS_BAR, hInst, NULL);


	if (!hWndProgressBar)
	{
		int index1 = GetLastError();
		std::string test1 = std::to_string(index1);
		MessageBoxA(NULL, test1.c_str(), "testx", MB_OK);
	}
	else
	{
		ShowWindow(hWndProgressBar, 1);

		SendMessage(hWndProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 6));
		SendMessage(hWndProgressBar, PBM_SETSTEP, (WPARAM)1, 0);

	}

	return hWndProgressBar;
}

HWND CreateIpAdressControl(HWND hParent, HINSTANCE hInst)
{
	LoadLibrary(L"comctl32.dll");

	INITCOMMONCONTROLSEX	icc;

	icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icc.dwICC = ICC_INTERNET_CLASSES;
	InitCommonControlsEx(&icc);

	HWND hIPAdress = CreateWindow(WC_IPADDRESS, L"", WS_CHILD | WS_VISIBLE, 325, 240, 150, 20, hParent, (HMENU)ID_IPADRESS, hInst, NULL);
	SendMessage(hIPAdress, IPM_SETADDRESS, 0, MAKEIPADDRESS(109, 222, 17, 6));

	ChildWindowEnum.push_back(hIPAdress);

	return hIPAdress;
}

HWND CreateToolTip(int IDFor, HWND hParent, LPWSTR TipText, HINSTANCE hInst)
{
	if (!IDFor || !hParent)
	{
		return FALSE;
	}

	HWND hTool = GetDlgItem(hParent, IDFor);

	HWND hToolTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON, 
			0, 0, 50, 50, hParent, NULL, hInst, NULL);

	if (!hTool || !hToolTip)
	{
		return NULL;
	}

	TOOLINFO toolInfo = { 0 };

	toolInfo.cbSize = sizeof(toolInfo) - 4;
	toolInfo.hwnd = hParent;
	toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	toolInfo.uId = (UINT_PTR)hTool;
	toolInfo.lpszText = TipText;

	SendMessage(hToolTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);

	return hToolTip;
}

HWND UpdateIPInfo(HWND hWnd)
{
	HWND					hInfoWindow;
	DWORD					CurAddress;

	LRESULT SM = SendMessage((HWND)GetDlgItem(hWnd,ID_IPADRESS), IPM_GETADDRESS, 0, (LPARAM)(LPDWORD)&CurAddress);

	BYTE IPPart1 = FIRST_IPADDRESS((LPARAM)CurAddress);
	BYTE IPPart2 = SECOND_IPADDRESS((LPARAM)CurAddress);
	BYTE IPPart3 = THIRD_IPADDRESS((LPARAM)CurAddress);
	BYTE IPPart4 = FOURTH_IPADDRESS((LPARAM)CurAddress);

	std::string info = "First Byte: ";
	info += std::to_string(IPPart1);
	info += "\nSecond Byte: ";
	info += std::to_string(IPPart2);
	info += "\nThird Byte: ";
	info += std::to_string(IPPart3);
	info += "\nFourth Byte: ";
	info += std::to_string(IPPart4);

	std::wstring stemp = std::wstring(info.begin(), info.end());
	LPCWSTR result = stemp.c_str();

	hInfoWindow = CreateWindow(L"STATIC", result, WS_CHILD | WS_VISIBLE, 340, 160, 150, 120, hWnd, (HMENU)ID_IPINFO, _hInstance, NULL);

	ChildWindowEnum.push_back(hInfoWindow);

	return hInfoWindow;
}

HWND CreateComboBox(HWND hParent, HINSTANCE hInst)
{
	HWND hComboBox = CreateWindow(L"COMBOBOX", TEXT("ddd"), CBS_SIMPLE | CBS_HASSTRINGS | WS_VSCROLL | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
			400, 250, 200, 100, hParent, (HMENU)ID_COMBOBOX, hInst, NULL);

	TCHAR SomeOption[9][5] = { TEXT("1111"), TEXT("2222"), TEXT("3333"), TEXT("4444"),
								TEXT("5555"), TEXT("6666"), TEXT("7777"), TEXT("8888"), TEXT("9999") };

	TCHAR A[255];

	memset(&A, 0, sizeof(A));
	for (int i = 0; i <= 8; i++)
	{
		wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)SomeOption[i]);

		SendMessage(hComboBox, (UINT)CB_ADDSTRING, 0, (LPARAM)A);
	}

	SendMessage(hComboBox, CB_SETCURSEL, 0, 0);

	ChildWindowEnum.push_back(hComboBox);

	return hComboBox;
}

HWND CreateToolbar(HWND hWndParent, HINSTANCE hInst)
{
	HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | TBSTYLE_WRAPABLE, 0, 0, 100, 40, hWndParent, NULL, hInst, NULL);

	SendMessage(hWndToolbar, TB_LOADIMAGES, (WPARAM)IDB_STD_SMALL_COLOR, (LPARAM)HINST_COMMCTRL);

	TBBUTTON tbButtons[3] =
	{
		{ MAKELONG(STD_FILENEW,  0), IDM_NEW,  TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)L"New" },
		{ MAKELONG(STD_FILEOPEN, 0), IDM_OPEN, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)L"Open"},
		{ MAKELONG(STD_FILESAVE, 0), IDM_SAVE, 0,               BTNS_AUTOSIZE, {0}, 0, (INT_PTR)L"Save"}
	};

	SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM)3, (LPARAM)&tbButtons);

	SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
	ShowWindow(hWndToolbar, TRUE);

	ChildWindowEnum.push_back(hWndToolbar);

	return hWndToolbar;
}
