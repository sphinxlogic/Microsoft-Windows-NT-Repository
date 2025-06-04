MPACK/MUNPACK, Utilities, Files/Compression/Mail/News/Mime/ Mime / Base64 Encode/Decode

This is part of the MPACK 1.5 distribution: it's just the parts that
are needed for OpenVMS.

Nearly all of the code comes from Carnegie Mellon University
with no author listed; and some of the VMS specific additions
are by David Mathog.

I found that I got lots of errors when I tried to compile it
using DEC C 4.0 and OpenVMS 7.0, there were linker errors, and
the resulting executables didn't work.  I've added some
additional function prototypes and invocations of header files,
and in a few places conditionalized to use the RTL definitions
(for example, malloc) instead of their prototypes.  I didn't
change any executable code: I just added some references to keep
the compiler happy, and I use the GETOPT which is built into the
RTL instead of their local code. 

The programs now compile for me with only a couple of
informational warnings (which are annoying, but not serious),
and the executables work. 


Bart Z. Lederman

