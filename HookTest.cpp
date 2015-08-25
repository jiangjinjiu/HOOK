// HookTest.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

BOOL CALLBACK DialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK JournalRecordProc(int code,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK JournalPlaybackProc(int code,WPARAM wParam,LPARAM lParam);
void Clear();

HINSTANCE g_hInst;
HHOOK g_Record;
HHOOK g_Play;

HWND g_hWnd;

int g_nCount = 0;
int g_nCount1 = 0;
int g_nCount2 = 0;

struct EVENTMSG_NODE
{
	EVENTMSG msg;
	EVENTMSG_NODE *pNext;
};

EVENTMSG_NODE *g_pHead = NULL;
EVENTMSG_NODE *g_pEnd = NULL;

EVENTMSG_NODE *g_pLast;
EVENTMSG_NODE *g_pNext;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.

	g_hInst = hInstance;
	HWND hWnd = CreateDialog(hInstance,MAKEINTRESOURCE(IDD_HOOK),NULL,DialogProc);
	ShowWindow(hWnd,SW_SHOW);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

BOOL CALLBACK DialogProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				g_hWnd = hWnd;
				EnableWindow(GetDlgItem(hWnd, IDC_STOP),FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_PLAY),FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_RECORD),TRUE);
				EnableWindow(GetDlgItem(hWnd, IDC_SAVE),FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_LOAD),TRUE);
			}
			break;
		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
		case WM_COMMAND:
			{
				switch(LOWORD(wParam))
				{
					case IDC_RECORD:
						{	
							g_Record = SetWindowsHookEx(WH_JOURNALRECORD, JournalRecordProc, g_hInst, 0);
							EnableWindow(GetDlgItem(hWnd, IDC_STOP),TRUE);
							EnableWindow(GetDlgItem(hWnd, IDC_PLAY),FALSE);
							EnableWindow(GetDlgItem(hWnd, IDC_LOAD),TRUE);
						}
						break;
					case IDC_STOP:
						{
							
							EnableWindow(GetDlgItem(hWnd, IDC_RECORD),FALSE);
							EnableWindow(GetDlgItem(hWnd, IDC_PLAY),TRUE);
							EnableWindow(GetDlgItem(hWnd, IDC_STOP),FALSE);
							EnableWindow(GetDlgItem(hWnd, IDC_SAVE),TRUE);
							EnableWindow(GetDlgItem(hWnd, IDC_LOAD),FALSE);

							UnhookWindowsHookEx(g_Record);	
						}
						break;
					case IDC_PLAY:
						{
							g_pLast = g_pHead;
							g_pNext = g_pHead;
							g_Play = SetWindowsHookEx(WH_JOURNALPLAYBACK, JournalPlaybackProc, g_hInst, 0);
							EnableWindow(GetDlgItem(hWnd, IDC_RECORD),FALSE);
							EnableWindow(GetDlgItem(hWnd, IDC_STOP),FALSE);
							EnableWindow(GetDlgItem(hWnd, IDC_SAVE),FALSE);
							EnableWindow(GetDlgItem(hWnd, IDC_LOAD),TRUE);
							EnableWindow(GetDlgItem(hWnd, IDC_PLAY),TRUE);
						}
					break;
					case IDC_SAVE:
						{
							if(NULL != g_pHead)
							{
								struct t
								{
									char si[100];
									int n;
								};

								EVENTMSG_NODE *pTemp = g_pHead;
								ofstream fsave("save.txt",ios::out);
								while (NULL != pTemp)
								{
									//fsave.write((char *)(pTemp),sizeof(EVENTMSG_NODE));
									fsave/*<<pTemp->msg.hwnd*/<<'\t'<<pTemp->msg.message<<'\t'<<pTemp->msg.paramH<<'\t'<<pTemp->msg.paramL<<'\t'<<pTemp->msg.time<<endl;
									pTemp = pTemp->pNext;
									g_nCount2++;
								}	
								fsave.close();
							}	
						}
						break;
					case IDC_LOAD:
						{
							if (NULL == g_pHead)
							{
								ifstream fload("save.txt", ios::in);
								if(!fload)
								{
									MessageBox(hWnd,"ÎÄ¼þÎª¿Õ£¡", "´íÎó", MB_OK);
								}
							
								g_pHead = new EVENTMSG_NODE;
								g_pHead->msg.hwnd = 0;
								//fload.read((char *)g_pHead->msg.hwnd, sizeof(HWND));
								fload/*>>g_pHead->msg.hwnd*/>>g_pHead->msg.message>>g_pHead->msg.paramH>>g_pHead->msg.paramL>>g_pHead->msg.time;
								//fload.read((char *)g_pHead, sizeof(EVENTMSG_NODE));
								EVENTMSG_NODE *pTemp = g_pHead;
							
								//while (NULL != pTemp->)
								//{
								//}

								EVENTMSG_NODE *pNew = NULL;
								//	for(int i = 0; i<g_nCount; i++)
								while(!fload.eof())
									{
										pNew = new EVENTMSG_NODE;
										pNew->pNext = NULL;
										pNew->msg.hwnd = 0;
										//fload.read((char *)g_pHead->msg.hwnd, sizeof(HWND));
										fload/*>>pNew->msg.hwnd*/>>pNew->msg.message>>pNew->msg.paramH>>pNew->msg.paramL>>pNew->msg.time;
										//fload.read((char *)pNew, sizeof(EVENTMSG_NODE));
										pTemp->pNext = pNew;
										pTemp = pNew;
										g_pEnd = pNew;
										g_nCount1++;
									}
									g_pEnd->pNext = NULL;
									fload.close();
								
							}
							EnableWindow(GetDlgItem(hWnd, IDC_PLAY),TRUE);
							EnableWindow(GetDlgItem(hWnd, IDC_RECORD),FALSE);
						}
						break;
					default:
						break;
				}
			}
			break;
		default:
			return FALSE;
	}
	return TRUE;
}


LRESULT CALLBACK JournalRecordProc(int code,WPARAM wParam,LPARAM lParam)
{
	if (code < 0)
	{
		CallNextHookEx(g_Record, code, wParam, lParam);
	}

	if (HC_ACTION == code)
	{
		EVENTMSG_NODE *pNew = new EVENTMSG_NODE;
		pNew->pNext = NULL;
		pNew->msg.hwnd = ((LPEVENTMSG)(lParam))->hwnd;
		pNew->msg.message = ((LPEVENTMSG)(lParam))->message;
		pNew->msg.paramH = ((LPEVENTMSG)(lParam))->paramH;
		pNew->msg.paramL = ((LPEVENTMSG)(lParam))->paramL;
		pNew->msg.time = ((LPEVENTMSG)(lParam))->time;

		if (NULL == g_pHead)
		{
			g_pHead = pNew;
		}
		else
		{
			g_pEnd->pNext = pNew;
		}

		g_nCount++;
		g_pEnd = pNew;

	}

	return CallNextHookEx(g_Record, code, wParam, lParam);
}



LRESULT CALLBACK JournalPlaybackProc(int code,WPARAM wParam,LPARAM lParam)
{
	DWORD dwTime = 0;
	
	switch (code)
	{
		case HC_GETNEXT:
			{
				LPEVENTMSG pEvent = (LPEVENTMSG)lParam;
				pEvent->hwnd = g_pLast->msg.hwnd;
				pEvent->message = g_pLast->msg.message;
				pEvent->paramH = g_pLast->msg.paramH;
				pEvent->paramL = g_pLast->msg.paramL;
				pEvent->time = g_pLast->msg.time;

				dwTime = g_pNext->msg.time - g_pLast->msg.time;

				if (dwTime < 0)
				{
					dwTime = 1;
				}
				
				if (g_pLast != g_pNext)
				{
					g_pLast = g_pNext;
				}

				return dwTime;
			}
		case HC_SKIP:
			{
				if (NULL == g_pNext->pNext)
				{

					EnableWindow(GetDlgItem(g_hWnd, IDC_RECORD),TRUE);
					EnableWindow(GetDlgItem(g_hWnd, IDC_PLAY),TRUE);
					EnableWindow(GetDlgItem(g_hWnd, IDC_STOP),FALSE);

					Clear();

					return UnhookWindowsHookEx(g_Play);
				}

				g_pNext = g_pNext->pNext;
				return 0;
			}
		default:
			break;
	}
	return CallNextHookEx(g_Play, code, wParam, lParam);;
}

void Clear()
{
	if (NULL != g_pHead)
	{
		EVENTMSG_NODE *pTemp = g_pHead;
		while (NULL != pTemp)
		{
			g_pHead = pTemp->pNext;
			delete pTemp;
			pTemp = g_pHead;
		}
	}
}