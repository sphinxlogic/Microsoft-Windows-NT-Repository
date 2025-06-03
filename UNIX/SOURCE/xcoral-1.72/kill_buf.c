/*
** Copyright 1989, 1992 by Lionel Fournigault
**
** Permission to use, copy, and distribute for non-commercial purposes,
** is hereby granted without fee, providing that the above copyright
** notice appear in all copies and that both the copyright notice and this
** permission notice appear in supporting documentation.
** The software may be modified for your own purposes, but modified versions
** may not be distributed.
** This software is provided "as is" without any expressed or implied warranty.
**
**
*/

#include <stdio.h>
#ifndef apollo
#include <malloc.h>
#endif
#include <string.h>
#include <X11/Xlib.h>

#include "text.h"
#include "flist.h"

#define KB_SIZE_BUF 256
#define KB_SIZE 20

typedef struct {
	char *p;		/* Le buffer */
	unsigned int size;	/* La taille du buffer */
	unsigned int s_len;	/* Longueur de s dans le buffer */
	unsigned int s_lines;	/* Nb de lignes */
} kb;

static kb killbuf [KB_SIZE];
static void ShiftBuffers ();


/*
**	Function name : InitKillBuf
**
**	Description : Creation de la pile des buffers pour ranger ce
**		qui a ete efface.
**	Input : 
**	Ouput :
*/
void InitKillBuf ()
{
	register int i;

	for ( i=0; i < KB_SIZE; i++ ) {
		killbuf [i].p = (char *) malloc ( KB_SIZE_BUF );
		killbuf [i].size = KB_SIZE_BUF;
		killbuf [i].s_len = 0;
		killbuf [i].s_lines = 0;
       }
}


/*
**	Function name : StoreInKillBuf
**
**	Description : Met la chaine s de longeur n et
**		contenant n lignes dans la pile.
**
**	Input : La chaine, sa longueur, le nb de lignes.
**	Ouput :
*/
void StoreInKillBuf ( s, len, n )
	register char *s;
	register int len;
	register int n;
{
	/*
	 * Pas les lignes vides
	 */
	if ( (len == 1) && (*s == '\n' ))
		return;

	/*
	 * Si ya deja quelque chose, on decale.
	 */
	if ( killbuf [0].s_len != 0 ) 
		ShiftBuffers ();

	/*
	 * Si ya pas assez de place, on change
	 * le pointeur courant.
	 */
	if ( len > killbuf [0].size ) { 
      		if ( killbuf [0].p != 0 )
			(void) free ( killbuf [0].p );
		killbuf [0].p = (char *) malloc ( (unsigned) len + 1 );
		killbuf [0].size = len + 1;
	}

	/*
	 * Copie et mis a jour des infos.
	 */       
	(void) strncpy ( killbuf [0].p, s, len ); 
	killbuf [0].s_len = len;
	killbuf [0].s_lines = n;

#ifdef DEBUG
       (void) fprintf ( stderr, "Store len = %d size = %d lines = %d\n", 
	       killbuf [0].s_len, killbuf [0].size, killbuf [0].s_lines );
#endif
}


/*
**	Function name : ShiftBuffers
**
**	Description :  On decale les buffers pour liberer le premier.
**		Le buffer no 20 est perdu.
**	Input : 
**	Ouput :
*/
static void ShiftBuffers ()
{
      register int i;

      for ( i=KB_SIZE-1; i > 0; i-- ) { /* On commence par le dernier */

	     if ( killbuf [i-1].size > killbuf [i].size ) {
         		if ( killbuf [i].p != 0 )
			free ( killbuf [i].p );
		killbuf [i].p = (char *) malloc ( killbuf [i-1].size );
	     }

             (void) strncpy ( killbuf [i].p, killbuf [i-1].p, (int) killbuf [i-1].s_len );

	     killbuf [i].size = killbuf [i-1].size;
	     killbuf [i].s_len = killbuf [i-1].s_len;
	     killbuf [i].s_lines = killbuf [i-1].s_lines;

#ifdef DEBUG
	     fprintf ( stderr, "Store i = %d p = %d len = %d\n",
		      i, killbuf [i].p, killbuf [i].s_len );
#endif
      }
}


/*
**	Function name : RestoreKillBuff
**
**	Description : Restore le la chaine contenu dans
**		le buffer i.
**
**	Input : Le numero dans la pile, longueur, et nb lignes.
**	Ouput : La chaine.
*/
char *RestoreKillBuf ( i, len, dn )
register int i;
register int *len; /* Return */
register int *dn;  /* Return */
{
       if ( (i < 0) || (i > (KB_SIZE-1)) || (killbuf [i].s_len == 0))
              return 0;

       *len = killbuf [i].s_len;
       *dn =  killbuf [i].s_lines;
       return ( (char *) killbuf [i].p );
}


/*
**	Function name : LoadKillBuffer
**
**	Description : Charge le contenu de la pile dans
**		un buffer. ( uniquement les debuts de lignes ).
**
**	Input : Le buffer
**	Ouput :
*/
void LoadKillBuffer ( buf )
Buf *buf;
{
	register int i, len;
	char tmp[8];
   	register char *p;

#define MAX_LEN	30

   	for ( i=0; i < KB_SIZE; i++ ) {
      		bzero ( tmp, 8 );
      		(void) sprintf ( tmp, "%d  ", i + 1 );
		InsertNchar ( buf, tmp, strlen(tmp) );
		if ( killbuf [i].s_lines < 2 ) {
			if ( (len = killbuf [i].s_len) > MAX_LEN )
				len = MAX_LEN;
        		}
      		else {
      			p = killbuf [i].p;
      			while ( *p != '\n' ) p++;
      			len = p - killbuf [i].p;
			if ( len > MAX_LEN )
				len = MAX_LEN;
		} 
		if ( len != 0 )
			InsertNchar ( buf, killbuf [i].p, len );

		InsertNchar ( buf, " ...\n", 5 ); 
	}
}


