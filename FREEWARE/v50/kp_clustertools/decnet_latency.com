$!
$! DECNET_LATENCY.COM
$!
$! Measure DECnet latency
$!	P1: Remote nodename
$!	P2: [optional] Loop count (default=100 packets)
$!							V1.0 12/5/2000
$! Note: This breaks if it happens to run across Midnight.  Sorry.
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$! http://www.geocities.com/keithparris/ or http://encompasserve.org/~parris/
$!
$	if p1 .eqs. ""
$	then
$		write sys$output "Please supply a DECnet nodename as the P1 parameter."
$		exit
$	endif
$	p1 = p1 - "::"
$	loop_count = 100	!Should take about 6 seconds at 60 ms latency (2500 miles)
$	if f$integer(p2) .gt. 0 then loop_count = f$integer(p2)
$	write sys$output " Looping DECnet data to node ''p1, ''loop_count' packets"
$	start_time = f$cvtime(,,"TIME")
$	mcr ncp loop node 'p1' length 512 count 'loop_count'
$	end_time = f$cvtime(,,"TIME")
$! Calculate the difference between the two timestamps
$	s_h = f$integer(f$element(0,":",start_time))	!Hours
$	s_m = f$integer(f$element(1,":",start_time))	!Minutes
$	temp = f$element(2,":",start_time)		!ss.cc
$	s_s = f$integer(f$element(0,".",temp))	!Seconds
$	s_c = f$integer(f$element(1,".",temp))	!Centi-seconds
$!! show symbol s_*
$	e_h = f$integer(f$element(0,":",end_time))
$	e_m = f$integer(f$element(1,":",end_time))
$	temp = f$element(2,":",end_time)
$	e_s = f$integer(f$element(0,".",temp))
$	e_c = f$integer(f$element(1,".",temp))
$!! show symbol e_*
$	hr = e_h - s_h
$	mn = e_m - s_m
$	sc = e_s - s_s
$	cs = e_c - s_c
$	if cs .lt. 0
$	then
$		cs = cs + 100
$		sc = sc - 1
$	endif
$	if sc .lt. 0
$	then
$		sc = sc + 60
$		mn = mn - 1
$	endif
$	if mn .lt. 0
$	then
$		mn = mn + 60
$		hr = hr - 1
$	endif
$!! write sys$output f$fao("!2UL:!2ZL:!2ZL.!2ZL",hr,mn,sc,cs)	!!!
$	centiseconds = cs + 100*(sc + 60*(mn + 60*hr))
$	milliseconds = centiseconds*10
$!!!	latency_ms = milliseconds/'loop_count'
$	microseconds = milliseconds * 1000
$	latency_us = microseconds/'loop_count'
$	latency_ms_units = latency_us/1000
$	latency_us_fraction = latency_us - (1000*latency_ms_units)
$	write sys$output "  Average latency: ''latency_ms_units'.",-
		f$extract(0,1,f$fao("!3ZL",latency_us_fraction)),-
		" milliseconds."
$	exit
