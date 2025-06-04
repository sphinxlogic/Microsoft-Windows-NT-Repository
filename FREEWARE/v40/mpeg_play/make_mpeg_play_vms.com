$	verify = f$verify(1)
$!
$!===========================================================================
$! MAKE_MPEG_PLAY_VMS.COM - Command file to compile/link MPEG_PLAY for VMS
$!
$! Revision History:
$!  5-Feb-94    Max Calvani 
$! 11-Mar-93	Michael Kimura
$!	First version
$!===========================================================================
$!
$! Build MPEG_PLAY via DCL commands
$!
$! Compile the source files
$!
$ define X11 decw$include
$ define sys sys$library, SYS$COMMON:[DECW$INCLUDE.EXTENSIONS]
$ define XM decw$include
$ OPT = "/OPT"
$
$ !---------------------------------------------------------------------------
$ !
$	on error then goto end	! 'f$verify(0)'
$	set verify
$ !
$ !---------------------------------------------------------------------------
$ !
$ ! To just LINK do @xxx LINK
$ ! To build debug do @xxx "" "/DEBUG"
$ !
$ if P1 .nes. "" then goto 'P1'
$ if P2 .nes. "" then OPT = "/NOOPT"
$
$ compile :== CC/stand=vaxc/NoList/define=(SH_MEM)'P2''OPT'
$
$ del *.obj;*
$ del *.olb;*
$
$ compile util.c
$ compile video.c
$ compile parseblock.c
$ compile motionvector.c
$ compile decoders.c
$ compile main.c
$ compile gdith.c
$ compile fs2.c
$ compile fs2fast.c
$ compile fs4.c
$ compile hybrid.c
$ compile hybriderr.c
$ compile 2x2.c
$ compile gray.c
$ compile mono.c
$ compile jrevdct.c
$ compile 24bit.c
$ compile util32.c
$ compile ordered.c
$ compile ordered2.c
$ compile mb_ordered.c
$ compile info.c
$ compile ui.c
$
$ !
$ !  Create library and delete objects
$ !
$ lib/create mpglib.olb
$ lib/insert mpglib.olb *.obj
$ del *.obj;*
$
$! 
$! create mib library
$!
$ compile MIBBUTTON.C
$ compile MIBDRAWAREA.C    
$ compile MIBFRAME.C       
$ compile MIBLABEL.C       
$ compile MIBLIST.C        
$ compile MIBLOAD.C        
$ compile MIBMENU.C        
$ compile MIBRADIOBOX.C    
$ compile MIBSCALE.C       
$ compile MIBSCROLLBAR.C   
$ compile MIBTEXTBIG.C     
$ compile MIBTEXTBOX.C     
$ compile MIBTOGGLE.C      
$ lib/create miblib.olb
$ lib/insert miblib.olb *.obj
$ del *.obj;*
$
$ link:
$
$!
$! Link the files
$!
$	Link/notrace/exe=mpeg_play'P2' -
		mpglib/lib/inc=(util, -
				video, -
				parseblock, -
				motionvector, -
				decoders, -
				fs2, -
				fs2fast, -
				fs4, -
				hybrid, -
				hybriderr, -
				2x2, -
				gdith, -
				gray, -
				mono, -
				main, -
				jrevdct, -
				24bit, -
				util32, -
				ordered, -
				ordered2, -
				mb_ordered, -
				info, -
				ui), -
				mpeg_play.opt/options
$!
$	goto end	! 'f$verify(0)'
$!
$!---------------------------------------------------------------------------
$! Restore verification and exit
$!
$end:	exit ($status .or. %x10000000)+0*'f$verify(verify)'
