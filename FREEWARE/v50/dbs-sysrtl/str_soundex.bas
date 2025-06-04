	!%SBTTL "External subprogram - str_soundex"
	!%IDENT "V1-003"

 sub str_soundex (string soundex_out, string text_in)

!+
! Version:	V1-003
!
! Facility:	String routines.
!
! Abstarct:	To return the soundex equivalent of an alpha string.
!
! Environment:	User mode.
!
! History:
!
!	17-Jun-1986, DBS; Version V1-001
! 001 -	Original version.
!	19-Jun-1992, DBS; Version V1-002
! 002 -	Modified for VAX basic.
!	29-Jun-1992, DBS; Version V1-003
! 003 -	Modified to not include trailing zeros.
!-

!++
! Functional Description:
!	This module will return the soundex code for an alpha string.
!
! Calling Sequence:
!	call str_soundex (soundex_value by desc, my_text by desc)
!
! Formal Arguments:
!	soundex_out.wt.ds	The soundex code for the word
!	text_in.rt.ds		The text to be converted.
!
! Implicit Inputs:
!	None.
!
! Implicit Outputs:
!	None.
!
! Completion Codes:
!	None.
!
! Side Effects:
!	None.
!--

	!%SBTTL "Declarations"

	declare long		i

	declare string text
	declare string soundex
	declare string char

	declare string function substitute

	!%SBTTL "Mainline"

Mainline:

	!+
	!   here we control the flow of logic
	!-

	text	= edit$(text_in, 255%)
	soundex	= left$(text, 1%)
	text	= right$(text, 2%)
	text	= substitute(text, 'A', '')
	text	= substitute(text, 'E', '')
	text	= substitute(text, 'I', '')
	text	= substitute(text, 'O', '')
	text	= substitute(text, 'U', '')
	text	= substitute(text, 'W', '')
	text	= substitute(text, 'H', '')
	text	= substitute(text, 'Y', '')
	text	= substitute(text, string$(2%, i), chr$(i)) &
				for i = 65% to 90%
!003	text	= text + '***'
	text	= left$(text, 3%) if (len(text) > 3%)

	for i = 1% to len(text)
		char = mid$(text, i, 1%)

		if (pos('BFPV', char, 1%))
			then
			soundex = soundex + '1'
		else
		if (pos('CGJKQSXZ', char, 1%))
			then
			soundex = soundex + '2'
		else
		if ((char = 'D') or (char = 'T'))
			then
			soundex = soundex + '3'
		else
		if (char = 'L')
			then
			soundex = soundex + '4'
		else
		if ((char = 'M') or (char = 'N'))
			then
			soundex = soundex + '5'
		else
		if (char = 'R')
			then
			soundex = soundex + '6'
		else
!003		soundex = soundex + '0'
		end if
		end if
		end if
		end if
		end if
		end if
	next i

	soundex_out = soundex

 subexit

def string substitute (string original, string old_bit, string new_bit)

	declare long len_old_bit
	declare long len_new_bit
	declare long found_it

	len_old_bit = len(old_bit)
	len_new_bit = len(new_bit)
	found_it    = pos(original, old_bit, 1%)

	while found_it
		original = left$(original, found_it - 1%) + new_bit	  &
				+ right$(original, found_it + len_old_bit)
		found_it = pos(original, old_bit, found_it + len_new_bit)
	next

	substitute	= original

end def

end sub
