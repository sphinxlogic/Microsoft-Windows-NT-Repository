PROCLIST, UTILITIES, List all processes for this username

This utility generates a list of all processes for the current
username on the system/cluster.  For example:

 Username      Node  Process name       PID    State  # Terminal   Image
 KACHELMYER   MAIN   KACHELMYER       2020225A CUR      LTA5115:   PROCLIST
 KACHELMYER   MYNODE DECW$SESSION     22600183 LEF    1 FTA20:     DECW$SESSION
              MYNODE VUE$KACHELMYE_7  22600217 LEF      (22600183)
 KACHELMYER   MYNODE DECW$MWM         226001AE LEF      MBA258:    DECW$MWM
 KACHELMYER   MYNODE KACHELMYER       226001AF LEF      MBA261:    DECW$CALENDAR
 KACHELMYER   MYNODE VUE$KACHELMYE_3  226001B1 LEF      MBA263:    DECW$CARDFILE
 KACHELMYER   MYNODE LATservice_1     226001B2 LEF    5 MBA267:
              MYNODE PLANET_1         22600315 HIB      (226001B2) LTPAD
              MYNODE CRAYON_1         226002AD HIB      (226001B2) LTPAD
 KACHELMYER   MYNODE DECW$TE_01B3     226001B3 LEF      Other      DECW$TERMINAL
 KACHELMYER   MYNODE _FTA25:          226001B9 LEF      FTA25:
 KACHELMYER   MYNODE VUE$KACHELMYE_8  22600243 LEF      MBA493:

Note that the subprocesses for a given parent are listed under that
parent process.

I use this to get an idea of what I've got going or to help identify a
process that I want to stop.

