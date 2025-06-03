# This file was generated automatically by configure.  Do not edit.
# Put configuration parameters in configparms instead.
config-machine = i386
config-vendor = unknown
config-os = sysv4
config-sysdirs = unix/sysv/sysv4/i386 unix/sysv/sysv4 unix/sysv/i386 unix/sysv unix/i386 unix i386 unix/common posix ieee754 unix/inet generic stub
prefix = /opt
config-defines =  -DHAVE__SYS_SIGLIST=1 -DHAVE___CTYPE=1
INSTALL = /opt/bin/install -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644
RANLIB = ranlib

stdarg.h = # The installed <stdarg.h> defines __gnuc_va_list.
stddef.h = # The installed <stddef.h> seems to be libc-friendly.
unix-generated :=  __mkdir.S __rmdir.S
