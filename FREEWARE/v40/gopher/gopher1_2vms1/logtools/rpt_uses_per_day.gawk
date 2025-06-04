# RPT_USES_PER_DAY.GAWK	1993-07-02	D. Sherman
#	This program will report the number of uses of the Gopher
#	service, as recorded in _gopher.LOG, on a daily basis.  A list of 
#	dates with the number of uses on that date will be produced, and
#	the day with the highest number of uses will be reported.  The
#	average number of connections per day will be reported.
#
# Modification Log:
#
#  19930702	D.Sherman	Original version, based on UnCover report
#

BEGIN		{
				# output in machine readable format?
		mrf = (mfmt == "Y" || mfmt == "y"); 
		if (!mrf){	# if not mach. rd. format, print headings
		  # print headings
		  printf("\n\nGopher usage: Number of Connections / Day\n");
		  printf("%s\n\n", dtm);
		  printf("Date                   ");
		  printf("    Connections\n");
		  printf("====================== ");
		  printf("===================\n");
		  }
		# initialize variables
		num_connect = 0;	# connections
		tot_num_connect = 0; 	# total connections
		num_restarts = 0;	# number of server restarts
		hold_date = " ";	# date of first rec in log
		high_date = " ";	# init date of most activity
		num_high_date = 0; 	# init total of high date
		num_dates = 0;		# number of dates reported
		}

		{ 
		rec_date = sprintf( "%s-%s-%s", $2,$3,$5); # build dd-mmm-yyyy date

		if (FNR == 1) hold_date = rec_date; # init first date
		if (hold_date != rec_date)
		  {			# print date totals
		  if (mrf){		# if printing to file...
		    printf( "%s,%i\n", hold_date, num_connect );
		  } else {		# if printing on screen
		    printf( "%-28s%8i\n", hold_date, num_connect );
		    }
		  num_dates++; 		# keep track of how many days
		  if (num_high_date < num_connect)
		    {			# reset date of highest activity
 		    num_high_date = num_connect;
		    high_date = hold_date;
		    }
		  hold_date = rec_date;
		  num_connect = 0;	# connections
		  }
		}

		{ num_connect++; tot_num_connect++}
/: ======/ 	{ num_connect--; tot_num_connect--}
/Stopped /	{ num_connect--; tot_num_connect--}
/Starting /	{ num_connect--; tot_num_connect--; num_restarts++}
	
END		{			# print date totals
		if (mrf){		# if printing to file...
		  printf( "%s,%i\n", hold_date, num_connect );
		} else {		# if printing on screen
		  printf( "%-28s%8i\n", hold_date, num_connect );
		  }
		if (num_high_date < num_connect)
		  {			# reset date of highest activity
 		  num_high_date = num_connect;
		  high_date = hold_date;
		  }
		if (!mrf){		# only do this if printing on screen
		  num_dates++; 		# keep track of how many days
		  label = "Total connections:";
		  printf( "\n\n%-22s%14i", label, tot_num_connect );
		  label = "Average connections/day:";
		  printf( "\n\n%-26s%10i", label, tot_num_connect/num_dates );
		  printf( "\n\nMost Activity: %s", high_date );
		  label = "Number of restarts:";
		  printf( "\n\n%-23s%13i", label, num_restarts );
		  }
		}

