#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "injector.h"

typedef NTSTATUS (WINAPI* T_NtQuerySystemInformation)(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID pSystemInformation, ULONG SystemInformationLength, PULONG pLength);

const wchar_t* DLL_NAME = L"nodesktopwm_revamp.dll";

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmd, int cmdshow)
{
    HANDLE hExplorerTask = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_VM_OPERATION, FALSE, 0);
    if (!hExplorerTask)
        return 0;
    
    PVOID pLoadLibrary = (PVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
    

    return 0;
}