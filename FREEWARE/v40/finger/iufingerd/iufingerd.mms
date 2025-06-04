!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! iufingerd.mms
!
! (C) Copyright 1992-1993 The Trustees of Indiana University
!
! Permission to use, copy, modify, and distribute this program for
! non-commercial use and without fee is hereby granted, provided that
! this copyright and permission notice appear on all copies and
! supporting documentation, the name of Indiana University not be used
! in advertising or publicity pertaining to distribution of the program
! without specific prior permission, and notice be given in supporting
! documentation that copying and distribution is by permission of
! Indiana University.
! 
! Indiana University makes no representations about the suitability of
! this software for any purpose. It is provided "as is" without express
! or implied warranty.
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

CFLAGS = /list/opt/define=($(TCPIP),$(ARCH))$(CC_OPTIONS)

.IFDEF UCX
OBJS =  -
        args.obj, -
        cache.obj, -
        fuip.obj, -
        hash.obj, -
        jpi.obj, -
        mail.obj, -
        main.obj, -
        net.obj, -
        plan.obj, -
        qio.obj, -
        rfc931.obj, -
        uai.obj, -
        util.obj, -
        ucx_ioctl.obj
.ELSE
OBJS =  -
        args.obj, -
        cache.obj, -
        fuip.obj, -
        hash.obj, -
        jpi.obj, -
        mail.obj, -
        main.obj, -
        net.obj, -
        plan.obj, -
        qio.obj, -
        rfc931.obj, -
        uai.obj, -
        util.obj
.ENDIF

iufingerd.exe : $(OBJS)
    link /executable=iufingerd_$(ARCH)_$(TCPIP).exe $(OBJS), $(TCPIP).opt/opt

