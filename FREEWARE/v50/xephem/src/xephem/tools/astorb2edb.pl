#!/usr/bin/perl
# convert astorb.dat on stdin to asteroids.edb on stdout.
#
# actually, only those asteroids that might ever be brighter than dimmag are
# sent to stdout; the others are saved in asteroids_dim.edb.
#
# astorb is a set of elements for 30k+ asteroids maintained by Lowell
# Observatory. See ftp://ftp.lowell.edu/pub/elgb/astorb.html. From the
# Acknowledgments section of same:
#   The research and computing needed to generate astorb.dat were funded
#   principally by NASA grant NAGW-1470, and in part by the Lowell Observatory
#   endowment. astorb.dat may be freely used, copied, and transmitted provided
#   attribution to Dr. Edward Bowell and the aforementioned funding sources is
#   made. 
#
# Elwood Downey
#  2 Jul 1996 first draft
# 10 Oct 1996 update for field 18 change; add notion of astorb_dim.edb
#  1 Sep 1997 change name to asteroids*.edb for OCAAS
# 10 Mar 1999 update for field 1 change

# mimic RCS version
$ver = '$Revision: 1.2 $';
$ver =~ s/\$//g;

# setup mag and filename
$dimmag = 13;			# set to dimmest mag to be sent to stdout
$dimfn = "asteroids_dim.edb";	# name of file for dim asteroids

# build some common boilerplate
($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime;
$year += 1900;
$mon += 1;
$when = "# Processed $year-$mon-$mday $hour:$min:$sec UTC\n";
$from = "# From ftp://ftp.lowell.edu/pub/elgb/astorb.dat.gz\n";
$what = "# Generated by astorb2edb.pl $ver by Elwood Downey\n";

# open the dim file
open (DIM, ">$dimfn") || die "Can not create $dimfn\n";

# add boilerplate to each file
print DIM "# Asteroids never brighter than $dimmag.\n";
print DIM $from;
print DIM $what;
print DIM $when;

print "# Asteroids ever brighter than $dimmag.\n";
print $from;
print $what;
print $when;

# column table, from sample FORTRAN format
# 1..5		A5,1X,
# 7..24		A18,1X,
# 26..40	A15,1X,
# 42..46	A5,1X,
# 48..52	F5.2,1X,
# 54..57	A4,1X,
# 59..63	A5,1X,
# 65..68	A4, 1X,
# 70..93	6I4,1X,
# 95..104	2I5,1X,
# 106..109 110..111 112..113  I4,2I2.2,1X,
# 115..124 126..135 137..146 3(F10.6,1X),
# 148..156 	F9.6,1X,
# 158..167 	F10.8, 1X,
# 169..180 	F12.8,1X,
# 182..185 186..187 188..189 I4,2I2.2,1X,

# process each astorb.dat entry
while (<STDIN>) {
    # build the name
    $nmbr = &s(1,5) + 0;
    $name = &s(7,24);
    $name =~ s/^ *//;
    $name =~ s/ *$//;

    # gather the orbital params
    $i = &s(148,156) + 0;
    $O = &s(137,146) + 0;
    $o = &s(126,135) + 0;
    $a = &s(169,180) + 0;
    $e = &s(158,167) + 0;
    $M = &s(115,124) + 0;
    $H = &s(42,46) + 0;
    $G = &s(48,52) + 0;
    $TM = &s(110,111) + 0;
    $TD = &s(112,113) + 0;
    $TY = &s(106,109) + 0;

    # decide whether it's ever bright
    $per = $a*(1 - $e);
    $aph = $a*(1 + $e);
    if ($per < 1.1 && $aph > .9) {
	$fd = STDOUT;	# might be in the back yard some day :-)
    } else {
	$maxmag = $H + 5*&log10($per*&absv($per-1));
	$fd = $maxmag > $dimmag ? DIM : STDOUT;
    }

    # print
    print $fd "$nmbr " if ($nmbr != 0);
    print $fd "$name";
    print $fd ",e,$i,$O,$o,$a,0,$e,$M,$TM/$TD/$TY,2000.0,$H,$G\n";
}

# like substr($_,first,last), but one-based.
sub s {
    substr ($_, $_[0]-1, $_[1]-$_[0]+1);
}

# return log base 10
sub log10 {
    .43429*log($_[0]);
}

# return absolute value
sub absv {
    $_[0] < 0 ? -$_[0] : $_[0];
}

# For RCS Only -- Do Not Edit
# @(#) $RCSfile: astorb2edb.pl,v $ $Date: 1999/03/11 05:26:44 $ $Revision: 1.2 $ $Name:  $
