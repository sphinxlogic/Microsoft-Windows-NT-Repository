XGAS, MOTIF_TOYS,  Animated simulation of an ideal gas


xgas is a physical simulation of an ideal gas in a heated box.
Gas molecules move around the box with velocities
dependent on their temperature.
A chamber consisting of two boxes contains the gas molecules;
the temperature of each box can be independently controlled
by a scrollbar.
When gas molecules collide with the walls, their temperature
approaches that of the box.
 
Use mouse button 1 to create molecules one at a time
at the cursor position.

Use mouse button 2 to create the maximum number of molecules
at the cursor position.

Copyright 1991, Massachusetts Institute of Technology.

Larry Medwin

XGas port under OpenVMS                     December 1995
=======================


XGas compiles out of the box under OpenVMS. However, you need DECUS X11R4 XAW &
XMU library kit available on your system (you can grab it at
ada.cenaath.cena.dgac.fr or ftp2.cnam.fr servers into [.DECWINDOWS] directory,
or find it at DECUS Web server www.decus.org).

Object and executables are also provided.

To rebuild, various COMPILE*.COM and LINK*.COM procedure are provided.

The resource file XGAS.DAT may be installed either under your SYS$LOGIN
directory or (for a system-wide availability) under
SYS$COMMON:[DECW$DEFAUKTS.USER] directory with World Read acces.

Enjoy !!

Patrick Moreau - CENA/Athis-Mons - FRANCE

pmoreau@cena.dgac.fr
moreau_p@decus.decus.fr
