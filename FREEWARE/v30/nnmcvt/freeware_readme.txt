NNMCVT, UTILITIES, Export VMS mail in a format that can be read by NMAIL

Nnmcvt is a self-contained utility to export VMS mail in a format that can be
read directly by Nnmail.  It has the following features and restrictions:

It processes only the default mail file.  If you don't know what this means,
you probably won't notice a problem.

It processes all mail messages in all mail folders in a single execution.  One
consequence is that you must have enough working disk space to create all the
converted files.

It places the converted mail in subdirectories of your current default
directory.  Each mail message is in a separate file.  The files are named
MSGnnnn.MAI, where 'nnnn' is an arbitrary sequence number.

It does not disturb your VMS Mail in any way, except to read it.

For example, if you had mail in folders A, B, and MAIL, you would get a number
of .MAI files in [.A], [.B], and [.MAIL].  You then copy the files in these
directories to your Windows NT system.  One way to do this is by mounting your
VMS directory as a share using Pathworks and then selecting and dragging the
files to subdirectories of your Nnmail storage directories.  When you are
satisfied that the result is correct, you can delete the files from the VMS
system.

The utility is a VAX/VMS image linked on V5.5-2 named NNMCVT.EXE.  To execute
it, copy it to your system and RUN it.

Comments and problem reports will be received at jack@JAMMER.enet.dec.com.
However, this utility is not a work project and must receive a corresponding 
level of attention.

- Marty Jack 11-May-1995
