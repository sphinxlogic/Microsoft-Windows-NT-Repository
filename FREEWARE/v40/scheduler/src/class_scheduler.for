        PROGRAM CLASS_SCHEDULER
C******************************************************************************
C
C  Author        : Curtis Rempel
C  Creation date : October 11, 1994
C  Revision date : February 23, 1995
C
C  Abstract:
C
C       To implement class based CPU scheduling on OpenVMS V6.0 and higher.
C
C               - Quantum is granted to classes at 1 second (1000 ms) intervals.
C
C                 For scheduling class 1 (10% CPU):
C
C                       10 ticks * (10 ms / tick) 
C                    = 100 ms per 1000 ms interval 
C                    = 10% maximum CPU utilization
C
C               - Processes are grouped into 10 scheduling classes according 
C                 to their UAF ACCOUNT field (i.e. CLASS_n where n = 0..9)
C
C                 Class Definitions:
C
C                       0               no class scheduling
C                       1               10% quantum
C                       2               20% quantum
C                              ...
C                       9               90% quantum
C
C******************************************************************************

        IMPLICIT        NONE            ! catch any undeclared stuff

        PARAMETER       N_CLASS  =   9  ! maximum scheduling classes
        PARAMETER       N_PROC   = 500  ! maximum processes

        INTEGER*4       CLASS_IDX       ! class index to use for a process
        INTEGER*4       CSHC_SIZE       ! class structure size
        INTEGER*4       CSHP_SIZE       ! process structure size
        INTEGER*4       I               ! general purpose counter
        INTEGER*4       PPE_SIZE        ! per process entry size
        INTEGER*4       QUANTUM_SIZE    ! size of quantum array
        INTEGER*4       QUANTUM(0:N_CLASS)! class quantum array
        INTEGER*4       STATUS          ! system service completion status
        INTEGER*4       TRANSLATION_LEN ! logical name translation length

        INTEGER*4       LIB$WAIT        ! sleep function

        INTEGER*4       SYS$DELLNM      ! delete logical name
        INTEGER*4       SYS$SCHED       ! affect process scheduling
        INTEGER*4       SYS$TRNLNM      ! logical name translation

        CHARACTER*255   TRANSLATION     ! logical name translation

        EXTERNAL        SCHED_OPRSHUTDWN

        INCLUDE         '($CSHDEF)'     ! class scheduling definitions
        INCLUDE         '($LNMDEF)'     ! logical name definitions
        INCLUDE         '($PSLDEF)'     ! processor status longword definitions
        INCLUDE         '($SSDEF)'      ! system service completion definitions

        STRUCTURE /ITMLST/
          UNION
            MAP
              INTEGER*2 BUFLEN,ITMCOD
              INTEGER*4 BUFADR,RETADR
            END MAP
            MAP
              INTEGER*4 END_LIST
            END MAP
          END UNION
        END STRUCTURE

        RECORD          /CSHC/          CSHC_LIST(N_CLASS)
        RECORD          /CSHP/          CSHP_LIST(N_PROC)

        RECORD          /ITMLST/        LNMBUF(3)

        LNMBUF(1).BUFLEN = 255
        LNMBUF(1).ITMCOD = LNM$_STRING          ! translation
        LNMBUF(1).BUFADR = %LOC(TRANSLATION)
        LNMBUF(1).RETADR = 0

        LNMBUF(2).BUFLEN = 4
        LNMBUF(2).ITMCOD = LNM$_LENGTH          ! translation length
        LNMBUF(2).BUFADR = %LOC(TRANSLATION_LEN)
        LNMBUF(2).RETADR = 0

        LNMBUF(3).END_LIST = 0

C******************************************************************************
C
C   Get rid of the shutdown logical name in case it exists.
C
C******************************************************************************

        STATUS = SYS$TRNLNM (,'LNM$SYSTEM_TABLE','CLASS_DISABLE',
        2                    ,LNMBUF)
        IF (STATUS .EQ. SS$_NORMAL) THEN
                STATUS = SYS$DELLNM ('LNM$SYSTEM_TABLE',
        2                            'CLASS_DISABLE',
        2                            PSL$C_SUPER)
                IF (.NOT. STATUS) CALL SHUTDOWN (STATUS)
        ELSE IF (STATUS .NE. SS$_NOLOGNAM) THEN
                CALL SHUTDOWN (STATUS)
        END IF

C******************************************************************************
C
C   Initialization.
C
C******************************************************************************

        DO I = 0,N_CLASS
          QUANTUM(I) = I * 10
        END DO

        QUANTUM_SIZE = N_CLASS * 4

C******************************************************************************
C
C   Inform the operator that CPU class scheduling is starting.
C
C******************************************************************************

        CALL OPERATOR ('CPU class scheduling enabled.')

C******************************************************************************
C
C   Obtain data for processes not presently classed scheduled.
C
C******************************************************************************

10      CSHP_SIZE = CSHP$K_LENGTH * N_PROC

        STATUS = SYS$SCHED (%VAL(CSH$_READ_NEW),
        2                   CSHP_LIST,
        2                   CSHP_SIZE,
        2                   PPE_SIZE)
        IF (.NOT. STATUS) CALL SHUTDOWN (STATUS)

        IF (CSHP_SIZE .EQ. 0) GO TO 20

C******************************************************************************
C
C   Determine which class each process should be placed into.
C
C   All processes with an account field of CLASS_1 to CLASS_9 are candidates
C   for class scheduling (CLASS_0 means "no class scheduling").
C
C   Processes exempt from class scheduling include those with an account field
C   of "<start> " (system processes) and those with a base priority greater
C   than 8.
C
C******************************************************************************

        CSHC_SIZE = 0

        DO I = 1,CSHP_SIZE/PPE_SIZE

        IF (CSHP_LIST(I).CSHP$T_ACCOUNT .EQ. '<start> ' .OR.
        2   CSHP_LIST(I).CSHP$B_PRIB .GT. 8 .OR. 
        2   CSHP_LIST(I).CSHP$T_ACCOUNT .EQ. 'CLASS_0' .OR.
        2   (CSHP_LIST(I).CSHP$T_ACCOUNT .NE. 'CLASS_1' .AND.
        2    CSHP_LIST(I).CSHP$T_ACCOUNT .NE. 'CLASS_2' .AND.
        2    CSHP_LIST(I).CSHP$T_ACCOUNT .NE. 'CLASS_3' .AND.
        2    CSHP_LIST(I).CSHP$T_ACCOUNT .NE. 'CLASS_4' .AND.
        2    CSHP_LIST(I).CSHP$T_ACCOUNT .NE. 'CLASS_5' .AND.
        2    CSHP_LIST(I).CSHP$T_ACCOUNT .NE. 'CLASS_6' .AND.
        2    CSHP_LIST(I).CSHP$T_ACCOUNT .NE. 'CLASS_7' .AND.
        2    CSHP_LIST(I).CSHP$T_ACCOUNT .NE. 'CLASS_8' .AND.
        2    CSHP_LIST(I).CSHP$T_ACCOUNT .NE. 'CLASS_9')) THEN
                CLASS_IDX = -1
        ELSE
                READ (CSHP_LIST(I).CSHP$T_ACCOUNT(7:7),'(I1)') CLASS_IDX
        END IF

C******************************************************************************
C
C   Update class list with this process.
C
C******************************************************************************

        CSHC_LIST(I).CSHC$L_EPID = CSHP_LIST(I).CSHP$L_EPID
        CSHC_LIST(I).CSHC$W_CLASS = CLASS_IDX
        CSHC_LIST(I).CSHC$W_WINDFALL = 0

        CSHC_SIZE = CSHC_SIZE + CSHC$K_LENGTH

        END DO

C******************************************************************************
C
C   Set up quantum for each class.
C
C******************************************************************************

20      STATUS = SYS$SCHED (%VAL(CSH$_SET_QUANT),
        2                   QUANTUM,
        2                   QUANTUM_SIZE,
        2                   )
        IF (.NOT. STATUS) CALL SHUTDOWN (STATUS)

        IF (CSHP_SIZE .EQ. 0) GO TO 30

C******************************************************************************
C
C   Place each process into the required scheduling class.
C
C******************************************************************************

        STATUS = SYS$SCHED (%VAL(CSH$_SET_CLASS),
        2                   CSHC_LIST,
        2                   CSHC_SIZE,
        2                   CSHC$K_LENGTH)
        IF (.NOT. STATUS) CALL SHUTDOWN (STATUS)

C******************************************************************************
C
C   Wait 1 second.
C
C******************************************************************************

30      STATUS = LIB$WAIT (1.0)
        IF (.NOT. STATUS) CALL SHUTDOWN (STATUS)

C******************************************************************************
C
C   Check for CPU class scheduling disable request.
C
C******************************************************************************

        STATUS = SYS$TRNLNM (,'LNM$SYSTEM_TABLE','CLASS_DISABLE',
        2                    ,LNMBUF)
        IF (STATUS .EQ. SS$_NORMAL) THEN
                CALL SHUTDOWN (%LOC(SCHED_OPRSHUTDWN))
        ELSE IF (STATUS .EQ. SS$_NOLOGNAM) THEN
                GO TO 10
        ELSE
                CALL SHUTDOWN (STATUS)
        END IF

9999    END
