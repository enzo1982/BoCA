@echo off
set CDK_INSTALL_PATH=%CD%
set PATH=%CDK_INSTALL_PATH%\system64\bin

mkdir %CDK_INSTALL_PATH%\system64\etc 2> nul

set BUILD_WIN32=True
set BUILD_X86_64=True

echo fre:ac Component Development Kit v1.1 RC 1 (x64)
echo Copyright (C) 2001-2020 Robert Kausch

bash
