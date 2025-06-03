#!/bin/sh

#
# Cpus - prepare data for the Cpus C program that puts
#        up a pretty window showing who's using what CPU.
#
# NOTE:  There are several things in this file that need to
#        be edited in order to get it running on your machine.
#        
#        1) If you are not running a BSD-alike unix, you will
#           probably need to change the PS_FLAGS to be some
#           string of flags that you can pass to ps which will
#           give you the user name and % of CPU of job in the output.
#
#        2) Set PS_USER_NAME_FIELD to be the number of the field
#           that contains the user name in ps's output.
#
#        3) Set PS_CPU_PERCENT_FIELD to be the number of the field
#           that contains the % of CPU taken by the job in ps's output.
#
#        4) Set DEFAULT_MACHINES to be a list of machines that should
#           be examined by default.
#
#        5) Set CPUS_EXECUTABLE to the full path to the executable
#           you get from compiling cpus.c
#
#        All these changes take place immediately following this comment.
#
# Terry Jones (terry@santafe.edu)
# May 4, 1993.
#

PS_FLAGS=-ef
PS_USER_NAME_FIELD=1
PS_CPU_PERCENT_FIELD=4
DEFAULT_MACHINES="allegro adagio onlyyou"
CPUS_EXECUTABLE=/opt/bin/cpus

#
# You shouldn't need to change anything below this line.
#
case $# in
	0) machines="$DEFAULT_MACHINES";;
	*) machines="$@";;
esac

echo "Gathering statistics... " >&2

(echo $machines | wc -w | tr -d ' '

for machine in $machines
do
	echo "$machine " >&2
	echo -n "$machine "
	rsh $machine nice -19 ps $PS_FLAGS | tail +2 | grep -v "ps $PS_FLAGS" | sort -r | awk '{
		if ($'$PS_USER_NAME_FIELD' != last) {
                	if (NR != 1){
				if (total >= 1.0){
	                       	 	printf "%s=%f ", last, total
				}
               		}
			total = $'$PS_CPU_PERCENT_FIELD'
                	last = $'$PS_USER_NAME_FIELD'
        	}
        	else {
                	total += $'$PS_CPU_PERCENT_FIELD'
        	}}

        	END { 
			if (total >= 1.0){
				printf "%s=%f\n", last, total
			}
			else {
				printf "\n"
			}
		}' 
done) | $CPUS_EXECUTABLE
