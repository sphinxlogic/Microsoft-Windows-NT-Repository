#
#  GNU awk program for VMS to create a command procedure to execute the
#	suite of grep tests developed by Henry Spencer and distributed
#	with GNU grep.
#
#  Author:	Hunter Goatley, 12-SEP-1991, based on original awk program
#
#  To use:
#
#	$ gawk "F:" -f vmsspencer.awk spencer.tests >spencer.com
#
BEGIN { print "$! DCL command procedure to test GNU egrep";
	print "$ set noon"; print "$ failures=0"; }
!/^#/ && NF == 3 {
	#
	#  Because the exit statuses were changed for VMS, reflect those
	#  changes here.
	#
	if ($1 == 1)
	   stat = 3
	else
	   if ($1 == 2)
	      stat = 44
	   else
	      stat = 1
	print "$ define/user sys$output nl:"
	print "$ define/user sys$error nl:"
	print "$ egrep \042" $2 "\042 >NL:"
	print $3
	printf "$ if $status.ne.%d\n",stat
	printf "$ then write sys$output \042Spencer test \#%d failed\042\n", ++n
	print "$\tfailures=1"
	print "$ endif"
}
END { print "$ write sys$output \042''failures' failures\042"; 
	print "$ exit"; }
