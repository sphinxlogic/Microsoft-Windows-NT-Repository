dnl -*- sh -*-
dnl Here are some global variables that need initialized.

AC_DEFUN(JD_INIT,
[
#These variable are initialized by JD init function
JD_This_Dir=`pwd`
# Note: these will differ if one is a symbolic link
if test -f /usr/bin/dirname; then
  JD_Above_Dir=`dirname $JD_This_Dir`
else
# system is a loser
  JD_Above_Dir=`cd ..;pwd`
fi
JD_Above_Dir2=`cd ..;pwd`
])

dnl-------------------------------------------------------------------------

AC_DEFUN(JD_SET_OBJ_SRC_DIR,
[
#---------------------------------------------------------------------------
# Set the source directory and object directory.   The makefile assumes an
# abcolute path name.  This is bacause src/Makefile cds to OBJDIR and compiles
# the src file which is in SRCDIR
#---------------------------------------------------------------------------
SRCDIR=$JD_This_Dir
if test "$1" != "."
then
  if test -z "$1"
  then
    SRCDIR=$SRCDIR/src
  else
    SRCDIR=$SRCDIR/$1
  fi
fi

OBJDIR=$SRCDIR/"$ARCH"objs
ELFDIR=$SRCDIR/"$ARCH"elfobjs
AC_SUBST(SRCDIR)dnl
AC_SUBST(OBJDIR)dnl
AC_SUBST(ELFDIR)dnl
])

dnl-------------------------------------------------------------------------

AC_DEFUN(JD_FIND_GENERIC,
[
changequote(<<, >>)dnl
define(<<JD_UP_NAME>>, translit($1, [a-z], [A-Z]))dnl
changequote([, ])dnl
# Look for the JD_UP_NAME package
#JD_UP_NAME[]_INCLUDE=""
#JD_UP_NAME[]_LIB_DIR=""

# This list consists of "include,lib include,lib ..."
JD_Search_Dirs="$JD_Above_Dir2/$1/src,$JD_Above_Dir2/$1/src/"$ARCH"objs \
                $JD_Above_Dir/$1/src,$JD_Above_Dir/$1/src/"$ARCH"objs \
		$HOME/include,$HOME/lib"

if test -n "$ARCH" 
then
 Slang_Search_Dirs="$JD_Search_Dirs $HOME/include,$HOME/$ARCH/lib"
 Slang_Search_Dirs="$JD_Search_Dirs $HOME/include,$HOME/sys/$ARCH/lib"
fi

# Now add the standard system includes.  The reason for doing this is that 
# the other directories may have a better chance of containing a more recent
# version.

JD_Search_Dirs="$JD_Search_Dirs \
                /usr/local/include,/usr/local/lib \
		/usr/include,/usr/lib \
		/usr/include/$1,/usr/lib \
		/usr/include/$1,/usr/lib/$1"

echo looking for the JD_UP_NAME library

for include_and_lib in $JD_Search_Dirs
do
  # Yuk.  Is there a better way to set these variables??
  generic_include=`echo $include_and_lib | tr ',' ' ' | awk '{print [$]1}'`
  generic_lib=`echo $include_and_lib | tr ',' ' ' | awk '{print [$]2}'`
  echo Looking for $1.h in $generic_include
  echo and lib$1.a in $generic_lib
  if test -r $generic_include/$1.h && test -r $generic_lib/lib$1.a
  then
    echo Found it.
    JD_UP_NAME[]_LIB_DIR="$generic_lib"
    JD_UP_NAME[]_INCLUDE="$generic_include"
    break
  else
    if test -r $generic_include/$1.h && test -r $generic_lib/lib$1.so
    then
      echo Found it.
      JD_UP_NAME[]_LIB_DIR="$generic_lib"
      JD_UP_NAME[]_INCLUDE="$generic_include"
      break
    fi
  fi
done

if test -z "[$]JD_UP_NAME[]_LIB_DIR"
then
    echo Unable to find the $JD_UP_NAME library.  
    echo You may have to edit $JD_This_Dir/src/Makefile.
    JD_UP_NAME[]_INCLUDE=$JD_Above_Dir/$1/src
    JD_UP_NAME[]_LIB_DIR=$JD_Above_Dir/$1/src/"$ARCH"objs
fi

AC_SUBST(JD_UP_NAME[]_LIB_DIR)dnl
AC_SUBST(JD_UP_NAME[]_INCLUDE)dnl
undefine([JD_UP_NAME])dnl
])

dnl-------------------------------------------------------------------------

AC_DEFUN(JD_FIND_SLANG,
[
JD_FIND_GENERIC(slang)
])

dnl-------------------------------------------------------------------------
AC_DEFUN(JD_GCC_WARNINGS,
[
AC_ARG_ENABLE(warnings,
	      [  --enable-warnings       turn on GCC compiler warnings],
	      [gcc_warnings=$enableval])
if test -n "$GCC"
then
  CFLAGS="$CFLAGS -fno-strength-reduce"
  if test -n "$gcc_warnings"
  then
    CFLAGS="$CFLAGS -Wall \
            -Wpointer-arith -Wcast-align -Wcast-qual \
	    -Wtraditional -Wstrict-prototypes \
	    -Wshadow \
	    -Wconversion \
	    -Waggregate-return \
	    -Wmissing-prototypes \
	    -Wnested-externs"
    # Now trim excess whitespace
    CFLAGS=`echo $CFLAGS`
  fi
fi
])

dnl-------------------------------------------------------------------------
AC_DEFUN(JD_CREATE_ORULE,
[
PROGRAM_OBJECT_RULES="$PROGRAM_OBJECT_RULES
\$(OBJDIR)/$1.o : \$(SRCDIR)/$1.c
	cd \$(OBJDIR); \$(COMPILE_CMD) \$(SRCDIR)/$1.c
"
])
dnl-------------------------------------------------------------------------
AC_DEFUN(JD_CREATE_ELFORULE,
[
PROGRAM_ELF_ORULES="$PROGRAM_ELF_ORULES
\$(ELFDIR)/$1.o : \$(SRCDIR)/$1.c
	cd \$(ELFDIR); \$(ELFCOMPILE_CMD) \$(SRCDIR)/$1.c
"
])

dnl-------------------------------------------------------------------------
AC_DEFUN(JD_CREATE_EXEC_RULE, 
[  
PROGRAM_OBJECT_RULES="$PROGRAM_OBJECT_RULES
$1 : \$(OBJDIR)/$1
	@echo $1 created in \$(OBJDIR)
\$(OBJDIR)/$1 : \$(OBJDIR)/$1.o \$(EXECDEPS)
	\$(CC) -o \$(OBJDIR)/$1 \$(OBJDIR)/$1.o \$(EXECLIBS)
\$(OBJDIR)/$1.o : \$(SRCDIR)/$1.c
	cd \$(OBJDIR); \$(COMPILE_CMD) \$(EXECINC) \$(SRCDIR)/$1.c
"
])
dnl-------------------------------------------------------------------------

AC_DEFUN(JD_GET_MODULES,
[
 Program_Modules=`cat $1`
 PROGRAM_OFILES=""
 PROGRAM_OBJECTS=""
 PROGRAM_ELFOBJECTS=""
 PROGRAM_OBJECT_RULES=""
 PROGRAM_ELF_ORULES=""
 for program_module in $Program_Modules
 do
   PROGRAM_OFILES="$PROGRAM_OFILES $program_module.o"
   PROGRAM_OBJECTS="$PROGRAM_OBJECTS \$(OBJDIR)/$program_module.o"
   PROGRAM_ELFOBJECTS="$PROGRAM_ELFOBJECTS \$(ELFDIR)/$program_module.o"
   JD_CREATE_ORULE($program_module)
   JD_CREATE_ELFORULE($program_module)
 done
 AC_SUBST(PROGRAM_OFILES)dnl
 AC_SUBST(PROGRAM_OBJECTS)dnl
 AC_SUBST(PROGRAM_ELFOBJECTS)dnl
])

dnl-------------------------------------------------------------------------
AC_DEFUN(JD_APPEND_RULES,
[ 
 echo "$PROGRAM_OBJECT_RULES" >> $1
])
dnl-------------------------------------------------------------------------

AC_DEFUN(JD_APPEND_ELFRULES,
[ 
 echo "$PROGRAM_ELF_ORULES" >> $1
])

dnl-------------------------------------------------------------------------
AC_DEFUN(JD_CREATE_MODULE_EXEC_RULES,
[
 for program_module in $Program_Modules
 do
   JD_CREATE_EXEC_RULE($program_module)
 done
])
dnl-------------------------------------------------------------------------

AC_DEFUN(JD_TERMCAP,
[
JD_Terminfo_Dirs="/usr/lib/terminfo \
                 /usr/share/lib/terminfo \
		 /usr/local/lib/terminfo"

TERMCAP=-ltermcap

AC_MSG_CHECKING(for Terminfo)
for terminfo_dir in $JD_Terminfo_Dirs
do
   if test -d $terminfo_dir 
   then
      AC_MSG_RESULT(yes)
      TERMCAP=""
      break
   fi
done
if test "$TERMCAP"; then
  AC_MSG_RESULT(no)
  AC_DEFINE(USE_TERMCAP)
fi
AC_SUBST(TERMCAP)dnl
])

dnl-------------------------------------------------------------------------
AC_DEFUN(JD_ANSI_CC,
[
AC_PROG_CC
AC_PROG_GCC_TRADITIONAL
AC_ISC_POSIX
AC_AIX

dnl #This stuff came from Yorick config script
dnl
dnl # HPUX needs special stuff
dnl
AC_EGREP_CPP(yes,
[#ifdef hpux
  yes
#endif
], [
AC_DEFINE(_HPUX_SOURCE)
if test "$CC" = cc; then CC="cc -Aa"; fi
])dnl
dnl
dnl #Be sure we've found compiler that understands prototypes
dnl
AC_MSG_CHECKING(C compiler that understands ANSI prototypes)
AC_TRY_COMPILE([ ],[
 extern int silly (int);], [
 AC_MSG_RESULT($CC looks ok.  Good.)], [
 AC_MSG_RESULT($CC is not a good enough compiler)
 AC_MSG_ERROR(Set env variable CC to your ANSI compiler and rerun configure.)
 ])dnl
])dnl
dnl #----------------------------------------------------------------------


