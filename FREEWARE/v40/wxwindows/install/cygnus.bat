rem Cygnus Gnu-Win32 environment variables
rem Assumes that compiler and wxWindows are installed on the d: drive.
rem
set WXWIN=d:\wx
path C:\WINDOWS;C:\WINDOWS\COMMAND;d:\gnuwin32\b18\H-i386-cygwin32\bin;d:\gnuwin32\b18\H-i386-cygwin32\lib\gcc-lib\i386-cygwin32\cygnus-2.7.2-970404;c:\bin
set GCC_EXEC_PREFIX=D:\gnuwin32\b18\H-i386-cygwin32\lib\gcc-lib\
set RCINCLUDE=%WXWIN\include\msw:%WXWIN\contrib\fafa
set CPLUS_INCLUDE_PATH=/d/gnuwin32/b18/h-i386-cygwin32/i386-cygwin32/include:/d/gnuwin32/b18/include/g++:/d/gnuwin32/b18/H-i386-cygwin32/lib/gcc-lib/i386-cygwin32/cygnus-2.7.2-970404/include:/d/wx/include/msw:/d/wx/contrib/fafa:/d/gnuwin32/b18/include/g++
mount D: /d

rem 4DOS users only...
unalias make
alias makegnu make -f makefile.g95
