# DAY_FIRST.GAWK	19930702	D. Sherman
#	This program extracts the day of the week from connection
#	records, prepends a digit so a sort will order the day of 
#	the week correctly, and writes it out to a file for further
#	processing.  Startup and Shutdown records are ignored.
#
# Modification Log:
#
#  19930702	D. Sherman	Original version
#

		{printflag = "y"}
/: ======/	{printflag = "n"}
/Stopped /	{printflag = "n"}
/Starting /	{printflag = "n"}

		{if (printflag == "y")
		    if      ($1 == "Mon") printf("1%s\n", $1)
		    else if ($1 == "Tue") printf("2%s\n", $1)
		    else if ($1 == "Wed") printf("3%s\n", $1)
		    else if ($1 == "Thu") printf("4%s\n", $1)
		    else if ($1 == "Fri") printf("5%s\n", $1)
		    else if ($1 == "Sat") printf("6%s\n", $1)
		    else if ($1 == "Sun") printf("7%s\n", $1);
		}

