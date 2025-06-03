C  FLECS TRANSLATOR (PRELIMINARY VERSION 22.)                             !00001
C  (FOR A MORE PRECISE VERSION NUMBER, SEE THE STRING SVER)               !00002
C                                                                         !00003
C  AUTHOR --    TERRY BEYER                                               !00004
C                                                                         !00005
C  ADDRESS --   COMPUTING CENTER                                          !00006
C           UNIVERSITY OF OREGON                                          !00007
C           EUGENE, OREGON 97405                                          !00008
C                                                                         !00009
C  TELEPHONE -- (503)  686-4416                                           !00010
C                                                                         !00011
C  DATE --      NOVEMBER 20, 1974                                         !00012
C                                                                         !00013
C---------------------------------------                                  !00014
C                                                                         !00015
C  DISCLAIMER                                                             !00016
C                                                                         !00017
C     NEITHER THE AUTHOR NOR THE UNIVERSITY OF OREGON SHALL BE            !00018
C  LIABLE FOR ANY DIRECT OR INDIRECT, INCIDENTAL, CONSEQUENTIAL,          !00019
C  OR SPECIFIC DAMAGES OF ANY KIND OR FROM ANY CAUSE WHATSOEVER           !00020
C  ARISING OUT OF OR IN ANY WAY CONNECTED WITH THE USE OR                 !00021
C  PERFORMANCE OF THIS PROGRAM.                                           !00022
C                                                                         !00023
C---------------------------------------                                  !00024
C                                                                         !00025
C  PERMISSION                                                             !00026
C                                                                         !00027
C     THIS PROGRAM IS IN THE PUBLIC DOMAIN AND MAY BE ALTERED             !00028
C  OR REPRODUCED WITHOUT EXPLICIT PERMISSION OF THE AUTHORC               !00029
C---------------------------------------                                  !00030
C                                                                         !00031
C  NOTE TO THE PROGRAMMER WHO WISHES TO ALTER THIS CODE                   !00032
C                                                                         !00033
C                                                                         !00034
C     THE PROGRAM BELOW IS THE RESULT OF ABOUT SIX MONTHS OF              !00035
C  RAPID EVOLUTION IN ADDITION TO BEING THE FIRST SUCH                    !00036
C  PROGRAM I HAVE EVER WRITTEN.  YOU WILL FIND IT IS UNCOMMENTED,         !00037
C  AND IN MANY PLACES OBSCURE.  THE LOGIC IS FREQUENTLY                   !00038
C  BURIED UNDER A PILE OF PATCHES WHICH BARELY TOLERATE EACH              !00039
C  OTHER S EXISTENCE.                                                     !00040
C                                                                         !00041
C     I PLAN TO WRITE A CLEANER, SMALLER, AND FASTER VERSION OF           !00042
C  THIS PROGRAM WHEN GIVEN THE OPPORTUNITY.  IT WAS NEVER                 !00043
C  MY INTENT TO PRODUCE A PROGRAM MAINTAINABLE BY ANYONE OTHER            !00044
C  THAN MYSELF ON THIS FIRST PASS.  NEVERTHLESS PLEASE                    !00045
C  ACCEPT MY APOLOGIES FOR THE CONDITION OF THE CODE BELOW.               !00046
C  I WOULD PREFER IT IF YOU WOULD CONTACT ME AND WAIT FOR                 !00047
C  THE NEWER VERSION BEFORE MAKING ANY BUT THE MOST NECESSARY             !00048
C  CHANGES TO THIS PROGRAM.  YOU WILL PROBABLY SAVE YOURSELF              !00049
C  MUCH TIME AND GRIEF.                                                   !00050
C                                                                         !00051
C---------------------------------------                                  !00052
C  MODIFIED 7-13-79 BY WILLIAM TANENBAUM                                  !00053
C  HARVARD UNIVERSITY PHYSICS DEPARTMENT                                  !00054
C  FOR DIGITAL VAX 11/780 USING CHARACTER DATA TYPE                       !00055
C  FACTOR OF 4 TO 5 SPEEDUP IN RUN TIME ACHIEVED                          !00056
C  ALSO ALLOWS EXCLAMATION POINT COMMENTS ON FLECS STATEMENTS             !00057
C  ALSO ALLOWS DEBUG LINES FEATURE.                                       !00058
C                                                                         !00059
C---------------------------------------------------------------          !00060
C                                                                         !00061
C  INTEGER DECLARATIONS                                                   !00062
C                                                                         !00063
C                                                                         !00064
      IMPLICIT INTEGER*4(A-Z)                                             !00065
C                                                                         !00066
C---------------------------------------                                  !00067
C                                                                         !00068
C  LOGICAL DECLARATIONS                                                   !00069
C                                                                         !00070
C                                                                         !00071
      LOGICAL DOFORT, DOLIST, DEBUGG, COMENT, CFIRST, ENDIF               !00072
      LOGICAL BADCH , CONT  , DONE  , ENDFIL, ENDPGM, ERLST               !00073
      LOGICAL FIRST , FOUND , INDENT, INSERT, INVOKE, MINER               !00074
      LOGICAL NDERR , NIERR , NOPGM , PASS  , SAVED                       !00075
C                                                                         !00076
C---------------------------------------                                  !00077
C                                                                         !00078
C  ARRAY DECLARATIONS                                                     !00079
C                                                                         !00080
C                                                                         !00081
C  ARRAYS WHICH HOLD RESULTS OF SCANNERS ANALYSIS                         !00082
      DIMENSION  UTYPE(3), USTART(3), UFIN(3)                             !00083
C                                                                         !00084
C  STACK/TABLE AREA AND POINTER TO TOP OF STACK                           !00085
      DIMENSION STACK(2000)                                               !00086
      CHARACTER*8000 CSTAK                                                !00087
      EQUIVALENCE (STACK,CSTAK)                                           !00088
C                                                                         !00089
C  SYNTAX ERROR STACK AND TOP POINTER                                     !00090
      DIMENSION ERRSTK(5)                                                 !00091
C                                                                         !00092
C---------------------------------------                                  !00093
C                                                                         !00094
C  MNEMONIC DECLARATIONS                                                  !00095
C                                                                         !00096
C                                                                         !00097
C  I/O CLASS CODES FOR USE WITH SUBROUTINE PUT                            !00098
      DATA  LISTCL /2/, ERRCL /3/                                         !00099
C                                                                         !00100
C  ACTION CODES FOR USE ON ACTION STACK                                   !00101
      DATA ACSEQ/1/, AELSE/2/, AFSEQ/3/, ADSEQ/4/, ARSEQ/5/               !00102
      DATA ATSEQ/6/, AMSEQ/7/, AGRET/8/, ASSEQ/9/                         !00103
C                                                                         !00104
C  TYPE CODES USED BY SCANNERS                                            !00105
      DATA UEXP/1/, UFORT/2/, UOWSE/3/, UPINV/4/, UDO/5/                  !00106
C                                                                         !00107
C  TYPE CODES OF CHARACTERS (SUPPLIED BY CHTYPE)                          !00108
C  WARNING - LOGIC IS SENSITIVE TO THE ORDER OF THESE VALUES.             !00109
      DATA TLETTR/1/, TDIGIT/2/, THYPHN/3/, TLP/4/, TRP/5/                !00110
      DATA TBLANK/6/, TOTHER/7/, TEOL/8/                                  !00111
C                                                                         !00112
C  TYPE CODES ASSIGNED TO THE VARIABLE CLASS                              !00113
      DATA TCEXP/1/, TELSE/2/, TEND/3/, TEXEC/4/, TFIN/5/, TTO/6/         !00114
C                                                                         !00115
C  TYPE CODES ASSIGNED TO THE VARIABLE EXTYPE                             !00116
      DATA TCOND/1/, TDO/2/, TFORT/3/, TIF/4/, TINVOK/5/, TRUNTL/6/       !00117
      DATA TRWHIL/7/, TSELCT/8/, TUNLES/9/, TUNTIL/10/, TWHEN/11/         !00118
      DATA TWHILE/12/                                                     !00119
C                                                                         !00120
C  CODES INDICATING SOURCE OF NEXT STATEMENT                              !00121
C  IN ANALYZE-NEXT-STATEMENT                                              !00122
      DATA SETUP /1/, RETRY /2/, READ /3/                                 !00123
C                                                                         !00124
C---------------------------------------                                  !00125
C                                                                         !00126
C                                                                         !00127
C  PARAMETERS                                                             !00128
C                                                                         !00129
C  THE FOLLOWING VARIABLES ARE PARAMETERS FOR THE PROGRAM.                !00130
C  THE MEANING OF EACH IS GIVEN BRIEFLY BELOW. FOR MORE INFORMATION       !00131
C  ON THE EFFECT OF THESE PARAMETERS, CONSULT THE SYSTEM MODIFICATION     !00132
C  GUIDE.                                                                 !00133
C                                                                         !00134
C                                                                         !00135
C  LISTING WIDTH IN CHARACTERS                                            !00136
      DATA LWIDTH /132/                                                   !00137
C                                                                         !00138
C  SIZE OF THE MAIN STACK                                                 !00139
      DATA MAXSTK /2000/                                                  !00140
C                                                                         !00141
C  NUMBER OF CHARACTERS PER WORD (PER INTEGER) IN A FORMAT                !00142
      DATA NCHPWD /4/                                                     !00143
C                                                                         !00144
C  SIZE OF HASH TABLE FOR PROCEDURE NAMES -  SHOULD BE PRIME.             !00145
      DATA PRIME /53/                                                     !00146
C                                                                         !00147
C  SAFETY MARGIN BETWEEN TOP AND MAX AT BEGINNING OF EACH LOOP            !00148
      DATA SAFETY /35/                                                    !00149
C                                                                         !00150
C  SEED FOR GENERATION OF STATEMENT NUMBERS                               !00151
      DATA SEEDNO /100000/                                                !00152
C                                                                         !00153
C                                                                         !00154
      CHARACTER*1 CH, CHZERO                                              !00155
      BYTE ICH, ICHZER, CHTYP(0:255)                                      !00156
      DATA CHTYP/9*7,6,8,21*7,6,7*7,4,5,3*7,3,2*7,10*2,7*7,26*1,6*7,      !00157
     126*1,133*7/                                                         !00158
C       EQUIVALENCE (CHZERO,ICHZER),(CH,ICH)    !TG 4/1/82                !00159
      EQUIVALENCE (CH,ICH)                                                !00160
      DATA ICHZER/'0'/                                                    !00161
      CHARACTER*80 BLANKS                                                 !00162
      DATA BLANKS/' '/                                                    !00163
      CHARACTER* 5 SBLN                                                   !00164
      CHARACTER*40 SDASH                                                  !00165
      CHARACTER*30 SENDER                                                 !00166
      CHARACTER* 5 SENDIF                                                 !00167
      CHARACTER*53 SGUP1                                                  !00168
      CHARACTER*44 SGUP2                                                  !00169
      CHARACTER*54 SICOND                                                 !00170
      CHARACTER*45 SIELSE                                                 !00171
      CHARACTER*44 SIFIN                                                  !00172
      CHARACTER*52 SIFIN2                                                 !00173
      CHARACTER*45 SIGN                                                   !00174
      CHARACTER*44 SINSRT                                                 !00175
      CHARACTER*51 SINS2                                                  !00176
      CHARACTER*50 SITODM                                                 !00177
      CHARACTER*39 SIWHEN                                                 !00178
      CHARACTER* 6 SLINE                                                  !00179
      CHARACTER*50 SMULER                                                 !00180
      CHARACTER*42 SNDER1                                                 !00181
      CHARACTER*44 SNDER2                                                 !00182
      CHARACTER*46 SNIER1                                                 !00183
      CHARACTER*44 SNIER2                                                 !00184
      CHARACTER*37 STABH                                                  !00185
      CHARACTER*21 SVER                                                   !00186
      CHARACTER*54 SXER1                                                  !00187
      CHARACTER*53 SXER2                                                  !00188
      CHARACTER*50 SXER3                                                  !00189
      CHARACTER*40 SXER4                                                  !00190
      CHARACTER*50 SXER5                                                  !00191
      CHARACTER*40 SXER6                                                  !00192
C                                                                         !00193
      COMMON/FILES/DOFORT,DOLIST,DEBUGG                                   !00194
C                                                                         !00195
C---------------------------------------                                  !00196
C                                                                         !00197
C  STRING DECLARATIONS                                                    !00198
C                                                                         !00199
C                                                                         !00200
C  THE FOLLOWING ARRAYS ARE USED FOR STORAGE OF WORKING STRINGS           !00201
C  AND CORRESPOND TO STRINGS OF THE LENGTHS INDICATED.                    !00202
C  THE SIZES GIVEN BELOW ARE EXCESSIVE AND SHOULD BE                      !00203
C  BE REDUCED AFTER CAREFUL ANALYSIS (NO TIME NOW).                       !00204
C                                                                         !00205
      CHARACTER*80 SCOMN                                                  !00206
      CHARACTER*100 SFLX                                                  !00207
      CHARACTER*100 SHOLD                                                 !00208
      CHARACTER*200 SLIST                                                 !00209
      CHARACTER*80 SPINV                                                  !00210
      CHARACTER*200 SST                                                   !00211
      DATA SSTMAX /200/                                                   !00212
C                                                                         !00213
C  THE FOLLOWING STRINGS REPRESENT CONSTANTS                              !00214
C                                                                         !00215
      DATA SDASH  /'----------------------------------------'/            !00216
      DATA SENDER /'***** END statement is missing.'/                     !00217
      DATA SGUP1                                                          !00218
     1/'***** Translator has used up alloted space for tables.'/          !00219
      DATA SGUP2  /'***** Translation must terminate immediately.'/       !00220
      DATA SICOND                                                         !00221
     1/'*****   (CONDITIONAL or SELECT is apparently missing.)'/          !00222
      DATA SIELSE /'*****    (ELSE necessary to match line      )'/       !00223
      DATA SIFIN  /'*****    (FIN necessary to match line      )'/        !00224
      DATA SIFIN2                                                         !00225
     1/'*****   (FIN necessary to match line assumed above.)'/            !00226
      DATA SIGN   /'*****   (No control phrase for FIN to match.)'/       !00227
      DATA SINSRT /'***** Statement(s) needed before line      )'/        !00228
      DATA SINS2                                                          !00229
     1/'***** Statement(s) needed before line assumed below'/             !00230
      DATA SITODM                                                         !00231
     1/'*****   (Only TO and END are valid at this point.)'/              !00232
      DATA SIWHEN /'*****    (WHEN to match following ELSE)'/             !00233
      DATA SMULER                                                         !00234
     1/'*****    (Procedure already defined on line      )'/              !00235
      DATA SNDER1 /'***** The next procedures were invoked on'/           !00236
      DATA SNDER2 /'***** the lines given but were never defined:'/       !00237
      DATA SNIER1 /'***** The following procedures were defined on'/      !00238
      DATA SNIER2 /'***** the lines given but were never invoked:'/       !00239
      DATA STABH  /'      Procedure Cross-Reference Table'/               !00240
      DATA SVER   /'(FLECS version 22.60)'/                               !00241
      DATA SXER1                                                          !00242
     1/'*****  (Invalid character in statement number field.)'/           !00243
      DATA SXER2                                                          !00244
     1/'*****  (Recognizable statement followed by garbage.)'/            !00245
      DATA SXER3                                                          !00246
     1/'*****   (Left paren does not follow control word.)'/              !00247
      DATA SXER4  /'*****   (Too few right parentheses.)'/                !00248
      DATA SXER5                                                          !00249
     1/'*****    (Valid procedure name does not follow TO)'/              !00250
      DATA SXER6  /'*****   (Too many right parentheses.)'/               !00251
C                                                                         !00252
C---------------------------------------                                  !00253
C                                                                         !00254
C  MAIN PROGRAM                                                           !00255
C                                                                         !00256
C      PERFORM-INITIALIZATION                                             !00257
      ASSIGN 99998 TO I99999                                              !00257
      GO TO 99999                                                         !00257
99998 CONTINUE                                                            !00257
      IF(.TRUE.)GOTO99997                                                 !00258
      DO WHILE(.NOT.(DONE))                                               !00258
99997   CONTINUE                                                          !00258
        CALLNO=CALLNO+1                                                   !00259
        CALL OPENF(CALLNO,DONE,SVER)                                      !00260
        IF(.NOT.(DONE))THEN                                               !00261
          ENDFIL=.FALSE.                                                  !00262
          MINCNT=0                                                        !00263
          MAJCNT=0                                                        !00264
          LINENO=0                                                        !00265
          SLINE=' '                                                       !00266
          IF(.TRUE.)GOTO99996                                             !00267
          DO WHILE(.NOT.(ENDFIL))                                         !00267
99996       CONTINUE                                                      !00267
C            PREPARE-TO-PROCESS-PROGRAM                                   !00268
            ASSIGN 99994 TO I99995                                        !00268
            GO TO 99995                                                   !00268
99994       CONTINUE                                                      !00268
C            PROCESS-PROGRAM                                              !00269
            ASSIGN 99992 TO I99993                                        !00269
            GO TO 99993                                                   !00269
99992       CONTINUE                                                      !00269
            ENDDO                                                         !00270
          CALL CLOSEF(MINCNT,MAJCNT)                                      !00271
          ENDIF                                                           !00272
        ENDDO                                                             !00273
C     RETURN                                                              !00274
      IF(MAJCNT.EQ.0)THEN                                                 !00275
        CALL EXIT                                                         !00275
      ELSE                                                                !00276
        CALL SYS$EXIT(%VAL(44))                                           !00276
        ENDIF                                                             !00276
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO ABORT-ENDIF                                                   !00278
99991 CONTINUE                                                            !00278
        ENDIF=.FALSE.                                                     !00279
        IND=INDEX(SST(1:LSST),'!')                                        !00280
        IF(SST(1:5).NE.' ')THEN                                           !00282
          L=INDEX(SST(1:LSST),'ENDIF')                                    !00283
          SST(1:LSST+3)=SST(1:L-1)//'CONTINUE'//SST(L+5:LSST+1)           !00284
          LSST=LSST+3                                                     !00285
C          PUT-ENDIF                                                      !00286
          ASSIGN 99989 TO I99990                                          !00286
          GO TO 99990                                                     !00286
99989     CONTINUE                                                        !00286
        ELSEIF(IND.NE.0)THEN                                              !00288
          CALL PUTF(SENDIF,SST(IND:LSST))                                 !00288
          ENDIF                                                           !00288
      GO TO I99991                                                        !00290
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO ANALYZE-ERRORS-AND-LIST                                       !00291
99988 CONTINUE                                                            !00291
        IF(SOURCE.EQ.SETUP)THEN                                           !00293
          SOURCE=RETRY                                                    !00293
        ELSEIF(ERROR.EQ.0.AND.ERSTOP.EQ.0)THEN                            !00294
          SOURCE=READ                                                     !00295
C          LIST-FLEX                                                      !00296
          ASSIGN 99986 TO I99987                                          !00296
          GO TO 99987                                                     !00296
99986     CONTINUE                                                        !00296
        ELSE                                                              !00298
          MINER=(((ERROR.GE.5).AND.(ERROR.LE.6)).OR.                      !00299
     1    ((ERROR.GE.13).AND.(ERROR.LE.15)))                              !00300
          MINER=MINER.OR.((ERROR.GE.1).AND.(ERROR.LE.3))                  !00301
     2    .OR.(ERROR.EQ.25)                                               !00302
          IF(MINER)THEN                                                   !00303
            MINCNT=MINCNT+1                                               !00303
          ELSE                                                            !00304
            MAJCNT=MAJCNT+1                                               !00304
            ENDIF                                                         !00304
          IF(ERROR.EQ.0)THEN                                              !00305
            ERTYPE=1                                                      !00305
          ELSE                                                            !00306
            IF(ERROR.LE.3)THEN                                            !00308
C              INSERT-FIN                                                 !00308
              ASSIGN 99984 TO I99985                                      !00308
              GO TO 99985                                                 !00308
99984         CONTINUE                                                    !00308
            ELSEIF(ERROR.EQ.4)THEN                                        !00309
C              INSERT-ELSE                                                !00309
              ASSIGN 99982 TO I99983                                      !00309
              GO TO 99983                                                 !00309
99982         CONTINUE                                                    !00309
            ELSEIF(ERROR.LE.6)THEN                                        !00310
              ERTYPE=3                                                    !00310
            ELSEIF(ERROR.EQ.7)THEN                                        !00311
C              INSERT-ELSE                                                !00311
              ASSIGN 99981 TO I99983                                      !00311
              GO TO 99983                                                 !00311
99981         CONTINUE                                                    !00311
            ELSEIF(ERROR.EQ.8)THEN                                        !00312
C              INSERT-WHEN                                                !00312
              ASSIGN 99979 TO I99980                                      !00312
              GO TO 99980                                                 !00312
99979         CONTINUE                                                    !00312
            ELSEIF(ERROR.EQ.9)THEN                                        !00313
C              INSERT-TO-DUMMY-PROCEDURE                                  !00313
              ASSIGN 99977 TO I99978                                      !00313
              GO TO 99978                                                 !00313
99977         CONTINUE                                                    !00313
            ELSEIF(ERROR.EQ.10)THEN                                       !00314
C              INSERT-WHEN-OR-FIN                                         !00314
              ASSIGN 99975 TO I99976                                      !00314
              GO TO 99976                                                 !00314
99975         CONTINUE                                                    !00314
            ELSEIF(ERROR.LE.12)THEN                                       !00315
C              INSERT-FIN                                                 !00315
              ASSIGN 99974 TO I99985                                      !00315
              GO TO 99985                                                 !00315
99974         CONTINUE                                                    !00315
            ELSEIF(ERROR.LE.15)THEN                                       !00316
C              INSERT-FIN                                                 !00316
              ASSIGN 99973 TO I99985                                      !00316
              GO TO 99985                                                 !00316
99973         CONTINUE                                                    !00316
            ELSEIF(ERROR.EQ.16)THEN                                       !00317
C              INSERT-ELSE                                                !00317
              ASSIGN 99972 TO I99983                                      !00317
              GO TO 99983                                                 !00317
99972         CONTINUE                                                    !00317
            ELSEIF(ERROR.EQ.17)THEN                                       !00318
C              INSERT-CONDITIONAL                                         !00318
              ASSIGN 99970 TO I99971                                      !00318
              GO TO 99971                                                 !00318
99970         CONTINUE                                                    !00318
            ELSEIF(ERROR.EQ.18)THEN                                       !00319
C              INSERT-TO-DUMMY-PROCEDURE                                  !00319
              ASSIGN 99969 TO I99978                                      !00319
              GO TO 99978                                                 !00319
99969         CONTINUE                                                    !00319
            ELSEIF(ERROR.LE.19)THEN                                       !00320
C              INSERT-CONDITIONAL                                         !00320
              ASSIGN 99968 TO I99971                                      !00320
              GO TO 99971                                                 !00320
99968         CONTINUE                                                    !00320
            ELSEIF(ERROR.EQ.20)THEN                                       !00321
C              INSERT-ELSE                                                !00321
              ASSIGN 99967 TO I99983                                      !00321
              GO TO 99983                                                 !00321
99967         CONTINUE                                                    !00321
            ELSEIF(ERROR.EQ.21)THEN                                       !00322
C              INSERT-TO-DUMMY-PROCEDURE                                  !00322
              ASSIGN 99966 TO I99978                                      !00322
              GO TO 99978                                                 !00322
99966         CONTINUE                                                    !00322
            ELSEIF(ERROR.LE.23)THEN                                       !00323
C              INSERT-FIN                                                 !00323
              ASSIGN 99965 TO I99985                                      !00323
              GO TO 99985                                                 !00323
99965         CONTINUE                                                    !00323
            ELSEIF(ERROR.EQ.24)THEN                                       !00324
C              INSERT-ELSE                                                !00324
              ASSIGN 99964 TO I99983                                      !00324
              GO TO 99983                                                 !00324
99964         CONTINUE                                                    !00324
            ELSEIF(ERROR.EQ.25)THEN                                       !00325
              ERTYPE=4                                                    !00325
            ELSEIF(ERROR.EQ.26)THEN                                       !00326
              ERTYPE=5                                                    !00326
              ENDIF                                                       !00326
            ENDIF                                                         !00328
          SOURCE=READ                                                     !00329
          IF((1).EQ.(ERTYPE))THEN                                         !00331
            CALL PUTN(SLINE(2:6),SHOLD(1:LSHOLD),ERRCL)                   !00332
            DO I=1,ERSTOP                                                 !00333
              IF((1).EQ.(ERRSTK(I)))THEN                                  !00335
                CALL PUTZ(SXER1,ERRCL)                                    !00335
              ELSEIF((2).EQ.(ERRSTK(I)))THEN                              !00336
                CALL PUTZ(SXER2,ERRCL)                                    !00336
              ELSEIF((3).EQ.(ERRSTK(I)))THEN                              !00337
                CALL PUTZ(SXER3,ERRCL)                                    !00337
              ELSEIF((4).EQ.(ERRSTK(I)))THEN                              !00338
                CALL PUTZ(SXER4,ERRCL)                                    !00338
              ELSEIF((5).EQ.(ERRSTK(I)))THEN                              !00339
                CALL PUTZ(SXER5,ERRCL)                                    !00339
              ELSEIF((6).EQ.(ERRSTK(I)))THEN                              !00340
                CALL PUTZ(SXER6,ERRCL)                                    !00340
                ENDIF                                                     !00340
              ENDDO                                                       !00342
          ELSEIF((2).EQ.(ERTYPE))THEN                                     !00344
            SOURCE=SETUP                                                  !00344
          ELSEIF((3).EQ.(ERTYPE))THEN                                     !00345
            CALL PUTN(SLINE(2:6),SFLX(1:LSFLX),ERRCL)                     !00346
            CALL PUTZ(SIGN,ERRCL)                                         !00347
          ELSEIF((4).EQ.(ERTYPE))THEN                                     !00349
            CALL PUTZ(SENDER,ERRCL)                                       !00349
          ELSEIF((5).EQ.(ERTYPE))THEN                                     !00350
            CALL PUT(SLINE(2:6),SFLX(1:LSFLX),ERRCL)                      !00351
            WRITE(SMULER(45:49),'(I5)')MLINE                              !00352
            CALL PUTZ(SMULER,ERRCL)                                       !00353
            ENDIF                                                         !00354
          ENDIF                                                           !00356
        IF(ENDPGM)THEN                                                    !00358
C          PROCESS-TABLE                                                  !00359
          ASSIGN 99962 TO I99963                                          !00359
          GO TO 99963                                                     !00359
99962     CONTINUE                                                        !00359
C          LIST-BLANK-LINE                                                !00360
          ASSIGN 99960 TO I99961                                          !00360
          GO TO 99961                                                     !00360
99960     CONTINUE                                                        !00360
          CALL PUTZ(SVER,LISTCL)                                          !00361
          ENDIF                                                           !00362
      GO TO I99988                                                        !00363
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO ANALYZE-NEXT-STATEMENT                                        !00364
99959 CONTINUE                                                            !00364
        IF((READ).EQ.(SOURCE))THEN                                        !00366
C          READ-NEXT-STATEMENT                                            !00366
          ASSIGN 99957 TO I99958                                          !00366
          GO TO 99958                                                     !00366
99957     CONTINUE                                                        !00366
        ELSEIF((SETUP).EQ.(SOURCE))THEN                                   !00367
        ELSEIF((RETRY).EQ.(SOURCE))THEN                                   !00368
          LINENO=HOLDNO                                                   !00369
          WRITE(SLINE,'(I6.5)')LINENO                                     !00370
          LSFLX=LSHOLD                                                    !00371
          SFLX(1:LSFLX)=SHOLD(1:LSHOLD)                                   !00372
          ENDIF                                                           !00373
        ERROR=0                                                           !00375
        SAVED=.FALSE.                                                     !00376
        NUNITS=0                                                          !00377
        ERSTOP=0                                                          !00378
        CURSOR=0                                                          !00379
        CLASS=0                                                           !00380
C        SCAN-STATEMENT-NUMBER                                            !00381
        ASSIGN 99955 TO I99956                                            !00381
        GO TO 99956                                                       !00381
99955   CONTINUE                                                          !00381
C        SCAN-CONTINUATION                                                !00382
        ASSIGN 99953 TO I99954                                            !00382
        GO TO 99954                                                       !00382
99953   CONTINUE                                                          !00382
        IF(CONT.OR.PASS)THEN                                              !00383
          CLASS=TEXEC                                                     !00384
          EXTYPE=TFORT                                                    !00385
        ELSE                                                              !00387
C          SCAN-KEYWORD                                                   !00387
          ASSIGN 99951 TO I99952                                          !00387
          GO TO 99952                                                     !00387
99951     CONTINUE                                                        !00387
          ENDIF                                                           !00387
        IF((TEXEC).EQ.(CLASS))THEN                                        !00389
          IF((TFORT).EQ.(EXTYPE))THEN                                     !00391
          ELSEIF((TINVOK).EQ.(EXTYPE))THEN                                !00392
C            SCAN-GARBAGE                                                 !00392
            ASSIGN 99949 TO I99950                                        !00392
            GO TO 99950                                                   !00392
99949       CONTINUE                                                      !00392
          ELSEIF((TCOND).EQ.(EXTYPE))THEN                                 !00393
C            SCAN-GARBAGE                                                 !00393
            ASSIGN 99948 TO I99950                                        !00393
            GO TO 99950                                                   !00393
99948       CONTINUE                                                      !00393
          ELSEIF((TSELCT).EQ.(EXTYPE))THEN                                !00394
C            SCAN-CONTROL                                                 !00395
            ASSIGN 99946 TO I99947                                        !00395
            GO TO 99947                                                   !00395
99946       CONTINUE                                                      !00395
            IF(NUNITS.GT.1)THEN                                           !00396
              NUNITS=1                                                    !00397
              CURSOR=USTART(2)                                            !00398
C              RESET-GET-CHARACTER                                        !00399
              ASSIGN 99944 TO I99945                                      !00399
              GO TO 99945                                                 !00399
99944         CONTINUE                                                    !00399
C              SCAN-GARBAGE                                               !00400
              ASSIGN 99943 TO I99950                                      !00400
              GO TO 99950                                                 !00400
99943         CONTINUE                                                    !00400
              ENDIF                                                       !00401
          ELSE                                                            !00403
C            SCAN-CONTROL                                                 !00403
            ASSIGN 99942 TO I99947                                        !00403
            GO TO 99947                                                   !00403
99942       CONTINUE                                                      !00403
            ENDIF                                                         !00403
        ELSEIF((TFIN).EQ.(CLASS))THEN                                     !00406
C          SCAN-GARBAGE                                                   !00406
          ASSIGN 99941 TO I99950                                          !00406
          GO TO 99950                                                     !00406
99941     CONTINUE                                                        !00406
        ELSEIF((TEND).EQ.(CLASS))THEN                                     !00407
C          SCAN-GARBAGE                                                   !00407
          ASSIGN 99940 TO I99950                                          !00407
          GO TO 99950                                                     !00407
99940     CONTINUE                                                        !00407
        ELSEIF((TELSE).EQ.(CLASS))THEN                                    !00408
C          SCAN-PINV-OR-FORT                                              !00408
          ASSIGN 99938 TO I99939                                          !00408
          GO TO 99939                                                     !00408
99938     CONTINUE                                                        !00408
        ELSEIF((TTO).EQ.(CLASS))THEN                                      !00409
          CSAVE=CURSOR                                                    !00410
C          SCAN-PINV                                                      !00411
          ASSIGN 99936 TO I99937                                          !00411
          GO TO 99937                                                     !00411
99936     CONTINUE                                                        !00411
          IF(FOUND)THEN                                                   !00412
C            SCAN-PINV-OR-FORT                                            !00412
            ASSIGN 99935 TO I99939                                        !00412
            GO TO 99939                                                   !00412
99935       CONTINUE                                                      !00412
          ELSE                                                            !00413
            ERSTOP=ERSTOP+1                                               !00414
            ERRSTK(ERSTOP)=5                                              !00415
C            SAVE-ORIGINAL-STATEMENT                                      !00416
            ASSIGN 99933 TO I99934                                        !00416
            GO TO 99934                                                   !00416
99933       CONTINUE                                                      !00416
            LSFLX=CSAVE+15                                                !00417
            SFLX(CSAVE+1:LSFLX)='DUMMY-PROCEDURE'                         !00418
            CURSOR=CSAVE                                                  !00419
C            RESET-GET-CHARACTER                                          !00420
            ASSIGN 99932 TO I99945                                        !00420
            GO TO 99945                                                   !00420
99932       CONTINUE                                                      !00420
C            SCAN-PINV                                                    !00421
            ASSIGN 99931 TO I99937                                        !00421
            GO TO 99937                                                   !00421
99931       CONTINUE                                                      !00421
            ENDIF                                                         !00422
        ELSEIF((TCEXP).EQ.(CLASS))THEN                                    !00424
C          SCAN-CONTROL                                                   !00424
          ASSIGN 99930 TO I99947                                          !00424
          GO TO 99947                                                     !00424
99930     CONTINUE                                                        !00424
          ENDIF                                                           !00424
        IF(ERSTOP.GT.0)  CLASS=0                                          !00426
        LSTLEV=LEVEL                                                      !00427
      GO TO I99959                                                        !00428
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-CEXP                                                  !00429
99929 CONTINUE                                                            !00429
        IF(UTYPE(1).EQ.UEXP)THEN                                          !00430
          IF(CFIRST)THEN                                                  !00431
            IF(ENDIF)THEN                                                 !00432
C              PUT-ENDIF                                                  !00432
              ASSIGN 99928 TO I99990                                      !00432
              GO TO 99990                                                 !00432
99928         CONTINUE                                                    !00432
              ENDIF                                                       !00432
            CFIRST=.FALSE.                                                !00433
            LSST=UFIN(1)-USTART(1)+13                                     !00434
            SST(1:LSST)='      IF'//SFLX(USTART(1):UFIN(1))//'THEN'       !00435
          ELSE                                                            !00437
C            ABORT-ENDIF                                                  !00438
            ASSIGN 99927 TO I99991                                        !00438
            GO TO 99991                                                   !00438
99927       CONTINUE                                                      !00438
            LSST=UFIN(1)-USTART(1)+17                                     !00439
            SST(1:LSST)='      ELSEIF'//SFLX(USTART(1):UFIN(1))//'THEN'   !00440
            ENDIF                                                         !00441
        ELSE                                                              !00443
C          ABORT-ENDIF                                                    !00444
          ASSIGN 99926 TO I99991                                          !00444
          GO TO 99991                                                     !00444
99926     CONTINUE                                                        !00444
          LSST=10                                                         !00445
          SST(6:10)=' ELSE'                                               !00446
          ENDIF                                                           !00447
        STNO=FLXNO                                                        !00448
C        PUT-STATEMENT                                                    !00449
        ASSIGN 99924 TO I99925                                            !00449
        GO TO 99925                                                       !00449
99924   CONTINUE                                                          !00449
        AXSEQ=AFSEQ                                                       !00450
C        COMPLETE-ACTION                                                  !00451
        ASSIGN 99922 TO I99923                                            !00451
        GO TO 99923                                                       !00451
99922   CONTINUE                                                          !00451
      GO TO I99929                                                        !00452
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-CONDITIONAL                                           !00453
99921 CONTINUE                                                            !00453
        IF(ENDIF)THEN                                                     !00454
C          PUT-ENDIF                                                      !00454
          ASSIGN 99920 TO I99990                                          !00454
          GO TO 99990                                                     !00454
99920     CONTINUE                                                        !00454
          ENDIF                                                           !00454
C        PUT-CONTINUE-OR-COMMENT                                          !00455
        ASSIGN 99918 TO I99919                                            !00455
        GO TO 99919                                                       !00455
99918   CONTINUE                                                          !00455
        CFIRST=.TRUE.                                                     !00456
        TOP=TOP+2                                                         !00457
        STACK(TOP)=ACSEQ                                                  !00458
        STACK(TOP-1)=LINENO                                               !00459
        LEVEL=LEVEL+1                                                     !00460
      GO TO I99921                                                        !00461
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-DO                                                    !00462
99917 CONTINUE                                                            !00462
        IF(ENDIF)THEN                                                     !00463
C          PUT-ENDIF                                                      !00463
          ASSIGN 99916 TO I99990                                          !00463
          GO TO 99990                                                     !00463
99916     CONTINUE                                                        !00463
          ENDIF                                                           !00463
        LSST=UFIN(1)+8-USTART(1)                                          !00464
        SST(1:LSST)='      DO '//SFLX(USTART(1)+1:UFIN(1)-1)              !00465
        STNO=FLXNO                                                        !00466
C        PUT-STATEMENT                                                    !00467
        ASSIGN 99915 TO I99925                                            !00467
        GO TO 99925                                                       !00467
99915   CONTINUE                                                          !00467
        AXSEQ=ADSEQ                                                       !00468
C        COMPLETE-ACTION                                                  !00469
        ASSIGN 99914 TO I99923                                            !00469
        GO TO 99923                                                       !00469
99914   CONTINUE                                                          !00469
      GO TO I99917                                                        !00470
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-ELSE                                                  !00471
99913 CONTINUE                                                            !00471
C        ABORT-ENDIF                                                      !00472
        ASSIGN 99912 TO I99991                                            !00472
        GO TO 99991                                                       !00472
99912   CONTINUE                                                          !00472
        TOP=TOP-2                                                         !00473
        LSST=10                                                           !00474
        SST(6:10)=' ELSE'                                                 !00475
        STNO=FLXNO                                                        !00476
C        PUT-STATEMENT                                                    !00477
        ASSIGN 99911 TO I99925                                            !00477
        GO TO 99925                                                       !00477
99911   CONTINUE                                                          !00477
        AXSEQ=AFSEQ                                                       !00478
C        COMPLETE-ACTION                                                  !00479
        ASSIGN 99910 TO I99923                                            !00479
        GO TO 99923                                                       !00479
99910   CONTINUE                                                          !00479
      GO TO I99913                                                        !00480
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-END                                                   !00481
99909 CONTINUE                                                            !00481
        IF(ENDIF)THEN                                                     !00482
C          PUT-ENDIF                                                      !00482
          ASSIGN 99908 TO I99990                                          !00482
          GO TO 99990                                                     !00482
99908     CONTINUE                                                        !00482
          ENDIF                                                           !00482
C        SORT-TABLE                                                       !00483
        ASSIGN 99906 TO I99907                                            !00483
        GO TO 99907                                                       !00483
99906   CONTINUE                                                          !00483
C        PUT-COPY                                                         !00484
        ASSIGN 99904 TO I99905                                            !00484
        GO TO 99905                                                       !00484
99904   CONTINUE                                                          !00484
        IF (ENDFIL)   ERROR=25                                            !00485
        ENDPGM=.TRUE.                                                     !00486
      GO TO I99909                                                        !00487
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-EXEC                                                  !00488
99903 CONTINUE                                                            !00488
        IF((TFORT).EQ.(EXTYPE))THEN                                       !00490
C          PUT-COPY                                                       !00490
          ASSIGN 99902 TO I99905                                          !00490
          GO TO 99905                                                     !00490
99902     CONTINUE                                                        !00490
        ELSEIF((TIF).EQ.(EXTYPE))THEN                                     !00491
C          COMPILE-IF                                                     !00491
          ASSIGN 99900 TO I99901                                          !00491
          GO TO 99901                                                     !00491
99900     CONTINUE                                                        !00491
        ELSEIF((TUNLES).EQ.(EXTYPE))THEN                                  !00492
C          COMPILE-UNLESS                                                 !00492
          ASSIGN 99898 TO I99899                                          !00492
          GO TO 99899                                                     !00492
99898     CONTINUE                                                        !00492
        ELSEIF((TWHEN).EQ.(EXTYPE))THEN                                   !00493
C          COMPILE-WHEN                                                   !00493
          ASSIGN 99896 TO I99897                                          !00493
          GO TO 99897                                                     !00493
99896     CONTINUE                                                        !00493
        ELSEIF((TWHILE).EQ.(EXTYPE))THEN                                  !00494
C          COMPILE-WHILE                                                  !00494
          ASSIGN 99894 TO I99895                                          !00494
          GO TO 99895                                                     !00494
99894     CONTINUE                                                        !00494
        ELSEIF((TUNTIL).EQ.(EXTYPE))THEN                                  !00495
C          COMPILE-UNTIL                                                  !00495
          ASSIGN 99892 TO I99893                                          !00495
          GO TO 99893                                                     !00495
99892     CONTINUE                                                        !00495
        ELSEIF((TRWHIL).EQ.(EXTYPE))THEN                                  !00496
C          COMPILE-RWHILE                                                 !00496
          ASSIGN 99890 TO I99891                                          !00496
          GO TO 99891                                                     !00496
99890     CONTINUE                                                        !00496
        ELSEIF((TRUNTL).EQ.(EXTYPE))THEN                                  !00497
C          COMPILE-RUNTIL                                                 !00497
          ASSIGN 99888 TO I99889                                          !00497
          GO TO 99889                                                     !00497
99888     CONTINUE                                                        !00497
        ELSEIF((TINVOK).EQ.(EXTYPE))THEN                                  !00498
C          COMPILE-INVOKE                                                 !00498
          ASSIGN 99886 TO I99887                                          !00498
          GO TO 99887                                                     !00498
99886     CONTINUE                                                        !00498
        ELSEIF((TCOND).EQ.(EXTYPE))THEN                                   !00499
C          COMPILE-CONDITIONAL                                            !00499
          ASSIGN 99885 TO I99921                                          !00499
          GO TO 99921                                                     !00499
99885     CONTINUE                                                        !00499
        ELSEIF((TSELCT).EQ.(EXTYPE))THEN                                  !00500
C          COMPILE-SELECT                                                 !00500
          ASSIGN 99883 TO I99884                                          !00500
          GO TO 99884                                                     !00500
99883     CONTINUE                                                        !00500
        ELSEIF((TDO).EQ.(EXTYPE))THEN                                     !00501
C          COMPILE-DO                                                     !00501
          ASSIGN 99882 TO I99917                                          !00501
          GO TO 99917                                                     !00501
99882     CONTINUE                                                        !00501
          ENDIF                                                           !00501
      GO TO I99903                                                        !00503
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-FORTRAN                                               !00504
99881 CONTINUE                                                            !00504
        IF(ENDIF)THEN                                                     !00505
C          PUT-ENDIF                                                      !00505
          ASSIGN 99880 TO I99990                                          !00505
          GO TO 99990                                                     !00505
99880     CONTINUE                                                        !00505
          ENDIF                                                           !00505
        LSST=UFIN(2)-USTART(2)+7                                          !00506
        SST(6:LSST)=' '//SFLX(USTART(2):UFIN(2))                          !00507
        STNO=FLXNO                                                        !00508
        IND=INDEX(SST(1:LSST),'!')                                        !00509
        IF (IND.EQ.0) IND=LSST+1                                          !00510
        IF(STNO.NE.0)THEN                                                 !00512
C          PUT-STATEMENT                                                  !00512
          ASSIGN 99879 TO I99925                                          !00512
          GO TO 99925                                                     !00512
99879     CONTINUE                                                        !00512
        ELSEIF(SST(7:IND-1).NE.'CONTINUE')THEN                            !00513
C          PUT-STATEMENT                                                  !00513
          ASSIGN 99878 TO I99925                                          !00513
          GO TO 99925                                                     !00513
99878     CONTINUE                                                        !00513
        ELSEIF(IND.NE.LSST+1)THEN                                         !00514
          CALL PUTF(SLINE(2:6),SST(IND:LSST))                             !00514
          ENDIF                                                           !00514
      GO TO I99881                                                        !00516
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-IF                                                    !00517
99901 CONTINUE                                                            !00517
        IF(ENDIF)THEN                                                     !00518
C          PUT-ENDIF                                                      !00518
          ASSIGN 99877 TO I99990                                          !00518
          GO TO 99990                                                     !00518
99877     CONTINUE                                                        !00518
          ENDIF                                                           !00518
        IF(NUNITS.EQ.2.AND.UTYPE(2).EQ.UFORT)THEN                         !00519
C          PUT-COPY                                                       !00519
          ASSIGN 99876 TO I99905                                          !00519
          GO TO 99905                                                     !00519
99876     CONTINUE                                                        !00519
        ELSE                                                              !00520
          LSST=UFIN(1)-USTART(1)+13                                       !00521
          SST(1:LSST)='      IF'//SFLX(USTART(1):UFIN(1))//'THEN'         !00522
          STNO=FLXNO                                                      !00523
C          PUT-STATEMENT                                                  !00524
          ASSIGN 99875 TO I99925                                          !00524
          GO TO 99925                                                     !00524
99875     CONTINUE                                                        !00524
          AXSEQ=AFSEQ                                                     !00525
C          COMPLETE-ACTION                                                !00526
          ASSIGN 99874 TO I99923                                          !00526
          GO TO 99923                                                     !00526
99874     CONTINUE                                                        !00526
          ENDIF                                                           !00527
      GO TO I99901                                                        !00528
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-INVOKE                                                !00529
99887 CONTINUE                                                            !00529
        IF(ENDIF)THEN                                                     !00530
C          PUT-ENDIF                                                      !00530
          ASSIGN 99873 TO I99990                                          !00530
          GO TO 99990                                                     !00530
99873     CONTINUE                                                        !00530
          ENDIF                                                           !00530
C        FIND-ENTRY                                                       !00531
        ASSIGN 99871 TO I99872                                            !00531
        GO TO 99872                                                       !00531
99871   CONTINUE                                                          !00531
        CALL PUTF(SLINE(2:6),'C'//BLANKS(1:2*FLEVEL+6)//                  !00532
     1  SFLX(USTART(J):LSFLX))                                            !00533
        ENTNO=STACK(PENT+1)                                               !00534
        RETNO=NEWNO(0)                                                    !00535
        MAX=MAX-2                                                         !00536
        STACK(MAX+1)=STACK(PENT+3)                                        !00537
        STACK(PENT+3)=MAX+1                                               !00538
        STACK(MAX+2)=LINENO                                               !00539
        LSST=28                                                           !00540
        SST(6:13)=' ASSIGN '                                              !00541
        SST(19:23)=' TO I'                                                !00542
        WRITE(SST(14:18),'(I5)')RETNO                                     !00543
        WRITE(SST(24:28),'(I5)')ENTNO                                     !00544
        STNO=FLXNO                                                        !00545
C        PUT-STATEMENT                                                    !00546
        ASSIGN 99870 TO I99925                                            !00546
        GO TO 99925                                                       !00546
99870   CONTINUE                                                          !00546
        LSST=17                                                           !00547
        SST(6:12)=' GO TO '                                               !00548
        WRITE(SST(13:17),'(I5)')ENTNO                                     !00549
C        PUT-STATEMENT                                                    !00550
        ASSIGN 99869 TO I99925                                            !00550
        GO TO 99925                                                       !00550
99869   CONTINUE                                                          !00550
        STNO=RETNO                                                        !00551
C        PUT-CONTINUE                                                     !00552
        ASSIGN 99867 TO I99868                                            !00552
        GO TO 99868                                                       !00552
99867   CONTINUE                                                          !00552
      GO TO I99887                                                        !00553
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-RUNTIL                                                !00554
99889 CONTINUE                                                            !00554
        IF(ENDIF)THEN                                                     !00555
C          PUT-ENDIF                                                      !00555
          ASSIGN 99866 TO I99990                                          !00555
          GO TO 99990                                                     !00555
99866     CONTINUE                                                        !00555
          ENDIF                                                           !00555
        ENTNO=NEWNO(0)                                                    !00556
        SST(1:20)='      IF(.TRUE.)GOTO'                                  !00557
        WRITE(SST(21:25),'(I5)')ENTNO                                     !00558
        LSST=25                                                           !00559
        STNO=FLXNO                                                        !00560
C        PUT-STATEMENT                                                    !00561
        ASSIGN 99865 TO I99925                                            !00561
        GO TO 99925                                                       !00561
99865   CONTINUE                                                          !00561
        LSST=UFIN(1)-USTART(1)+22                                         !00562
        SST(6:LSST)=' DO WHILE(.NOT.'//SFLX(USTART(1):UFIN(1))//')'       !00563
C        PUT-STATEMENT                                                    !00564
        ASSIGN 99864 TO I99925                                            !00564
        GO TO 99925                                                       !00564
99864   CONTINUE                                                          !00564
        STNO=ENTNO                                                        !00565
        FLEVEL=FLEVEL+1                                                   !00566
C        PUT-CONTINUE                                                     !00567
        ASSIGN 99863 TO I99868                                            !00567
        GO TO 99868                                                       !00567
99863   CONTINUE                                                          !00567
        FLEVEL=FLEVEL-1                                                   !00568
        AXSEQ=ADSEQ                                                       !00569
C        COMPLETE-ACTION                                                  !00570
        ASSIGN 99862 TO I99923                                            !00570
        GO TO 99923                                                       !00570
99862   CONTINUE                                                          !00570
      GO TO I99889                                                        !00571
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-RWHILE                                                !00572
99891 CONTINUE                                                            !00572
        IF(ENDIF)THEN                                                     !00573
C          PUT-ENDIF                                                      !00573
          ASSIGN 99861 TO I99990                                          !00573
          GO TO 99990                                                     !00573
99861     CONTINUE                                                        !00573
          ENDIF                                                           !00573
        ENTNO=NEWNO(0)                                                    !00574
        SST(1:20)='      IF(.TRUE.)GOTO'                                  !00575
        WRITE(SST(21:25),'(I5)')ENTNO                                     !00576
        LSST=25                                                           !00577
        STNO=FLXNO                                                        !00578
C        PUT-STATEMENT                                                    !00579
        ASSIGN 99860 TO I99925                                            !00579
        GO TO 99925                                                       !00579
99860   CONTINUE                                                          !00579
        LSST=UFIN(1)-USTART(1)+15                                         !00580
        SST(6:LSST)=' DO WHILE'//SFLX(USTART(1):UFIN(1))                  !00581
C        PUT-STATEMENT                                                    !00582
        ASSIGN 99859 TO I99925                                            !00582
        GO TO 99925                                                       !00582
99859   CONTINUE                                                          !00582
        STNO=ENTNO                                                        !00583
        FLEVEL=FLEVEL+1                                                   !00584
C        PUT-CONTINUE                                                     !00585
        ASSIGN 99858 TO I99868                                            !00585
        GO TO 99868                                                       !00585
99858   CONTINUE                                                          !00585
        FLEVEL=FLEVEL-1                                                   !00586
        AXSEQ=ADSEQ                                                       !00587
C        COMPLETE-ACTION                                                  !00588
        ASSIGN 99857 TO I99923                                            !00588
        GO TO 99923                                                       !00588
99857   CONTINUE                                                          !00588
      GO TO I99891                                                        !00589
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-SELECT                                                !00590
99884 CONTINUE                                                            !00590
        IF(ENDIF)THEN                                                     !00591
C          PUT-ENDIF                                                      !00591
          ASSIGN 99856 TO I99990                                          !00591
          GO TO 99990                                                     !00591
99856     CONTINUE                                                        !00591
          ENDIF                                                           !00591
C        PUT-CONTINUE-OR-COMMENT                                          !00592
        ASSIGN 99855 TO I99919                                            !00592
        GO TO 99919                                                       !00592
99855   CONTINUE                                                          !00592
        CFIRST=.TRUE.                                                     !00593
        L=(UFIN(1)-USTART(1))/NCHPWD+4                                    !00594
        TOP=TOP+L+1                                                       !00595
        IF(TOP+SAFETY.LT.MAX)THEN                                         !00596
          STACK(TOP)=ASSEQ                                                !00597
          STACK(TOP-1)=LINENO                                             !00598
          STACK(TOP-2)=L                                                  !00599
          I=TOP-L                                                         !00600
          IP=4*I                                                          !00601
          STACK(I)=UFIN(1)-USTART(1)+1                                    !00602
          CSTAK(IP+1:IP+STACK(I))=SFLX(USTART(1):UFIN(1))                 !00603
        ELSE                                                              !00605
C          GIVE-UP                                                        !00605
          ASSIGN 99853 TO I99854                                          !00605
          GO TO 99854                                                     !00605
99853     CONTINUE                                                        !00605
          ENDIF                                                           !00605
        LEVEL=LEVEL+1                                                     !00606
      GO TO I99884                                                        !00607
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-SEQ-FIN                                               !00608
99852 CONTINUE                                                            !00608
        IF(ENDIF)THEN                                                     !00609
C          PUT-ENDIF                                                      !00609
          ASSIGN 99851 TO I99990                                          !00609
          GO TO 99990                                                     !00609
99851     CONTINUE                                                        !00609
          ENDIF                                                           !00609
C        PUT-CONTINUE-OR-COMMENT                                          !00610
        ASSIGN 99850 TO I99919                                            !00610
        GO TO 99919                                                       !00610
99850   CONTINUE                                                          !00610
        STNO=FLXNO                                                        !00611
        IF(STNO.NE.0.)THEN                                                !00612
C          PUT-CONTINUE                                                   !00612
          ASSIGN 99849 TO I99868                                          !00612
          GO TO 99868                                                     !00612
99849     CONTINUE                                                        !00612
          ENDIF                                                           !00612
C        POP-STACK                                                        !00613
        ASSIGN 99847 TO I99848                                            !00613
        GO TO 99848                                                       !00613
99847   CONTINUE                                                          !00613
        LEVEL=LEVEL-1                                                     !00614
      GO TO I99852                                                        !00615
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-SEXP                                                  !00616
99846 CONTINUE                                                            !00616
        IF(UTYPE(1).EQ.UEXP)THEN                                          !00617
          IF(CFIRST)THEN                                                  !00618
            IF(ENDIF)THEN                                                 !00619
C              PUT-ENDIF                                                  !00619
              ASSIGN 99845 TO I99990                                      !00619
              GO TO 99990                                                 !00619
99845         CONTINUE                                                    !00619
              ENDIF                                                       !00619
            CFIRST=.FALSE.                                                !00620
            I=TOP-STACK(TOP-2)                                            !00621
            IP=4*I                                                        !00622
            LSST=UFIN(1)-USTART(1)+STACK(I)+19                            !00623
            SST(1:LSST)='      IF('//SFLX(USTART(1):UFIN(1))//            !00624
     1      '.EQ.'//CSTAK(IP+1:IP+STACK(I))//')THEN'                      !00625
          ELSE                                                            !00627
C            ABORT-ENDIF                                                  !00628
            ASSIGN 99844 TO I99991                                        !00628
            GO TO 99991                                                   !00628
99844       CONTINUE                                                      !00628
            I=TOP-STACK(TOP-2)                                            !00629
            IP=4*I                                                        !00630
            LSST=UFIN(1)-USTART(1)+STACK(I)+23                            !00631
            SST(1:LSST)='      ELSEIF('//SFLX(USTART(1):UFIN(1))//        !00632
     1      '.EQ.'//CSTAK(IP+1:IP+STACK(I))//')THEN'                      !00633
            ENDIF                                                         !00634
        ELSE                                                              !00636
C          ABORT-ENDIF                                                    !00637
          ASSIGN 99843 TO I99991                                          !00637
          GO TO 99991                                                     !00637
99843     CONTINUE                                                        !00637
          LSST=10                                                         !00638
          SST(6:10)=' ELSE'                                               !00639
          ENDIF                                                           !00640
        STNO=FLXNO                                                        !00641
C        PUT-STATEMENT                                                    !00642
        ASSIGN 99842 TO I99925                                            !00642
        GO TO 99925                                                       !00642
99842   CONTINUE                                                          !00642
        AXSEQ=AFSEQ                                                       !00643
C        COMPLETE-ACTION                                                  !00644
        ASSIGN 99841 TO I99923                                            !00644
        GO TO 99923                                                       !00644
99841   CONTINUE                                                          !00644
      GO TO I99846                                                        !00645
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-SIMPLE-FIN                                            !00646
99840 CONTINUE                                                            !00646
        IF(ENDIF)THEN                                                     !00647
C          PUT-ENDIF                                                      !00647
          ASSIGN 99839 TO I99990                                          !00647
          GO TO 99990                                                     !00647
99839     CONTINUE                                                        !00647
          ENDIF                                                           !00647
        IF((AFSEQ).EQ.(ACTION))THEN                                       !00649
          STNO=FLXNO                                                      !00650
C          STORE-ENDIF                                                    !00651
          ASSIGN 99837 TO I99838                                          !00651
          GO TO 99838                                                     !00651
99837     CONTINUE                                                        !00651
        ELSEIF((ADSEQ).EQ.(ACTION))THEN                                   !00653
          STNO=FLXNO                                                      !00654
          IF(STNO.NE.0)THEN                                               !00655
C            PUT-CONTINUE                                                 !00655
            ASSIGN 99836 TO I99868                                        !00655
            GO TO 99868                                                   !00655
99836       CONTINUE                                                      !00655
            ENDIF                                                         !00655
          LSST=11                                                         !00656
          SST(1:11)='      ENDDO'                                         !00657
C          PUT-STATEMENT                                                  !00658
          ASSIGN 99835 TO I99925                                          !00658
          GO TO 99925                                                     !00658
99835     CONTINUE                                                        !00658
        ELSEIF((ARSEQ).EQ.(ACTION))THEN                                   !00660
          STNO=FLXNO                                                      !00661
          IF(STNO.NE.0)THEN                                               !00662
C            PUT-CONTINUE                                                 !00662
            ASSIGN 99834 TO I99868                                        !00662
            GO TO 99868                                                   !00662
99834       CONTINUE                                                      !00662
            ENDIF                                                         !00662
          ENDIF                                                           !00663
        FLEVEL=FLEVEL-1                                                   !00665
        LEVEL=LEVEL-1                                                     !00666
        TOP=TOP-2                                                         !00667
      GO TO I99840                                                        !00668
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-TO                                                    !00669
99833 CONTINUE                                                            !00669
        IF(ENDIF)THEN                                                     !00670
C          PUT-ENDIF                                                      !00670
          ASSIGN 99832 TO I99990                                          !00670
          GO TO 99990                                                     !00670
99832     CONTINUE                                                        !00670
          ENDIF                                                           !00670
        CALL PUTF('     ','C')                                            !00671
        CALL PUTF('     ','C'//SDASH)                                     !00672
        CALL PUTF('     ','C')                                            !00673
        CALL PUTF(SLINE(2:6),'C      '//SFLX(7:LSFLX))                    !00674
C        FIND-ENTRY                                                       !00675
        ASSIGN 99831 TO I99872                                            !00675
        GO TO 99872                                                       !00675
99831   CONTINUE                                                          !00675
        IF(STACK(PENT+2).NE.0)THEN                                        !00676
          ERROR=26                                                        !00677
          MLINE=STACK(PENT+2)                                             !00678
          ENTNO=NEWNO(0)                                                  !00679
        ELSE                                                              !00681
          ENTNO=STACK(PENT+1)                                             !00682
          STACK(PENT+2)=LINENO                                            !00683
          ENDIF                                                           !00684
        STNO=FLXNO                                                        !00685
        IF(STNO.NE.0)THEN                                                 !00686
C          PUT-CONTINUE                                                   !00686
          ASSIGN 99830 TO I99868                                          !00686
          GO TO 99868                                                     !00686
99830     CONTINUE                                                        !00686
          ENDIF                                                           !00686
        STNO=ENTNO                                                        !00687
C        PUT-CONTINUE                                                     !00688
        ASSIGN 99829 TO I99868                                            !00688
        GO TO 99868                                                       !00688
99829   CONTINUE                                                          !00688
        TOP=TOP+2                                                         !00689
        STACK(TOP)=AGRET                                                  !00690
        STACK(TOP-1)=ENTNO                                                !00691
        UTYPE(1)=0                                                        !00692
        AXSEQ=ARSEQ                                                       !00693
C        COMPLETE-ACTION                                                  !00694
        ASSIGN 99828 TO I99923                                            !00694
        GO TO 99923                                                       !00694
99828   CONTINUE                                                          !00694
      GO TO I99833                                                        !00695
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-UNLESS                                                !00696
99899 CONTINUE                                                            !00696
        IF(ENDIF)THEN                                                     !00697
C          PUT-ENDIF                                                      !00697
          ASSIGN 99827 TO I99990                                          !00697
          GO TO 99990                                                     !00697
99827     CONTINUE                                                        !00697
          ENDIF                                                           !00697
        IF(NUNITS.EQ.2.AND.UTYPE(2).EQ.UFORT)THEN                         !00698
          LSST=+UFIN(1)+UFIN(2)-USTART(1)-USTART(2)+17                    !00699
          SST(6:LSST)=' IF(.NOT.'//SFLX(USTART(1):UFIN(1))//              !00700
     1    ')'//SFLX(USTART(2):UFIN(2))                                    !00701
          STNO=FLXNO                                                      !00702
C          PUT-STATEMENT                                                  !00703
          ASSIGN 99826 TO I99925                                          !00703
          GO TO 99925                                                     !00703
99826     CONTINUE                                                        !00703
        ELSE                                                              !00705
          LSST=UFIN(1)-USTART(1)+20                                       !00706
          SST(1:LSST)='      IF(.NOT.'//SFLX(USTART(1):UFIN(1))//')THEN'  !00707
          STNO=FLXNO                                                      !00708
C          PUT-STATEMENT                                                  !00709
          ASSIGN 99825 TO I99925                                          !00709
          GO TO 99925                                                     !00709
99825     CONTINUE                                                        !00709
          AXSEQ=AFSEQ                                                     !00710
C          COMPLETE-ACTION                                                !00711
          ASSIGN 99824 TO I99923                                          !00711
          GO TO 99923                                                     !00711
99824     CONTINUE                                                        !00711
          ENDIF                                                           !00712
      GO TO I99899                                                        !00713
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-UNTIL                                                 !00714
99893 CONTINUE                                                            !00714
        IF(ENDIF)THEN                                                     !00715
C          PUT-ENDIF                                                      !00715
          ASSIGN 99823 TO I99990                                          !00715
          GO TO 99990                                                     !00715
99823     CONTINUE                                                        !00715
          ENDIF                                                           !00715
        LSST=UFIN(1)-USTART(1)+22                                         !00716
        SST(6:LSST)=' DO WHILE(.NOT.'//SFLX(USTART(1):UFIN(1))//')'       !00717
        STNO=FLXNO                                                        !00718
C        PUT-STATEMENT                                                    !00719
        ASSIGN 99822 TO I99925                                            !00719
        GO TO 99925                                                       !00719
99822   CONTINUE                                                          !00719
        AXSEQ=ADSEQ                                                       !00720
C        COMPLETE-ACTION                                                  !00721
        ASSIGN 99821 TO I99923                                            !00721
        GO TO 99923                                                       !00721
99821   CONTINUE                                                          !00721
      GO TO I99893                                                        !00722
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-WHEN                                                  !00723
99897 CONTINUE                                                            !00723
        IF(ENDIF)THEN                                                     !00724
C          PUT-ENDIF                                                      !00724
          ASSIGN 99820 TO I99990                                          !00724
          GO TO 99990                                                     !00724
99820     CONTINUE                                                        !00724
          ENDIF                                                           !00724
        TOP=TOP+2                                                         !00725
        STACK(TOP-1)=LINENO                                               !00726
        STACK(TOP)=AELSE                                                  !00727
        LSST=UFIN(1)-USTART(1)+13                                         !00728
        SST(6:LSST)=' IF'//SFLX(USTART(1):UFIN(1))//'THEN'                !00729
        STNO=FLXNO                                                        !00730
C        PUT-STATEMENT                                                    !00731
        ASSIGN 99819 TO I99925                                            !00731
        GO TO 99925                                                       !00731
99819   CONTINUE                                                          !00731
        AXSEQ=AFSEQ                                                       !00732
C        COMPLETE-ACTION                                                  !00733
        ASSIGN 99818 TO I99923                                            !00733
        GO TO 99923                                                       !00733
99818   CONTINUE                                                          !00733
      GO TO I99897                                                        !00734
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPILE-WHILE                                                 !00735
99895 CONTINUE                                                            !00735
        IF(ENDIF)THEN                                                     !00736
C          PUT-ENDIF                                                      !00736
          ASSIGN 99817 TO I99990                                          !00736
          GO TO 99990                                                     !00736
99817     CONTINUE                                                        !00736
          ENDIF                                                           !00736
        LSST=UFIN(1)-USTART(1)+15                                         !00737
        SST(6:LSST)=' DO WHILE'//SFLX(USTART(1):UFIN(1))                  !00738
        STNO=FLXNO                                                        !00739
C        PUT-STATEMENT                                                    !00740
        ASSIGN 99816 TO I99925                                            !00740
        GO TO 99925                                                       !00740
99816   CONTINUE                                                          !00740
        AXSEQ=ADSEQ                                                       !00741
C        COMPLETE-ACTION                                                  !00742
        ASSIGN 99815 TO I99923                                            !00742
        GO TO 99923                                                       !00742
99815   CONTINUE                                                          !00742
      GO TO I99895                                                        !00743
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO COMPLETE-ACTION                                               !00744
99923 CONTINUE                                                            !00744
        IF(NUNITS.EQ.1)THEN                                               !00745
C          PUSH-FINSEQ                                                    !00745
          ASSIGN 99813 TO I99814                                          !00745
          GO TO 99814                                                     !00745
99813     CONTINUE                                                        !00745
        ELSE                                                              !00746
          FLEVEL=FLEVEL+1                                                 !00747
          IF(UTYPE(2).EQ.UPINV)THEN                                       !00749
C            COMPILE-INVOKE                                               !00749
            ASSIGN 99812 TO I99887                                        !00749
            GO TO 99887                                                   !00749
99812       CONTINUE                                                      !00749
          ELSEIF(SFLX(USTART(2):UFIN(2)).NE.'CONTINUE')THEN               !00750
C            COMPILE-FORTRAN                                              !00750
            ASSIGN 99811 TO I99881                                        !00750
            GO TO 99881                                                   !00750
99811       CONTINUE                                                      !00750
            ENDIF                                                         !00750
          IF((AFSEQ).EQ.(AXSEQ))THEN                                      !00753
            STNO=0                                                        !00754
C            STORE-ENDIF                                                  !00755
            ASSIGN 99810 TO I99838                                        !00755
            GO TO 99838                                                   !00755
99810       CONTINUE                                                      !00755
          ELSEIF((ADSEQ).EQ.(AXSEQ))THEN                                  !00757
            STNO=FLXNO                                                    !00758
            IF(STNO.NE.0)THEN                                             !00759
C              PUT-CONTINUE                                               !00759
              ASSIGN 99809 TO I99868                                      !00759
              GO TO 99868                                                 !00759
99809         CONTINUE                                                    !00759
              ENDIF                                                       !00759
            LSST=11                                                       !00760
            SST(1:11)='      ENDDO'                                       !00761
C            PUT-STATEMENT                                                !00762
            ASSIGN 99808 TO I99925                                        !00762
            GO TO 99925                                                   !00762
99808       CONTINUE                                                      !00762
            ENDIF                                                         !00763
          FLEVEL=FLEVEL-1                                                 !00765
          ENDIF                                                           !00766
      GO TO I99923                                                        !00767
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO FIND-ENTRY                                                    !00768
99872 CONTINUE                                                            !00768
        IF(UTYPE(1).EQ.UPINV)THEN                                         !00769
          J=1                                                             !00769
        ELSE                                                              !00770
          J=2                                                             !00770
          ENDIF                                                           !00770
        LSPINV=UFIN(J)-USTART(J)+1                                        !00771
        SPINV(1:LSPINV)=SFLX(USTART(J):UFIN(J))                           !00772
        IF(SPINV(1:LSPINV).EQ.'DUMMY-PROCEDURE')THEN                      !00773
          PENT=PDUMMY                                                     !00774
          STACK(PENT+2)=0                                                 !00775
        ELSE                                                              !00777
          P=MAXSTK-HASH(SPINV(1:LSPINV),PRIME)                            !00778
          FOUND=.FALSE.                                                   !00779
          IF(.NOT.(STACK(P).EQ.0))THEN                                    !00780
            IF(.TRUE.)GOTO99807                                           !00781
            DO WHILE(.NOT.(STACK(P).EQ.0.OR.FOUND))                       !00781
99807         CONTINUE                                                    !00781
              P=STACK(P)                                                  !00782
              IP=4*(P+4)                                                  !00783
              IF (SPINV(1:LSPINV).EQ.CSTAK(IP+1:IP+STACK(P+4))) FOUND=.T  !00784
     1RUE.                                                                !00784
              ENDDO                                                       !00785
            ENDIF                                                         !00786
          IF(FOUND)THEN                                                   !00787
            PENT=P                                                        !00787
          ELSE                                                            !00788
            TMAX=MAX-(6+(LSPINV+NCHPWD-1)/NCHPWD)                         !00789
            IF(TMAX.LE.TOP+SAFETY)THEN                                    !00790
              PENT=PDUMMY                                                 !00791
              STACK(PENT+2)=0                                             !00792
            ELSE                                                          !00794
              MAX=TMAX                                                    !00795
              PENT=MAX+2                                                  !00796
              STACK(PENT)=0                                               !00797
              STACK(P)=PENT                                               !00798
              STACK(PENT+1)=NEWNO(0)                                      !00799
              STACK(PENT+2)=0                                             !00800
              STACK(PENT+3)=0                                             !00801
              IP=4*(PENT+4)                                               !00802
              STACK(PENT+4)=LSPINV                                        !00803
              CSTAK(IP+1:IP+LSPINV)=SPINV(1:LSPINV)                       !00804
              ENDIF                                                       !00805
            ENDIF                                                         !00806
          ENDIF                                                           !00807
      GO TO I99872                                                        !00808
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO GENERATE-RETURN-FROM-PROC                                     !00809
99806 CONTINUE                                                            !00809
        LSST=18                                                           !00810
        SST(6:13)=' GO TO I'                                              !00811
        WRITE(SST(14:18),'(I5)')STACK(TOP-1)                              !00812
        STNO=FLXNO                                                        !00813
C        PUT-STATEMENT                                                    !00814
        ASSIGN 99805 TO I99925                                            !00814
        GO TO 99925                                                       !00814
99805   CONTINUE                                                          !00814
        TOP=TOP-2                                                         !00815
      GO TO I99806                                                        !00816
C                                                                         !00817
C-------------------------------------------------------------------------!00818
C                                                                         !00819
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO GET-CHARACTER                                                 !00820
99804 CONTINUE                                                            !00820
C                                                                         !00821
        CURSOR=CURSOR+1                                                   !00822
        IF(CURSOR.GT.LSFLX)THEN                                           !00823
          CHTYPE=TEOL                                                     !00823
        ELSE                                                              !00824
          CH = SFLX(CURSOR:CURSOR)                                        !00825
C       CHTYPE = CHTYP(ICH)                     !TG 4/1/82                !00826
          CHTYPE = CHTYP(ICHAR(CH))                                       !00827
          ENDIF                                                           !00828
      GO TO I99804                                                        !00829
C                                                                         !00830
C-------------------------------------------------------------------------!00831
C                                                                         !00832
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO GIVE-UP                                                       !00833
99854 CONTINUE                                                            !00833
        CALL PUTZ(SGUP1,ERRCL)                                            !00834
        CALL PUTZ(SGUP2,ERRCL)                                            !00835
        CALL CLOSEF(MINCNT,-1)                                            !00836
      GO TO I99854                                                        !00837
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO INSERT-CONDITIONAL                                            !00838
99971 CONTINUE                                                            !00838
C        PREPARE-FOR-INSERTION                                            !00839
        ASSIGN 99802 TO I99803                                            !00839
        GO TO 99803                                                       !00839
99802   CONTINUE                                                          !00839
        LSFLX=17                                                          !00840
        SFLX(1:LSFLX)='      CONDITIONAL'                                 !00841
        CALL PUTZ(SICOND,ERRCL)                                           !00842
      GO TO I99971                                                        !00843
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO INSERT-ELSE                                                   !00844
99983 CONTINUE                                                            !00844
C        PREPARE-FOR-INSERTION                                            !00845
        ASSIGN 99801 TO I99803                                            !00845
        GO TO 99803                                                       !00845
99801   CONTINUE                                                          !00845
        LSFLX=19                                                          !00846
        SFLX(1:LSFLX)='      ELSE CONTINUE'                               !00847
        WRITE(SIELSE(40:44),'(I5)')STACK(TOP-1)                           !00848
        CALL PUTZ(SIELSE,ERRCL)                                           !00849
      GO TO I99983                                                        !00850
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO INSERT-FIN                                                    !00851
99985 CONTINUE                                                            !00851
C        PREPARE-FOR-INSERTION                                            !00852
        ASSIGN 99800 TO I99803                                            !00852
        GO TO 99803                                                       !00852
99800   CONTINUE                                                          !00852
        LSFLX=9                                                           !00853
        SFLX(1:LSFLX)='      FIN'                                         !00854
        IF(STACK(TOP-1).EQ.0)THEN                                         !00855
          CALL PUTZ(SIFIN2,ERRCL)                                         !00855
        ELSE                                                              !00856
          WRITE(SIFIN(39:43),'(I5)')STACK(TOP-1)                          !00857
          CALL PUTZ(SIFIN,ERRCL)                                          !00858
          ENDIF                                                           !00859
      GO TO I99985                                                        !00860
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO INSERT-TO-DUMMY-PROCEDURE                                     !00861
99978 CONTINUE                                                            !00861
C        PREPARE-FOR-INSERTION                                            !00862
        ASSIGN 99799 TO I99803                                            !00862
        GO TO 99803                                                       !00862
99799   CONTINUE                                                          !00862
        LSFLX=24                                                          !00863
        SFLX(1:LSFLX)='      TO DUMMY-PROCEDURE'                          !00864
        CALL PUTZ(SITODM,ERRCL)                                           !00865
      GO TO I99978                                                        !00866
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO INSERT-WHEN                                                   !00867
99980 CONTINUE                                                            !00867
C        PREPARE-FOR-INSERTION                                            !00868
        ASSIGN 99798 TO I99803                                            !00868
        GO TO 99803                                                       !00868
99798   CONTINUE                                                          !00868
        LSFLX=24                                                          !00869
        SFLX(1:LSFLX)='      WHEN (.TRUE.) STOP'                          !00870
        CALL PUTZ(SIWHEN,ERRCL)                                           !00871
      GO TO I99980                                                        !00872
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO INSERT-WHEN-OR-FIN                                            !00873
99976 CONTINUE                                                            !00873
        IF(TOP.LE.7)THEN                                                  !00875
C          INSERT-WHEN                                                    !00875
          ASSIGN 99797 TO I99980                                          !00875
          GO TO 99980                                                     !00875
99797     CONTINUE                                                        !00875
        ELSEIF(STACK(TOP-6).EQ.AELSE)THEN                                 !00876
C          INSERT-FIN                                                     !00876
          ASSIGN 99796 TO I99985                                          !00876
          GO TO 99985                                                     !00876
99796     CONTINUE                                                        !00876
        ELSE                                                              !00877
C          INSERT-WHEN                                                    !00877
          ASSIGN 99795 TO I99980                                          !00877
          GO TO 99980                                                     !00877
99795     CONTINUE                                                        !00877
          ENDIF                                                           !00877
      GO TO I99976                                                        !00879
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO LIST-BLANK-LINE                                               !00880
99961 CONTINUE                                                            !00880
        LSTLEV=LEVEL                                                      !00881
        LSLIST=3*LSTLEV+6                                                 !00882
        IF(LSTLEV.EQ.0.OR.LSLIST.GT.WWIDTH)THEN                           !00883
          CALL PUT(SBLN,' ',LISTCL)                                       !00883
        ELSE                                                              !00884
          SLIST(1:6)=' '                                                  !00885
          DO I=7,LSLIST,3                                                 !00886
            SLIST(I:I+2)='.  '                                            !00886
            ENDDO                                                         !00886
          CALL PUT(SBLN,SLIST(1:LSLIST),LISTCL)                           !00887
          ENDIF                                                           !00888
        SBLN=' '                                                          !00889
      GO TO I99961                                                        !00890
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO LIST-COMMENT-LINE                                             !00891
99794 CONTINUE                                                            !00891
        CURSOR=1                                                          !00892
C        RESET-GET-CHARACTER                                              !00893
        ASSIGN 99793 TO I99945                                            !00893
        GO TO 99945                                                       !00893
99793   CONTINUE                                                          !00893
        INDENT=.TRUE.                                                     !00894
        I=2                                                               !00895
        IF(.TRUE.)GOTO99792                                               !00896
        DO WHILE(I.LE.6.AND.INDENT)                                       !00896
99792     CONTINUE                                                        !00896
C          GET-CHARACTER                                                  !00897
          ASSIGN 99791 TO I99804                                          !00897
          GO TO 99804                                                     !00897
99791     CONTINUE                                                        !00897
          IF (CHTYPE.NE.TBLANK.AND.CHTYPE.NE.TEOL) INDENT=.FALSE.         !00898
          I=I+1                                                           !00899
          ENDDO                                                           !00900
        IF(INDENT)THEN                                                    !00901
          LSTLEV=LEVEL                                                    !00902
          CLASS=0                                                         !00903
C          LIST-FLEX                                                      !00904
          ASSIGN 99790 TO I99987                                          !00904
          GO TO 99987                                                     !00904
99790     CONTINUE                                                        !00904
        ELSE                                                              !00906
          CALL PUT(SLINE(2:6),SFLX(1:LSFLX),LISTCL)                       !00906
          ENDIF                                                           !00906
      GO TO I99794                                                        !00907
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO LIST-DASHES                                                   !00908
99789 CONTINUE                                                            !00908
        CALL PUTZ(' ',LISTCL)                                             !00909
        CALL PUTZ(SDASH,LISTCL)                                           !00910
        CALL PUTZ(' ',LISTCL)                                             !00911
      GO TO I99789                                                        !00912
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO LIST-FLEX                                                     !00913
99987 CONTINUE                                                            !00913
        IF(CLASS.EQ.TTO)THEN                                              !00914
C          LIST-DASHES                                                    !00914
          ASSIGN 99788 TO I99789                                          !00914
          GO TO 99789                                                     !00914
99788     CONTINUE                                                        !00914
          ENDIF                                                           !00914
        IF(LSFLX.LT.7)THEN                                                !00915
          LSFLX=LSFLX+7                                                   !00916
          SFLX(LSFLX-6:LSFLX)=' '                                         !00917
          ENDIF                                                           !00918
        SLIST(1:6)=SFLX(1:6)                                              !00919
        LSLIST=6                                                          !00920
        IF(.NOT.(LSTLEV.EQ.0.OR.(3*LSTLEV+LSFLX.GT.WWIDTH)))THEN          !00921
          DO I=1,LSTLEV                                                   !00922
            LSLIST=LSLIST+3                                               !00923
            SLIST(LSLIST-2:LSLIST)='.  '                                  !00924
            ENDDO                                                         !00925
          IF(CLASS.EQ.TFIN) SLIST(LSLIST-2:LSLIST)='...'                  !00926
          ENDIF                                                           !00927
        I=7                                                               !00928
        DO WHILE(SFLX(I:I).EQ.' '.AND.I.LT.LSFLX)                         !00929
          I=I+1                                                           !00929
          ENDDO                                                           !00929
        LSLIST=LSLIST+LSFLX-I+1                                           !00930
        SLIST(LSLIST-LSFLX+I:LSLIST)=SFLX(I:LSFLX)                        !00931
        IF(COMENT)THEN                                                    !00932
          COMENT=.FALSE.                                                  !00933
          LSLIST=LSLIST+LSCOMN                                            !00934
          SLIST(LSLIST-LSCOMN+1:LSLIST)=SCOMN                             !00935
          ENDIF                                                           !00936
        IF((.NOT. COMENT) .AND.LSCOMN.NE.0)THEN!added 7-30-82 by HARN     !00937
          LSLIST=LSLIST+LSCOMN                    !(rutgers) to preserve  !00938
          SLIST(LSLIST-LSCOMN+1:LSLIST)=SCOMN     !comments after keyword !00939
          LSCOMN=0                                !declaration of multi-li!00940
          ENDIF!structure.                                                !00941
        IF(LSLIST.GT.WWIDTH)THEN!Syntax corrected 11-10-95                !00942
          LSLIST=LSFLX                            !by Steve Lionel (Digita!00943
          SLIST(1:LSLIST)=SFLX(1:LSFLX)                                   !00944
          ENDIF                                                           !00945
        IF(ERLST)THEN                                                     !00946
          CALL PUT(SLINE(2:6),SLIST(1:LSLIST),ERRCL)                      !00947
          ERLST=.FALSE.                                                   !00948
        ELSE                                                              !00950
          CALL PUT(SLINE(2:6),SLIST(1:LSLIST),LISTCL)                     !00950
          ENDIF                                                           !00950
      GO TO I99987                                                        !00951
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO PERFORM-INITIALIZATION                                        !00952
99999 CONTINUE                                                            !00952
        CALLNO=0                                                          !00953
        SBLN=' '                                                          !00954
        WWIDTH=LWIDTH-6                                                   !00955
        REFNO=LWIDTH-15                                                   !00956
        ERLST=.FALSE.                                                     !00957
        ENDIF=.FALSE.                                                     !00958
      GO TO I99999                                                        !00959
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO POP-STACK                                                     !00960
99848 CONTINUE                                                            !00960
        TOPTYP=STACK(TOP)                                                 !00961
        IF((ASSEQ).EQ.(TOPTYP))THEN                                       !00963
          TOP=TOP-STACK(TOP-2)-1                                          !00963
        ELSEIF((ACSEQ).EQ.(TOPTYP))THEN                                   !00964
          TOP=TOP-2                                                       !00964
        ELSEIF((ARSEQ).EQ.(TOPTYP))THEN                                   !00965
          TOP=TOP-2                                                       !00965
        ELSEIF((ADSEQ).EQ.(TOPTYP))THEN                                   !00966
          TOP=TOP-2                                                       !00966
        ELSEIF((AFSEQ).EQ.(TOPTYP))THEN                                   !00967
          TOP=TOP-2                                                       !00967
        ELSEIF((AELSE).EQ.(TOPTYP))THEN                                   !00968
          TOP=TOP-2                                                       !00968
        ELSEIF((ATSEQ).EQ.(TOPTYP))THEN                                   !00969
          TOP=TOP-1                                                       !00969
        ELSEIF((AMSEQ).EQ.(TOPTYP))THEN                                   !00970
          TOP=TOP-1                                                       !00970
        ELSEIF((AGRET).EQ.(TOPTYP))THEN                                   !00971
          TOP=TOP-2                                                       !00971
          ENDIF                                                           !00971
      GO TO I99848                                                        !00973
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO PREPARE-FOR-INSERTION                                         !00974
99803 CONTINUE                                                            !00974
        ERTYPE=2                                                          !00975
C        SAVE-ORIGINAL-STATEMENT                                          !00976
        ASSIGN 99787 TO I99934                                            !00976
        GO TO 99934                                                       !00976
99787   CONTINUE                                                          !00976
        LINENO=0                                                          !00977
        SLINE=' '                                                         !00978
        IF(SOURCE.EQ.READ)THEN                                            !00979
          IF(HOLDNO.LE.0)THEN                                             !00980
            CALL PUTZ(SINS2,ERRCL)                                        !00980
          ELSE                                                            !00981
            WRITE(SINSRT(39:43),'(I5)')HOLDNO                             !00982
            CALL PUTZ(SINSRT,ERRCL)                                       !00983
            ENDIF                                                         !00984
          ENDIF                                                           !00985
      GO TO I99803                                                        !00986
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO PREPARE-TO-PROCESS-PROGRAM                                    !00987
99995 CONTINUE                                                            !00987
        DUMMY=NEWNO(SEEDNO)                                               !00988
        ENDPGM=.FALSE.                                                    !00989
        MAX=MAXSTK-(PRIME+4)                                              !00990
        PDUMMY=MAX+1                                                      !00991
        DO I=MAX,MAXSTK                                                   !00992
          STACK(I)=0                                                      !00992
          ENDDO                                                           !00992
        TOP=1                                                             !00993
        STACK(TOP)=AMSEQ                                                  !00994
        COMENT=.FALSE.                                                    !00995
        ERROR=0                                                           !00996
        FIRST=.TRUE.                                                      !00997
        NOPGM=.FALSE.                                                     !00998
        SOURCE=READ                                                       !00999
        LEVEL=0                                                           !01000
        FLEVEL=0                                                          !01001
        LSTLEV=0                                                          !01002
C        LIST-DASHES                                                      !01003
        ASSIGN 99786 TO I99789                                            !01003
        GO TO 99789                                                       !01003
99786   CONTINUE                                                          !01003
      GO TO I99995                                                        !01004
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO PROCESS-PROGRAM                                               !01005
99993 CONTINUE                                                            !01005
        IF(.TRUE.)GOTO99785                                               !01006
        DO WHILE(.NOT.(ENDPGM))                                           !01006
99785     CONTINUE                                                        !01006
          IF(TOP+SAFETY.GT.MAX)THEN                                       !01007
C            GIVE-UP                                                      !01007
            ASSIGN 99784 TO I99854                                        !01007
            GO TO 99854                                                   !01007
99784       CONTINUE                                                      !01007
            ENDIF                                                         !01007
          ACTION=STACK(TOP)                                               !01008
          IF((AGRET).EQ.(ACTION))THEN                                     !01010
C            GENERATE-RETURN-FROM-PROC                                    !01010
            ASSIGN 99783 TO I99806                                        !01010
            GO TO 99806                                                   !01010
99783       CONTINUE                                                      !01010
          ELSE                                                            !01011
C            ANALYZE-NEXT-STATEMENT                                       !01012
            ASSIGN 99782 TO I99959                                        !01012
            GO TO 99959                                                   !01012
99782       CONTINUE                                                      !01012
            IF((AMSEQ).EQ.(ACTION))THEN                                   !01014
              IF((TEXEC).EQ.(CLASS))THEN                                  !01016
C                COMPILE-EXEC                                             !01016
                ASSIGN 99781 TO I99903                                    !01016
                GO TO 99903                                               !01016
99781           CONTINUE                                                  !01016
              ELSEIF((TEND).EQ.(CLASS))THEN                               !01017
                IF(NOPGM)THEN                                             !01018
                  ENDPGM=.TRUE.                                           !01018
                ELSE                                                      !01019
C                  COMPILE-END                                            !01019
                  ASSIGN 99780 TO I99909                                  !01019
                  GO TO 99909                                             !01019
99780             CONTINUE                                                !01019
                  ENDIF                                                   !01019
              ELSEIF((TFIN).EQ.(CLASS))THEN                               !01021
                ERROR=5                                                   !01021
              ELSEIF((TELSE).EQ.(CLASS))THEN                              !01022
                ERROR=8                                                   !01022
              ELSEIF((TTO).EQ.(CLASS))THEN                                !01023
                STACK(TOP)=ATSEQ                                          !01024
C                COMPILE-TO                                               !01025
                ASSIGN 99779 TO I99833                                    !01025
                GO TO 99833                                               !01025
99779           CONTINUE                                                  !01025
              ELSEIF((TCEXP).EQ.(CLASS))THEN                              !01027
                ERROR=17                                                  !01027
                ENDIF                                                     !01027
            ELSEIF((ASSEQ).EQ.(ACTION))THEN                               !01030
              IF((TCEXP).EQ.(CLASS))THEN                                  !01032
C                COMPILE-SEXP                                             !01032
                ASSIGN 99778 TO I99846                                    !01032
                GO TO 99846                                               !01032
99778           CONTINUE                                                  !01032
              ELSEIF((TFIN).EQ.(CLASS))THEN                               !01033
C                COMPILE-SEQ-FIN                                          !01033
                ASSIGN 99777 TO I99852                                    !01033
                GO TO 99852                                               !01033
99777           CONTINUE                                                  !01033
              ELSEIF((TEND).EQ.(CLASS))THEN                               !01034
                ERROR=3                                                   !01034
              ELSEIF((TELSE).EQ.(CLASS))THEN                              !01035
                ERROR=12                                                  !01035
              ELSEIF((TTO).EQ.(CLASS))THEN                                !01036
                ERROR=15                                                  !01036
              ELSEIF((TEXEC).EQ.(CLASS))THEN                              !01037
                ERROR=23                                                  !01037
                ENDIF                                                     !01037
            ELSEIF((ACSEQ).EQ.(ACTION))THEN                               !01040
              IF((TCEXP).EQ.(CLASS))THEN                                  !01042
C                COMPILE-CEXP                                             !01042
                ASSIGN 99776 TO I99929                                    !01042
                GO TO 99929                                               !01042
99776           CONTINUE                                                  !01042
              ELSEIF((TFIN).EQ.(CLASS))THEN                               !01043
C                COMPILE-SEQ-FIN                                          !01043
                ASSIGN 99775 TO I99852                                    !01043
                GO TO 99852                                               !01043
99775           CONTINUE                                                  !01043
              ELSEIF((TEND).EQ.(CLASS))THEN                               !01044
                ERROR=2                                                   !01044
              ELSEIF((TELSE).EQ.(CLASS))THEN                              !01045
                ERROR=11                                                  !01045
              ELSEIF((TTO).EQ.(CLASS))THEN                                !01046
                ERROR=14                                                  !01046
              ELSEIF((TEXEC).EQ.(CLASS))THEN                              !01047
                ERROR=22                                                  !01047
                ENDIF                                                     !01047
            ELSEIF((AELSE).EQ.(ACTION))THEN                               !01050
              IF((TELSE).EQ.(CLASS))THEN                                  !01052
C                COMPILE-ELSE                                             !01052
                ASSIGN 99774 TO I99913                                    !01052
                GO TO 99913                                               !01052
99774           CONTINUE                                                  !01052
              ELSEIF((TEND).EQ.(CLASS))THEN                               !01053
                ERROR=4                                                   !01053
              ELSEIF((TFIN).EQ.(CLASS))THEN                               !01054
                ERROR=7                                                   !01054
              ELSEIF((TTO).EQ.(CLASS))THEN                                !01055
                ERROR=16                                                  !01055
              ELSEIF((TCEXP).EQ.(CLASS))THEN                              !01056
                ERROR=20                                                  !01056
              ELSEIF((TEXEC).EQ.(CLASS))THEN                              !01057
                ERROR=24                                                  !01057
                ENDIF                                                     !01057
            ELSEIF((ATSEQ).EQ.(ACTION))THEN                               !01060
              IF((TTO).EQ.(CLASS))THEN                                    !01062
C                COMPILE-TO                                               !01062
                ASSIGN 99773 TO I99833                                    !01062
                GO TO 99833                                               !01062
99773           CONTINUE                                                  !01062
              ELSEIF((TEND).EQ.(CLASS))THEN                               !01063
C                COMPILE-END                                              !01063
                ASSIGN 99772 TO I99909                                    !01063
                GO TO 99909                                               !01063
99772           CONTINUE                                                  !01063
              ELSEIF((TFIN).EQ.(CLASS))THEN                               !01064
                ERROR=6                                                   !01064
              ELSEIF((TELSE).EQ.(CLASS))THEN                              !01065
                ERROR=9                                                   !01065
              ELSEIF((TCEXP).EQ.(CLASS))THEN                              !01066
                ERROR=18                                                  !01066
              ELSEIF((TEXEC).EQ.(CLASS))THEN                              !01067
                ERROR=21                                                  !01067
                ENDIF                                                     !01067
            ELSE                                                          !01070
              IF((TEXEC).EQ.(CLASS))THEN                                  !01072
C                COMPILE-EXEC                                             !01072
                ASSIGN 99771 TO I99903                                    !01072
                GO TO 99903                                               !01072
99771           CONTINUE                                                  !01072
              ELSEIF((TFIN).EQ.(CLASS))THEN                               !01073
C                COMPILE-SIMPLE-FIN                                       !01073
                ASSIGN 99770 TO I99840                                    !01073
                GO TO 99840                                               !01073
99770           CONTINUE                                                  !01073
              ELSEIF((TEND).EQ.(CLASS))THEN                               !01074
                ERROR=1                                                   !01074
              ELSEIF((TELSE).EQ.(CLASS))THEN                              !01075
                ERROR=10                                                  !01075
              ELSEIF((TTO).EQ.(CLASS))THEN                                !01076
                ERROR=13                                                  !01076
              ELSEIF((TCEXP).EQ.(CLASS))THEN                              !01077
                ERROR=19                                                  !01077
                ENDIF                                                     !01077
              ENDIF                                                       !01079
            IF(.NOT.(NOPGM))THEN                                          !01081
C              ANALYZE-ERRORS-AND-LIST                                    !01081
              ASSIGN 99769 TO I99988                                      !01081
              GO TO 99988                                                 !01081
99769         CONTINUE                                                    !01081
              ENDIF                                                       !01081
            ENDIF                                                         !01082
          ENDDO                                                           !01084
      GO TO I99993                                                        !01085
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO PROCESS-TABLE                                                 !01086
99963 CONTINUE                                                            !01086
        IF(.NOT.(PTABLE.EQ.0))THEN                                        !01087
          TABLCL=LISTCL                                                   !01088
C          LIST-DASHES                                                    !01089
          ASSIGN 99768 TO I99789                                          !01089
          GO TO 99789                                                     !01089
99768     CONTINUE                                                        !01089
          CALL PUTZ(STABH,LISTCL)                                         !01090
          CALL PUTZ(' ',LISTCL)                                           !01091
          P=PTABLE                                                        !01092
          NDERR=.FALSE.                                                   !01093
          NIERR=.FALSE.                                                   !01094
          IF(.TRUE.)GOTO99767                                             !01095
          DO WHILE(.NOT.(P.EQ.0))                                         !01095
99767       CONTINUE                                                      !01095
            IF(STACK(P+2).EQ.0)THEN                                       !01096
              NDERR=.TRUE.                                                !01097
              MAJCNT=MAJCNT+1                                             !01098
              ENDIF                                                       !01099
            IF(STACK(P+3).EQ.0)THEN                                       !01100
              NIERR=.TRUE.                                                !01101
              MINCNT=MINCNT+1                                             !01102
              ENDIF                                                       !01103
C            PRODUCE-ENTRY-LISTING                                        !01104
            ASSIGN 99765 TO I99766                                        !01104
            GO TO 99766                                                   !01104
99765       CONTINUE                                                      !01104
            P=STACK(P)                                                    !01105
            ENDDO                                                         !01106
          IF(NDERR)THEN                                                   !01107
            CALL PUTZ(SNDER1,ERRCL)                                       !01108
            CALL PUTZ(SNDER2,ERRCL)                                       !01109
C            LIST-BLANK-LINE                                              !01110
            ASSIGN 99764 TO I99961                                        !01110
            GO TO 99961                                                   !01110
99764       CONTINUE                                                      !01110
            P=PTABLE                                                      !01111
            TABLCL=ERRCL                                                  !01112
            IF(.TRUE.)GOTO99763                                           !01113
            DO WHILE(.NOT.(P.EQ.0))                                       !01113
99763         CONTINUE                                                    !01113
              IF(STACK(P+2).EQ.0)THEN                                     !01114
C                PRODUCE-ENTRY-LISTING                                    !01114
                ASSIGN 99762 TO I99766                                    !01114
                GO TO 99766                                               !01114
99762           CONTINUE                                                  !01114
                ENDIF                                                     !01114
              P=STACK(P)                                                  !01115
              ENDDO                                                       !01116
            ENDIF                                                         !01117
          IF(NIERR)THEN                                                   !01118
            CALL PUTZ(SNIER1,ERRCL)                                       !01119
            CALL PUTZ(SNIER2,ERRCL)                                       !01120
C            LIST-BLANK-LINE                                              !01121
            ASSIGN 99761 TO I99961                                        !01121
            GO TO 99961                                                   !01121
99761       CONTINUE                                                      !01121
            P=PTABLE                                                      !01122
            TABLCL=ERRCL                                                  !01123
            IF(.TRUE.)GOTO99760                                           !01124
            DO WHILE(.NOT.(P.EQ.0))                                       !01124
99760         CONTINUE                                                    !01124
              IF(STACK(P+3).EQ.0)THEN                                     !01125
C                PRODUCE-ENTRY-LISTING                                    !01125
                ASSIGN 99759 TO I99766                                    !01125
                GO TO 99766                                               !01125
99759           CONTINUE                                                  !01125
                ENDIF                                                     !01125
              P=STACK(P)                                                  !01126
              ENDDO                                                       !01127
            ENDIF                                                         !01128
          ENDIF                                                           !01129
      GO TO I99963                                                        !01130
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO PRODUCE-ENTRY-LISTING                                         !01131
99766 CONTINUE                                                            !01131
        SST(1:7)=' '                                                      !01132
        IF (STACK(P+2).NE.0) WRITE(SST(1:6),'(I6.5)')STACK(P+2)           !01133
        IP=4*(P+4)                                                        !01134
        CALL PUTZ(SST(2:7)//CSTAK(IP+1:IP+STACK(P+4)),TABLCL)             !01135
        QP=STACK(P+3)                                                     !01136
        DO WHILE(.NOT.(QP.EQ.0))                                          !01137
          LSST=4                                                          !01138
          SST(1:4)=' '                                                    !01139
          DO WHILE(.NOT.(QP.EQ.0.OR.LSST.GT.REFNO))                       !01140
            LSST=LSST+7                                                   !01141
            WRITE(SST(LSST-6:LSST),'(I7.5)')STACK(QP+1)                   !01142
            QP=STACK(QP)                                                  !01143
            ENDDO                                                         !01144
          CALL PUTZ(SST(1:LSST),TABLCL)                                   !01145
          ENDDO                                                           !01146
        CALL PUTZ(' ',LISTCL)                                             !01147
      GO TO I99766                                                        !01148
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO PUSH-FINSEQ                                                   !01149
99814 CONTINUE                                                            !01149
        TOP=TOP+2                                                         !01150
        STACK(TOP-1)=LINENO                                               !01151
        STACK(TOP)=AXSEQ                                                  !01152
        LEVEL=LEVEL+1                                                     !01153
        FLEVEL=FLEVEL+1                                                   !01154
      GO TO I99814                                                        !01155
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO PUT-CONTINUE                                                  !01156
99868 CONTINUE                                                            !01156
        LSST=14                                                           !01157
        SST(6:14)=' CONTINUE'                                             !01158
C        PUT-STATEMENT                                                    !01159
        ASSIGN 99758 TO I99925                                            !01159
        GO TO 99925                                                       !01159
99758   CONTINUE                                                          !01159
      GO TO I99868                                                        !01160
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO PUT-CONTINUE-OR-COMMENT                                       !01161
99919 CONTINUE                                                            !01161
        STNO=FLXNO                                                        !01162
        IF(STNO.NE.0)THEN                                                 !01164
C          PUT-CONTINUE                                                   !01164
          ASSIGN 99757 TO I99868                                          !01164
          GO TO 99868                                                     !01164
99757     CONTINUE                                                        !01164
        ELSEIF(COMENT)THEN                                                !01165
          COMENT=.FALSE.                                                  !01166
          CALL PUTF(SLINE(2:6),SCOMN(1:LSCOMN))                           !01167
          ENDIF                                                           !01168
      GO TO I99919                                                        !01170
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO PUT-COPY                                                      !01171
99905 CONTINUE                                                            !01171
        IF(ENDIF)THEN                                                     !01172
C          PUT-ENDIF                                                      !01172
          ASSIGN 99756 TO I99990                                          !01172
          GO TO 99990                                                     !01172
99756     CONTINUE                                                        !01172
          ENDIF                                                           !01172
        IF(LSFLX.LT.7.OR.PASS)THEN                                        !01173
          CALL PUTF(SLINE(2:6),SFLX(1:LSFLX))                             !01173
        ELSE                                                              !01174
          CH = SFLX(9:9)                                                  !01175
C       IF (LSFLX.GE.9.AND.SFLX(1:8).EQ.' '.AND.CHTYP(ICH).EQ.TDIGIT)     !01176
          IF(LSFLX.GE.9.AND.SFLX(1:8).EQ.' '.AND.CHTYP(ICHAR(CH)).EQ.TDI  !01177
     1GIT)THEN                                                            !01177
            SFLX(6:6) = CH                                                !01178
            SFLX(9:9) = ' '                                               !01179
            ENDIF                                                         !01180
          I=7                                                             !01181
          DO WHILE(SFLX(I:I).EQ.' '.AND.I.LT.LSFLX)                       !01182
            I=I+1                                                         !01182
            ENDDO                                                         !01182
          LSST=LSFLX+7-I                                                  !01183
          SST(6:6)=SFLX(6:6)                                              !01184
          SST(7:LSST)=SFLX(I:LSFLX)                                       !01185
          STNO=FLXNO                                                      !01186
C          PUT-STATEMENT                                                  !01187
          ASSIGN 99755 TO I99925                                          !01187
          GO TO 99925                                                     !01187
99755     CONTINUE                                                        !01187
          ENDIF                                                           !01188
      GO TO I99905                                                        !01189
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO PUT-ENDIF                                                     !01190
99990 CONTINUE                                                            !01190
        ENDIF=.FALSE.                                                     !01191
        IND=INDEX(SST(1:LSST),'!')                                        !01192
        IF(IND.EQ.0)THEN                                                  !01193
          IND=LSST+1                                                      !01194
          SST(IND:IND)=' '                                                !01195
          ENDIF                                                           !01196
        LSAV=LSST                                                         !01197
        LSST=IND-1                                                        !01198
        IF(LSST.LE.72)THEN                                                !01199
          IF(LSAV.GE.IND)THEN                                             !01200
            CALL PUTF(SENDIF,SST(1:LSST)//SST(IND:LSAV))                  !01200
          ELSE                                                            !01201
            CALL PUTF(SENDIF,SST(1:LSST))                                 !01201
            ENDIF                                                         !01201
        ELSE                                                              !01203
          IF(LSAV.GE.IND)THEN                                             !01204
            CALL PUTF(SLINE(2:6),SST(1:72)//SST(IND:LSAV))                !01204
          ELSE                                                            !01205
            CALL PUTF(SLINE(2:6),SST(1:72))                               !01205
            ENDIF                                                         !01205
          S=73                                                            !01206
          IF(.TRUE.)GOTO99754                                             !01207
          DO WHILE(.NOT.(S.GT.LSST))                                      !01207
99754       CONTINUE                                                      !01207
            L=S+65                                                        !01208
            IF (L.GT.LSST) L=LSST                                         !01209
            CALL PUTF(SENDIF,'     1'//SST(S:L))                          !01210
            S=S+66                                                        !01211
            ENDDO                                                         !01212
          ENDIF                                                           !01213
      GO TO I99990                                                        !01214
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO PUT-STATEMENT                                                 !01215
99925 CONTINUE                                                            !01215
        IF(ENDIF)THEN                                                     !01216
C          PUT-ENDIF                                                      !01216
          ASSIGN 99753 TO I99990                                          !01216
          GO TO 99990                                                     !01216
99753     CONTINUE                                                        !01216
          ENDIF                                                           !01216
        IF(COMENT)THEN                                                    !01217
          COMENT=.FALSE.                                                  !01218
          SST(LSST+1:LSST+LSCOMN)=SCOMN(1:LSCOMN)                         !01219
          LSST=LSST+LSCOMN                                                !01220
          ENDIF                                                           !01221
        WRITE(SST(1:5),'(I5.0)')STNO                                      !01222
        STNO=0                                                            !01223
        FLXNO=0                                                           !01224
        NB=2*FLEVEL                                                       !01225
        SST(LSST+1:LSST+1)='!'                                            !01226
        I=0                                                               !01227
        IF(.TRUE.)GOTO99752                                               !01228
        DO WHILE(.NOT.(SST(I:I).EQ.'!'))                                  !01228
99752     CONTINUE                                                        !01228
          I=I+1                                                           !01229
          IF (SST(I:I).EQ.'''')I=INDEX(SST(I+1:LSST+1),'''')+I+1          !01230
          ENDDO                                                           !01231
        SST(LSST+1:LSST+1)=' '                                            !01232
        LSAV=LSST                                                         !01233
C       IF (LSAV.LT.I) LSAV=I   !CLNS   LSAV SOMETIMES <I                 !01234
        LSST=I-1                                                          !01235
        IF(LSST.LE.72-NB)THEN                                             !01236
          IF(NB.NE.0)THEN                                                 !01237
            IF(LSAV.GE.I)THEN                                             !01238
              CALL PUTF(SLINE(2:6),SST(1:6)//BLANKS(1:NB)//SST(7:LSST)//  !01239
     1        SST(I:LSAV))                                                !01240
            ELSE                                                          !01242
              CALL PUTF(SLINE(2:6),SST(1:6)//BLANKS(1:NB)//SST(7:LSST))   !01243
              ENDIF                                                       !01244
          ELSE                                                            !01246
            IF(LSAV.GE.I)THEN                                             !01247
              CALL PUTF(SLINE(2:6),SST(1:6)//SST(7:LSST)//SST(I:LSAV))    !01248
            ELSE                                                          !01250
              CALL PUTF(SLINE(2:6),SST(1:6)//SST(7:LSST))                 !01251
              ENDIF                                                       !01252
            ENDIF                                                         !01253
        ELSE                                                              !01255
          IF(NB.NE.0)THEN                                                 !01256
            IF(LSAV.GE.I)THEN                                             !01257
              CALL PUTF(SLINE(2:6),SST(1:6)//BLANKS(1:NB)//SST(7:72-NB)/  !01258
     1/                                                                   !01258
     1        SST(I:LSAV))                                                !01259
            ELSE                                                          !01261
              CALL PUTF(SLINE(2:6),SST(1:6)//BLANKS(1:NB)//SST(7:72-NB))  !01261
              ENDIF                                                       !01261
          ELSE                                                            !01263
            IF(LSAV.GE.I)THEN                                             !01264
              CALL PUTF(SLINE(2:6),SST(1:6)//SST(7:72-NB)//               !01265
     1        SST(I:LSAV))                                                !01266
            ELSE                                                          !01268
              CALL PUTF(SLINE(2:6),SST(1:6)//SST(7:72-NB))                !01268
              ENDIF                                                       !01268
            ENDIF                                                         !01269
          S=73-NB                                                         !01270
          IF(.TRUE.)GOTO99751                                             !01271
          DO WHILE(.NOT.(S.GT.LSST))                                      !01271
99751       CONTINUE                                                      !01271
            L=S+65                                                        !01272
            IF (L.GT.LSST) L=LSST                                         !01273
            CALL PUTF(SLINE(2:6),'     1'//SST(S:L))                      !01274
            S=S+66                                                        !01275
            ENDDO                                                         !01276
          ENDIF                                                           !01277
      GO TO I99925                                                        !01278
C                                                                         !01279
C-------------------------------------------------------------------------!01280
C                                                                         !01281
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO READ-NEXT-STATEMENT                                           !01282
99958 CONTINUE                                                            !01282
C                                                                         !01283
        IF(.TRUE.)GOTO99750                                               !01284
        DO WHILE(.NOT.(FOUND))                                            !01284
99750     CONTINUE                                                        !01284
          CALL GET(LSFLX,LINENO,SFLX,ENDFIL)                              !01285
          WRITE(SLINE,'(I6.5)') LINENO                                    !01286
C                                                                         !01287
          IF(FIRST)THEN                                                   !01288
            FIRST=.FALSE.                                                 !01289
            IF(ENDFIL) NOPGM=.TRUE.                                       !01290
C                                                                         !01292
            ENDIF                                                         !01291
          IF(ENDFIL)THEN                                                  !01293
            LSFLX = 9                                                     !01294
            SFLX(1:LSFLX) = '      END'                                   !01295
            LINENO = 0                                                    !01296
            SLINE = ' '                                                   !01297
C                                                                         !01299
            ENDIF                                                         !01298
          CH = SFLX(1:1)                                                  !01300
C                                                                         !01301
C                                                                         !01303
          IF(LSFLX.EQ.0)THEN                                              !01304
            SBLN=SLINE(2:6)                                               !01305
C            LIST-BLANK-LINE                                              !01306
            ASSIGN 99749 TO I99961                                        !01306
            GO TO 99961                                                   !01306
99749       CONTINUE                                                      !01306
            FOUND=.FALSE.                                                 !01307
C                                                                         !01309
          ELSEIF(CH.EQ.'C'.OR.CH.EQ.'!')THEN                              !01310
C            LIST-COMMENT-LINE                                            !01311
            ASSIGN 99748 TO I99794                                        !01311
            GO TO 99794                                                   !01311
99748       CONTINUE                                                      !01311
            CALL PUTF(SLINE(2:6),SFLX(1:LSFLX))                           !01312
            FOUND=.FALSE.                                                 !01313
C                                                                         !01315
          ELSEIF(CH.EQ.'D')THEN                                           !01316
C                                                                         !01317
            IF(DEBUGG)THEN                                                !01318
              FOUND=.TRUE.                                                !01319
              SFLX(1:1)=' '                                               !01320
C                                                                         !01322
            ELSE                                                          !01323
C              LIST-COMMENT-LINE                                          !01324
              ASSIGN 99747 TO I99794                                      !01324
              GO TO 99794                                                 !01324
99747         CONTINUE                                                    !01324
              CALL PUTF(SLINE(2:6),SFLX(1:LSFLX))                         !01325
              FOUND=.FALSE.                                               !01326
C                                                                         !01328
              ENDIF                                                       !01327
C                                                                         !01330
          ELSE                                                            !01331
            FOUND=.TRUE.                                                  !01331
C                                                                         !01332
            ENDIF                                                         !01331
C                                                                         !01334
          ENDDO                                                           !01335
C                                                                         !01336
      GO TO I99958                                                        !01337
C                                                                         !01338
C-------------------------------------------------------------------------!01339
C                                                                         !01340
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO RESET-GET-CHARACTER                                           !01341
99945 CONTINUE                                                            !01341
C                                                                         !01342
        CURSOR=CURSOR-1                                                   !01343
C        GET-CHARACTER                                                    !01344
        ASSIGN 99746 TO I99804                                            !01344
        GO TO 99804                                                       !01344
99746   CONTINUE                                                          !01344
      GO TO I99945                                                        !01345
C                                                                         !01346
C-------------------------------------------------------------------------!01347
C                                                                         !01348
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO REVERSE-LIST                                                  !01349
99745 CONTINUE                                                            !01349
C                                                                         !01350
        LL = 0                                                            !01351
        LR = STACK(LP)                                                    !01352
C                                                                         !01353
        DO WHILE(.NOT.(LR.EQ.0))                                          !01354
          LT=STACK(LR)                                                    !01355
          STACK(LR)=LL                                                    !01356
          LL=LR                                                           !01357
          LR=LT                                                           !01358
          ENDDO                                                           !01359
C                                                                         !01360
        STACK(LP)=LL                                                      !01361
      GO TO I99745                                                        !01362
C                                                                         !01363
C-------------------------------------------------------------------------!01364
C                                                                         !01365
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO SAVE-ORIGINAL-STATEMENT                                       !01366
99934 CONTINUE                                                            !01366
C                                                                         !01367
        IF(.NOT.(SAVED))THEN                                              !01368
          SAVED=.TRUE.                                                    !01369
          HOLDNO=LINENO                                                   !01370
          LSHOLD=LSFLX                                                    !01371
          SHOLD(1:LSHOLD)=SFLX(1:LSFLX)                                   !01372
C                                                                         !01374
          ENDIF                                                           !01373
      GO TO I99934                                                        !01375
C                                                                         !01376
C-------------------------------------------------------------------------!01377
C                                                                         !01378
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO SCAN-CONTINUATION                                             !01379
99954 CONTINUE                                                            !01379
C        GET-CHARACTER                                                    !01380
        ASSIGN 99744 TO I99804                                            !01380
        GO TO 99804                                                       !01380
99744   CONTINUE                                                          !01380
C                                                                         !01381
C                                                                         !01383
        IF(CHTYPE.EQ.TEOL)THEN                                            !01384
          CONT=.FALSE.                                                    !01384
C                                                                         !01385
        ELSEIF(CH.EQ.'0'.OR.CH.EQ.' ')THEN                                !01386
          CONT=.FALSE.                                                    !01386
C                                                                         !01387
        ELSE                                                              !01388
          CONT=.TRUE.                                                     !01388
          ENDIF                                                           !01388
C                                                                         !01390
        IF (CH.EQ.'0') SFLX(6:6)=' '                                      !01391
      GO TO I99954                                                        !01392
C                                                                         !01393
C-------------------------------------------------------------------------!01394
C                                                                         !01395
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO SCAN-CONTROL                                                  !01396
99947 CONTINUE                                                            !01396
C                                                                         !01397
        DO WHILE(CHTYPE.EQ.TBLANK)                                        !01398
C          GET-CHARACTER                                                  !01398
          ASSIGN 99743 TO I99804                                          !01398
          GO TO 99804                                                     !01398
99743     CONTINUE                                                        !01398
          ENDDO                                                           !01398
        START=CURSOR                                                      !01399
C                                                                         !01400
        IF(CHTYPE.NE.TLP)THEN                                             !01401
          ERSTOP=ERSTOP+1                                                 !01402
          ERRSTK(ERSTOP)=3                                                !01403
C          SAVE-ORIGINAL-STATEMENT                                        !01404
          ASSIGN 99742 TO I99934                                          !01404
          GO TO 99934                                                     !01404
99742     CONTINUE                                                        !01404
          LSFLX=LSFLX-1                                                   !01405
          SFLX(START:LSFLX)='('//SFLX(START:LSFLX-1)                      !01406
C                                                                         !01408
        ELSE                                                              !01409
          PCNT=1                                                          !01410
          FOUND=.TRUE.                                                    !01411
C                                                                         !01412
          IF(.TRUE.)GOTO99741                                             !01413
          DO WHILE(.NOT.(PCNT.EQ.0.OR..NOT.FOUND))                        !01413
99741       CONTINUE                                                      !01413
C            GET-CHARACTER                                                !01414
            ASSIGN 99740 TO I99804                                        !01414
            GO TO 99804                                                   !01414
99740       CONTINUE                                                      !01414
            IF((TRP).EQ.(CHTYPE))THEN                                     !01416
              PCNT=PCNT-1                                                 !01416
            ELSEIF((TLP).EQ.(CHTYPE))THEN                                 !01417
              PCNT=PCNT+1                                                 !01417
            ELSEIF((TEOL).EQ.(CHTYPE))THEN                                !01418
              FOUND=.FALSE.                                               !01418
              ENDIF                                                       !01418
            ENDDO                                                         !01420
          IF(.NOT.(FOUND))THEN                                            !01421
            ERSTOP=ERSTOP+1                                               !01422
            ERRSTK(ERSTOP)=4                                              !01423
C            SAVE-ORIGINAL-STATEMENT                                      !01424
            ASSIGN 99739 TO I99934                                        !01424
            GO TO 99934                                                   !01424
99739       CONTINUE                                                      !01424
            DO I=1,PCNT                                                   !01425
              LSFLX=LSFLX+1                                               !01426
              SFLX(LSFLX:LSFLX+1)=')'                                     !01427
              ENDDO                                                       !01428
            CURSOR=LSFLX                                                  !01429
C            RESET-GET-CHARACTER                                          !01430
            ASSIGN 99738 TO I99945                                        !01430
            GO TO 99945                                                   !01430
99738       CONTINUE                                                      !01430
            ENDIF                                                         !01431
          ENDIF                                                           !01432
C        GET-CHARACTER                                                    !01433
        ASSIGN 99737 TO I99804                                            !01433
        GO TO 99804                                                       !01433
99737   CONTINUE                                                          !01433
        NUNITS=NUNITS+1                                                   !01434
        UTYPE(NUNITS)=UEXP                                                !01435
        USTART(NUNITS)=START                                              !01436
        UFIN(NUNITS)=CURSOR-1                                             !01437
CLNS    warn if too many paren                                            !01439
        PCNT=0                                                            !01440
        DO WHILE(.NOT.(CHTYPE.EQ.TEOL .OR. CH.EQ.'''' .OR. CH.EQ.'!'))    !01441
          IF((TRP).EQ.(CHTYPE))THEN                                       !01443
            PCNT=PCNT-1                                                   !01443
          ELSEIF((TLP).EQ.(CHTYPE))THEN                                   !01444
            PCNT=PCNT+1                                                   !01444
            ENDIF                                                         !01444
C          GET-CHARACTER                                                  !01446
          ASSIGN 99736 TO I99804                                          !01446
          GO TO 99804                                                     !01446
99736     CONTINUE                                                        !01446
          ENDDO                                                           !01447
        IF(CH.EQ.'''') PCNT=0   ! can't handle lines with quoted strs     !01448
        IF(PCNT.LT.0)THEN                                                 !01450
          ERSTOP=ERSTOP+1                                                 !01451
          ERRSTK(ERSTOP)=6                                                !01452
C          SAVE-ORIGINAL-STATEMENT                                        !01453
          ASSIGN 99735 TO I99934                                          !01453
          GO TO 99934                                                     !01453
99735     CONTINUE                                                        !01453
        ELSEIF(PCNT.GT.0)THEN                                             !01455
          ERSTOP=ERSTOP+1                                                 !01456
          ERRSTK(ERSTOP)=4                                                !01457
C          SAVE-ORIGINAL-STATEMENT                                        !01458
          ASSIGN 99734 TO I99934                                          !01458
          GO TO 99934                                                     !01458
99734     CONTINUE                                                        !01458
          ENDIF                                                           !01459
CLNS    restore character pointer                                         !01462
        CURSOR=UFIN(NUNITS)                                               !01463
C        GET-CHARACTER                                                    !01464
        ASSIGN 99733 TO I99804                                            !01464
        GO TO 99804                                                       !01464
99733   CONTINUE                                                          !01464
        IF(SFLX(START:CURSOR-1).EQ.'(OTHERWISE)')UTYPE(NUNITS)=UOWSE      !01466
C        SCAN-PINV-OR-FORT                                                !01467
        ASSIGN 99732 TO I99939                                            !01467
        GO TO 99939                                                       !01467
99732   CONTINUE                                                          !01467
      GO TO I99947                                                        !01468
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO SCAN-GARBAGE                                                  !01470
99950 CONTINUE                                                            !01470
        DO WHILE(CHTYPE.EQ.TBLANK)                                        !01471
C          GET-CHARACTER                                                  !01471
          ASSIGN 99731 TO I99804                                          !01471
          GO TO 99804                                                     !01471
99731     CONTINUE                                                        !01471
          ENDDO                                                           !01471
        IF(CHTYPE.NE.TEOL)THEN                                            !01472
          IF(CH.EQ.'!')THEN                                               !01473
            COMENT=.TRUE.                                                 !01474
            LSCOMN=LSFLX-CURSOR+1                                         !01475
            SCOMN(1:LSCOMN)=SFLX(CURSOR:LSFLX)                            !01476
          ELSE                                                            !01478
            ERSTOP=ERSTOP+1                                               !01479
            ERRSTK(ERSTOP)=2                                              !01480
C            SAVE-ORIGINAL-STATEMENT                                      !01481
            ASSIGN 99730 TO I99934                                        !01481
            GO TO 99934                                                   !01481
99730       CONTINUE                                                      !01481
            ENDIF                                                         !01482
          LSFLX=CURSOR-1                                                  !01483
          ENDIF                                                           !01484
      GO TO I99950                                                        !01485
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO SCAN-KEYWORD                                                  !01486
99952 CONTINUE                                                            !01486
C        GET-CHARACTER                                                    !01487
        ASSIGN 99729 TO I99804                                            !01487
        GO TO 99804                                                       !01487
99729   CONTINUE                                                          !01487
        DO WHILE(CHTYPE.EQ.TBLANK)                                        !01488
C          GET-CHARACTER                                                  !01488
          ASSIGN 99728 TO I99804                                          !01488
          GO TO 99804                                                     !01488
99728     CONTINUE                                                        !01488
          ENDDO                                                           !01488
        IF((TLETTR).EQ.(CHTYPE))THEN                                      !01490
          START=CURSOR                                                    !01491
          INVOKE=.FALSE.                                                  !01492
          BADCH=.FALSE.                                                   !01493
          IF(.TRUE.)GOTO99727                                             !01494
          DO WHILE(.NOT.(BADCH))                                          !01494
99727       CONTINUE                                                      !01494
C            GET-CHARACTER                                                !01495
            ASSIGN 99726 TO I99804                                        !01495
            GO TO 99804                                                   !01495
99726       CONTINUE                                                      !01495
            IF(CHTYPE.LE.TDIGIT)THEN                                      !01497
            ELSEIF(CHTYPE.EQ.THYPHN)THEN                                  !01498
              INVOKE=.TRUE.                                               !01498
            ELSE                                                          !01499
              BADCH=.TRUE.                                                !01499
              ENDIF                                                       !01499
            ENDDO                                                         !01501
          IF(INVOKE)THEN                                                  !01502
            CLASS=TEXEC                                                   !01503
            EXTYPE=TINVOK                                                 !01504
            NUNITS=1                                                      !01505
            UTYPE(1)=UPINV                                                !01506
            USTART(1)=START                                               !01507
            UFIN(1)=CURSOR-1                                              !01508
          ELSE                                                            !01510
            CLASS=TEXEC                                                   !01511
            EXTYPE=TFORT                                                  !01512
            IF(.NOT.(CH.EQ.'='))THEN                                      !01513
              IF((2).EQ.(CURSOR-START))THEN                               !01515
                IF(SFLX(START:CURSOR-1).EQ.'IF')THEN                      !01517
                  EXTYPE=TIF                                              !01517
                ELSEIF(SFLX(START:CURSOR-1).EQ.'TO')THEN                  !01518
                  CLASS=TTO                                               !01518
                ELSEIF(SFLX(START:CURSOR-1).EQ.'DO')THEN                  !01519
                  DO WHILE(CHTYPE.EQ.TBLANK)                              !01520
C                    GET-CHARACTER                                        !01520
                    ASSIGN 99725 TO I99804                                !01520
                    GO TO 99804                                           !01520
99725               CONTINUE                                              !01520
                    ENDDO                                                 !01520
                  IF(CHTYPE.EQ.TDIGIT)THEN                                !01521
                    EXTYPE=TFORT                                          !01521
                  ELSE                                                    !01522
                    EXTYPE=TDO                                            !01522
                    ENDIF                                                 !01522
                  ENDIF                                                   !01523
              ELSEIF((3).EQ.(CURSOR-START))THEN                           !01526
                IF(SFLX(START:CURSOR-1).EQ.'FIN')THEN                     !01528
                  CLASS=TFIN                                              !01528
                ELSEIF(SFLX(START:CURSOR-1).EQ.'END')THEN                 !01529
                  CLASS=TEND                                              !01529
                  ENDIF                                                   !01529
              ELSEIF((4).EQ.(CURSOR-START))THEN                           !01532
                IF(SFLX(START:CURSOR-1).EQ.'WHEN')THEN                    !01534
                  EXTYPE=TWHEN                                            !01534
                ELSEIF(SFLX(START:CURSOR-1).EQ.'ELSE')THEN                !01535
                  CLASS=TELSE                                             !01536
                  NUNITS=1                                                !01537
                  UTYPE(1)=UOWSE                                          !01538
                  ENDIF                                                   !01539
              ELSEIF((5).EQ.(CURSOR-START))THEN                           !01542
                IF(SFLX(START:CURSOR-1).EQ.'WHILE')THEN                   !01544
                  EXTYPE=TWHILE                                           !01544
                ELSEIF(SFLX(START:CURSOR-1).EQ.'UNTIL')THEN               !01545
                  EXTYPE=TUNTIL                                           !01545
                  ENDIF                                                   !01545
              ELSEIF((6).EQ.(CURSOR-START))THEN                           !01548
                IF(SFLX(START:CURSOR-1).EQ.'REPEAT')THEN                  !01550
                  DO WHILE(CHTYPE.EQ.TBLANK)                              !01551
C                    GET-CHARACTER                                        !01551
                    ASSIGN 99724 TO I99804                                !01551
                    GO TO 99804                                           !01551
99724               CONTINUE                                              !01551
                    ENDDO                                                 !01551
                  START=CURSOR                                            !01552
                  DO WHILE(CHTYPE.EQ.TLETTR)                              !01553
C                    GET-CHARACTER                                        !01553
                    ASSIGN 99723 TO I99804                                !01553
                    GO TO 99804                                           !01553
99723               CONTINUE                                              !01553
                    ENDDO                                                 !01553
                  IF(SFLX(START:CURSOR-1).EQ.'WHILE')THEN                 !01555
                    EXTYPE=TRWHIL                                         !01555
                  ELSEIF(SFLX(START:CURSOR-1).EQ.'UNTIL')THEN             !01556
                    EXTYPE=TRUNTL                                         !01556
                    ENDIF                                                 !01556
                ELSEIF(SFLX(START:CURSOR-1).EQ.'SELECT')THEN              !01559
                  EXTYPE=TSELCT                                           !01559
                ELSEIF(SFLX(START:CURSOR-1).EQ.'UNLESS')THEN              !01560
                  EXTYPE=TUNLES                                           !01560
                  ENDIF                                                   !01560
              ELSEIF((11).EQ.(CURSOR-START))THEN                          !01563
                IF (SFLX(START:CURSOR-1).EQ.'CONDITIONAL') EXTYPE=TCOND   !01564
                ENDIF                                                     !01565
              ENDIF                                                       !01567
            ENDIF                                                         !01568
        ELSEIF((TLP).EQ.(CHTYPE))THEN                                     !01570
          CLASS=TCEXP                                                     !01570
        ELSE                                                              !01571
          CLASS=TEXEC                                                     !01572
          EXTYPE=TFORT                                                    !01573
          ENDIF                                                           !01574
      GO TO I99952                                                        !01576
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO SCAN-PINV                                                     !01577
99937 CONTINUE                                                            !01577
        DO WHILE(CHTYPE.EQ.TBLANK)                                        !01578
C          GET-CHARACTER                                                  !01578
          ASSIGN 99722 TO I99804                                          !01578
          GO TO 99804                                                     !01578
99722     CONTINUE                                                        !01578
          ENDDO                                                           !01578
        FOUND=.FALSE.                                                     !01579
        IF(CHTYPE.EQ.TLETTR)THEN                                          !01580
          START=CURSOR                                                    !01581
          IF(.TRUE.)GOTO99721                                             !01582
          DO WHILE(.NOT.(CHTYPE.GT.THYPHN))                               !01582
99721       CONTINUE                                                      !01582
C            GET-CHARACTER                                                !01583
            ASSIGN 99720 TO I99804                                        !01583
            GO TO 99804                                                   !01583
99720       CONTINUE                                                      !01583
            IF(CHTYPE.EQ.THYPHN) FOUND=.TRUE.                             !01584
            ENDDO                                                         !01585
        ELSE                                                              !01587
          IF(CH.EQ.'!')THEN                                               !01588
            COMENT=.TRUE.                                                 !01589
            LSCOMN=LSFLX-CURSOR+1                                         !01590
            SCOMN(1:LSCOMN)=SFLX(CURSOR:LSFLX)                            !01591
            LSFLX=CURSOR-1                                                !01592
            ENDIF                                                         !01593
          ENDIF                                                           !01594
        IF(FOUND)THEN                                                     !01595
          NUNITS=NUNITS+1                                                 !01596
          UTYPE(NUNITS)=UPINV                                             !01597
          USTART(NUNITS)=START                                            !01598
          UFIN(NUNITS)=CURSOR-1                                           !01599
          ENDIF                                                           !01600
      GO TO I99937                                                        !01601
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO SCAN-PINV-OR-FORT                                             !01602
99939 CONTINUE                                                            !01602
        DO WHILE(CHTYPE.EQ.TBLANK)                                        !01603
C          GET-CHARACTER                                                  !01603
          ASSIGN 99719 TO I99804                                          !01603
          GO TO 99804                                                     !01603
99719     CONTINUE                                                        !01603
          ENDDO                                                           !01603
        IF(.NOT.(CHTYPE.EQ.TEOL))THEN                                     !01604
          CSAVE=CURSOR                                                    !01605
C          SCAN-PINV                                                      !01606
          ASSIGN 99718 TO I99937                                          !01606
          GO TO 99937                                                     !01606
99718     CONTINUE                                                        !01606
          IF(FOUND)THEN                                                   !01607
C            SCAN-GARBAGE                                                 !01607
            ASSIGN 99717 TO I99950                                        !01607
            GO TO 99950                                                   !01607
99717       CONTINUE                                                      !01607
          ELSE                                                            !01608
            IF(.NOT.(COMENT))THEN                                         !01609
              NUNITS=NUNITS+1                                             !01610
              UTYPE(NUNITS)=UFORT                                         !01611
              USTART(NUNITS)=CSAVE                                        !01612
              UFIN(NUNITS)=LSFLX                                          !01613
              ENDIF                                                       !01614
            ENDIF                                                         !01615
          ENDIF                                                           !01616
      GO TO I99939                                                        !01617
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO SCAN-STATEMENT-NUMBER                                         !01618
99956 CONTINUE                                                            !01618
        FLXNO=0                                                           !01619
        PASS=.FALSE.                                                      !01620
        DO I=1,5                                                          !01621
C          GET-CHARACTER                                                  !01622
          ASSIGN 99716 TO I99804                                          !01622
          GO TO 99804                                                     !01622
99716     CONTINUE                                                        !01622
          IF((TBLANK).EQ.(CHTYPE))THEN                                    !01624
C       (TDIGIT) FLXNO=FLXNO*10+ICH-ICHZER      !TG 4/1/82                !01625
          ELSEIF((TDIGIT).EQ.(CHTYPE))THEN                                !01626
            FLXNO = FLXNO*10 + ICHAR(CH) - ICHZER                         !01626
          ELSEIF((TEOL).EQ.(CHTYPE))THEN                                  !01627
          ELSE                                                            !01628
            PASS=.TRUE.                                                   !01628
            ENDIF                                                         !01628
          ENDDO                                                           !01630
      GO TO I99956                                                        !01631
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO SORT-TABLE                                                    !01632
99907 CONTINUE                                                            !01632
        P=MAX                                                             !01633
        STACK(MAX)=0                                                      !01634
        ITEMP=MAXSTK-PRIME+1                                              !01635
        DO I=ITEMP,MAXSTK                                                 !01636
          IF(.NOT.(STACK(I).EQ.0))THEN                                    !01637
            STACK(P)=STACK(I)                                             !01638
            IF(.TRUE.)GOTO99715                                           !01639
            DO WHILE(.NOT.(STACK(P).EQ.0))                                !01639
99715         CONTINUE                                                    !01639
              P=STACK(P)                                                  !01640
              LP=P+3                                                      !01641
C              REVERSE-LIST                                               !01642
              ASSIGN 99714 TO I99745                                      !01642
              GO TO 99745                                                 !01642
99714         CONTINUE                                                    !01642
              ENDDO                                                       !01643
            ENDIF                                                         !01644
          ENDDO                                                           !01645
        Q=MAX-1                                                           !01646
        STACK(Q)=0                                                        !01647
        DO WHILE(.NOT.(STACK(MAX).EQ.0))                                  !01648
          P=STACK(MAX)                                                    !01649
          STACK(MAX)=STACK(P)                                             !01650
          QM=Q                                                            !01651
          QP=STACK(QM)                                                    !01652
          INSERT=.FALSE.                                                  !01653
          DO WHILE(.NOT.(INSERT))                                         !01654
            IP=4*(P+4)                                                    !01655
            IQ=4*(QP+4)                                                   !01656
            IF(QP.EQ.0)THEN                                               !01658
              INSERT=.TRUE.                                               !01658
            ELSEIF(CSTAK(IP+1:IP+STACK(P+4)).LT.CSTAK(IQ+1:IQ+STACK(QP+4  !01659
     1)))THEN                                                             !01659
              INSERT=.TRUE.                                               !01660
            ELSE                                                          !01662
              QM=QP                                                       !01663
              QP=STACK(QM)                                                !01664
              ENDIF                                                       !01665
            ENDDO                                                         !01667
          STACK(P)=QP                                                     !01668
          STACK(QM)=P                                                     !01669
          ENDDO                                                           !01670
        PTABLE=STACK(Q)                                                   !01671
      GO TO I99907                                                        !01672
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO STORE-ENDIF                                                   !01673
99838 CONTINUE                                                            !01673
        LSST=11                                                           !01674
        SST(1:11)='      ENDIF'                                           !01675
        SENDIF=SLINE(2:6)                                                 !01676
        ENDIF=.TRUE.                                                      !01677
        IF(COMENT)THEN                                                    !01678
          COMENT=.FALSE.                                                  !01679
          SST(LSST+1:LSST+LSCOMN)=SCOMN(1:LSCOMN)                         !01680
          LSST=LSST+LSCOMN                                                !01681
          ENDIF                                                           !01682
        WRITE(SST(1:5),'(I5.0)')STNO                                      !01683
        STNO=0                                                            !01684
        FLXNO=0                                                           !01685
        NB=2*FLEVEL                                                       !01686
        SST(1:LSST+NB)=SST(1:6)//BLANKS(1:NB)//SST(7:LSST)                !01687
        LSST=LSST+NB                                                      !01688
      GO TO I99838                                                        !01689
      END                                                                 !01690
