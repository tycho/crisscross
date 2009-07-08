@echo off
set PATH=C:\cygwin\bin;%PATH%
cd "%1"
perl.exe "tools/gen_buildnum_h.pl" "%1/source/crisscross/build_number.h"
