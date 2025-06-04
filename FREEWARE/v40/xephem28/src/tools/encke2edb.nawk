#!/bin/nawk -f
# convert from Jost Jahn postings on http://encke.jpl.nasa.gov to {x}ephem
# we don't try to select out non-entries.
# v1 1/6/95	first cut
# v2 6/22/95	fix up names a bit

{
	# pick out the name; put year designation first, if any, then whatever
	#   fits of the name up to xephem's max of 13 chars. the designation
	#   is in columns 28..33; we trim off trailing blanks.
	name = substr ($0, 28, 6)
	sub (/ *$/, "", name)
	i = length (name)
	if (i > 0)
	    name = name " " substr ($0, 1, 12-i)
	else
	    name = substr ($0, 1, 13)	# no designation at all
	sub (/ *$/, "", name)

	# inclination
	i = substr ($0, 84, 9) + 0

	# long of asc node
	O = substr ($0, 75, 9) + 0

	# arg of peri
	o = substr ($0, 66, 9) + 0

	# eccentricity
	e = substr ($0, 57, 9) + 0

	# epoch of peri
	T = sprintf ("%g/%g/%g", substr($0,38,2)+0, substr($0,40,8)+0,
							substr($0,34,4)+0)

	# peri distance
	q = substr($0, 48, 9) + 0

	# mag model -- says H/n but works with my g/k
	g = substr($0,93,5) + 0
	k = substr($0,98,4) + 0

	if (e < 1) {
	    # elliptical
	    a = q/(1-e)
	    printf "%s,e,%g,%g,%g,%g,%g,%g,%g,%s,2000,g%g,%g\n", \
		    name, i, O, o, a, 0, e, 0, T, g, k
	} else if (e > 1) {
	    # hyperbolic
	    printf "%s,h,%s,%g,%g,%g,%g,%g,2000,%g,%g\n", \
		    name, T, i, O, o, e, q, g, k
	} else {
	    # parabolic
	    printf "%s,p,%s,%g,%g,%g,%g,2000,%g,%g\n", \
		    name, T, i, o, q, O, g, k
	}
}
