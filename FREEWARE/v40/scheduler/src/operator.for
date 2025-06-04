        SUBROUTINE OPERATOR (TEXT)
C******************************************************************************
C
C  Author        : Curtis Rempel
C  Date          : October 20, 1994
C  Last modified : October 20, 1994
C
C  Files used    : <none>
C
C  Purpose:
C
C       To send a message to the central system operator.
C
C******************************************************************************

        IMPLICIT        NONE            ! catch any undeclared stuff

        BYTE            PACKET_TYPE     ! packet id code

        INTEGER*4       OPER_CLASS      ! operator class
        INTEGER*4       STATUS          ! system service completion status

        INTEGER*4       SYS$SNDOPR      ! send a message to the operator 

        CHARACTER*8     HEADER          ! item list control information
        CHARACTER*(*)   TEXT            ! text sent to the operator 

        EQUIVALENCE     (PACKET_TYPE,HEADER(1:1))
        EQUIVALENCE     (OPER_CLASS,HEADER(2:3))

        INCLUDE         '($OPCDEF)'
        INCLUDE         '($SSDEF)'

C******************************************************************************
C
C   Initialize the buffer to indicate that we are sending a request to the
C   central system operator.
C
C******************************************************************************

        PACKET_TYPE = OPC$_RQ_RQST
        OPER_CLASS = OPC$M_NM_CENTRL

C******************************************************************************
C
C   Let it rip ...
C
C******************************************************************************

        STATUS = SYS$SNDOPR (HEADER//TEXT,)
        IF (.NOT. STATUS) CALL SHUTDOWN (STATUS)

        RETURN

9999    END
