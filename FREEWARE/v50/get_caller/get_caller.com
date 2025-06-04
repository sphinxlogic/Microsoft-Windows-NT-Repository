$ vfy = f$verify('debug'+0)
$!
$! PROCEDURE: GET_CALLER.COM
$!
$! ABSTRACT:
$!
$!    This code fragment obtains the name of the procedure which called this
$!    procedure.
$!
$!From: Ken Bosward <bosward.ken.kw@bhp.com.au>
$!Newsgroups: comp.os.vms
$!Subject: DCL Tricks
$!Date: Fri, 01 Nov 1996 12:35:58 +1000
$!Message-ID: <3279620E.3091@bhp.com.au>
$!
$! HISTORY:
$!
$!    15-MAY-1993  MDC  Written by Michael Clay
$!     1-Nov-1996  Ken Bosward, clean up temporary file
$!    06-Nov-1996  Dave Cantor, shorten report code using !AC
$!                      Added nesting level
$!    20-JUL-2000 Chris Sharman, Alpha-ize (without breaking VAX),
$!			add parms to suppress self (P1 boolean) &
$!			make global symbols get_caller__N (if p1 or p2 = S*),
$!			where N=0 is this proc, 1 is calling proc, and so on;
$!			ie inverse of depth: self-relative seemed more use.
$!
$!-----------------------------------------------------------------------------
$  fac =  f$parse(f$environment("procedure"),,,"name")
$  pid =  f$getjpi(0,"pid")
$  temp =  "sys$scratch:''fac'_''pid'.tmp"
$  on warning then goto FINISH
$  on control_y then goto FINISH
$!
$! Define known symbol values ...
$!
$  arch := 'f$getsyi("arch_name")'
$  vax = "!"
$  alpha = "!"
$  'arch' :=
$  'vax'ctl$ag_clidata =  %x7FFE2A5C   ! From SYS$SYSTEM:SYS.STB
$  'alpha'ctl$ag_clidata = %x7ffce260
$  ppd$l_prc      =  %x00000008   ! From SYS$SYSTEM:DCLDEF.STB
$  idf_l_lnk      =  %x00000000   ! From SYS$SYSTEM:DCLDEF.STB
$  idf_l_filename =  %x00000068   ! From SYS$SYSTEM:DCLDEF.STB
$!
$! Get value of PRC_L_IDFLNK from DCLDEF.STB (changes with version of VMS) ...
$!
$  call fetchsym_'arch' dcldef prc_l_idflnk
$!
$! Traverse IDF list
$!
$  ppd =  ctl$ag_clidata
$  prc =  f$cvui(0,32,f$fao("!AD",4,ppd+ppd$l_prc))
$  idf =  f$cvui(0,32,f$fao("!AD",4,prc+prc_l_idflnk))
$  lev =  f$environment("depth")
$  baselev = lev
$  makesym = (f$ext(0,1,p1).eqs."S").or.(f$ext(0,1,p2).eqs."S")
$  if p1 then $ goto tonext
$next_filename:
$     if idf .eq. 0 then goto FINISH
$     procname = f$fao("!AC",f$cvui(0,32,f$fao("!AD",4,idf+idf_l_filename)))
$     if makesym
$     then get_caller__'f$str(baselev-lev) == procname
$     else write sys$output f$fao(" !2UL -> !AS",lev,procname)
$     endif
$tonext:     idf =  f$cvui(0,32,f$fao("!AD",4,idf+idf_l_lnk))
$     lev = lev - 1
$     goto next_filename
$!
$fetchsym_alpha: subroutine ! file sym
$  search sys$loadable_images:'p1'.stb 'p2' /exa/form=passall/out='temp'
$! record too big for dcl
$  set file/attr=(rfm:fix,mrs:8,lrl:8) 'temp' ! Quadword aligned (+2)
$  sym = "   " + p2
$  sym[0,16] = 0
$  sym[16,8] = f$length(p2)
$  ver = 1
$  before = 3
$  after = (f$len(sym)-1)/8
$symloop: ! search for each quadword of the symbol in turn
$  sym8 = """"+f$ext(8*ver-8,8,sym)+""""
$  siz = f$len(sym8)-2
$  sea 'temp';'ver' &sym8 /key=(pos:1,siz:'siz')/exa/form=passall/win=('before','after')/out='temp'
$  ver = ver + 1
$  before = before + 1
$  after = after - 1
$  if after.ge.0 then $ goto symloop
$!
$  open/read temp 'temp'
$  read temp valquad
$  close temp
$  delete/nolog 'temp';*
$  'p2' ==  f$cvui(16,32,valquad)
$  endsubroutine
$!
$fetchsym_vax: subroutine ! file sym
$  search sys$system:'p1'.stb 'p2' /exact/format=passall /output='temp'
$  open/read temp 'temp'
$  read temp symbols
$  close temp
$  delete/nolog 'temp';*
$  loc_sym =  f$locate("''p2'",symbols)
$  'p2' ==  f$cvui((loc_sym-5)*8,32,symbols)
$finish:  endsubroutine
$!
$FINISH:
$ set noon
$ if f$type(prc_l_idflnk).nes."" then $ deletexx/sym/glo prc_l_idflnk
$ if f$trnlnm("TEMP").nes."" then close/nolog temp
$ if f$search(temp) .nes. "" then delete/nolog 'temp';*
