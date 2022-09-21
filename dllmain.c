#include <Windows.h>
#include <Handleapi.h>

#include <stdio.h>

typedef BOOL    (WINAPI* T_GetWindowBand)     (HWND hWnd, PDWORD pdwBand);
typedef BOOL    (WINAPI* T_SetWindowBand)     (HWND hWnd, DWORD dwBand);

enum ZBID /*  def from https://blog.adeltax.com/window-z-order-in-windows-10/ */
{
     ZBID_DEFAULT = 0,
     ZBID_DESKTOP = 1,
     ZBID_UIACCESS = 2,
     ZBID_IMMERSIVE_IHM = 3,
     ZBID_IMMERSIVE_NOTIFICATION = 4,
     ZBID_IMMERSIVE_APPCHROME = 5,
     ZBID_IMMERSIVE_MOGO = 6,
     ZBID_IMMERSIVE_EDGY = 7,
     ZBID_IMMERSIVE_INACTIVEMOBODY = 8,
     ZBID_IMMERSIVE_INACTIVEDOCK = 9,
     ZBID_IMMERSIVE_ACTIVEMOBODY = 10,
     ZBID_IMMERSIVE_ACTIVEDOCK = 11,
     ZBID_IMMERSIVE_BACKGROUND = 12,
     ZBID_IMMERSIVE_SEARCH = 13,
     ZBID_GENUINE_WINDOWS = 14,
     ZBID_IMMERSIVE_RESTRICTED = 15,
     ZBID_SYSTEM_TOOLS = 16,
 
     /* Windows 10+ */
     ZBID_LOCK = 17,
     ZBID_ABOVELOCK_UX = 18
};

T_GetWindowBand f_GetWindowBand = NULL;
T_SetWindowBand f_SetWindowBand = NULL;
HMODULE g_hUser32Module = NULL;
HMODULE g_hCurrentModule = NULL;
DWORD g_hExplorerTid = 0;
WNDPROC g_originalWndProc = NULL;

HWND g_hwndCache = NULL;

LRESULT CALLBACK CustomWndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    /* Don't care, go away */

    ShowWindow(hWnd, SW_HIDE); /* double team, the less milliseconds on my screen the better */
    DWORD res = g_originalWndProc(hWnd, msg, wparam, lparam);
    ShowWindow(hWnd, SW_HIDE);

    return res;

    /* return g_originalWndProc(hWnd, msg, wparam, lparam); */
}

BOOL CALLBACK PerWindowOp(HWND hWnd, LPARAM param)
{
    DWORD dwWindowBand = 0;
    f_GetWindowBand(hWnd, &dwWindowBand);
    if (dwWindowBand == ZBID_GENUINE_WINDOWS)
    {
        /* printf("found overlay\n"); */

        /* Come here, little pig! */

        g_hwndCache = hWnd;
        g_originalWndProc = (WNDPROC)GetWindowLongPtrW(hWnd, GWLP_WNDPROC);
        SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)CustomWndProc);

        f_SetWindowBand(hWnd, ZBID_DEFAULT);
        ShowWindow(hWnd, SW_HIDE);
    }

    return TRUE;
}

VOID WindowWatchProc(PVOID pThreadArg)
{
/*	if (GetConsoleWindow() == NULL)
	{
		AllocConsole();
		SetConsoleTitleA("No Watermark Debug Console");
#if defined(__MINGW32__) || defined(__MINGW64__)
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
		freopen("CONIN$", "r", stdin);
#else
        FILE* stream = NULL;
		freopen_s(&stream, "CONOUT$", "w", stdout);
		freopen_s(&stream, "CONOUT$", "w", stderr);
		freopen_s(&stream, "CONIN$", "r", stdin);
#endif
		HWND ConsoleHandle = GetConsoleWindow();

		HANDLE stdHnd = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dwMode;
		GetConsoleMode(stdHnd, &dwMode);
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(stdHnd, dwMode);

		SetWindowPos(ConsoleHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		ShowWindow(ConsoleHandle, SW_NORMAL);
	}*/

    EnumChildWindows(GetDesktopWindow(), &PerWindowOp, 0);
    
    return;
}

BOOL WINAPI DllMain(HINSTANCE hCurrentModule, DWORD dwCallReason, LPVOID)
{
    g_hCurrentModule = hCurrentModule;
    g_hExplorerTid = GetCurrentThreadId();

    if (dwCallReason == DLL_PROCESS_ATTACH)
    {
        g_hUser32Module = GetModuleHandleA("user32.dll");
        while (!g_hUser32Module)
        {
            g_hUser32Module = GetModuleHandleA("user32.dll");
            Sleep(0);
        }

        f_GetWindowBand = (T_GetWindowBand)GetProcAddress(g_hUser32Module, "GetWindowBand");
        f_SetWindowBand = (T_SetWindowBand)GetProcAddress(g_hUser32Module, "SetWindowBand");

        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&WindowWatchProc, NULL, 0, NULL);
    }

    if (dwCallReason == DLL_PROCESS_DETACH)
    {
        SetWindowLongPtrW(g_hwndCache, GWLP_WNDPROC, (LONG_PTR)g_originalWndProc);
    }

    return TRUE;
}