GNU for POSIX, utilities, GNU tools for POSIX for OpenVMS

		    GNU tools for POSIX for OpenVMS
		    ===============================

This directory contains the following GNU utilities, each configured
for POSIX for OpenVMS:

	Utility		    File
	-------		    ----
	flex		    posix_flex.tar
	gawk		    posix_gawk.tar
	gmake		    posix_gmake.tar
	gzip		    posix_gzip.tar


For each utility, the tar file should be unpacked within a POSIX container 
filesystem using the command,

psx> tar -xvf posix_gmake.tar

This will create a directory hierarchy under a directory called

POSIX_gmake-3.72.1

Since several of the files do not conform to VMS naming ocnventions (too
many dots) and the lack of these files rpevents the kit from building,
this kit _must_ be unpacked within a container.

To build the executable you should run make within this directory. 
For example,

psx> cd gmake-3.72.1
psx> make

You may see make or compiler warning and informational messages, but I
believe these are benign.

NB ** Do not run configure. **  

These kits have been pre-configured for POSIX for OpenVMS.  This is because 
the configure script gets confused by differences in behaviour between what 
GNU expects and what POSIX provides.  The script will incorrectly guess
what functions/facilities/headers POSIX provides, and set up the kit in
such a way that it will not build.

If you have run configure, the best way forward is to delete the kit and 
untar it again.
