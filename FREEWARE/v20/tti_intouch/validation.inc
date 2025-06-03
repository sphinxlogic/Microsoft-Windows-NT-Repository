92000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! C H E C K   V A L I D   E N T R Y
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        !  Given a response in ans$, it checks the rules in valid_element$
        !    to see if it is OK.
        !  Allowed validation rules are:
        !       "NUMBER", "ALLOW", "INTEGER", "DATE", "FULLTIME", "RANGE", 
        !       "DIGITS", "CONTAINS", and "REQUIRED"
        !       (digits 2 3  means 2 digits (max) before the decimal point
        !        and 3 digits (max) after it, negative signs included)
        !
        ! Expected:
        !       fld = the index number of the current field
        !       max_validation() = number of validation elements for that
        !               field
        !       valid_element$(fld,el) contains validation element el for
        !               field fld.  Note that in the case of an "allow",
        !               valid_element$(fld,el+1) as the actual allowed stuff.
        !
        ! Result  :
        !       error = true if there is a problem (such as it violates
        !               one of the rules
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine check_valid_entry

92020   !if  max_validation(fld) = 0 then exit routine 
        error = false
        for valid_element = 1 to max_validation(fld)
          select case valid_element$(fld,valid_element) 
            case 'NUMBER'  : gosub do_number_check
            case 'INTEGER' : gosub do_integer_check
            case 'ALLOW'   : gosub do_allow_check
            case 'DATE'    : gosub do_date_check
            case 'RANGE'   : gosub do_range_check
            case 'REQUIRED': gosub do_required_check
            case 'DIGITS'  : gosub do_digit_check
            case 'CONTAINS': gosub do_contains_check
            case 'FULLTIME': gosub do_fulltime_check
            case else
              message error : "Unknown validation rule:  " + &
                               valid_element$(fld, valid_element)
          end select
          if  _error then exit for
        next valid_element
        if  _error  then error = true
        if  error  then  exit routine
        !gosub do_format_check ++DJS++ 16-FEB-1993 not a useful check anymore
        ! ++DJS++ 16-FEB-1993 format$ no longer returns an error
        
92099   end routine



93000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! D O   R A N G E   C H E C K
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        !  Checks ans$ to see if it is in the proper range.  Expects that the
        !       valid_element$(fld, valid_element + 1) and 
        !       valid_element$(fld, valid_element + 2) have the allowed
        !       range limits.  Increments valid_element
        !       extra times so that you do not try to interpert the
        !       allowed stuff as another valid check.
        !
        ! Expected:
        !       ans$ = user response
        !       valid_element$(fld, valid_element + 1) has the low range limit
        !       valid_element$(fld, valid_element + 2) has the high limit
        !
        ! Result  :
        !       error = true if ans$ is not in the range
        !       valid_element is incremented.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine do_range_check

93020   valid_element = valid_element + 2
        if  trim$(ans$) = '' then exit routine
        if  not(valid(ans$, 'integer')) then
          message error: "Integer expected: " + ans$
          exit routine
        end if
        
        low$ = trim$(valid_element$(fld, valid_element - 1))
        high$ = trim$(valid_element$(fld, valid_element))
     
        z = val(ans$)
        if  z < val(low$)  or  z > val(high$)  then &
          message error : "Allowed range:  " + low$ + &
                              " to " + high$

93099   end routine



94000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! D O   N U M B E R   C H E C K
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Check ans$ to see if it is a number (or blank)
        !
        ! Expected:
        !       ans$ = user response
        !
        ! Result  :
        !       error = true if ans$ is not a number
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine do_number_check

94020   if  trim$(ans$) = '' then exit routine
        if  not(valid(ans$, 'number')) then &
          message error: "Number expected: " + ans$

94099   end routine                       



94500   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! D O   I N T E G E R   C H E C K
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Check ans$ to see if it is an integer or blank
        !
        ! Expected:
        !       ans$ = user response
        !
        ! Result  :
        !       error = true if ans$ is not an integer
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine do_integer_check

94520   if  trim$(ans$) = '' then exit routine
        if  not(valid(ans$, 'integer')) then &
          message error: "Integer expected: " + ans$
                
94599   end routine                       


95000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! D O   A L L O W   C H E C K
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        !  Checks ans$ to see if it is allowed.  Expects that the
        !       valid_element$(fld, valid_element + 1) has the allowed
        !       responses, separated by commas.  Increments valid_element
        !       an extra time so that you do not try to interpert the
        !       allowed stuff as another valid check.
        !
        ! Expected:
        !       ans$ = user response
        !       valid_element$(fld, valid_element+1) has the 
        !               allowed strings.  It shouldn't have leading/trailing
        !               spaces.
        !
        ! Result  :
        !       error = true if ans$ is not in 
        !               valid_element$(fld, valid_element + 1)
        !       valid_element is incremented.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine do_allow_check

95020   valid_element = valid_element + 1
        if  trim$(ans$) = '' then exit routine
        z$ = valid_element$(fld, valid_element)
        if z$ [1:1] = "'" or z$[1:1] = '"' then z$ = z$[2:len(z$)]
        z= len(z$)
        if z$ [z:z] = "'" or z$[z:z] = '"' then z$ = z$[1:z-1] 
        if  fld_uppercase(fld)  then z$ = ucase$(z$)
     
        z1 = match(z$, ans$)
        if  z1 = 0  then &
          message error : 'Allowed responses: ' + z$

95099   end routine



95100   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! D O   C O N T A I N S   C H E C K
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        !  Checks ans$ to see if it has the required character.  Expects 
        !       that the valid_element$(fld, valid_element + 1) has the 
        !       required character. Increments valid_element
        !       an extra time so that you do not try to interpert the
        !       character stuff as another valid check.
        !
        !       this check is used to make sure that a character exists in 
        !       the answer.  For example, a code translation file is set up
        !       as code_prefix:code.  Therefore, a : is required
        !
        ! Expected:
        !       ans$ = user response
        !       valid_element$(fld, valid_element+1) has the 
        !               allowed character.  It shouldn't have leading/trailing
        !               spaces.
        !
        ! Result  :
        !       error = true if ans$ is not in 
        !               valid_element$(fld, valid_element + 1)
        !       valid_element is incremented.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine do_contains_check

95120   valid_element = valid_element + 1
        if  trim$(ans$) = '' then exit routine
        z$ = valid_element$(fld, valid_element)
        if z$ [1:1] = "'" or z$[1:1] = '"' then z$ = z$[2:len(z$)]
        z= len(z$)
        if z$ [z:z] = "'" or z$[z:z] = '"' then z$ = z$[1:z-1] 
        if  fld_uppercase(fld)  then z$ = ucase$(z$)
     
        z1 = pos(ans$, z$)
        if  z1 = 0  then &
          message error : 'Your answer must contain the character "' + z$ + &
                '"'

95199   end routine



95500   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! D O   D A T E   C H E C K
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        !  Checks ans$ to see if it has a valid date.  Expects
        !       that, as the user just entered it, it is in MDY
        !       format.
        !
        ! Expected:
        !       ans$ = user response
        !
        ! Result  :
        !       _error = true if ans$ is not a valid MDY date
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine do_date_check

95520   valid_element = valid_element + 1
        ans$ = trim$(ans$)
        if  ans$ = '' then exit routine
        if  not(valid(ans$[5:8] + ans$[1:4], "DATE"))  then &
          message error : "Invalid data:  " + ans$
                ! +++ RPR Debug - Intouch cannot handle 8-character DATE:MDY


95599   end routine

        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! D O   F U L L T I M E   C H E C K
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Brief description:
        !
        ! Expected:
        !
        ! Locals:
        !
        ! Results:
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    routine do_fulltime_check

95600   valid_element = valid_element +1
        ans$ = trim$(ans$)
        if  ans$ = '' then exit routine
        if len(ans$) = 15 then ans_new$ = ans$[5:8] +ans$[1:4]+ans$[9:15]
        if len(ans$) = 13 then ans_new$ = ans$[3:6] +ans$[1:2]+ans$[7:13]

        if  not(valid(ans_new$, "FULLTIME"))  then &
          message error : "Invalid fullltime:  " + ans$

        end routine





96000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! D O   R E Q U I R E D   C H E C K
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        !  checks to see if response is required
        !
        ! Expected:
        !       ans$ = user response
        !
        ! Result  :
        !        error = true if no user response
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine do_required_check

96020   if  trim$(ans$) = '' then &
          message error: "Response required" 

96099   end routine



96500   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  D O   D I G I T   C H E C K
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  Check that there are the proper number of digits before and
        !    after the decimal point in ans$.  Expects that
        !       valid_element$(fld, valid_element + 1) and
        !       valid_element$(fld, valid_element + 2) have the number
        !       of digits (maximum) before and after the decimal point.
        !       Negative numbers ARE allowed (the behavior will be described
        !       momentarily)
        !    Also increments valid_element twice so that you don't try to
        !       use the numeric parameters as things to check.
        !
        !    The convolutions for negative numbers are actually logical
        !       extensions of negative scaling factors in limited-length
        !       fields and scaling factors which are larger than the field
        !       length.
        !
        !    IF the first parameter is negative, it checks that there
        !       are only negative signs and zeros before the decimal point
        !       and there are abs(first parameter) zeros after it.
        !    IF the second parameter is negative, it checks that there
        !       are only zeros after the decimal point (if there is one)
        !       and either the number is zero or there are abs(second
        !       parameter) zeros immediately prior to the decimal point.
        !
        ! Expected:
        !       ans$ = user response
        !       valid_element$(fld, valid_element+1) has the max. number of
        !               digits before the decimal point
        !       valid_element$(fld, valid_element + 2) has the max. number of
        !               digits after the decimal point
        !
        ! Result  :
        !       error = true if ans$ is not in 
        !               valid_element$(fld, valid_element + 1)
        !       valid_element is incremented.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine do_digit_check

96520   valid_element = valid_element + 2
        if  not(valid(ans$,'NUMBER'))  then exit routine
                                !  No automatic numeric check
        nbr$ = ans$
        if  trim$(nbr$) = ''  or  val(nbr$) = 0.0  then exit routine
                                !  Zero is always O.K.
        if  pos(ans$,'.') = 0  then nbr$ = nbr$ + "."
        dp = pos(nbr$,'.')
        before = val(valid_element$(fld, valid_element - 1))
        after = val(valid_element$(fld, valid_element))
        gosub digit_check_before
        if  _error  then exit routine
        gosub digit_check_after

96599   end routine



96600   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  D I G I T   C H E C K   B E F O R E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  Check the number of digits "before" the decimal point.  Only
        !    allow a certain number of digits before the decimal point
        !    If the number of digits allowed < 0, then force zeros after
        !    the decimal point.
        !  Expects:
        !    before = # digits before the decimal point (or zeros after the
        !               decimal point if before is negative)
        !    nbr$   = the number you are checking (with a trailing decimal point
        !               if none was indicated)
        !    dp     = the location of the decimal point
        !  Result:
        !    _error = true if the number has too many/too few characters
        !       in the proper positions.
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine digit_check_before

96620   if  before > 0  then
          ! Check that there are not more than before digits before the decimal
          if  dp - 1 > before  then &
            message error: "Maximum digits (including negative sign) " + &
                           "before decimal point:  " + str$(before)
        else
          if  val(nbr$) < 0  then before = before - 1   ! Make it count
                                !  negative signs - force another zero.
          !  Check that there are at least -before zeros after the decimal
          if  nbr$[dp + 1: dp - before] <> repeat$('0',-before)  then &
            message error:  "Minimum zeros after decimal point:  " + &
                            str$(-before)
        end if

96699   end routine



96700   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  D I G I T   C H E C K   A F T E R
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  Check the number of digits "after" the decimal point.  Only
        !    allow a certain number of digits after the decimal point
        !    If the number of digits allowed < 0, then force zeros before
        !    the decimal point.
        !  Expects:
        !    after = # digits after the decimal point (or zeros before the
        !               decimal point if after is negative)
        !    nbr$  = the number you are checking (with a trailing decimal point
        !               if none was indicated)
        !    dp    = the location of the decimal point
        !  Result:
        !    _error = true if the number has too many/too few characters
        !       in the proper positions.
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine digit_check_after

96720   if  after >= 0 then
          ! Check that there are not more than after digits after the decimal
          if  len(nbr$) - dp > after  then &
            message error:  "Maximum digits after the decimal point:  " + &
                            str$(after)
        else
          ! Check that there are at least -after zeros before the decimal
          if  ans$[dp + after:dp - 1] <> repeat$('0', -after)  then &
            message error:  "Minimum zeros before the decimal point:  " + &
                            str$(-after)
        end if

96799   end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! D O   F O R M A T   C H E C K
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !make sure that the data fits the mask
        !
        ! Expected:
        !
        ! Result  :
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine do_format_check

        if  tot_fld_lines(change_field) > 1 then  exit routine
        dfc_numeric = false
        for valid_element = 1 to max_validation(fld)
          select case valid_element$(fld,valid_element) 
            case 'NUMBER'  : dfc_numeric = true
            case 'INTEGER' : dfc_numeric = true
            case 'DIGITS'  : dfc_numeric = true
            case else
          end select
        next valid_element
        if  dfc_numeric  then
          display_data = val(ans$)
        else
          display_data = ans$
        end if
        when exception in
          z$ = format$(display_data, str_mask$(change_field))
        use
        end when
        if  not _error  then  exit routine
        message error : "Data doesn't fit print mask: " + str_mask$(change_field)
        error = true              ! To keep it from wiping out the message
        default$ = ans$
        gosub ask_new_data
        took_default = false      ! They didn't take the default, or they
                                  !   wouldn't be here in the first place.
        error = (_back or _exit)  ! Fixed the error unless you did one of these


        end routine




