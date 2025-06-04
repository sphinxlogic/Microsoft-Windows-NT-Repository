$! Analyze Map for OpenVMS AXP
$!
$ SET SYMBOL/GENERAL/SCOPE=(NOLOCAL,NOGLOBAL)
$ SAY := "WRITE_ SYS$OUTPUT"
$ 
$ IF F$SEARCH("''P1'") .EQS. ""
$ THEN
$    SAY "  ANALYZE_MAP.COM:  Error, no mapfile provided"
$    EXIT_
$ ENDIF
$ IF "''P2'" .EQS. ""
$ THEN
$    SAY "  ANALYZE_MAP.COM:  Error, no output file provided"
$    EXIT_
$ ENDIF
$
$ LINK_TMP  = F$PARSE(P2,,,"DEVICE")+F$PARSE(P2,,,"DIRECTORY")+F$PARSE(P2,,,"NAME")+".TMP"
$
$ SAY "  creating PSECT list in ''P2'"
$ OPEN_/READ IN 'P1'
$ OPEN_/WRITE OUT 'P2'
$ WRITE_ OUT "!"
$ WRITE_ OUT "! ### PSECT list extracted from ''P1'"
$ WRITE_ OUT "!" 
$ LOOP_PSECT_SEARCH:
$    READ_/END=EOF_PSECT IN REC
$    if F$EXTRACT(0,5,REC) .nes. "$DATA" then goto LOOP_PSECT_SEARCH
$ LAST = ""
$ LOOP_PSECT:
$    READ_/END=EOF_PSECT IN REC
$    if F$EXTRACT(0,1,REC) .eqs. "$" .and. F$EXTRACT(0,5,REC) .nes. "$DATA" then goto EOF_PSECT
$    if REC - "NOPIC,OVR,REL,GBL,NOSHR,NOEXE,  WRT,NOVEC" .nes. REC
$    then 
$       J = F$LOCATE(" ",REC)
$       S = F$EXTRACT(0,J,REC)
$       IF S .EQS. LAST THEN GOTO LOOP_PSECT
$       WRITE_ OUT "symbol_vector = (" +  S + " = PSECT)"
$       P$_'S= 1
$       LAST = S
$    endif
$    GOTO LOOP_PSECT
$
$ EOF_PSECT:
$    CLOSE_ IN
$    CLOSE_ OUT 
$
$ SAY "  appending list of UNIVERSAL procedures to ''P2'"
$ SEARCH_/NOHIGH/WINDOW=(0,0) 'P1' " R-"/OUT='LINK_TMP
$ OPEN_/READ IN 'LINK_TMP
$ OPEN_/APPEND OUT 'P2'
$ WRITE_ OUT "!"
$ WRITE_ OUT "! ### UNIVERSAL procedures and global definitions extracted from ''P1'"
$ WRITE_ OUT "!" 
$ LOOP_UNIVERSAL:
$    READ_/END=EOF_UNIVERSAL IN REC
$    J = F$LOCATE(" R-",REC)
$    S = F$EXTRACT(J+3,F$length(rec),REC)
$    IF F$TYPE(P$_'S').EQS.""
$    THEN
$       WRITE_ OUT "symbol_vector = ("+S+"	= DATA)"
$!$       WRITE_ OUT "symbol_vector = ("+S+"	= PROCEDURE)"
$    ELSE
$       WRITE_ OUT "symbol_vector = ("+S+"	= DATA)"
$    ENDIF
$    GOTO LOOP_UNIVERSAL
$ EOF_UNIVERSAL:
$    CLOSE_ IN
$    CLOSE_ OUT
$    if f$search("''LINK_TMP'") .nes. "" then DELETE_/NOLOG/NOCONFIRM 'LINK_TMP';*
$
$ EXIT_ 
