!*  CALLMON - A Call Monitor for OpenVMS Alpha
!*
!*  File:     CALLMON$MESSAGES.MSG
!*  Author:   Thierry Lelegard
!*  Version:  1.0
!*  Date:     24-JUL-1996
!*

.TITLE	CALLMON$MESSAGES "CALLMON Messages"

.FACILITY CALLMON, 812/PREFIX=CALLMON$_

.SEVERITY INFORMATIONAL

BRTARGET/FAO=1 "branch target at !XL"
BSR/FAO=3 "!XL: BSR R!SL, !AZ"
BSRDEPTH/FAO=1 "BSR depth is !SL"
BSRMOD/FAO=0 "modified BSR code stream follows"
BSRREP/FAO=2 "BSR replacement is !AZ = !SL"
BSRSEQ/FAO=0 "code stream before BSR follows"
BSRTOJSR/FAO=1 "replacing BSR by JSR at !XL"
CEFIX/FAO=1 "code address fixup to entry at !XL"
CPFIX/FAO=1 "code address fixup to PDSC at !XL"
CREFLW/FAO=1 "creating file !AZ, please wait..."
DOFIXUP/FAO=6 "look up !AZ fixup in !AZ!/!_[pdsc: !XL, entry: !XL] => [pdsc: !XL, entry: !XL]"
DOISD/FAO=4 "look up image section !SL in !AZ!/!_Start = !XL, end = !XL"
DORELOC/FAO=6 "look up !AZ relocations in !AZ!/!_[pdsc: !XL, entry: !XL] => [pdsc: !XL, entry: !XL]"
DUPNAM/FAO=3 "routine !AZ from !AZ also in !AZ"
DUPVAL/FAO=2 "routine !AZ also named !AZ"
FILDONE/FAO=1 "file !AZ is complete"
FIXUPCNT/FAO=3 "!SL fixups to !AZ in !AZ"
INIT/FAO=0 "initializing Call Monitor, please wait..."
INOK/FAO=0 "Call Monitor OK, !SL image!%S, !SL routine!%S"
INSDUMP/FAO=1 "!SL instructions processed"
INTCPTALL/FAO=4 <CALLMON$INTERCEPT_ALL ("!AZ", !XL, !XL, !AZ)>
INTERCEPT/FAO=3 <CALLMON$INTERCEPT ("!AZ", !XL, !XL)>
LDIMGOK/FAO=1 "loaded image !AZ, !SL routine!%S"
LEFIX/FAO=1 "longword address fixup to entry at !XL"
LEREL/FAO=1 "longword relocation to entry at !XL"
LKFIX/FAO=1 "linkage pair fixup at !XL"
LOADIMG/FAO=6 "loading image !AZ!/!_Base = !XL, start = !XL, end = !XL!/!_Vector = !XL (!SL bytes)"
LOADRTN/FAO=2 "loading routine !AD"
LPFIX/FAO=1 "longword address fixup to PDSC at !XL"
LPREL/FAO=1 "longword relocation to PDSC at !XL"
NOPR27/FAO=2 "Before BSR, last NOP at !XL, last LDx R27 at !XL"
NOTREL/FAO=1 "routine !AZ is not relocatable"
PASS1/FAO=2 "first pass complete, !SL instructions, !SL branch targets"
QEFIX/FAO=1 "quadword address fixup to entry at !XL"
QEREL/FAO=1 "quadword relocation to entry at !XL"
QPFIX/FAO=1 "quadword address fixup to PDSC at !XL"
QPREL/FAO=1 "quadword relocation to PDSC at !XL"
RELOCCNT/FAO=3 "!SL relocations to !AZ in !AZ"
SETPAGWRT/FAO=1 "modify page at !XL to user-write"
TRFLG/FAO=1 "trace flags are !XL"
UDUNINT/FAO=1 "user-defined uninterceptable routine !AZ"
UDUNREL/FAO=1 "user-defined unrelocatable routine !AZ"
VALINT/FAO=2 "!AZ = !SL"
VALHEX/FAO=2 "!AZ = !XL"
VALSTR/FAO=2 <!AZ = "!AZ">

.SEVERITY WARNING

FROMNOTMON/FAO=0 "calls from some images will not be monitored"
IVBSRREP/FAO=1 "invalid BSR replacement !AZ"
IVTRFLG/FAO=1 "invalid trace flag !AZ"
NOSYM/FAO=2 "routine !AZ not found in image !AZ"
PROTECTD/FAO=1 "image !AZ is protected"
RESIDENT/FAO=1 "image !AZ is resident"
TONOTMON/FAO=0 "calls to this image will not be monitored"

.SEVERITY ERROR

CRETVA/FAO=0 "error creating virtual address space"
DEFINE/FAO=1 "error defining logical name !AZ"
DELTVA/FAO=0 "error deleting virtual address space"
EXPREG/FAO=1 "error expanding memory by !UL pagelets"
FNDENTRY/FAO=0 "internal error in tree of routines (by entry)"
FNDIMAGE/FAO=0 "internal error in tree of images"
FNDNAME/FAO=0 "internal error in tree of routines (by name)"
FNDVALUE/FAO=0 "internal error in tree of routines (by value)"
GETJPI/FAO=0 "error getting process information"
GETSYI/FAO=0 "error getting system information"
GETVM/FAO=1 "error allocating !SL bytes of memory"
INF/FAO=0 "image not found"
INSIMGTREE/FAO=1 "error inserting !AZ in image tree"
INSRTNTREE/FAO=1 "error inserting !AZ in routine tree"
INVBSRDEP/FAO=1 "invalid value for CALLMON$BSR_DEPTH: !AZ"
MAPGST/FAO=1 "can't map global symbol table of image file !AZ"
MAPHEADER/FAO=1 "can't map header of image file !AZ"
MEMPROT/FAO=2 "can't unprotect memory range !XL - !XL"
NORESTRA/FAO=0 "cannot trace resident images"
NOTACTIVE/FAO=1 "image !AZ is not active in current process"
OPEN/FAO=1 "can't open !AZ"
PARSE/FAO=1 "don't understand !AZ"
READHEAD/FAO=1 "can't read header of !AZ"
RESROUT/FAO=0 "reserved routine, cannot intercept"
RMSPARSE/FAO=1 "cannot parse !AZ"
RNF/FAO=0 "routine not found"
RTNNAMTOOLNG/FAO=3 "routine name !AD is longer than !UL characters"
SETPAGWRTFAIL/FAO=1 "failed to modify page at !XL to user-write"

.SEVERITY FATAL

IMGNAMTOOLNG/FAO=2 "image name !AC is longer than !UL characters"

.END
