10	'***************************************************************
20	' PCUSQNEW.BAS - Version 1.0 - 3/17/85
30	'    by Kim Levitt - 213-653-6398 (MBBS, 300/1200 baud)
40	'    Update of original 11/25/84 version of USQ.BAS by
50	'    Dave (it can't be done) Rand - 805-493-1987 (voice)
60	'
65	'	This WILL run on an IBM-PC under interpreter BASICA,
66	'	but VERY SLOWLY.. When I compiled it under BASCOM
67	'	and LINKed it with BASCOM.LIB on the PC-AT, it ran
68	'	fast enough to be useable..
69	' 
70	'**************************************************************
80	' INITIALIZATION
90	'
100	DEFINT A-Z : GETC$=" " : GETW$="  " ' for speed!
110	DIM USQ.TABLE%(1,257), BUF$(128), OUTF$(127)
120	FILE$="" : SF%=0 : FO%=-1 : CSL%=0 : CSH%=0 : OD$=""
340	'***************************************************************
350	' COMMON CODE
360	'
370	' Common code, accessed with blank command line or begining of
380	' non-CP/M versions.. (Interactive version)
390	'
400	SF%=-1
410	PRINT : PRINT "PCUSQNEW.BAS -- A SLOW But Sure BASIC Unsqueezer"
420	PRINT
430	PRINT "(Enter name of file to 'unsqueeze'. Use upper case ONLY.)"
440	INPUT "Name of squeezed file";FILE$ : ID$=""
450	PRINT : PRINT "(Enter 'O' for console output only)"
460	INPUT "Console output? (Y/N/O)";CO$
470	IF CO$="O" OR CO$="o" THEN FO%=0 : GOTO 520
480	IF CO$="N" OR CO$="n" THEN SF%=0
490	PRINT : PRINT "(Enter return only for current disk)"
500	INPUT "Output drive";OD$
510	IF LEN(OD$)=1 THEN OD$=OD$+":"
520	PRINT : PRINT "Unsqueezing ";ID$;FILE$;" --> ";
530	OPEN "R", 1, ID$+FILE$, 128
540	GET #1, 1
550	IF EOF(1) THEN PRINT "[File not found]" : CLOSE #1 :
	   KILL ID$+FILE$ : SYSTEM
560	FOR X=0 TO 127 : FIELD 1, X AS D$, 1 AS BUF$(X) : NEXT X
570	GOSUB 900
580	IF USQ.ERR%<>0 THEN 
	   PRINT "[Error in header, or not squeezed]" : SYSTEM
590	PRINT "(";OD$;OLD.NAME$;")";
600	IF FO%=0 THEN PRINT " (console output only)" ELSE PRINT
610	PRINT
620	IF FO%=0 THEN 650
630	OPEN "R", 2, OD$+OLD.NAME$, 128 : OUTR%=1 : OUTS%=0
640	FOR X=0 TO 127 : FIELD #2, X AS D$, 1 AS OUTF$(X) : NEXT X
650	USQ.EOF%=0
660	GOSUB 1170
670	WHILE NOT(USQ.EOF%)
680	   IF INKEY$=CHR$(3) THEN 800
690	   IF SF% AND USQ$=CHR$(26) THEN SF%=0
700	   IF (SF% AND USQ$<>CHR$(10)) THEN PRINT USQ$;
710	   IF FO% THEN GOSUB 840
720	   CSL%=CSL%+ASC(USQ$) : CAR%=INT(CSL%/256) : CSL%=CSL%-(256*CAR%)
730	   CSH%=CSH%+CAR% : CSH%=(CSH% AND 255)
740	   GOSUB 1170
750	WEND
760	NCS%=CVI(CHR$(CSL%)+CHR$(CSH%))
770	IF NCS%<>OLD.CHECK% THEN PRINT : PRINT "[USQ checksum error"; :
	   IF FO% THEN PRINT ", check output file]" ELSE PRINT "]"
780	CLOSE 1 : IF FO% THEN IF OUTS% THEN PUT #2, OUTR% : CLOSE 2 ELSE CLOSE 2
790	SYSTEM
800	CLOSE : IF SF% THEN PRINT : PRINT
810	PRINT "[USQ aborted]"
820	IF FO% THEN KILL OD$+OLD.NAME$ : PRINT "[";OD$;OLD.NAME$;" killed]"
830	SYSTEM
840	'***************************************************************
850	' TRANSPARENT FILE OUTPUT ROUTINE
860	'
870	LSET OUTF$(OUTS%)=USQ$ : OUTS%=OUTS%+1
880	IF OUTS%=128 THEN PUT #2, OUTR% : OUTR%=OUTR%+1 : OUTS%=0
890	RETURN
900	'***************************************************************
910	' OPEN SQUEEZED FILE AND EXTRACT NAME, CHECKSUM AND TABLE
920	'
930	CUR.PNT%=128 : CUR.REC%=-1 : USQ.ERR%=0 : GOSUB 1120
940	IF GETW%<>-138 THEN USQ.ERR%=1 : GOTO 1040
950	GOSUB 1120 : OLD.CHECK%=GETW%
960	GOSUB 1050 : OLD.NAME$=""
970	WHILE GETC$<>CHR$(0) : OLD.NAME$=OLD.NAME$+GETC$ : GOSUB 1050 : WEND
980	GOSUB 1120 : COUNT%=GETW%
990	FOR X=0 TO COUNT%-1
1000	   GOSUB 1120 : USQ.TABLE%(0,X)=GETW%
1010	   GOSUB 1120 : USQ.TABLE%(1,X)=GETW%
1020	NEXT X
1030	REP.CNT%=0 : USQ.LFT%=0
1040	RETURN
1050	'****************************************************************
1060	' GET CHARACTER FROM SQUEEZED FILE
1070	'
1080	IF CUR.REC%<0 THEN CUR.REC%=0
1090	IF CUR.PNT%>127 THEN CUR.PNT%=0 : CUR.REC%=CUR.REC%+1 : GET 1,CUR.REC%
1100	LSET GETC$=BUF$(CUR.PNT%)
1110	CUR.PNT%=CUR.PNT%+1 : RETURN
1120	'***************************************************************
1130	' GET WORD FROM SQUEEZED FILE
1140	'
1150	GOSUB 1050 : MID$(GETW$,1,1)=GETC$ : GOSUB 1050 : MID$(GETW$,2,1)=GETC$
1160	GETW%=CVI(GETW$) : RETURN
1170	'***************************************************************
1180	' GET UNSQUEEZED CHARACTER
1190	'
1200	IF REP.CNT%<>0 THEN USQ$=USQ.LAST$ : REP.CNT%=REP.CNT%-1 : GOTO 1250
1210	GOSUB 1260 : IF USQ$<>CHR$(144) THEN USQ.LAST$=USQ$ : GOTO 1250
1220	GOSUB 1260 : REP.CNT%=ASC(USQ$)-2
1230	IF REP.CNT%=-2 THEN
	   REP.CNT%=0 : USQ$=CHR$(144) : USQ.LAST$=USQ$ : GOTO 1250
1240	USQ$=USQ.LAST$
1250	RETURN
1260	'***************************************************************
1270	' TABLE LOOKUP ROUTINE
1280	'
1290	CUR.USQ%=0
1300	IF USQ.LFT%=0 THEN GOSUB 1050 : USQ.BYTE%=ASC(GETC$) : USQ.LFT%=8
1310	BIT%=USQ.BYTE% AND 1 : USQ.BYTE%=INT(USQ.BYTE%/2) : USQ.LFT%=USQ.LFT%-1
1320	CUR.USQ%=USQ.TABLE%(BIT%,CUR.USQ%)
1330	IF CUR.USQ%>=0 THEN GOTO 1300
1340	IF CUR.USQ%<-255 THEN USQ.EOF%=-1 : GOTO 1360
1350	USQ$=CHR$(-CUR.USQ%-1)
1360	RETURN
1370	END
