/*
 * Simple Decnet access routines for DECthreads.
 */
#ifdef VMS
int dnetx_initialize ( char *default_taskname );
int dnetx_define_task ( char *logical_task );
int dnetx_parse_task 
	( char *logical_task, int *task_len, char ***tasklist, int *rr_point );
#else
/*
 * Make dummies for decnet_searchlist.c routines.
 */
#define dnetx_initialize(a) 1
#define dnetx_define_task(a) 1
#define dnetx_parse_task(a,b,c,d) 0
#endif
