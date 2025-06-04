$! create a bigggg disk
$!
$!  Volset             STA           DSA           LDA           Label
$!---------------------------------------------------------------------
$!                                                 LDA1
$!                                   DSA10                       ST0
$!                                                 LDA2
$!                     STA20
$!                                                 LDA3
$!                                   DSA11                       ST1
$!                                                 LDA4
$!  VOLSET
$!                                                 LDA5
$!                                   DSA12                       ST2
$!                                                 LDA6
$!                     STA21
$!                                                 LDA7
$!                                   DSA13                       ST3
$!                                                 LDA8
$!
$ if f$getsyi("alloclass") .eqs. 0
$ then
$   write sys$output "An Allocation calls needs to be defined!"
$   exit
$ endif
$ set proc/priv=all
$ size = 4096
$ ld create/log/nobackup/size='size' $8$dia0:[000000]ld1.dsk
$ ld create/log/nobackup/size='size' $8$dia0:[000000]ld2.dsk
$ ld create/log/nobackup/size='size' $8$dia0:[000000]ld3.dsk
$ ld create/log/nobackup/size='size' $8$dia0:[000000]ld4.dsk
$ ld create/log/nobackup/size='size' $8$dia0:[000000]ld5.dsk
$ ld create/log/nobackup/size='size' $8$dia0:[000000]ld6.dsk
$ ld create/log/nobackup/size='size' $8$dia0:[000000]ld7.dsk
$ ld create/log/nobackup/size='size' $8$dia0:[000000]ld8.dsk
$ ld conn/log $8$dia0:[000000]ld1.dsk lda1
$ ld conn/log $8$dia0:[000000]ld2.dsk lda2
$ ld conn/log $8$dia0:[000000]ld3.dsk lda3
$ ld conn/log $8$dia0:[000000]ld4.dsk lda4
$ ld conn/log $8$dia0:[000000]ld5.dsk lda5
$ ld conn/log $8$dia0:[000000]ld6.dsk lda6
$ ld conn/log $8$dia0:[000000]ld7.dsk lda7
$ ld conn/log $8$dia0:[000000]ld8.dsk lda8
$ ld trace/size=50 lda1
$ ld trace/size=50 lda2
$ ld trace/size=50 lda3
$ ld trace/size=50 lda4
$ ld trace/size=50 lda5
$ ld trace/size=50 lda6
$ ld trace/size=50 lda7
$ ld trace/size=50 lda8
$ ld show/all
$ init/nohigh/system lda1: st0
$ init/nohigh/system lda2: st0
$ init/nohigh/system lda3: st1
$ init/nohigh/system lda4: st1
$ init/nohigh/system lda5: st2
$ init/nohigh/system lda6: st2
$ init/nohigh/system lda7: st3
$ init/nohigh/system lda8: st3
$ stripe init/noconfirm lda1,lda3 st0,st1
$ stripe init/noconfirm lda5,lda7 st2,st3
$ dismount lda1
$ dismount lda3
$ dismount lda5
$ dismount lda7
$ mount/system dsa10:/shadow='f$getdvi("lda1:","alldevnam")' st0
$ mount/system dsa11:/shadow='f$getdvi("lda3:","alldevnam")' st1
$ stripe bind dsa10,dsa11/device=sta20
$ mount/system dsa12:/shadow='f$getdvi("lda5:","alldevnam")' st2
$ mount/system dsa13:/shadow='f$getdvi("lda7:","alldevnam")' st3
$ stripe bind dsa12,dsa13/device=sta21
$ mount/system dsa10:/shadow='f$getdvi("lda2:","alldevnam")' st0
$ mount/system dsa11:/shadow='f$getdvi("lda4:","alldevnam")' st1
$ mount/system dsa12:/shadow='f$getdvi("lda6:","alldevnam")' st2
$ mount/system dsa13:/shadow='f$getdvi("lda8:","alldevnam")' st3
$ init/nohig sta20: vset0
$ init/nohig sta21: vset1
$ mount/system/bind=bigdisk sta20,sta21: vset0,vset1 bigdisk
$ ld show/trace/status lda1
$ ld show/trace/status lda2
$ ld show/trace/status lda3
$ ld show/trace/status lda4
$ ld show/trace/status lda5
$ ld show/trace/status lda6
$ ld show/trace/status lda7
$ ld show/trace/status lda8
$ ld show/trace/output=lda1.log lda1
$ ld show/trace/output=lda2.log lda2
$ ld show/trace/output=lda3.log lda3
$ ld show/trace/output=lda4.log lda4
$ ld show/trace/output=lda5.log lda5
$ ld show/trace/output=lda6.log lda6
$ ld show/trace/output=lda7.log lda7
$ ld show/trace/output=lda8.log lda8
