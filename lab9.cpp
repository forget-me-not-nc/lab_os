#define ID_RICHEDIT		9000
#define ID_START		9001

#include <Windows.h>
#include <richedit.h>
#include <string>
#include <winsock.h>
//------------------------
HINSTANCE _hInstance;
RECT _Rect = {};
HWND hwndMain;
//------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void LastError(const char* phase);
//------------------------
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
	_hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hWnd, GWLP_HINSTANCE));
	hwndMain = hWnd;

	switch (message)
	{
		case WM_CREATE:
		{
			LoadLibrary(L"riched20.dll");

			GetClientRect(hwndMain, &_Rect);

			HWND hRichEdit = CreateWindowEx(NULL, RICHEDIT_CLASS, NULL,
				WS_VISIBLE | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_NOHIDESEL | ES_AUTOVSCROLL | ES_MULTILINE | ES_SAVESEL | ES_SUNKEN,
				_Rect.right - 560, _Rect.top + 150, 250, 120, hWnd, (HMENU)ID_RICHEDIT, _hInstance, NULL);

			HWND hButton1 = CreateWindow(L"BUTTON", L"Start", WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
				_Rect.right - 500, _Rect.top + 70, 120, 40, hWnd, (HMENU)ID_START, _hInstance, NULL);

			
			break;
		}

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case ID_START:
				{
					WSADATA wsaData;

					int res = WSAStartup(MAKEWORD(2, 2), &wsaData);

					if (res != 0)
					{
						LastError("SOCKET_ERROR1");

						break;
					}

					SOCKET mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

					if (mySocket == INVALID_SOCKET)
					{
						LastError("INVALID_SOCKET1");
						WSACleanup();

						break;
					}
				
					sockaddr_in addr;
					addr.sin_family = AF_INET;
					addr.sin_addr.s_addr = inet_addr("192.168.0.109");
					addr.sin_port = htons(25565);

					res = bind(mySocket, (SOCKADDR*)&addr, (int)sizeof(addr));

					if (res == SOCKET_ERROR)
					{
						LastError("SOCKET_ERROR2");
						closesocket(mySocket);
						WSACleanup();

						break;
					}

					res = listen(mySocket, SOMAXCONN);

					if (res == SOCKET_ERROR)
					{
						LastError("SOCKET_ERROR3");
						closesocket(mySocket);
						WSACleanup();

						break;
					}

					SOCKET clientSocket = accept(mySocket, NULL, NULL);

					if (clientSocket == INVALID_SOCKET)
					{
						LastError("INVALID_SOCKET2");
						closesocket(mySocket);
						WSACleanup();

						break;
					}

					closesocket(mySocket);

					int iResult;

					char buffer[512];
					char resend[] = "I recieved your message..";

					memset(&buffer, 0, 512);

					do
					{
						iResult = recv(clientSocket, buffer, 512, NULL);
						if (iResult > 0)
						{
							MessageBoxA(hWnd, buffer, "MessageFromClient", MB_OK);

							send(clientSocket, resend, strlen(resend), NULL);

							memset(&buffer, 0, 512);
						}

					} while (iResult > 0);

					res = shutdown(clientSocket, 2);

					if (res == SOCKET_ERROR)
					{
						LastError("SOCKET_ERROR2");
						closesocket(mySocket);
						WSACleanup();

						break;
					}

					closesocket(clientSocket);
					WSACleanup();

					break;
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

		default: break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void LastError(const char* phase)
{
	std::string temp = std::to_string(WSAGetLastError());

	MessageBoxA(hwndMain, phase, temp.c_str(), MB_OK);
}