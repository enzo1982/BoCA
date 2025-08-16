#pragma once

/**************************************************************************************************
Warnings
**************************************************************************************************/
// only do the warnings for MSVC
#ifdef _MSC_VER

// compiler warnings
#pragma warning(disable: 4068) // outputs about unknown pragma, but they might be known by Clang but not the compiler
#pragma warning(disable: 4514) // this warns about an unreferrenced inline, but it's a complete non-issue (before we used a define to include the inline, but that made reading the code harder)
#pragma warning(disable: 4710) // function not inlined that happens all the time from fwprintf, etc. in the Console project
#pragma warning(disable: 4711) // informational only about inlining
#pragma warning(disable: 4820) // this warns about padding after data when we don't use 1-byte alignment, but this is done for efficiency so is better
#pragma warning(disable: 5039) // about calling a function that could throw but CompressFileW, DecompressFileW, etc. in APESimple.cpp do this so we need to keep this (7/17/2022); it's also fired by winbase.h and GdiplusGraphics.h (7/21/2022)
#pragma warning(disable: 5045) // this is about Spectre insertion, but we don't care

// Clang warnings
#pragma clang diagnostic ignored "-Wc++98-compat" // older compiler support is not needed
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic" // older compiler support is not needed (pedantic!)
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage" // unsafe buffer usage gets flagged all over and seems totally unhelpful
#pragma clang diagnostic ignored "-Wswitch-default" // this wants default in all the switches, but then it warns about having a default when all the cases are covered
#pragma clang diagnostic ignored "-Wexit-time-destructors" // this shows for global variables which are needed sometimes
#pragma clang diagnostic ignored "-Wglobal-constructors" // some variables like the application object need to be global

#endif // _MSC_VER
