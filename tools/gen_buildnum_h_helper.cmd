@echo off
set PATH=C:\Program Files\Git\bin;C:\cygwin64\bin;C:\cygwin\bin;C:\msysgit\bin;C:\msysgit\mingw\bin;C:\perl64\bin;C:\perl\bin;%PATH%
cd "%1"
perl.exe "tools/gen_buildnum_h.pl" "%1/source/crisscross/build_number.h"
