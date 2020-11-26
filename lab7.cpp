#define	ID_ALLOCMEMBUTTON	9001
#define ID_ALLOCPROTECTED	9002
#define ID_REPEAT			9003
#define ID_CREATEFILE		9004
#define ID_RICHEDIT			9005

#include <windows.h>
#include <richedit.h>
#include <string>
#include <process.h>
#include <psapi.h>
#include <cstdio>
#include <Winnt.h>
#include <richedit.h>

//----------------------------
TCHAR Notepad[] = L"notepad FileForMap.txt";
//----------------------------
HINSTANCE _hInstance;
HWND hwndMain;
//----------------------------
DWORD _pSize = 0;
INT _pages = 0;
DWORD _allocatedPages = 0;
LPTSTR _nextPage;
//----------------------------
LPVOID _protAddress;
BOOL _pressed = FALSE;
//----------------------------
static STARTUPINFO _sInfo;
static PROCESS_INFORMATION _procInfo;
//----------------------------
DWORD _exCode;
VOID FileMapping();
//----------------------------
VOID LastError(const char* phase);
VOID _catch_exept(LPVOID _baseAddress);
INT _exept_filter(DWORD _exeptCode);
//----------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//----------------------------
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

	if (hwnd = CreateWindowEx(NULL, wc.lpszClassName, L"Main_Window", WS_OVERLAPPEDWINDOW, 350, 100, 800, 500, nullptr, nullptr, wc.hInstance, nullptr); hwnd == INVALID_HANDLE_VALUE)
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
	hwndMain = hWnd;

	SYSTEM_INFO _sysInfo = { sizeof(SYSTEM_INFO) };
	memset(&_sysInfo, 0, sizeof(SYSTEM_INFO));

	MEMORY_BASIC_INFORMATION _mbi;
	memset(&_mbi, 0, sizeof(MEMORY_BASIC_INFORMATION));

	switch (message)
	{
		case WM_CREATE:
		{
			if (lParam == ID_ALLOCPROTECTED)
			{
				HWND hRepeatButton = CreateWindow(L"BUTTON", L"<-----Repeat", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 240, 160, 100, 30, hWnd,
					(HMENU)ID_REPEAT, _hInstance, NULL);

				break;
			}
			else
			{
				_sInfo.cb = sizeof(STARTUPINFO);
				_sInfo.dwFlags = STARTF_USESHOWWINDOW;
				_sInfo.wShowWindow = SW_SHOWNORMAL;

				HWND hButton1 = CreateWindow(L"BUTTON", L"AllocateMemory", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 40, 100, 110, 30, hWnd,
					(HMENU)ID_ALLOCMEMBUTTON, _hInstance, NULL);

				HWND hButton2 = CreateWindow(L"BUTTON", L"AllocateProtectedMemory", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 40, 160, 180, 30, hWnd,
					(HMENU)ID_ALLOCPROTECTED, _hInstance, NULL);

				HWND hButton3 = CreateWindow(L"BUTTON", L"CreateFile", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 40, 230, 100, 30, hWnd,
					(HMENU)ID_CREATEFILE, _hInstance, NULL);

				LoadLibrary(L"riched20.dll");

				HWND hRichEdit = CreateWindowEx(NULL, RICHEDIT_CLASS, NULL,
					WS_VISIBLE | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_NOHIDESEL | ES_AUTOVSCROLL | ES_MULTILINE | ES_SAVESEL | ES_SUNKEN,
					350, 150, 250, 230, hWnd, (HMENU)ID_RICHEDIT, _hInstance, NULL);

				DWORD _maxSize;
				DWORD _minSize;

				GetProcessWorkingSetSize(GetCurrentProcess(), &_minSize, &_maxSize);

				std::string _sSize = "WorkingSize: Min: ";
				_sSize += std::to_string(_minSize / 1024);
				_sSize += " Max: ";
				_sSize += std::to_string(_maxSize / 1024);

				MessageBoxA(hWnd, _sSize.c_str(), "Size", MB_OK);

				if (!SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1))
				{
					LastError("!SPWS");
				}

				GetProcessWorkingSetSize(GetCurrentProcess(), &_minSize, &_maxSize);

				_sSize = "Min: ";
				_sSize += std::to_string(_minSize / 1024);
				_sSize += " Max: ";
				_sSize += std::to_string(_maxSize / 1024);

				MessageBoxA(hWnd, _sSize.c_str(), "Size", MB_OK);

				break;
			}

			break;
		}

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case ID_ALLOCMEMBUTTON:
				{
					LPVOID		_baseAddress;
					MEMORY_BASIC_INFORMATION _mbi;

					memset(&_mbi, 0, sizeof(MEMORY_BASIC_INFORMATION));

					GetSystemInfo(&_sysInfo);

					_pSize = _sysInfo.dwPageSize;
					_pages = 10;

					std::string pSize = std::to_string(_pSize);
					MessageBoxA(hWnd, pSize.c_str(), "_pSize", MB_OK);

					if (_baseAddress = VirtualAlloc(NULL, _pages * _pSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); _baseAddress == NULL)
					{
						LastError("VirtualAlloc(Reserve|Commit)");
						
						break;
					}

					_catch_exept(_baseAddress);

					VirtualFree(_baseAddress, 0, MEM_RELEASE);

					break;
				}

				case ID_ALLOCPROTECTED:
				{
					GetSystemInfo(&_sysInfo);

					_pSize = _sysInfo.dwPageSize;
					_pages = 1;

					std::string pSize = std::to_string(_pSize);
					MessageBoxA(hWnd, pSize.c_str(), "_pSize", MB_OK);

					if (_protAddress = VirtualAlloc(NULL, _pages * _pSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE | PAGE_GUARD); _protAddress == NULL)
					{
						LastError("(Protected)VirtualAlloc(Reserve|Commit)");

						break;
					}

					if (!VirtualQuery(_protAddress, &_mbi, sizeof(MEMORY_BASIC_INFORMATION))) LastError("!VirtualQuery");
					else
					{
						std::string _temp;

						std::string _addInfo = "AddresInfo:\n";
						_addInfo += "AllocationProtect: ";
						_addInfo += std::to_string(_mbi.AllocationProtect);
						_addInfo += "\n";
						_addInfo += "Protect: ";
						_addInfo += std::to_string(_mbi.Protect); 
						_addInfo += "\n";
						_addInfo += "RegionSize: ";
						_addInfo += std::to_string(_mbi.RegionSize);
						_addInfo += "\n";
						_addInfo += "State: ";
						_addInfo += std::to_string(_mbi.State);
						_addInfo += "\n";
						_addInfo += "Type: ";
						_addInfo += std::to_string(_mbi.Type);
						_addInfo += "\n";

						std::wstring temp = std::wstring(_addInfo.begin(), _addInfo.end());
						LPCWSTR Log = temp.c_str();

						SetWindowText((HWND)GetDlgItem(hWnd, ID_RICHEDIT), Log);
					}

					_catch_exept(_protAddress);

					SendMessage(hWnd, WM_CREATE, 0, ID_ALLOCPROTECTED);
				
					break;
				}

				case ID_REPEAT:
				{
					_pressed = TRUE;

					if (!VirtualQuery(_protAddress, &_mbi, sizeof(MEMORY_BASIC_INFORMATION))) LastError("!VirtualQuery");
					else
					{
						std::string _addInfo = "AddresInfo:\n";
						_addInfo += "AllocationBase: ";
						_addInfo += std::string((CHAR*)_mbi.AllocationBase);
						_addInfo += "\n";
						_addInfo += "AllocationProtect: ";
						_addInfo += std::to_string(_mbi.AllocationProtect);
						_addInfo += "\n";
						_addInfo += "BaseAddress: ";
						_addInfo += std::string((CHAR*)_mbi.BaseAddress);
						_addInfo += "\n";
						_addInfo += "Protect: ";
						_addInfo += std::to_string(_mbi.Protect);
						_addInfo += "\n";
						_addInfo += "RegionSize: ";
						_addInfo += std::to_string(_mbi.RegionSize);
						_addInfo += "\n";
						_addInfo += "State: ";
						_addInfo += std::to_string(_mbi.State);
						_addInfo += "\n";
						_addInfo += "Type: ";
						_addInfo += std::to_string(_mbi.Type);
						_addInfo += "\n";

						std::wstring temp = std::wstring(_addInfo.begin(), _addInfo.end());
						LPCWSTR Log = temp.c_str();

						SetWindowText((HWND)GetDlgItem(hWnd, ID_RICHEDIT), Log);
					}

					_catch_exept(_protAddress);

					VirtualFree(_protAddress, 0, MEM_RELEASE);

					DestroyWindow((HWND)GetDlgItem(hWnd, ID_REPEAT));

					break;
				}

				case ID_CREATEFILE:
				{
					GetExitCodeProcess(_procInfo.hProcess, &_exCode);

					if (_exCode != STILL_ACTIVE)
					{
						if (CreateProcess(NULL, Notepad, NULL, NULL, FALSE, PROCESS_VM_OPERATION, NULL, NULL, &_sInfo, &_procInfo))
						{
							WaitForSingleObject(_procInfo.hProcess, INFINITE);
							
							FileMapping();
						}
						else
						{
							LastError("!CreateProc(Notepad)");
						}
					}
					else
					{
						LastError("STILL_ACTIVE(Notepad)");
					}

					break;
				}

				default: break;
			}
			
			break;
		}

		case WM_DESTROY:
		{
			if (!_pressed)
			{
				if (_protAddress) VirtualFree(_protAddress, 0, MEM_RELEASE);
			}

			DestroyWindow(hWnd);
			PostQuitMessage(EXIT_SUCCESS);

			break;
		}

		default: return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

VOID LastError(const char* phase)
{
	std::string temp = std::to_string(GetLastError());

	MessageBoxA(hwndMain, phase, temp.c_str(), MB_OK);
}

VOID _catch_exept(LPVOID _baseAddress)
{
	CHAR* _charsets = static_cast<CHAR*>(_baseAddress);
	
	_nextPage = (LPTSTR)_baseAddress;

	for (DWORD _iter = 0; _iter < (_pages * _pSize); _iter++)
	{
		__try
		{
			_charsets[_iter] = '~';
		}
		__except (_exept_filter(GetExceptionCode()))		
		{
			return;
		}
	}

}

INT _exept_filter(DWORD _exeptCode)
{
	LPVOID _newAddress;

	if (_exeptCode != EXCEPTION_ACCESS_VIOLATION)
	{
		LastError("EXCEPTION_ACCESS_VIOLATION");

		return EXCEPTION_EXECUTE_HANDLER;
	}

	if (_allocatedPages >= _pages)
	{
		LastError("Out of PAGE_LIMIT");

		return EXCEPTION_EXECUTE_HANDLER;
	}

	if (_newAddress = VirtualAlloc((LPVOID)_nextPage, _pSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); _newAddress == NULL)
	{
		LastError("_newAddress == NULL");

		return EXCEPTION_EXECUTE_HANDLER;
	}

	_allocatedPages++;
	_nextPage = (LPTSTR)((PCHAR)_nextPage + _pSize);

	return EXCEPTION_CONTINUE_EXECUTION;
}

VOID FileMapping()
{
	GetExitCodeProcess(_procInfo.hProcess, &_exCode);

	if (_exCode == STILL_ACTIVE)
	{
		LastError("PROC - STILL_ACTIVE");

		return;
	}

	HANDLE hFile = CreateFile(L"FileForMap.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		LastError("INVALID_HANDLE_VALUE");

		return;
	}

	DWORD _fsize = GetFileSize(hFile, NULL);

	if (_fsize == INVALID_FILE_SIZE)
	{
		CloseHandle(hFile);
		LastError("INVALID_FILE_SIZE");

		return;
	}

	HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, NULL, NULL, NULL);

	if (!hMapping)
	{
		CloseHandle(hFile);
		LastError("CreateFileMapping");

		return;
	}

	CHAR* _fdat = (CHAR*)MapViewOfFile(hMapping, FILE_MAP_READ, NULL, NULL, _fsize);

	CHAR* _buff = (CHAR*)malloc(30);
	DWORD _bRead;

	const CHAR* _toWrite = "Written by WriteProcessMemory";

	if (!_fdat)
	{
		CloseHandle(hMapping);
		CloseHandle(hFile);
		LastError("Couldn`t get address");

		return;
	}
	else
	{
		DWORD  _oldRights = NULL;

		if (VirtualProtectEx(GetCurrentProcess(), _fdat, 30, PAGE_WRITECOPY, &_oldRights))
		{
			if (!WriteProcessMemory(GetCurrentProcess(), _fdat, (LPVOID)_toWrite, 30, &_bRead))
			{
				LastError("WriteProcMem");
			}
		}
		else
		{
			LastError("!VirtualProtectEx");
		}

		if (!ReadProcessMemory(GetCurrentProcess(), _fdat, (LPVOID)_buff, 30, &_bRead))
		{
			LastError("ReadProcMem");
		}

		SetWindowTextA((HWND)GetDlgItem(hwndMain, ID_RICHEDIT), _buff);

		UnmapViewOfFile(_fdat);

		CloseHandle(hMapping);
		CloseHandle(hFile);
	}	

	return;
}