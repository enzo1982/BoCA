/**************************************************************************************************
Warnings
**************************************************************************************************/
#ifdef _MSC_VER
#pragma warning(disable: 4711) // informational only about inlining
#pragma warning(disable: 5039) // about calling a function that could throw but CompressFileW, DecompressFileW, etc. in APESimple.cpp do this so we need to keep this (7/17/2022); it's also fired by winbase.h and GdiplusGraphics.h (7/21/2022)
#pragma warning(disable: 5045) // this is about Spectre insertion, but we don't care
#pragma warning(disable: 4820) // this warns about padding after data when we don't use 1-byte alignment, but this is done for efficiency so is better
#pragma warning(disable: 4514) // this warns about an unreferrenced inline, but it's a complete non-issue (before we used a define to include the inline, but that made reading the code harder)
#endif
