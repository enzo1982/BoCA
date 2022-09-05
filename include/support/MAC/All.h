#pragma once

/**************************************************************************************************
Platform
 
One of the following platforms should be defined (either in code or as a project setting):
PLATFORM_WINDOWS
PLATFORM_APPLE
PLATFORM_LINUX
**************************************************************************************************/
#if !defined(PLATFORM_WINDOWS) && !defined(PLATFORM_APPLE) && !defined(PLATFORM_LINUX)
    #pragma message("No platform set for MACLib, defaulting to Windows")
    #define PLATFORM_WINDOWS
#endif

#ifdef PLATFORM_ANDROID
#undef MBN
#undef ODN
#undef ODS
#undef CATCH_ERRORS
#undef ASSERT
#undef _totlower
#undef ZeroMemory
#undef __forceinline
#endif

/**************************************************************************************************
64-bit
**************************************************************************************************/
#if defined(_WIN64) || defined(__x86_64__)
#define APE_64_BIT
#endif

/**************************************************************************************************
Warnings
**************************************************************************************************/
#include "Warnings.h"

/**************************************************************************************************
Global includes
**************************************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <math.h>

#if defined(PLATFORM_WINDOWS)
    #ifndef NOMINMAX
        #define NOMINMAX // remove the global min / max macros so ape_min / ape_max will always be used
    #endif
    #include "WindowsEnvironment.h"
    #define WIN32_LEAN_AND_MEAN
    #ifdef _MSC_VER
        #pragma warning(push) // push and pop warnings because the windows includes suppresses some like 4514
    #endif
    #include <windows.h>
    #ifdef _MSC_VER
        #pragma warning(pop)
    #endif
    #include <tchar.h>
    #include <assert.h>
#else
    #include <unistd.h>
    #include <time.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <wchar.h>
    #include "NoWindows.h"
#endif
#define ape_max(a,b)    (((a) > (b)) ? (a) : (b))
#define ape_min(a,b)    (((a) < (b)) ? (a) : (b))
#define CLEAR(destination) memset(&destination, 0, sizeof (destination))

/**************************************************************************************************
Packing

We need to pack to the next byte or else we get warning 4820.  We could also get around the
warning by adding padding to all our structures that is unused, but this isn't as elegant.  The
actual packing code is in each header and CPP file because doing it globally leads to compiler
warnings on Linux.
**************************************************************************************************/

/**************************************************************************************************
Smart pointer
**************************************************************************************************/
#include "SmartPtr.h"

/**************************************************************************************************
Version
**************************************************************************************************/
#include "Version.h"

// year in the copyright strings
#define MAC_YEAR 2022

// build the version string
#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
#define MAC_VER_FILE_VERSION_STR                        STRINGIZE(MAC_VERSION_MAJOR) _T(".") STRINGIZE(MAC_VERSION_REVISION) 
#define MAC_VER_FILE_VERSION_STR_NARROW                 STRINGIZE(MAC_VERSION_MAJOR) "." STRINGIZE(MAC_VERSION_REVISION) 

#define MAC_FILE_VERSION_NUMBER                         3990
#define MAC_VERSION_STRING                              MAC_VER_FILE_VERSION_STR
#define MAC_VERSION_NUMBER                              MAC_VERSION_MAJOR MAC_VERSION_REVISION
#define MAC_NAME                                        _T("Monkey's Audio ") MAC_VER_FILE_VERSION_STR
#define PLUGIN_NAME                                     "Monkey's Audio Player " MAC_VER_FILE_VERSION_STR_NARROW
#define MJ_PLUGIN_NAME                                  _T("APE Plugin (v") MAC_VER_FILE_VERSION_STR _T(")")
#define MAC_RESOURCE_VERSION_COMMA                      MAC_VERSION_MAJOR, MAC_VERSION_REVISION, 0, 0
#define MAC_RESOURCE_VERSION_STRING                     MAC_VER_FILE_VERSION_STR
#define MAC_RESOURCE_COPYRIGHT                          "Copyright (c) 2000-" STRINGIZE(MAC_YEAR) " Matthew T. Ashland"
#define CONSOLE_NAME                                    _T("--- Monkey's Audio Console Front End (v ") MAC_VER_FILE_VERSION_STR _T(") (c) Matthew T. Ashland ---\n")
#define PLUGIN_ABOUT                                    _T("Monkey's Audio Player v") MAC_VER_FILE_VERSION_STR _T("\nCopyrighted (c) 2000-") STRINGIZE(MAC_YEAR) _T(" by Matthew T. Ashland")

/**************************************************************************************************
Global compiler settings (useful for porting)
**************************************************************************************************/
// assembly code (helps performance, but limits portability)
#if defined __SSE2__
    #define ENABLE_SSE_ASSEMBLY
#endif

#if defined __AVX2__
    #define ENABLE_AVX_ASSEMBLY
#endif

#ifdef _MSC_VER // doesn't compile in gcc
    #if defined(_M_IX86) || defined(_M_X64)
        #define ENABLE_SSE_ASSEMBLY
        #define ENABLE_AVX_ASSEMBLY
    #endif
#endif

// APE_BACKWARDS_COMPATIBILITY is only needed for decoding APE 3.92 or earlier files.  It
// has not been possible to make these files for over 10 years, so it's unlikely
// that disabling APE_BACKWARDS_COMPATIBILITY would have any effect on a normal user.  For
// porting or third party usage, it's probably best to not bother with APE_BACKWARDS_COMPATIBILITY.
// A future release of Monkey's Audio itself may remove support for these obsolete files.
#if !defined(PLATFORM_ANDROID)
    #define APE_BACKWARDS_COMPATIBILITY
#endif

// disable this to turn off compression code
#define APE_SUPPORT_COMPRESS

// compression modes
#define ENABLE_COMPRESSION_MODE_FAST
#define ENABLE_COMPRESSION_MODE_NORMAL
#define ENABLE_COMPRESSION_MODE_HIGH
#define ENABLE_COMPRESSION_MODE_EXTRA_HIGH

/**************************************************************************************************
Global types
**************************************************************************************************/
namespace APE
{
    // integer types
    typedef uint64_t                                    uint64;
    typedef uint32_t                                    uint32;
    typedef uint16_t                                    uint16;
    typedef uint8_t                                     uint8;
    
    typedef int64_t                                     int64;
    typedef int32_t                                     int32;
    typedef int16_t                                     int16;
    typedef int8_t                                      int8;

    typedef intptr_t                                    intn; // native integer, can safely hold a pointer
    typedef uintptr_t                                   uintn;

    // string types
    typedef char                                        str_ansi;
    typedef unsigned char                               str_utf8;
    typedef int16                                       str_utf16;
    typedef wchar_t                                     str_utfn; // could be UTF-16 or UTF-32 depending on platform
}

/**************************************************************************************************
Global macros
**************************************************************************************************/
#define WAVE_FORMAT_PCM 1
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE
#define WAVE_FORMAT_ALAW 0x0006
#define WAVE_FORMAT_MULAW 0x0007

#define APE_TRUNCATE ((size_t)-1)

#define POINTER_TO_INT64(POINTER) (int64)(uintptr_t)POINTER

#if defined(PLATFORM_WINDOWS)
    #define IO_USE_WIN_FILE_IO
    #define DLLEXPORT                                   __declspec(dllexport)
    #define SLEEP(MILLISECONDS)                         ::Sleep(MILLISECONDS)
    #define MESSAGEBOX(PARENT, TEXT, CAPTION, TYPE)     ::MessageBox(PARENT, TEXT, CAPTION, TYPE)
    #define PUMP_MESSAGE_LOOP                           { MSG Msg; while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE) != 0) { TranslateMessage(&Msg); DispatchMessage(&Msg); } }
    #define ODS                                         OutputDebugString
    #define TICK_COUNT_TYPE                             unsigned long long
    #define TICK_COUNT_READ(VARIABLE)                   VARIABLE = GetTickCount64()
    #define TICK_COUNT_FREQ                             1000

    #if !defined(ASSERT)
        #if defined(_DEBUG)
            #define ASSERT(e)                            assert(e)
        #else
            #define ASSERT(e)                            
        #endif
    #endif
#else
    #define IO_USE_STD_LIB_FILE_IO
    #define DLLEXPORT
    #define SLEEP(MILLISECONDS)                         { struct timespec t; t.tv_sec = (MILLISECONDS) / 1000; t.tv_nsec = (MILLISECONDS) % 1000 * 1000000; nanosleep(&t, NULL); }
    #define MESSAGEBOX(PARENT, TEXT, CAPTION, TYPE)
    #define PUMP_MESSAGE_LOOP
    #undef  ODS
    #define ODS                                         printf
    #define TICK_COUNT_TYPE                             unsigned long long
    #define TICK_COUNT_READ(VARIABLE)                   { struct timeval t; gettimeofday(&t, NULL); VARIABLE = t.tv_sec * 1000000LLU + t.tv_usec; }
    #define TICK_COUNT_FREQ                             1000000
    #undef  ASSERT
    #define ASSERT(e)
    #define wcsncpy_s(A, B, C, D) wcsncpy(A, C, D)
    inline wchar_t * wcscpy_s(wchar_t * dest, size_t destsz, const wchar_t * src)
    {
        (void) destsz; // this avoids an unreferenced variable
        return wcscpy(dest, src);
    }
    #define wcscat_s(A, B, C) wcscat(A, C)
    #define sprintf_s(A, B, C, D) sprintf(A, C, D)
    #define strcpy_s(A, B, C) strcpy(A, C)
    #define _tcscat_s(A, B, C) _tcscat(A, C)
#endif

/**************************************************************************************************
WAVE format descriptor (binary compatible with Windows define, but in the APE namespace)
**************************************************************************************************/
namespace APE
{
    #pragma pack(push, 1)
    typedef struct tWAVEFORMATEX
    {
        tWAVEFORMATEX() :
            wFormatTag(0),
            nChannels(0),
            nSamplesPerSec(0),
            nAvgBytesPerSec(0),
            nBlockAlign(0),
            wBitsPerSample(0),
            cbSize(0)
        {
        }
        WORD        wFormatTag;         /* format type */
        WORD        nChannels;          /* number of channels (i.e. mono, stereo...) */
        uint32      nSamplesPerSec;     /* sample rate */
        uint32      nAvgBytesPerSec;    /* for buffer estimation */
        WORD        nBlockAlign;        /* block size of data */
        WORD        wBitsPerSample;     /* number of bits per sample of mono data */
        WORD        cbSize;             /* the count in bytes of the size of */
        /* extra information (after cbSize) */
    } WAVEFORMATEX;
    #pragma pack(pop)
}

/**************************************************************************************************
Modes
**************************************************************************************************/
namespace APE
{
    enum MAC_MODES
    {
        MODE_COMPRESS,
        MODE_DECOMPRESS,
        MODE_VERIFY,
        MODE_CONVERT,
        MODE_MAKE_APL,
        MODE_CHECK,
        MODE_COUNT,
    };
}

/**************************************************************************************************
Global defines
**************************************************************************************************/
#define ONE_MILLION                  1000000
#ifdef PLATFORM_WINDOWS
    #define APE_FILENAME_SLASH '\\'
#else
    #define APE_FILENAME_SLASH '/'
#endif
#define BYTES_IN_KILOBYTE            1024
#define BYTES_IN_MEGABYTE            (1024 * BYTES_IN_KILOBYTE)
#define BYTES_IN_GIGABYTE            int64(1024 * BYTES_IN_MEGABYTE)

/**************************************************************************************************
Byte order
**************************************************************************************************/
#define APE_LITTLE_ENDIAN     1234
#define APE_BIG_ENDIAN        4321
#define APE_BYTE_ORDER        APE_LITTLE_ENDIAN

/**************************************************************************************************
Channels
**************************************************************************************************/
#define APE_MINIMUM_CHANNELS 1
#define APE_MAXIMUM_CHANNELS 32

/**************************************************************************************************
Macros
**************************************************************************************************/
#define MB(TEST) MESSAGEBOX(NULL, TEST, _T("Information"), MB_OK);
#define MBN(NUMBER) { TCHAR cNumber[16]; _stprintf(cNumber, _T("%d"), NUMBER); MESSAGEBOX(NULL, cNumber, _T("Information"), MB_OK); }

#define SAFE_DELETE(POINTER) if (POINTER) { delete POINTER; POINTER = NULL; }
#define SAFE_ARRAY_DELETE(POINTER) if (POINTER) { delete [] POINTER; POINTER = NULL; }
#define SAFE_VOID_CLASS_DELETE(POINTER, Class) { Class *pClass = (Class *) POINTER; if (pClass) { delete pClass; POINTER = NULL; } }
#define SAFE_FILE_CLOSE(HANDLE) if (HANDLE != INVALID_HANDLE_VALUE) { CloseHandle(HANDLE); HANDLE = INVALID_HANDLE_VALUE; }

#define ODN(NUMBER) { TCHAR cNumber[16]; _stprintf(cNumber, _T("%d\n"), int(NUMBER)); ODS(cNumber); }

#define CATCH_ERRORS(CODE) try { CODE } catch(...) { }

#define RETURN_ON_ERROR(FUNCTION) {    int nFunctionResult = FUNCTION; if (nFunctionResult != 0) { return nFunctionResult; } }
#define RETURN_VALUE_ON_ERROR(FUNCTION, VALUE) { int nFunctionResult = FUNCTION; if (nFunctionResult != 0) { return VALUE; } }
#define RETURN_ON_EXCEPTION(CODE, VALUE) { try { CODE } catch(...) { return VALUE; } }

#define THROW_ON_ERROR(CODE) { intn nThrowResult = (intn) CODE; if (nThrowResult != 0) throw(nThrowResult); }

#define EXPAND_8_TIMES(CODE) CODE CODE CODE CODE CODE CODE CODE CODE
#define EXPAND_9_TIMES(CODE) CODE CODE CODE CODE CODE CODE CODE CODE CODE
#define EXPAND_16_TIMES(CODE) CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE
#define EXPAND_32_TIMES(CODE) CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE CODE

/**************************************************************************************************
Error Codes
**************************************************************************************************/

// success
#undef ERROR_SUCCESS
#define ERROR_SUCCESS                                   0

// file and i/o errors (1000's)
#define ERROR_IO_READ                                   1000
#define ERROR_IO_WRITE                                  1001
#define ERROR_INVALID_INPUT_FILE                        1002
#define ERROR_INVALID_OUTPUT_FILE                       1003
#define ERROR_INPUT_FILE_TOO_LARGE                      1004
#define ERROR_INPUT_FILE_UNSUPPORTED_BIT_DEPTH          1005
#define ERROR_INPUT_FILE_UNSUPPORTED_SAMPLE_RATE        1006
#define ERROR_INPUT_FILE_UNSUPPORTED_CHANNEL_COUNT      1007
#define ERROR_INPUT_FILE_TOO_SMALL                      1008
#define ERROR_INVALID_CHECKSUM                          1009
#define ERROR_DECOMPRESSING_FRAME                       1010
#define ERROR_INITIALIZING_UNMAC                        1011
#define ERROR_INVALID_FUNCTION_PARAMETER                1012
#define ERROR_UNSUPPORTED_FILE_TYPE                     1013
#define ERROR_UNSUPPORTED_FILE_VERSION                  1014
#define ERROR_OPENING_FILE_IN_USE                       1015

// memory errors (2000's)
#define ERROR_INSUFFICIENT_MEMORY                       2000

// dll errors (3000's)
#define ERROR_LOADING_APE_DLL                           3000
#define ERROR_LOADING_APE_INFO_DLL                      3001
#define ERROR_LOADING_UNMAC_DLL                         3002

// general and misc errors
#define ERROR_USER_STOPPED_PROCESSING                   4000
#define ERROR_SKIPPED                                   4001

// programmer errors
#define ERROR_BAD_PARAMETER                             5000

// IAPECompress errors
#define ERROR_APE_COMPRESS_TOO_MUCH_DATA                6000

// unknown error
#define ERROR_UNDEFINED                                -1
