{************************************************************************
*									*
*  JIBDEF								*
*									*
*  Declarations for Job Information Block in PCB.			*
*									*
************************************************************************}

CONST

JIB$C_DETACHED	= 0 ;
JIB$C_NETWORK	= 1 ;
JIB$C_BATCH	= 2 ;
JIB$C_LOCAL	= 3 ;
JIB$C_DIALUP	= 4 ;
JIB$C_REMOTE	= 5 ;
JIB$M_BYTCNT_WAITERS	= 1 ;
JIB$M_TQCNT_WAITERS	= 2 ;
JIB$K_LENGTH	= 148 ;
JIB$C_LENGTH	= 148 ;
JIB$S_JIBDEF	= 148 ;
JIB$L_MTLFL	= 0 ;
JIB$L_MTLBL	= 4 ;
JIB$W_SIZE	= 8 ;
JIB$B_TYPE	= 10 ;
JIB$B_DAYTYPES	= 11 ;
JIB$S_USERNAME	= 12 ;
JIB$T_USERNAME	= 12 ;
JIB$S_ACCOUNT	= 8 ;
JIB$T_ACCOUNT	= 24 ;
JIB$L_BYTCNT	= 32 ;
JIB$L_BYTLM	= 36 ;
JIB$L_PBYTCNT	= 40 ;
JIB$L_PBYTLIM	= 44 ;
JIB$W_FILCNT	= 48 ;
JIB$W_FILLM	= 50 ;
JIB$W_TQCNT	= 52 ;
JIB$W_TQLM	= 54 ;
JIB$L_PGFLQUOTA	= 56 ;
JIB$L_PGFLCNT	= 60 ;
JIB$L_CPULIM	= 64 ;
JIB$W_PRCCNT	= 68 ;
JIB$W_PRCLIM	= 70 ;
JIB$W_SHRFCNT	= 72 ;
JIB$W_SHRFLIM	= 74 ;
JIB$W_ENQCNT	= 76 ;
JIB$W_ENQLM	= 78 ;
JIB$W_MAXJOBS	= 80 ;
JIB$W_MAXDETACH	= 82 ;
JIB$L_MPID	= 84 ;
JIB$L_JLNAMFL	= 88 ;
JIB$L_JLNAMBL	= 92 ;
JIB$L_PDAYHOURS	= 96 ;
JIB$L_ODAYHOURS	= 100 ;
JIB$B_JOBTYPE	= 104 ;
JIB$B_FLAGS	= 105 ;
JIB$V_BYTCNT_WAITERS	= 0 ;
JIB$V_TQCNT_WAITERS	= 1 ;
JIB$L_ORG_BYTLM	= 108 ;
JIB$L_ORG_PBYTLM	= 112 ;
JIB$L_SPARE	= 116 ;
JIB$L_CWPS_TIME	= 120 ;
JIB$L_CWPS_COUNT	= 124 ;
JIB$S_CWPS_Q1	= 8 ;
JIB$Q_CWPS_Q1	= 128 ;
JIB$L_CWPS_L1	= 136 ;
JIB$L_CWPS_L2	= 140 ;
JIB$L_JTQUOTA	= 144 ;

