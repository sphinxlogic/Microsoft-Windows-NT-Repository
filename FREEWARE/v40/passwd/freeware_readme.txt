PASSWD, SYSTEM_MANAGEMENT, Password Utility Routines

PWDEXP - takes one parameter, the username. Displays date
    password last changed, date password will expire, password
    lifetime, and, if set displays DISUSER flag.

PWDSET - username parameter required. Will prompt for new password
    which is not echoed. It then prompts for password verification,
    again not echoed. This procedure bypasses the history, dictionary,
    and length restrictions of AUTHORIZE. It is intended for use
    by system managers in unusual situations, and should not be made
    available globally. 

The provided images are VMS 7.1 and DecC 5.6 but there should be no
problem rebuilding in other environments. Each utility attempts to
check for required privs, but use with approriate caution.

Lyle W. West
