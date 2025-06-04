        SUBROUTINE SHUTDOWN (EXIT_STATUS)
C******************************************************************************
C
C  Author        : Curtis Rempel
C  Date          : October 20, 1994
C  Last modified : October 20, 1994
C
C  File used     : <none>
C
C  Purpose:
C
C       To gracefully shut down the CPU class scheduling process with a final
C  status message logged to OPCOM. 
C
C******************************************************************************

        IMPLICIT        NONE            ! catch any undeclared stuff
        
        INTEGER*2       MSG_LEN         ! length of message text

        INTEGER*4       EXIT_STATUS     ! CPU class scheduling completion status
        INTEGER*4       STATUS          ! system service completion status

        INTEGER*4       SYS$GETMSG      ! get message status text

        CHARACTER*2     CRLF            ! carriage return / line feed
        CHARACTER*255   MSG             ! message text

        INCLUDE         '($SSDEF)'

        CRLF = CHAR(13)//CHAR(10)

C******************************************************************************
C
C   Inform the operator that CPU class scheduling is being disabled.
C
C******************************************************************************

        STATUS = SYS$GETMSG (%VAL(EXIT_STATUS),MSG_LEN,MSG,,)
        
        CALL OPERATOR ('CPU class scheduling disabled.'//CRLF//
        2              'Reason: '//MSG(1:MSG_LEN))

        CALL LIB$STOP (%VAL(EXIT_STATUS))

9999    END
