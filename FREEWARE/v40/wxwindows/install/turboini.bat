@echo off
if "%1" == "" goto usage
echo Making a directory of include files for Turbo C++.
echo This makes the subdirectories %1\include\turboc.
echo CTRL-C if this is wrong.
pause

mkdir %1\include\turboc
copy %1\contrib\gauge\*.h %1\include\turboc
copy %1\contrib\fafa\*.h %1\include\turboc
copy %1\contrib\itsybits\*.h %1\include\turboc
copy %1\contrib\ctl3d\*.h %1\include\turboc
copy %1\utils\prologio\src\*.h %1\include\turboc
copy %1\utils\dib\*.h %1\include\turboc
copy %1\utils\rcparser\src\*.h %1\include\turboc

rem Avoid the need for YACC and LEX tools: pregenerated C files.
copy %1\utils\prologio\src\doslex.c %1\utils\prologio\src\lex_yy.c
copy %1\utils\prologio\src\dosyacc.c %1\utils\prologio\src\y_tab.c
copy %1\utils\prologio\src\doslex.c %1\include\turboc

echo Done.
goto end

:usage
echo turboini -- make a directory of include files for Turbo C++.
echo Usage: turboini DIRECTORY
echo e.g. turboini a: c:\wx

:end

