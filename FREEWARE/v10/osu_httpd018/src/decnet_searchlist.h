/*
 * Simple Decnet access routines for DECthreads.
 */
int dnetx_initialize ( char *default_taskname );
int dnetx_define_task ( char *logical_task );
int dnetx_parse_task 
	( char *logical_task, int *task_len, char ***tasklist, int *rr_point );
