$!
$! This command file sets up DCL symbols for commonly-used tools.
$!
$! This procedure determines which images are needed for the current
$! system architecture (VAX or Alpha), and sets up the symbols...
$!
$ Arch       = f$getsyi("ARCH_NAME")
$ Device     = f$parse(f$environment("PROCEDURE"),,,"DEVICE")
$ Direct     = f$parse(f$environment("PROCEDURE"),,,"DIRECTORY")
$ Direct     = Direct - "][" - "><" - ">" - "<" - "]" - "["
$ DevDir     = Device + "[" + Direct + "]"
$ DevDirArch = Device + "[" + Direct + "." + Arch + "_IMAGES]"
$ GsDoc      = Device + "[" + Direct + ".gs-6_01.Doc]"
$ GsLib      = Device + "[" + Direct + ".gs-6_01.Lib]"
$
$ BZIP       :== $'DevDirArch'BZIP2.exe        ! BZip2 (De)Compression Tool
$ D64        :== $'DevDirArch'DECODE_64.exe    ! decode base64 encoded file
$ GS         :== $'DevDirArch'GS.exe           ! Ghostscript
$ GV         :== $'DevDirArch'GV.exe           ! Ghostview
$ GZIP       :== $'DevDirArch'GZIP.exe         ! Gnu Zip
$ MXRN       :== $'DevDirArch'MXRN.exe         ! Motif XRN News Reader
$ XPDF       :== $'DevDirArch'XPDF.exe         ! X Windows PDF (Alpha!)
$ UNZI*P     :== $'DevDirArch'UNZIP.exe        ! Unzip Decompression Tool
$ UUDE*CODE  :== $'DevDirArch'UUDECODE.exe     ! UUdecode File Decoding Tool
$ UUEN*CODE  :== $'DevDirArch'UUENCODE.exe     ! UUencode File Encoding Tool
$ VMST*AR    :== $'DevDirArch'VMSTAR.exe       ! VMS Tar Utility
$ ZIP        :== $'DevDirArch'ZIP.exe          ! Zip Compression Tool
$
$ RESE*T     :== @'DevDir'RESET_BACKUP_SAVESET_FILE_ATTRIBUTES.COM
$
$ Define/NoLog/Job     GS_LIB     'gslib'
$ Define/NoLog/Job     GS_DOC     'gsdoc'
$
$ Exit
