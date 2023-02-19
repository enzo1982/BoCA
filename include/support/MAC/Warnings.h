/**************************************************************************************************
Warnings
**************************************************************************************************/
#ifdef _MSC_VER
#pragma warning(disable: 4251) // missing DLL interface on CSmartPtr<> class
#pragma warning(disable: 4626) // implicitly deleted assignment operator because of const members
#pragma warning(disable: 4711) // informational only about inlining
#pragma warning(disable: 5039) // about calling a function that could throw but CompressFileW, DecompressFileW, etc. in APESimple.cpp do this so we need to keep this (7/17/2022); it's also fired by winbase.h and GdiplusGraphics.h (7/21/2022)
#pragma warning(disable: 5045) // this is about Spectre insertion, but we don't care
#pragma warning(disable: 26451) // we intentionally use integer (for legacy support reasons) when calculating the dot products in NNFilter
#endif
