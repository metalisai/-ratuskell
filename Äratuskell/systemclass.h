#include <Windows.h>
#include <CommCtrl.h>

#include <wchar.h>
#include <Strsafe.h>

#include "resources.h"

#pragma comment(lib,"ComCtl32.lib")
#pragma comment(lib,"winmm.lib")

#define SHELL_ICON_ID	606
#define SHELL_CALLBACK WM_USER+1

#define IDB_BUTTON1		400 // turn on button
#define IDB_RBUTTON1	401 // everyday button
#define IDB_RBUTTON2	402 // weekday button
#define IDB_RBUTTON3	403 // date button
#define IDB_DBUTTON1	403 // date picker
#define IDB_BUTTON2		404 // turnoff button

#define ALERTMODE_DATE		0
#define ALERTMODE_WEEKDAY	1
#define ALERTMODE_EVERYDAY	2

#define MAX_THREADS 1
#define BUF_SIZE 255

typedef struct MyData {
    int val1;
    int val2;
} MYDATA, *PMYDATA;

class SystemClass
{
private:

	bool RegisterMainWindowClass();
	bool CreateMainWindow();
	bool CreateTrayIcon();

	bool CreateChildWindows(HWND hwnd);

	void Picktime();
	void CheckTime();
	void Alert(SYSTEMTIME);
	void Disable();
	void Enable();
	void EndCycle();
	void SetMode(int mode);
	void Reset();
	void StopAlerting();
	//void PlaySound();
	//void CloseThreads();

	// variables

	HINSTANCE m_hinstance;
	HWND m_hwnd;
	WNDCLASSEX m_wc;

	NOTIFYICONDATA m_nid;
	HMENU m_stmenu; // system tray menu

	HWND m_TimePick;
	HWND m_DatePick;

	HANDLE hThreadArray[MAX_THREADS];
	PMYDATA pDataArray[MAX_THREADS];

	HWND m_Edit1;

	bool m_alertactive;
	bool m_alertedtoday;

	SYSTEMTIME m_lastalert;

	int m_alertmode;

	int timer1;
	int timer2;

public:

	// constructor
	SystemClass();
	SystemClass(const SystemClass&);
	//  destructor
	~SystemClass();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

	bool Initialize();
	int Run();
	void ShutDown();

	int hour,minute,second;
	int day,month,year;
	bool alerting;

	int errorcode;
};

/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI MyThreadFunction( LPVOID lpParam );


/////////////
// GLOBALS //
/////////////
static SystemClass* ApplicationHandle = 0;