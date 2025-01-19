@echo off
set CDK_INSTALL_PATH=%CD%
set PATH=%CDK_INSTALL_PATH%\system-arm64\usr\bin

mkdir %CDK_INSTALL_PATH%\system-arm64\tmp 2> nul

set MSYSTEM=MINGW64
set CMAKE_GENERATOR=MSYS Makefiles
set BUILD_WIN32=True
set BUILD_ARM64=True

set CC=aarch64-w64-mingw32-clang
set CXX=aarch64-w64-mingw32-clang++

echo fre:ac Component Development Kit v1.1.7 (arm64)
echo Copyright (C) 2001-2025 Robert Kausch

bash
