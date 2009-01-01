@echo off
set CDK_INSTALL_PATH=%CD%
set PATH=%CDK_INSTALL_PATH%\system64\bin
set C_INCLUDE_PATH=%CDK_INSTALL_PATH%/system64/include
set CPLUS_INCLUDE_PATH=%CDK_INSTALL_PATH%/system64/include
set LIBRARY_PATH=%CDK_INSTALL_PATH%/system64/lib

set BUILD_X64=True

echo BonkEnc Component Development Kit v1.1 Beta 1 (x64)
echo Copyright (C) 2001-2009 Robert Kausch

bash
