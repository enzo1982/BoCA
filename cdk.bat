@echo off
set CDK_INSTALL_PATH=%CD%
set PATH=%CDK_INSTALL_PATH%\system\bin

mkdir %CDK_INSTALL_PATH%\system\etc 2> nul

set BUILD_WIN32=True
set BUILD_X86=True

echo fre:ac Component Development Kit v1.1 Beta 1
echo Copyright (C) 2001-2019 Robert Kausch

bash
