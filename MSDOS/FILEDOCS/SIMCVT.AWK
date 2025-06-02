#! /usr/bin/awk -f
# simcvt.awk
# Usage:  awk -f simcvt.awk simibm.idx > simibm.lst
# Author: Daniel Lewart (d-lewart@uiuc.edu)

BEGIN {
	FS = "\""
	print "        WSMR-SIMTEL20.ARMY.MIL [192.88.110.20] MS-DOS FILE LIST"
	print ""
	print "NOTE: Type B is Binary; Type A is ASCII"
	print ""
}

{
	dir2 = $2$4
	if ( dir1 != dir2 ) {
		if ( dir1 != "" ) print ""
		print "Directory", dir2
		print " Filename   Type Length   Date    Description"
		print "=============================================="
		dir1 = dir2
	}
	Name = $6
	split($7,a,",")
	Type = "?"
	if (a[4] == 7) Type = "A"
	if (a[4] == 8) Type = "B"
	Size = a[3]
	Date = a[5]
	Description = $8
	printf("%-12s  %s %7d  %6d  %s\n",Name,Type,Size,Date,Description)
}
