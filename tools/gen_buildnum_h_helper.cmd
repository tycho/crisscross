@echo off
set PATH=C:\cygwin\bin;%PATH%
cd "%1"
bash.exe tools/gen_buildnum_h.sh source/crisscross/build_number.h