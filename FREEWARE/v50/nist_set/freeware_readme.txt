NIST_SET, SYSTEM_MGMT, Coordinate system clock with NIST-ACTS time

This program sets the system clock from or compares it with NIST-ACTS, the
National Institute of Standards and Technology Automated Computer Time Service. 
It uses modem commands given by the user to dial the service, then reads and
responds to the messages that are sent by NIST-ACTS.  NIST-ACTS will compute
the actual round trip delay and if it receives four consecutive echos that fall
within a certain range of variation, it will calculate the round trip time and
divide by two, then send a special character in a final message to indicate
that time synchronization has been achieved.  If synchronization can be
achieved, and if the SYS$SETIME service is enabled, the program will set the
clock.  In any case, it logs all data traffic along with the difference between
the system time and NIST time.

SYS$SETIME service may be disabled on your system if it is part of a cluster or
it uses a time service like DTSS.  The program must run with both OPER and
LOG_IO privileges.

This program has worked with both a direct-connect line (TTA1 on an AS1200 and
an ES40, TTA0 on a DEC3000/600S) and with a LAT line through a terminal server
(a DS200MC) to an outgoing modem.  The LAT line synchronizes less often than the
direct-connect line because the terminal server is another source of variable
delay (the DS200MC adds its own delay of approximately 20 milliseconds).  Of
course, any connection can fail to synchronize if any resource (system, terminal
server, telephone line) is subject to variable loads.

Files in the saveset:

FREEWARE_README.TXT	This file
NIST_FAQ.LIS		FAQ on the use of NIST-ACTS
NIST_SET.COM		Sample com file to set the clock
NIST_SET_A.EXE		Alpha executable built on VMS v7.1-1h2
			 with Fortran90 v7.1
NIST_SET_DIAL.PAR	Sample modem command file
NIST_SET_HIST.LIS	Sample history list
NIST_SET_LOG.LIS	Sample data traffic log
[.SRC]NIST_SET_A.FOR	Fortran90 source

Suggested usage:

0. Look at the comments in [.SRC]NIST_SET_A.FOR.  There are explanations of all
  input and output files, messages, and suggested usage (also reproduced in
  1 - 6 below).

1. Make sure the SYS$TIMEZONE_DIFFERENTIAL is set properly.  (See
  SYS$MANAGER:UTC$CONFIGURE_TDF.COM.)

2. Run the program with DF_SETTIME set to N.  Look at the log file to
  determine the difference between the system time and the NIST time.  If the
  difference is too large, check your system setup.

3. When you are satisfied, run the program with DF_SETTIME set to Y.

4. If you are not achieving synchronization, it's probably because the
  program is not running at a high enough priority, or, if using a LAT line,
  because the terminal server is too busy.

5. The program should work with both direct-connect and LAT terminal lines.

6. Suggested com file:
  $ on error then goto error_exit
  $ if p1 .eqs. "" then p1 := <default terminal line, for example TTA1>
  $ if p2 .eqs. "" then p2 := N
  $ all 'p1'
  $ if $severity .ne. 1 then exit $status
  $ set ter 'p1' /pasthru /nohostsync /nottsync /altypeahd -
   /nobroadcast /noecho /nowrap /nodialup
  $ ass/use 'p1' lu_term
  $ ass/use nist_set_dial.par lu_dial
  $ def/use df_settime "''p2'"
  $ ass/use nist_set_hist.lis lu_hist
  $ ass/use nist_set_log.lis for$print
  $ run nist_set_a
  $error_exit:
  $ save_status = $status
  $ on error then continue
  $ deall 'p1'
  $ sho sym save_status
  $ exit save_status

Dale Dellutri, ddellutr@enteract.com, 1998-01-08
C & D Commodities, Inc.
