#include <windows.h>
#include <stdlib.h>
#include <string.h>

#define RH_SUCCESS 0
#define RH_ACCESS_DENIED 1
#define RH_DOESNT_EXIST 2
#define RH_INVALID_INPUT 3

// Folders within the registry are called a Key.
// The registry is within a format that is easily serialized. You can easily find where the registry is stored on disk.

// This library takes multiple types of paths.
//   : RH_KeyExists("Computer\\HKEY_LOCAL_MACHINE\\SOFTWARE");
//   : RH_KeyExists("HKEY_LOCAL_MACHINE\\SOFTWARE");
//   : RH_KeyExists("HKLM\\SOFTWARE");

#define RH_TYPE_DWORD 1
#define RH_TYPE_QWORD 2
#define RH_TYPE_STRING 3
#define RH_TYPE_ENVIRONMENTSTRING 4
#define RH_TYPE_STRINGTABLE 4
#define RH_TYPE_BINARY 6

typedef struct _RH_KeyValue
{
    DWORD dwValueType;
    LPCSTR szValueName;
} RH_KeyValue;

extern PVOID RH_HeapHandle;

DWORD RH_CreateKey(LPCSTR szKeyPath);
DWORD RH_RemoveKey(LPCSTR szKeyPath);
DWORD RH_KeyExists(LPCSTR szKeyPath, PBOOL pbExists);

DWORD RH_GetKeysOnKey(LPCSTR szKeyPath, LPSTR** ppszKeys, PDWORD pdwKeyCount);
DWORD RH_GetValuesOnKey(LPCSTR szKeyPath, RH_KeyValue* pValueArray, PDWORD pdwValueCount);

DWORD RH_WriteDwordOnKey             (LPCSTR szKeyPath, LPCSTR szValueName, DWORD Value);
DWORD RH_WriteQwordOnKey             (LPCSTR szKeyPath, LPCSTR szValueName, DWORD64 Value);
DWORD RH_WriteStringOnKey            (LPCSTR szKeyPath, LPCSTR szValueName, LPCSTR szValue);
DWORD RH_WriteEnvironmentStringOnKey (LPCSTR szKeyPath, LPCSTR szValueName, LPCSTR szValue);
DWORD RH_WriteStringTableOnKey       (LPCSTR szKeyPath, LPCSTR szValueName, LPCSTR* pszValueTable, unsigned dwValueCount);
DWORD RH_WriteBinaryDataOnKey        (LPCSTR szKeyPath, LPCSTR szValueName, PVOID pValue, unsigned dwByteCount);

DWORD RH_ReadDword             (LPCSTR szKeyPath, PDWORD pdwValue);
DWORD RH_ReadQword             (LPCSTR szKeyPath, PDWORD64 pqwValue);
DWORD RH_ReadString            (LPCSTR szKeyPath, LPSTR* pszValue);
DWORD RH_ReadEnvironmentString (LPCSTR szKeyPath, LPSTR* pszValue);
DWORD RH_ReadStringTable       (LPCSTR szKeyPath, LPSTR** ppszStrings, PDWORD dwStringCount);
DWORD RH_ReadBinaryData        (LPCSTR szKeyPath, PVOID* ppData, PDWORD dwByteCount);