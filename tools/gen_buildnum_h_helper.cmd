@echo off
set PATH=C:\cygwin\bin;%PATH%
perl.exe "%1/tools/gen_buildnum_h.pl" "%1/source/crisscross/build_number.h"
