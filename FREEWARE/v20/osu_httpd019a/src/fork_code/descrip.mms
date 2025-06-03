! MMS description for building fork-based scripts for HTTP server
!

scripts : testfork.exe,indirect.exe,chatter.exe
    write sys$output "Scripts done"

testfork.obj : testfork.c
    CC $(CFLAGS) testfork.c

TESTFORK.EXE : testfork.obj
    IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" -
	THEN DEFINE/NOLOG CC_LIBS CC_LIBS_AXP
    LINK $(LINKFLAGS) testfork,CC_LIBS/OPT
    COPY testfork.exe [-.cgi-bin]

indirect.obj : indirect.c
    CC $(CFLAGS) indirect.c

indirect.exe : indirect.obj
    IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" -
	THEN DEFINE/NOLOG CC_LIBS CC_LIBS_AXP
    LINK $(LINKFLAGS) indirect,CC_LIBS/OPT
    COPY indirect.exe [-.cgi-bin]

chatter.obj : chatter.c
    CC $(CFLAGS) chatter.c

chatter.exe : chatter.obj
    IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" -
	THEN DEFINE/NOLOG CC_LIBS CC_LIBS_AXP
    LINK $(LINKFLAGS) chatter,CC_LIBS/OPT
    COPY chatter.exe [-.cgi-bin]

