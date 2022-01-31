@echo off
cd "%1"
py.exe "tools/gen_buildnum_h.py" "%1/source/crisscross/build_number.h"
if %errorlevel% equ 0 exit /b 0
perl.exe "tools/gen_buildnum_h.pl" "%1/source/crisscross/build_number.h"
if %errorlevel% equ 0 exit /b 0
sh tools/gen_buildnum_h.sh "%1/source/crisscross/build_number.h"
