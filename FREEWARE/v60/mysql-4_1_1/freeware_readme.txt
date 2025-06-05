MySQL_4_1_1, UTILITIES, MySQL 4.1.1 for OpenVMS

 This is a port of MySQL 4.1.1 to OpenVMS. The source was cloned from
 bitkeeper on 13-Sep-2003. The code was ported on OpenVMS Alpha V7.3-1
 and the PCSI kit included can be installed on OpenVMS Alpha 7.3-1 or
 later. The kit also includes a port of the MySQL DBI driver for perl.
 

 Once the PCSI kit is installed, the MySQL server can be started as follows:

	$ @sys$common:[mysql_4_1_1]mysql_setup
	$ @mysql_root:[mysql.bin]start_mysqld

 The sources can be found in MYSQL_4_1_1-SRC.ZIP.


 
   
