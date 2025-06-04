@echo off
Rem
Rem The SmallEnv gorp is for those who have too small environment
Rem size which will cause the variables we set to be truncated
Rem but who cannot be bothered to read the Out Of Environment Space
Rem error messages spitted by the stock DOS shell
set XSRC=.
if not "%XSRC%" == "." goto SmallEnv
if "%1" == "" goto inplace
set XSRC=%1
if not "%XSRC%" == "%1" goto SmallEnv
:inplace
test -d %XSRC%
if not errorlevel 1 goto chkdir
echo %XSRC% is not a directory
goto end
:chkdir
test -f %XSRC%/configure
if not errorlevel 1 goto argsok
echo I cannot find the configure script in directory %XSRC%
goto end
:argsok
rem set SYSROOT=c:
set PATH_SEPARATOR=:
if not "%PATH_SEPARATOR%" == ":" goto SmallEnv
set PATH_EXPAND=y
if not "%PATH_EXPAND%" == "y" goto SmallEnv
if not "%HOSTNAME%" == "" goto hostdone
if "%windir%" == "" goto msdos
set OS=MS-Windows
if not "%OS%" == "MS-Windows" goto SmallEnv
goto haveos
:msdos
set OS=MS-DOS
if not "%OS%" == "MS-DOS" goto SmallEnv
:haveos
if not "%USERNAME%" == "" goto haveuname
if not "%USER%" == "" goto haveuser
echo No USERNAME and no USER found in the environment, using default values
set HOSTNAME=Unknown PC
if not "%HOSTNAME%" == "Unknown PC" goto SmallEnv
:haveuser
set HOSTNAME=%USER%'s PC
if not "%HOSTNAME%" == "%USER%'s PC" goto SmallEnv
goto userdone
:haveuname
set HOSTNAME=%USERNAME%'s PC
if not "%HOSTNAME%" == "%USERNAME%'s PC" goto SmallEnv
:userdone
set HOSTNAME=%HOSTNAME%, %OS%
:hostdone
set OS=
Rem
Rem XDvi is not supported on MS-DOS
set CONFIG_SHELL=bash.exe
rem set INSTALL=${DJDIR}/bin/ginstall -c
set YACC=bison -y
set LEX=flex
set RANLIB=ranlib
set CC=gcc
if not "%RANLIB%" == "ranlib" goto SmallEnv
Rem Use a response file to avoid exceeding the 126-character limit
rem echo --prefix='${DJDIR}' --datadir='${DJDIR}'/share --srcdir=%XSRC% > cfg.rf
echo --prefix='${DJDIR}' --with-gzip --without-x > cfg.rf
echo Configuring...
sh ./configure i386-pc-msdos.djgppv2 @cfg.rf
echo Done.
goto CleanUp
:SmallEnv
echo Your environment size is too small.  Please enlarge it and run me again.
set HOSTNAME=
set OS=
:CleanUp
set XSRC=
set CONFIG_SHELL=
set INSTALL=
set YACC=
set LEX=
set RANLIB=
set CC=gcc
set HOSTNAME=
if exist cfg.rf del cfg.rf
:end
