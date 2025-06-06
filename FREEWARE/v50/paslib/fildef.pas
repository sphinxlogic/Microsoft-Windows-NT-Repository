{************************************************************************
*									*
*  FILDEF								*
*									*
*  File Header Definitions from LIB.MLB					*
*									*
************************************************************************}

CONST

FH1$C_LEVEL1	= 257 ;
FH1$K_LENGTH	= 46 ;
FH1$C_LENGTH	= 46 ;
FH1$S_FH1DEF	= 512 ;
FH1$B_IDOFFSET	= 0 ;
FH1$B_MPOFFSET	= 1 ;
FH1$S_FID	= 4 ;
FH1$W_FID	= 2 ;
FH1$W_FID_NUM	= 2 ;
FH1$W_FID_SEQ	= 4 ;
FH1$W_STRUCLEV	= 6 ;
FH1$W_FILEOWNER	= 8 ;
FH1$B_UICMEMBER	= 8 ;
FH1$B_UICGROUP	= 9 ;
FH1$W_FILEPROT	= 10 ;
FH1$S_SYSPRO	= 4 ;
FH1$V_SYSPRO	= 0 ;
FH1$S_OWNPRO	= 4 ;
FH1$V_OWNPRO	= 4 ;
FH1$S_GROUPPRO	= 4 ;
FH1$V_GROUPPRO	= 8 ;
FH1$S_WORLDPRO	= 4 ;
FH1$V_WORLDPRO	= 12 ;
FH1$W_FILECHAR	= 12 ;
FH1$B_USERCHAR	= 12 ;
FH1$V_NOBACKUP	= 1 ;
FH1$V_READCHECK	= 3 ;
FH1$V_WRITCHECK	= 4 ;
FH1$V_CONTIGB	= 5 ;
FH1$V_LOCKED	= 6 ;
FH1$V_CONTIG	= 7 ;
FH1$B_SYSCHAR	= 13 ;
FH1$V_SPOOL	= 4 ;
FH1$V_BADBLOCK	= 6 ;
FH1$V_MARKDEL	= 7 ;
FH1$S_RECATTR	= 32 ;
FH1$W_RECATTR	= 14 ;
FH1$W_CHECKSUM	= 510 ;

FH2$C_LEVEL1	= 257 ;
FH2$C_LEVEL2	= 512 ;
FH2$M_NOBACKUP	= 2 ;
FH2$M_WRITEBACK	= 4 ;
FH2$M_READCHECK	= 8 ;
FH2$M_WRITCHECK	= 16 ;
FH2$M_CONTIGB	= 32 ;
FH2$M_LOCKED	= 64 ;
FH2$M_CONTIG	= 128 ;
FH2$M_BADACL	= 2048 ;
FH2$M_SPOOL	= 4096 ;
FH2$M_DIRECTORY	= 8192 ;
FH2$M_BADBLOCK	= 16384 ;
FH2$M_MARKDEL	= 32768 ;
FH2$M_NOCHARGE	= 65536 ;
FH2$M_ERASE	= 131072 ;
FH2$M_ONLY_RU	= 1 ;
FH2$M_RUJNL	= 2 ;
FH2$M_BIJNL	= 4 ;
FH2$M_AIJNL	= 8 ;
FH2$M_ATJNL	= 16 ;
FH2$M_NEVER_RU	= 32 ;
FH2$M_JOURNAL_FILE	= 64 ;
FH2$C_RU_FACILITY_RMS	= 1 ;
FH2$C_RU_FACILITY_DBMS	= 2 ;
FH2$C_RU_FACILITY_RDB	= 3 ;
FH2$C_RU_FACILITY_CHKPNT	= 4 ;
FH2$K_LENGTH	= 80 ;
FH2$C_LENGTH	= 80 ;
FH2$K_SUBSET0_LENGTH	= 88 ;
FH2$C_SUBSET0_LENGTH	= 88 ;
FH2$K_FULL_LENGTH	= 108 ;
FH2$C_FULL_LENGTH	= 108 ;
FH2$S_FH2DEF	= 512 ;
FH2$B_IDOFFSET	= 0 ;
FH2$B_MPOFFSET	= 1 ;
FH2$B_ACOFFSET	= 2 ;
FH2$B_RSOFFSET	= 3 ;
FH2$W_SEG_NUM	= 4 ;
FH2$W_STRUCLEV	= 6 ;
FH2$B_STRUCVER	= 6 ;
FH2$B_STRUCLEV	= 7 ;
FH2$S_FID	= 6 ;
FH2$W_FID	= 8 ;
FH2$W_FID_NUM	= 8 ;
FH2$W_FID_SEQ	= 10 ;
FH2$W_FID_RVN	= 12 ;
FH2$B_FID_RVN	= 12 ;
FH2$B_FID_NMX	= 13 ;
FH2$S_EXT_FID	= 6 ;
FH2$W_EXT_FID	= 14 ;
FH2$W_EX_FIDNUM	= 14 ;
FH2$W_EX_FIDSEQ	= 16 ;
FH2$W_EX_FIDRVN	= 18 ;
FH2$B_EX_FIDRVN	= 18 ;
FH2$B_EX_FIDNMX	= 19 ;
FH2$S_RECATTR	= 32 ;
FH2$W_RECATTR	= 20 ;
FH2$L_FILECHAR	= 52 ;
FH2$V_NOBACKUP	= 1 ;
FH2$V_WRITEBACK	= 2 ;
FH2$V_READCHECK	= 3 ;
FH2$V_WRITCHECK	= 4 ;
FH2$V_CONTIGB	= 5 ;
FH2$V_LOCKED	= 6 ;
FH2$V_CONTIG	= 7 ;
FH2$V_BADACL	= 11 ;
FH2$V_SPOOL	= 12 ;
FH2$V_DIRECTORY	= 13 ;
FH2$V_BADBLOCK	= 14 ;
FH2$V_MARKDEL	= 15 ;
FH2$V_NOCHARGE	= 16 ;
FH2$V_ERASE	= 17 ;
FH2$W_RECPROT	= 56 ;
FH2$B_MAP_INUSE	= 58 ;
FH2$B_ACC_MODE	= 59 ;
FH2$L_FILEOWNER	= 60 ;
FH2$W_UICMEMBER	= 60 ;
FH2$W_UICGROUP	= 62 ;
FH2$W_FILEPROT	= 64 ;
FH2$S_BACKLINK	= 6 ;
FH2$W_BACKLINK	= 66 ;
FH2$W_BK_FIDNUM	= 66 ;
FH2$W_BK_FIDSEQ	= 68 ;
FH2$W_BK_FIDRVN	= 70 ;
FH2$B_BK_FIDRVN	= 70 ;
FH2$B_BK_FIDNMX	= 71 ;
FH2$B_JOURNAL	= 72 ;
FH2$V_ONLY_RU	= 0 ;
FH2$V_RUJNL	= 1 ;
FH2$V_BIJNL	= 2 ;
FH2$V_AIJNL	= 3 ;
FH2$V_ATJNL	= 4 ;
FH2$V_NEVER_RU	= 5 ;
FH2$V_JOURNAL_FILE	= 6 ;
FH2$B_RU_ACTIVE	= 73 ;
FH2$L_HIGHWATER	= 76 ;
FH2$S_CLASS_PROT	= 20 ;
FH2$R_CLASS_PROT	= 88 ;
FH2$W_CHECKSUM	= 510 ;

FM1$K_POINTERS	= 10 ;
FM1$C_POINTERS	= 10 ;
FM1$K_LENGTH	= 10 ;
FM1$C_LENGTH	= 10 ;
FM1$S_FM1DEF	= 10 ;
FM1$B_EX_SEGNUM	= 0 ;
FM1$B_EX_RVN	= 1 ;
FM1$W_EX_FILNUM	= 2 ;
FM1$W_EX_FILSEQ	= 4 ;
FM1$B_COUNTSIZE	= 6 ;
FM1$B_LBNSIZE	= 7 ;
FM1$B_INUSE	= 8 ;
FM1$B_AVAIL	= 9 ;
FM1$S_FM1DEF1	= 4 ;
FM1$B_HIGHLBN	= 0 ;
FM1$B_COUNT	= 1 ;
FM1$W_LOWLBN	= 2 ;
FM1$S_FM1DEF2	= 5 ;
FM1$B_PREVHLBN	= -4 ;
FM1$B_PREVCOUNT	= -3 ;
FM1$W_PREVLLBN	= -2 ;

FM2$C_PLACEMENT	= 0 ;
FM2$C_FORMAT1	= 1 ;
FM2$C_FORMAT2	= 2 ;
FM2$C_FORMAT3	= 3 ;
FM2$K_LENGTH0	= 2 ;
FM2$C_LENGTH0	= 2 ;
FM2$K_LENGTH1	= 4 ;
FM2$C_LENGTH1	= 4 ;
FM2$S_FM2DEF	= 4 ;
FM2$W_WORD0	= 0 ;
FM2$S_FORMAT	= 2 ;
FM2$V_FORMAT	= 14 ;
FM2$V_EXACT	= 0 ;
FM2$V_ONCYL	= 1 ;
FM2$V_LBN	= 12 ;
FM2$V_RVN	= 13 ;
FM2$S_HIGHLBN	= 6 ;
FM2$V_HIGHLBN	= 8 ;
FM2$S_COUNT2	= 14 ;
FM2$V_COUNT2	= 0 ;
FM2$B_COUNT1	= 0 ;
FM2$W_LOWLBN	= 2 ;
FM2$K_LENGTH2	= 6 ;
FM2$C_LENGTH2	= 6 ;
FM2$S_FM2DEF1	= 6 ;
FM2$L_LBN2	= 2 ;
FM2$K_LENGTH3	= 8 ;
FM2$C_LENGTH3	= 8 ;
FM2$S_FM2DEF2	= 8 ;
FM2$W_LOWCOUNT	= 2 ;
FM2$L_LBN3	= 4 ;

