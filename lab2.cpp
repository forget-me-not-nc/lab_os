#define ID_RICHTEXT 1
#define ID_BUTTON 2

#include <Windows.h>
#include <richedit.h>
#include <gdiplus.h>

using namespace Gdiplus;

LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void DrawObject(HDC hdc, HWND hWnd);

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR szCmdLine, int nCmdShow)
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplsuToken;
	Gdiplus::GdiplusStartup(&gdiplsuToken, &gdiplusStartupInput, nullptr);
	
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

	if (hwnd = CreateWindowEx(NULL, wc.lpszClassName, L"Window", WS_OVERLAPPEDWINDOW , 350, 350, 800, 500, nullptr, nullptr, wc.hInstance, nullptr); hwnd == INVALID_HANDLE_VALUE)
	{
		return EXIT_FAILURE;
	}

	ShowWindow(hwnd,nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Gdiplus::GdiplusShutdown(gdiplsuToken);

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE	hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hWnd, GWLP_HINSTANCE));
	RECT		hRect;
	PAINTSTRUCT	ps{};
	HDC			hdc;

	switch (uMsg)
	{
		case WM_CREATE:
		{
			LoadLibrary(L"riched20.dll");

			HWND		hTextLabel;
			HWND		hAnimationButton;
			HWND		hRichEdit;
			HFONT		hFont;

			GetClientRect(hWnd, &hRect);

			hRichEdit = CreateWindowEx(NULL, RICHEDIT_CLASS, NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_NOHIDESEL | ES_AUTOVSCROLL | ES_MULTILINE | ES_SAVESEL | ES_SUNKEN,
										hRect.left + 40, hRect.top + 100, 300,300, hWnd, reinterpret_cast<HMENU>(ID_RICHTEXT), hInstance, NULL);

			hTextLabel = CreateWindow(L"STATIC", L"Text Editor:", WS_VISIBLE | WS_CHILD, hRect.left + 40, hRect.top + 80, 100, 20,
										hWnd, nullptr, hInstance, NULL);

			hAnimationButton = CreateWindow(L"BUTTON", L"LOAD", BS_FLAT | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 360, 45, 50, 25, hWnd, (HMENU)ID_BUTTON, hInstance, NULL);
			
			SendMessage(hWnd, WM_SETTEXT, NULL, LOWORD(hRichEdit));

			hFont = CreateFont(-20, 0, 0, 0, FW_BOLD, TRUE, FALSE, TRUE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH,NULL);
			SendMessage(hRichEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

			break;
		}

		case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC)wParam;
			SetTextColor(hdcStatic, RGB(0, 0, 0)); 
			SetBkColor(hdcStatic, RGB(128, 128, 128));

			return (LRESULT)GetStockObject(NULL_BRUSH);
		}
		return 0;

		case WM_PAINT:
		{
			
			hdc = BeginPaint(hWnd, &ps);

			DrawObject(hdc, hWnd);

			EndPaint(hWnd, &ps);

			break;
		}

		case WM_SETTEXT:
		{
			SetWindowText((HWND)lParam, L"YOU_CAN_EDIT_ME");

			break;
		}

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case 2:
				{
					Rect rect = { 500, 20, 200, 40 };
					HDC _hdc = GetDC(hWnd);
					Metafile *emf = new Metafile(L"MetaFile_1.wmf", _hdc);
					Graphics *gf =  new Graphics(emf);
					SolidBrush brush(Color(255, 51, 51, 255));

					gf->SetSmoothingMode(SmoothingModeHighQuality);
					gf->FillRectangle(&brush, rect);
					
					delete gf;
					delete emf;

					CloseMetaFile(_hdc);
					ReleaseDC(hWnd, _hdc);
					///////////////////////////////////////////////////////////
				
					HDC wmf = CreateMetaFile(L"MetaFile_2.wmf");
					HBRUSH hBrush = CreateSolidBrush(RGB(77, 0, 77));
		
					RECT _rect = { 150, 20, 280, 60 };

					SelectObject(wmf, hBrush);
					FillRect(wmf, &_rect, hBrush);

					CloseMetaFile(wmf);

					HDC hdc = GetDC(hWnd);

					PlayMetaFile(hdc, GetMetaFile(L"MetaFile_2.wmf"));

					ReleaseDC(hWnd, hdc);

					Graphics graph(hWnd);
					Image im(L"MetaFile_1.wmf");
					graph.DrawImage(&im, rect);

					break;
				}

				break;
			}

			break;
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

		case WM_DESTROY:
		{
			DestroyWindow(hWnd);
			PostQuitMessage(EXIT_SUCCESS);
			
			break;
		}
		
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void DrawObject(HDC hdc, HWND hWnd)
{

	Gdiplus::Rect RectToPaint = { 371, 106, 28, 288 };

	Gdiplus::Graphics gf(hdc);
	Gdiplus::Pen pen1(Gdiplus::Color(255, 0, 255, 0));
	Gdiplus::Pen pen2(Gdiplus::Color(255, 0, 0, 0));

	for (int i = 370; i < 400; i++)
	{
		gf.DrawLine(&pen2, 370, 103, i, 103);
		gf.DrawLine(&pen2, 370, 397, i, 397);
		Sleep(40);
	}

	for (int i = 103; i < 397; i++)
	{
		gf.DrawLine(&pen2, 370, 103, 370, i);
		gf.DrawLine(&pen2, 400, 103, 400, i);
		Sleep(10);
	}

	for (int i = 0; i < 288; i++)
	{
		gf.DrawLine(&pen1, 371, 395 - i, 399, 395 - i);
		Sleep(1);
	}

	Gdiplus::Rect MFRect = { 430, 100, 305, 300 };
	Gdiplus::Graphics graph(hWnd);
	Gdiplus::Image Metafile(L"hanabi.wmf");
	graph.DrawImage(&Metafile, MFRect);
}

