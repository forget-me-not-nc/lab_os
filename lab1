//#pragma comment(linker,"\"/manifestdependency:type='win32' \
//name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
//processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#define BUTTON1 1

LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HCURSOR hCursor = ::LoadCursorFromFile(L"stella.ico");


int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR szCmdLine, int nCmdShow) 
																					 
{
	MSG			msg{};

	HWND		hwnd{};

	WNDCLASSEX	wc{ sizeof(WNDCLASSEX) };

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));

	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	wc.hIcon = reinterpret_cast<HICON>(LoadImage(hInstance, L"Caesar.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	wc.hIconSm = reinterpret_cast<HICON>(LoadImage(hInstance, L"Caesar.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	
	wc.hInstance = hInstance;

	wc.lpfnWndProc = WinProc; 

	wc.lpszClassName = L"Laba_1"; 
	wc.lpszMenuName = nullptr; 
	wc.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wc))
	{
		return EXIT_FAILURE;
	}

	
	if (hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED, wc.lpszClassName, L"FirstWindow", WS_OVERLAPPEDWINDOW, 100, 100, 500, 500, nullptr, nullptr, wc.hInstance, nullptr); hwnd == INVALID_HANDLE_VALUE)
	{
		return EXIT_FAILURE;
	}

	
	ShowWindow(hwnd, nCmdShow);
	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA); // LWA_COLORKEY
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
			HWND		hButton;
			RECT		Rect;

			GetClientRect(hWnd, &Rect);

			hButton = CreateWindowExW(	NULL, 
										L"BUTTON", 
										L"ANIMATION", 
										BS_FLAT | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 
										Rect.right - 50, Rect.bottom - 50, 40, 40, hWnd, reinterpret_cast<HMENU>(BUTTON1), (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), nullptr);     // ��������� ������

		}
		return 0;

		case WM_SIZE:
		{
			HWND elemControl;
			RECT Rect;

			GetClientRect(hWnd, &Rect);

			elemControl = GetDlgItem(hWnd, BUTTON1);
			SetWindowPos(elemControl, NULL, Rect.right - 160, Rect.bottom - 50, 150, 40, SWP_NOZORDER);

		}
		return 0;

		case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO min_max = (LPMINMAXINFO)lParam;
			min_max->ptMinTrackSize.x = 300;				
			min_max->ptMinTrackSize.y = 300;

		}
		return 0;

		case WM_LBUTTONDOWN:
		{
			MessageBox(hWnd, L"Left mouse-button clicked", L"ALARM", MB_ICONERROR);
			
		}
		return 0;

		/*case WM_MOVING:
		{
							MessageBox(hWnd, L"Dragging", L"Careful", MB_ICONWARNING);
		}*/
		case WM_RBUTTONDOWN:
		{
			MessageBox(hWnd, L"Right mouse-button clicked", L"ALARM", MB_ICONERROR);

		}
		return 0;

		case WM_SETCURSOR:
		{
			::SetCursor(hCursor); 
			
		}
		return 0;

		case WM_PAINT:
		{
			PAINTSTRUCT     ps;		
			HDC				hdc;		
			BITMAP			Bitmap;		
			HDC				hdcMEM;		
			HGDIOBJ			oldBitmap;	
			RECT			Rect;

			GetClientRect(hWnd, &Rect);

			HBITMAP hBitmap = reinterpret_cast<HBITMAP>(LoadImage(NULL, L"sunset.bmp", IMAGE_BITMAP,Rect.right, Rect.bottom,LR_LOADFROMFILE));

			hdc = BeginPaint(hWnd, &ps);
			hdcMEM = CreateCompatibleDC(hdc);

			oldBitmap = SelectObject(hdcMEM, hBitmap);
			
			GetObject(hBitmap, sizeof(Bitmap), &Bitmap);
		
			BitBlt(hdc, 0, 0, Rect.right, Rect.bottom, hdcMEM, 0, 0, SRCCOPY);
			
			SelectObject(hdcMEM, oldBitmap);
			DeleteDC(hdcMEM);

		
			EndPaint(hWnd, &ps);

		}
		return 0;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case 1:
				{
					HWND	hParent = (HWND)lParam;
					HWND	hChild = CreateWindowExA(	WS_EX_TOPMOST,
														"EDIT", 
														"TEST",
														WS_OVERLAPPEDWINDOW, 
														600, 600, 400, 200, hParent, nullptr, (HINSTANCE)GetWindowLongPtr(hParent, GWLP_HINSTANCE), nullptr);

					EnableWindow(hParent, false);
					AnimateWindow(hChild, 1000, AW_CENTER);

				}
				break;


				////////
			}

		}
		return 0;

		case WM_DESTROY:
		{

			DestroyWindow(hWnd);
			PostQuitMessage(EXIT_SUCCESS);	
			
		}
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
