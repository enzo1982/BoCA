@echo off
set CDK_INSTALL_PATH=%CD%
set PATH=%CDK_INSTALL_PATH%\system64\usr\bin

mkdir %CDK_INSTALL_PATH%\system64\tmp 2> nul

set MSYSTEM=MINGW64
set BUILD_WIN32=True
set BUILD_X86_64=True

echo fre:ac Component Development Kit v1.1.5 (x64)
echo Copyright (C) 2001-2021 Robert Kausch

bash
