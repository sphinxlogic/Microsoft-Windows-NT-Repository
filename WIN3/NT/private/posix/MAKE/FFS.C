ffs (num)
{
	long
		temp;
	register
		i;

	for ( temp = (long) num, i = 31; i > -1; i-- )
		if ( temp & 1 << i )
			return i;
}
#ifdef FFS_TEST
main (argc, argv)
char	**argv;
{
	printf ("ffs X%dX\n", ffs (0x2));
	printf ("ffs X%dX\n", ffs (0x1));
	printf ("ffs X%dX\n", ffs (0xf));
}
#endif
