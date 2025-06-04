SWITCH BASIC
! RSTS RT11 command file built on Mon Nov 14 16:28:55 1983
!
! ** Compile DTANSW
!
cc DTANSW.C
as DTANSW.S/d
!
! ** Compile DTCLOS
!
cc DTCLOS.C
as DTCLOS.S/d
!
! ** Compile DTCMD
!
cc DTCMD.C
as DTCMD.S/d
!
! ** Compile DTDCHA
!
cc DTDCHA.C
as DTDCHA.S/d
!
! ** Compile DTDCS
!
cc DTDCS.C
as DTDCS.S/d
!
! ** Compile DTDIAL
!
cc DTDIAL.C
as DTDIAL.S/d
!
! ** Compile DTDRAI
!
cc DTDRAI.C
as DTDRAI.S/d
!
! ** Compile DTDUMP
!
cc DTDUMP.C
as DTDUMP.S/d
!
! ** Compile DTEOL
!
cc DTEOL.C
as DTEOL.S/d
!
! ** Compile DTGESC
!
cc DTGESC.C
as DTGESC.S/d
!
! ** Compile DTGET
!
cc DTGET.C
as DTGET.S/d
!
! ** Compile DTHANG
!
cc DTHANG.C
as DTHANG.S/d
!
! ** Compile DTINIT
!
cc DTINIT.C
as DTINIT.S/d
!
! ** Compile DTINKE
!
cc DTINKE.C
as DTINKE.S/d
!
! ** Compile DTIOGE
!
cc DTIOGE.C
as DTIOGE.S/d
!
! ** Compile DTIOPU
!
cc DTIOPU.C
as DTIOPU.S/d
!
! ** Compile DTISKE
!
cc DTISKE.C
as DTISKE.S/d
!
! ** Compile DTISTI
!
cc DTISTI.C
as DTISTI.S/d
!
! ** Compile DTISVA
!
cc DTISVA.C
as DTISVA.S/d
!
! ** Compile DTKEYP
!
cc DTKEYP.C
as DTKEYP.S/d
!
! ** Compile DTMSG
!
cc DTMSG.C
as DTMSG.S/d
!
! ** Compile DTOFFH
!
cc DTOFFH.C
as DTOFFH.S/d
!
! ** Compile DTONHO
!
cc DTONHO.C
as DTONHO.S/d
!
! ** Compile DTOPEN
!
cc DTOPEN.C
as DTOPEN.S/d
!
! ** Compile DTPESC
!
cc DTPESC.C
as DTPESC.S/d
!
! ** Compile DTPHON
!
cc DTPHON.C
as DTPHON.S/d
!
! ** Compile DTPTES
!
cc DTPTES.C
as DTPTES.S/d
!
! ** Compile DTPUT
!
cc DTPUT.C
as DTPUT.S/d
!
! ** Compile DTREAD
!
cc DTREAD.C
as DTREAD.S/d
!
! ** Compile DTRESE
!
cc DTRESE.C
as DTRESE.S/d
!
! ** Compile DTSAVE
!
cc DTSAVE.C
as DTSAVE.S/d
!
! ** Compile DTST
!
cc DTST.C
as DTST.S/d
!
! ** Compile DTSYNC
!
cc DTSYNC.C
as DTSYNC.S/d
!
! ** Compile DTTALK
!
cc DTTALK.C
as DTTALK.S/d
!
! ** Compile DTTEST
!
cc DTTEST.C
as DTTEST.S/d
!
! ** Compile DTTIME
!
cc DTTIME.C
as DTTIME.S/d
!
! ** Compile DTTONE
!
cc DTTONE.C
as DTTONE.S/d
!
! ** Compile DTTRAP
!
cc DTTRAP.C
as DTTRAP.S/d
!
! ** Compile DTVISI
!
cc DTVISI.C
as DTVISI.S/d
RUN $LIBR
DTLIB.OBJ=//
DTANSW.OBJ,DTCLOS.OBJ,DTCMD.OBJ,DTDCHA.OBJ
DTDCS.OBJ,DTDIAL.OBJ,DTDRAI.OBJ,DTDUMP.OBJ
DTEOL.OBJ,DTGESC.OBJ,DTGET.OBJ,DTHANG.OBJ
DTINIT.OBJ,DTINKE.OBJ,DTIOGE.OBJ,DTIOPU.OBJ
DTISKE.OBJ,DTISTI.OBJ,DTISVA.OBJ,DTKEYP.OBJ
DTMSG.OBJ,DTOFFH.OBJ,DTONHO.OBJ,DTOPEN.OBJ
DTPESC.OBJ,DTPHON.OBJ,DTPTES.OBJ,DTPUT.OBJ
DTREAD.OBJ,DTRESE.OBJ,DTSAVE.OBJ,DTST.OBJ
DTSYNC.OBJ,DTTALK.OBJ,DTTEST.OBJ,DTTIME.OBJ
DTTONE.OBJ,DTTRAP.OBJ,DTVISI.OBJ//
^Z
pip DTANSW.OBJ/de
pip DTCLOS.OBJ/de
pip DTCMD.OBJ/de
pip DTDCHA.OBJ/de
pip DTDCS.OBJ/de
pip DTDIAL.OBJ/de
pip DTDRAI.OBJ/de
pip DTDUMP.OBJ/de
pip DTEOL.OBJ/de
pip DTGESC.OBJ/de
pip DTGET.OBJ/de
pip DTHANG.OBJ/de
pip DTINIT.OBJ/de
pip DTINKE.OBJ/de
pip DTIOGE.OBJ/de
pip DTIOPU.OBJ/de
pip DTISKE.OBJ/de
pip DTISTI.OBJ/de
pip DTISVA.OBJ/de
pip DTKEYP.OBJ/de
pip DTMSG.OBJ/de
pip DTOFFH.OBJ/de
pip DTONHO.OBJ/de
pip DTOPEN.OBJ/de
pip DTPESC.OBJ/de
pip DTPHON.OBJ/de
pip DTPTES.OBJ/de
pip DTPUT.OBJ/de
pip DTREAD.OBJ/de
pip DTRESE.OBJ/de
pip DTSAVE.OBJ/de
pip DTST.OBJ/de
pip DTSYNC.OBJ/de
pip DTTALK.OBJ/de
pip DTTEST.OBJ/de
pip DTTIME.OBJ/de
pip DTTONE.OBJ/de
pip DTTRAP.OBJ/de
pip DTVISI.OBJ/de
