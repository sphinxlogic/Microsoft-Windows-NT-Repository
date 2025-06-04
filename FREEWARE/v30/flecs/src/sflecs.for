C                                                                         !00001
C  FLECS subroutine support:                                              !00002
C                                                                         !00003
C  The subroutines below form the machine and I/O interface               !00004
C  for the FLECS translator.  Please see the FLECS system                 !00005
C  modification guide for details.                                        !00006
C                                                                         !00007
C  The FLECS system modification guide may be obtained                    !00008
C  by writing to                                                          !00009
C                    Terry Beyer                                          !00010
C                    Computing Center                                     !00011
C                    University of Oregon                                 !00012
c                    Eugene, Oregon 97403                                 !00013
C                                                                         !00014
C                                                                         !00015
C---------------------------------------                                  !00016
C                                                                         !00017
C  Disclaimer                                                             !00018
C                                                                         !00019
C     Neither the author nor the University of Oregon shall be            !00020
C  libel for any direct or indirect, incidental, consequential,           !00021
C  or specific damages of any kind or from any cause whatsoever           !00022
C  arising out of or in any way connected with the use or                 !00023
C  performance of this program.                                           !00024
C                                                                         !00025
C---------------------------------------                                  !00026
C                                                                         !00027
C  Permission                                                             !00028
C                                                                         !00029
C     This program is in the public domain and may be altered             !00030
C  or reproduced without explicit permission of the author.               !00031
C                                                                         !00032
C----------------------------------------                                 !00033
C Modified by William Tanenbaum 7-13-79                                   !00034
C for Digital VAX 11/780 using CHARACTER data type                        !00035
C All subroutines except NEWNO have been extensively                      !00036
C changed from original versions described in                             !00037
C the FLEX modification guide.  Several subroutines                       !00038
C have even disappeared!                                                  !00039
C                                                                         !00040
C-----------------------------------------                                !00041
C                                                                         !00042
      SUBROUTINE OPENF(CALLNO,DONE,SVER)                                  !00043
C                                                                         !00044
C  OPENF IS THE FILE OPENING SUBROUTINE FOR THE FLECS TRANSLATOR.         !00045
C  FOR A DESCRIPTION SEE THE FLECS SYSTEM MODIFICATION GUIDE,             !00046
C  SECTION 6.1.                                                           !00047
C                                                                         !00048
C                                                                         !00049
      COMMON /FILES/ DOLIST, DOFORT, DEBUGG                               !00050
      COMMON /INCFIL/ LVERBOSE, DOINC, NOLIST, INCFLI, LINC               !00051
      LOGICAL LVERBOSE,                                                   !00052
     1DOFORT, DOLIST, DEBUGG, DOINC, INCFLI, LINC                         !00053
      INTEGER CALLNO,SVER,IX,INDEX,LONG,ISLEN,ISBEG,IY,IEND               !00054
      LOGICAL DONE                                                        !00055
      DIMENSION SVER(1)                                                   !00056
c       clns - increase length of fortran and listing file spec max       !00057
c       12 was too short for 9.3, even. 80 should be ok for 39.39         !00058
      CHARACTER INPUT*72,FORTRAN*80,LISTING*80,STRING*80                  !00059
      DONE=.TRUE.                                                         !00061
      DEBUGG=.FALSE.                                                      !00062
      LVERBOSE=.FALSE.        !CLNS                                       !00063
      DOLIST=.TRUE.                                                       !00064
      DOFORT=.TRUE.                                                       !00065
      DOINC = .TRUE.                                                      !00066
      INPUT=' '                                                           !00067
      LISTING=' '                                                         !00068
      FORTRAN=' '                                                         !00069
      IF(CALLNO.EQ.1)THEN                                                 !00070
        ISTAT=LIB$GET_FOREIGN(STRING,,ISLEN)                              !00071
        IF(ISLEN.EQ.0)THEN                                                !00072
          TYPE 100                                                        !00073
  100     FORMAT(' File(s): '$)                                           !00074
          ACCEPT 110, ISLEN,STRING                                        !00075
  110     FORMAT(Q,A)                                                     !00076
          ENDIF                                                           !00077
        ISBEG=1                                                           !00078
        ENDIF                                                             !00079
      IF(.NOT.(ISBEG.GT.ISLEN))THEN                                       !00080
        IX=INDEX(STRING(1:ISLEN),',')                                     !00081
        IY=INDEX(STRING(1:ISLEN),'+')                                     !00082
        IF(IX*IY.EQ.0)THEN                                                !00083
          IEND=IX+IY                                                      !00083
        ELSE                                                              !00084
          IEND=MIN0(IX,IY)                                                !00084
          ENDIF                                                           !00084
        IF(IEND.EQ.0)THEN                                                 !00085
          IEND=ISLEN+1                                                    !00085
        ELSE                                                              !00086
          STRING(IEND:IEND)=' '                                           !00086
          ENDIF                                                           !00086
        LONG=IEND-ISBEG                                                   !00087
        INPUT(1:LONG)=STRING(ISBEG:IEND-1)                                !00088
        ISBEG=IEND+1                                                      !00089
        IX=INDEX(INPUT(1:LONG),'/')                                       !00090
        IF(IX.NE.0)THEN                                                   !00091
C          PROCESS-SWITCHES                                               !00092
          ASSIGN 99998 TO I99999                                          !00092
          GO TO 99999                                                     !00092
99998     CONTINUE                                                        !00092
          LONG=IX-1                                                       !00093
C                                                                         !00095
C  MODIFIDED 9/17/81 TO ALLOW FLEXING OUTSIDE DEFAULT AREA                !00096
C       updated 5/25/83 to allow just DEV:FILE.EXT;ver                    !00097
C                                                                         !00098
          ENDIF                                                           !00094
        ICOLN=INDEX(INPUT(1:LONG),':')                                    !00099
        IBRKT=INDEX(INPUT(1:LONG),']')                                    !00100
        IF(IBRKT.EQ.0)THEN! NO ']'                                        !00101
          IF(ICOLN.EQ.0)THEN                                              !00102
            ISTART=1                                                      !00102
          ELSE                                                            !00103
            ISTART=ICOLN+1                             !MAYBE JUST DEV:   !00103
            ENDIF                                                         !00103
        ELSE! FILE ELSEWHERE                                              !00105
          ISTART=IBRKT+1                                                  !00106
          ENDIF                                                           !00107
        IX=INDEX(INPUT(ISTART:LONG),'.')                ! NEW             !00108
C                                                                         !00109
C       IX=INDEX(INPUT(1:LONG),'.')                     ! OLD             !00110
C                                                                         !00111
        IF(IX.EQ.0)THEN                                                   !00112
          INPUT(LONG+1:LONG+4)='.FLX'                                     !00112
        ELSE                                                              !00113
          LONG=ISTART+IX-2                           !clns                !00113
C                                                                         !00114
C THIS OPENS THE FOR AND FLI FILES IN THE CURRENT AREA                    !00115
C                                                                         !00116
C       LISTING(1:LONG)=INPUT(1:LONG)                   ! OLD             !00117
C       LISTING(LONG+1:LONG+4)='.FLI'                   ! OLD             !00118
C       FORTRAN(1:LONG)=INPUT(1:LONG)                   ! OLD             !00119
C       FORTRAN(LONG+1:LONG+4)='.FOR'                   ! OLD             !00120
C                                                                         !00121
          ENDIF                                                           !00113
        NEWLONG=LONG-ISTART+1                           ! NEW             !00122
        LISTING(1:NEWLONG)=INPUT(ISTART:LONG)           ! NEW             !00123
        LISTING(NEWLONG+1:NEWLONG+4)='.FLI'             ! NEW             !00124
        FORTRAN(1:NEWLONG)=INPUT(ISTART:LONG)           ! NEW             !00125
        FORTRAN(NEWLONG+1:NEWLONG+4)='.FOR'             ! NEW             !00126
C                                                                         !00127
C END OF MODIDIFICATIONS - 9/17/81                                        !00128
C                                                                         !00129
clns    don't type file names unless /VERBOSE is set                      !00130
        IF (LVERBOSE)   TYPE 1000, INPUT(1:LONG+4)                        !00131
 1000   FORMAT(1X,A)                                                      !00132
        OPEN(UNIT=1,FILE=INPUT(1:LONG+4),STATUS='OLD',READONLY,SHARED)    !00133
        IF(DOLIST)THEN                                                    !00134
          OPEN(UNIT=3,FILE=LISTING,STATUS='NEW',CARRIAGECONTROL='LIST')   !00135
          IF (LVERBOSE)   TYPE 1000, LISTING                              !00136
          ENDIF                                                           !00137
        IF(DOFORT)THEN                                                    !00138
          OPEN(UNIT=2,FILE=FORTRAN,STATUS='NEW',CARRIAGECONTROL='LIST')   !00139
          IF (LVERBOSE)   TYPE 1000, FORTRAN                              !00140
          ENDIF                                                           !00141
        DONE=.FALSE.                                                      !00142
        ENDIF                                                             !00143
      RETURN                                                              !00144
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO PROCESS-SWITCHES                                              !00145
99999 CONTINUE                                                            !00145
        IF (INDEX(INPUT(IX:LONG),'/D').NE.0) DEBUGG=.TRUE.                !00146
        IF (INDEX(INPUT(IX:LONG),'/NOL').NE.0) DOLIST=.FALSE.             !00147
        IF (INDEX(INPUT(IX:LONG),'/NOF').NE.0) DOFORT=.FALSE.             !00148
        IF (INDEX(INPUT(IX:LONG),'/NOI').NE.0) DOINC = .FALSE.            !00149
clns    add verbosity switch for echo of output file names                !00150
        IF (INDEX(INPUT(IX:LONG),'/VER').NE.0) LVERBOSE = .TRUE.          !00151
        INPUT(IX:LONG)=' '                                                !00152
      GO TO I99999                                                        !00153
      END                                                                 !00154
C                                                                         !00155
C*************************************************************************!00156
C*************************************************************************!00157
      SUBROUTINE GET(LSTR,LINENO,STRING,ENDFIL)                           !00158
C                                                                         !00159
C  GET IS THE INPUT SUBROUTINE FOR THE FLECS TRANSLATOR.                  !00160
C  FOR A DESCRIPTION SEE THE FLECS SYSTEM MODIFICATION GUIDE,             !00161
C  SECTION 6.2.                                                           !00162
C                                                                         !00163
      COMMON /INCFIL/ LVERBOSE, DOINC, NOLIST, INCFLI, LINC               !00164
      LOGICAL LVERBOSE                                                    !00165
      LOGICAL ENDFIL, DOINC, LINC, INCFLI, NOVAX                          !00166
      CHARACTER*80 TEMP, STRING                                           !00167
      CHARACTER*30 INKLUDE_FILE                                           !00168
      CHARACTER*1 TAB, VT, FF, APOS,ITERM                                 !00169
      INTEGER LINENO, LEN, LSTR, I, K, MOD, NSPACE                        !00170
      DATA TAB/9/, VT/11/, FF/12/, APOS/39/                               !00171
      DATA IIN/1/, LINC/.FALSE./                                          !00172
   10 FORMAT(Q,A)                                                         !00173
C                                                                         !00174
      IF(.TRUE.)GOTO99999                                                 !00175
      DO WHILE(.NOT.(K.GT.0))                                             !00175
99999   CONTINUE                                                          !00175
   15   READ(IIN,10,END=20) LEN,TEMP                                      !00176
C        REMOVE-TAB-CHARACTERS                                            !00177
        ASSIGN 99997 TO I99998                                            !00177
        GO TO 99998                                                       !00177
99997   CONTINUE                                                          !00177
        LINENO=LINENO+1                                                   !00178
        DO WHILE(STRING(K:K).EQ.' ')                                      !00179
          K=K-1                                                           !00179
          ENDDO                                                           !00179
        INX=INDEX(STRING(1:K),'!')                                        !00180
        IF(INX.EQ.0)THEN                                                  !00182
        ELSEIF(INDEX(STRING(INX:K),'VAX').NE.0)THEN                       !00183
          IF(STRING(1:1).NE.'C')THEN                                      !00185
          ELSEIF(STRING(1:2).EQ.'CD')THEN                                 !00186
            STRING(1:2)='D '                                              !00186
          ELSE                                                            !00187
            STRING(1:1)=' '                                               !00187
            ENDIF                                                         !00187
        ELSEIF(INDEX(STRING(INX:K),'PDP10').NE.0)THEN                     !00190
          STRING(1:1)='C'                                                 !00190
          ENDIF                                                           !00190
C                                                                         !00192
C       Intercept the include's:                                          !00193
C                                                                         !00194
        IF(DOINC .AND. STRING(1:1).NE.'C')THEN                            !00195
          INX = INDEX(STRING(1:K),'INCLUDE')      !look for the keyword   !00196
C                                                                         !00197
          IF(INX.GT.0)THEN                                                !00198
C            SCAN-INCLUDE-FILE                                            !00199
            ASSIGN 99995 TO I99996                                        !00199
            GO TO 99996                                                   !00199
99995       CONTINUE                                                      !00199
C                                                                         !00200
            IF(.NOT.NOVAX)THEN                                            !00201
C       There were !vax and/or !pdp10 lines, so have FLECS                !00202
C       process the INCLUDE file:                                         !00203
              INCFLI = (NOLIST.EQ.0)          !put INCLUDE file in the FLI!00204
              STRING(1:1) = 'C'                                           !00205
              LINC = .TRUE.                                               !00206
              IIN = 4                                                     !00207
C                                                                         !00209
              ENDIF                                                       !00208
C                                                                         !00211
            ENDIF                                                         !00210
C                                                                         !00213
          ENDIF                                                           !00212
        IF(K.GT.80)THEN                                                   !00214
          K = 80                                                          !00215
C                                                                         !00216
          DO WHILE(STRING(K:K).EQ.' ')                                    !00217
            K = K - 1                                                     !00217
            ENDDO                                                         !00217
C                                                                         !00218
C                                                                         !00220
          ENDIF                                                           !00219
        ENDDO                                                             !00221
C                                                                         !00222
      LSTR = K                                                            !00223
      RETURN                                                              !00224
C                                                                         !00225
   20 CONTINUE                                                            !00226
C                                                                         !00227
      IF(LINC)THEN                                                        !00228
        LINC = .FALSE.                                                    !00229
        CLOSE(UNIT=4)                                                     !00230
        IIN = 1                                                           !00231
        GO TO 15                                                          !00232
C                                                                         !00234
        ENDIF                                                             !00233
      ENDFIL = .TRUE.                                                     !00235
      LINENO = 0                                                          !00236
      RETURN                                                              !00237
C                                                                         !00238
C*************************************************************************!00239
C*************************************************************************!00240
C                                                                         !00241
C                                                                         !00242
C-------------------------------------------------------------------------!00243
C                                                                         !00244
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO REMOVE-TAB-CHARACTERS                                         !00245
99998 CONTINUE                                                            !00245
C                                                                         !00246
        I = 0                                                             !00247
        K = 0                                                             !00248
C                                                                         !00249
        DO WHILE(K.LT.80 .AND. I.LT.LEN)                                  !00250
          I = I + 1                                                       !00251
C                                                                         !00252
C                                                                         !00254
          IF((TAB).EQ.(TEMP(I:I)))THEN                                    !00255
            NSPACE = 8 - MOD(K,8)                                         !00256
C                                                                         !00257
            DO J=1,NSPACE                                                 !00258
              K = K + 1                                                   !00259
              STRING(K:K) = ' '                                           !00260
              ENDDO                                                       !00261
C                                                                         !00262
C                                                                         !00264
          ELSEIF((FF).EQ.(TEMP(I:I)))THEN                                 !00265
C                                                                         !00266
          ELSEIF((VT).EQ.(TEMP(I:I)))THEN                                 !00267
C                                                                         !00268
          ELSE                                                            !00269
            K = K + 1                                                     !00270
            STRING(K:K) = TEMP(I:I)                                       !00271
C                                                                         !00273
            ENDIF                                                         !00272
C                                                                         !00275
          ENDDO                                                           !00276
C                                                                         !00277
      GO TO I99998                                                        !00278
C                                                                         !00279
C-------------------------------------------------------------------------!00280
C                                                                         !00281
C                                                                         !     
C----------------------------------------                                 !     
C                                                                         !     
C        TO SCAN-INCLUDE-FILE                                             !00282
99996 CONTINUE                                                            !00282
C                                                                         !00283
C       Added by Tom Gentile 4/1/82 to make FLECS process INCLUDE files.  !00284
C-------                                                                  !00285
C       Modified 5/12/83 by S.Ball to be compatible with DEC-10 formats:  !00286
C       1. always include file if double quote sign seen                  !00287
C       2. never include file if apostrophe-parenthesis seen              !00288
C               (VMS text library module extraction done by FORTRAN)      !00289
C       3. always include file if parenthesis seen alone (10 FLI)         !00290
C       4. conditionally include file if apostrophe seen alone            !00291
C               (previous VMS inclusion format)                           !00292
D       type *, 'scan-include-file'                                       !00294
D       type *,'line="',string(1:k),'"'                                   !00295
        NOVAX = .TRUE.                                                    !00296
C                                                                         !00297
C       Pick off the file specification:                                  !00298
        IST1 = INDEX(STRING(1:K),'"') + 1       !find 1st quote           !00299
        IST2 = INDEX(STRING(1:K),'''(') + 1     ! apostrophe parenthesis  !00300
        IST3 = INDEX(STRING(1:K),'(') + 1       ! Parenthesis             !00301
        IST4 = INDEX(STRING(1:K),APOS) + 1      ! apostrophe              !00302
D       TYPE *,'IST1=',IST1,', IST2=',IST2,', IST3=',IST3,', IST4=',IST4  !00303
        IF(IST1.GT.1)THEN                                                 !00305
C       A quote seen: always include the file                             !00306
          ITERM='"'                                                       !00307
          IST=IST1                                                        !00308
        ELSEIF(IST2.GT.1)THEN                                             !00310
C       Apostrophe parenthesis seen: never include the file               !00311
          NOVAX=.TRUE.                                                    !00312
          GOTO 240                                                        !00313
        ELSEIF(IST3.GT.1)THEN                                             !00315
C       A parenthesis seen: always include file                           !00316
          ITERM=')'                                                       !00317
          IST=IST3                                                        !00318
        ELSEIF(IST4.GT.1)THEN                                             !00320
C       Apostrophe seen: maybe include file                               !00321
          ITERM=''''                                                      !00322
          IST=IST4                                                        !00323
        ELSE                                                              !00325
C       No file spec seen                                                 !00326
          NOVAX=.TRUE.                                                    !00327
          GOTO 240                                                        !00328
          ENDIF                                                           !00329
        IF(IST.GT.INX)THEN                                                !00332
C       Position of 1st quote must be > than position of INCLUDE          !00333
C                                                                         !00334
C       CLNS:                                                             !00335
C       Scan the interval between "INCLUDE" and the file spec:            !00336
C       there should be only spaces or tabs there.                        !00337
C                                                                         !00338
          IP=INX+6                                                        !00339
          IPBAD=0                                                         !00340
          IF(.TRUE.)GOTO99994                                             !00341
          DO WHILE(IPBAD.EQ.0 .AND. IP.LT.IST-2)                          !00341
99994       CONTINUE                                                      !00341
            IP=IP+1                                                       !00342
            IPBAD=INDEX(                                                  !00343
     1      'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz'        !00344
     1      //'1234567890@#$%^&*);:|\}{?><,./`~+=-_',                     !00345
     2      string(IP:IP) )                                               !00346
            ENDDO                                                         !00347
          IF(IPBAD.NE.0)THEN                                              !00348
D       TYPE *,' IPBAD=',IPBAD                                            !00349
D       TYPE 1313,STRING(:IP-1),'/\',STRING(IP:)                          !00350
D1313   FORMAT(1X,3A)                                                     !00351
            NOVAX=.TRUE.                                                  !00352
            GOTO 240                                                      !00353
C                                                                         !00355
            ENDIF                                                         !00354
          NOLIST = INDEX(STRING(1:K),'/NOL')      !check for a /NOLIST opt!00356
          ISLASH = INDEX(STRING(1:K),'/')   !clns: might be another qualif!00357
C                                                                         !00358
          IF(ISLASH.EQ.0)THEN                                             !00359
C       There wasn't a qualifier, so just find the next terminator:       !00360
            IFN = INDEX(STRING(IST+1:K),iterm) - 1                        !00361
            IFN = INDEX(STRING(IST+1:K),iterm) - 1                        !00362
C                                                                         !00364
          ELSE                                                            !00365
C       There was a qualifier, so the end of the file name is             !00366
C       just before the /:                                                !00367
            IFN = ISLASH - IST - 1                                        !00368
C                                                                         !00370
            ENDIF                                                         !00369
          INKLUDE_FILE = STRING(IST:IST+IFN)      !get the file specificat!00371
D       type *,' inklude_file=',inklude_file                              !00372
          OPEN(UNIT=4,FILE=INKLUDE_FILE,STATUS='OLD',                     !00373
     *    READONLY,SHARED,ERR=220)                                        !00374
C                                                                         !00375
CLNS    if a quote (")  or parenthesis was seen,                          !00376
Clns    Always include the file - the user has specified it               !00377
C                                                                         !00378
          IF(IST.EQ.IST1 .OR. IST.EQ.IST3) NOVAX=.FALSE.  !CLNS           !00379
C                                                                         !00380
C       Now scan through the file and see if any lines have a "pdp10"     !00381
C       or a "vax": at this point, NOVAX=.true. iff ITST4 .GT. 1          !00382
C                                                                         !00383
          DO WHILE(NOVAX)                                                 !00384
            READ(4,10,END=210) LEN, TEMP                                  !00385
            INX = INDEX(TEMP(1:LEN),'VAX') + INDEX(TEMP(1:LEN),'PDP10')   !00386
            IF(INX.NE.0) NOVAX = .FALSE.                                  !00387
            ENDDO                                                         !00388
C                                                                         !00389
  210     IF(.NOT.NOVAX) REWIND 4                                         !00390
C                                                                         !00392
          ENDIF                                                           !00391
        GO TO 240               !jump over the OPEN error handler         !00393
C                                                                         !00394
  220   CONTINUE                                                          !00395
        INKMXC=INDEX(INKLUDE_FILE,'    ')                                 !00396
        TYPE 230, INKLUDE_FILE(1:INKMXC)                                  !00397
  230   FORMAT(' %FLECS-W-NOINCL - INCLUDE file ''', A, ''' not found.')  !00398
  240   CONTINUE                                                          !00399
      GO TO I99996                                                        !00399
C                                                                         !00400
      END                                                                 !00401
C                                                                         !00402
C*************************************************************************!00403
C                                                                         !00404
      SUBROUTINE PUT(SLINE,STRING,IOCLAS)                                 !00405
C                                                                         !00406
C  PUT IS THE OUTPUT ROUTINE FOR THE FLECS TRANSLATOR.                    !00407
C  FOR A DESCRIPTION SEE THE FLECS SYSTEM MODIFICATION GUIDE,             !00408
C  SECTION 6.3.                                                           !00409
C                                                                         !00410
C  ASSUMPTIONS--                                                          !00411
C     -- FILE FORTOUT IS WRITTEN ON FORTRAN UNIT FOUT.                    !00412
C        (SEE DATA STATEMENT BELOW)                                       !00413
C     -- FILE LISTOUT IS WRITTEN ON FORTRAN UNIT LOUT.                    !00414
C        (SEE DATA STATEMENT BELOW)                                       !00415
C     -- THE LINE NUMBERS TO BE WRITTEN TO FORTOUT SHOULD APPEAR          !00416
C        IN COLUMNS 76 THROUGH 80.                                        !00417
C     -- OUTPUT CLASS ERR IS TO APPEAR ONLY ON THE LISTING.               !00418
C                                                                         !00419
C  NOTE TIMESHARING SYSTEMS SHOULD HAVE CODE ADDED WHICH SENDS ALL        !00420
C  ERR CLASS OUTPUT TO THE USERS TERMINAL AS WELL AS TO THE LISTING.      !00421
C                                                                         !00422
      CHARACTER*(*) STRING                                                !00423
      CHARACTER*80 BLANKS                                                 !00424
      CHARACTER*5 SLINE                                                   !00425
      CHARACTER*1 TAB                                                     !00426
      INTEGER ERR                                                         !00427
      INTEGER FOUT, LOUT                                                  !00428
      LOGICAL DOLIST, DOFORT, DEBUGG, DOINC, INCFLI, LINC                 !00429
C                                                                         !00430
C                                                                         !00431
C  OUTPUT CONTROL PARAMETERS                                              !00432
      COMMON/FILES/DOLIST,DOFORT,DEBUGG                                   !00433
      COMMON /INCFIL/ LVERBOSE, DOINC, NOLIST, INCFLI, LINC               !00434
C                                                                         !00435
C  MNEMONICS FOR IOCLASSES                                                !00436
      DATA  LIST/2/, ERR/3/                                               !00437
C                                                                         !00438
C  FORTOUT AND LISTOUT UNIT NUMBERS                                       !00439
      DATA FOUT/2/, LOUT/3/                                               !00440
C                                                                         !00441
C BLANKS WILL BE RIGHT FILLED WITH SPACES                                 !00442
      DATA BLANKS(1:74)/' '/                                              !00443
      DATA BLANKS(75:75)/'!'/                                             !00444
      DATA TAB/9/                                                         !00445
C                                                                         !00446
      LSTR = LEN(STRING)                                                  !00447
C                                                                         !00448
      IF(DOLIST)THEN                                                      !00449
C                                                                         !00450
        IF(LINC)THEN                                                      !00451
C       If we are processing an INCLUDE file, only write it out to the    !00452
C       FLI file if it had !vax or !pdp10 lines and did not               !00453
C       have a /NOLIST option:                                            !00454
C                                                                         !00455
          IF(NOLIST.GT.0)THEN                                             !00456
            STRING(1:1) = ' '                                             !00457
            NOLIST = 0                                                    !00458
C                                                                         !00460
            ENDIF                                                         !00459
          IF(INCFLI .OR. NOLIST.EQ.0)THEN                                 !00461
            WRITE(LOUT,40) ' '//SLINE//' '//STRING(1:LSTR)                !00462
            NOLIST = -1     !NOLIST = 0 forces the INCLUDE statement to be!00463
C                                                                         !00465
            ENDIF                                                         !00464
C                                                                         !00467
        ELSE                                                              !00468
          WRITE(LOUT,40) ' '//SLINE//' '//STRING(1:LSTR)                  !00469
C                                                                         !00471
          ENDIF                                                           !00470
C                                                                         !00473
        ENDIF                                                             !00472
      IF (IOCLAS.EQ.ERR) TYPE 40, ' '//SLINE//' '//STRING(1:LSTR)         !00474
      RETURN                                                              !00475
C                                                                         !00476
C-------------------------------------------------------------------------!00477
C                                                                         !00478
      ENTRY PUTZ(STRING,IOCLAS)                                           !00479
      LSTR=LEN(STRING)                                                    !00480
      IF(DOLIST)THEN                                                      !00481
        WRITE(LOUT,40) BLANKS(1:7)//STRING(1:LSTR)                        !00482
        ENDIF                                                             !00483
      IF (IOCLAS.EQ.ERR) TYPE 40, BLANKS(1:7)//STRING(1:LSTR)             !00484
      RETURN                                                              !00485
C                                                                         !00486
C-------------------------------------------------------------------------!00487
C                                                                         !00488
      ENTRY PUTF(SLINE,STRING)                                            !00489
      IF(DOFORT)THEN                                                      !00490
        LSTR=MIN0(LEN(STRING),74)                                         !00491
        WRITE(FOUT,40) STRING(1:LSTR)//BLANKS(LSTR+1:75)//SLINE           !00492
        ENDIF                                                             !00493
      RETURN                                                              !00494
   40 FORMAT(A)                                                           !00495
      END                                                                 !00496
C                                                                         !00497
C*************************************************************************!00498
C                                                                         !00499
      SUBROUTINE PUTN(SLINE,STRING,IOCLAS)                                !00500
      CHARACTER*(*) STRING                                                !00501
      CHARACTER*5 SLINE                                                   !00502
      LOGICAL DOLIST,DOFORT,DEBUGG                                        !00503
C                                                                         !00504
C                                                                         !00505
C  OUTPUT CONTROL PARAMETERS                                              !00506
      COMMON/FILES/DOLIST,DOFORT,DEBUGG                                   !00507
C                                                                         !00508
C  MNEMONICS FOR IOCLASSES                                                !00509
      DATA  LIST/2/, ERR/3/                                               !00510
C                                                                         !00511
C  FORTOUT AND LISTOUT UNIT NUMBERS                                       !00512
      DATA FOUT/2/, LOUT/3/                                               !00513
C  OUTPUT FORMAT USED TO INDICATE DELETED LINE.                           !00514
   30 FORMAT('+-----')                                                    !00515
      CALL PUT(SLINE,STRING,IOCLAS)                                       !00516
      IF (DOLIST) WRITE(LOUT,30)                                          !00517
      RETURN                                                              !00518
      END                                                                 !00519
C                                                                         !00520
C*************************************************************************!00521
C                                                                         !00522
      SUBROUTINE CLOSEF(MINCNT,MAJCNT)                                    !00523
      INTEGER MINCNT,MAJCNT                                               !00524
      LOGICAL DOLIST,DOFORT,DEBUGG                                        !00525
C                                                                         !00526
C  OUTPUT PARAMETERS                                                      !00527
      COMMON/FILES/DOLIST,DOFORT,DEBUGG                                   !00528
C                                                                         !00529
C  CLOSEF IS THE FILE CLOSING SUBROUTINE FOR THE FLECS TRANSLATOR         !00530
C  FOR A DESCRIPTION SEE THE FLECS SYSTEM MODIFICATION GUIDE,             !00531
C  SECTION 6.4.                                                           !00532
C                                                                         !00533
clns    no output message if no errors                                    !00534
      IF(MINCNT+MAJCNT.GE.1)THEN                                          !00535
        TYPE 100, MINCNT,MAJCNT                                           !00536
  100   FORMAT(' There were',I4,' Minor and',I4,' Major errors.')         !00537
        ENDIF                                                             !00538
      CLOSE(UNIT=1)                                                       !00539
      IF (DOFORT) CLOSE(UNIT=2)                                           !00540
      IF (DOLIST) CLOSE(UNIT=3)                                           !00541
      RETURN                                                              !00542
      END                                                                 !00543
C                                                                         !00544
C*************************************************************************!00545
C                                                                         !00546
      INTEGER FUNCTION HASH(A,PRIME)                                      !00547
C                                                                         !00548
C  HASH COMPUTES AN INTEGER IN THE RANGE 0 TO PRIME-1 BY HASHING          !00549
C  THE STRING A INCLUDING ITS LENGTH.                                     !00550
C  FOR A DESCRIPTION SEE THE FLECS SYSTEM MODIFICATION GUIDE,             !00551
C  SECTION 4.8.                                                           !00552
C                                                                         !00553
      INTEGER PRIME,LEN,L,I,ICH                                           !00554
C                                                                         !00555
      CHARACTER*(*) A                                                     !00556
C                                                                         !00557
      L=LEN(A)                                                            !00558
C                                                                         !00559
C  HASH THE LENGTH AND ALL WORDS, REDUCING                                !00560
C  EACH MOD PRIME BEFORE SUMMING TO AVOID INTEGER OVERFLOW.               !00561
      HASH=L                                                              !00562
      DO I=1,L                                                            !00563
        ICH=ICHAR(A(I:I))                                                 !00564
        HASH=HASH+ICH-(ICH/PRIME)*PRIME                                   !00565
        ENDDO                                                             !00566
C                                                                         !00567
C  COMPLETE HASHING                                                       !00568
      IF(HASH.LT.0) HASH=-HASH                                            !00569
      HASH=HASH-(HASH/PRIME)*PRIME                                        !00570
      RETURN                                                              !00571
      END                                                                 !00572
      INTEGER FUNCTION NEWNO(N)                                           !00573
C                                                                         !00574
C  NEWNO IS A SEQUENTIAL NUMBER GENERATOR.                                !00575
C  FOR A DESCRIPTION SEE THE FLECS SYSTEM MODIFICATION GUIDE.             !00576
C                                                                         !00577
C  AUTHOR -- TERRY BEYER                                                  !00578
C  VERSION -- 2.1                                                         !00579
C  DATE -- AUGUST 12, 1974                                                !00580
C                                                                         !00581
C  THIS SUBPROGRAM IS MACHINE INDEPENDENT.                                !00582
C                                                                         !00583
      INTEGER N,OLDNO                                                     !00584
      DATA OLDNO/0/                                                       !00585
      IF(N.NE.0)THEN                                                      !00586
        NEWNO=N                                                           !00586
      ELSE                                                                !00587
        NEWNO=OLDNO-1                                                     !00587
        ENDIF                                                             !00587
      OLDNO = NEWNO                                                       !00588
      RETURN                                                              !00589
      END                                                                 !00590
