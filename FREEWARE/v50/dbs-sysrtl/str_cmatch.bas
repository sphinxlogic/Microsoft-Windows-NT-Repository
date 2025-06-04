	!%SBTTL "External subprogram - str_cmatch"
	!%IDENT "V1-003"

 sub str_cmatch (string my_word, string your_word, long your_score)

!+
! Version:	V1-003
!
! Facility:	String routines.
!
! Abstract:	To match two words (text) and return a score.
!
! Environment:	User mode.
!
! History:
!
!	29-Nov-1985, DBS; Version V1-001
! 001 -	Original version.
!	02-Dec-1985, DBS; Version V1-002
! 002 -	Introduced 'ring in' characters.
!	19-Jun-1992, DBS; version V1-003
! 003 -	Modified for VAX basic.
!-

!++
! Functional Description:
!	This module will return a score indicating how closely your word
!	resembles my word.
!
!
! Calling Sequence:
!	call str_cmatch (my_word by desc, your_word by desc, score by ref)
!
! Formal Parameters:
!	my_word.rt.ds	The real word we suspect you may have meant to use.
!	your_word.rt.ds	The word you actually used.
!	your_score.wl.r	The compatability score for the words.
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

	declare long constant	zero = 0
	declare long constant	distance_limit = 2
	declare long constant	ringin_maximum = 2
	declare long constant	bonus = 30
	declare long constant	benefit = 26
	declare long constant	wager = 20
	declare long constant	advantage = 24
	declare long constant	disadvantage = -8
	declare long constant	blank_i = 23

	declare string constant	null = ''
	declare string constant	underscore = '_'

	declare string function substitute

	declare long	distance
	declare long	how_far
	declare long	i
	declare long	index
	declare long	interchanges
	declare long	letter
	declare long	matched
	declare long	misplaced
	declare long	my_length
	declare long	position
	declare long	ringer_i
	declare long	ring_in
	declare long	score
	declare long	score_percentage
	declare long	unmatched
	declare long	your_length

	declare string	blank
	declare string	letter_t
	declare string	mine
	declare string	my_letter
	declare string	yours
	declare string	your_letter

	dimension	long mine_i(24%)				  &
			,string ringer(26%)

	data	A,SQZW,		B,VNGH,		C,XVDF,			  &
		D,SFXCER,	E,WRSD34,	F,DGCVRT,		  &
		G,FHVBTY,	H,GJBNYU,	I,UOJK89,		  &
		J,HKNMUI,	K,JLMIO,	L,KOP,			  &
		M,NJK,		N,BMHJ,		O,IPKL90,		  &
		P,0OL,		Q,12AW,		R,ETDF45,		  &
		S,ADZXWE,	T,RYFG56,	U,YIHJ78,		  &
		V,CBFG,		W,QEAS23,	X,ZCSD,			  &
		Y,TUGH67,	Z,ASX

	!%SBTTL "Mainline"

Mainline:

	!+
	!   here we control the flow of logic
	!-

	restore

	for i = 1% to 26%
		read letter_t, ringer(i)
		ringer(i) = letter_t + ringer(i)
	next i

	blank		= chr$(blank_i)
	mine		= edit$(my_word, 255%)
	yours		= edit$(your_word, 255%)
	mine		= substitute(mine, underscore, null)
	yours		= substitute(yours, underscore, null)
	my_length	= len(mine)
	your_length	= len(yours)
	length_difference = your_length - my_length
	maximum_score	= my_length*bonus
	maximum_score	= your_length*bonus if (your_length < my_length)
	score		= zero

	gosub check_words

	if (placed = your_length)	 ! either ok or abbreviation
		then
		score_percentage = 100%
	else
	gosub check_unmatched
	end if

	your_score = score_percentage

 subexit

check_words:

! first check for right letter in right spot

	placed = zero

	for letter = 1% to your_length
		your_letter = mid$(yours, letter, 1%)
		my_letter   = mid$(mine, letter, 1%)

		if (your_letter = my_letter)
			then
			placed = placed + 1%

			gosub placed_it
		end if
	next letter

! now check for right letter in wrong spot

	misplaced = zero

	for letter = 1% to your_length
		your_letter = mid$(yours, letter, 1%)

		if (your_letter > blank)
			then
			position = instr(1%, mine, your_letter)
			distance = abs(letter - position)

			if ((distance <= distance_limit)		&
					and (position <> zero))
				then
				misplaced = misplaced + 1%

				gosub misplaced_it
			end if
		end if
	next letter

 return

placed_it:

	gosub replace_yours

	mine = left$(mine, letter - 1%) + chr$(letter)			&
				+ right$(mine, letter + 1%)

 return

misplaced_it:

	gosub replace_yours

	mine = left$(mine, position - 1%) + chr$(letter)		&
				+ right$(mine, position + 1%)

 return

replace_yours:

	yours = left$(yours, letter - 1%) + blank			&
				+ right$(yours, letter + 1%)

 return

check_unmatched:

	matched		= placed + misplaced
	unmatched	= your_length - matched
	interchanges	= zero
	distance	= zero
	ring_in		= zero

	gosub do_swaps_and_distance					&
			for position = 1% to my_length if (misplaced)

	gosub check_ringins if (unmatched)

	score = placed*bonus						&
			+ interchanges*benefit				&
			+ ring_in*wager					&
			+ (misplaced - interchanges)*advantage
	score = score - interchanges*advantage if (interchanges > 2%)
	score = score + distance*disadvantage/misplaced			&
					if (misplaced <> zero)
	score = score + (length_difference - 1%)*disadvantage		&
					if (length_difference > 1%)

	score_percentage = int(1.0*score/maximum_score*100.0)
	score_percentage = zero if (score_percentage < zero)

 return

do_swaps_and_distance:

	change mine to mine_i

	if ((mine_i(position) > zero) and (mine_i(position) <= my_length))
		then
		if (mine_i(position) <> position)
			then
			distance = distance				&
				    + abs(position - mine_i(position))
			interchanges = interchanges + 1%		&
				if (mine_i(mine_i(position)) = position)
		end if
	end if

	how_far  = distance/misplaced
	distance = zero if (how_far*misplaced = distance)

 return

check_ringins:

	for letter = 1% to your_length
		your_letter = mid$(yours, letter, 1%)
		my_letter   = mid$(mine, letter, 1%)

		if ((my_letter >= 'A') and (my_letter <= 'Z'))
			then
			if ((your_letter > blank) and (my_letter > blank))
				then
				index  = ascii(my_letter) - 64%
				ringer_i = (instr(1%, ringer(index)	&
							,your_letter)	&
						<> zero)
				ring_in = ring_in + 1% if (ringer_i and &
					(ring_in < ringin_maximum))
			end if
		end if
	next letter

	unmatched = unmatched - ring_in

 return

def string substitute (string original, string old_bit, string new_bit)

	declare long len_old_bit
	declare long len_new_bit
	declare long found_it

	len_old_bit = len(old_bit)
	len_new_bit = len(new_bit)
	found_it    = pos(original, old_bit, 1%)

	while found_it
		original = left$(original, found_it - 1%) + new_bit	&
				+ right$(original, found_it + len_old_bit)
		found_it = pos(original, old_bit, found_it + len_new_bit)
	next

	substitute = original

end def

end sub
