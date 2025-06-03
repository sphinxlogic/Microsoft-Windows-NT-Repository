/* Interface between modules of the slipd program */
/* @(#)slipd.h	1.1 (30 Sep 1993) */

extern int slipd_dialout(char *zdialparams, 
			 char *zifn,
			 int *pifd,
			 void **ppstate);

extern void slipd_undial(void *pstate);
