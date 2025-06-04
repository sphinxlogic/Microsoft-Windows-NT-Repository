! PWD_UPDATE: makefile
!
!       A utility to allow a helpdesk to change non-privledged passwords.
!
!       Robert Eden
!       Techinical Support
!       Comanche Peak Steam Electric Station
!       Texas Utilities
!       POB 1002 (MZ E05)
!       Glen Rose, Tx 76043
!       (817) 897-0491
!       robert@cpvax.cpses.tu.com

PWD_UPDATE.EXE : PWD_UPDATE.OBJ, PWD_UPDATE.VEC, PWD_UPDATE.MEM
                 LINK/NOTRACE/NODEBUG/EXEC=$@ $+

PWD_UPDATE.OBJ : PWD_UPDATE.FOR, UAFDEF.FOR

PWD_UPDATE.MEM : PWD_UPDATE.FLB
                  FMS/MEMORY/OUTPUT=PWD_UPDATE.MEM PWD_UPDATE.FLB

PWD_UPDATE.VEC : PWD_UPDATE.FLB
                  FMS/VECTOR/OUTPUT=PWD_UPDATE.VEC PWD_UPDATE.FLB

PWD_UPDATE.FLB : PWD_UPDATE.FRM

PWD_UPDATE.FRM : PWD_UPDATE.FLG
                  FMS/TRANSLATE/OUTPUT=$@ $+
