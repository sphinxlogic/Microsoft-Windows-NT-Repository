#!/usr/bin/awk -f
# tle2edb.awk: awk script to convert NASA "2-line" geocentric orbital elements
# to xephem db. we crack everything that looks reasonable.
# (c) 1993,1995 Elwood Charles Downey
#
# v2: 6/26/95:   allow names up to 22 chars long (official format change).
# v1.3  10/26/93 looks even harder.
# v1.2  9/12/93 fixes for NORAD-format (by bon@LTE.E-TECHNIK.uni-erlangen.de)
# v1.1  9/8/93  change type code to E
# v1.0  8/10/93 initial cut
#
# usage:
#	awk -f tle2edb.awk [file]
#
# or, if you call this file tle2edb, make it executable, your awk is in /usr/bin
# and your system supports #!, then you can also run this script simply as:
#
#	tle2edb [file]
#
#
# Data for each satellite consists of three lines in the following format:
#
# AAAAAAAAAAAAAAAAAAAAAA
# 1 NNNNNU NNNNNAAA NNNNN.NNNNNNNN +.NNNNNNNN +NNNNN-N +NNNNN-N N NNNNN
# 2 NNNNN NNN.NNNN NNN.NNNN NNNNNNN NNN.NNNN NNN.NNNN NN.NNNNNNNNNNNNNN
#
# Line 0 is a 22-character name.
#
# Lines 1 and 2 are the standard Two-Line Orbital Element Set Format identical
# to that used by NORAD and NASA.  The format description is:
#
# Line 1
# Column     Description
#  01-01     Line Number of Element Data
#  03-07     Satellite Number
#  10-11     International Designator (Last two digits of launch year)
#  12-14     International Designator (Launch number of the year)
#  15-17     International Designator (Piece of launch)
#  19-20     Epoch Year (Last two digits of year)
#  21-32     Epoch (Julian Day and fractional portion of the day)
#  34-43     First Time Derivative of the Mean Motion
#         or Ballistic Coefficient (Depending on ephemeris type)
#  45-52     Second Time Derivative of Mean Motion (decimal point assumed;
#            blank if N/A)
#  54-61     BSTAR drag term if GP4 general perturbation theory was used.
#            Otherwise, radiation pressure coefficient.  (Decimal point assumed)
#  63-63     Ephemeris type
#  65-68     Element number
#  69-69     Check Sum (Modulo 10)
#            (Letters, blanks, periods, plus signs = 0; minus signs = 1)
#
# Line 2
# Column     Description
#  01-01     Line Number of Element Data
#  03-07     Satellite Number
#  09-16     Inclination [Degrees]
#  18-25     Right Ascension of the Ascending Node [Degrees]
#  27-33     Eccentricity (decimal point assumed)
#  35-42     Argument of Perigee [Degrees]
#  44-51     Mean Anomaly [Degrees]
#  53-63     Mean Motion [Revs per day]
#  64-68     Revolution number at epoch [Revs]
#  69-69     Check Sum (Modulo 10)
#
# All other columns are blank or fixed.
#
# Example:
#
# NOAA 6
# 1 11416U          86 50.28438588 0.00000140           67960-4 0  5293
# 2 11416  98.5105  69.3305 0012788  63.2828 296.9658 14.24899292346978

# keep rolling successive lines into l1, l2 and l3.
# once they all look reasonable, then generate some output.
{
	sub (/ *$/, "")		# strip trailing blanks from $0
	l1 = l2
	l2 = l3
	l3 = $0

	if (length(l1)>22 || substr(l2,1,2)!="1 " || substr(l3,1,2)!="2 ")
	    next

	# first line, then, is the satellite name
	name = l1

	# pick out the goodies from the first "meaningful" line, l2.
	year = substr(l2,19,2)
	dayno = substr(l2,21,12)
       
	decay = substr(l2,34,10)
	set = substr(l2,65,4)

	# pick out the goodies from the second "meaningful" line, l3.
	inc = substr(l3,9,8)
	ra = substr(l3,18,8)
	e = substr(l3,27,7) * 1e-7
	ap = substr(l3,35,8)
	anom = substr(l3,44,8)
	n = substr(l3,53,11)
	rev = substr(l3,64,5)

	# print in xephem format.
	printf "%s-%d,E,1/%s/19%d,%s,%s,%s,%s,%s,%s,%s,%d\n", \
		name, set, dayno, year, inc, ra, e, ap, anom, n, decay, rev
}
