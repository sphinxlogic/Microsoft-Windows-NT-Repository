$	Define/Nolog	PBM$	[-.PBM]
$	Define/Nolog	PGM$	[-.PGM]
$	Define/Nolog	PPM$	[-.PPM]
$	Define/Nolog	C$INCLUDE	PBM$,PGM$,PPM$,[-]
$	Define/Nolog	DECC$USER_INCLUDE C$INCLUDE
$	Qual	:= "/DEFINE=(SYSV,PBM,PGM,PPM)"
$
$	If (P1 .NES. "") Then Goto 'P1
$
$	CC 'QUAL' 'P1' LIBPNM1
$	CC 'QUAL' 'P1' LIBPNM2
$	CC 'QUAL' 'P1' LIBPNM3
$	CC 'QUAL' 'P1' LIBPNM4
$	CC 'QUAL' 'P1' PNMARITH
$	CC 'QUAL' 'P1' PNMCAT
$	CC 'QUAL' 'P1' PNMCONVOL
$	CC 'QUAL' 'P1' PNMCROP
$	CC 'QUAL' 'P1' PNMCUT
$	CC 'QUAL' 'P1' PNMDEPTH
$	CC 'QUAL' 'P1' PNMENLARGE
$	CC 'QUAL' 'P1' PNMFILE
$	CC 'QUAL' 'P1' PNMFLIP
$	CC 'QUAL' 'P1' PNMGAMMA
$	CC 'QUAL' 'P1' PNMINVERT
$	CC 'QUAL' 'P1' PNMMERGE
$	CC 'QUAL' 'P1' PNMNORAW
$	CC 'QUAL' 'P1' PNMPASTE
$	CC 'QUAL' 'P1' PNMROTATE
$	CC 'QUAL' 'P1' PNMSCALE
$	CC 'QUAL' 'P1' PNMSHEAR
$	CC 'QUAL' 'P1' PNMTILE
$	CC 'QUAL' 'P1' PNMTOPS
$	CC 'QUAL' 'P1' PNMTORAST
$!!!!!	CC 'QUAL' 'P1' PNMTOTIFF
$	CC 'QUAL' 'P1' PNMTOXWD
$	CC 'QUAL' 'P1' RASTTOPNM
$!!!!!	CC 'QUAL' 'P1' TIFFTOPNM
$	CC 'QUAL' 'P1' XWDTOPNM
$
$	Lib/Create/Obj	LIBPNM.OLB	LIBPNM1.OBJ,LIBPNM2.OBJ,LIBPNM3.OBJ,-
					LIBPNM4.OBJ
$
$LINK:
$	Link_Libs	= ",LIBPNM/LIB"	+ ",PGM$:LIBPGM/LIB" + -
			   ",PBM$:LIBPBM/LIB"	+ ",PPM$:LIBPPM/LIB"
$
$	Link PNMARITH	'Link_Libs'
$	Link PNMCAT	'Link_Libs'
$	Link PNMCONVOL	'Link_Libs'
$	Link PNMCROP	'Link_Libs'
$	Link PNMCUT	'Link_Libs'
$	Link PNMDEPTH	'Link_Libs'
$	Link PNMENLARGE	'Link_Libs'
$	Link PNMFILE	'Link_Libs'
$	Link PNMFLIP	'Link_Libs'
$	Link PNMGAMMA	'Link_Libs'
$	Link PNMINVERT	'Link_Libs'
$! Cannot build MERGE as it references all others
$!	Link PNMMERGE	'Link_Libs'
$	Link PNMNORAW	'Link_Libs'
$	Link PNMPASTE	'Link_Libs'
$	Link PNMROTATE	'Link_Libs'
$	Link PNMSCALE	'Link_Libs'
$	Link PNMSHEAR	'Link_Libs'
$	Link PNMTILE	'Link_Libs'
$	Link PNMTOPS	'Link_Libs'
$	Link PNMTORAST	'Link_Libs'
$!!!!!	Link PNMTOTIFF	'Link_Libs'
$	Link PNMTOXWD	'Link_Libs'
$	Link RASTTOPNM	'Link_Libs'
$!!!!!	Link TIFFOTPNM	'Link_Libs'
$	Link XWDTOPNM	'Link_Libs'
$
$	Exit
