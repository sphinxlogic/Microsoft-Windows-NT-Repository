REGEDIT, Utilities, A program to look at or modify the registry database

Regedit is a program to look at or modify the registryserver database.
It is a full screen (SMG) program and looks like the (MS-)Windows regedit.
If you use the DecWindows environment, you can use mouse-clicks, 
otherwise you must use keyboard-keys.
Because the registry-server exists only on alpha, there is not VAX version.

Usage:

Run regedit

Once in the program you can use PF2/HELP for help info
F10 or ^z will return you to DCL. If you have a Decwindows/VXT display, use
can use the mouse, otherwise you must use the keyboard.

Building
The Alpha executable is in the kit, but if you want to rebuild REGEDIT,
go to the regedit directory and use @create_regedit


Contents:

Ths package contains the following files
In the home directory

 REGEDIT.EXE		The executable
 CREATE_REGEDIT.COM	The command file to compile/link
 FREEWARE_README.TXT    This file

In the [.SRC] directory
 REGEDIT.FOR		The Fortran source
 REGEDIT.INC		The include file

In the [.ALPHA] directory
 REGEDIT.OBJ		The object

There is no VAX equivalent, since the registry server only runs on Alpha.

Instructions:

Unpack the save set.
If you want to rebuild:
 If you have a fortran compiler
  @CREATE_REGEDIT
 Else to link only
  @CREATE_REGEDIT LINK

Author : Fekko Stubbe
