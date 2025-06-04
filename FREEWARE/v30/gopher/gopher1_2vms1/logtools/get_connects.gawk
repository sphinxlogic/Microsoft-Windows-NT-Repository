# GET_CONNECTS.GAWK	19930901	D. Sherman
#	This program extracts the connecting address from connection
#	records, and writes it out to a file for further
#	processing.  Startup and Shutdown records are ignored.
#
# Modification Log:
#
#  19930901	D. Sherman	Original version
#

		{printflag = "y"}
/: ======/	{printflag = "n"}
/Stopped /	{printflag = "n"}
/Starting /	{printflag = "n"}

		{if (printflag == "y") printf( "%s\n", $7);
		}

