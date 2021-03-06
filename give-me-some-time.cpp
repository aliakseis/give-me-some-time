#include "stdafx.h"
#include "give-me-some-time.h"

namespace {

enum { MAX_LOADSTRING = 100 };
enum { WM_USER_SHELLICON = WM_USER + 1 };
enum { IDEVT_TIMER = 1234 };

// Global Variables:
HINSTANCE hInst;	// current instance
NOTIFYICONDATA nidApp;
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
BOOL bDisable = FALSE;							// keep application state


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM /*lParam*/)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_USER_SHELLICON:
        // systray msg callback 
        if (LOWORD(lParam) == WM_RBUTTONDOWN)
        {
            POINT clickPoint;
            UINT uFlag = MF_BYPOSITION | MF_STRING;
            GetCursorPos(&clickPoint);
            HMENU hPopMenu = CreatePopupMenu();
            InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_ABOUT, _T("About"));
            if (bDisable)
            {
                uFlag |= MF_GRAYED;
            }
            InsertMenu(hPopMenu, 0xFFFFFFFF, MF_SEPARATOR, IDM_SEP, _T("SEP"));
            if (bDisable)
            {
                InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_ENABLE, _T("Enable"));
            }
            else
            {
                InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_DISABLE, _T("Disable"));
            }
            InsertMenu(hPopMenu, 0xFFFFFFFF, MF_SEPARATOR, IDM_SEP, _T("SEP"));
            InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_EXIT, _T("Exit"));

            SetForegroundWindow(hWnd);
            TrackPopupMenu(hPopMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, clickPoint.x, clickPoint.y, 0, hWnd, NULL);
            return TRUE;
        }
        break;
    case WM_COMMAND:
        // Parse the menu selections:
        switch (LOWORD(wParam))
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_DISABLE:
            KillTimer(hWnd, IDEVT_TIMER);
            bDisable = TRUE;
            break;
        case IDM_ENABLE:
            SetTimer(hWnd, IDEVT_TIMER, 20000, nullptr);
            bDisable = FALSE;
            break;
        case IDM_EXIT:
            Shell_NotifyIcon(NIM_DELETE, &nidApp);
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_TIMER:
        if (wParam == IDEVT_TIMER)
        {
            ::SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
        }
        else
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
        /*
        case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code here...
        EndPaint(hWnd, &ps);
        break;*/
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_QUERYENDSESSION:
        return TRUE; // allow automatic close
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GIVEMESOMETIME));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_GIVEMESOMETIME);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
BOOL InitInstance(HINSTANCE hInstance, int /*nCmdShow*/)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    HICON hMainIcon = LoadIcon(hInstance, (LPCTSTR)MAKEINTRESOURCE(IDI_GIVEMESOMETIME));

    nidApp.cbSize = sizeof(NOTIFYICONDATA); // sizeof the struct in bytes 
    nidApp.hWnd = (HWND)hWnd;              //handle of the window which will process this app. messages 
    nidApp.uID = IDI_GIVEMESOMETIME;           //ID of the icon that willl appear in the system tray 
    nidApp.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; //ORing of all the flags 
    nidApp.hIcon = hMainIcon; // handle of the Icon to be displayed, obtained from LoadIcon 
    nidApp.uCallbackMessage = WM_USER_SHELLICON;
    LoadString(hInstance, IDS_APPTOOLTIP, nidApp.szTip, MAX_LOADSTRING);
    Shell_NotifyIcon(NIM_ADD, &nidApp);

    SetTimer(hWnd, IDEVT_TIMER, 20000, nullptr);

    return TRUE;
}

HRESULT RegisterApplicationRestartStub(PCWSTR pwzCommandline, DWORD dwFlags)
{
    typedef HRESULT(_stdcall *RegisterApplicationRestartT)(PCWSTR pwzCommandline, DWORD dwFlags);

    HMODULE hModule = ::LoadLibrary(_T("kernel32.dll"));
    if (hModule == NULL)
        return E_FAIL;
    RegisterApplicationRestartT proc =
        reinterpret_cast<RegisterApplicationRestartT>(::GetProcAddress(hModule, "RegisterApplicationRestart"));
    if (proc == NULL)
    {
        ::FreeLibrary(hModule);
        return E_FAIL;
    }
    HRESULT ret = (proc)(pwzCommandline, dwFlags);
    ::FreeLibrary(hModule);
    return ret;
}

} // namespace

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE, //hPrevInstance
                     LPTSTR,    //lpCmdLine
                     int       nCmdShow)
{
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_GIVEMESOMETIME, szWindowClass, MAX_LOADSTRING);
	
    ::CreateMutex(NULL, FALSE, _T("95676754JNTJN45K4T45TJN45KT756758A95"));
    DWORD dwLastError = ::GetLastError();
    if (dwLastError == ERROR_ALREADY_EXISTS)
    {
        //HWND hWndOther = FindWindow(szWindowClass, szTitle);
        //if (hWndOther != NULL)
        //{
        //    SendNotifyMessage(hWndOther, WM_RESTORE, 0, 0);
        //}
        return 0;
    }

	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

    RegisterApplicationRestartStub(L"", 0);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GIVEMESOMETIME));

	// Main message loop:
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}
