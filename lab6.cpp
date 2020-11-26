#pragma warning(disable : 4996)

#define ID_FILE_OPEN		9001
#define ID_DELETE			9002
#define IDM_EXIT			9003
#define ID_BUTTONCONSOLE	9004
#define ID_BUTTONCHECK		9005
#define ID_THREAD1			9006
#define ID_RICHEDIT			9007
#define ID_THREAD2			9008
#define ID_THREADCONTEXT	9009
#define ID_SECONDWINDOW		9010
#define ID_PIPE_LOG			9011

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <process.h>
#include <stdlib.h>
#include <richedit.h>
#include <string.h>
#include <wchar.h>

TCHAR Notepad[] = L"notepad ReadMe.txt";
TCHAR PipeName[] = L"\\\\.\\pipe\\Pipe";
HINSTANCE _hInstance;
RECT _Rect = {};
HWND hwndMAIN;
HWND hwndSecond;

WNDPROC OriginProc;

unsigned long VAR = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SecondWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

unsigned __stdcall _start_address(void* param);
unsigned __stdcall _second_thread(void* param);
unsigned __stdcall _third_thread(void* param);
unsigned __stdcall _context_thread(void* param);
unsigned __stdcall _second_start_adress(void* param);

void LastError(const char* phase);

LPCWSTR ContextLog;

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

	if (hwnd = CreateWindowEx(NULL, wc.lpszClassName, L"Window", WS_OVERLAPPEDWINDOW, 350, 100, 800, 500, nullptr, nullptr, wc.hInstance, nullptr); hwnd == INVALID_HANDLE_VALUE)
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
	DWORD						exitCode;
	static LARGE_INTEGER		Create;
	static LARGE_INTEGER		Exit;
	static LARGE_INTEGER		kernel;
	static LARGE_INTEGER		user;
	static LARGE_INTEGER		freq;
	static LARGE_INTEGER		End;
	static LARGE_INTEGER		Start;
	static __int64				kernelTime;
	static __int64				userTime;
	static __int64				totalTime;
	RECT						_hRect;

	static STARTUPINFO				tin;
	static PROCESS_INFORMATION		pInfo;

	//----------------------------------
	CONTEXT Context = { sizeof(CONTEXT) };

	Context.ContextFlags = CONTEXT_ALL;
	//----------------------------------

	_hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hWnd, GWLP_HINSTANCE));
	hwndMAIN = hWnd;

	switch (message)
	{
		case WM_CREATE:
		{
			if (lParam == 10)
			{
				AllocConsole();
				HANDLE hWrite = CreateFile(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

				if (hWrite == INVALID_HANDLE_VALUE)
				{
					LastError("hWrite = INVALID_HANDLE_VALUE");
				}
				else
				{
					DWORD ActualWritten = 0;

					WriteConsole(hWrite, ContextLog, wcslen(ContextLog), &ActualWritten, NULL);
				}
			}
			else if (lParam == 20)
			{
				hwndSecond = CreateWindowEx(NULL, L"MainWindow", L"Window", WS_OVERLAPPEDWINDOW, 100, 100, 800, 500, nullptr, nullptr, _hInstance, nullptr);

				OriginProc = (WNDPROC)SetWindowLongPtr(hwndSecond, GWLP_WNDPROC, (LONG_PTR)SecondWndProc);
				SendMessage(hwndSecond, WM_CREATE, 0, 0);

				ShowWindow(hwndSecond, 1);
			}
			else if (lParam == 30)
			{
				HANDLE hNamedPipe;

				if (!(hNamedPipe = CreateFile(PipeName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)))
				{
					LastError("CreateFile(Pipe)");

					break;
				}

				DWORD bytesRead = 0;
				DWORD bytesWritten = 0;

				DWORD totalRead = 0;

				WCHAR buffer[1024];

				memset(&buffer, 0, 1024);

				if (!ReadFile(hNamedPipe, buffer, sizeof(buffer), &bytesWritten, NULL))
				{
					LastError("ReadPipe");

					break;
				}

				SetWindowText((HWND)GetDlgItem(hWnd, ID_RICHEDIT), buffer);

				CloseHandle(hNamedPipe);

				break;
			}
			else
			{
				tin.cb = sizeof(STARTUPINFO);
				tin.dwFlags = STARTF_USESHOWWINDOW;
				tin.wShowWindow = SW_SHOWNORMAL;

				GetClientRect(hwndMAIN, &_Rect);

				HWND hButton1 = CreateWindow(L"BUTTON", L"OpenNote", WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
					_Rect.left + 20, _Rect.top + 20, 70, 30, hWnd, (HMENU)ID_FILE_OPEN, _hInstance, NULL);

				HWND hButton2 = CreateWindow(L"BUTTON", L"KillProc", WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
					_Rect.left + 670, _Rect.top + 400, 70, 30, hWnd, (HMENU)ID_DELETE, _hInstance, NULL);

				HWND hButton3 = CreateWindow(L"BUTTON", L"ConsoleProc", WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
					_Rect.left + 20, _Rect.top + 70, 90, 30, hWnd, (HMENU)ID_BUTTONCONSOLE, _hInstance, NULL);

				HWND hButton4 = CreateWindow(L"BUTTON", L"Check", WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
					_Rect.left + 670, _Rect.top + 50, 70, 30, hWnd, (HMENU)ID_BUTTONCHECK, _hInstance, NULL);

				HWND hButton5 = CreateWindow(L"BUTTON", L"Thread_1", WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
					_Rect.left + 20, _Rect.top + 120, 70, 30, hWnd, (HMENU)ID_THREAD1, _hInstance, NULL);

				HWND hButton6 = CreateWindow(L"BUTTON", L"Thread_2", WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
					_Rect.left + 20, _Rect.top + 170, 70, 30, hWnd, (HMENU)ID_THREAD2, _hInstance, NULL);

				HWND hButton7 = CreateWindow(L"BUTTON", L"ThreadContext", WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
					_Rect.left + 20, _Rect.top + 220, 100, 30, hWnd, (HMENU)ID_THREADCONTEXT, _hInstance, NULL);

				HWND hButton8 = CreateWindow(L"BUTTON", L"SecondWindow", WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
					_Rect.left + 20, _Rect.top + 270, 100, 30, hWnd, (HMENU)ID_SECONDWINDOW, _hInstance, NULL);

				HWND hButton9 = CreateWindow(L"BUTTON", L"SyncPipe", WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
					_Rect.left + 320, _Rect.top + 340, 100, 30, hWnd, (HMENU)ID_PIPE_LOG, _hInstance, NULL);

				LoadLibrary(L"riched20.dll");

				GetClientRect(hwndMAIN, &_Rect);

				HWND hRichEdit = CreateWindowEx(NULL, RICHEDIT_CLASS, NULL,
					WS_VISIBLE | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_NOHIDESEL | ES_AUTOVSCROLL | ES_MULTILINE | ES_SAVESEL | ES_SUNKEN,
					_Rect.right - 560, _Rect.top + 150, 250, 120, hWnd, (HMENU)ID_RICHEDIT, _hInstance, NULL);
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

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case ID_SECONDWINDOW:
				{
					SendMessage(hWnd, WM_CREATE, 0, 20);

					break;
				}

				case ID_BUTTONCHECK:
				{
					GetExitCodeProcess(pInfo.hProcess, &exitCode);

					std::string test1 = std::to_string(exitCode);
					test1 += "\nCurrent Prority: ";
					test1 += std::to_string(GetPriorityClass(GetCurrentProcess()));
					MessageBoxA(NULL, test1.c_str(), "testx", MB_OK);

					break;
				}
				
				case ID_FILE_OPEN:
				{
					GetExitCodeProcess(pInfo.hProcess, &exitCode);
					if (exitCode != STILL_ACTIVE)
					{
						CreateProcess(NULL, Notepad, NULL, NULL, FALSE, 0, NULL, NULL, &tin, &pInfo);
					}

					break;
				}

				case ID_PIPE_LOG:
				{
					SendMessage(hWnd, WM_CREATE, 0, 30);

					break;
				}
				
				case ID_BUTTONCONSOLE:
				{
				
					GetExitCodeProcess(pInfo.hProcess, &exitCode);

					if (exitCode != STILL_ACTIVE)
					{
						CreateProcess(L"C:\\windows\\system32\\cmd.exe", NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &tin, &pInfo);
					}
				
					break;
				}

				case ID_DELETE:
				{
					GetExitCodeProcess(pInfo.hProcess, &exitCode);

					if (exitCode == STILL_ACTIVE)
					{
						TerminateProcess(pInfo.hProcess, 0);
						MessageBox(hWnd, L"Proccess killed while was active", L"!", MB_OK | MB_ICONEXCLAMATION);
					}

					break;
				}

				case ID_THREAD1:
				{
					HANDLE hThread;

					/*if (hThread = (HANDLE)_beginthreadex(NULL, 0, _start_address, NULL, 0, NULL)) 
					{
						WaitForSingleObject(hThread, INFINITE);
						GetThreadTimes(hThread, (FILETIME*)&Create.u, (FILETIME*)&Exit.u, (FILETIME*)&kernel.u, (FILETIME*)&user.u);

						CloseHandle(hThread);

						kernelTime = kernel.QuadPart;
						userTime = user.QuadPart;
						totalTime = Exit.QuadPart - Create.QuadPart;

						std::string temp = "KernelTime: ";
						temp += std::to_string(kernelTime);
						temp += "\nUserTime: ";
						temp += std::to_string(userTime);
						temp += "\nTotalTime: ";
						temp += std::to_string(totalTime);

						std::wstring stemp = std::wstring(temp.begin(), temp.end());
						LPCWSTR Log = stemp.c_str();

						SetWindowText((HWND)GetDlgItem(hWnd, ID_RICHEDIT), Log);
					}*/

					QueryPerformanceFrequency(&freq);

					Sleep(0);

					QueryPerformanceCounter(&Start);

					if (hThread = (HANDLE)_beginthreadex(NULL, 0, _start_address, NULL, 0, NULL))
					{
						WaitForSingleObject(hThread, INFINITE);

						QueryPerformanceCounter(&End);

						std::string temp = "total Time: ";
						temp += std::to_string(((End.QuadPart - Start.QuadPart) * 10000) / freq.QuadPart);

						std::wstring stemp = std::wstring(temp.begin(), temp.end());
						LPCWSTR Log = stemp.c_str();

						SetWindowText((HWND)GetDlgItem(hWnd, ID_RICHEDIT), Log);
					}

					break;
				}

				case ID_THREAD2:
				{
					std::string temp = std::to_string(VAR);
					std::wstring stemp = std::wstring(temp.begin(), temp.end());
					LPCWSTR Log = stemp.c_str();

					MessageBox(hWnd, L"Thread_2_1!", Log, MB_OK);

					_beginthreadex(NULL, 0, _second_thread, NULL, 0, NULL);
					_beginthreadex(NULL, 0, _third_thread, NULL, 0, NULL);

					break;
				}

				case ID_THREADCONTEXT:
				{

					HWND hThread = reinterpret_cast<HWND>(_beginthreadex(NULL, 0, _context_thread, NULL, 0, NULL));

					if (hThread)
					{
						SuspendThread(hThread);

						if (!GetThreadContext(hThread, &Context))
						{
							LastError("GetThreadContext");
						}
						else
						{
							std::string TContext = "Context:\nSegCs: ";
							TContext += std::to_string(Context.SegCs);
							TContext += "\nSegDs:";
							TContext += std::to_string(Context.SegDs);
							TContext += "\nSegEs:";
							TContext += std::to_string(Context.SegEs);
							TContext += "\nSegFs:";
							TContext += std::to_string(Context.SegFs);
							TContext += "\nSegGs:";
							TContext += std::to_string(Context.SegGs);
							TContext += "\nSegSs:";
							TContext += std::to_string(Context.SegSs);

							std::wstring stemp = std::wstring(TContext.begin(), TContext.end());
							ContextLog = stemp.c_str();

							SendMessage(hWnd, WM_CREATE, 0, 10);
						}

					}
				
					break;
				}

				default: return DefWindowProc(hWnd, message, wParam, lParam);
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

unsigned __stdcall _start_address(void* param)
{
	for (int i = 0; i < 100000; i++);

	std::string temp = std::to_string(VAR);
	std::wstring stemp = std::wstring(temp.begin(), temp.end());
	LPCWSTR Log = stemp.c_str();

	MessageBox((HWND)param, L"Process Worked!", Log, MB_OK);

	return 0;
}

unsigned __stdcall _second_thread(void* param)
{
	Sleep(5000);

	InterlockedExchangeAdd(&VAR, 1);

	return 0;
}

unsigned __stdcall _third_thread(void* param)
{
	std::string temp = std::to_string(InterlockedExchangeAdd(&VAR, 1));
	std::wstring stemp = std::wstring(temp.begin(), temp.end());
	LPCWSTR Log = stemp.c_str();

	MessageBox(hwndMAIN, L"Thread_2_2!", Log, MB_OK);

	return 0;
}

unsigned __stdcall _context_thread(void* param)
{
	return 0;
}

LRESULT CALLBACK SecondWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
		{
			ShowWindow((HWND)GetDlgItem(hwndSecond, ID_PIPE_LOG), 0);

			break;
		}

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case ID_THREADCONTEXT:
				{
					if (!_beginthreadex(NULL, 0, _second_start_adress, NULL, 0, NULL))
					{
						LastError("Couldn`t start process");
					}

					break;
				}

				default: break;
			}

			break;
		}

		case WM_DESTROY:
		{
			DestroyWindow(hWnd);

			break;
		}

		default: break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

unsigned __stdcall _second_start_adress(void* param)
{
	SendMessage(hwndMAIN, WM_COMMAND, ID_THREADCONTEXT, 0);

	SendMessage((HWND)GetDlgItem(hwndMAIN, ID_RICHEDIT), WM_SETTEXT, FALSE, (LPARAM)L"SecondWindow Worked!");
	UpdateWindow((HWND)GetDlgItem(hwndMAIN, ID_RICHEDIT));

	return 0;
}

void LastError(const char* phase)
{
	std::string temp = std::to_string(GetLastError());

	MessageBoxA(hwndMAIN, phase, temp.c_str(), MB_OK);
}