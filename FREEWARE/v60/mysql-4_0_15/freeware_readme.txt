MySQL, Utilities, MySQL Database

© Tous droits réservés 2003 Jean-François PIÉRONNE

MySQL V4.0.15
Software Requirements

   1. CPQ SSL V1.0-B or V1.1 (available as a free download from HP)
   2. JFP ZLIB 1.1-4

New SYSUAF Account

This installation procedure creates the unprivileged SYSUAF account MYSQL_SERVER
to run the server. After installation is complete, you will want to review the
security settings on all volumes, directories, and files this account will
access during operation and adjust them according to your needs.

By default, the installation procedure installs MySQL in the SYS$COMMON:[MYSQL]
directory. To install it in another directory, dev:[dir], use the /DESTINATION
parameter of the PRODUCT command. In this case, MySQL will be installed in the
dev:[dir.MYSQL] directory. Installation

   1. Make the directory which holds the ZIP file your default directory
   2. Extract the PCSI kit from the ZIP archive.

$ UNZIP "-V" MYSQL-V0400-15-1

   3. Install MySQL to your chosen destination directory.

$ PRODUCT INSTALL mysql  (default)

  or ... 

$ PRODUCT INSTALL mysql /DESTINATION=dev:[dir]

   4. Change the default directory to the VMS-specific directory and run the 
      two DCL procedure files, LOGICALS.COM and SYMBOLS.COM, to define the 
      necessary logicals and symbols for MySQL.

$ SET DEFAULT SYS$COMMON:[MYSQL.VMS]  (default)

  or ...

$ SET DEFAULT dev:[dir.MYSQL.VMS]

  then ...

$ @LOGICALS "/SYSTEM/EXEC"
$ @SYMBOLS

   5. (Optional) Configure the MySQL package to suit your environment by 
      editing the file MYSQL_ROOT:[VMS]MY.CNF.
   6. Run the special DCL procedure FIRST_START_MYSQLD.COM to start the 
      MySQL server for the first time.

$ @[.MYSQL]FIRST_START_MYSQLD

. . .and wait here a few moments for the database to be created.

   7. Run the special DCL procedure MYSQL_PRIVILEGES.COM to create the 
      system tables.

$ @[.MYSQL]MYSQL_PRIVILEGES
    

   8. Start the MySQL monitor (client).

$ mysql
Welcome to the MySQL monitor.  Commands end with ; or \g.
Your MySQL connection id is 1 to server version: 4.0.15-log

Type 'help;' or '\h' for help. Type '\c' to clear the buffer.

   9. Connect to the master system table and display the existing password 
      for ROOT (should be "" at this point).

mysql> use mysql
Database changed
mysql> select Host, User, Password from user;
+-----------+------+----------+
| Host      | User | Password |
+-----------+------+----------+
| localhost | root |          |
+-----------+------+----------+
1 row in set (0.00 sec)

  10. IMPORTANT! Define a password for the ROOT MySQL account.

mysql> update user set Password=PASSWORD('myrootpassword') where User='root';
Query OK, 0 rows affected (0.00 sec)
Rows matched: 1  Changed: 1  Warnings: 0

mysql> select Host, User, Password from user;
+-----------+------+------------------+
| Host      | User | Password         |
+-----------+------+------------------+
| localhost | root | 6b6403a743ef6b56 |
+-----------+------+------------------+
1 row in set (0.00 sec)

mysql> flush privileges;
Query OK, 0 rows affected (0.16 sec)
$

  11. (Optional): You must also change the HOST field contents if you want 
      to access MySQL from another host using the ROOT account. For example, 
      setting HOST to '%' allows access from anywhere.

  12. (Optional): Verify your password and/or host changes took effect.

$ MYSQLADMIN -u ROOT -p STATUS
Enter password:
                Uptime: 25  Threads: 1  Questions: 1  Slow queries: 0  Opens: 6
 Flush tables: 1  Open tables: 0  Queries per second avg: 0.040
$

  13. Shut down the MySQL server process.

mysql> exit
Bye

$ MYSQLADMIN -u ROOT -p SHUTDOWN

$ MYSQLADMIN -u ROOT -p SHUTDOWN   That's right, quit twice :-)

  14. Finally, run the DCL procedure MYSQL_STARTUP.COM.

(You might want to add this line to your SYSTARTUP_VMS.COM file.)

$ @MYSQL_ROOT:[VMS]MYSQL_STARTUP

You should now have a running MySQL server!


