# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

# This line allows NMAKE to work as well

all: clist.exe perfexp.exe 

# Update the object files if necessary

clist.obj: clist.c
    $(cc) $(cflags) $(cvars) clist.c

clist.exe: clist.obj
    $(link) $(conflags) -out:clist.exe clist.obj $(guilibs) advapi32.lib

perfexp.obj: perfexp.c
    $(cc) $(cflags) $(cvars) perfexp.c

perfexp.exe: perfexp.obj
    $(link) $(conflags) -out:perfexp.exe perfexp.obj $(guilibs) advapi32.lib

