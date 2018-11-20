#include <windows.h>
#include <io.h>
#include "resource.h"

#define TRAY_NOTIFY (WM_APP + 100)
#define EXENAME "MemStatService.exe"
#define SRVNAME "MemStatService"
#define DISPNAME "Memory Status Log Service"
/*
#define EXENAME "ServiceExample.exe"
#define SRVNAME "ServiceExample"
#define DISPNAME "ServiceExample Log Service"
*/
#define SERVICE_CONTROL_NEWFILE 128

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

void Install();
void UnInstall();
void MemStart();
void MemControl(DWORD dwControl);
void QueryService();

HINSTANCE g_hInst;
HWND hWndMain;
//HWND hWnd;
HWND hDlgMain, hStatic;
LPCTSTR lpszClass=TEXT("TrayPopup");
SC_HANDLE hScm, hSrv;
SERVICE_STATUS ss;

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance
	  ,LPSTR lpszCmdParam,int nCmdShow)
{
	
	MSG Message;
	WNDCLASS WndClass;
	g_hInst=hInstance;

	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=WndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=NULL;
	WndClass.style=CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	HWND hWnd=CreateWindow(lpszClass,lpszClass,WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
		NULL,(HMENU)NULL,hInstance,NULL);
	//ShowWindow(hWnd,nCmdShow);

	while (GetMessage(&Message,NULL,0,0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	NOTIFYICONDATA nid;
	HMENU hMenu, hPopupMenu;
	POINT pt;
	TCHAR *Mes="Ʈ���� �������� ���� ���콺�� Ŭ���ϸ� �˾� �޴��� �����ݴϴ�";

	switch (iMessage) {
	case WM_CREATE:
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hWnd;
		hDlgMain = hWnd;
		nid.uID = 0;
		nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
		nid.uCallbackMessage = TRAY_NOTIFY;
		nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON1));
		lstrcpy(nid.szTip, "ProZone Agent");
		Shell_NotifyIcon(NIM_ADD, &nid);

		//SetWindowPos(hDlg, HWND_TOP, 100, 100, 0, 0, SWP_NOSIZE);
		//hDlgMain = hDlg;
		//hStatic = GetDlgItem(hDlg, IDC_STATIC1);

		// SCM�� ���� ������ ���� ���´�.
		/*
		hScm = OpenSCManager(NULL, NULL, GENERIC_READ);
		if (hScm == NULL) {
			MessageBox(hWnd, "SCM�� �� �� �����ϴ�", "�˸�", MB_OK);
			EndDialog(hWnd, 0);
		}
		
		// ���񽺰� ��ġ�Ǿ� ���� ������ ������ �� ����.
		hSrv = OpenService(hScm, SRVNAME, SERVICE_ALL_ACCESS);
		if (hSrv == NULL) {
			MessageBox(hWnd, "MemStatService ���񽺰� ��ġ�Ǿ� ���� �ʽ��ϴ�", "�˸�", MB_OK);
			EndDialog(hWnd, 0);
		}
		else {
			CloseServiceHandle(hSrv);
		}
		*/
		QueryService();
		
		return 0;
	case TRAY_NOTIFY:
		switch (lParam) {
		case WM_LBUTTONDOWN:
			break;
		case WM_RBUTTONDOWN:
			hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_MENU1));
			hPopupMenu = GetSubMenu(hMenu, 0);
			GetCursorPos(&pt);
			SetForegroundWindow(hWnd);
			TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON |
				TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
			SetForegroundWindow(hWnd);
			DestroyMenu(hPopupMenu);
			DestroyMenu(hMenu);
			break;
		}
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDM_ABOUT:
			MessageBox(hWnd,"Tmax ProZone Agent",
				"Agent Information",MB_OK);
			break;
		case IDM_INSTALL:
			Install();
			break;
		case IDM_UNINSTALL:
			UnInstall();
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDC_START:
			MemStart();
			break;
		case IDC_STOP:
			MemControl(SERVICE_CONTROL_STOP);
			break;
		case IDC_PAUSE:
			MemControl(SERVICE_CONTROL_PAUSE);
			break;
		case IDC_CONTINUE:
			MemControl(SERVICE_CONTROL_CONTINUE);
			break;
		case IDC_NEWFILE:
			MemControl(SERVICE_CONTROL_NEWFILE);
			break;
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlgMain, 0);
			break;
		}
		return 0;
	case WM_PAINT:
		hdc=BeginPaint(hWnd, &ps);
		TextOut(hdc,50,50,Mes,lstrlen(Mes));
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hWnd;
		nid.uID = 0;
		Shell_NotifyIcon(NIM_DELETE, &nid);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}


// ���񽺸� ��ġ�Ѵ�.
void Install()
{
	SC_HANDLE hScm, hSrv;
	TCHAR SrvPath[MAX_PATH];
	SERVICE_DESCRIPTION lpDes;
	TCHAR Desc[1024];

	// SCM�� ����
	hScm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (hScm == NULL) {
		MessageBox(hDlgMain, "SCM�� �� �� �����ϴ�.", "�˸�", MB_OK);
		return;
	}

	// ����� ���� ������ �ִ��� ������ ���� ��θ� ���Ѵ�.
	GetCurrentDirectory(MAX_PATH, SrvPath);
	lstrcat(SrvPath, "\\");
	lstrcat(SrvPath, EXENAME);
	if (_access(SrvPath, 0) != 0) {
		CloseServiceHandle(hScm);
		MessageBox(hDlgMain, "���� ���丮�� ���� ������ �����ϴ�.", "�˸�", MB_OK);
		return;
	}

	// ���񽺸� ����Ѵ�.
	hSrv = CreateService(hScm, SRVNAME, DISPNAME, SERVICE_PAUSE_CONTINUE |
		SERVICE_CHANGE_CONFIG, SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START,
		SERVICE_ERROR_IGNORE, SrvPath, NULL, NULL, NULL, NULL, NULL);
	if (hSrv == NULL) {
		MessageBox(hDlgMain, "��ġ���� ���߽��ϴ�.", "�˸�", MB_OK);
	}
	else {
		// ������ ����Ѵ�.
		//GetDlgItemText(hDlgMain, IDC_DESC, Desc, 1024);
		//lpDes.lpDescription = Desc;
		//ChangeServiceConfig2(hSrv, SERVICE_CONFIG_DESCRIPTION, &lpDes);

		MessageBox(hDlgMain, "��ġ�߽��ϴ�.", "�˸�", MB_OK);
		SetWindowText(hStatic, "���� ����:��ġ�Ǿ� �ֽ��ϴ�");
		CloseServiceHandle(hSrv);
	}

	CloseServiceHandle(hScm);

}

// ���񽺸� �����Ѵ�.
void UnInstall()
{
	SC_HANDLE hScm, hSrv;
	SERVICE_STATUS ss;

	// SCM�� ����
	hScm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (hScm == NULL) {
		MessageBox(hDlgMain, "SCM�� �� �� �����ϴ�.", "�˸�", MB_OK);
		return;
	}

	// ������ �ڵ��� ���Ѵ�.
	hSrv = OpenService(hScm, SRVNAME, SERVICE_ALL_ACCESS);
	if (hSrv == NULL) {
		CloseServiceHandle(hScm);
		MessageBox(hDlgMain, "���񽺰� ��ġ�Ǿ� ���� �ʽ��ϴ�.", "�˸�", MB_OK);
		return;
	}

	// �������̸� ������Ų��.
	QueryServiceStatus(hSrv, &ss);
	if (ss.dwCurrentState != SERVICE_STOPPED) {
		ControlService(hSrv, SERVICE_CONTROL_STOP, &ss);
		Sleep(2000);
	}

	// ���� ����
	if (DeleteService(hSrv)) {
		MessageBox(hDlgMain, "���񽺸� �����߽��ϴ�.", "�˸�", MB_OK);
		SetWindowText(hStatic, "���� ����:��ġ���� �ʾҽ��ϴ�");
	}
	else {
		MessageBox(hDlgMain, "���񽺸� �������� ���߽��ϴ�.", "�˸�", MB_OK);
	}
	CloseServiceHandle(hSrv);
	CloseServiceHandle(hScm);
}

// ���� ���� ���¿� ���� ��ư�� ���¸� �����Ų��.
void QueryService()
{
	SC_HANDLE hScm, hSrv;
	SERVICE_STATUS ss;
	hScm = OpenSCManager(NULL, NULL, GENERIC_READ);
	hSrv = OpenService(hScm, SRVNAME, SERVICE_INTERROGATE);

	do {
		ControlService(hSrv, SERVICE_CONTROL_INTERROGATE, &ss);
	} while ((ss.dwCurrentState != SERVICE_STOPPED) &&
		(ss.dwCurrentState != SERVICE_RUNNING) &&
		(ss.dwCurrentState != SERVICE_PAUSED));
	//hDlgMain = hWnd;
	EnableWindow(GetDlgItem(hDlgMain, IDC_START), FALSE);
	EnableWindow(GetDlgItem(hDlgMain, IDC_STOP), FALSE);
	EnableWindow(GetDlgItem(hDlgMain, IDC_PAUSE), FALSE);
	EnableWindow(GetDlgItem(hDlgMain, IDC_CONTINUE), FALSE);
	EnableWindow(GetDlgItem(hDlgMain, IDC_NEWFILE), FALSE);
	switch (ss.dwCurrentState) {
	case SERVICE_STOPPED:
		EnableWindow(GetDlgItem(hDlgMain, IDC_START), TRUE);
		SetWindowText(hStatic, "���� ����:����");
		break;
	case SERVICE_RUNNING:
		EnableWindow(GetDlgItem(hDlgMain, IDC_STOP), TRUE);
		EnableWindow(GetDlgItem(hDlgMain, IDC_PAUSE), TRUE);
		EnableWindow(GetDlgItem(hDlgMain, IDC_NEWFILE), TRUE);
		SetWindowText(hStatic, "���� ����:������");
		break;
	case SERVICE_PAUSED:
		EnableWindow(GetDlgItem(hDlgMain, IDC_STOP), TRUE);
		EnableWindow(GetDlgItem(hDlgMain, IDC_CONTINUE), TRUE);
		SetWindowText(hStatic, "���� ����:�Ͻ�����");
		break;
	}

	CloseServiceHandle(hSrv);
	CloseServiceHandle(hScm);
}

// ���񽺸� ���۽�Ų��.
void MemStart()
{
	SC_HANDLE hScm, hSrv;
	SERVICE_STATUS ss;
	hScm = OpenSCManager(NULL, NULL, GENERIC_READ);
	hSrv = OpenService(hScm, SRVNAME, SERVICE_START | SERVICE_QUERY_STATUS);

	// ���񽺸� ���۽�Ű�� ������ ������ ������ ����Ѵ�.
	SetCursor(LoadCursor(NULL, IDC_WAIT));
	if (StartService(hSrv, 0, NULL) == TRUE) {
		QueryServiceStatus(hSrv, &ss);
		while (ss.dwCurrentState != SERVICE_RUNNING) {
			Sleep(ss.dwWaitHint);
			QueryServiceStatus(hSrv, &ss);
		}
	}
	SetCursor(LoadCursor(NULL, IDC_ARROW));

	CloseServiceHandle(hSrv);
	CloseServiceHandle(hScm);
	QueryService();
}

// ���񽺿� ���� �ڵ带 ������.
void MemControl(DWORD dwControl)
{
	SC_HANDLE hScm, hSrv;
	SERVICE_STATUS ss;
	hScm = OpenSCManager(NULL, NULL, GENERIC_READ);
	hSrv = OpenService(hScm, SRVNAME, GENERIC_EXECUTE);

	ControlService(hSrv, dwControl, &ss);

	CloseServiceHandle(hSrv);
	CloseServiceHandle(hScm);
	QueryService();
}