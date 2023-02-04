#pragma once

#if !defined(PLATFORM_WINDOWS)

// we treat bool as a global type, so don't declare it in the namespace
#ifdef PLATFORM_APPLE
    #include <AvailabilityMacros.h>

    #ifndef OBJC_BOOL_DEFINED
        #if OBJC_BOOL_IS_BOOL
            typedef bool BOOL;
        #else
            typedef signed char BOOL; // this is the way it's defined in Obj-C
        #endif
    #endif
#else
    typedef unsigned char BOOL; // this is the way it's defined in X11
#endif

typedef wchar_t *           LPTSTR;
typedef const wchar_t *     LPCTSTR;
typedef wchar_t             TCHAR;

namespace APE
{
#undef __forceinline
#define __forceinline inline

#ifndef __stdcall
#define __stdcall
#endif

#define NEAR
#define FAR

typedef unsigned long       DWORD;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef void *              HANDLE;
typedef unsigned int        UINT;
typedef long                LRESULT;
typedef struct _GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
} GUID;

#undef ZeroMemory
#define ZeroMemory(POINTER, BYTES) memset(POINTER, 0, BYTES);

#define TRUE 1
#define FALSE 0

#define CALLBACK

#ifdef _T
#undef _T
#endif
#define _T(x) L ## x

#define _strnicmp strncasecmp
#define _wtoi(x) wcstol(x, NULL, 10)
#define _tcscat wcscat
#undef _totlower
#define _totlower towlower
#define _totupper towupper
#define _tcschr wcschr

#ifdef PLATFORM_APPLE
    #if MAC_OS_X_VERSION_MIN_REQUIRED >= 1070
        #define _tcsicmp wcscasecmp
    #else
        #define _tcsicmp wcscmp // fall back to case sensitive comparison on Mac OS X 10.6.x and earlier
    #endif
#else
    #define _tcsicmp wcscasecmp
#endif

#define _tcscpy wcscpy
#define _tcslen wcslen
#define _tcsncpy wcsncpy
#define _tcsstr wcsstr
#define _ftprintf fwprintf
#define _tcsnicmp _wcsnicmp
#define _tcscpy_s wcscpy_s
#define _tcsncpy_s wcsncpy_s
#define _ttoi _wtoi
#define _tcscmp wcscmp
#define strncpy_s(a, b, c, d) strncpy(a, c, d)
#define MAX_PATH    4096

}

#include <wctype.h>
#include <string.h>

#endif // #ifndef _WIN32
