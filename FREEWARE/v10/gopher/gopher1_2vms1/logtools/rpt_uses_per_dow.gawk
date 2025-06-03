# RPT_USES_PER_DOW.GAWK	19930702	D. Sherman
#	This program will report the number of uses of the Gopher
#	server, as recorded in _gopher.LOG, by day of the week.  A list of 
#	days with the number of uses on that day will be produced. 
#
# Modification Log:
#
#  19930702	D. Sherman	Original version based on UnCover report
#

BEGIN		{
				# output in machine readable format?
		mrf = (mfmt == "Y" || mfmt == "y"); 
		if (!mrf){	# if not mach. rd. format, print headings
		  # print headings
		  printf("\n\nGopher usage: Number of Connections / Weekday\n");
		  printf("%s\n\n", dtm);
		  printf("Weekday                ");
		  printf(" Connections\n");
		  printf("====================== ");
		  printf("=============\n");
		  }
		# initialize variables
		hold_dow = " ";# day of week of first rec in log
		num_connects = 0; # number of connections for current weekday
		}

		{ 
		rec_dow = substr( $1, 2, 3 ); # extract dow w/o first char

		if (FNR == 1) hold_dow = rec_dow; # init first weekday
		if (hold_dow != rec_dow)
		  {			# print dow totals
		  if (mrf){		# if printing to file...
		    printf( "%s,%i\n", hold_dow, num_connects );
		  } else {		# if printing on screen
		    printf( "%-28s%8i\n", hold_dow, num_connects );
		    }
		  hold_dow = rec_dow;
		  num_connects = 1;	# initialize number of connects
		} else {
		  num_connects++	# increment number of connections
		  }
		}

	
END		{			# print dow totals
		if (mrf){		# if printing to file...
		  printf( "%s,%i\n", hold_dow, num_connects );
		} else {		# if printing on screen
		  printf( "%-28s%8i\n", hold_dow, num_connects );
		  }
		}

