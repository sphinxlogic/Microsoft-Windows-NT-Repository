/*	FORCEX.C
 *	Force a process exit
 *
 */

main(int argc, char *argv[])
{
    long proc;
    long code=8;	/* Default code for ACCVIO */

    if (argc<2)
	return;
    sscanf (argv[1], "%x", &proc);
    if (argc>2)
	sscanf(argv[2], "%x", &code);
    sys$forcex(&proc, 0, code);
}
