/*#if defined(LIBC_SCCS) && !defined(lint)*/
#ifdef LIBC_SCCS
#endif
/* Does nothing, just a dummy to let the rest of the prog alone */

setkey(key)
char *key;
{
 return;
}

encrypt(block, edflag)
char *block;
{
/* do nothing */
 return;
}

char *
crypt(pw,salt)
char *pw;
char *salt;
{
	register i, j, c;
	int temp;
	static char block[66], iobuf[16];

	for(i=0; i<66; i++)
		block[i] = 0;
	
	return(iobuf);
}
