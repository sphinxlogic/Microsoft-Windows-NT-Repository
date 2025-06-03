static char *RcsID = "$Id: rotate.c,v 1.1 1993/02/13 18:40:43 rfs Exp $";
/*
 * $Log: rotate.c,v $
 * Revision 1.1  1993/02/13  18:40:43  rfs
 * Initial revision
 *
 *
*/

/**********************************************************************/
/*   Function  to  rotate a block of memory. Can be used for example  */
/*   with an array of pointers.					      */
/**********************************************************************/
# include	<malloc.h>
# include	<memory.h>

# if defined(MSDOS) || defined(__MSDOS__)
# 	define	TMP_SIZE	1024
# else
# 	define	TMP_SIZE	8192
# endif

static void	my_memcpy();

/**********************************************************************/
/*   If amount > 0 then copy from start=>start+amount.		      */
/*   If amount < 0 then copy from start+amount=>start.		      */
/**********************************************************************/
void
rotate_mem(ptr, ptr_end, amount)
char	*ptr;
char	*ptr_end;
int	amount;
{	char	tmpbuf[TMP_SIZE];
	char	*tmp;
	int	abs_amount = amount < 0 ? -amount : amount;
	int	size = ptr_end - ptr;

	if (amount == 0 || size == 0)
		return;

	if (abs_amount <= TMP_SIZE)
		tmp = tmpbuf;
	else
		tmp = malloc(abs_amount);
	if (amount < 0) {
		memcpy(tmp, ptr, abs_amount);
		memcpy(ptr, ptr + abs_amount, size - abs_amount);
		memcpy(ptr + size - abs_amount, tmp, abs_amount);
		}
	else {
		memcpy(tmp, ptr + size - abs_amount, abs_amount);
		my_memcpy(ptr + abs_amount, ptr, size - abs_amount);
		memcpy(ptr, tmp, abs_amount);
		}
	if (tmp != tmpbuf)
		free(tmp);
}
static void
my_memcpy(p1, p2, len)
char	*p1;
char	*p2;
int	len;
{
	p1 += len;
	p2 += len;
	if (((int) p1 & 3) == 0 && ((int) p2 & 3) == 0 && (len & 3) == 0
	   && sizeof(long) == 4) {
		while (len >= sizeof(long)) {
			p1 -= sizeof(long);
			p2 -= sizeof(long);
			*(long *) p1 = *(long *) p2;
			len -= sizeof(long);
			}
		}
	while (len-- > 0)
		*--p1 = *--p2;
}
