# Generated automatically from mk_idfiles.sh.in by configure.
# @(#)mk_idfiles.sh.in	1.2 (13 Oct 1993)

# This shell script creates Master, Node, and System files, using the driver 
# name provided by configure (DRIVER_NAME).  We can't just let configure
# create Master from a Master.in, etc., because configure prepends a
# line like "#created automatically by configure..." which confuses idinstall.

rm -f Master Node System

echo "$0: Creating Master... \c"
echo 'tslip	s	Sciof	tsl_	0	0	1	128	-1' > Master

echo "Node... \c"
echo 'clone	tslip	c	tslip' > Node

echo "System"
echo 'tslip	Y	1	0	0	0	0	0	0	0' > System

chmod -w Master Node System
