#include <windows.h>
#include <psapi.h>
#include <shlwapi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "injector.h"

typedef struct _PROCESS_BASIC_INFORMATION {
    NTSTATUS ExitStatus;
    PVOID PebBaseAddress;
    ULONG_PTR AffinityMask;
    KPRIORITY BasePriority;
    ULONG_PTR UniqueProcessId;
    ULONG_PTR InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION;

//typedef NTSTATUS (WINAPI* T_NtQuerySystemInformation)(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID pSystemInformation, ULONG SystemInformationLength, PULONG pLength);
//typedef NTSTATUS (WINAPI* T_NtQueryInformationProcess)(HANDLE hProcess, int ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength);
//T_NtQuerySystemInformation F_NtQuerySystemInformation = NULL;
//T_NtQueryInformationProcess F_NtQueryInformationProcess = NULL;

PWCHAR RefExtractNameOnlyFromPathW(PWCHAR wstr, DWORD eos)
{
    PWCHAR name_filenameonly = NULL;
    for (unsigned j = eos; j > 0; --j)
    {
        if (wstr[j] == L'\\')
        {
            name_filenameonly = &wstr[j + 1];
            break;
        }
    }
    return name_filenameonly;
}

int main(int argc, char* argv[])
{
    if (argc <= 1) return 0;

    TCHAR name_dll[MAX_PATH + 1];
    DWORD ret = GetFullPathNameA(argv[1], sizeof(name_dll), (LPSTR)&name_dll, NULL);
    if (!ret)
    {
        return 0;
    }

    //F_NtQuerySystemInformation = (T_NtQuerySystemInformation)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQuerySystemInformation");
    //F_NtQueryInformationProcess = (T_NtQueryInformationProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");

    size_t multiple = 128;
    DWORD* pid_buf = malloc(sizeof(DWORD) * multiple);
    if (!pid_buf)
    {
        return 0;
    }

    DWORD written;
    while (1)
    {
        if (!EnumProcesses(pid_buf, sizeof(DWORD) * multiple, &written))
        {
            free(pid_buf);
            return 0;
        }
        if (written == (sizeof(DWORD) * multiple))
        {
            multiple *= 2;
            DWORD* tmp_pid_buf = (DWORD*)realloc(pid_buf, sizeof(DWORD) * multiple);
            if (!tmp_pid_buf)
            {
                free(pid_buf);
                return 0;
            }
            pid_buf = tmp_pid_buf;

            continue;
        }
        
        break;
    }

    BOOL bFirstTime = TRUE;
    DWORD desiredExplorerPid = 0;
    FILETIME oldestExplorerTime = { 0x00000000 };
    for (unsigned i = 0u; i < written / 4; ++i)
    {
        HANDLE hTmpProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid_buf[i]);
        if (!hTmpProcess)
            continue;
        
        WCHAR name[MAX_PATH + 1];
        PWCHAR name_filenameonly = NULL;
        DWORD eos = GetProcessImageFileNameW(hTmpProcess, (LPWSTR)&name, MAX_PATH);
        if (!eos)
            continue;
        name_filenameonly = RefExtractNameOnlyFromPathW(name, eos);
        if (name_filenameonly == NULL)
            continue;
        if (!wcscmp(name_filenameonly, L"explorer.exe"))
        {
/*            PROCESS_BASIC_INFORMATION pbi;
            F_NtQueryInformationProcess(hTmpProcess, 0, &pbi, sizeof(pbi), NULL);
            HANDLE hTmpProcess2 = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pbi.InheritedFromUniqueProcessId);
            if (!hTmpProcess2)
            {
                printf("couldn't perform critical logic\n");
                CloseHandle(hTmpProcess);
                continue;
            }
            eos = GetProcessImageFileNameW(hTmpProcess2, (LPWSTR)&name, MAX_PATH);
            name_filenameonly = RefExtractNameOnlyFromPathW(name, eos);
            if (!wcscmp(name_filenameonly, L"winlogon.exe"))
            {
                desiredExplorerPid = pbi.InheritedFromUniqueProcessId;
                CloseHandle(hTmpProcess);
                CloseHandle(hTmpProcess2);
                break;
            }
            CloseHandle(hTmpProcess2);*/
            FILETIME creationtime;
            FILETIME exittime;
            FILETIME usertime;
            FILETIME kerneltime;
            if (!GetProcessTimes(hTmpProcess, &creationtime, &exittime, &kerneltime, &usertime))
            {
                printf("can't read explorer\n");
                free(pid_buf);
                return 0;
            }
            if (bFirstTime == TRUE)
            {
                bFirstTime = FALSE;
                oldestExplorerTime = creationtime;
                desiredExplorerPid = pid_buf[i];
                CloseHandle(hTmpProcess);
                continue;
            }
            if (CompareFileTime(&creationtime, &oldestExplorerTime) == -1)
            {
                oldestExplorerTime = creationtime;
                desiredExplorerPid = pid_buf[i];
            }
        }
        CloseHandle(hTmpProcess);
    }

    printf("Target pid %i\n", desiredExplorerPid);

    free(pid_buf);

    //winlogon.exe

    HANDLE hExplorerTask = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_VM_OPERATION, FALSE, desiredExplorerPid);
    if (!hExplorerTask)
    {
        printf("failed to open process handle\n");
        return 0;
    }

    LPVOID pPath = VirtualAllocEx(hExplorerTask, 0, strlen(name_dll) + 1, MEM_COMMIT, PAGE_READWRITE);
    if (!pPath)
    {
        printf("failed to allocate string in external process\n");
        return 0;
    }

    if (!WriteProcessMemory(hExplorerTask, pPath, (LPVOID)name_dll, strlen(name_dll) + 1, NULL))
    {
        printf("failed to write to region of external allocated memory\n");
        VirtualFreeEx(hExplorerTask, pPath, 0, MEM_RELEASE);
        return 0;
    }

    PVOID pLoadLibraryA = (PVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
    HANDLE hRemoteThread = CreateRemoteThread(hExplorerTask, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryA, pPath, 0, NULL);
    WaitForSingleObject(hRemoteThread, INFINITE);
    VirtualFreeEx(hExplorerTask, pPath, 0, MEM_RELEASE);

    printf("DLL successfully injected.\n");

    return 0;
}