#define MAX_PATH 260
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef int BOOL;
typedef void *HANDLE;
typedef char CHAR;
typedef const CHAR *LPCSTR;

typedef struct _FILETIME _FILETIME;
struct _FILETIME
{
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
};
typedef _FILETIME FILETIME;
typedef _FILETIME *PFILETIME;
typedef _FILETIME *LPFILETIME;

typedef struct _WIN32_FIND_DATAA _WIN32_FIND_DATAA;
struct _WIN32_FIND_DATAA
{
#define FILE_ATTRIBUTE_DIRECTORY 0x10
    DWORD    dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD    nFileSizeHigh;
    DWORD    nFileSizeLow;
    DWORD    dwReserved0;
    DWORD    dwReserved1;
    CHAR     cFileName[MAX_PATH];
    CHAR     cAlternateFileName[14];
    DWORD    dwFileType;
    DWORD    dwCreatorType;
    WORD     wFinderFlags;
};

typedef _WIN32_FIND_DATAA WIN32_FIND_DATAA;
typedef _WIN32_FIND_DATAA *PWIN32_FIND_DATAA;
typedef _WIN32_FIND_DATAA *LPWIN32_FIND_DATAA;

HANDLE FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);
BOOL FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);

#pragma comment(lib, "User32.lib")

DD_PRIVATE_FUNCTION_IMPL DD_b32
_DD_OS_IMPL_FileIter_Increment(DD_FileIter *it, DD_String8 path, DD_FileInfo *out_info)
{
    DD_b32 result = 0;
    
    WIN32_FIND_DATAA find_data = {0};
    HANDLE state = *(HANDLE *)(&it->state);
    if(state == 0)
    {
        DD_b32 need_star = 0;
        if(path.str[path.size-1] == '/' ||
           path.str[path.size-1] == '\\')
        {
            need_star = 1;
        }
        char *cpath = need_star ? DD_PushCStringF("%.*s*", DD_StringExpand(path)) : DD_CStringFromString8(path);
        state = FindFirstFileA(cpath, &find_data);
        result = !!state;
    }
    else
    {
        result = !!FindNextFileA(state, &find_data);
    }
    
    it->state = *(DD_u64 *)(&state);
    if(result)
    {
        out_info->flags = 0;
        if(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            out_info->flags |= DD_FileFlag_Directory;
        }
        out_info->path = DD_PushStringF("%s", find_data.cFileName);
        out_info->extension = DD_ExtensionString(out_info->path);
        out_info->file_size = ((((DD_u64)find_data.nFileSizeHigh) << 32) |
                               ((DD_u64)find_data.nFileSizeLow));
    }
    
    return result;
}