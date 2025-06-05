INFO-ZIP, UTILITIES, Zip V2.3 and UnZip V5.5 for OpenVMS

ZIP V2.3  -  Info-ZIP's portable Zip, version 2.3
UnZip V5.5 -  Info-ZIP's portable UnZip, version 5.5.

Zip is a utility to compress multiple files into a single .ZIP archive.
UnZip decompresses the files stored in a .ZIP archive, restoring them to
their original state.

Use the appropriate UNZIP executable to unzip the source archives.

    $ unzip :== disk$freeware60_1:[info-zip]unzip.alpha_exe
    $ unzip disk$freeware60_1:[info-zip]unz550xv.zip

These distributions include OpenVMS binaries for VAX and Alpha under 
the [.VMS-BINARIES] directories.  

To (re)produce the executable images from the supplied object code (for
use on other and particularly older OpenVMS versions), simply set default
to [.VMS-BINARIES] and issue the command @LINK

Packaged by Hunter Goatly.

