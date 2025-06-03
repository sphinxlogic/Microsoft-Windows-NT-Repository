/*
 *	dysize(year): Return the number of days in the given year.
 *
 *	George Ferguson, ferguson@cs.rochester.edu, 15 Nov 1990.
 */

int
dysize(year)
int year;
{
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
	return(366);
    else
	return(365);
}
