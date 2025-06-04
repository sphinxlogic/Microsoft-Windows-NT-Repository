INFO-ZIP, UTILITIES, Zip V2.3 and UnZip V5.42 for OpenVMS

ZIP V2.3  -  Info-ZIP's portable Zip, version 2.3
UnZip V5.42  -  Info-ZIP's portable UnZip, version 5.42.

    Previous Revisions Also Included

Zip is a utility to compress multiple files into a single .ZIP archive.
UnZip decompresses the files stored in a .ZIP archive, restoring them to
their original state.

Use the appropriate UNZIP executable to unzip the source archives.

    $ unzip :== $dka400:[info-zip]unzip.alpha_exe
    $ unzip dka400:[info-zip]unzip542

These distributions include VMS binaries for VAX and Alpha under the
[.VMS-BINARIES] directories.  To produce .EXE files from the supplied
binaries, simply set default to that directory and do:  @LINK

VMS binaries supplied by Hunter Goatley

VAX_VAXC_* compiled with VAX C V3.2
VAX_DECC_* compiled with DEC C V6.0
AXP_* compiled with DEC C V6.2
