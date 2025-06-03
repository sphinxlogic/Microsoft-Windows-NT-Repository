# COUNT_RETRIEVALS.GAWK	19930706	D. Sherman
#	This program takes as input a file of information extracted
#	from the gopher log and sorted, and creates a file in the 
#	format #|item, where # is the number of times item has been
#	stored in the gopher log.
#
# Modification Log:
#
#  19930706	D. Sherman	Original version
#

BEGIN		{
		# initialize variables
		hold_item = " ";# item in first rec in log
		num_connects = 0; # number of connections for current item
		}

		{ 
		if (FNR == 1) hold_item = $0; # init first item
		if (hold_item != $0) {
		  printf( "%08i|%s\n", num_connects, hold_item );
		  hold_item = $0;
		  num_connects = 1;	# initialize number of connects
		} else {
		  num_connects++	# increment number of connections
		  }
		}

	
END		{			# print item totals
		printf( "%08i|%s\n", num_connects, hold_item );
		}

