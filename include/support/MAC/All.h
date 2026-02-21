#pragma once

/**************************************************************************************************
Platform

One of the following platforms should be defined (either in code or as a project setting):
PLATFORM_WINDOWS
PLATFORM_APPLE
PLATFORM_LINUX
PLATFORM_ANDROID
**************************************************************************************************/
#if !defined(PLATFORM_WINDOWS) && !defined(PLATFORM_APPLE) && !defined(PLATFORM_LINUX) && !defined(PLATFORM_ANDROID)
    #pragma message("No platform set for MACLib, defaulting to Windows")
    #define PLATFORM_WINDOWS
#endif

#ifdef PLATFORM_ANDROID
#undef ASSERT
#undef ZeroMemory
#undef __forceinline
#endif

/**************************************************************************************************
Warnings
**************************************************************************************************/
#include "Warnings.h"

/**************************************************************************************************
Override (define for C++11)
**************************************************************************************************/
#if __cplusplus >= 201103L
#define APE_OVERRIDE override
#else
#define APE_OVERRIDE
#endif

/**************************************************************************************************
NULL (define to nullptr on all platforms)
**************************************************************************************************/
#if __cplusplus >= 201103L
#define APE_NULL nullptr
#else
#define APE_NULL 0
#endif

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
        #define NOMINMAX // remove the global min / max macros so APE_MIN / APE_MAX will always be used
    #endif
    #include "WindowsEnvironment.h"
    #define WIN32_LEAN_AND_MEAN
    #ifdef _MSC_VER
        #pragma warning(push) // push and pop warnings because the windows includes suppresses some like 4514
    #endif
    #include <Windows.h>
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
#define APE_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define APE_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define APE_CAP(value, low, high) (((value) < (low)) ? (low) : ((value) > (high)) ? (high) : (value))
#define APE_CLEAR(destination) memset(&destination, 0, sizeof(destination))
#define APE_CLEAR_ARRAY(destination, elements) memset(&destination[0], 0, sizeof(destination[0]) * static_cast<size_t>(elements))

/**************************************************************************************************
Version
**************************************************************************************************/
#include "Version.h"

// year in the copyright strings
#define APE_YEAR 2026

// used to make strings from numbers
#define APE_STRINGIZE2(s) #s
#define APE_STRINGIZE(s) APE_STRINGIZE2(s)

#define APE_WIDEN_(s) L##s
#define APE_WIDEN(s) APE_WIDEN_(s)
#define APE_STRINGIZE_WIDE(s) APE_WIDEN(APE_STRINGIZE(s))

// build the version strings
#define APE_VER_FILE_VERSION_STR                        APE_STRINGIZE(APE_VERSION_MAJOR) "." APE_STRINGIZE(APE_VERSION_REVISION)
#define APE_VER_FILE_VERSION_STR_WIDE                   APE_STRINGIZE_WIDE(APE_VERSION_MAJOR) L"." APE_STRINGIZE_WIDE(APE_VERSION_REVISION)
#define APE_FILE_VERSION_NUMBER                         3990

// names and copyrights
#define APE_NAME                                        L"Monkey's Audio " APE_VER_FILE_VERSION_STR_WIDE
#define APE_WINAMP_PLUGIN_NAME                          "Monkey's Audio Player " APE_VER_FILE_VERSION_STR
#define APE_MC_PLUGIN_NAME                              L"APE Plugin (v" APE_VER_FILE_VERSION_STR_WIDE L")"
#define APE_RESOURCE_VERSION_COMMA                      APE_VERSION_MAJOR, APE_VERSION_REVISION, 0, 0
#define APE_RESOURCE_VERSION_STRING                     APE_VER_FILE_VERSION_STR_WIDE
#define APE_RESOURCE_COPYRIGHT                          L"Copyright 2000-" APE_STRINGIZE_WIDE(APE_YEAR) L" Matthew T. Ashland"
#define CONSOLE_NAME                                    L"--- Monkey's Audio Console Front End (v " APE_VER_FILE_VERSION_STR_WIDE L") " APE_RESOURCE_COPYRIGHT L" ---\n"
#define PLUGIN_ABOUT                                    L"Monkey's Audio Player v" APE_VER_FILE_VERSION_STR_WIDE L"\n" APE_RESOURCE_COPYRIGHT

/**************************************************************************************************
Global compiler settings (useful for porting)
**************************************************************************************************/
// APE_BACKWARDS_COMPATIBILITY is only needed for decoding earlier than APE 3.99 files. It
// has not been possible to make these files for over 10 years, so it's unlikely
// that disabling APE_BACKWARDS_COMPATIBILITY would have any effect on a normal user. For
// porting or third party usage, it's probably best to not bother with APE_BACKWARDS_COMPATIBILITY.
#define APE_BACKWARDS_COMPATIBILITY

// disable this to turn off compression code
#define APE_SUPPORT_COMPRESS

// flip this to enable float compression
#define APE_SUPPORT_FLOAT_COMPRESSION

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
    typedef wchar_t                                     str_utfn; // could be UTF-16 or UTF-32 depending on platform
}

/**************************************************************************************************
Smart pointer
**************************************************************************************************/
#include "SmartPtr.h"

/**************************************************************************************************
Global macros
**************************************************************************************************/

// use to set the thread count automatically using the system hardware
#define APE_THREADS_AUTOMATIC -1

// formats
#define WAVE_FORMAT_PCM 1
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE
#define WAVE_FORMAT_ALAW 0x0006
#define WAVE_FORMAT_MULAW 0x0007

// we use this to check for zero because Clang warns if we just equate a double with zero
#define APE_DOUBLE_ZERO 0.00001

// we use more than the Windows system MAX_PATH since filenames can actually be longer
#define APE_MAX_PATH 8192

// undefined file size (pipe, etc.)
#define APE_FILE_SIZE_UNDEFINED -1

// make a pointer into an int64
#define APE_POINTER_TO_INT64(POINTER) static_cast<APE::int64>(reinterpret_cast<uintptr_t>(POINTER))

// platform defines
#if defined(PLATFORM_WINDOWS)
    #define IO_USE_WIN_FILE_IO
    #define DLLEXPORT                                   __declspec(dllexport)
    #define SLEEP(MILLISECONDS)                         ::Sleep(MILLISECONDS)
    #define APE_ODS                                     OutputDebugString
    #if !defined(ASSERT)
        #if defined(_DEBUG)
            #define ASSERT(e)                            assert(e)
        #else
            #define ASSERT(e)
        #endif
    #endif

    #if !defined(_MSC_VER) && _WIN32_WINNT < 0x600
        #define wcsncpy_s(A, B, C, D) wcsncpy(A, C, D)
        #define wcscpy_s(A, B, C) wcscpy(A, C)
        #define wcscat_s(A, B, C) wcscat(A, C)
        #define strcpy_s(A, B, C) strcpy(A, C)

        #undef _stprintf_s
        #define _stprintf_s(A, B, C, ...) _stprintf(A, C, __VA_ARGS__)

        #undef swprintf_s
        #define swprintf_s(A, B, C, ...) swprintf(A, C, __VA_ARGS__)
    #endif
#else
    #define IO_USE_STD_LIB_FILE_IO
    #define DLLEXPORT                                   __attribute__ ((visibility ("default")))
    #define SLEEP(MILLISECONDS)                         { struct timespec t; t.tv_sec = (MILLISECONDS) / 1000; t.tv_nsec = (MILLISECONDS) % 1000 * 1000000; nanosleep(&t, NULL); }
    #define APE_ODS                                     printf
    #undef  ASSERT
    #define ASSERT(e)

    #ifndef __STDC_LIB_EXT1__
        inline int wcsncpy_s(wchar_t * dest, size_t destsz, const wchar_t * src, size_t count)
        {
            if (!dest || !src || destsz == 0 || count == 0 || (count >= destsz && destsz <= wcslen(src)))
                return -1;
            wcsncpy(dest, src, count);
            return 0;
        }
        inline int wcscpy_s(wchar_t * dest, size_t destsz, const wchar_t * src)
        {
            if (!dest || !src || destsz == 0 || destsz <= wcslen(src))
                return -1;
            wcscpy(dest, src);
            return 0;
        }
        inline int wcscat_s(wchar_t * dest, size_t destsz, const wchar_t * src)
        {
            if (!dest || !src || destsz == 0 || destsz <= wcslen(dest) + wcslen(src))
                return -1;
            wcscat(dest, src);
            return 0;
        }
        inline int strcpy_s(char * dest, size_t destsz, const char * src)
        {
            if (!dest || !src || destsz == 0 || destsz <= strlen(src))
                return -1;
            strcpy(dest, src);
            return 0;
        }
    #endif
#endif

/**************************************************************************************************
WAVE format descriptor (binary compatible with Windows define, but in the APE namespace)
**************************************************************************************************/
namespace APE
{
    #pragma pack(push, 1)
    class WAVEFORMATEX
    {
    public:
        WORD        wFormatTag;         /* format type */
        WORD        nChannels;          /* number of channels (i.e. mono, stereo...) */
        uint32      nSamplesPerSec;     /* sample rate */
        uint32      nAvgBytesPerSec;    /* for buffer estimation */
        WORD        nBlockAlign;        /* block size of data */
        WORD        wBitsPerSample;     /* number of bits per sample of mono data */
        WORD        cbSize;             /* the count in bytes of the size of */
        /* extra information (after cbSize) */

        WAVEFORMATEX()
        {
            // initialize everything to empty on construction (avoids the need to clear variables separately)
            wFormatTag = 0;
            nChannels = 0;
            nSamplesPerSec = 0;
            nAvgBytesPerSec = 0;
            nBlockAlign = 0;
            wBitsPerSample = 0;
            cbSize = 0;
        }
    };
    #pragma pack(pop)
}

/**************************************************************************************************
Modes
**************************************************************************************************/
namespace APE
{
    enum APE_MODES
    {
        MODE_COMPRESS,
        MODE_DECOMPRESS,
        MODE_VERIFY,
        MODE_CONVERT,
        MODE_MAKE_APL,
        MODE_CHECK,
        MODE_COUNT
    };
}

/**************************************************************************************************
Global defines
**************************************************************************************************/
#ifdef PLATFORM_WINDOWS
    #define APE_FILENAME_SLASH '\\'
#else
    #define APE_FILENAME_SLASH '/'
#endif
#define APE_ONE_MILLION                  1000000
#define APE_BYTES_IN_KILOBYTE            1024
#define APE_BYTES_IN_MEGABYTE            1048576
#define APE_BYTES_IN_GIGABYTE            APE::int64(1073741824)

#define APE_WAV_HEADER_OR_FOOTER_MAXIMUM_BYTES (APE_BYTES_IN_MEGABYTE * 8)

/**************************************************************************************************
Byte order
**************************************************************************************************/
#define APE_LITTLE_ENDIAN     1234
#define APE_BIG_ENDIAN        4321

#if defined(_BIG_ENDIAN) || defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
#define APE_BYTE_ORDER        APE_BIG_ENDIAN
#else
#define APE_BYTE_ORDER        APE_LITTLE_ENDIAN
#endif

/**************************************************************************************************
Channels
**************************************************************************************************/
#define APE_MINIMUM_CHANNELS 1
#define APE_MAXIMUM_CHANNELS 32

/**************************************************************************************************
Macros
**************************************************************************************************/
#define APE_SAFE_DELETE(POINTER) if (POINTER) { delete POINTER; POINTER = APE_NULL; }
#define APE_SAFE_ARRAY_DELETE(POINTER) if (POINTER) { delete [] POINTER; POINTER = APE_NULL; }
#define APE_SAFE_FILE_CLOSE(HANDLE) if (HANDLE != INVALID_HANDLE_VALUE) { CloseHandle(HANDLE); HANDLE = INVALID_HANDLE_VALUE; }

#define APE_ODN(NUMBER) { TCHAR cNumber[16]; _stprintf(cNumber, _T("%d\n"), static_cast<int>(NUMBER)); APE_ODS(cNumber); }

#define APE_CATCH_ERRORS(CODE) try { CODE } catch(...) { }

#define RETURN_ON_ERROR(FUNCTION) { const int nFunctionResult = static_cast<int>(FUNCTION); if (nFunctionResult != ERROR_SUCCESS) { return nFunctionResult; } }
#define RETURN_VALUE_ON_ERROR(FUNCTION, VALUE) { int nFunctionResult = FUNCTION; if (nFunctionResult != 0) { return VALUE; } }
#define RETURN_ON_EXCEPTION(CODE, VALUE) { try { CODE } catch(...) { return VALUE; } }

#define THROW_ON_ERROR(CODE) { const intn nThrowResult = static_cast<intn> (CODE); if (nThrowResult != 0) throw(nThrowResult); }

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
#define ERROR_UAC_PERMISSION                            1016

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
