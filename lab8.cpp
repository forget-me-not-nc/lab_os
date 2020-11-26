#pragma warning(disable : 4996)

#define ID_TREEVIEW		9001
#define ID_CREATEFILE		9002
#define ID_CREATEDIR		9003
#define ID_DELETEFILE		9004
#define ID_DELETEDIR		9005
#define ID_READFILE		9006
#define ID_RICHEDIT		9007
#define ID_FILEPOINTERCHECKBOX	9008
#define ID_EDIT1	9009
#define ID_WRITEFILE	9010
#define ID_EDIT2	9011
#define ID_FILEATTR	9012
#define ID_DISKINFO	9013
#define ID_DIROUT	9014

#include <windows.h>
#include <string>
#include <sstream>
#include <atlstr.h>
#include <io.h>
#include <richedit.h>
//-----------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//-----------------------
HINSTANCE _hInstance;
HWND _hMain;
//-----------------------
wchar_t* mainDir;
wchar_t* getTxt(int WINDOW);
//-----------------------
VOID* _start_address;
VOID _catch_exept(LPVOID _baseAddress);
INT _exept_filter(DWORD _exeptCode);
//-----------------------
VOID LastError(const char* phase);
//-----------------------
int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	MSG			msg{};
	HWND		hwnd{};
	WNDCLASSEX	wc{ sizeof(WNDCLASSEX) };

	mainDir = new wchar_t[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, mainDir);
	std::wstring temp = mainDir;
	temp += L"\\";
	mainDir = _wcsdup(temp.c_str());

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

	if (hwnd = CreateWindowEx(NULL, wc.lpszClassName, L"Window", WS_OVERLAPPEDWINDOW, 350, 100, 800, 700, nullptr, nullptr, wc.hInstance, nullptr); hwnd == INVALID_HANDLE_VALUE)
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
	_hMain = hWnd;

	switch (message)
	{
		case WM_CREATE:
		{
			HWND hButton1 = CreateWindow(L"BUTTON", L"CreateDir", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 40, 100, 100, 30, hWnd,
				(HMENU)ID_CREATEDIR, _hInstance, NULL);

			HWND hButton2 = CreateWindow(L"BUTTON", L"CreateFile", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 40, 140, 100, 30, hWnd,
				(HMENU)ID_CREATEFILE, _hInstance, NULL);

			HWND hButton3 = CreateWindow(L"BUTTON", L"ReadFile", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 40, 190, 100, 30, hWnd,
				(HMENU)ID_READFILE, _hInstance, NULL);

			HWND hButton4 = CreateWindow(L"BUTTON", L"DeleteFile", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 40, 230, 100, 30, hWnd,
				(HMENU)ID_DELETEFILE, _hInstance, NULL);

			HWND hButton5 = CreateWindow(L"BUTTON", L"DeleteDir", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 40, 270, 100, 30, hWnd,
				(HMENU)ID_DELETEDIR, _hInstance, NULL);

			HWND hCheckbox1 = CreateWindow(L"BUTTON", L"SetFilePointer", BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE, 150, 195, 120, 20, hWnd,
				(HMENU)ID_FILEPOINTERCHECKBOX, _hInstance, NULL);

			HWND hEditForCheckBox1 = CreateWindow(L"EDIT", NULL, WS_CHILD | WS_VISIBLE, 285, 195, 30, 20, hWnd,
				(HMENU)ID_EDIT1, _hInstance, NULL);

			LoadLibrary(L"riched20.dll");

			HWND hRichEdit = CreateWindowEx(NULL, RICHEDIT_CLASS, NULL,
				WS_VISIBLE | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_NOHIDESEL | ES_AUTOVSCROLL | ES_MULTILINE | ES_SAVESEL | ES_SUNKEN,
				450, 400, 250, 230, hWnd, (HMENU)ID_RICHEDIT, _hInstance, NULL);

			HWND hEdit2 = CreateWindow(L"EDIT", NULL, WS_CHILD | WS_VISIBLE, 160, 310, 150, 20, hWnd,
				(HMENU)ID_EDIT2, _hInstance, NULL);

			HWND hButton6 = CreateWindow(L"BUTTON", L"WriteFile", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 40, 310, 100, 30, hWnd,
				(HMENU)ID_WRITEFILE, _hInstance, NULL);

			HWND hButton7 = CreateWindow(L"BUTTON", L"FileAttr", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 40, 350, 100, 30, hWnd,
				(HMENU)ID_FILEATTR, _hInstance, NULL);

			HWND hButton8 = CreateWindow(L"BUTTON", L"DiskInfo", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 40, 390, 100, 30, hWnd,
				(HMENU)ID_DISKINFO, _hInstance, NULL);

			HWND hButton9 = CreateWindow(L"BUTTON", L"OutDirFiles", BS_FLAT | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 40, 430, 100, 30, hWnd,
				(HMENU)ID_DIROUT, _hInstance, NULL);

			break;
		}

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case ID_CREATEDIR:
				{
					wchar_t* buffer;

					if (buffer = getTxt(ID_RICHEDIT); buffer == NULL) break;

					if (!CreateDirectory(buffer, NULL))
					{
						LastError("CreateDir");

						delete[] buffer;
						break;
					}

					delete[] buffer;

					break;
				}
				case ID_DIROUT:
				{
					WIN32_FIND_DATA findData;
					std::wstring dirFiles = mainDir;
					dirFiles += '*';

					HANDLE hFile = FindFirstFile(dirFiles.c_str(), &findData);

					if (hFile == INVALID_HANDLE_VALUE)
					{
						LastError("INVALID_HANDLE_VALUE");

						break;
					}

					dirFiles = mainDir;
					dirFiles += L":\n";

					while (FindNextFile(hFile, &findData))
					{
						dirFiles += findData.cFileName;
						dirFiles += '\n';
					}

					SetWindowText((HWND)GetDlgItem(hWnd, ID_RICHEDIT), dirFiles.c_str());

					FindClose(hFile);

					break;
				}
				case ID_CREATEFILE:
				{
					wchar_t* buffer;

					if (buffer = getTxt(ID_RICHEDIT); buffer == NULL) break;

					HANDLE hFile = CreateFile(buffer, GENERIC_WRITE, NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

					if (hFile == INVALID_HANDLE_VALUE)
					{
						LastError("CreateFile");

						delete[] buffer;
						break;
					}

					CloseHandle(hFile);

					break;
				}
				case ID_DISKINFO:
				{
					DWORD logicalDrives = GetLogicalDrives();
					std::wstring diskInfo = L"Logical Drives: ";

					if (!logicalDrives)
					{
						LastError("GetLogicalDrives");
					}
					else
					{
						for (int i = 0; i < 26; i++)
						{
							if (!(logicalDrives & (1 << i)))
							{
								diskInfo += ('A' + i);
								diskInfo += L":\\\n";
							}
						}
					}
				
					wchar_t nameBuffer[MAX_PATH];
					wchar_t SysNameBuffer[MAX_PATH];
					DWORD VSNumber;
					DWORD MCLength;
					DWORD FileSF;

					if (!GetVolumeInformation(L"C:\\", nameBuffer, sizeof(nameBuffer) / 2, &VSNumber, &MCLength, &FileSF,
						SysNameBuffer, sizeof(SysNameBuffer) / 2))
					{
						LastError("GetVolumeInformation");
					}
					else
					{
						diskInfo += L"\nDisk C:\n";
						diskInfo += L"nameBuffer: ";
						diskInfo += nameBuffer;
						diskInfo += L"\nSysNameBuffer: ";
						diskInfo += SysNameBuffer;
						diskInfo += L"\nVSNumber: ";
						diskInfo += std::to_wstring(VSNumber);
						diskInfo += L"\nMCLength: ";
						diskInfo += std::to_wstring(MCLength);
						diskInfo += L"\nFileSF: ";
						diskInfo += std::to_wstring(FileSF);
					}

					DWORD SectorsPerCluster;
					DWORD ByetesPerSecond;
					DWORD NumberOfFreeCluster;
					DWORD TotalNumberOfFreeCluster;

					if (!GetDiskFreeSpace(L"C:\\", &SectorsPerCluster, &ByetesPerSecond, &NumberOfFreeCluster, &TotalNumberOfFreeCluster))
					{
						LastError("GetDiskFreeSpace");
					}
					else
					{
						diskInfo += std::to_wstring(SectorsPerCluster);
						diskInfo += L"\nSectorsPerCluster: ";
						diskInfo += std::to_wstring(ByetesPerSecond);
						diskInfo += L"\nByetesPerSecond: ";
						diskInfo += std::to_wstring(NumberOfFreeCluster);
						diskInfo += L"\nNumberOfFreeCluster: ";
						diskInfo += std::to_wstring(TotalNumberOfFreeCluster);
						diskInfo += L"\nTotalNumberOfFreeCluster: ";
					}

					SetWindowText((HWND)GetDlgItem(hWnd, ID_RICHEDIT), diskInfo.c_str());

					break;
				}
				case ID_READFILE:
				{
					wchar_t* buffer;

					if (buffer = getTxt(ID_RICHEDIT); buffer == NULL) break;

					HANDLE hFile = CreateFile(buffer, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

					if (hFile == INVALID_HANDLE_VALUE)
					{
						LastError("CreateFile");

						delete[] buffer;
						break;
					}

					DWORD dwBytesRead = 0;
					DWORD size = GetFileSize(hFile, NULL);

					LRESULT result = SendMessage((HWND)GetDlgItem(hWnd, ID_FILEPOINTERCHECKBOX), BM_GETCHECK, 0, 0);

					if (result == BST_CHECKED)
					{
						wchar_t *dist = new wchar_t[4];
						GetWindowText((HWND)GetDlgItem(hWnd, ID_EDIT1), dist, 4);
						INT d = _wtoi(dist);
						delete[] dist;

						d *= 2;

						if (d >= size)
						{
							LastError("Invalid dist");

							break;
						}

						if (!SetFilePointer(hFile, d, NULL, NULL))
						{
							LastError("SetFilePointer");
						}

						size -= d;
					}

					wchar_t* content = new wchar_t[(size + 1)];

					if (!ReadFile(hFile, content, (size + 1), &dwBytesRead, NULL))
					{
						LastError("ReadFile");

						delete[] content;
						break;
					}

					content[dwBytesRead / 2] = '\0';

					SetWindowText((HWND)GetDlgItem(hWnd, ID_RICHEDIT), content);

					CloseHandle(hFile);

					delete[] content;

					break;
				}
				case ID_FILEATTR:
				{
					wchar_t* file = getTxt(ID_EDIT2);

					WIN32_FILE_ATTRIBUTE_DATA getFIL;

					if (!GetFileAttributesEx(file, GetFileExInfoStandard, &getFIL))
					{
						LastError("GetFileAttributesEx");

						delete[] file;
						break;
					}

					std::wstring attributes = L"File ";
					attributes += file;
					attributes += L": \ndwFileAttributes: ";
					attributes += std::to_wstring(getFIL.dwFileAttributes);
					attributes += L"\nnFileSizeHigh: ";
					attributes += std::to_wstring(getFIL.nFileSizeHigh);
					attributes += L"\nFileSizeLow: ";
					attributes += std::to_wstring(getFIL.nFileSizeLow);

					SetWindowText((HWND)GetDlgItem(hWnd, ID_RICHEDIT), attributes.c_str());

					break;
				}
				case ID_DELETEFILE:
				{
					wchar_t* buffer;

					if (buffer = getTxt(ID_RICHEDIT); buffer == NULL) break;

					if (!DeleteFile(buffer))
					{
						LastError("DeleteFile");
					}

					break;
				}
				case ID_WRITEFILE:
				{
					wchar_t* file = getTxt(ID_EDIT2);
					wchar_t* text = getTxt(ID_RICHEDIT);

					if (file == NULL || text == NULL)
					{
						break;
					}

					HANDLE hFile = CreateFile(file, GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

					if (hFile == INVALID_HANDLE_VALUE)
					{
						LastError("CreateFile");

						delete[] text;
						delete[] file;

						break;
					}

					DWORD dwBytesWritten = 0;

					if (!WriteFile(hFile, text, wcslen(text) * sizeof(wchar_t), &dwBytesWritten, NULL))
					{
						LastError("WriteFile");
					}

					CloseHandle(hFile);

					delete[] text;
					delete[] file;

					break;
				}
				case ID_DELETEDIR:
				{
					wchar_t* buffer;

					if (buffer = getTxt(ID_RICHEDIT); buffer == NULL) break;

					SetCurrentDirectory(mainDir);

					if (!RemoveDirectory(buffer))
					{
						LastError("Remove Dir");
					}

					delete[] buffer;

					break;
				}

				default: break;
			}

			break;
		}
		case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO min_max = (LPMINMAXINFO)lParam;
			min_max->ptMinTrackSize.x = 800;
			min_max->ptMinTrackSize.y = 700;

			min_max->ptMaxTrackSize.x = 800;
			min_max->ptMaxTrackSize.y = 700;

			break;
		}
		case WM_DESTROY:
		{
			DestroyWindow(hWnd);
			PostQuitMessage(EXIT_SUCCESS);

			break;
		}

		default: break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

VOID LastError(const char* phase)
{
	std::string temp = std::to_string(GetLastError());

	MessageBoxA(_hMain, phase, temp.c_str(), MB_OK);
}

wchar_t* getTxt(int WINDOW)
{
	INT txtLength = GetWindowTextLength((HWND)GetDlgItem(_hMain, WINDOW)) + 1;
	wchar_t* buffer = new wchar_t[txtLength];
	if (!GetWindowText((HWND)GetDlgItem(_hMain, WINDOW), buffer, txtLength))
	{
		LastError("GetWindowText");
		delete[] buffer;
		
		return NULL;
	}

	return buffer;
}
