# Makefile for cccp in the Emacs distribution only.
# Here we assume that you are using sys V.0
# (since cccp is used by Emacs only to deal with shortnames lossage).
# Therefore, we add -DEMACS to CFLAGS and add -lPW to linking.

.FIRST :
	extra_defs = ""

CFLAGS = /debug /include=(sys$disk:[]) /define=(EMACS=1'extra_defs')

all : cpp.exe
	!
cpp.exe : cccp.exe
	- if f$search("cpp.exe.*") .nes. "" then delete cpp.exe.*
	copy cccp.exe cpp.exe
cccp.exe : cccp.obj cexp.obj
	link /exe=cccp.exe cccp.obj,cexp.obj,[-.lib-src]alloca.obj,[-.vms]vmslink.opt/opt
testexp.exe : cexp.c
	a = ""
	if f$extract(0,3,f$getsyi("HW_NAME")) .eqs. "DEC" then -
		a = "/standard=vaxc/prefix=all/nested=include"
	extra_defs = ",TEST_EXP_READER=1,""rindex""=""strrchr"",""index""=""strchr"""
	cc $(CFLAGS) 'a' cexp.c /obj=_cexp.obj
	link /exe=testexp.exe _cexp.obj,[-.vms]vmslink.opt/opt
cexp.c : $(srcdir)cexp.y
	@ write sys$output "expect 40 shift/reduce conflicts"
	bison/yacc $(srcdir)cexp.y
	rename y_tab.c cexp.c
cccp.obj : $(srcdir)cccp.c
	a = ""
	if f$extract(0,3,f$getsyi("HW_NAME")) .eqs. "DEC" then -
		a = "/standard=vaxc/prefix=all/nested=include"
	extra_defs = ",TEST_EXP_READER=1,""rindex""=""strrchr"",""index""=""strchr"""
	cc $(CFLAGS) 'a' $(srcdir)cccp.c /obj=cccp.obj
cexp.obj : cexp.c
