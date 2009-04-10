@echo off
set PATH=C:\cygwin\bin;%PATH%
bash.exe "%1/tools/gen_buildnum_h.sh" "%1/source/crisscross/build_number.h"
