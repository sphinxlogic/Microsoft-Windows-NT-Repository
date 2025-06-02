#define DEBUG
/*	hsort - general purpose heapsort 

	Author...
		Copyright (c) 1985  James R. Van Zandt

		All rights reserved.
		This program may be copied for personal, non-profit use only.

		Based on algorithms by Jon Bentley [Communications of the ACM v
		28 n 3 p 245 (Mar 85) and v 28 n 5 p 456 (May 85)], and the
		sort interface routines by Allen I.  Holub [Dr.  Dobb's Journal
		#102 (Apr 85)].
 
	Usage...

		Including a #define for DEBUG will make this file a stand-alone
		program which sorts argv and prints the result, along with the
		heap at the intermediate stages.  This is instructive if you
		want to see how the heap sort works.  #defining DEBUG2 will
		also print results of comparisons.

	Notes...
		This routine sorts N objects in worst-case time proportional to
		N*log(N).  The heapsort was discovered by J.  W.  J.  Williams
		[Communications of the ACM v 7 p 347-348 (1964)] and is
		discussed by D.  E.  Knuth [The Art of Computer Programming,
		Volume 3: Sorting and Searching, Addison-Wesley, Reading,
		Mass., 1973, section 5.2.3].

		This algorithm depends on a portion of an array having the
		"heap" property.  The array X has the property heap[L,U] if:

			for all      L, i, and U
			such that    2L <= i <= U
			we have      X[i div 2] <= X[i]
		
		sift_down enlarges the heap: It accepts an array with heap[L+1,U]
		and returns an array with heap[L,U].
*/

typedef int (*PFI)();		/* pointer to a function returning int	*/
static PFI Comp;			/* pointer to comparison routine		*/
static int Width;			/* width of an object in bytes			*/
static char *Base;			/* pointer to element [-1] of array		*/
static char *a, *b, tmp;	/* temporaries for exchanges			*/

#ifdef DEBUG
int Exchanges=0, Comparisons=0;
#endif

/*--------------------------------------------------------------------------*/
int argvcmp(s1p,s2p) char **s1p,**s2p;
{
	/*	Comparison routine for sorting an argv like list of pointers to
		strings.  Just remove one level of indirection and call strcmp
		to do the comparison												*/

#ifdef DEBUG
	Comparisons++;
#endif
#ifdef DEBUG2
	register int rval;
	rval=strcmp(*s1p,*s2p);
	printf("   argvcmp(<%s><%s>) = %d\n",*s1p,*s2p,rval);
	return (rval);
#else
	return (strcmp(*s1p,*s2p));
#endif
}

hsort(base,nel,width,compare)
char *base;
int	nel,width;
int	(*compare)();
{
static int i,j,n,stop;
	/*	Perform a heap sort on an array starting at base.  The array is
		nel elements large and width is the size of a single element in
		bytes.  Compare is a pointer to a comparison routine which will
		be passed pointers to two elements of the array.  It should
		return a negative number if the left-most argument is less than
		the rightmost, 0 if the two arguments are equal, a positive
		number if the left argument is greater than the right.  (That
		is, it acts like a "subtract" operator.) If compare is 0 then
		the default comparison routine, argvcmp (which sorts an
		argv-like array of pointers to strings), is used.					*/

#ifdef DEBUG
	static int ii;
	printf("Sorting %d element array of %d byte elements at 0x%x\n",
		nel,width,base);
	printf("Comparison routine at 0x%x. Unsorted list:\n",compare);
	ptext(1,nel,base);
	for ( ii=1 ; ii<=nel ; ii++ ) printf("[%d]     ",ii);
	printf("\n");
#endif
	Width=width;
	Comp=(compare==(PFI)0) ? &argvcmp : compare ;
	n=nel*Width;
	Base=base-Width;
	for (i=(n/Width/2)*Width; i>=Width; i-=Width) sift_down(i,n);
#ifdef DEBUG
	printf("Heap constructed\n");
	for ( ii=1 ; ii<=nel ; ii++ ) printf("[%d]     ",ii);
	printf("\n");
#endif
	stop=Width+Width;
	for (i=n; i>=stop; )
		{for (b=base, a=Base+i, j=Width ; j-- ; )
			{tmp = *b; *b++ = *a; *a++ = tmp;
			}
#ifdef DEBUG
		Exchanges++;
#endif
		sift_down(Width,i-=Width);
		}

#ifdef DEBUG
	printf("\nSort complete, list is:\n");
	ptext(1,nel,base);
	printf("%d comparisons and %d exchanges were performed \n",
	Comparisons,Exchanges);
#endif
}

/*---------------------------------------------------------------------------*/
static sift_down(L,U) int L,U;
/*	pre: heap(L+1,U)		*/
{	int c,count;

#ifdef DEBUG
	int i1;
	i1=L;
#endif
	while(1)
		{c=L+L;
		if(c>U) break;
		if( (c+Width <= U) && ((*Comp)(Base+c+Width,Base+c)>0) ) c+= Width;
		if ((*Comp)(Base+L,Base+c)>=0) break;
		for(b=Base+L,a=Base+c,count=Width; count-- ; )
			{tmp=*b; *b++ = *a; *a++ = tmp;
			}
#ifdef DEBUG
		Exchanges++;
#endif
		L=c;
		}
#ifdef DEBUG
	ptext(i1/2,U/2,Base+Width);
#endif
/*	post: heap(L,U)			*/
}

/*--------------------------------------------------------------------------*/
/*		Test routine for hsort, compiled if DEBUG is #defined				*/

#ifdef DEBUG
static ptext( start, end, argv)
int start,end;
char **argv;
{
	/*	Print out argv, one element per line	*/

	register int i;
	for (i=1; i<start; i++) {printf("        "); argv++;}
	for ( i=start ; i<=end ; i++ ) printf("%-8s",*argv++);
	printf("\n");
}

main(argc,argv) int argc; char **argv;
{	
	/*	Test routine for hsort.  Sorts argv (less the first element).	*/

	hsort(++argv,--argc,sizeof(PFI),0);
}

#endif
