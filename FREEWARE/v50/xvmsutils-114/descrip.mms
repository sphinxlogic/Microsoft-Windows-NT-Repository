#!-----------------------descrip.mms for Xvmsutils------------------------------
cc_defs = 
c_deb =

.ifdef __DECC__
pref = /DECC/prefix=all
.endif

OBJS=CHOOSE_WINDOW.OBJ,\
CREATE_DIR.OBJ,\
FATALERR.OBJ,\
GETHOSTNAME.OBJ,\
GETLOGIN.OBJ,\
HPWD.OBJ,\
LNM.OBJ,\
LOWERCASE.OBJ,\
PRINT_EVENT.OBJ,\
PSEUDO_ROOT.OBJ,\
STRDUP.OBJ,\
STRCASECMP.OBJ,\
TIMER.OBJ,\
TZSET.OBJ,\
USLEEP.OBJ,\
VALIDATE.OBJ,\
VMS_STAT.OBJ,\
strings.obj

.ifdef __UCX__
OBJ1=ioctl.obj
.else
OBJ1=
.endif

.ifdef __OLDX__
OBJ2=xmisc.obj
.else
OBJ2=
.endif

.ifdef __DECC__
.ifdef __VV7__
OBJ3=
.else
OBJ3=DIRECTORY.OBJ,\
GETPWNAM.OBJ,\
GETPWUID.OBJ,\
popen.obj,\
putenv.obj,\
random.obj,\
rand48.obj,\
UNAME.OBJ,\
UNLINK.OBJ,\
unix_times.obj
.ifdef __VV62__
.else
OBJ3 = $(OBJ3),getopt.obj
.endif
.ifdef __OLDC__
OBJ3=$(OBJ3),tempnam.obj
.endif
.endif
.else
OBJ3=DIRECTORY.OBJ,\
getopt.obj,\
GETPWNAM.OBJ,\
GETPWUID.OBJ,\
popen.obj,\
putenv.obj,\
random.obj,\
tempnam.obj,\
UNAME.OBJ,\
UNLINK.OBJ,\
unix_times.obj
.endif

CFLAGS = $(C_DEB) $(CC_DEFS) $(PREF)

xvmsutils : xvmsutils.olb($(OBJS) $(OBJ1) $(OBJ2) $(OBJ3))
            @ write sys$output "Xvmsutils available"
            
clean :  
            delete *.obj;*,xvmsutils.olb;*            
          
#          
# Other dependencies
#          
CHOOSE_WINDOW.OBJ : vmsutil.h
DIRECTORY.OBJ     : dirent.h sys_dirent.h
GETOPT.OBJ        : vmsutil.h
GETPWNAM.OBJ      : pwd.h
GETPWUID.OBJ      : pwd.h
TEMPNAM.OBJ       : unix_types.h
UNAME.OBJ         : utsname.h
UNIX_TIMES.OBJ    : unix_time.h
VALIDATE.OBJ      : vmsutil.h pwd.h
VMS_STAT.OBJ      : unix_types.h
XMISC.OBJ         : copyright.h Xlibint.h
.ifdef __ALPHA__
strings.obj       : strings.mar arch_defs.mar
                    macro/migration/obj=strings.obj $(mopts) arch_defs+strings.mar
.else
strings.obj       : strings.mar 
.endif
