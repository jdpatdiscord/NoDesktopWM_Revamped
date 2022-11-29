#include <windows.h>
#include <handleapi.h>

#include <stdio.h>
#include <stdint.h>

#include "minhook/include/MinHook.h"

//#define DEBUGMODE

/*  def from https://blog.adeltax.com/window-z-order-in-windows-10/ */
enum ZBID
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

typedef BOOL    (WINAPI* T_GetWindowBand)      (HWND hWnd, PDWORD pdwBand);
typedef BOOL    (WINAPI* T_ShowWindowAsync)         (HWND hWnd, int nCmdShow);

T_GetWindowBand f_GetWindowBand = NULL;

void InitConsole()
{
	if (GetConsoleWindow() == NULL)
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
	}
}

HWND guilty = NULL;
T_ShowWindowAsync Tramp_ShowWindowAsync = NULL;
BOOL Hook_ShowWindowAsync(HWND hWnd, int nCmdShow)
{
    // These comments will serve a bit as documentation.

    // The previous version of this program was hooking WndProc to have an
    // event-based method of actually hiding the guilty window. This worked...
    // it was just buggy. As in, it showed up again for a split second.
    // This new method fixes it.

    // So, as for what was necessary to find this single point of failure...
    // I figured Microsoft was calling *something* to make their Watermark show again.
    // Hooking ShowWindow, the guilty window was not showing up in the calls.
    // But, I found ShowWindowAsync, a close neighbor and thought to hook it.
    // It wasn't obvious that this would be the one being called because
    // Microsoft actually bothered to OBFUSCATE the code related to the watermark.
    // However it WAS being called... the guilty window was being passed to this.
    // What seems the last bug to this program is fixed, and hooking WndProc is
    // no longer needed. Have fun!

    // I tried two methods of deleting this window. DestroyWindow and SendMessageA.
    // Neither worked. Returned access denied, for both. Makes sense for
    // DestroyWindow but isn't immediately obvious for SendMessageA. However it
    // wouldn't make sense to outright destroy this window anyways. Some Microsoft
    // code is probably running to start it up again. If anyone wants to go on
    // hunting and doing more research into what exactly Microsoft is doing here,
    // be my guest. Load up explorer.exe into IDA with symbols, and look for
    // `CImmersiveWatermark__Initialize` in the functions window. This is what
    // leads to where the watermark is  created, loaded, and maintained wholesale.
    // If symbols aren't available for you, just look for the CreateWindowInBand
    // xref.

    // The concept of "window bands" might be a bit new for you. It's quite easy.
    // As it have may been made obvious to you earlier, the ZBID enum is a list of
    // Z-index types for what windows supercede others in a list. The index being
    // used by Microsoft to keep the overlay on the screen at all times is quite
    // high on that list. You are not able to use GetWindowBand/SetWindowBand in
    // your own programs. Microsoft directly checks the caller's process to see if
    // it is signed Microsoft code, which is quite unfortunate... and called
    // downright anticompetitive depending on who you ask. 

    if (hWnd == guilty)
    {
        nCmdShow = SW_HIDE;
    }
    BOOL ret = Tramp_ShowWindowAsync(hWnd, nCmdShow);
    return ret;
}

BOOL CALLBACK PerWindowOp(HWND hWnd, LPARAM param)
{
    DWORD dwWindowBand = 0;
    f_GetWindowBand(hWnd, &dwWindowBand);
    if (dwWindowBand == ZBID_GENUINE_WINDOWS)
    {
        Tramp_ShowWindowAsync(hWnd, SW_HIDE);
        guilty = hWnd;
    }

    return TRUE;
}

BOOL WINAPI DllMain(HINSTANCE hCurrentModule, DWORD dwCallReason, LPVOID)
{
    if (dwCallReason == DLL_PROCESS_ATTACH)
    {
#if defined(DEBUGMODE)
        InitConsole();
#endif
        MH_Initialize();

        HANDLE g_hUser32Module = GetModuleHandleA("user32.dll");
        while (!g_hUser32Module)
        {
            g_hUser32Module = GetModuleHandleA("user32.dll");
            Sleep(0);
        }

        f_GetWindowBand = (T_GetWindowBand)GetProcAddress(g_hUser32Module, "GetWindowBand");
        
        MH_CreateHookApi(L"user32.dll", "ShowWindowAsync", Hook_ShowWindowAsync, (LPVOID*)&Tramp_ShowWindowAsync);
        MH_EnableHook(MH_ALL_HOOKS);

        EnumChildWindows(GetDesktopWindow(), &PerWindowOp, 0);
    }

    return TRUE;
}