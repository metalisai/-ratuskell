#include "systemclass.h"

SystemClass::SystemClass()
{
	// initialize pointers to 0
	m_hwnd = 0;
	m_hinstance = 0;

	hour=0;
	minute=0;
	second=0;
	m_alertactive=false;
	alerting = false;
}

SystemClass::SystemClass(const SystemClass& /*other*/)
{
}

SystemClass::~SystemClass()
{
	// initialize pointers to 0
	m_hwnd = 0;
	m_hinstance = 0;

	//m_MainWndClassName = L"Alert";
}

bool SystemClass::Initialize()
{
	ApplicationHandle = this;

	m_hinstance = GetModuleHandle(NULL);

	if(!RegisterMainWindowClass())
		return false;

	if(!CreateMainWindow()) 
		return false;

	if(!CreateTrayIcon())
		return false;

	SetMode(ALERTMODE_DATE);
	Reset();

	return true;
}

bool SystemClass::RegisterMainWindowClass()
{
	INITCOMMONCONTROLSEX icex;

	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_DATE_CLASSES;

	InitCommonControlsEx(&icex);

    m_wc.cbSize        = sizeof(WNDCLASSEX);
    m_wc.style         = 0;
    m_wc.lpfnWndProc   = WndProc;
    m_wc.cbClsExtra    = 0;
    m_wc.cbWndExtra    = 0;
    m_wc.hInstance     = m_hinstance;
	m_wc.hIconSm       = LoadIcon(m_hinstance, (LPCTSTR)IDI_ICON16X16);
    m_wc.hIcon         = LoadIcon(m_hinstance, (LPCTSTR)IDI_ICON32X32);
    m_wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    m_wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    m_wc.lpszMenuName  = NULL;
    m_wc.lpszClassName = L"WindowClass";

	if(!RegisterClassEx(&m_wc))
	{
		errorcode = 3001;
		MessageBox(m_hwnd,L"Error: 3001 \nUnable to register window class",L"lol",MB_OK|MB_ICONERROR);
        return false;
	}

	return true;
}

bool SystemClass::CreateMainWindow()
{
	m_hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"WindowClass",
        L"Äratus",
        WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 170,
        NULL, NULL, m_hinstance, NULL);

	if(m_hwnd == NULL)
    {
		errorcode = 3002;
		MessageBox(m_hwnd,L"Error: 3002 \nUnable to create window",L"lol",MB_OK|MB_ICONERROR);
        return 0;
    }

	ShowWindow(m_hwnd, SW_SHOW);
	UpdateWindow(m_hwnd);
	return true;
}

bool SystemClass::CreateTrayIcon()
{
	ZeroMemory(&m_nid,sizeof(NOTIFYICONDATA));
	m_nid.cbSize = sizeof(NOTIFYICONDATA);
	m_nid.hWnd = m_hwnd;
	m_nid.uID = SHELL_ICON_ID;
	m_nid.uFlags = NIF_ICON|NIF_TIP|NIF_MESSAGE;
	m_nid.uCallbackMessage = SHELL_CALLBACK;
	m_nid.hIcon = LoadIcon(m_hinstance, (LPCTSTR)IDI_ICON16X16);
	wcscpy_s(m_nid.szTip,L"Äratuskell");
	if(!Shell_NotifyIcon(NIM_ADD,&m_nid))
	{
		errorcode = 3003;
		MessageBox(m_hwnd,L"Error: 3003 \nUnable to create tray icon",L"lol",MB_OK|MB_ICONERROR);
		return 0;
	}

	m_stmenu = GetSubMenu(LoadMenu(m_hinstance,MAKEINTRESOURCE(IDR_MYMENU)),0);
	return true;
}

int SystemClass::Run()
{
	MSG Msg;

	// Initialize the message structure.
	ZeroMemory(&Msg, sizeof(MSG));

	while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
		TranslateMessage(&Msg);
        DispatchMessage(&Msg);
	}
	return Msg.wParam;
}

void SystemClass::ShutDown()
{
	//CloseThreads();

	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(L"WindowClass", m_hinstance);
	m_hinstance = NULL;

	ApplicationHandle = NULL;

	return;
}

bool SystemClass::CreateChildWindows(HWND hwnd)
{
	if(CreateWindow(L"Button",L"Lülita sisse",
		WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
		180,10,100,30,
		hwnd,(HMENU)IDB_BUTTON1,m_hinstance,NULL) == NULL)
		return 0;

	if(CreateWindow(L"Button",L"Lülita välja",
		WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
		180,45,100,30,
		hwnd,(HMENU)IDB_BUTTON2,m_hinstance,NULL) == NULL)
		return 0;

	m_TimePick = CreateWindow(DATETIMEPICK_CLASS,TEXT("DateTime"),
		WS_BORDER|WS_CHILD|WS_VISIBLE|DTS_TIMEFORMAT,
		10,20,160,20,
        hwnd,NULL,GetModuleHandle(NULL),NULL);
	if(m_TimePick == NULL) return 0;

	m_DatePick = CreateWindow(DATETIMEPICK_CLASS,TEXT("DateTime"),
		WS_BORDER|WS_CHILD|WS_VISIBLE|DTS_SHOWNONE,
		10,45,160,20,
        hwnd,(HMENU)IDB_DBUTTON1,GetModuleHandle(NULL),NULL);
	if(m_DatePick == NULL) return 0;

	SendMessage(m_DatePick,DTM_SETSYSTEMTIME,GDT_NONE,NULL);

	if(CreateWindow(L"Button",L"Igapäevane",
		WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON,
		10,75,100,30,
		hwnd,(HMENU)IDB_RBUTTON1,m_hinstance,NULL) == NULL)
		return 0;

	if(CreateWindow(L"Button",L"Argipäeviti",
		WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON,
		110,75,100,30,
		hwnd,(HMENU)IDB_RBUTTON2,m_hinstance,NULL) == NULL)
		return 0;

	if(CreateWindow(L"Button",L"Kuupäev",
		WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON,
		210,75,100,30,
		hwnd,(HMENU)IDB_RBUTTON3,m_hinstance,NULL) == NULL)
		return 0;

	m_Edit1 = CreateWindow(L"Edit",L"Äratus: Väljas",
		WS_CHILD|WS_VISIBLE|ES_READONLY,
		10,110,200,30,
		hwnd,NULL,m_hinstance,NULL);
	if(m_Edit1 == NULL) return 0;

	SetMode(ALERTMODE_DATE);
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	SendMessage(m_DatePick,DTM_SETSYSTEMTIME,GDT_VALID,(LPARAM)&systime);

	return 1;
}

void SystemClass::Picktime()
{
	if(m_TimePick == NULL) return ;

	SYSTEMTIME systime;

	SendMessage(m_TimePick,DTM_GETSYSTEMTIME,NULL,(LPARAM)&systime);

	/*wchar_t string[50];

	swprintf(string,50,L"Aeg: %d:%d:%d, Mode:%d Date: %d / %d / %d",systime.wHour,systime.wMinute,systime.wSecond,m_alertmode,systime.wDay,systime.wMonth,systime.wYear);

	MessageBox(m_hwnd,(LPWSTR)string,L"Teade",MB_OK);*/

	hour=systime.wHour;
	minute=systime.wMinute;
	second=systime.wSecond;

	switch(m_alertmode)
	{
	case ALERTMODE_DATE:
		day=systime.wDay;
		month=systime.wMonth;
		year=systime.wYear;
		break;

	default: 
		break;
	}

	Enable();
	
	return ;
}

void SystemClass::Disable()
{
	m_alertactive = false;
	SetWindowText(m_Edit1,L"Äratus: Väljas \0");
	Reset();
	CheckMenuItem(m_stmenu, ID_TRAYMENU_OFF, MF_CHECKED);
	CheckMenuItem(m_stmenu, ID_TRAYMENU_ON, MF_UNCHECKED);
	return ;
}
void SystemClass::Enable()
{
	m_alertactive = true;

	wchar_t string[90];

	int h,min,d,m,y;

	switch(m_alertmode)
	{
	case ALERTMODE_DATE:
		d=day;
		m=month;
		y=year;
		h=hour;
		min=minute;
		StringCchPrintf(string,90,L"Äratus: %d:%.2d %d/%.2d/%d",h,min,d,m,y);
		//swprintf(string,90,L"Äratus: %d:%.2d %d/%.2d/%d",h,min,d,m,y);
		break;
	case ALERTMODE_EVERYDAY:
		h=hour;
		min=minute;
		StringCchPrintf(string,90,L"Äratus: Igapäev %d:%.2d",h,min);
		break;
	case ALERTMODE_WEEKDAY:
		h=hour;
		min=minute;
		StringCchPrintf(string,90,L"Äratus: Tööpäeviti %d:%.2d",h,min);
		break;
	}

	SetWindowText(m_Edit1,string);

	CheckMenuItem(m_stmenu, ID_TRAYMENU_OFF, MF_UNCHECKED);
	CheckMenuItem(m_stmenu, ID_TRAYMENU_ON, MF_CHECKED);

	return ;
}

void SystemClass::Reset()
{
	ZeroMemory(&m_lastalert,sizeof(SYSTEMTIME));
	return ;
}

void SystemClass::CheckTime()
{
	SYSTEMTIME curTime;
	GetLocalTime(&curTime);

	if(m_alertactive && m_lastalert.wDay == curTime.wDay && m_lastalert.wMonth == curTime.wMonth && m_lastalert.wYear == curTime.wYear)
	{
		//MessageBox(m_hwnd,L"lol221",L"lol",MB_OK);
		return ;
	}

	else if(hour==curTime.wHour && minute==curTime.wMinute && m_alertactive)
	{
		switch(m_alertmode)
		{
		case ALERTMODE_DATE:
			if(curTime.wDay == day && curTime.wMonth == month && curTime.wYear == year)
			{
				Disable();
				Alert(curTime);
			}
			break;
		case ALERTMODE_WEEKDAY:
			if(curTime.wDayOfWeek > 0 && curTime.wDayOfWeek < 6)
			{
				Disable();
				Alert(curTime);
			}
			break;
		case ALERTMODE_EVERYDAY:
			Disable();
			Alert(curTime);
			break;
		}
	}
	return ;
}

void SystemClass::Alert(SYSTEMTIME Time)
{
	switch(m_alertmode)
	{
	case ALERTMODE_DATE:
		Reset();
		break;
	default:
		m_lastalert = Time;
	}
	
	alerting = true;

	PlaySound(MAKEINTRESOURCE(IDR_ALARM1),m_hinstance,SND_RESOURCE|SND_ASYNC|SND_LOOP);
		//PlaySound();

	ShowWindow(m_hwnd,SW_SHOW);

	SetWindowText(m_Edit1,L"Äratab!");

	if(m_alertmode != ALERTMODE_DATE) Enable();

	return  ;
}

void SystemClass::StopAlerting()
{
	PlaySound(NULL,NULL,NULL);
	alerting = false;

	wchar_t string[90];

	switch(m_alertmode)
	{
	case ALERTMODE_DATE:
		StringCchPrintf(string,90,L"Äratus: Väljas");
		//swprintf(string,90,L"Äratus: %d:%.2d %d/%.2d/%d",h,min,d,m,y);
		break;
	case ALERTMODE_EVERYDAY:
		StringCchPrintf(string,90,L"Äratus: Igapäev %d:%.2d",hour,minute);
		break;
	case ALERTMODE_WEEKDAY:
		StringCchPrintf(string,90,L"Äratus: Tööpäeviti %d:%.2d",hour,minute);
		break;
	}

	SetWindowText(m_Edit1,string);
}

/*void SystemClass::PlaySound()
{
    DWORD   dwThreadIdArray[MAX_THREADS];

    // Create MAX_THREADS worker threads.

    for( int i=0; i<MAX_THREADS; i++ )
    {
        // Allocate memory for thread data.

        pDataArray[i] = (PMYDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                sizeof(MYDATA));

        if( pDataArray[i] == NULL )
        {
           // If the array allocation fails, the system is out of memory
           // so there is no point in trying to print an error message.
           // Just terminate execution.
            ExitProcess(2);
        }

        // Generate unique data for each thread to work with.

        pDataArray[i]->val1 = i;
        pDataArray[i]->val2 = i+100;

        // Create the thread to begin execution on its own.

        hThreadArray[i] = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            MyThreadFunction,       // thread function name
            pDataArray[i],          // argument to thread function 
            0,                      // use default creation flags 
            &dwThreadIdArray[i]);   // returns the thread identifier 


        // Check the return value for success.
        // If CreateThread fails, terminate execution. 
        // This will automatically clean up threads and memory. 

        if (hThreadArray[i] == NULL) 
        {
           //ErrorHandler(TEXT("CreateThread"));
           ExitProcess(3);
        }
    } // End of main thread creation loop.

    return ;
}*/

/*void SystemClass::CloseThreads()
{
	
	// Wait until all threads have terminated.

    WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

    // Close all thread handles and free memory allocations.

	for(int i=0; i<MAX_THREADS; i++)
    {
        CloseHandle(hThreadArray[i]);
        if(pDataArray[i] != NULL)
        {
            HeapFree(GetProcessHeap(), 0, pDataArray[i]);
            pDataArray[i] = NULL;    // Ensure address is not reused.
        }
    }
}*/

void SystemClass::EndCycle()
{
	return ;
}

void SystemClass::SetMode(int mode)
{
	switch(mode)
	{
	case ALERTMODE_DATE:
		CheckRadioButton(m_hwnd,IDB_RBUTTON1,IDB_RBUTTON3,IDB_RBUTTON3);
		m_alertmode=ALERTMODE_DATE;
		Disable();
		EndCycle();
		break;
	case ALERTMODE_WEEKDAY:
		CheckRadioButton(m_hwnd,IDB_RBUTTON1,IDB_RBUTTON3,IDB_RBUTTON2);
		m_alertmode=ALERTMODE_WEEKDAY;
		Disable();
		EndCycle();
		break;
	case ALERTMODE_EVERYDAY:
		CheckRadioButton(m_hwnd,IDB_RBUTTON1,IDB_RBUTTON3,IDB_RBUTTON1);
		m_alertmode=ALERTMODE_EVERYDAY;
		Disable();
		EndCycle();
		break;
	default:
		break;
	}
	return ;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	int wmId;

	switch(umsg)
	{

	case WM_CREATE:
		if(!CreateChildWindows(hwnd))
		{
			MessageBox(hwnd,L"Error: \nUnable to create controls",L"Error",MB_OK|MB_ICONERROR);
			SendMessage(hwnd,WM_CLOSE,NULL,NULL);
		}
			
		// Create timer
		SetTimer(hwnd,NULL,1000,NULL);
		return 0;
		break;
	//-----------------------------------------------------------------------

	case WM_CLOSE:
		Shell_NotifyIcon(NIM_DELETE, &m_nid);
		DestroyWindow(hwnd);
		return 0;
		break;

	//-----------------------------------------------------------------------
	case WM_NOTIFY:
		NMDATETIMECHANGE* datetimechange;
		datetimechange = (NMDATETIMECHANGE*)lparam;
		if(((LPNMHDR)lparam)->hwndFrom == m_DatePick)
		{
			switch (((LPNMHDR)lparam)->code)
			{
				case DTN_DATETIMECHANGE:
					switch(m_alertmode)
					{
					case ALERTMODE_WEEKDAY:
						SetMode(ALERTMODE_DATE);
						SYSTEMTIME systime;
						GetLocalTime(&systime);
						SendMessage(m_DatePick,DTM_SETSYSTEMTIME,GDT_VALID,(LPARAM)&systime);
						break;

					case ALERTMODE_EVERYDAY:
						SetMode(ALERTMODE_DATE);
						SYSTEMTIME Systime;
						GetLocalTime(&Systime);
						SendMessage(m_DatePick,DTM_SETSYSTEMTIME,GDT_VALID,(LPARAM)&Systime);
						break;

					case ALERTMODE_DATE:
						if(datetimechange->dwFlags == GDT_NONE)
						{
							SetMode(ALERTMODE_EVERYDAY);
							SendMessage(m_DatePick,DTM_SETSYSTEMTIME,GDT_NONE,NULL);
							Disable();
						}
						break;
				
					break;
					}
				break;
			}
		}
	return 0;
	break;

	//--------------------------------------------------------------------------
	case WM_COMMAND:
		wmId    = LOWORD(wparam);
		//wmEvent = HIWORD(wparam);

		switch(wmId)
		{
		case IDB_BUTTON1:
			Picktime();
			break;

		case IDB_BUTTON2:
			if(alerting)
				StopAlerting();
			else if(m_alertactive)
				Disable();
			break;

		case IDB_RBUTTON1:
			m_alertmode=ALERTMODE_EVERYDAY;
			SendMessage(m_DatePick,DTM_SETSYSTEMTIME,GDT_NONE,NULL);
			Disable();
			break;

		case IDB_RBUTTON2:
			m_alertmode=ALERTMODE_WEEKDAY;
			SendMessage(m_DatePick,DTM_SETSYSTEMTIME,GDT_NONE,NULL);
			Disable();
			break;

		case IDB_RBUTTON3:
			m_alertmode=ALERTMODE_DATE;
			SYSTEMTIME systime;
			GetLocalTime(&systime);
			SendMessage(m_DatePick,DTM_SETSYSTEMTIME,GDT_VALID,(LPARAM)&systime);
			Disable();
			break;

		case ID_TRAYMENU_EXIT:
			PostMessage(hwnd,WM_CLOSE,NULL,NULL);
			break;

		case ID_TRAYMENU_ON:
			Picktime();
			break;

		case ID_TRAYMENU_OFF:
			if(alerting)
				StopAlerting();
			else if(m_alertactive)
				Disable();
			break;

		case ID_TRAYMENU_ALEOFF:
			StopAlerting();
			break;

		break;
		}
		return 0;
	break;

	//--------------------------------------------------------------------------

	case WM_TIMER:
		wmId = wparam;
		//MessageBox(hwnd,L"Alert check",L"Notification",MB_OK);
		CheckTime();
		return 0;
		break;

	//---------------------------------------------------------------------------
	
	case SHELL_CALLBACK:
		switch(lparam)
		{
		case WM_RBUTTONDOWN:
			SetForegroundWindow(hwnd);
			POINT pnt;
			GetCursorPos(&pnt);
			TrackPopupMenu(m_stmenu,TPM_RIGHTBUTTON,pnt.x,pnt.y,NULL,m_hwnd,NULL);
			PostMessage(hwnd, WM_NULL, 0, 0);
			break;

		case WM_LBUTTONDBLCLK:
			ShowWindow(hwnd,SW_SHOW);
		}
		return 0;
		break;

	case WM_SYSCOMMAND:
	switch(wparam)
	{
		case SC_MINIMIZE:
			ShowWindow(hwnd,SW_HIDE);
			return 0;
			break;
	}

	//---------------------------------------------------------------------------
	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}

	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
	case WM_DESTROY:
		PostQuitMessage(0);
	break;
	//////////////////////////////////////////////////////////////////
	default:
            return ApplicationHandle->MessageHandler(hwnd, msg, wParam, lParam);
    }
    return 0;
}

/*DWORD WINAPI MyThreadFunction( LPVOID lpParam ) 
{ 
    HANDLE hStdout;
    PMYDATA pDataArray;

    TCHAR msgBuf[BUF_SIZE];
    size_t cchStringSize;
    DWORD dwChars;

    // Make sure there is a console to receive output results. 

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if( hStdout == INVALID_HANDLE_VALUE )
        return 1;

    // Cast the parameter to the correct data type.
    // The pointer is known to be valid because 
    // it was checked for NULL before the thread was created.
 
    pDataArray = (PMYDATA)lpParam;

    // Print the parameter values using thread-safe functions.

    StringCchPrintf(msgBuf, BUF_SIZE, TEXT("Parameters = %d, %d\n"), 
        pDataArray->val1, pDataArray->val2); 
    StringCchLength(msgBuf, BUF_SIZE, &cchStringSize);
    WriteConsole(hStdout, msgBuf, (DWORD)cchStringSize, &dwChars, NULL);

	if(ApplicationHandle->alerting)
		PlaySound(L"alarm.wav",NULL,SND_LOOP);

    return 0; 
} */