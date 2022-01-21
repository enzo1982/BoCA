@echo off
set CDK_INSTALL_PATH=%CD%
set PATH=%CDK_INSTALL_PATH%\system-i686\usr\bin

mkdir %CDK_INSTALL_PATH%\system-i686\tmp 2> nul

set MSYSTEM=MINGW32
set BUILD_WIN32=True
set BUILD_X86=True

echo fre:ac Component Development Kit v1.1.5 (i686)
echo Copyright (C) 2001-2022 Robert Kausch

bash
