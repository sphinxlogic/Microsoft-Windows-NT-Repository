$ ! Procedure:	SYS_CALC_FREE_BLOCKS.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy_saved = f$verify(0)
$ if (f$type('__vfy') .nes. "") then __vfy_saved = f$verify(&__vfy)
$ set noon
$ symbol = f$edit(P1, "collapse,upcase")
$ device = f$edit(P2, "collapse,upcase")
$ max_symbol = f$edit(P3, "collapse,upcase")
$ if (symbol .eqs. "") then symbol = "FREE_BLOCKS"
$ if (device .eqs. "") then device = "SYS$DISK"
$ if (max_symbol .eqs. "") then max_symbol = "TOTAL_BLOCKS"
$ total_freeblocks = 0
$ total_blocks = 0
$ gosub calc_blocks
$ 'symbol' == total_freeblocks
$ 'max_symbol' == total_blocks
$ !'f$verify(__vfy_saved)'
$ exitt 1
$
$calc_blocks:
$	freeblocks = f$getdvi(device, "freeblocks")
$	maxblock = f$getdvi(device, "maxblock")
$	total_freeblocks = total_freeblocks + freeblocks
$	total_blocks = total_blocks + maxblock
$	device = f$getdvi(device, "nextdevnam")
$	if (device .nes. "") then gosub calc_blocks
$exit_calc_blocks:
$ return
$ !+==========================================================================
$ !
$ ! Procedure:	SYS_CALC_FREE_BLOCKS.COM
$ !
$ ! Purpose:	To return the number of free blocks on the specified device.
$ !
$ ! Parameters:
$ !	[P1]	symbol to contain the result, default is FREE_BLOCKS.
$ !	[P2]	disk to calculate the free space on, default is SYS$DISK.
$ !	[P3]	symbol to contain the total available space, default is
$ !		TOTAL_BLOCKS
$ !
$ ! History:
$ !		06-Jan-1992, DBS; Version V1-001
$ !  	001 -	Original version.
$ !		27-Aug-1992, DBS; Version V1-002
$ !	002 -	Added p3 for total available space.
$ !-==========================================================================
