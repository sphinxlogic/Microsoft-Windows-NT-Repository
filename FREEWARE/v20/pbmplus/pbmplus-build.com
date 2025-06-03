$	Set Verify
$
$	CC	:== CC/STANDARD=VAXC/Float=G_Float
$	Define	DECC$LIBRARY_INCLUDE SYS$LIBRARY
$
$!	Build VMS piece (it's added to LIBPBM.OLB in the PBM build portion)
$
$	CC Argproc
$
$! Build TIFF pieces
$
$	Set Def [.LIBTIFF]
$	Delete/Nolog *.OBJ;*,*.LIS;*,*.EXE;*,*.MAP;*,*.OLB;*
$	@Libtiff
$
$! Build bitmap pieces
$
$	Set Def [-.PBM]
$	Delete/Nolog *.OBJ;*,*.LIS;*,*.EXE;*,*.MAP;*,*.OLB;*
$	@PBMBuild
$
$! Build grayscale pieces
$
$	Set Def [-.PGM]
$	Delete/Nolog *.OBJ;*,*.LIS;*,*.EXE;*,*.MAP;*,*.OLB;*
$	@PGMBuild
$
$! Build full color pieces
$
$	Set Def [-.PPM]
$	Delete/Nolog *.OBJ;*,*.LIS;*,*.EXE;*,*.MAP;*,*.OLB;*
$	@PPMBuild
$
$! Build content independent pieces
$
$	Set Def [-.PNM]
$	Delete/Nolog *.OBJ;*,*.LIS;*,*.EXE;*,*.MAP;*,*.OLB;*
$	@PNMBuild
