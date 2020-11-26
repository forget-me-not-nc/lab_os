#define ID_SAVEFILEB 9001
#define ID_COLORPANELBUTTON 9002
#define ID_COLOREDTEXT		9003
#define ID_FONTPANELBUTTON		9004
#define ID_FONTTEXT				9005
#define ID_OPENFILESBUTTON			9006
#define ID_RICHEDIT		9007

#include <windows.h>
#include <string>
#include <atlbase.h>
#include <richedit.h>
#include "resource.h"

LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK SaveFProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

HINSTANCE _hInstance;
RECT _Rect = {};
RECT _RectDlg = {};
HWND hwndOwner;
COLORREF Color = 0x00FFFFFF; //DWORD
HFONT hFont;

BOOL LoadFileText(TCHAR pszFileName[]);

VOID SaveFile(TCHAR fileName[]);
VOID ShowColorPanel();
VOID ShowFontPanel();
VOID OpenFiles();
VOID RenewText(INT Param);

using std::string;
using std::wstring;

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR szCmdLine, int nCmdShow)
{
	MSG			msg{};
	HWND		hwnd{};
	WNDCLASSEX	wc{ sizeof(WNDCLASSEX) };

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(GRAY_BRUSH));
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
	hwndOwner = hwnd;

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
			if (LOWORD(wParam) == ID_COLORPANELBUTTON)
			{
				ShowColorPanel();
				
				DestroyWindow((HWND)GetDlgItem(hWnd, ID_COLOREDTEXT));

				RenewText(ID_COLORPANELBUTTON);

				SendMessage((HWND)GetDlgItem(hWnd, ID_COLOREDTEXT), WM_CTLCOLORSTATIC, Color, 0);
			}
			else if (LOWORD(wParam) == ID_FONTPANELBUTTON)
			{
				ShowFontPanel();

				DestroyWindow((HWND)GetDlgItem(hWnd, ID_FONTTEXT));

				RenewText(ID_FONTPANELBUTTON);

				SendMessage((HWND)GetDlgItem(hWnd, ID_FONTTEXT), WM_SETFONT, (WPARAM)hFont, 0);
			}
			else if (LOWORD(wParam) == ID_OPENFILESBUTTON)
			{
				OpenFiles();
			}
			else
			{
				GetClientRect(hWnd, &_Rect);

				HWND hSaveFileButton = CreateWindow(L"BUTTON", L"Save File", BS_FLAT | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, _Rect.left + 40, _Rect.top + 230, 125, 40,
					hWnd, (HMENU)ID_SAVEFILEB, _hInstance, NULL);

				HWND hButton1 = CreateWindow(L"BUTTON", L"TextColor", BS_FLAT | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, _Rect.left + 40, _Rect.top + 180, 125, 40,
						hWnd, (HMENU)ID_COLORPANELBUTTON, _hInstance, NULL);
				
				HWND hButton2 = CreateWindow(L"BUTTON", L"SetFont", BS_FLAT | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, _Rect.left + 40, _Rect.top + 130, 125, 40,
					hWnd, (HMENU)ID_FONTPANELBUTTON, _hInstance, NULL);

				HWND hButton3 = CreateWindow(L"BUTTON", L"OpenFiles", BS_FLAT | WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, _Rect.left + 40, _Rect.top + 280, 125, 40,
					hWnd, (HMENU)ID_OPENFILESBUTTON, _hInstance, NULL);
			
				LoadLibrary(L"riched20.dll");

				HWND hRichEdit = CreateWindowEx(NULL, RICHEDIT_CLASS, NULL,
					WS_VISIBLE | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_NOHIDESEL | ES_AUTOVSCROLL | ES_MULTILINE | ES_SAVESEL | ES_SUNKEN,
					_Rect.right - 500, _Rect.top + 280, 450, 350, hWnd, (HMENU)ID_RICHEDIT, _hInstance, NULL);
			}

			break;
		}

		case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC)wParam;
			SetTextColor(hdcStatic, RGB(0, 0, 0));
			SetBkColor(hdcStatic, Color);

			return (LRESULT)GetStockObject(NULL_BRUSH);
		}

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case ID_SAVEFILEB:
				{
					ShowWindow(CreateDialog(_hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwndOwner, SaveFProc), 1);
				
					break;
				}

				case ID_COLORPANELBUTTON:
				{
					SendMessage(hWnd, WM_CREATE, ID_COLORPANELBUTTON, 0);

					break;
				}

				case ID_FONTPANELBUTTON:
				{
					SendMessage(hWnd, WM_CREATE, ID_FONTPANELBUTTON, 0);
				
					break;
				}

				case ID_OPENFILESBUTTON:
				{
					SendMessage(hWnd, WM_CREATE, ID_OPENFILESBUTTON, 0);

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

BOOL CALLBACK SaveFProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
		{
			if ((hwndOwner = GetParent(hwndDlg)) == NULL)
			{
				hwndOwner = GetDesktopWindow();
			}

			GetWindowRect(hwndOwner, &_Rect);

			SetWindowPos(hwndDlg, HWND_TOP, _Rect.left + 300, _Rect.top + 300, 0, 0, SWP_NOSIZE);

			return TRUE;
		}

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDOK:
				{
					TCHAR FileName[MAX_PATH];
					
					::ZeroMemory(FileName, MAX_PATH);

					GetDlgItemText(hwndDlg, IDC_EDIT1, FileName, MAX_PATH);

					SaveFile(FileName);

					SendMessage(hwndDlg, WM_DESTROY, 0, 0);

					break;
				}

				case IDCANCEL:
				{
					SendMessage(hwndDlg, WM_DESTROY, 0, 0);
					
					break;
				}

				default:break;
			}

			break;
		}

		case WM_DESTROY:
		{
			EndDialog(hwndDlg, 0);

			break;
		}

		default: return FALSE;
	}

	return TRUE;
}

VOID SaveFile(TCHAR fileName[])
{
	HANDLE FFile;
	WIN32_FIND_DATA fd;

	if (FFile = FindFirstFile(L".\*", &fd); FFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(hwndOwner, L"Could not open project directory..", L"Error", MB_ICONERROR);

		FindClose(FFile);

		return;
	}

	wstring wTempFN(&fileName[0]);
	string TempFN(wTempFN.begin(), wTempFN.end());

	if (TempFN.find_last_of('.') == string::npos)
	{
		TempFN += ".txt";
	}

	::ZeroMemory(fileName, MAX_PATH);

	for (int i = 0; i < TempFN.length(); i++)
	{
		fileName[i] = TempFN[i];
	}

	if (FFile = CreateFile(fileName, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0); GetLastError() == ERROR_FILE_EXISTS)
	{
		MessageBox(hwndOwner, L"File already exist!", L"Could not create file.", MB_ICONERROR);

		return;
	}

	TCHAR toWrite[] = L"---------Created custom file by user..---------";
	DWORD bytesWritten;

	if (WriteFile(FFile, toWrite, sizeof(toWrite), &bytesWritten, NULL); GetLastError())
	{
		int index1 = GetLastError();
		std::string test1 = std::to_string(index1);
		MessageBoxA(NULL, test1.c_str(), "testx", MB_OK);
	}

	CloseHandle(FFile);
}

VOID ShowColorPanel()
{
	CHOOSECOLOR		cc;                 
	COLORREF		acrCustClr[16];         
	HBRUSH			hBrush;                        

	ZeroMemory(&cc, sizeof(cc));

	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hwndOwner;
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.rgbResult = Color;
	cc.Flags = CC_SOLIDCOLOR | CC_RGBINIT;

	if (ChooseColor(&cc) == TRUE)
	{
		Color = cc.rgbResult;
	}
}

VOID ShowFontPanel()
{               
	HDC				hdc;               
	CHOOSEFONT		cf;           
	static LOGFONT	lf;        
	HFONT			hFontPrev;
	DWORD			rgbPrev;

	ZeroMemory(&cf, sizeof(cf));

	cf.lStructSize = sizeof(cf);
	cf.hwndOwner = hwndOwner;
	cf.lpLogFont = &lf;
	cf.rgbColors = Color;
	cf.Flags = CF_SCREENFONTS | CF_EFFECTS;

	hdc = GetDC(hwndOwner);

	if (ChooseFont(&cf) == TRUE)
	{
		hFont = CreateFontIndirect(cf.lpLogFont);
		hFontPrev = (HFONT)SelectObject(hdc, hFont);
		Color = cf.rgbColors;
	}
}

VOID RenewText(INT Param)
{
	if (Param == ID_COLORPANELBUTTON)
	{
		HWND hText = CreateWindow(L"STATIC", L"<<<TEXT TO SHOW BACHKGROUNG COLOR CHANGE>>>", WS_CHILD | WS_VISIBLE, _Rect.right - 500, _Rect.bottom - 600,
				400, 20, hwndOwner, (HMENU)ID_COLOREDTEXT, _hInstance, 0);

	}
	else if (Param == ID_FONTPANELBUTTON)
	{
		HWND hText = CreateWindow(L"STATIC", L"<<<TEXT TO SHOW FONT CHANGE>>>", WS_CHILD | WS_VISIBLE, _Rect.right - 500, _Rect.bottom - 500,
			400, 100, hwndOwner, (HMENU)ID_FONTTEXT, _hInstance, 0);
	}
}

VOID OpenFiles()
{
	OPENFILENAME ofn;
	TCHAR FileName[MAX_PATH];

	ZeroMemory(&ofn, sizeof(ofn));
	ZeroMemory(FileName, MAX_PATH);

	ofn.lStructSize = sizeof(ofn); 
	ofn.hwndOwner = hwndOwner;
	ofn.lpstrFilter = L"C++ Files(*.cpp)\0*.cpp\0Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = FileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = L"txt";

	if (GetOpenFileName(&ofn))
	{
		//to do with file
	}
}

