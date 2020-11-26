#pragma warning(disable : 4996)


#define ID_RICHEDIT		9000
#define ID_CREATEBUTTON	9001
#define ID_DELETEUSER	9002
#define ID_GETSID		9003
#define ID_GETTOKEN		9004
#define ID_PRIVILEGE	9005
#define ID_ADDRIGHTS	9006

#include <Windows.h>
#include <richedit.h>
#include <string>
#include <lmaccess.h>
#include <lm.h>
#include <cstdlib>
#include "atlbase.h"
#include "atlstr.h"
#include <sddl.h>
#include "comutil.h"
#include <ntsecapi.h>
//------------------------
TCHAR Notepad[] = L"notepad ReadMe.txt";
HINSTANCE hInstance;
RECT _Rect = {};
HWND hwndMain;
wchar_t* wName;
HANDLE _hToken;
PWSTR pName;
//------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void LastError(const char* phase);
void LsaLastError(const char* phase, NTSTATUS Status);
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
	hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hWnd, GWLP_HINSTANCE));
	hwndMain = hWnd;

	static STARTUPINFO				tin;
	static PROCESS_INFORMATION		pInfo;

	switch (message)
	{
		case WM_CREATE:
		{
			LoadLibrary(L"riched20.dll");

			GetClientRect(hwndMain, &_Rect);

			HWND hRichEdit = CreateWindowEx(NULL, RICHEDIT_CLASS, NULL,
				WS_VISIBLE | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_NOHIDESEL | ES_AUTOVSCROLL | ES_MULTILINE | ES_SAVESEL | ES_SUNKEN,
				_Rect.right - 560, _Rect.top + 150, 350, 220, hWnd, (HMENU)ID_RICHEDIT, hInstance, NULL);

			HWND hButton1 = CreateWindow(L"BUTTON", L"CreateUser", WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
				_Rect.left + 30, _Rect.top + 30, 100, 30, hWnd, (HMENU)ID_CREATEBUTTON, hInstance, NULL);

			HWND hButton2 = CreateWindow(L"BUTTON", L"DeleteUser", WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
				_Rect.left + 30, _Rect.top + 70, 100, 30, hWnd, (HMENU)ID_DELETEUSER, hInstance, NULL);
			
			HWND hButton3 = CreateWindow(L"BUTTON", L"GetSIDofCurrentUser", WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
				_Rect.left + 30, _Rect.top + 110, 150, 30, hWnd, (HMENU)ID_GETSID, hInstance, NULL);
			
			HWND hButton4 = CreateWindow(L"BUTTON", L"GetToken", WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
				_Rect.left + 30, _Rect.top + 150, 100, 30, hWnd, (HMENU)ID_GETTOKEN, hInstance, NULL);

			HWND hButton5 = CreateWindow(L"BUTTON", L"PrivilegeList", WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
				_Rect.left + 30, _Rect.top + 190, 100, 30, hWnd, (HMENU)ID_PRIVILEGE, hInstance, NULL);

			HWND hButton6 = CreateWindow(L"BUTTON", L"AddRights", WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
				_Rect.left + 30, _Rect.top + 230, 100, 30, hWnd, (HMENU)ID_ADDRIGHTS, hInstance, NULL);

			break;
		}

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case ID_CREATEBUTTON:
				{
					USER_INFO_1 userInfo1{};
					DWORD dwError = 0;
					NET_API_STATUS nStatus;

					wName = new wchar_t[GetWindowTextLength((HWND)GetDlgItem(hWnd, ID_RICHEDIT)) + 1];
					GetWindowText((HWND)GetDlgItem(hWnd, ID_RICHEDIT), wName, GetWindowTextLength((HWND)GetDlgItem(hWnd, ID_RICHEDIT)) + 1);
					userInfo1.usri1_name = wName;
					userInfo1.usri1_password = wName;
					userInfo1.usri1_priv = USER_PRIV_USER;
					userInfo1.usri1_home_dir = NULL;
					userInfo1.usri1_comment = NULL;
					userInfo1.usri1_flags = UF_SCRIPT;
					userInfo1.usri1_script_path = NULL;

					nStatus = NetUserAdd(NULL, 1, (LPBYTE)&userInfo1, &dwError);

					if (nStatus == NERR_Success)
					{
						LastError("UserCreated");
					}
					else
					{
						LastError("NetAccessAdd");
					}

					break;
				}

				case ID_DELETEUSER:
				{
					NET_API_STATUS nStatus;

					nStatus = NetUserDel(NULL, wName);

					if (nStatus == NERR_Success)
					{
						LastError("UserDeleted");
					}
					else
					{
						LastError("NetUserDel");
					}

					delete[] wName;

					break;
				}

				case ID_GETSID:
				{
					BYTE bSID[SECURITY_MAX_SID_SIZE];
					SID_NAME_USE sidNameUse;

					TCHAR username[UNLEN + 1];
					TCHAR domainName[UNLEN + 1];
					DWORD domainName_len = UNLEN + 1;
					DWORD username_len = UNLEN + 1;
					DWORD dwSidSize = SECURITY_MAX_SID_SIZE;
					GetUserName(username, &username_len);

					if (!LookupAccountName(NULL, username, &bSID, &dwSidSize, domainName, &domainName_len, &sidNameUse))
					{
						LastError("LookupAccountName");
					}

					LPTSTR szSID = NULL;

					if (!ConvertSidToStringSid(&bSID, &szSID))
					{
						LastError("ConvertSidToStringSid");
					}

					SetWindowText((HWND)GetDlgItem(hWnd, ID_RICHEDIT), szSID);

					break;
				}

				case ID_GETTOKEN:
				{
					HANDLE hToken;

					if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
					{
						LastError("OpenProcessToken");

						break;
					}

					if (!hToken)
					{
						LastError("GetCurrentProcessToken");
					
						break;
					}

					_hToken = hToken;

					PTOKEN_GROUPS tUser;
					DWORD returnedLength = 0;

					if (!GetTokenInformation(hToken, TokenGroups, NULL, returnedLength, &returnedLength))
					{
						DWORD lError = GetLastError();
						if(lError != ERROR_INSUFFICIENT_BUFFER) LastError("GetCurrentProcessToken 1");
					}

					tUser = (PTOKEN_GROUPS)GlobalAlloc(GPTR, returnedLength);

					if (!GetTokenInformation(hToken, TokenGroups, tUser, returnedLength, &returnedLength))
					{
						LastError("GetCurrentProcessToken 2");
						CloseHandle(hToken);

						break;
					}

					PSID pSid = NULL;
					DWORD dwSize;
					wchar_t lpName[256];
					wchar_t lpDomain[256];
					memset(&lpName, 0, 256);
					memset(&lpDomain, 0, 256);
					SID_NAME_USE SidType;
					SID_IDENTIFIER_AUTHORITY SIDAuth = SECURITY_NT_AUTHORITY;

					if (!AllocateAndInitializeSid(&SIDAuth, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0,&pSid))
					{
						LastError("AllocateAndInitializeSid");
						CloseHandle(hToken);
						if (tUser) GlobalFree(tUser);
						if (pSid) FreeSid(pSid);
						break;
					}

					for (int i = 0; i < tUser->GroupCount; i++)
					{
						dwSize = 256;
						if (EqualSid(pSid, tUser->Groups[i].Sid))
						{
							if (!LookupAccountSid(NULL, tUser->Groups[i].Sid, lpName, &dwSize, lpDomain, &dwSize, &SidType))
							{
								LastError("LookupAccountSid");
							}

							if (tUser->Groups[i].Attributes & SE_GROUP_ENABLED)
							{
								MessageBox(hWnd, L"The group SID is enabled.", L"", MB_OK);
							}
							else if (tUser->Groups[i].Attributes & SE_GROUP_USE_FOR_DENY_ONLY)
							{
								MessageBox(hWnd, L"The group SID is a deny-only SID.", L"", MB_OK);
							}
							else
							{
								MessageBox(hWnd, L"The group SID is not enabled.", L"", MB_OK);
							}
						}
					}

					std::wstring text = L"GroupsTokenInfo:\n";
					text += L"tUser->GroupCount: ";
					text += std::to_wstring(tUser->GroupCount);
					text += L";\ntUser->Groups->Attributes: ";
					text += std::to_wstring(tUser->Groups->Attributes);
					text += L"\nlpName:";
					if(lpName) text += lpName;
					text += L"\nlpDomain: ";
					if (lpDomain) text += lpDomain;

					SetWindowText((HWND)GetDlgItem(hWnd, ID_RICHEDIT), text.c_str());

					if(hToken) CloseHandle(hToken);
					if(tUser) GlobalFree(tUser);
					if(pSid) FreeSid(pSid);

					break;
				}

				case ID_PRIVILEGE: 
				{
					TCHAR cName[MAX_COMPUTERNAME_LENGTH + 1];
					DWORD cNameSize;

					GetComputerName(cName, &cNameSize);

					LSA_OBJECT_ATTRIBUTES ObjectAttributes;
					USHORT SystemNameLength;
					LSA_UNICODE_STRING lusSystemName;
					LSA_HANDLE lsahPolicyHandle;

					ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

					pName = cName;
					SystemNameLength = wcslen(cName);
					lusSystemName.Buffer = cName;
					lusSystemName.Length = SystemNameLength * sizeof(WCHAR);
					lusSystemName.MaximumLength = (SystemNameLength + 1) * sizeof(WCHAR);

					NTSTATUS Result = LsaOpenPolicy(&lusSystemName, &ObjectAttributes, POLICY_ALL_ACCESS, &lsahPolicyHandle);

					if (Result != 0x0)
					{
						LsaLastError("LsaOpenPolicy", Result);

						break;
					}
					
					BYTE bSID[SECURITY_MAX_SID_SIZE];
					SID_NAME_USE sidNameUse;

					TCHAR username[UNLEN + 1];
					TCHAR domainName[UNLEN + 1];
					DWORD domainName_len = UNLEN + 1;
					DWORD username_len = UNLEN + 1;
					DWORD dwSidSize = SECURITY_MAX_SID_SIZE;
					GetUserName(username, &username_len);

					if (!LookupAccountName(NULL, username, &bSID, &dwSidSize, domainName, &domainName_len, &sidNameUse))
					{
						LastError("LookupAccountName");

						break;
					}

					ULONG righstCount;
					PLSA_UNICODE_STRING pPrivs = new LSA_UNICODE_STRING;
					pPrivs->Buffer = cName;
					pPrivs->Length = wcslen(cName) * sizeof(WCHAR);
					pPrivs->MaximumLength = (wcslen(cName) + 1) * sizeof(WCHAR);

					LSA_ENUMERATION_INFORMATION* buf = new LSA_ENUMERATION_INFORMATION;
					buf->Sid = bSID;

					if (Result = LsaEnumerateAccountRights(lsahPolicyHandle, bSID, &pPrivs, &righstCount); Result != 0x0)
					{
						LsaLastError("LsaEnumerateAccountRights", Result);
					}

					DWORD dwSize = UNLEN + 1;
					wchar_t* displayName = new wchar_t[dwSize];
					DWORD languageId;

					if (!LookupPrivilegeDisplayName(cName, pPrivs->Buffer, displayName, &dwSize, &languageId))
					{
						LastError("LookupPrivilegeDisplayName");
					}

					INT originalLenght = GetWindowTextLength((HWND)GetDlgItem(hWnd, ID_RICHEDIT));
					SendMessage((HWND)GetDlgItem(hWnd, ID_RICHEDIT), EM_SETSEL, (WPARAM)originalLenght, (LPARAM)originalLenght);
					SendMessage((HWND)GetDlgItem(hWnd, ID_RICHEDIT), EM_REPLACESEL, 0, (LPARAM)displayName);

					if (displayName) delete[] displayName;
					if (lsahPolicyHandle) LsaClose(lsahPolicyHandle);
					if (buf) delete buf;

					break;
				}

				case ID_ADDRIGHTS:
				{
					BYTE bSID[SECURITY_MAX_SID_SIZE];
					SID_NAME_USE sidNameUse;
					NTSTATUS ntsResult;

					TCHAR username[UNLEN + 1];
					TCHAR domainName[UNLEN + 1];
					DWORD domainName_len = UNLEN + 1;
					DWORD username_len = UNLEN + 1;
					DWORD dwSidSize = SECURITY_MAX_SID_SIZE;
					GetUserName(username, &username_len);

					if (!LookupAccountName(NULL, username, &bSID, &dwSidSize, domainName, &domainName_len, &sidNameUse))
					{
						LastError("LookupAccountName");
						
						break;
					}

					WCHAR lpBuffer[MAX_COMPUTERNAME_LENGTH + 1];
					DWORD dwBufferSize = MAX_COMPUTERNAME_LENGTH + 1;

					if (ntsResult = GetComputerName(lpBuffer, &dwBufferSize); ntsResult == 0)
					{
						LastError("GetComputerName");

						break;
					}

					LSA_OBJECT_ATTRIBUTES ObjectAttributes;
					WCHAR SystemName[MAX_COMPUTERNAME_LENGTH + 1];
					wcscpy(SystemName, lpBuffer);
					USHORT SystemNameLength;
					LSA_UNICODE_STRING lusSystemName;
					LSA_HANDLE lsahPolicyHandle;

					ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

					SystemNameLength = wcslen(SystemName);
					lusSystemName.Buffer = SystemName;
					lusSystemName.Length = SystemNameLength * sizeof(WCHAR);
					lusSystemName.MaximumLength = (SystemNameLength + 1) * sizeof(WCHAR);

					ntsResult = LsaOpenPolicy(&lusSystemName,&ObjectAttributes, POLICY_ALL_ACCESS, &lsahPolicyHandle);
					
					if (ntsResult != 0x0)
					{
						LsaLastError("LsaOpenPolicy", ntsResult);

						break;
					}

					PLSA_UNICODE_STRING pLsaString = new LSA_UNICODE_STRING;
					DWORD dwLen = wcslen(L"SeMachineAccountPrivilege");
					pLsaString->Buffer = (WCHAR*)L"SeMachineAccountPrivilege";
					pLsaString->Length = (USHORT)dwLen * sizeof(WCHAR);
					pLsaString->MaximumLength = (USHORT)(dwLen + 1) * sizeof(WCHAR);

					if (ntsResult = LsaAddAccountRights(lsahPolicyHandle, bSID, pLsaString, 1); ntsResult != 0x0)
					{
						LsaLastError("LsaAddAccountRights", ntsResult);

						LsaClose(lsahPolicyHandle);

						break;
					}

					LSA_ENUMERATION_INFORMATION* buf = new LSA_ENUMERATION_INFORMATION;
					buf->Sid = bSID;
					DWORD righstCount = 0;

					if (ntsResult = LsaEnumerateAccountRights(lsahPolicyHandle, bSID, &pLsaString, &righstCount); ntsResult != 0x0)
					{
						LsaLastError("LsaEnumerateAccountRights", ntsResult);
					}

					DWORD dwSize = UNLEN + 1;
					wchar_t* displayName = new wchar_t[dwSize];
					DWORD languageId;

					if (!LookupPrivilegeDisplayName(SystemName, pLsaString->Buffer, displayName, &dwSize, &languageId))
					{
						LastError("LookupPrivilegeDisplayName");
					}

					INT originalLenght = GetWindowTextLength((HWND)GetDlgItem(hWnd, ID_RICHEDIT));
					SendMessage((HWND)GetDlgItem(hWnd, ID_RICHEDIT), EM_SETSEL, (WPARAM)originalLenght, (LPARAM)originalLenght);
					SendMessage((HWND)GetDlgItem(hWnd, ID_RICHEDIT), EM_REPLACESEL, 0, (LPARAM)displayName);

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
	std::string temp = std::to_string(GetLastError());

	MessageBoxA(hwndMain, phase, temp.c_str(), MB_OK);
}

void LsaLastError(const char* phase, NTSTATUS Status)
{
	std::string temp = std::to_string(LsaNtStatusToWinError(Status));

	MessageBoxA(hwndMain, phase, temp.c_str(), MB_OK);
}
