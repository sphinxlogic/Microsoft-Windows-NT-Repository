* CD Program by:
*  Peter Galbraith       [Phone: (902) 494-7007] |  GALBRTH@AC.DAL.CA (InterNet)
*  Dept. of Oceanography,  Dalhousie University  |  GALBRTH@DALAC     (BitNet)
*  Halifax, Nova Scotia, Canada, B3H 4J1         |  {uunet watmath}!dalcs!rhogee
*
      options /extend_source
      program cd

      character*60 message, command
      integer      status, lib$get_foreign, i, len

      status = lib$get_foreign(command)

      call cd_routine(command,message)
      write (*,*) message
      end
********************************************************************************
      options /extend_source
      subroutine cd_routine(command,message)

      character*60  message, command, ini_disk, ini_dir, login, login_dev, empty
      character*60  fin_dir, fin_disk, parts(10), search_path, found_file
      character*60  fin_spec, tmpstr
      character*1   id
      integer       lcommand, lini_disk, lini_dir, llogin, llogin_dev, i, tmpi
      integer       lfin_dir, lfin_disk, lparts(10), part_number, lsearch_path
      integer       status, lib$sys_trnlog, sys$setddir, lib$set_logical
      integer       lib$find_file, lib$find_file_end, lib$set_symbol
      integer       lib$get_symbol, context, pop_up, done
      integer       ltop_path, offset, newoffset, plevels, rlevels, newtmpi

      empty = '                                                             '
      message = empty

C Get command line length
      do i = 60,1,-1
        if (command(i:i) .ne. ' ') then
          lcommand = i
          goto 100
        endif
      enddo
      lcommand = 0
 100  continue

C Get SYS$LOGIN_DEVICE
      status = lib$sys_trnlog('SYS$LOGIN_DEVICE',llogin_dev,login_dev)
C Get SYS$LOGIN
      status = lib$sys_trnlog('SYS$LOGIN',llogin,login)

C Get default directory
      status = lib$sys_trnlog('SYS$DISK',lini_disk,ini_disk)
      status = sys$setddir(' ',lini_dir,ini_dir)

C Display default directory and exit on empty command
      if (lcommand .eq. 0) then
        if (login_dev(1:llogin_dev) .eq. ini_disk(1:lini_disk)) then
          message = ini_dir(1:lini_dir)
        else
          message = ini_disk(1:lini_disk) // ini_dir(1:lini_dir)
        endif
        return
      endif

C Trip tokens and process them
C .. -> SET DEF [-]
C .  -> Ignore
C /  -> Ignore
C #  -> A tempdisk
C \  -> SYS$LOGIN
C ~  -> SYS$LOGIN
C <  -> Toggle
C $  -> No validation
C *  -> Disply stack
C ^  -> Store on stack
C >  -> Go to stack

 1000 continue
C done:   number of characters processed in token
        done = 0
C status: if status = 0 along the line then reset to initial directory and exit
        status = 1
        if (command(1:2) .eq. '..') then
          status = pop_up(message)
          done = 2
        else
          if (command(1:1) .eq. '.') done = 1
        endif
        if (command(1:1) .eq. '/') done = 1
********
        if (command(1:1) .eq. '<') then
c get last dir and disk
          status = lib$get_symbol('eve_last_dir',fin_dir)
          status = lib$get_symbol('eve_last_disk',fin_disk)
c change dir and disk
          status = lib$set_logical('SYS$DISK',fin_disk(1:index(fin_disk,' ')-1))
          status = sys$setddir(fin_dir(1:index(fin_dir,' ')-1),,)
          done = 1
        endif
********
      if (command(1:1) .eq. '$') then
c force directory-change without validation or abbreviations
        command = command(2:60)                                                 
c translate logical if any
        status = lib$sys_trnlog(command(1:index(command,' ')-1),tmpi,tmpstr)
C Change dir and disk
        status = lib$set_logical('SYS$DISK',tmpstr(1:index(tmpstr,':')))
        offset = offset + 1
        fin_dir = tmpstr(index(tmpstr,':')+1:index(tmpstr,' ')-1)
        status = sys$setddir(tmpstr(index(tmpstr,':')+1:index(tmpstr,' ')-1),,)
        done = 0
        command = ' '
        lcommand = 0
      endif         
********
      IF (command(1:1) .EQ. '#') THEN
C force directory change to a tempdisk
C if followed by a valid number then that tempdisk, otherwise than the default
        id = command(2:2)
        if ((((id .ne. '1') .and. (id .ne. '2')) .and.
     &       ((id .ne. '3') .and. (id .ne. '4'))) .and.
     &        (id .ne. '5')                      )  then
c get default disk number
          id = login(5:5)
          done = 1
        else
          done = 2
        endif
        if (id .eq. '5') then
          status = lib$set_logical('SYS$DISK','DAL_TEMPDISK:')
        else
          fin_disk = 'TEMPDISK :'
          fin_disk(9:9) = id
          status = lib$set_logical('SYS$DISK',fin_disk(1:10))
        endif
C Change dir and disk
        status = sys$setddir('[000000]',,)
        if (command(done+1:done+1) .eq. ' ') then
C If default directory exists go there, otherwise go to [000000]
C   determine default directory name
          status = lib$find_file(
     .             login(index(login,'[')+1:index(login,']')-1)//'.DIR',
     .             found_file,context)
          status = lib$find_file_end(context)
          if (found_file(index(found_file,' ')-1:index(found_file,' ')-1) 
     .        .ne. ';') then 
            status = sys$setddir(login(index(login,'['):index(login,']')),,)
          endif
        endif
      endif
********
C If first character is \ or ~ goto SYS$LOGIN
      if ((command(1:1) .eq. '\') .or. (command(1:1) .eq. '~')) then
C LOGIN looks like 'DISK4:[GALBRTH]'
        status = lib$set_logical('SYS$DISK',login(1:index(login,':')))
        status = sys$setddir(login(index(login,'['):index(login,']')),,)
        done = 1                                                    
      endif
********
        if (status .eq. 0) goto 2000
        if (done .eq. 0) goto 2000
        command = command(done+1:60)//empty(1:done)
        lcommand = lcommand - done
      if (lcommand .gt. 0) goto 1000

 2000 continue
      if (lcommand .le. 0) goto 5500
        
      if (status .eq. 0) then
        status = lib$sys_trnlog('SYS$DISK',lfin_disk,fin_disk)
        status = sys$setddir(' ',lfin_dir,fin_dir)
C Error-- reset default directory
        if (fin_disk(1:lfin_disk) .ne. ini_disk(1:lini_disk)) then
          status = lib$set_logical('SYS$DISK',ini_disk(1:lini_disk))
        endif
        if (fin_dir(1:lfin_dir) .ne. ini_dir(1:lini_dir)) then
          status = sys$setddir(ini_dir,lfin_dir,fin_dir)
        endif
        return
      endif

C Tokens all processed... process rest of string.
C Substitute all / for .
      do i = 1,10
        tmpi = index(command,'/')
        if (tmpi .eq. 0) goto 2500  
        command(tmpi:tmpi) = '.'
      enddo
 2500 continue

C Count levels required for destination directory
      tmpi = 1
      rlevels = 1
      do i = 1,10
        newtmpi = index(command(tmpi:60),'.')
        if (newtmpi .eq. 0) goto 2600
        tmpi = newtmpi + tmpi
        rlevels = rlevels + 1
      enddo
 2600 continue
C Count levels already deep
      status = sys$setddir(' ',tmpi,tmpstr)
      tmpi = 1
      plevels = 1
      do i = 1,10
        newtmpi = index(tmpstr(tmpi:60),'.')
        if (newtmpi .eq. 0) goto 2700
        tmpi = tmpi + newtmpi
        plevels = plevels + 1
      enddo
 2700 continue
C If sum is greater than 8 than I have to pop before I search
C Can't have more than 8 directory levels...
      rlevels = plevels + rlevels - 8
      do i = 1,rlevels
        if (pop_up(message) .eq. 0) goto 6000
      enddo
      status = lib$sys_trnlog('SYS$DISK',lfin_disk,fin_disk)
      status = sys$setddir(' ',lfin_dir,fin_dir)
        
C part1.part2.part3 searched as DISK:[CURRENT.CURRENT.PART1*.PART2*]PART3.DIR
C if not found search for DISK:[CURRENT.PART1*.PART2*]PART3.DIR
C and finally for DISK:[PART1*.PART2*]PART3.DIR
C except if DISK:[CURRENT] is SYS$LOGIN, then go go pass that

      do i = 1,10
        lparts(i) = index(command,'.') - 1
        if (lparts(i) .eq. -1) goto 3000
        parts(i) = command(1:lparts(i))
        command = command(lparts(i)+2:60)//empty(1:lparts(i)+1)
        lcommand = lcommand - (lparts(i) + 1)
      enddo  
 3000 continue
C Got `i' parts
        part_number = i
        parts(part_number) = command(1:lcommand)
        lparts(i) = lcommand

C Build search path...
C DISK:[CURRENT.CURRENT
        search_path = fin_disk(1:lfin_disk)//fin_dir(1:lfin_dir-1)
        lsearch_path = lfin_disk + lfin_dir - 1
C ltop_path remembers the length of the top path - use to pop if no find
        ltop_path    = lsearch_path

 4000   continue
        do i = 1,part_number-1
          search_path = search_path(1:lsearch_path) // '.' 
     .    // parts(i)(1:lparts(i)) // '*'
          lsearch_path = lsearch_path + lparts(i) + 2  
        enddo
C DISK:[CURRENT.CURRENT.PART1*.PART2*
        search_path = search_path(1:lsearch_path) // ']' 
     .  // parts(part_number)(1:lparts(part_number)) // '*.DIR'
        lsearch_path = lsearch_path + lparts(i) + 6  
C DISK:[CURRENT.CURRENT.PART1*.PART2*]PART3*.DIR   all done for this level
                                  
d       write (*,*) ' search ',search_path(1:lsearch_path)

        status = lib$find_file(search_path(1:lsearch_path),found_file,context)
        status = lib$find_file_end(context)
        if (found_file(1:lsearch_path) .ne. search_path(1:lsearch_path)) 
     .  goto 5000

C Did not find it... strip out a top directory 
        offset = index(search_path(1:ltop_path),'.') 
        if (offset .eq. 0) then
C no more upper directopries, except maybe 000000
C pop-up to 000000 if not at sys$login and try again
d         write (*,*) ' No more upper directories (except maybe 000000)'
          if (login(1:llogin-1) .eq. search_path(1:llogin-1)) goto 6000
d         write (*,*) ' Not at sys$login! check for 000000'
          if (search_path(ltop_path-5:ltop_path) .eq. '000000') goto 6000
d         write (*,*) ' Not at 000000! go to it!'
          lsearch_path = index(search_path,':') + 7
          search_path = search_path(1:lsearch_path-7) // '[000000'
        else
C there are upper directories... strip one off
d         write (*,*) ' Strip off an upper directory'
          do i = 1,10
C Find last dot offset on top directories
d           write (*,*) ' looking for last dot in ',search_path(offset+1:ltop_path)
            newoffset = index(search_path(offset+1:ltop_path),'.')
            if (newoffset .eq. 0) goto 4500
C found a dot, look beyond
            offset = newoffset + offset
          enddo
 4500     continue
          lsearch_path = offset - 1
        endif
d       write (*,*) ' Pop ',search_path(1:lsearch_path)
C Reset ltop_path
        ltop_path    = lsearch_path
      goto 4000

 5000 continue
C found it in form 
C DISK:[CURRENT.CURRENT.PART1.PART2]PART3.DIR  change default to
C      [CURRENT.CURRENT.PART1.PART2.PART3]
      offset = index(found_file,']')
      status = sys$setddir(found_file(index(found_file,'['):offset),lfin_dir,
     .         fin_dir)

      found_file = '[.' // found_file(offset+1:index(found_file,'.DIR')-1) //']'
      status = sys$setddir(found_file(1:index(found_file,']')),lfin_dir,fin_dir)

C Get default disk
 5500 status = lib$sys_trnlog('SYS$DISK',lfin_disk,fin_disk)
      status = sys$setddir(' ',lfin_dir,fin_dir)
c reset last dir and disk
      status = lib$set_symbol('eve_last_dir',ini_dir(1:lini_dir))
      status = lib$set_symbol('eve_last_disk',ini_disk(1:lini_disk))
      if (login_dev(1:llogin_dev) .eq. fin_disk(1:lfin_disk)) then
        message = fin_dir(1:lfin_dir)
      else                      
        message = fin_disk(1:lfin_disk) // fin_dir(1:lfin_dir)
      endif
      return

 6000 continue
C Could not find it
      message = 'Directory not found'
      return
      end
********************************************************************************
      options /extend_source
      function pop_up(message)

      character*60  fin_dir, ini_dir, message
      integer       lfin_dir, lini_dir, pop_up
      integer       status, lib$sys_trnlog, sys$setddir

      pop_up = 1
      status = sys$setddir('[-]',lini_dir,ini_dir)
      status = sys$setddir(' ',lfin_dir,fin_dir)
      if (ini_dir(1:lini_dir) .eq. fin_dir(1:lfin_dir)) then
        pop_up = 0                 
        message = 'Cannot go up that high'
      endif
      return
      end
