MBX, UTILITIES, Work with mailboxes from DCL without programming

MBX is a utility that allows to work with mailboxes from DCL.
It features a new DCL command that can create, delete, attach,
detach, write to, and read from mailboxes.

MBX also features a callable interface (a.k.a. API) so that
programmers can use the same mechanism from within their
programs, without having to write cumbersome system calls.
This API is available as an upward-compatible shareable
image. The kit contains a definition of the six service calls
in pascal environment format, and in SDL syntax.

The package contains the DCL command definition, the executables,
an addition for the VMS help library, and documentation.

The MBX utility is packaged as a PCSI kit or VMSINSTAL save-set 
for OpenVMS Alpha, and as a VMSINSTAL save-set only for OpenVMS
VAX. The file MBX.MEM contains the installation instructions.
It can be printed on any ascii printer.

Program sources (Pascal, Macro, SDL, CLD, and Message) are in
the SRC subdirectory. To build the executables, copy the contents
of the SRC subdirectory to an empty directory of your system, set
default to this directory, and execute the procedure BUILD.COM.
For a successful build, you will need a full VMS installation
(i.e. with the library and optional components installed), the
DEC Pascal compiler (another one most probably won't work) and
the SDL V3.2 package from the freeware CD.

The build has been tested under OpenVMS Alpha V6.2 and under
OpenVMS VAX V5.5-2, with DEC Pascal V5.6. Earlier versions
might work but have not been tested. To use the package, the
base VMS only is enough; earliest possible versions are 
OpenVMS Alpha V6.2 or  OpenVMS VAX V5.5-2. More recent versions
have not been tested but should be OK.

Constructive comments and suggestions about this package are
welcome at Marc.Vandyck@Skynet.Be .
