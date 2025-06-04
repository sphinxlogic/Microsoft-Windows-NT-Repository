PWD_UPDATE, SYSTEM_MANAGEMENT, allow Helpdesk to changes passwords safely

 				PWD_UPDATE
 			A helpdesk password update routine

requires:FORTRAN,FMS

Author:  Robert Eden                robert@cpvax.tu.com
 	 Comanche Peak S.E.S        eden@fallout.lonestar.org 
         Glen Rose Tx, 76043        
         (817) 897-0491

 
         This program is designed to be installed with SYSPRV and controlled
         via an ACL to allow non-privileged users to change the password of
         certain accounts. I recommend creating a HELPDESK identifier
         and assigning it to HELPDESK personnel.

         The user is prompted for a username and then presented with the
         last LOGIN and PASSWORD CHANGE date.  The caller should be asked these
         questions.  If they are answered correctly, the password can be
         changed.  If not, the account can be disabled.

         This program uses the SYS$SETUAI service to change the password
         and set any relevant flags, but due to a "feature" of LOGINOUT,
         it has to modify the UAF file directly to pre-expire the account.
         (LOGINOUT doesn't look at the PWD_EXPIRED bit to determine if it
         should prompt for a new user)

         There will be no auditing on the password expiration call, but the
         call to SYS$SETUAI will provide an audit record.
         
 	 If you end up using this program, please drop a note to me at
         the above addresses so I know makes use of this stuff!

