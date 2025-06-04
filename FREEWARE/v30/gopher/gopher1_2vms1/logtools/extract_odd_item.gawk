# EXTRACT_ODD_ITEM.GAWK	19930929	D. Sherman
#	This program extracts the items reported in the gopher
#	usage log and writes it to a file for further processing.

# Modification Log:
#
#  19930929	D. Sherman	Original version
#

		{printflag = "y"}
/retrieved /	{printflag = "n"}
/search /	{printflag = "n"}
/=======/	{printflag = "n"}
/Root /		{printflag = "n"}
/Starting/	{printflag = "n"}
/Shutdown/	{printflag = "n"}
/version/	{printflag = "n"}
/VMSSearchstr/	{printflag = "n"}

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

