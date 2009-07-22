@echo off
set PATH=C:\cygwin\bin;C:\msysgit\bin;C:\msysgit\mingw\bin;%PATH%
cd "%1"
perl.exe "tools/gen_buildnum_h.pl" "%1/source/crisscross/build_number.h"
