SHOW_GBLSECTION_USERS, SYSTEM MANAGEMENT, Global Sections users.

This software accepts a global section name, eg LIBRTL_001 or SYSXQP_000 etc.
It then interrogates all processes on the system in order to find which 
processes have the given global section mapped. 

Note that it only runs on OpenVMS VAX and that it needs CMKRNL privilege.

Also note that this version will only work for system global sections.
To amend it to search for references to Delete-Pending global sections or
Group global sections, see the comments in the source code.

To generate a .EXE file, simply use MACRO and then LINK.
