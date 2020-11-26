#define ID_RICHEDIT		9001
#define ID_PIPE_LOG		9002

#include <windows.h>
#include <richedit.h>
#include <string>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

TCHAR PipeName[] = L"\\\\.\\pipe\\Pipe";
HINSTANCE _hInstance;
RECT _Rect = {};
HWND hwndMain;

void LastError(const char* phase);

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	MSG			msg{};
	HWND		hwnd{};
	WNDCLASSEX	wc{ sizeof(WNDCLASSEX) };

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(GRAY_BRUSH));
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = L"MainWindow";
	wc.lpszMenuName = nullptr;
	wc.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wc))
	{
		return EXIT_FAILURE;
	}

	if (hwnd = CreateWindowEx(NULL, wc.lpszClassName, L"Server", WS_OVERLAPPEDWINDOW, 350, 100, 800, 500, nullptr, nullptr, wc.hInstance, nullptr); hwnd == INVALID_HANDLE_VALUE)
	{
		return EXIT_FAILURE;
	}

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	_hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hWnd, GWLP_HINSTANCE));
	GetClientRect(hWnd, &_Rect);
	hwndMain = hWnd;

	switch (message)
	{
		case WM_CREATE:
		{
			HWND hButton9 = CreateWindow(L"BUTTON", L"SyncPipe", WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
				_Rect.left + 320, _Rect.top + 340, 100, 30, hWnd, (HMENU)ID_PIPE_LOG, _hInstance, NULL);

			LoadLibrary(L"riched20.dll");

			GetClientRect(hWnd, &_Rect);

			HWND hRichEdit = CreateWindowEx(NULL, RICHEDIT_CLASS, NULL,
				WS_VISIBLE | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_NOHIDESEL | ES_AUTOVSCROLL | ES_MULTILINE | ES_SAVESEL | ES_SUNKEN,
				_Rect.right - 560, _Rect.top + 150, 250, 120, hWnd, (HMENU)ID_RICHEDIT, _hInstance, NULL);

			break;
		}

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case ID_PIPE_LOG:
				{
					HANDLE hNamedPipe;

					if (!(hNamedPipe = CreateNamedPipe(PipeName, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE, 1, 0, 0, 0, NULL)))
					{
						LastError("CreateNamedPipe");

						break;
					}

					if (!ConnectNamedPipe(hNamedPipe, NULL))
					{
						LastError("ConnectNamedPipe");
						CloseHandle(hNamedPipe);

						break;
					}

					DWORD bytesWritten = 0;
					DWORD bytesRead = 0;

					WCHAR buffer[1024];

					memset(&buffer, 0, 1024);

					SendMessage((HWND)GetDlgItem(hWnd, ID_RICHEDIT), WM_GETTEXT, 1024, (LPARAM)buffer);

					if (!WriteFile(hNamedPipe, buffer, sizeof(buffer), &bytesRead, NULL))
					{
						LastError("WritePipe");

						break;
					}

					CloseHandle(hNamedPipe);
				}

				default: break;
			}

			break;
		}

		case WM_DESTROY:
		{
			DestroyWindow(hWnd);
			PostQuitMessage(EXIT_SUCCESS);

			break;
		}

		default: return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void LastError(const char* phase)
{
	std::string temp = std::to_string(GetLastError());

	MessageBoxA(hwndMain, phase, temp.c_str(), MB_OK);
}