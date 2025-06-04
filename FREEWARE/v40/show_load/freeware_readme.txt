SHOW_LOAD, SYSTEM_MANAGEMENT, Display load averages in DCL

This directory contains a modification of the SHOW command which allows one to
display 1, 5, and 15 minute load averages of the current node.  Load averages
for the number of computables processes and disk queue lengths are displayed.
The top 5 CPU users can also be displayed with the /HOGS qualifier.  If a user
is running at a rate that is consuming more than 30 CPU minutes per hour of
connect time (MPH), the user is flagged with an asterisk.  
