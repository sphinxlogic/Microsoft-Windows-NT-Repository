# EXTRACT_ITEM.GAWK	19930702	D. Sherman
#	This program extracts the item retrieved from the gopher
#	usage log and writes it to a file for further processing.
#	Only one record is reported per restart.

# Modification Log:
#
#  19930702	D. Sherman	Original version
#

		{printflag = "n"}
/retrieved /	{printflag = "y"}
/search /	{printflag = "y"}

		{if (printflag == "y") {
		  $1 = "";
		  $2 = "";
		  $3 = "";
		  $4 = "";
		  $5 = "";
		  $6 = "";
		  $7 = "";
		  $8 = "";
		  line = substr( $0,9 );	# remove 9 leading spaces
		  printf( "%s\n", line);
		  }
		}

