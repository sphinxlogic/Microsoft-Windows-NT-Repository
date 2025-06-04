  routine check_setup
	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	! check_company
	!   author       :  Robert Ringrose
	!   Date created :  6-2-88
	!   last modified:  6-2-88
	!   Purpose      :
	!     Pass to check_company
	!       c_company$
	!     and it will return
	!       c_company$ = the suggestion
	!       c_violate = true if the company name
	!         violated any rules for database company names
	!       c_rule$ = a string containing the violated rules
	!
	!    YOU MUST call check_setup at some point BEFORE the
	!      first call to check_company
	!

	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	! c h e c k _ s e t u p
	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	!   Sets up the variables for calls to check_company.
	!   This is so that you can call it once and not take
	!     the time to do the structure extraction each time.
	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	! name rule dispatch table

	chk$num_rules = 7
	dim chk$rule_proc$(chk$num_rules)
	dim chk$rule_name$(chk$num_rules)

	chk$rule_proc$(1) = 'CHK$RULE_11'
	chk$rule_name$(1) = '11 (Extra spaces)'
	chk$rule_proc$(2) = 'CHK$RULE_8'
	chk$rule_name$(2) = '8 (Punctuation)'
	chk$rule_proc$(3) = 'CHK$RULE_4'
	chk$rule_name$(3) = '4 (THE)'
	chk$rule_proc$(4) = 'CHK$RULE_2'
	chk$rule_name$(4) = '2 (Abbreviations)'
	chk$rule_proc$(5) = 'CHK$RULE_3'
	chk$rule_name$(5) = '3 (Omit INC)'
	chk$rule_proc$(6) = 'CHK$RULE_5'
	chk$rule_name$(6) = '5 (DEC)'
	chk$rule_proc$(7) = 'CHK$RULE_9'
	chk$rule_name$(7) = '9 (Initials)'

	chk$omit$ = 'COMPANY,CO,CORPORATION,CORP,INCORPORATED,INC,LIMITED,'+&
	        'LTD,UNLIMITED,UNLTD'
	! These are the disallowed words of rule 3
	chk$letters$ = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
	! Upper case letters
	chk$allowed_chars$ = chk$letters$ + '0123456789&'
	! All allowed characters
	open structure chk$abb  : name 'tti_run:abbreviations'

	chk$abb_allowed$ = ''
	chk$abb_disallowed$ = ''
	extract structure chk$abb
	  chk$abb_allowed$ = chk$abb_allowed$  + ',' + chk$abb(allowed)
	  chk$abb_disallowed$ = chk$abb_disallowed$ + ',' + chk$abb(disallowed)
	end extract
	chk$abb_allowed$ = chk$abb_allowed$[2:len(chk$abb_allowed$)]
	chk$abb_disallowed$ = chk$abb_disallowed$[2:len(chk$abb_disallowed$)]
	close structure chk$c_abb

	end routine



  routine check_company
	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	! check_company
	!   See comments above the related routine check_setup
	!
	! A rule procedure should expect:
	!   c_company$ = the company name so far
	! and return
	!   c_violate = true if the rule was violated (unchangd otherwise)
	!   c_rules$ unchanged if the rule was not violated
	!            = c_rules$ + chk$rule_name$() + ", " if it was
	!              violated
	!
	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	c_violate = false
	c_company$ = rtrim$(c_company$)
	c_rules$ = ''

	for chk$rule_num = 1 to chk$num_rules
	  dispatch chk$rule_proc$(chk$rule_num)
	next chk$rule_num

	c_company$ = edit$(c_company$, 8 + 16 + 128)
	! Remove leading and trailing spaces and tabs

	c_rules$ = c_rules$[1:len(c_rules$) - 2]
	! Remove ", " from the end so the list of violations looks normal

	exit routine



	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	! C h k $ r u l e   1 1
	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	! Rule 11  (in list)
	!   Multiple spaces, leading spaces, and tabs are not
	!   allowed.
	!   Should be called first, as it adds a space
	!   to each end of the company name to make the other
	!   rule routines happy.
	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine chk$rule_11

	if  c_company$ <> edit$(c_company$, 4 + 8 + 16 + 128)  then
	  c_violate = true
	  c_rules$ = chk$rule_name$(chk$rule_num) + ', '  + c_rules$
	end if

	c_company$ = ' ' + edit$(c_company$, 4 + 8 + 16 + 32 + 128) + ' '

	end routine



	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	! C h k $ r u l e   4
	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	! Rule 4  (in list)
	!   Cannot begin with the word THE
	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine chk$rule_4

	if c_company$[1:5] = ' THE ' then
	  c_violate = true
	  c_company$ = c_company$[5:len(c_company$)]
	  c_rules$ = chk$rule_name$(chk$rule_num) + ', '  + c_rules$
	end if

	end routine



	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	! C h k $ r u l e   3
	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	! Rule 3  (in list)
	!   Cannot include any of the words in chk$omit$.
	!   The one exception is that "& CO" is O.K.
	!
	!   Assumes the punctuation has been removed.
	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine chk$rule_3

	for chk$t = 1 to elements(chk$omit$)
	  do
	    chk$z0$ = element$(chk$omit$, chk$t)
	    chk$p1 = pos(c_company$, ' ' + chk$z0$ + ' ')

	    if (chk$p1 <> 0) then
	      if (c_company$[chk$p1-1:chk$p1+3] <> '& CO ') then
	        c_violate = true
	        c_company$ = c_company$[1:chk$p1] +  &
	             c_company$[chk$p1 + 1 + len(chk$z0$): len(c_company$)]
	        c_rules$ = chk$rule_name$(chk$rule_num) + ', '  + c_rules$
	        repeat do	! in case there are 2 omittable words
	      end if
	    end if
	  end do
	next chk$t

	end routine



	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	! C h k $ r u l e   5
	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	! Rule 5  (in list)
	!   Use Digital Equipment, not DEC or DIGITAL EQUIPMENT CORP
	!   (Actually, DIGITAL EQUIPMENT CORP is taken care of;
	!    CORP is removed automatically)
	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine chk$rule_5

	do
	  p1 = pos(c_company$, ' DEC ')
	  if  p1 <> 0  then 
	    c_company$ = c_company$[1:p1] + 'DIGITAL EQUIPMENT' + &
	                 c_company$[p1+4:len(c_company$)]
	    c_violate = true
	    c_rules$ = chk$rule_name$(chk$rule_num) + ', '  + c_rules$
	  end if
	loop while p1 <> 0

	end routine



	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	! C h k $ r u l e   8
	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	! Rule 8  (in list)
	!   No punctuation of any kind allowed; also, "&" MUST
	!    have spaces around it.
	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    routine chk$rule_8

	chk$remove$ = edit$(change$(c_company$, chk$allowed_chars$,  &
	                        space$(len(chk$allowed_chars$))), 2)

	if chk$remove$ <> '' then
	  c_rules$ = chk$rule_name$(chk$rule_num) + ', '  + c_rules$
	  c_violate = true
	end if
	c_company$ = change$(c_company$,chk$remove$,space$(len(chk$remove$)))
	! All characters which are not allowed are now spaces


	chk$p1 = 0
	do
	  chk$p1 = pos(c_company$, '&', chk$p1 + 1)
	  if chk$p1 <> 0 then
	    if c_company$[chk$p1-1:chk$p1+1] <> ' & ' then
	      c_violate = true
	      c_rules$ = chk$rule_name$(chk$rule_num) + ', '  + c_rules$
	      c_company$ = c_company$[1:chk$p1-1] + ' & ' + &
	                   c_company$[chk$p1 + 1:len(c_company$)]
	      chk$p1 = chk$p1 + 1
	    end if
	  end if
	loop until chk$p1 = 0
	!  "&" has at least one space on either side

	c_company$ = edit$(c_company$, 16)	! extra spaces

	end routine



	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	! C h k $ r u l e   9
	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	! Rule 9  (in list)
	!   No spaces allowed between initials
	!   Do it by
	!     1)make a copy of c_company$ with all O.K. letters
	!       changed into 'a' (an unused letter; lower case is not 
	!       allowed)
	!     2)look for ' a a ' (assume two letters like this are 
	!       initials)
	!     3)change all spaces between initials into 'b' (another
	!       unused letter).  Becase I go from left to right
	!       I need to check for ' a a ' and 'ba a '
	!     4)find all instances of 'b'; delete the character in
	!       that position from both the copy and the original,
	!       as it is a space between two initials.
	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine chk$rule_9

	chk$z0$ = change$(c_company$, chk$letters$, &
	              repeat$('a', len(chk$letters$)))

	if  pos(chk$z0$, ' a a ') <> 0  then
	  c_rules$ = chk$rule_name$(chk$rule_num) + ', '  + c_rules$
	  c_violate = true
	end if

	do	! find spaces between initials
	  chk$p1 = pos(chk$z0$, ' a a ')
	  chk$p2 = pos(chk$z0$, 'ba a ')
	  if  chk$p2 <> 0  then chk$p1 = chk$p2
	  if  chk$p1 <> 0  then &
	    chk$z0$ = chk$z0$[1:chk$p1 + 1] + 'b' + &
	              chk$z0$[chk$p1+3:len(chk$z0$)]
	loop while chk$p1 <> 0

	do	! Remove spaces between initials ('b' indicates such a space)
	  chk$p1 = pos(chk$z0$, 'b')
	  if chk$p1 <> 0  then
	    chk$z0$ = chk$z0$[1:chk$p1-1] + chk$z0$[chk$p1 + 1:len(chk$z0$)]
	    c_company$ = c_company$[1:chk$p1 - 1] + &
	                 c_company$[chk$p1 + 1:len(c_company$)]
	  end if
	loop while chk$p1 <> 0

	end routine



	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	! C h k $ r u l e   2
	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	! Rule 2  (in list)
	!   Use abbreviations everywhere.
	!   Check each non-allowed version.  If it is there, replace
	!     it with the allowed version.
	!   The reason for doing this instead of checking each word
	!     in c_company$, which would be faster, is because
	!     you might want to replace "EI DU PONT" with "DUPONT"
	!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine chk$rule_2

	for chk$t = 1 to elements(chk$abb_disallowed$)
	  do
	    chk$z0$ = element$(chk$abb_disallowed$, chk$t)
	    chk$p1 = pos(c_company$, ' ' + chk$z0$ + ' ')
	    if chk$p1 <> 0 then 
	      c_rules$ = chk$rule_name$(chk$rule_num) + ', '  + c_rules$
	      c_violate = true
	      c_company$ = c_company$[1:chk$p1] + &
	                   element$(chk$abb_allowed$, chk$t) + &
	                   c_company$[chk$p1 + 1 + len(chk$z0$): &
	                              len(c_company$)]
	    end if
	  loop while chk$p1 <> 0
	next chk$t

	end routine

	end routine
	!   (check_company)
