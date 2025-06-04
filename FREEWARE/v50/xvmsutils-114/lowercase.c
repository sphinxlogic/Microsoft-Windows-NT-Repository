/*++
	LowerCase - Convert string to lower case

	This is a fudge to fix certain MIT clients.
	(The case-sensitive name is just for appearances.)
	It is also not the most elegant or efficient way to
	do this, but it works!

	NOTE:
	Does not take account of multinational characters.
--*/

LowerCase(from, to)
char from[], to[];
{
    int i;

    for (i=0; from[i] != '\0'; i++)
    {
	if (from[i] >= 'A' && from[i] <= 'Z')
	    to[i] = from[i] | 0x20;
	else
	    to[i] = from[i];
    }
}

