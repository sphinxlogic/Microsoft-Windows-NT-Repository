SAMBA 2.0.6 for OpenVMS ALPHA is ready here.  Enjoy.

I do not have the VAX version working.

The ALPHA version has been tested with OpenVMS Alpha 7.2.

I would expect it to work with other versions, but I have not tested it.

I do not know if this is faster than previous versions.  It has Oplocks and
Encrypted passwords.  Oplocks will probably slow things down, so if you are
having performance issues try turning them off.

There are several kits:

     samba_source_2_0_6.zip	- The UNIX source
				  encapsulated in a
				  ZIP archive.

     samba_vms_source_2_0_6_jem_1.zip

				- The VMS specific source.
				  This is the modules and
				  build routines.

     building_samba_vms.txt	- Instructions on how to build
				  SAMBA for VMS.

     installing_samba_vms_2_0_6.txt

				- Instructions on installing
				  SAMBA for VMS.

     samba_vms_base_2_0_6_jem_axp_1.zip

				- SAMBA for OpenVMS ALPHA kit.
				  UNZIP and install as directed.

     samba_vms_base_2_0_6_jem_vax_1.zip

				- SAMBA for OpenVMS VAX kit.
				  UNZIP and install as directed.


For both VAX and Alpha I am seeing that occasionally the NMBD process stops
responding, and attempting to kill it puts it into a RWMBX state.  I do not
know the reason.  The symptoms appear to be the same as mentioned about the
select() call in the dec-axpvms-vms721_acrtl-v0200--4 EDO kit.  I have not
tested SAMBA with that ECO kit installed.


SAMBA_VMS will require the FRONTPORT library, that can be
found in the [-.frontport] directory or "../frontport/" for browsers.

This software has mostly been tested on OpenVMS Alpha 7.2 and OpenVMS VAX
7.1.  I have received 1 report that it will not link properly on OpenVMS
Alpha 6.2.  I do not have the details of the problem.


One last minute change for the smb.conf file is to add
"case sensitive=yes" for the shares.  This causes the UNIX code that
emulates a case insensitive file system to not be executed, and provides
a slight performance increase.  OpenVMS will still treat the file name
as case insensitive.  This tip applies to earlier versions of SAMBA for
OpenVMS.


This software is provided as is with no warrantee.
