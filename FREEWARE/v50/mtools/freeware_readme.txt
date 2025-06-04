MTOOLS, UTILITIES, UNIX mtools utilities for OpenVMS Alpha

This is port of Unix mtools utilities for OpenVMS Alpha (I didn't test it
with OpenVMS VAX).

Now mtools has CLI interface for following commands (check MMD.CLD):
mcd, mdir, mdel, mread, mwrite, mcopy, mlabel, minfo, mattrib, mmd, mrd,
mmove, mren, mbadblocks, mformat

The old interface via symbols should work too:
copy mtools.exe as separate mXXXXX.exe files
msetup.com contains example of symbol definitions

Compilation:

If you have MMS installed on your machine, just type MMS to build
optimized/non-debug version of mtools and MMS/MACRO=(DEBUG=1) -
if you want to know how it works. ;)

For users without MMS there are two command procedures to build mtools:
COMP.COM and LINK.COM. You would need to modify first line of each
file to select proper build command.

I successfully built mtools on OpenVMS Alpha V7.1, DEC C V5.7-006
(included mtools.exe) and tested with floppy drive on AlphaStation 200/233.

By default, drive A: assigned to DVA0:. You can modify mtools.conf config
file to point to correct device.

VMS FOREVER!

Sergey Tikhonov
Solvo Ltd.
tsv@solvo.spb.su
