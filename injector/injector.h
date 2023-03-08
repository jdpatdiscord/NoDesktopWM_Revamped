#ifndef NOWM_INJECTOR_H
#define NOWM_INJECTOR_H

#include <windows.h>
#include <psapi.h>
#include <shlwapi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
typedef struct _PROCESS_BASIC_INFORMATION {
    NTSTATUS ExitStatus;
    PVOID PebBaseAddress;
    ULONG_PTR AffinityMask;
    KPRIORITY BasePriority;
    ULONG_PTR UniqueProcessId;
    ULONG_PTR InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION;

typedef NTSTATUS (WINAPI* T_NtQuerySystemInformation)(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID pSystemInformation, ULONG SystemInformationLength, PULONG pLength);
typedef NTSTATUS (WINAPI* T_NtQueryInformationProcess)(HANDLE hProcess, int ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength);
T_NtQuerySystemInformation F_NtQuerySystemInformation = NULL;
T_NtQueryInformationProcess F_NtQueryInformationProcess = NULL;
*/

#endif