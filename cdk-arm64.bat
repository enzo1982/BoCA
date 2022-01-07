@echo off
set CDK_INSTALL_PATH=%CD%
set PATH=%CDK_INSTALL_PATH%\system-arm64\usr\bin

mkdir %CDK_INSTALL_PATH%\system-arm64\tmp 2> nul

set MSYSTEM=MINGW64
set BUILD_WIN32=True
set BUILD_ARM64=True

echo fre:ac Component Development Kit v1.1.5 (arm64)
echo Copyright (C) 2001-2022 Robert Kausch

bash
