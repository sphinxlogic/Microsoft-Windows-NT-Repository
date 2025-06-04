! DESCRIP.MMS
CC = cc/debug
COMMONFILES = dc.th,configdc.th,dccFiles.mng
dcd.exe depends_on dcblk.obj,dcdecl.obj,dcdir.obj,dcexp.obj,dcext.obj,-
	dcfmt.obj,dcinst.obj,dcmain.obj,dcprag.obj,dcrec.obj,dcrecdir.obj,-
	dctxttok.obj
  link/exe=dc dcblk.obj,dcdecl.obj,dcdir.obj,dcexp.obj,dcext.obj,dcfmt.obj,-
	dcinst.obj,dcmain.obj,dcprag.obj,dcrec.obj,dcrecdir.obj,dctxttok.obj
  link/exe=dcd/debug dcblk.obj,dcdecl.obj,dcdir.obj,dcexp.obj,dcext.obj,-
	dcfmt.obj,dcinst.obj,dcmain.obj,dcprag.obj,dcrec.obj,dcrecdir.obj,-
	dctxttok.obj
dcblk.obj depends_on dcblk.c,dcblk.h,dcblk.ph,dcdecl.h,dcdir.ph,dcext.h,-
	dcmain.h,dcprag.h,dcrec.h,dcrec.ph,$(COMMONFILES)
  $(CC) dcblk.c
dcdecl.obj depends_on dcdecl.c,dcdecl.h,dcblk.h,dcdir.h,dcexp.h,dcext.h,-
	dcinst.h,dcmain.h,dcprag.h,dcrec.h,dctxttok.h,$(COMMONFILES)
  $(CC)/warn=disable=(expandeddefined) dcdecl.c
dcdir.obj depends_on dcdir.c,dcdir.h,dcdir.ph,dcblk.h,dcblk.ph,dcdecl.h,-
	dcexp.h,dcext.h,dcmain.h,dcmain.ph,dcprag.h,dcprag.ph,dcrec.h,-
	dcrec.ph,dcrecdir.ph,dcrecdir.th,$(COMMONFILES)
  $(CC) dcdir.c
dcexp.obj depends_on dcexp.c,dcexp.h,dcexp.ph,dcblk.h,dcdecl.h,dcdir.h,dcext.h,-
	dcfmt.h,dcinst.h,dcprag.h,dcrec.h,dctxttok.h,$(COMMONFILES)
  $(CC)/warn=disable=(expandeddefined) dcexp.c
dcext.obj depends_on dcext.c,dcmsg.txt,dcext.h,dcext.ph,dcdecl.h,dcinst.h,-
	dcmain.h,dcprag.h,dcrec.h,$(COMMONFILES)
  $(CC) dcext.c
dcfmt.obj depends_on dcfmt.c,dcfmt.h,dcblk.h,dcext.h,$(COMMONFILES)
  $(CC) dcfmt.c
dcinst.obj depends_on dcinst.c,dcinst.h,dcblk.h,dcdecl.h,dcdir.h,dcexp.h,-
	dcexp.ph,dcext.h,dcrec.h,$(COMMONFILES)
  $(CC) dcinst.c
dcmain.obj depends_on dcmain.c,dcmain.h,dcmain.ph,dcblk.h,dcblk.ph,dcdecl.h,-
	dcdir.h,dcdir.ph,dcext.h,dcext.ph,dcprag.ph,dcrec.h,dcrec.ph,-
	dcrecdir.ph,dcrecdir.th,$(COMMONFILES)
  $(CC)/prefix=all dcmain.c
dcprag.obj depends_on dcprag.c,dcprag.h,dcprag.ph,dcblk.h,dcdecl.h,dcdir.h,-
	dcexp.h,dcext.h,dcmain.h,dcrec.h,dcrec.ph,dcrecdir.ph,dcrecdir.th,-
	$(COMMONFILES)
  $(CC) dcprag.c
dcrec.obj depends_on dcrec.c,dcrec.h,dcrec.ph,dcblk.h,dcblk.ph,dcdecl.h,-
	dcdir.ph,dcexp.h,dcext.h,dcmain.h,dcmain.ph,dcprag.ph,dcrecdir.ph,-
	dcrecdir.th,$(COMMONFILES)
  $(CC) dcrec.c
dcrecdir.obj depends_on dcrecdir.c,dcrecdir.ph,dcrecdir.th,dcmain.h,-
	$(COMMONFILES)
  $(CC) dcrecdir.c
dctxttok.obj depends_on dctxttok.c,dctxttok.h,$(COMMONFILES)
  $(CC) dctxttok.c
! End DESCRIP.MMS
