GUESS_PASS, SYSTEM_MANAGEMENT, Test users passwords.

DESCRIPTION:
This tool was developed to get users passwords.  This is to help determine
security issues releating to passwords.  I use this program to verify that new
users don't use "simple" passwords.


Parameter:	USERNAME

Qualifiers:	[no]COUNTWORDS	--  [Don't] Count the words in the dictionary.
		[no]DICTIONARY	--  [Don't] Use the dictionary.
		[no]DOUBLEUP	--  [Don't] Double words in the dictionary.
						ex:  test SAND	  <- Not doubled
						     test MAN     <- Not doubled
						     test SANDMAN <- doubled
						     test MANSAND <- doubled

		INTERVAL=time	--  Interval to display progress.  Default is 10
				    seconds.
						ex: /interval="0 00:01:00"
						    this indicates 0 days, 0
                                                    hours, 1 minute, and 0
                                                    seconds

		MAXADDITIONS=n	--  Largest number to add to the end of the
				    password.  Default is 9.
						ex: test SAND
						    test SAND0
						    test SAND1 ...

		MAXCOLUMNS=n	--  Largest number of columns to test.  Once all
                                    the words have been used in the dictionary,
                                    the program will start to try ALL possible
                                    passwords.  The default number of columns is
                                    set 5.  Which means it will test up to ZZZZZ
                                    then stop testing.  See NOTE #1 for more
                                    explanation.

		METHOD=method	--  Search method.  This does not apply to the
                                    dictionary words.  Options are BVM and JDW. 
                                    BVM is the default.  See NOTE #2 for more
                                    explanation.

		[no]REVERSE	--  [Don't] Reverse the passwords.
						ex: test SAND	<- not reversed
						    test DNAS	<- reversed password


NOTE #1: BVM method will test 0, 1, 2..., A, B, C..., 00, 10, 20..., A0, B0,
         C0..., YZ, ZZ, 000, 100, 200...  The default number of columns to test
         is 5.  Which means it will test up to ZZZZZ then stop testing.  See

NOTE #2: JDW method will test 0, 1, 2..., A, B, C..., 00, 01, 02..., 0A, 0B,
         0C...  The right most column will increase the fastest, where the BVM
         method will increase the left most column the fastest.

