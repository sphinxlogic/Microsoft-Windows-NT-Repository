      PROGRAM Notify 
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **          Notify           **
C*                  **                           **
C*                  *******************************
C*                  *******************************
C*
C*     AUTHOR :
C*          Arthur E. Ragosta  
C*          RAGOSTA@MERLIN.ARC.NASA.GOV
C*          
C*          MS 219-1
C*          NASA Ames Research Center
C*          Moffett Field, Ca.  94035
C*          (415) 604-5558
C*
C*     DESCRIPTION :
C*          NOTIFY is used to make sure someone is notified of important
C*          events or errors.
C*    
C*          P1 = message text or file name
C*          P2 = primary user list or file name
C*          P3 = secondary user list or file name
C*
C*     SUBPROGRAM REFERENCES :
C*          GETFOR, LENGTH, GET_USERS, TELL_USERS, MAIL_USERS, OPER, 
C*           EXIT
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*          Not portable.
C*          Primarily intended for Kronos support.
C*
C*     LANGUAGE AND COMPILER :
C*          ANSI FORTRAN 77
C*
C*     CHANGE HISTORY :
C*           4-APR-1991  -  INITIAL VERSION
C*
C***********************************************************************
C*
      character *1000 mess
      character *132 line
      character *80 p(4), q(2)
      character *20 pusers(10), susers(10)
c
      call getfor (nq, q, np, p)
      if (np .lt. 2) go to 1000
c
c --- first parameter is either a message or indirect command
c
      lp = length(p(1))
c
c --- read it from a file?
c
      mess = '%%% Message from Kronos %%%' // char(13) //char(10)
      lm = length(mess) + 1
      if (p(1)(1:1) .eq. '@') then
         OPEN (UNIT=1, FILE=p(1)(2:lp), STATUS='OLD', ERR=1100)
10       read(1,900,end=15) line
         mess(lm:) = line(1:length(line)) // char(13) // char(10)
         lm = lm + length(line) + 2
         go to 10
15       lm = lm - 1
         close(1)
c
c --- just a simple message
c
      else
         mess(lm:) = p(1)
      endif
c
c --- second parameter is either a user or a list of users
c
      call get_users (npri, pusers, p(2))
c
c --- if a third parameter, its a user or list of users
c
      if (np .lt. 3) then
         nsec = 0
      else
         call get_users (nsec, susers, p(3))
      endif
c
c --- try to tell the primary users
c
      nok = 0
      if (npri .gt. 0) call tell_users (npri, pusers, mess, nok)
c
c --- if not notified, go to secondary list
c
      if ((nok .eq. 0) .and. (nsec .gt. 0))
     $       call tell_users (nsec, susers, mess, nok)
c
c --- ok, resort to mail
c
      if ((nok .eq. 0) .and. (npri .gt. 0))
     $       call mail_users (npri, pusers, mess, nok)
c
c --- disaster if noone home
c
      if (nok .eq. 0) then
         call oper (
     $ '############################################','CENTRAL')
         call oper (
     $ 'NOTIFY(Kronos) - Unable to notify any users!','CENTRAL')
      endif
      go to 2000
c
1000  call oper ( '###############################################',
     $            'CENTRAL')
      call oper ( 'AT least two parameters are required by NOTIFY.',
     $            'CENTRAL' )
      go to 2000
c
1100  call oper ( '###############################################',
     $            'CENTRAL')
      call oper ( 'NOTIFY unable to open file ' // p(1)(2:lp) ,
     $            'CENTRAL')
2000  call exit
900   format(a)
      END
C
C---END Notify
C
      SUBROUTINE Get_Users (n, users, input)
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **         Get_Users         **
C*                  **                           **
C*                  *******************************
C*                  *******************************
C*
C*     AUTHOR :
C*          Arthur E. Ragosta  
C*          RAGOSTA@MERLIN.ARC.NASA.GOV
C*          
C*          MS 219-1
C*          NASA Ames Research Center
C*          Moffett Field, Ca.  94035
C*          (415) 604-5558
C*
C*     DESCRIPTION :
C*          Extract a list of users to notify from a list or from 
C*           a file.
C*
C*     INPUT ARGUMENTS :
C*          INPUT - either a list (seperated by commas) or a
C*                   filename preceded by "@"
C*
C*     OUTPUT ARGUMENTS :
C*          USERS - The list of user names
C*          N     - Number of users in list
C*
C*     SUBPROGRAM REFERENCES :
C*          GETOKE, CAPS, LENGTH, LEFT
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*
C*     LANGUAGE AND COMPILER :
C*          ANSI FORTRAN 77
C*
C*     CHANGE HISTORY :
C*           8-APR-1991  -  INITIAL VERSION
C*
C***********************************************************************
C*
      character *(*) input
      character *20 users(1), user
      character *1 type
      logical error
c
      n = 0
      if (input .eq. ' ') return
c
c --- is list in a file?
c
      if (input(1:1) .eq. '@') then
         OPEN (UNIT=1, FILE=input(2:length(input)), STATUS='OLD',
     $         ERR=1000)
10       read(1,900,end=15) users(n+1)
         n = n + 1
         call left (users(n))
         call caps (users(n))
         go to 10
15       close(1)
c
c --- list is in the input string
c
      else
         ii = 0
         li = length(input)
20       call getoke (input, li, ii, user, type, error)
         if (error .or. (type .eq. 'E')) return
         n = n + 1
         call caps(user)
         users(n) = user
         go to 20
      endif
1000  RETURN
900   format(a)
      END
C
C---END Get_Users
C
      SUBROUTINE Tell_Users (nusers, users, mess, nok)
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **        Tell_Users         **
C*                  **                           **
C*                  *******************************
C*                  *******************************
C*
C*     AUTHOR :
C*          Arthur E. Ragosta  
C*          RAGOSTA@MERLIN.ARC.NASA.GOV
C*          
C*          MS 219-1
C*          NASA Ames Research Center
C*          Moffett Field, Ca.  94035
C*          (415) 604-5558
C*
C*     DESCRIPTION :
C*          Send a message to any users logged on
C*
C*     INPUT ARGUMENTS :
C*          nusers - number of users to notify
C*          users  - user names of users to notify
C*          mess   - text of message (already formatted)
C*
C*     OUTPUT ARGUMENTS :
C*          nok    - number of users successfully notified
C*
C*     SUBPROGRAM REFERENCES :
C*          sendw
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*
C*     LANGUAGE AND COMPILER :
C*          ANSI FORTRAN 77
C*
C*     CHANGE HISTORY :
C*           8-APR-1991  -  INITIAL VERSION
C*
C***********************************************************************
C*
      character *(*) mess
      character *20 users(nusers)
c
      DO 10 I = 1, nusers
         call sendw ( users(i), char(7) // mess(1:length(mess)), n, nf )
         if (n .gt. 0) nok = nok + n
10    CONTINUE
      RETURN
      END
C
C---END Tell_Users
C
      SUBROUTINE Mail_Users (nusers, users, mess, nok)
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **        Mail_Users         **
C*                  **                           **
C*                  *******************************
C*                  *******************************
C*
C*     AUTHOR :
C*          Arthur E. Ragosta  
C*          RAGOSTA@MERLIN.ARC.NASA.GOV
C*          
C*          MS 219-1
C*          NASA Ames Research Center
C*          Moffett Field, Ca.  94035
C*          (415) 604-5558
C*
C*     DESCRIPTION :
C*          Send a mail message to users
C*
C*     INPUT ARGUMENTS :
C*          nusers - number of users to notify
C*          users  - user names of users to notify
C*          mess   - text of message (already formatted)
C*
C*     OUTPUT ARGUMENTS :
C*          nok    - number of users successfully notified
C*
C*     SUBPROGRAM REFERENCES :
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*
C*     LANGUAGE AND COMPILER :
C*          ANSI FORTRAN 77
C*
C*     CHANGE HISTORY :
C*           8-APR-1991  -  INITIAL VERSION
C*
C***********************************************************************
C*
      character *(*) mess
      character *120 line
      character *20 users(nusers)
      character *5 version
c
c  What this routine is supposed to do is mail the message to all users 
c  in the list and return the number of successful mailings to the 
c  caller.  Unfortunately, that is not implemented here.  Instead, it 
c  just tries to mail the message.  It never returns unless something 
c  goes wrong.
c
c  I'll fix it when I get the documentation for callable Mail
c
      line = ' '
      il = 1
      DO 10 I = 1, nusers
         line(il:) = users(i)(1:length(users(i))) // ','
         il = length(line)+1
10    CONTINUE
      line(il-1:) = ' '
c
c --- LINE now contains list of users
c
      OPEN (UNIT=1, FILE='SCRATCH:NOTIFY_TEMP.COM', STATUS='NEW',
     $ CARRIAGECONTROL='LIST', ERR=1000)
      call getfvers(1,version)
      call left(version)
      write(1,900)line(1:length(line)), mess(1:length(mess)), version
      close(1)
c
c --- LIB$DO_COMMAND
c
      call lib$do_command ('@SCRATCH:NOTIFY_TEMP.COM'//VERSION)
c
1000  RETURN
900   format(
     $ '$mail',/,
     $ 'send',/,
     $ a,/,
     $ 'Kronos message',/,
     $ a,/,
     $ '$delete scratch:notify_temp.com',a)
      END
C
C---END Mail_Users
C
