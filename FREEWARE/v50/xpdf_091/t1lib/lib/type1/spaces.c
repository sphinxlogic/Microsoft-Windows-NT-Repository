/* $XConsortium: spaces.c,v 1.4 91/10/10 11:19:16 rws Exp $ */
/* Copyright International Business Machines, Corp. 1991
 * All Rights Reserved
 * Copyright Lexmark International, Inc. 1991
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM or Lexmark not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM AND LEXMARK PROVIDE THIS SOFTWARE "AS IS", WITHOUT ANY WARRANTIES OF
 * ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.  THE ENTIRE RISK AS TO THE
 * QUALITY AND PERFORMANCE OF THE SOFTWARE, INCLUDING ANY DUTY TO SUPPORT
 * OR MAINTAIN, BELONGS TO THE LICENSEE.  SHOULD ANY PORTION OF THE
 * SOFTWARE PROVE DEFECTIVE, THE LICENSEE (NOT IBM OR LEXMARK) ASSUMES THE
 * ENTIRE COST OF ALL SERVICING, REPAIR AND CORRECTION.  IN NO EVENT SHALL
 * IBM OR LEXMARK BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
 /* SPACES   CWEB         V0021 ********                             */
/*
:h1 id=spaces.SPACES Module - Handles Coordinate Spaces
 
This module is responsible for handling the TYPE1IMAGER "XYspace" object.
 
&author. Jeffrey B. Lotspiech (lotspiech@almaden.ibm.com)
 
 
:h3.Include Files
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "objects.h"
#include "spaces.h"
#include "paths.h"
#include "pictures.h"
#include "fonts.h"
#include "arith.h"
#include "trig.h"

static int FindFfcn();
static int FindIfcn();
/*
:h3.Entry Points Provided to the TYPE1IMAGER User
*/
 
/*SHARED LINE(S) ORIGINATED HERE*/
 
/*
:h3.Entry Points Provided to Other Modules
*/
 
/*
In addition, other modules call the SPACES module through function
vectors in the "XYspace" structure.  The entry points accessed that
way are "FConvert()", "IConvert()", and "ForceFloat()".
*/
 
/*SHARED LINE(S) ORIGINATED HERE*/
/*
:h3.Macros and Typedefs Provided to Other Modules
 
:h4.Duplicating and Killing Spaces
 
Destroying XYspaces is so simple we can do it with a
macro:
*/
 
/*SHARED LINE(S) ORIGINATED HERE*/
/*
On the other hand, duplicating XYspaces is slightly more difficult
because of the need to keep a unique ID in the space, see
:hdref refid=dupspace..
 
:h4.Fixed Point Pel Representation
 
We represent pel positions with fixed point numbers.  This does NOT
mean integer, but truly means fixed point, with a certain number
of binary digits (FRACTBITS) representing the fractional part of the
pel.
*/
 
/*SHARED LINE(S) ORIGINATED HERE*/
/*
:h2.Data Structures for Coordinate Spaces and Points
*/
/*
:h3 id=matrix.Matrices
 
TYPE1IMAGER uses 2x2 transformation matrices.  We'll use C notation for
such a matrix (M[2][2]), the first index being rows, the second columns.
*/
 
/*
:h3.The "doublematrix" Structure
 
We frequently find it desirable to store both a matrix and its
inverse.  We store these in a "doublematrix" structure.
*/
 
/*SHARED LINE(S) ORIGINATED HERE*/
 
/*
:h3.The "XYspace" Structure
 
The XYspace structure represents the XYspace object.
*/
 
/*SHARED LINE(S) ORIGINATED HERE*/
#define    RESERVED  10      /* 'n' IDs are reserved for invalid & immortal spaces */
/*
*/
#define    NEXTID    ((SpaceID < RESERVED) ? (SpaceID = RESERVED) : ++SpaceID)
 
static unsigned int SpaceID = 1;
 
struct XYspace *CopySpace(S)
       register struct XYspace *S;
{
       S = (struct XYspace *)Allocate(sizeof(struct XYspace), S, 0);
       S->ID = NEXTID;
       return(S);
}
/*
:h3.The "fractpoint" Structure
 
A fractional point is just a "fractpel" x and y:
*/
 
/*SHARED LINE(S) ORIGINATED HERE*/
 
/*
:h3.Lazy Evaluation of Matrix Inverses
 
Calculating the inverse of a matrix is somewhat involved, and we usually
do not need them.  So, we flag whether or not the space has the inverse
already calculated:
*/
 
#define    HASINVERSE(flag)   ((flag)&0x80)
 
/*
The following macro forces a space to have an inverse:
*/
 
#define    CoerceInverse(S)   if (!HASINVERSE((S)->flag)) { \
    MatrixInvert((S)->tofract.normal, (S)->tofract.inverse); (S)->flag |= HASINVERSE(ON); }
/*
:h3.IDENTITY Space
 
IDENTITY space is (logically) the space corresponding to the identity
transformation matrix.  However, since all our transformation matrices
have a common FRACTFLOAT scale factor to convert to 'fractpel's, that
is actually what we store in 'tofract' matrix of IDENTITY:
*/
 
static struct XYspace identity = { SPACETYPE, ISPERMANENT(ON) + ISIMMORTAL(ON)
                        + HASINVERSE(ON), 2, /* added 3-26-91 PNM */
                        NULL, NULL,
                        NULL, NULL, NULL, NULL,
			INVALIDID + 1, 0,
		        {{{FRACTFLOAT, 0.0}, {0.0, FRACTFLOAT}},
                        {{1.0/FRACTFLOAT, 0.0}, {0.0, 1.0/FRACTFLOAT}}},
                        {{0, 0}, {0, 0}} };
struct XYspace *IDENTITY = &identity;
 
/*
*/
#define  MAXCONTEXTS   16
 
static struct doublematrix contexts[MAXCONTEXTS];
 
#ifdef notdef

static int nextcontext = 1;
 
/*SHARED LINE(S) ORIGINATED HERE*/

#if __STDC__
#define   pointer          void *
#else
#define   pointer          char *
#endif
 
/*
:h3.FindDeviceContext() - Find the Context Given a Device
 
This routine, given a device, returns the index of the device's
transformation matrix in the context array.  If it cannot find it,
it will allocate a new array entry and fill it out.
*/
 
static int FindDeviceContext(device)
       pointer device;       /* device token                                 */
{
       DOUBLE M[2][2];       /* temporary matrix                             */
       float Xres,Yres;      /* device  resolution                           */
       int orient = -1;      /* device orientation                           */
       int rc = -1;          /* return code for QueryDeviceState             */
 
       if (rc != 0)          /* we only bother with this check once          */
               abort("Context:  QueryDeviceState didn't work", 44);
 
       M[0][0] = M[1][0] = M[0][1] = M[1][1] = 0.0;
 
       switch (orient) {
           case 0:
               M[0][0] = Xres;  M[1][1] = -Yres;
               break;
           case 1:
               M[1][0] = Yres;  M[0][1] = Xres;
               break;
           case 2:
               M[0][0] = -Xres;  M[1][1] = Yres;
               break;
           case 3:
               M[1][0] = -Yres;  M[0][1] = -Xres;
               break;
           default:
               abort("QueryDeviceState returned invalid orientation", 45);
       }
       return(FindContext(M));
}
 
/*
:h3.FindContext() - Find the Context Given a Matrix
 
This routine, given a matrix, returns the index of that matrix matrix in
the context array.  If it cannot find it, it will allocate a new array
entry and fill it out.
*/
 
int FindContext(M)
       DOUBLE M[2][2];       /* array to search for                          */
{
       register int i;       /* loop variable for search                     */
       for (i=0; i < nextcontext; i++)
               if (M[0][0] == contexts[i].normal[0][0] && M[1][0] == contexts[i].normal[1][0]
                   && M[0][1] == contexts[i].normal[0][1] && M[1][1] == contexts[i].normal[1][1])
                       break;
 
       if (i >= nextcontext) {
               if (i >= MAXCONTEXTS)
                       abort("Context:  out of them", 46);
               LONGCOPY(contexts[i].normal, M, sizeof(contexts[i].normal));
               MatrixInvert(M, contexts[i].inverse);
               nextcontext++;
       }
 
       return(i);
}
 
/*
:h3.Context() - Create a Coordinate Space for a Device
 
This user operator is implemented by first finding the device context
array index, then transforming IDENTITY space to create an appropriate
cooridnate space.
*/
 
struct XYspace *Context(device, units)
       pointer device;       /* device token                                 */
       DOUBLE units;         /* multiples of one inch                        */
{
       DOUBLE M[2][2];       /* device transformation matrix                 */
       register int n;       /* will hold device context number              */
       register struct XYspace *S;  /* XYspace constructed                   */
 
       IfTrace2((MustTraceCalls),"Context(%x, %f)\n", device, &units);
 
       ARGCHECK((device == NULL), "Context of NULLDEVICE not allowed",
                    NULL, IDENTITY, (0), struct XYspace *);
       ARGCHECK((units == 0.0), "Context: bad units", NULL, IDENTITY, (0), struct XYspace *);
 
       n = FindDeviceContext(device);
 
       LONGCOPY(M, contexts[n].normal, sizeof(M));
 
       M[0][0] *= units;
       M[0][1] *= units;
       M[1][0] *= units;
       M[1][1] *= units;
 
       S = (struct XYspace *)Xform(IDENTITY, M);
 
       S->context = n;
       return(S);
}
#endif
 
/*
:h3.ConsiderContext() - Adjust a Matrix to Take Out Device Transform
 
Remember, we have :f/x times U times D/ and :f/M/ and and we want :f/x
times U times M times D/.  An easy way to do this is to calculate
:f/D sup <-1> times M times D/, because:
:formula.
x times U times D times D sup <-1> times M times D = x times U times M times D
:formula.
So this subroutine, given an :f/M/and an object, finds the :f/D/ for that
object and modifies :f/M/ so it is :f/D sup <-1> times M times D/.
*/
 
static void ConsiderContext(obj, M)
       register struct xobject *obj;  /* object to be transformed            */
       register DOUBLE M[2][2];    /* matrix (may be changed)                */
{
       register int context=0; /* index in contexts array                      */
 
       if (obj == NULL) return;
 
       if (ISPATHTYPE(obj->type)) {
               struct segment *path = (struct segment *) obj;
 
               context = path->context;
       }
       else if (obj->type == SPACETYPE) {
               struct XYspace *S = (struct XYspace *) obj;
 
               context = S->context;
       }
       else if (obj->type == PICTURETYPE) {

       }
       else
               context = NULLCONTEXT;
 
       if (context != NULLCONTEXT) {
               MatrixMultiply(contexts[context].inverse, M, M);
               MatrixMultiply(M, contexts[context].normal, M);
       }
}
 
/*
:h2.Conversion from User's X,Y to "fractpel" X,Y
 
When the user is building paths (lines, moves, curves, etc.) he passes
the control points (x,y) for the paths together with an XYspace.  We
must convert from the user's (x,y) to our internal representation
which is in pels (fractpels, actually).  This involves transforming
the user's (x,y) under the coordinate space transformation.  It is
important that we do this quickly.  So, we store pointers to different
conversion functions right in the XYspace structure.  This allows us
to have simpler special case functions for the more commonly
encountered types of transformations.
 
:h3.Convert(), IConvert(), and ForceFloat() - Called Through "XYspace" Structure
 
These are functions that fit in the "convert" and "iconvert" function
pointers in the XYspace structure.  They call the "xconvert", "yconvert",
"ixconvert", and "iyconvert" as appropriate to actually do the work.
These secondary routines come in many flavors to handle different
special cases as quickly as possible.
*/
 
int FXYConvert(pt, S, x, y)
       register struct fractpoint *pt;  /* point to set                      */
       register struct XYspace *S;  /* relevant coordinate space             */
       register DOUBLE x,y;  /* user's coordinates of point                  */
{
       pt->x = (*S->xconvert)(S->tofract.normal[0][0], S->tofract.normal[1][0], x, y);
       pt->y = (*S->yconvert)(S->tofract.normal[0][1], S->tofract.normal[1][1], x, y);
       return(0);
       
}
 
int IXYConvert(pt, S, x, y)
       register struct fractpoint *pt;  /* point to set                      */
       register struct XYspace *S;  /* relevant coordinate space             */
       register LONG x,y;    /* user's coordinates of point                  */
{
       pt->x = (*S->ixconvert)(S->itofract[0][0], S->itofract[1][0], x, y);
       pt->y = (*S->iyconvert)(S->itofract[0][1], S->itofract[1][1], x, y);
       return(0);
       
}
 
/*
ForceFloat is a substitute for IConvert(), when we just do not have
enough significant digits in the coefficients to get high enough
precision in the answer with fixed point arithmetic.  So, we force the
integers to floats, and do the arithmetic all with floats:
*/
 
int ForceFloat(pt, S, x, y)
       register struct fractpoint *pt;  /* point to set                      */
       register struct XYspace *S;  /* relevant coordinate space             */
       register LONG x,y;    /* user's coordinates of point                  */
{
       (*S->convert)(pt, S, (DOUBLE) x, (DOUBLE) y);
       return(0);
       
}
 
/*
:h3.FXYboth(), FXonly(), FYonly() - Floating Point Conversion
 
These are the routines we use when the user has given us floating
point numbers for x and y. FXYboth() is the general purpose routine;
FXonly() and FYonly() are special cases when one of the coefficients
is 0.0.
*/
 
fractpel FXYboth(cx, cy, x, y)
       register DOUBLE cx,cy;  /* x and y coefficients                       */
       register DOUBLE x,y;  /* user x,y                                     */
{
       register DOUBLE r;    /* temporary float                              */
 
       r = x * cx + y * cy;
       return((fractpel) r);
}
 
/*ARGSUSED*/
fractpel FXonly(cx, cy, x, y)
       register DOUBLE cx,cy;  /* x and y coefficients                       */
       register DOUBLE x,y;  /* user x,y                                     */
{
       register DOUBLE r;    /* temporary float                              */
 
       r = x * cx;
       return((fractpel) r);
}
 
/*ARGSUSED*/
fractpel FYonly(cx, cy, x, y)
       register DOUBLE cx,cy;  /* x and y coefficients                       */
       register DOUBLE x,y;  /* user x,y                                     */
{
       register DOUBLE r;    /* temporary float                              */
 
       r = y * cy;
       return((fractpel) r);
}
 
/*
:h3.IXYboth(), IXonly(), IYonly() - Simple Integer Conversion
 
These are the routines we use when the user has given us integers for
x and y, and the coefficients have enough significant digits to
provide precise answers with only "long" (32 bit?) multiplication.
IXYboth() is the general purpose routine; IXonly() and IYonly() are
special cases when one of the coefficients is 0.
*/
 
fractpel IXYboth(cx, cy, x, y)
       register fractpel cx,cy;  /* x and y coefficients                     */
       register LONG x,y;    /* user x,y                                     */
{
       return(x * cx + y * cy);
}
 
/*ARGSUSED*/
fractpel IXonly(cx, cy, x, y)
       register fractpel cx,cy;  /* x and y coefficients                     */
       register LONG x,y;    /* user x,y                                     */
{
       return(x * cx);
}
 
/*ARGSUSED*/
fractpel IYonly(cx, cy, x, y)
       register fractpel cx,cy;  /* x and y coefficients                     */
       register LONG x,y;    /* user x,y                                     */
{
       return(y * cy);
}
 
 
/*
:h3.FPXYboth(), FPXonly(), FPYonly() - More Involved Integer Conversion
 
These are the routines we use when the user has given us integers for
x and y, but the coefficients do not have enough significant digits to
provide precise answers with only "long" (32 bit?)  multiplication.
We have increased the number of significant bits in the coefficients
by FRACTBITS; therefore we must use "double long" (64 bit?)
multiplication by calling FPmult().  FPXYboth() is the general purpose
routine; FPXonly() and FPYonly() are special cases when one of the
coefficients is 0.
 
Note that it is perfectly possible for us to calculate X with the
"FP" method and Y with the "I" method, or vice versa.  It all depends
on how the functions in the XYspace structure are filled out.
*/
 
fractpel FPXYboth(cx, cy, x, y)
       register fractpel cx,cy;  /* x and y coefficients                     */
       register LONG x,y;    /* user x,y                                     */
{
       return( FPmult(x, cx) + FPmult(y, cy) );
}
 
/*ARGSUSED*/
fractpel FPXonly(cx, cy, x, y)
       register fractpel cx,cy;  /* x and y coefficients                     */
       register LONG x,y;    /* user x,y                                     */
{
       return( FPmult(x, cx) );
}
 
/*ARGSUSED*/
fractpel FPYonly(cx, cy, x, y)
       register fractpel cx,cy;  /* x and y coefficients                     */
       register LONG x,y;    /* user x,y                                     */
{
       return( FPmult(y, cy) );
}
 
 
 
/*
:h3.FillOutFcns() - Determine the Appropriate Functions to Use for Conversion
 
This function fills out the "convert" and "iconvert" function pointers
in an XYspace structure, and also fills the "helper"
functions that actually do the work.
*/
 
static void FillOutFcns(S)
       register struct XYspace *S;  /* functions will be set in this structure */
{
       S->convert = FXYConvert;
       S->iconvert = IXYConvert;
 
       FindFfcn(S->tofract.normal[0][0], S->tofract.normal[1][0], &S->xconvert);
       FindFfcn(S->tofract.normal[0][1], S->tofract.normal[1][1], &S->yconvert);
       FindIfcn(S->tofract.normal[0][0], S->tofract.normal[1][0],
                &S->itofract[0][0], &S->itofract[1][0], &S->ixconvert);
       FindIfcn(S->tofract.normal[0][1], S->tofract.normal[1][1],
                &S->itofract[0][1], &S->itofract[1][1], &S->iyconvert);
 
       if (S->ixconvert == NULL || S->iyconvert == NULL)
                S->iconvert = ForceFloat;
}
 
/*
:h4.FindFfcn() - Subroutine of FillOutFcns() to Fill Out Floating Functions
 
This function tests for the special case of one of the coefficients
being zero:
*/
 
static int FindFfcn(cx, cy, fcnP)
       register DOUBLE cx,cy;  /* x and y coefficients                       */
       register fractpel (**fcnP)();  /* pointer to function to set          */
{
       if (cx == 0.0)
               *fcnP = FYonly;
       else if (cy == 0.0)
               *fcnP = FXonly;
       else
               *fcnP = FXYboth;
       return(0);
       
}
 
/*
:h4.FindIfcn() - Subroutine of FillOutFcns() to Fill Out Integer Functions
 
There are two types of integer functions, the 'I' type and the 'FP' type.
We use the I type functions when we are satisfied with simple integer
arithmetic.  We used the FP functions when we feel we need higher
precision (but still fixed point) arithmetic.  If all else fails,
we store a NULL indicating that this we should do the conversion in
floating point.
*/
 
static int FindIfcn(cx, cy, icxP, icyP, fcnP)
       register DOUBLE cx,cy;  /* x and y coefficients                       */
       register fractpel *icxP,*icyP;  /* fixed point coefficients to set    */
       register fractpel (**fcnP)();  /* pointer to function to set          */
{
       register fractpel imax;  /* maximum of cx and cy                      */
 
       *icxP = cx;
       *icyP = cy;
 
       if (cx != (float) (*icxP) || cy != (float) (*icyP)) {
/*
At this point we know our integer approximations of the coefficients
are not exact.  However, we will still use them if the maximum
coefficient will not fit in a 'fractpel'.   Of course, we have little
choice at that point, but we haven't lost that much precision by
staying with integer arithmetic.  We have enough significant digits
so that
any error we introduce is less than one part in 2:sup/16/.
*/
 
               imax = TYPE1_MAX(TYPE1_ABS(*icxP), TYPE1_ABS(*icyP));
               if (imax < (fractpel) (1<<(FRACTBITS-1)) ) {
/*
At this point we know our integer approximations just do not have
enough significant digits for accuracy.  We will add FRACTBITS
significant digits to the coefficients (by multiplying them by
1<<FRACTBITS) and go to the "FP" form of the functions.  First, we
check to see if we have ANY significant digits at all (that is, if
imax == 0).  If we don't, we suspect that adding FRACTBITS digits
won't help, so we punt the whole thing.
*/
                       if (imax == 0) {
                               *fcnP = NULL;
                               return(0);
                       }
                       cx *= FRACTFLOAT;
                       cy *= FRACTFLOAT;
                       *icxP = cx;
                       *icyP = cy;
                       *fcnP = FPXYboth;
               }
               else
                       *fcnP = IXYboth;
       }
       else
               *fcnP = IXYboth;
/*
Now we check for special cases where one coefficient is zero (after
integer conversion):
*/
       if (*icxP == 0)
               *fcnP = (*fcnP == FPXYboth) ? FPYonly : IYonly;
       else if (*icyP == 0)
               *fcnP = (*fcnP == FPXYboth) ? FPXonly : IXonly;
       return(0);
       
}
/*
:h3.UnConvert() - Find User Coordinates From FractPoints
 
The interesting thing with this routine is that we avoid calculating
the matrix inverse of the device transformation until we really need
it, which is to say, until this routine is called for the first time
with a given coordinate space.
 
We also only calculate it only once.  If the inverted matrix is valid,
we don't calculate it; if not, we do.  We never expect matrices with
zero determinants, so by convention, we mark the matrix is invalid by
marking both X terms zero.
*/
 
void UnConvert(S, pt, xp, yp)
       register struct XYspace *S;  /* relevant coordinate space             */
       register struct fractpoint *pt;  /* device coordinates                */
       DOUBLE *xp,*yp;       /* where to store resulting x,y                 */
{
       DOUBLE x,y;
 
       CoerceInverse(S);
       x = pt->x;
       y = pt->y;
       *xp = S->tofract.inverse[0][0] * x + S->tofract.inverse[1][0] * y;
       *yp = S->tofract.inverse[0][1] * x + S->tofract.inverse[1][1] * y;
}
 
/*
:h2.Transformations
*/
/*
:h3 id=xform.Xform() - Transform Object in X and Y
 
TYPE1IMAGER wants transformations of objects like paths to be identical
to transformations of spaces.  For example, if you scale a line(1,1)
by 10 it should yield the same result as generating the line(1,1) in
a coordinate space that has been scaled by 10.
 
We handle fonts by storing the accumulated transform, for example, SR
(accumulating on the right).  Then when we map the font through space TD,
for example, we multiply the accumulated font transform on the left by
the space transform on the right, yielding SRTD in this case.  We will
get the same result if we did S, then R, then T on the space and mapping
an unmodified font through that space.
*/
 
struct xobject *t1_Xform(obj, M)
       register struct xobject *obj;  /* object to transform                 */
       register DOUBLE M[2][2];    /* transformation matrix                  */
{
       if (obj == NULL)
               return(NULL);
 
       if (obj->type == FONTTYPE) {
               register struct font *F = (struct font *) obj;
 
               F = UniqueFont(F);
               return((struct xobject*)F);
       }
       if (obj->type == PICTURETYPE) {
/*
In the case of a picture, we choose both to update the picture's
transformation matrix and keep the handles up to date.
*/
               register struct picture *P = (struct picture *) obj;
               register struct segment *handles;  /* temporary path to transform handles */
 
               P = UniquePicture(P);
               handles = PathSegment(LINETYPE, P->origin.x, P->origin.y);
               handles = Join(handles,
                              PathSegment(LINETYPE, P->ending.x, P->ending.y) );
               handles = (struct segment *)Xform((struct xobject *) handles, M);
               P->origin = handles->dest;
               P->ending = handles->link->dest;
               KillPath(handles);
               return((struct xobject *)P);
       }
 
       if (ISPATHTYPE(obj->type)) {
               struct XYspace pseudo;  /* local temporary space              */
               PseudoSpace(&pseudo, M);
               return((struct xobject *) PathTransform(obj, &pseudo));
       }
 
 
       if (obj->type == SPACETYPE) {
               register struct XYspace *S = (struct XYspace *) obj;
 
/* replaced ISPERMANENT(S->flag) with S->references > 1 3-26-91 PNM */
               if (S->references > 1)
                       S = CopySpace(S);
               else
                       S->ID = NEXTID;
 
               MatrixMultiply(S->tofract.normal, M, S->tofract.normal);
               /*
               * mark inverted matrix invalid:
               */
               S->flag &= ~HASINVERSE(ON);
 
               FillOutFcns(S);
               return((struct xobject *) S);
       }
 
       return(ArgErr("Untransformable object", obj, obj));
}
 
/*
:h3.Transform() - Transform an Object
 
This is the external user's entry point.
*/
struct xobject *t1_Transform(obj, cxx, cyx, cxy, cyy)
       struct xobject *obj;
       DOUBLE cxx,cyx,cxy,cyy;  /* 2x2 transform matrix elements in row order */
{
       DOUBLE M[2][2];
 
       IfTrace1((MustTraceCalls),"Transform(%p,", obj);
       IfTrace4((MustTraceCalls)," %f %f %f %f)\n", cxx, cyx, cxy, cyy);
 
       M[0][0] = cxx;
       M[0][1] = cyx;
       M[1][0] = cxy;
       M[1][1] = cyy;
       ConsiderContext(obj, M);
       return(Xform(obj, M));
}
/*
:h3.Scale() - Special Case of Transform()
 
This is a user operator.
*/
 
struct xobject *t1_Scale(obj, sx, sy)
       struct xobject *obj;  /* object to scale                              */
       DOUBLE sx,sy;         /* scale factors in x and y                     */
{
       DOUBLE M[2][2];
       IfTrace3((MustTraceCalls),"Scale(%p, %f, %f)\n", obj, sx, sy);
       M[0][0] = sx;
       M[1][1] = sy;
       M[1][0] = M[0][1] = 0.0;
       ConsiderContext(obj, M);
       return(Xform(obj, M));
}
 
/*
:h3 id=rotate.Rotate() - Special Case of Transform()
 
We special-case different settings of 'degrees' for performance
and accuracy within the DegreeSin() and DegreeCos() routines themselves.
*/
 
#ifdef notdef
struct xobject *xiRotate(obj, degrees)
       struct xobject *obj;  /* object to be transformed                     */
       DOUBLE degrees;       /* degrees of COUNTER-clockwise rotation        */
{
       DOUBLE M[2][2];
 
 
       IfTrace2((MustTraceCalls),"Rotate(%z, %f)\n", obj, &degrees);
 
       M[0][0] = M[1][1] = DegreeCos(degrees);
       M[1][0] = - (M[0][1] = DegreeSin(degrees));
       ConsiderContext(obj, M);
       return(Xform(obj, M));
}
#endif
 
/*
:h3.PseudoSpace() - Build a Coordinate Space from a Matrix
 
Since we have built all this optimized code that, given an (x,y) and
a coordinate space, yield transformed (x,y), it seems a shame not to
use the same logic when we need to multiply an (x,y) by an arbitrary
matrix that is not (initially) part of a coordinate space.  This
subroutine takes the arbitrary matrix and builds a coordinate
space, with all its nifty function pointers.
*/
 
void PseudoSpace(S, M)
       struct XYspace *S;    /* coordinate space structure to fill out       */
       DOUBLE M[2][2];       /* matrix that will become 'tofract.normal'     */
{
       S->type = SPACETYPE;
       S->flag = ISPERMANENT(ON) + ISIMMORTAL(ON);
       S->references = 2;   /* 3-26-91 added PNM  */
       S->tofract.normal[0][0] = M[0][0];
       S->tofract.normal[1][0] = M[1][0];
       S->tofract.normal[0][1] = M[0][1];
       S->tofract.normal[1][1] = M[1][1];
 
       FillOutFcns(S);
}
 
/*
:h2 id=matrixa.Matrix Arithmetic
 
Following the convention in Newman and Sproull, :hp1/Interactive
Computer Graphics/,
matrices are organized:
:xmp.
       | cxx   cyx |
       | cxy   cyy |
:exmp.
A point is horizontal, for example:
:xmp.
       [ x y ]
:exmp.
This means that:
:formula/x prime = cxx times x + cxy times y/
:formula/y prime = cyx times x + cyy times y/
I've seen the other convention, where transform matrices are
transposed, equally often in the literature.
*/
 
/*
:h3.MatrixMultiply() - Implements Multiplication of Two Matrices
 
Implements matrix multiplication, A * B = C.
 
To remind myself, matrix multiplication goes rows of A times columns
of B.
The output matrix may be the same as one of the input matrices.
*/
void MatrixMultiply(A, B, C)
       register DOUBLE A[2][2],B[2][2];  /* input matrices                   */
       register DOUBLE C[2][2];    /* output matrix                          */
{
       register DOUBLE txx,txy,tyx,tyy;
 
       txx = A[0][0] * B[0][0] + A[0][1] * B[1][0];
       txy = A[1][0] * B[0][0] + A[1][1] * B[1][0];
       tyx = A[0][0] * B[0][1] + A[0][1] * B[1][1];
       tyy = A[1][0] * B[0][1] + A[1][1] * B[1][1];
 
       C[0][0] = txx;
       C[1][0] = txy;
       C[0][1] = tyx;
       C[1][1] = tyy;
}
/*
:h3.MatrixInvert() - Invert a Matrix
 
My reference for matrix inversion was :hp1/Elementary Linear Algebra/
by Paul C. Shields, Worth Publishers, Inc., 1968.
*/
void MatrixInvert(M, Mprime)
       DOUBLE M[2][2];       /* input matrix                                 */
       DOUBLE Mprime[2][2];    /* output inverted matrix                     */
{
       register DOUBLE D;    /* determinant of matrix M                      */
       register DOUBLE txx,txy,tyx,tyy;
 
       txx = M[0][0];
       txy = M[1][0];
       tyx = M[0][1];
       tyy = M[1][1];
 
       D = M[1][1] * M[0][0] - M[1][0] * M[0][1];
       if (D == 0.0)
               abort("MatrixInvert:  can't", 47);
 
       Mprime[0][0] = tyy / D;
       Mprime[1][0] = -txy / D;
       Mprime[0][1] = -tyx / D;
       Mprime[1][1] = txx / D;
}
/*
:h2.Initialization, Queries, and Debug
*/
/*
:h3.InitSpaces() - Initialize Constant Spaces
 
For compatibility, we initialize a coordinate space called USER which
maps 72nds of an inch to pels on the default device.
*/
 
struct XYspace *USER = &identity;
 
void InitSpaces()
{
  /* extern char *DEFAULTDEVICE; */
 
       IDENTITY->type = SPACETYPE;
       FillOutFcns(IDENTITY);
 
       contexts[NULLCONTEXT].normal[1][0]
             = contexts[NULLCONTEXT].normal[0][1]
             = contexts[NULLCONTEXT].inverse[1][0]
             = contexts[NULLCONTEXT].inverse[0][1] = 0.0;
       contexts[NULLCONTEXT].normal[0][0]
             = contexts[NULLCONTEXT].normal[1][1]
             = contexts[NULLCONTEXT].inverse[0][0]
             = contexts[NULLCONTEXT].inverse[1][1] = 1.0;
 
       USER->flag |= ISIMMORTAL(ON);
       CoerceInverse(USER);
}
/*
:h3.QuerySpace() - Returns the Transformation Matrix of a Space
 
Since the tofract matrix of an XYspace includes the scale factor
necessary to produce fractpel results (i.e., FRACTFLOAT), this
must be taken out before we return the matrix to the user.  Fortunately,
this is simple:  just multiply by the inverse of IDENTITY!
*/
 
void QuerySpace(S, cxxP, cyxP, cxyP, cyyP)
       register struct XYspace *S;  /* space asked about                     */
       register DOUBLE *cxxP,*cyxP,*cxyP,*cyyP;  /* where to put answer      */
{
       DOUBLE M[2][2];       /* temp matrix to build user's answer           */
 
       if (S->type != SPACETYPE) {
               ArgErr("QuerySpace: not a space", S, NULL);
               return;
       }
       MatrixMultiply(S->tofract.normal, IDENTITY->tofract.inverse, M);
       *cxxP = M[0][0];
       *cxyP = M[1][0];
       *cyxP = M[0][1];
       *cyyP = M[1][1];
}
 
/*
:h3.FormatFP() - Format a Fixed Point Pel
 
We format the pel as "dddd.XXXX", where XX's are hexidecimal digits,
and the dd's are decimal digits.  This might be a little confusing
mixing hexidecimal and decimal like that, but it is convenient
to use for debug.
 
We make sure we have N (FRACTBITS/4) digits past the decimal point.
*/
#define  FRACTMASK   ((1<<FRACTBITS)-1)  /* mask for fractional part         */
 
void FormatFP(string, fpel)
       register char *string;  /* output string                              */
       register fractpel fpel; /* fractional pel input                       */
{
       char temp[8];
       register char *s;
       register char *sign;
 
       if (fpel < 0) {
               sign = "-";
               fpel = -fpel;
       }
       else
               sign = "";
 
       sprintf(temp, "000%x", fpel & FRACTMASK);
       s = temp + strlen(temp) - (FRACTBITS/4);
 
       sprintf(string, "%s%d.%sx", sign, fpel >> FRACTBITS, s);
}
 
/*
:h3.DumpSpace() - Display a Coordinate Space
*/
/*ARGSUSED*/
void DumpSpace(S)
       register struct XYspace *S;
{
       IfTrace4(TRUE,"--Coordinate space at %p,ID=%d,convert=%p,iconvert=%p\n",
                   S, S->ID, S->convert, S->iconvert);
       IfTrace2(TRUE,"             |  %12.3f  %12.3f  |",
                   S->tofract.normal[0][0], S->tofract.normal[0][1]);
       IfTrace2(TRUE,"   [  %d  %d ]\n", S->itofract[0][0], S->itofract[0][1]);
       IfTrace2(TRUE,"             |  %12.3f  %12.3f  |",
                   S->tofract.normal[1][0], S->tofract.normal[1][1]);
       IfTrace2(TRUE,"   [  %d  %d ]\n", S->itofract[1][0], S->itofract[1][1]);
}
