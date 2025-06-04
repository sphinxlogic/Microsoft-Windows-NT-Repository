GBLSEC Users, SYSTEM_MANAGEMENT, Show Global Sections users

This software accepts a global section name, eg LIBRTL_001 or SECURESHR_001 etc.
It then interrogates all processes on the system in order to find which 
processes have the given global section mapped. 

OpenVMS VAX source file name is SHOW_GBLSECTION_USERS_VAX,MAR and the
corresponding one for OpenVMS AXP is called SHOW_GBLSECTION_USERS_AXP.MAR. 

Note that this version will only work for system global sections. To amend it
to search for references to Delete-Pending global sections or Group global
sections, please see the comments in the source code. 


To generate a .EXE file on OpenVMS VAX, simply use MACRO and then LINK.

To generate a .EXE file on OpenVMS AXP, simply use MACRO and then LINK/SYSEXE.
