RESET_PASSWD, SYSTEM_MGMT, Helpdesk Password Change Utility 

  
    Permits non-priv'd user to change password for groups of users whose
    UAF group id is higher than the value of PWDMINGROUP which needs to
    defined in the system logical name table. User must enter the nonechoed
    password twice to complete successfully. In the event target user
    is DISUSER'ed, this image will exit with approriate message, and
    target password remains unchanged. Copy RESET_PASSWD.EXE to the system
    directory sys$sysroot:[sysexe] and change file protection to w:e to
    permit initial image activation. Then execute install.com. Define:
  
        DEFINE/SYSTEM/EXEC PWDMINGROUP 210
  
    where 210 represents (in octal) the minimum group number which the
    reset_passwd tool will permit setting of a users' password. As 
    distributed, RESET_PASSWD will exit with an error if the above logical
    name is not defined in the system logical name table. Also, define
    a foreign symbol to invoke the reset_passwd image:
  
        reset_passwd == "$reset_passwd.exe"
  
    Additionally, the invoking user is checked to see if rights identifier
    PWD_SETPASS has been granted. If not, the image will exit with an error.

    The provided images are VMS 7.2(-1) and DecC 6.2 but there should be no
    problem rebuilding in other environments. The utility checks for the
    required privs and identifiers, but use with approriate caution.

    Lyle W. West

