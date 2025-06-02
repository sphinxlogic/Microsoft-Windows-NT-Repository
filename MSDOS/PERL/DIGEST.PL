#! /usr/bin/perl
# Modified: 18 October 1992
#
#    This is 'digest' a program to run elm on a digest as a folder.
#    Copyright (C) 1990-1991  David J. Camp
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 1, or (at your option)
#    any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# david@campfire.stl.mo.us           ^     David J. Camp                #
# wupost.wustl.edu!campfire!david  < * >   +1 314 382 0584              #
#                                    v     "God loves material things." #
#         I am a member of the League for Programming Freedom.          #
#
# from the elm menu, type |digest<return>
#
$first = 1;
if (-t STDIN)
    {
    print STDERR "Usage: \"|digest\" from elm menu\n";
    exit 3;
    }
$| = 1;
open (FOLDER, ">/tmp/dig$$");
$first = 1;
while (<>)
    {
#    print $_;
    if ($first)
        {
        if (! m/^From /)
            {
	    print FOLDER "From dummy Wed Feb  29 12:12:12 1990\n";
            }
        $first = 0;
        }
    $match = 0;
    if (/^[0-9]*\)$/)
	{ $match = 1; }
    if (/^\([0-9]*\) ----\n$/)
	{ $match = 1; }
    if (/^*\n$/)
	{ $match = 1; }
    if (/^------\n$/)
	{ $match = 1; }
    if (/^-----------------------------\n$/)
	{ $match = 1; }
    if (/^------------------------------\n$/)
	{ $match = 1; }
    if (/^-------------------------------------------------------\n$/)
	{ $match = 1; }
    if (/^============================================================\n$/)
        { $match = 1; }
    if
(/^----------------------------------------------------------------------*\n$/)
	{ $match = 1; }
    if
(/^------------------------------------------------------------------------=\n$/)
	{ $match = 1; }
    if 
(/^=========================================================================$/)
	{ $match = 1; }
    if 
(/^========================================================================/)
	{ $match = 1; }
if ($match || $first)
        {
	$break = $_;
	$_ = "";
	for ($ctr = 0; ! (eof() || /[\041-\177]/); $ctr++)
            {
            $_ = <>;
            }
	if (/^[^ \t\r\n:]+: /)
	    {
	    print FOLDER "From dummy Wed Feb  29 12:12:12 1990\n";
	    }
	else
	    {
	    print FOLDER $break;
	    for ($ctr--; $ctr > 0; $ctr--)
		{
		print FOLDER "\n";
		}
	    }
        }
    print FOLDER $_;
    $first = 0;
    }
close (FOLDER);
exec ("elm -f /tmp/dig$$ <&2 ; /bin/rm -f /tmp/dig$$");
