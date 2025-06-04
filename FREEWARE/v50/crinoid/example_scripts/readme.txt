Some example scripts to get you started:

SIMPLE_FORM.CGI  a simple form, with link to CGI.PM docs & examples on the web
DEBUG_FORM.PL    dumps ENV and POST data
DIRECTORY.       makes an HTML directory of CGIs
LINEAR.CGI       linear regression utility using PGPLOT
UPLOAD.CGI       file upload test script (CGI.pm 2.74+)

The following scripts implement Netscape Roaming access; ROAM.CGI is
what actually processes Roaming access save/restores; it uses a file
of username and MD5-encrypted passwords.  You create user entries (and
the password file) with PWDCREATE and can change passwords with PWDCHANGE.
Note that you may want to change the directories used by these three
scripts.  The password file is created in SYS$LOGIN, while the Roaming
files are stored in directory ../netscape_roam  from the CGI script
area.

PWDCREATE and PWDCHANGE use https secure connections when transmitting
passwords, but unfortunately Netscape 4.7 does not allow https
connections for Roaming access.

ROAM.CGI         Netscape roaming access, use with PWDCREATE and PWDCHANGE
PWDCREATE.CGI    Create password or password file for a user
PWDCHANGE.CGI    Change a user's password
