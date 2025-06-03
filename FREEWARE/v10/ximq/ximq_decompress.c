/*
 * XIMQ - display NASA/PDS "IMQ" files on a X11 workstation
 *
 * Copyright © 1991, 1994 Digital Equipment Corporation
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL DIGITAL EQUIPMENT CORPORATION BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION.
 *
 * DIGITAL EQUIPMENT CORPORATION SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND DIGITAL EQUIPMENT CORPORATION HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Module:
 *
 *	XIMQ_DECOMPRESS.C
 *
 * Author:
 *
 *    Frederick G. Kleinsorge, November 1991
 *
 * Environment:
 *
 *    OpenVMS, VAX-C syntax
 *
 * Abstract:
 *
 *	This is the decompression logic.  It is pretty much the public
 *	domain logic from the NASA CD's contained in DECOMP.C.  There is
 *	no copyright information or author provided (it is implied that
 *	the author is Kris Becker from USGS in 1988).  The code has been
 *	prettied up a little... but not much.  A delete function is also
 *	provided to allow the huffman tree to be deleted.
 *
 * Modifications:
 *
 */ 

#include "ximq.h"


/*
 *  Routine:
 *
 *	huff_tree
 *
 *  Description:
 *
 *	Builds the huffman tree from the encoding data
 *
 *  Inputs:
 *
 *	Encoding Histogram array address
 *
 *  Outputs:
 *
 *	Huffman Tree
 *
 */
NODE
*huff_tree(int *hist)

  {
  /*  Local variables used */
    long int freq_list[512];      /* Histogram frequency list */
    NODE **node_list;             /* DN pointer array list */

    register long int *fp;        /* Frequency list pointer */
    register NODE **np;           /* Node list pointer */

    register long int num_freq;   /* Number non-zero frequencies in histogram */
    long int sum;                 /* Sum of all frequencies */

    register short int num_nodes; /* Counter for DN initialization */
    register short int cnt;       /* Miscellaneous counter */

    short int znull = -1;         /* Null node value */

    register NODE *temp;          /* Temporary node pointer */

  /* Functions called */
    void sort_freq();
    NODE *new_node();

/***************************************************************************
  Allocate the array of nodes from memory and initialize these with numbers
  corresponding with the frequency list.  There are only 511 possible
  permutations of first difference histograms.  There are 512 allocated
  here to adhere to the FORTRAN version.
****************************************************************************/

   fp = freq_list;
   node_list = (NODE **) malloc(sizeof(temp)*512);
   if (node_list == NULL)
    {
      printf("\nOut of memory in huff_tree!\n");
      exit(1);
    }
   np = node_list;

   for (num_nodes=1, cnt=512 ; cnt-- ; num_nodes++)
     {
/**************************************************************************
    The following code has been added to standardize the VAX byte order
    for the "long int" type.  This code is intended to make the routine
    as machine independant as possible.
***************************************************************************/
        unsigned char *cp = (unsigned char *) hist++;
        unsigned long int j;
        short int i;
        for (i=4 ; --i >= 0 ; j = (j << 8) | *(cp+i));

/* Now make the assignment */
        *fp++ = j;
        temp = new_node(num_nodes);
        *np++ = temp;
     }

     (*--fp) = 0;         /* Ensure the last element is zeroed out.  */

/***************************************************************************
  Now, sort the frequency list and eliminate all frequencies of zero.
****************************************************************************/

  num_freq = 512;
  sort_freq(freq_list,node_list,num_freq);

  fp = freq_list;
  np = node_list;

  for (num_freq=512 ; (*fp) == 0 && (num_freq) ; fp++, np++, num_freq--);


/***************************************************************************
  Now create the tree.  Note that if there is only one difference value,
  it is returned as the root.  On each interation, a new node is created
  and the least frequently occurring difference is assigned to the right
  pointer and the next least frequency to the left pointer.  The node
  assigned to the left pointer now becomes the combination of the two
  nodes and it's frequency is the sum of the two combining nodes.
****************************************************************************/

  for (temp=(*np) ; (num_freq--) > 1 ; )
    {
        temp = new_node(znull);
        temp->right = (*np++);
        temp->left = (*np);
        *np = temp;
        *(fp+1) = *(fp+1) + *fp;
        *fp++ = 0;
        sort_freq(fp,np,num_freq);
    }

  return temp;
 }

/*
 *  Routine:
 *
 *	delete_node
 *
 *  Description:
 *
 *	This is a recursive function that deletes a huffman tree
 *
 *  Inputs:
 *
 *	Address of a node (the root generally, but it may be a leaf)
 *
 *  Outputs:
 *
 *	The tree is deleted.
 *
 */
void
delete_node(NODE *node)

{
  if (node->right != NULL)
    {
      delete_node(node->right);
      node->right = 0;
    }

  if (node->left != NULL)
    {
      delete_node(node->left);
      node->left = 0;
    }

  free(node);

}    

/*
 *  Routine:
 *
 *	new_node
 *
 *  Description:
 *
 *	Creates a node in the huffman tree
 *
 *  Inputs:
 *
 *	value for the node
 *
 *  Outputs:
 *
 *	A new node is created and initialized
 *
 */
NODE
*new_node(int value)

  {
    NODE *temp;         /* Pointer to the memory block */

/***************************************************************************
  Allocate the memory and intialize the fields.
****************************************************************************/

  temp = (NODE *) malloc(sizeof(NODE));

  if (temp != NULL)
    {
      temp->right = NULL;
      temp->dn = value;
      temp->left = NULL;
    }
  else
    {
       printf("\nOut of memory in new_node!\n");
       exit(1);
    }

   return temp;
  }

/*
 *  Routine:
 *
 *	sort_freq
 *
 *  Description:
 *
 *	Sorts the encoding histogram frequencies
 *
 *  Inputs:
 *
 *  Outputs:
 *
 */
void
sort_freq(int *freq_list, NODE **node_list, int num_freq)

  {
    /* Local Variables */
    register long int *i;       /* primary pointer into freq_list */
    register long int *j;       /* secondary pointer into freq_list */

    register NODE **k;          /* primary pointer to node_list */
    register NODE **l;          /* secondary pointer into node_list */

    long int temp1;             /* temporary storage for freq_list */
    NODE *temp2;                /* temporary storage for node_list */

    register long int cnt;      /* count of list elements */


/************************************************************************
  Save the current element - starting with the second - in temporary
  storage.  Compare with all elements in first part of list moving
  each up one element until the element is larger.  Insert current
  element at this point in list.
*************************************************************************/

   if (num_freq <= 0) return;      /* If no elements or invalid, return */

   for (i=freq_list, k=node_list, cnt=num_freq ; --cnt ; *j=temp1, *l=temp2)
     {
        temp1 = *(++i);
        temp2 = *(++k);

        for (j = i, l = k ;  *(j-1) > temp1 ; )
          {
            *j = *(j-1);
            *l = *(l-1);
            j--;
            l--;
            if ( j <= freq_list) break;
          }

     }
  return;
  }

/*
 *  Routine:
 *
 *	dcmprs
 *
 *  Description:
 *
 *	Decompresses a scanline
 *
 *  Inputs:
 *
 *	Address of compressed scanline
 *	Address of buffer to put decompressed data
 *	Length of input line
 *	Max length for output
 *	Root of huffman tree
 *
 *  Outputs:
 *
 *	The scanline is decompressed into the buffer
 *
 */
void
dcmprs(unsigned char *ibuf, unsigned char *obuf, int *nin, int *nout, NODE *root)

  {
    /* Local Variables */
    register NODE *ptr = root;        /* pointer to position in tree */
    register unsigned char test;      /* test byte for bit set */
    register unsigned char idn;       /* input compressed byte */

    register unsigned char odn;                /* last dn value decompressed */

    unsigned char *ilim = ibuf + *nin;         /* end of compressed bytes */
    unsigned char *olim = obuf + *nout;        /* end of output buffer */

/**************************************************************************
  Check for valid input values for nin, nout and make initial assignments.
***************************************************************************/

    if (ilim > ibuf && olim > obuf)
       odn = *obuf++ = *ibuf++;
    else
       {
           printf("\nInvalid byte count in dcmprs!\n");
           exit(1);
       }

/**************************************************************************
  Decompress the input buffer.  Assign the first byte to the working
  variable, idn.  An arithmatic and (&) is performed using the variable
  'test' that is bit shifted to the right.  If the result is 0, then
  go to right else go to left.
***************************************************************************/

    for (idn=(*ibuf) ; ibuf < ilim  ; idn =(*++ibuf))
     {
        for (test=0x80 ; test ; test >>= 1)
           {
            ptr = (test & idn) ? ptr->left : ptr->right;

            if (ptr->dn != -1)
              {
                if (obuf >= olim) return;
                odn -= ptr->dn + 256;
                *obuf++ = odn;
                ptr = root;
              }
          }
     }
   return;
  }
