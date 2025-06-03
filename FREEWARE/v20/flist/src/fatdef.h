/*  Taken from VMS v5.5-1 $FATDEF macro in SYS$LIBRARY:LIB.MLB  */
/*
**	.MACRO	$FATDEF,$GBL
**	$DEFINI	FAT,$GBL
*/
#define	FAT$C_UNDEFINED	0
#define	FAT$C_FIXED	1
#define	FAT$C_VARIABLE	2
#define	FAT$C_VFC	3
#define	FAT$C_STREAM	4
#define	FAT$C_STREAMLF	5
#define	FAT$C_STREAMCR	6
#define	FAT$C_SEQUENTIAL	0
#define	FAT$C_RELATIVE	1
#define	FAT$C_INDEXED	2
#define	FAT$C_DIRECT	3
#define	FAT$M_FORTRANCC	1
#define	FAT$M_IMPLIEDCC	2
#define	FAT$M_PRINTCC	4
#define	FAT$M_NOSPAN	8
#define	FAT$K_LENGTH	32
#define	FAT$C_LENGTH	32
#define	FAT$S_FATDEF	32
#define	FAT$B_RTYPE	0
#define	FAT$S_RTYPE	4
#define	FAT$V_RTYPE	0
#define	FAT$S_FILEORG	4
#define	FAT$V_FILEORG	4
#define	FAT$B_RATTRIB	1
#define	FAT$V_FORTRANCC	0
#define	FAT$V_IMPLIEDCC	1
#define	FAT$V_PRINTCC	2
#define	FAT$V_NOSPAN	3
#define	FAT$W_RSIZE	2
#define	FAT$L_HIBLK	4
#define	FAT$W_HIBLKH	4
#define	FAT$W_HIBLKL	6
#define	FAT$L_EFBLK	8
#define	FAT$W_EFBLKH	8
#define	FAT$W_EFBLKL	10
#define	FAT$W_FFBYTE	12
#define	FAT$B_BKTSIZE	14
#define	FAT$B_VFCSIZE	15
#define	FAT$W_MAXREC	16
#define	FAT$W_DEFEXT	18
#define	FAT$W_GBC	20
#define	FAT$W_VERSIONS	30
/*
**	$DEFEND	FAT,$GBL,DEF
**	.ENDM
*/
