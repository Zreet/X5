// Win32Project1.cpp : ¶¨ÒåÓ¦ÓÃ³ÌÐòµÄÈë¿Úµã¡£
//

#include "stdafx.h"
#include "Win32Project1.h"
#include "bao.h"
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#define MAX_LOADSTRING 100
HWND hip, hbt5, het3;
HWND hport;
HFONT hFont;
CRITICAL_SECTION CS;
// È«¾Ö±äÁ¿: 
HINSTANCE hInst;                                // µ±Ç°ÊµÀý
WCHAR szTitle[MAX_LOADSTRING] = L"Fire Table";                  // ±êÌâÀ¸ÎÄ±¾
WCHAR szWindowClass[MAX_LOADSTRING] = L"Fire Table 1.0";            // Ö÷´°¿ÚÀàÃû

// ´Ë´úÂëÄ£¿éÖÐ°üº¬µÄº¯ÊýµÄÇ°ÏòÉùÃ÷: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void ReadNote(ofstream &out, int type)
{
	out << "name" << ',' << "artist" << ',' << "file name" << ',' << "half combo" << ',' << "full combo" << endl;
	out << "solo skill" << ',' << "solo max score" << endl;
	out << "5 solo point rank" << endl;
	out << "combo" << ',' << "score" ;
	if (type)
		out << ',' << "Bubble index";
	out << ',' << "type" << ',' << "bar" << ',' << "pos";
	out << endl;
	out << "5 dual point rank" << endl;
	out << "combo" << ',' << "combo" << ',' << "score";
	if (type)
		out << ',' << "Bubble index" << ',' << "Bubble index";
	out << ',' << "type" << ',' << "type" << ',' << "bar" << ',' << "pos" << ',' << "bar" << ',' << "pos";
	out << endl << endl;
}
DWORD WINAPI workerProcess(LPVOID lpParameter)
{
	char buf[0x7FFF] = { 0 };
	ofstream idols, pinballs, bubbles, crescents;
	idols.open("idol_mod.csv");
	ReadNote(idols, 0);
	getAll("idol", idols, 0);
	getAll("old_idol", idols, 1);
	idols.close();
	pinballs.open("pinball_mod.csv");
	ReadNote(pinballs, 0);
	getAll("pinball", pinballs, 0);
	getAll("old_pinball", pinballs, 1);
	pinballs.close();
	bubbles.open("bubble_mod.csv");
	ReadNote(bubbles, 1);
	getAll("bubble", bubbles, 0);
	getAll("old_bubble", bubbles, 1);
	bubbles.close();
	crescents.open("crescent_mod.csv");
	ReadNote(crescents, 0);
	getAll("crescent", crescents, 0);
	crescents.close();
	GetWindowText(het1, buf, 0x7FFF);
	strcat(buf, "Done! \r\n");
	SetWindowText(het1, buf);
	return 0;
}
DWORD WINAPI idolProcess(LPVOID lpParameter)
{
	char buf[0x7FFF] = { 0 };
	ofstream out;
	out.open("idol_mod.csv");
	ReadNote(out, 0);
	getAll("idol", out, 0);
	getAll("old_idol", out, 1);
	out.close();
	EnterCriticalSection(&CS);
	GetWindowText(het1, buf, 0x7FFF);
	strcat(buf, "idol Done! \r\n");
	SetWindowText(het1, buf);
	LeaveCriticalSection(&CS);
	return 0;
}
DWORD WINAPI pinballProcess(LPVOID lpParameter)
{
	ofstream out;
	char buf[0x7FFF] = { 0 };
	out.open("pinball_mod.csv");
	ReadNote(out, 0);
	getAll("pinball", out, 0);
	getAll("old_pinball", out, 1);
	out.close();
	EnterCriticalSection(&CS);
	GetWindowText(het1, buf, 0x7FFF);
	strcat(buf, "pinball Done~! \r\n");
	SetWindowText(het1, buf);
	LeaveCriticalSection(&CS);
	return 0;
}
DWORD WINAPI bubbleProcess(LPVOID lpParameter)
{
	ofstream out;
	char buf[0x7FFF] = { 0 };
	out.open("bubble_mod.csv");
	ReadNote(out, 1);
	getAll("bubble", out, 0);
	getAll("old_bubble", out, 1);
	out.close();
	EnterCriticalSection(&CS);
	GetWindowText(het1, buf, 0x7FFF);
	strcat(buf, "bubble Done!\r\n");
	SetWindowText(het1, buf);
	LeaveCriticalSection(&CS);
	return 0;
}

DWORD WINAPI crescentProcess(LPVOID lpParameter)
{
	char buf[0x7FFF] = { 0 };
	ofstream out;
	out.open("crescent_mod.csv");
	ReadNote(out, 0);
	getAll("crescent", out, 0);
	out.close();
	EnterCriticalSection(&CS);
	GetWindowText(het1, buf, 0x7FFF);
	strcat(buf, "crescent Done! \r\n");
	SetWindowText(het1, buf);
	LeaveCriticalSection(&CS);
	return 0;
}
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ÔÚ´Ë·ÅÖÃ´úÂë¡£

    // ³õÊ¼»¯È«¾Ö×Ö·û´®
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32PROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Ö´ÐÐÓ¦ÓÃ³ÌÐò³õÊ¼»¯: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));

    MSG msg;

    // Ö÷ÏûÏ¢Ñ­»·: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  º¯Êý: MyRegisterClass()
//
//  Ä¿µÄ: ×¢²á´°¿ÚÀà¡£
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32PROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));

    return RegisterClassExW(&wcex);
}

//
//   º¯Êý: InitInstance(HINSTANCE, int)
//
//   Ä¿µÄ: ±£´æÊµÀý¾ä±ú²¢´´½¨Ö÷´°¿Ú
//
//   ×¢ÊÍ: 
//
//        ÔÚ´Ëº¯ÊýÖÐ£¬ÎÒÃÇÔÚÈ«¾Ö±äÁ¿ÖÐ±£´æÊµÀý¾ä±ú²¢
//        ´´½¨ºÍÏÔÊ¾Ö÷³ÌÐò´°¿Ú¡£
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // ½«ÊµÀý¾ä±ú´æ´¢ÔÚÈ«¾Ö±äÁ¿ÖÐ

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_VSCROLL,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   InitializeCriticalSection(&CS);
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  º¯Êý: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ä¿µÄ:    ´¦ÀíÖ÷´°¿ÚµÄÏûÏ¢¡£
//
//  WM_COMMAND  - ´¦ÀíÓ¦ÓÃ³ÌÐò²Ëµ¥
//  WM_PAINT    - »æÖÆÖ÷´°¿Ú
//  WM_DESTROY  - ·¢ËÍÍË³öÏûÏ¢²¢·µ»Ø
//
//
void messageHead()
{
	char buf[0x7FFF] = { 0 };
	EnterCriticalSection(&CS);
	GetWindowText(het1, buf, 0x7FFF);
	string len = to_string(threshold);
	len = "now fire time: " + len + "\r\nIt will take several seconds, processing......\r\n";
	strcat(buf, len.c_str());
	SetWindowText(het1, buf);
	LeaveCriticalSection(&CS);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		AllocConsole();
		freopen("CONIN$", "r+t", stdin); // 重定向 STDIN
		freopen("CONOUT$", "w+t", stdout); // 重定向STDOUT
		SetWindowText(hWnd, "FireTable Generator");
		hbt1 = CreateWindow("Button", "update all", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
			35, 10, 140, 60, hWnd, (HMENU)IDB_ONE, hInst, NULL);
		hbt2 = CreateWindow("Button", "change time", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
			35, 80, 140, 60, hWnd, (HMENU)IDB_TWO, hInst, NULL);
		hbt3 = CreateWindow("Button", "update idol", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
			35, 150, 140, 60, hWnd, (HMENU)IDB_THREE, hInst, NULL);
		hbt4 = CreateWindow("Button", "update pinball", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
			35, 220, 140, 60, hWnd, (HMENU)IDB_FOUR, hInst, NULL);
		hbt5 = CreateWindow("Button", "update bubble", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
			35, 410, 140, 60, hWnd, (HMENU)IDB_FIVE, hInst, NULL);
		hbt6 = CreateWindow("Button", "query score", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
			35, 480, 150, 60, hWnd, (HMENU)IDB_SIX, hInst, NULL);
		het1 = CreateWindow(   //edit¿Ø¼þ    
			"edit",
			"",
			WS_VISIBLE | WS_CHILD | WS_BORDER | DT_VCENTER | WS_VSCROLL,
			200, 10, 1200, 470,
			hWnd,
			(HMENU)ID_EDIT,
			hInst,
			NULL);
		het3 = CreateWindow(   //edit¿Ø¼þ    
			"edit",
			"",
			WS_VISIBLE | WS_CHILD | WS_BORDER | DT_VCENTER ,
			550, 490, 300, 28,
			hWnd,
			(HMENU)ID_ROUTE,
			hInst,
			NULL);
		hip = CreateWindow(   //edit¿Ø¼þ    
			"edit",
			"",
			WS_VISIBLE | WS_CHILD | WS_BORDER | DT_VCENTER,
			35, 310, 150, 28,
			hWnd,
			(HMENU)ID_EDIT,
			hInst,
			NULL);
		hport = CreateWindow(   //edit¿Ø¼þ    
			"edit",
			"",
			WS_VISIBLE | WS_CHILD | WS_BORDER | DT_VCENTER | ES_NUMBER,
			35, 370, 150, 28,
			hWnd,
			(HMENU)ID_EDIT,
			hInst,
			NULL);
		hFont = CreateFont(
			-16/*¸ß¶È*/, 0/*¿í¶È*/, 0, 0, 400/*Ò»°ãÉèÖµÎª400*/,
			FALSE/*²»´øÐ±Ìå*/, FALSE/*²»´øÏÂ»®Ïß*/, FALSE/*²»´øÉ¾³ýÏß*/,
			DEFAULT_CHARSET, //Ê¹ÓÃÄ¬ÈÏ×Ö·û¼¯
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
			DEFAULT_QUALITY, //Ä¬ÈÏÊä³öÖÊÁ¿
			FF_DONTCARE, //²»Ö¸¶¨×ÖÌå×å
			TEXT("consolas") //×ÖÌåÃû
		);
		::SendMessage(hbt1, WM_SETFONT, (WPARAM)hFont, 1);
		::SendMessage(hbt2, WM_SETFONT, (WPARAM)hFont, 1);
		::SendMessage(hbt3, WM_SETFONT, (WPARAM)hFont, 1);
		::SendMessage(hbt4, WM_SETFONT, (WPARAM)hFont, 1);
		::SendMessage(hbt5, WM_SETFONT, (WPARAM)hFont, 1);
		::SendMessage(hbt6, WM_SETFONT, (WPARAM)hFont, 1);
		::SendMessage(het1, WM_SETFONT, (WPARAM)hFont, 1);  //ÉèÖÃ¿Ø¼þ×ÖÌå   
		::SendMessage(het3, WM_SETFONT, (WPARAM)hFont, 1);
		::SendMessage(hip, WM_SETFONT, (WPARAM)hFont, 1);
		::SendMessage(hport, WM_SETFONT, (WPARAM)hFont, 1);
		::PostMessageW(het1, EM_SETREADONLY, 1, 0);
		::PostMessageW(hip, EM_SETREADONLY, 1, 0);
		SetWindowText(hip, "320");
		SetWindowText(het1, "Please place me at muisc library folder\r\n");
		SetWindowText(het3, "idol/idol_100002.xml");
	}
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // ·ÖÎö²Ëµ¥Ñ¡Ôñ: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case IDB_ONE:
			{
				messageHead();
				HANDLE hThread1 = CreateThread(NULL, 0, idolProcess, NULL, 0, NULL);
				HANDLE hThread2 = CreateThread(NULL, 0, pinballProcess, NULL, 0, NULL);
				HANDLE hThread3 = CreateThread(NULL, 0, bubbleProcess, NULL, 0, NULL);
				HANDLE hThread4 = CreateThread(NULL, 0, crescentProcess, NULL, 0, NULL);
				break;
			}
			case IDB_TWO:
			{
				char buf[0x7FFF] = { 0 };
				GetWindowText(hport, buf, 0x7FFF);
				threshold = atoi(buf);
				SetWindowText(hip, buf);
				break;
			}
			case IDB_THREE:
			{
				messageHead();
				HANDLE hThread = CreateThread(NULL, 0, idolProcess, NULL, 0, NULL);
				break;
			}
			case IDB_FOUR:
			{
				messageHead();
				HANDLE hThread = CreateThread(NULL, 0, pinballProcess, NULL, 0, NULL);
				break;
				break;
			}
			case IDB_FIVE:
			{
				messageHead();
				HANDLE hThread = CreateThread(NULL, 0, bubbleProcess, NULL, 0, NULL);
				break;
			}
			case IDB_SIX:
			{
				char buf[0x7FFF] = { 0 };
				twoNum tmp(-1, -1);
				GetWindowText(het3, buf, 0x7FFF);
				string bbuf = buf;
				ofstream ftmp("tmp");
				AnalysisOne(buf, pinball, ftmp);
				ftmp.close();
				break;
			}
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			SelectObject(hdc, hFont);
			TextOut(hdc, 35, 290, "current fire time", strlen("current fire time"));
			TextOut(hdc, 35, 350, "reset fire time", strlen("reset fire time"));
			TextOut(hdc, 200, 490, "input file name", strlen("input file name"));
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		DeleteCriticalSection(&CS);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// ¡°¹ØÓÚ¡±¿òµÄÏûÏ¢´¦Àí³ÌÐò¡£
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
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
