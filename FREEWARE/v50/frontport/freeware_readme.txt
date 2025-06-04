FRONTPORT, SOFTWARE, Porting library for OpenVMS

The FRONTPORT library is a shared image that consists of
enhancements to the DEC C RTL to allow easier migration
of programs writen for the UNIX or LINUX operating systems.

It converts UNIX style file names into Pathwork's style
file names for ODS-2 volumes.

It is useful for programs that need fcntl() locking support
or for programs that need to have crypt() return a password
hash for a VMS username.

This library was developed to allow the building of SAMBA
for OpenVMS with as few patches to the source as possible.

Please read the FRONTPORT_DOCUMENTATION.TXT file for more
complete information.

The FRONTPORT library LINK procedure will not currently
work for versions of OpenVMS before 7.0 and has only been
tested on OpenVMS VAX 7.1 and OpenVMS Alpha 7.2.

By linking the FRONTPORT library with sys$share:decc$crtl.olb/lib
to produce the shared image, it may be possible to build it
with earlier versions.  The file sys$share:decc$crtl.olb is not
supplied with this kit.

This software is provided as is with no warrantee.
