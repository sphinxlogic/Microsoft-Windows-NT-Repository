c*
c*  STRING module.  The string module provides a simple string heap
c*    management capability.  This is necessary as the strings are
c*    moved/compared/etc. during job evaluation.  Also, the top of
c*    the heap is used for temporary storage by user-language
c*    function evaluations.  
c*
c*    NOTE:  This package could be modified to do dynamic allocation
c*    of virtual storage and to improve efficiency, but I probably won't
c*    waste my time on it.
c*
      function isave ( s, unique )
c*
c*   Save a string in the heap; return its location.  Normally, the heap
c*    is searched first for the string thus saving memory for two 
c*    identical strings, but if UNIQUE is .true. we don't do that 
c*    because Kronos intends to delete the string later.
c*
      include 'strings.cmn'
      character *(*) s
      character *80 temp
      logical unique
c
c --- strip quotes (if necessary)
c
      ls = length(s)
      is = 1
      if (s(1:1) .eq. '"') then
         is = 2
         if (s(ls:ls) .eq. '"') ls = ls - 1
      endif
c
c --- Search heap to see if this string has already been stored.
c ---  The UNIQUE flag is set when the string is to be stored at the
c ---  end of the heap, so it can be deleted later.
c
      nn = 1
      if ((.not. unique) .and. (nstring .gt. 1)) then
10       call strcpy (temp, nn)
         if (temp(1:length(temp)) .eq. s(is:ls)) then
            isave = nn
            return
         endif
         nn = nn + length(temp) + 1
         if (nn .lt. nstring) go to 10
      endif
c
c --- Not already stored, add it
c
      if ((nstring+ls+1-is) .gt. maxstring )
     $        call error (4, 'No room in string table.', 0)
      if (ls .gt. (is-1)) then
         strings(nstring:nstring+ls+1-is) = s(is:ls) // char(0)
      else
         strings(nstring:nstring) = char(0)
      endif
      isave = nstring
      nstring = nstring + ls + 2 - is
      return
      end
c
c---end isave
c
      subroutine free ( i )
c*
c*  Free a string in heap (no garbage collection)
c*   STRING MUST BE AT END OF HEAP !!!!!!!!!!
c*
      include 'strings.cmn'
c
      nstring = i
      return
      end
c
c---end free
c
      subroutine strcpy ( out, iptr)
c*
c*   Copy a string out of the string heap area.  Strings start at the 
c*    pointer location (inclusive) and end at a binary 0 (noninclusive).
c*
      include 'strings.cmn'
      character *(*) out
c
      ilast = min0((iptr+len(out)-1),nstring)
      do 10 i = iptr,ilast
         if (strings(i:i) .eq. char(0)) go to 20
10    continue
20    if (i .gt. iptr) then
         out = strings(iptr:i-1)
      else
         out = ' '
      endif
      return
      end
c
c---end strcpy
c
