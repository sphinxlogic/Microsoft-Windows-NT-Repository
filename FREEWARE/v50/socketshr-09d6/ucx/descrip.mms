!++
!	Build file for SOCKETSHR_UCX.EXE
!	V1.1			24-Oct-1994	IfN/Mey
!--
PACK		= UCX
PACKOBJS	= socket_ucx.obj, ucx_ioctl.obj, ntoh_hton.obj, inet.obj, \
			trnlnm.obj, readdb.obj

.INCLUDE	[-]socketshr
