FREE, UTILITIES, Display disk usage & free space summaries for mounted disks

V2.1 - March 26, 2000
Written by Hunter Goatley

FREE is a simple, but handy, utility to display summaries for mounted disks
showing the number of blocks in use, the number of free blocks, and the
total number of blocks per disk.

To build it, simply do:

$ BLISS FREE	!On Alpha, use BLISS/A64
$ LINK FREE

$ FREE :== $dev:[dir]FREE.EXE
$ FREE
Device Name      Volume Label   Type      Used Blocks     Free Blocks      Total
---------------- -------------- ------ --------------- --------------- ---------
$1$DKA0:         AXPVMSSYS      RZL26    1684944 (83%)    365916 (17%)   2050860
$1$DKA100:       GOAT           RZ28     2917632 (71%)   1192848 (29%)   4110480
$1$DKA300:       PROCESS        RZ28     4016452 (98%)     94028 ( 2%)   4110480
$1$VDA0:         SCRATCH        RP06      268813 (88%)     38387 (12%)    307200
$1$VDA3:         MULTINET       RP06     1610617 (79%)    442503 (21%)   2053120

Totals:                5 mounted disks    5372MB (84%)    1090MB (16%)    6462MB
$

Hunter Goatley
goathunter@PROCESS.COM
