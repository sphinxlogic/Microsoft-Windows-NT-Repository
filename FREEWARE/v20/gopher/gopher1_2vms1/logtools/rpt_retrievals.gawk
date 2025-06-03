# RPT_RETRIEVALS2.GAWK	19930702	D. Sherman
#	This program takes as input a file listing number of retrievals
#	and item id for each item listed in the gopher log, sorted by 
#	number of retrievals.  It produces a report.

# Modification Log:
#
#  19930706	D. Sherman	Original version
#

BEGIN		{
				# output in machine readable format?
		mrf = (mfmt == "Y" || mfmt == "y"); 
		if (!mrf){	# if not mach. rd. format, print headings
		  # print headings
		  printf("\n\nGopher usage: Summary number of retrievals, sort by number of retrievals\n");
		  printf("%s\n\n", dtm);
		  printf("Retrievals");
		  printf(" Logged Item\n");
		  printf("==========");
		  printf(" ================================================================\n");
		  }
		# initialize variables
		FS = "|";	# field separator
		}

		{ 
		  if ($1 > 0){
		    if (mrf){		# if printing to file...
		      printf( "%i,%s\n", $1, $2 );
		    } else {		# if printing on screen
		      printf( "%8i   %s\n", $1, $2 );
		      }
		    }
		}

	
END		{			# print item totals
		  if ($1 > 0){
		    if (mrf){		# if printing to file...
		      printf( "%i,%s\n", $1, $2 );
		    } else {		# if printing on screen
		      printf( "%8i   %s\n", $1, $2 );
		      }
		    }
		}

