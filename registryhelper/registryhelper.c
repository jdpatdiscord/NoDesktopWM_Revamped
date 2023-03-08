#include "registryhelper.h"

PVOID RH_HeapHandle;

PVOID RH_Malloc(DWORD dwSize)
{
    return malloc(dwSize);
}

void RH_Free(PVOID pAllocation)
{
    free(pAllocation);
}

DWORD RH_SelectPredefinedKey(LPCSTR szKeyPath, PDWORD pdwKey, DWORD* pdwOffset)
{
    // "HKEY_LOCAL_MACHINE\SOFTWARE\..."
    // "HKLM/HKU/HKCU/HKCR/HKCC\SOFTWARE\..."" for HKEY_LOCAL_MACHINE, HKEY_USERS, HKEY_CURRENT_USER, HKEY_CLASSES_ROOT, HKEY_CURRENT_CONFIG

    size_t dwKeyLen = strlen(szKeyPath);
    unsigned i = 0;
    if (!strncmp(szKeyPath, "HKU", 3) && (dwKeyLen == 3 || szKeyPath[3] == '\\')
        || !strncmp(szKeyPath, "HKEY_USERS", strlen("HKEY_USERS")))
        { *pdwOffset = 3; return HKEY_USERS; }
    if (!strncmp(szKeyPath, "HKLM", 4) && (dwKeyLen == 4 || szKeyPath[4] == '\\')
        || !strncmp(szKeyPath, "HKEY_LOCAL_MACHINE", strlen("HKEY_LOCAL_MACHINE")))
        { *pdwOffset = 4; return HKEY_LOCAL_MACHINE; }
    if (!strncmp(szKeyPath, "HKCU", 4) && (dwKeyLen == 4 || szKeyPath[4] == '\\')
        || !strncmp(szKeyPath, "HKEY_CURRENT_USER", strlen("HKEY_CURRENT_USER")))
        { *pdwOffset = 4; return HKEY_CURRENT_USER; }
    if (!strncmp(szKeyPath, "HKCC", 4) && (dwKeyLen == 4 || szKeyPath[4] == '\\')
        || !strncmp(szKeyPath, "HKEY_CURRENT_CONFIG", strlen("HKEY_CURRENT_CONFIG")))
        { *pdwOffset = 4; return HKEY_CURRENT_CONFIG; }
    if (!strncmp(szKeyPath, "HKCR", 4) && (dwKeyLen == 4 || szKeyPath[4] == '\\')
        || !strncmp(szKeyPath, "HKEY_CLASSES_ROOT", strlen("HKEY_CLASSES_ROOT")))
        { *pdwOffset = 4; return HKEY_CLASSES_ROOT; }
    return 0;
}

DWORD RH_CreateKey(LPCSTR szKeyPath)
{
    //unsigned subkeys = 0;
    //char* c = strchr(szKeyPath, '\\');
    //if (c == NULL)
    //    return RH_INVALID_INPUT;
    //while (c != NULL)
    //{
    //    ++subkeys;
    //    c = strchr(c + 1, '\\');
    //}
    char* str = (char*)szKeyPath;
    for (unsigned i = 0; i < strlen(szKeyPath); ++i)
    {
        if ( *(char*)(str + i) == '\\' )
        
    }
    return RH_SUCCESS;
};
DWORD RH_RemoveKey(LPCSTR szKeyPath)
{
    return RH_SUCCESS;
};
DWORD RH_KeyExists(LPCSTR szKeyPath, PBOOL pbExists)
{

    return RH_SUCCESS;
};

DWORD RH_GetKeysOnKey(LPCSTR szKeyPath, LPSTR** ppszKeys, PDWORD pdwKeyCount)
{
    return RH_SUCCESS;
};
DWORD RH_GetValuesOnKey(LPCSTR szKeyPath, RH_KeyValue* pValueArray, PDWORD pdwValueCount)
{
    return RH_SUCCESS;
};

DWORD RH_WriteDwordOnKey(LPCSTR szKeyPath, LPCSTR szValueName, DWORD Value)
{
    return RH_SUCCESS;
};
DWORD RH_WriteQwordOnKey(LPCSTR szKeyPath, LPCSTR szValueName, DWORD64 Value)
{
    return RH_SUCCESS;
};
DWORD RH_WriteStringOnKey(LPCSTR szKeyPath, LPCSTR szValueName, LPCSTR szValue)
{
    return RH_SUCCESS;
};
DWORD RH_WriteEnvironmentStringOnKey(LPCSTR szKeyPath, LPCSTR szValueName, LPCSTR szValue)
{
    return RH_SUCCESS;
};
DWORD RH_WriteStringTableOnKey(LPCSTR szKeyPath, LPCSTR szValueName, LPCSTR* pszValueTable, unsigned dwValueCount)
{
    return RH_SUCCESS;
};
DWORD RH_WriteBinaryDataOnKey(LPCSTR szKeyPath, LPCSTR szValueName, PVOID pValue, unsigned dwByteCount)
{
    return RH_SUCCESS;
};

DWORD RH_ReadDword(LPCSTR szKeyPath, PDWORD pdwValue)
{
    return RH_SUCCESS;
};
DWORD RH_ReadQword(LPCSTR szKeyPath, PDWORD64 pqwValue)
{
    return RH_SUCCESS;
};
DWORD RH_ReadString(LPCSTR szKeyPath, LPSTR* pszValue)
{
    return RH_SUCCESS;
};
DWORD RH_ReadEnvironmentString(LPCSTR szKeyPath, LPSTR* pszValue)
{
    return RH_SUCCESS;
};
DWORD RH_ReadStringTabl(LPCSTR szKeyPath, LPSTR** ppszStrings, PDWORD dwStringCount)
{
    return RH_SUCCESS;
};
DWORD RH_ReadBinaryData(LPCSTR szKeyPath, PVOID* ppData, PDWORD dwByteCount)
{
    return RH_SUCCESS;
};