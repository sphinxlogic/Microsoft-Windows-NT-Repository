      PROGRAM CHECK_PROCESS
      INCLUDE '($IODEF)'
      INCLUDE '($JPIDEF)'
      INCLUDE '($SSDEF)'
C
      PARAMETER (MAXLIST=60)
      CHARACTER *15 PROC_NAME, LIST(MAXLIST)
      LOGICAL IS_RUNNING(MAXLIST), ERRORS
      INTEGER *2 ITEM(8)
      INTEGER *4 ITMLST(4), IOSB(2)
      EQUIVALENCE (ITEM(1),ITMLST(1))
      INTEGER SYS$GETJPIW
C
C
C --- GET NAMES OF PROCESSES TO VERIFY
C
      list(1) = '<POP3-*'
      list(2) = 'AppleTalk ACP'
      list(3) = 'ATKGW$ACP'
      list(4) = 'AUDIT_SERVER'
      list(5) = 'CACHE_SERVER'
      list(6) = 'CLUSTER_SERVER'
      list(7) = 'CONFIGURE'
      list(8) = 'Control Master'
      list(9) = 'DFS$00010001_1'
      list(10)= 'DFS$COM_ACP'
      list(11)= 'ERRFMT'
      list(12)= 'EVL'
      list(13)= 'GFAX_HP_0001'
      list(14)= 'GFAX_PS_0001'
      list(15)= 'GFAX_SEND_0001'
      list(16)= 'IPCACP'
      list(17)= 'JOB_CONTROL'
      list(18)= 'Kronos'
      list(19)= 'LATACP'
      list(20)= 'MSAF$SERVER*'
      list(21)= 'MSAP$RCVR*'
      list(22)= 'MULTINET_SERVER'
      list(23)= 'NETACP'
      list(24)= 'NETBIOS'
      list(25)= 'OPCOM'
      list(26)= 'PMDF counters'
      list(27)= 'PWRK$ADMIN_0'
      list(28)= 'PWRK$KNBDAEMON'
      list(29)= 'PWRK$LICENSE_R'
      list(30)= 'PWRK$LICENSE_S'
      list(31)= 'PWRK$LMDMN'
      list(32)= 'PWRK$LMMCP'
      list(33)= 'PWRK$LMSRV'
      list(34)= 'PWRK$MASTER'
      list(35)= 'PWRK$MONITOR'
      list(36)= 'PWRK$NBDAEMON'
      list(37)= 'QUEUE_MANAGER'
      list(38)= 'REMACP'
      list(39)= 'SMISERVER'
      list(40)= 'SWAPPER'
      list(41)= 'SYMBIONT_*'
      list(42)= 'Watcher'
      num_list = 42
C
C --- ITEM 1, PROCESS NAME
C
      ITEM(1)   = 15
      ITEM(2)   = JPI$_PRCNAM
      ITMLST(2) = %LOC(PROC_NAME)
      ITMLST(3) = %LOC(LNAME)
C
      ITMLST(4)= 0
      IPID  = -1
C
C --- PROCESS LOOP
C
10    PROC_NAME = ' '
      ISTAT = SYS$GETJPIW(,IPID,,ITMLST,IOSB,,)
      IF (ISTAT .EQ. SS$_NOMOREPROC) GO TO 30
      IF (.NOT. ISTAT) CALL EXIT (IOSB(1))
C
      DO 20 I = 1, NUM_LIST
         if (is_running(i)) go to 20
         IL = INDEX(LIST(I),'*') - 1
         IF (IL .GT. 0) THEN
            IF (PROC_NAME(1:IL) .EQ. LIST(I)(1:IL)) THEN
               IS_RUNNING(I) = .TRUE.
               GO TO 10
            ENDIF
         ELSE
            IF (PROC_NAME(1:LNAME) .EQ. LIST(I)(1:LENGTH(LIST(I)))) THEN
               IS_RUNNING(I) = .TRUE.
               GO TO 10
            ENDIF
         ENDIF
20    CONTINUE
      GO TO 10
C
30    ERRORS = .FALSE.
      DO 40 I = 1, NUM_LIST
         IF (.NOT. IS_RUNNING(I)) THEN
            if (.not. errors) then
               OPEN (UNIT=2,
     $          FILE='KRONOS_ROOT:[RESOURCES]CHECK_PROCESS.OUT',
     $          STATUS='NEW', CARRIAGECONTROL='LIST')
               ERRORS = .TRUE.
            endif
            WRITE(2,*)
     $        ' HORRORS! Process ' // LIST(I)(1:LENGTH(LIST(I))) //
     $        ' isn''t running.'
         ENDIF
40    CONTINUE
      CALL EXIT
      END
C
C---END CHECK_PROCESS
C
