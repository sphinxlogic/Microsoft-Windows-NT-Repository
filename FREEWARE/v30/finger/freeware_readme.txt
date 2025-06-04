FINGER, UTILITIES, FINGER provides a user-information query service.

Three versions of finger are available on this CD.

NAME
  finger, f - Displays user information

SYNOPSIS

  finger [-bfhilmpqsw] [user ...]

  The finger command displays information about the users in the passwd file.

DESCRIPTION

  By default, information for each user on the host is listed.  This informa-
  tion includes the login name, terminal name and write status (an * (aster-
  isk)) before the terminal name appears (if write permission is denied), and
  login time.

  The finger command uses a longer output format when you specify a user or a
  list of users.  You can use account name, given name, or surname (as listed
  in /etc/passwd) to specify users.  This multiline format includes the
  information described previously, as well as full name, office number, and
  phone number (if known); the user's home directory and login shell; idle
  time; any plan that the user has placed in the file .plan in the user's
  home directory; and the project on which the user is working from the file
  .project in the home directory.  (Idle time is minutes if it is a single
  integer, hours and minutes if a : (colon) is present, or days and hours if
  a d is present.)

  If a host is not specified, the information is for users on the local host;
  otherwise, the information is for users at the specified host.  You can
  specify a user on a remote host by using the form user@host; if you specify
  @host alone, the standard format listing is provided on the remote system.

  The finger command displays user information only if the specified host has
  a fingerd server running or inetd is configured to start fingerd.  (For
  more information on setting up this server for your host, see the
  fingerd(8) daemon reference page.)

  If you want to make information available to other users who run finger on
  your user ID, you can create the following files in your home directory:

  .plan
      A file that contains plans.  The .plan file can contain more than one
      line.

  .project
      A file that states what project you are currently working on.  The
      .project file can contain only one line.

  The f command is a supported alias for the finger command.

FLAGS

  -b  Produces a briefer version of long format output.

  -f  Suppresses display of header line (the first line that defines the
      displayed fields).

  -h  Suppresses printing of .project files in long and brief long formats.

  -i  Produces a quick listing with idle times.

  -l  Forces long output format.

  -m  Assumes user specified is an account name, not a given name or surname.

  -p  Suppresses printing of .plan files in long and brief long formats.

  -q  Produces a quick listing, including only login name, terminal name, and
      login time.

  -s  Forces short output format.

  -w  Forces narrow, short format listing.

EXAMPLES

   1.  To get information about user frankel at host1, enter:
            $ finger frankel@host1
            Login name:  frankel            In real life:  Sam Frankel
            Office:  3D08  ext5555          Home phone:  555-9982
            Directory:  /u/frankel          Shell:  /usr/bin/sh
            Last login Thu Jun 28 10:37 on tty56 from venus.abc.org
            No plan.

   2.  To get information about user chen at host1, when chen has both a
       .plan and a .project file in his home directory, enter:
            $ finger chen@host1
            Login name:  chen                       In real life:  A. B. Chen
            Office:  3D10  ext5322                  Home phone:  210-9876
            Directory:  /u/chen                     Shell: /usr/bin/sh
            On since May 16 11:06 on tty3   1 minute 2 seconds Idle Time
            Project:  aquatic entomology
            Plan:
            Complete Phase 1 research by end of second quarter.
            Produce draft report by end of year.

   3.  To get information about all users logged in to the host host1, enter:
            $ finger @host1

FILES

  /var/adm/utmp
             Contains user and accounting information.

  /etc/passwd
             Contains user information.

  /var/adm/lastlog
             Contains last login times.

  $HOME/.plan
             Contains a user's plan.  (Optional file)

  $HOME/.project
             Contains one-line description of a user's project assignment.
             (Optional file)

RELATED INFORMATION

  Commands:  fingerd(8), hostname(1), inetd(8), rwho(1), users(1).
                                                                                       
