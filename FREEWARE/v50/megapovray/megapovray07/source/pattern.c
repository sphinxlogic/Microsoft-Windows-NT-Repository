/**************************************************************************
*                pattern.c
*
*  -------------------------------------------------------
*  ATTENTION:
*  This is an unofficial version of pattern.c modified by
*  Ryoichi Suzuki, rsuzuki@etl.go.jp, for use  with 
*  "isosurface" shape type.
*  -------------------------------------------------------
*  This module implements texturing functions that return a value to be
*  used in a pigment or normal.
*
*  from Persistence of Vision(tm) Ray Tracer
*  Copyright 1996,1999 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file.
*  If POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by email to team-coord@povray.org or visit us on the web at
*  http://www.povray.org. The latest version of POV-Ray may be found at this site.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
* Modifications by Hans-Detlev Fink, January 1999, used with permission
* Modifications by Thomas Willhalm, March 1999, used with permission
*
*****************************************************************************/

/*
 * Modification For OpenVMS By Robert Alan Byer <byer@mail.ourservers.net>
 * Feb. 13, 2001
 */

/*
 * Some texture ideas garnered from SIGGRAPH '85 Volume 19 Number 3,
 * "An Image Synthesizer" By Ken Perlin.
 * Further Ideas Garnered from "The RenderMan Companion" (Addison Wesley).
 */

#include "frame.h"
#include "vector.h"
#include "povproto.h"
#include "matrices.h"
#include "pattern.h"
#include "povray.h"
#include "texture.h"
#include "image.h"
#include "txttest.h"
#include "colour.h"
/** poviso: Jul.14 '96 R.S. **/
#ifdef POVISO
#include "isosrf.h"
#endif
/** --- **/
#include "objects.h"
#ifdef BlobPatternPatch
#include "pigment.h"
#include "blob.h"
#endif
#ifdef ProximityPatch
#include "ray.h"
#endif
#ifdef SplineWavePatch
#include "splines.h"
#endif
/*****************************************************************************
* Local preprocessor defines
******************************************************************************/


/*****************************************************************************
* Static functions
******************************************************************************/

#ifdef CellsPatch
static DBL cells (VECTOR EPoint);
#endif
#ifdef VanSicklePatternPatch
static DBL blotches (VECTOR EPoint);
static DBL banded (VECTOR EPoint, TPATTERN *TPat);
static DBL sheet (VECTOR EPoint, TPATTERN *TPat);
#endif
static DBL agate (VECTOR EPoint, TPATTERN *TPat);
static DBL brick (VECTOR EPoint, TPATTERN *TPat);
static DBL checker (VECTOR EPoint);
#ifdef TrianglulairSquarePatch 
static DBL square (VECTOR EPoint);
static DBL ternaire (VECTOR EPoint);
#endif

#ifdef CracklePatch
static DBL crackle (VECTOR EPoint, TPATTERN *TPat);
#else
static DBL crackle (VECTOR EPoint);
#endif
static DBL gradient (VECTOR EPoint, TPATTERN *TPat);
static DBL granite (VECTOR EPoint);
static DBL leopard (VECTOR EPoint);
static DBL magnet1m (VECTOR EPoint, TPATTERN *TPat);
static DBL magnet1j (VECTOR EPoint, TPATTERN *TPat);
static DBL magnet2m (VECTOR EPoint, TPATTERN *TPat);
static DBL magnet2j (VECTOR EPoint, TPATTERN *TPat);
static DBL mandel3 (VECTOR EPoint, TPATTERN *TPat);
static DBL mandel4 (VECTOR EPoint, TPATTERN *TPat);
static DBL julia (VECTOR EPoint, TPATTERN *TPat);
static DBL julia3 (VECTOR EPoint, TPATTERN *TPat);
static DBL julia4 (VECTOR EPoint, TPATTERN *TPat);
static DBL marble (VECTOR EPoint, TPATTERN *TPat);
static DBL onion (VECTOR EPoint);
static DBL radial (VECTOR EPoint);
#ifdef PolaricalPatch
static DBL polarical (VECTOR EPoint);
#endif
static DBL spiral1 (VECTOR EPoint, TPATTERN *TPat);
static DBL spiral2 (VECTOR EPoint, TPATTERN *TPat);
static DBL wood (VECTOR EPoint, TPATTERN *TPat);
static DBL hexagon (VECTOR EPoint);
static DBL planar_pattern (VECTOR EPoint);
static DBL spherical (VECTOR EPoint);
static DBL boxed (VECTOR EPoint);
static DBL cylindrical (VECTOR EPoint);
static DBL density_file (VECTOR EPoint, TPATTERN *TPat);
#ifdef SolidPatternPatch
static DBL SolidPat(VECTOR EPoint, TPATTERN *TPat);  /*Chris Huff 7/20/99 solid pattern*/
#endif
#ifdef ClothPatternPatch
  static DBL ClothPat(VECTOR EPoint); /*Chris Huff cloth pattern*/
  static DBL Cloth2Pat(VECTOR EPoint); /*Chris Huff cloth2 pattern*/
#endif
#ifdef TorodialPatch
  static DBL ToroidalSpiral(VECTOR EPoint, TPATTERN *TPat); /*Chris Huff torodil pattern */
#endif
/*YS*/ /*moved this to pattern.h*/
/* removed static */
/*long PickInCube (VECTOR tv, VECTOR p1);*/
/*YS*/

static DBL ripples_pigm (VECTOR EPoint, TPATTERN *TPat);
static DBL waves_pigm  (VECTOR EPoint, TPATTERN *TPat);
static DBL dents_pigm  (VECTOR EPoint);
static DBL wrinkles_pigm (VECTOR EPoint);
static DBL quilted_pigm (VECTOR EPoint, TPATTERN *TPat);
static TURB *Search_For_Turb (WARP *Warps);
/* static TURB *Copy_Turb (TURB *Old);   Unused function [AED] */
static unsigned short readushort(FILE *infile);
#ifdef ProximityPatch
  static DBL proximity(VECTOR EPoint, TPATTERN *TPat);/*Chris Huff proximity pattern*/
#endif
#ifdef ObjectPatternPatch
static DBL object(VECTOR EPoint, TPATTERN *TPat);/*Chris Huff proximity pattern*/
#endif
#ifdef BlobPatternPatch
static DBL blob_pattern (VECTOR EPoint, TPATTERN *TPat);/*Chris Huff-blob pattern*/
static DBL blob_pigment(VECTOR EPoint, TPATTERN *TPat, INTERSECTION *Intersection);/*Chris Huff-blob pigment*/
#endif
/** poviso: Jul. 14, 96 R.S. **/
#ifdef POVISO
static DBL func_pat (VECTOR EPoint, TPATTERN *TPat);
#endif
/** --- **/
/* -hdf Apr 98 */
static DBL slope (VECTOR EPoint, TPATTERN *TPat, INTERSECTION *Intersection);

#ifdef PigmentPatternPatch
static DBL pigment_pattern (VECTOR EPoint, TPATTERN *TPat, INTERSECTION *Inter);
#endif

/*****************************************************************************
*
* FUNCTION
*
*   agate
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*   Oct 1994    : adapted from agate pigment by [CY]
*
******************************************************************************/

static DBL agate (VECTOR EPoint, TPATTERN *TPat)
{
  register DBL noise, turb_val;
  TURB* Turb;

  Turb=Search_For_Turb(TPat->Warps);

  turb_val = TPat->Vals.Agate_Turb_Scale * Turbulence(EPoint,Turb);

  noise = 0.5 * (cycloidal(1.3 * turb_val + 1.1 * EPoint[Z]) + 1.0);

  if (noise < 0.0)
  {
    noise = 0.0;
  }
  else
  {
    noise = min(1.0, noise);
    noise = pow(noise, 0.77);
  }

  return(noise);
}


/*****************************************************************************
*
* FUNCTION
*
*   brick
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*   
* OUTPUT
*   
* RETURNS
*
*   DBL value exactly 0.0 or 1.0
*   
* AUTHOR
*
*   Dan Farmer
*   
* DESCRIPTION
*
* CHANGES
*   Oct 1994    : adapted from pigment by [CY]
*
******************************************************************************/

static DBL brick (VECTOR EPoint, TPATTERN *TPat)
{
  int ibrickx, ibricky, ibrickz;
  DBL brickheight, brickwidth, brickdepth;
  DBL brickmortar, mortarheight, mortarwidth, mortardepth;
  DBL brickx, bricky, brickz;
  DBL x, y, z, fudgit;

  fudgit=Small_Tolerance+TPat->Vals.Brick.Mortar;

  x =  EPoint[X]+fudgit;
  y =  EPoint[Y]+fudgit;
  z =  EPoint[Z]+fudgit;

  brickwidth  = TPat->Vals.Brick.Size[X];
  brickheight = TPat->Vals.Brick.Size[Y];
  brickdepth  = TPat->Vals.Brick.Size[Z];
  brickmortar = (DBL)TPat->Vals.Brick.Mortar;

  mortarwidth  = brickmortar / brickwidth;
  mortarheight = brickmortar / brickheight;
  mortardepth  = brickmortar / brickdepth;

  /* 1) Check mortar layers in the X-Z plane (ie: top view) */

  bricky = y / brickheight;
  ibricky = (int) bricky;
  bricky -= (DBL) ibricky;

  if (bricky < 0.0)
  {
    bricky += 1.0;
  }

  if (bricky <= mortarheight)
  {
    return(0.0);
  }

  bricky = (y / brickheight) * 0.5;
  ibricky = (int) bricky;
  bricky -= (DBL) ibricky;

  if (bricky < 0.0)
  {
    bricky += 1.0;
  }


  /* 2) Check ODD mortar layers in the Y-Z plane (ends) */

  brickx = (x / brickwidth);
  ibrickx = (int) brickx;
  brickx -= (DBL) ibrickx;

  if (brickx < 0.0)
  {
    brickx += 1.0;
  }

  if ((brickx <= mortarwidth) && (bricky <= 0.5))
  {
    return(0.0);
  }

  /* 3) Check EVEN mortar layers in the Y-Z plane (ends) */

  brickx = (x / brickwidth) + 0.5;
  ibrickx = (int) brickx;
  brickx -= (DBL) ibrickx;

  if (brickx < 0.0)
  {
    brickx += 1.0;
  }

  if ((brickx <= mortarwidth) && (bricky > 0.5))
  {
    return(0.0);
  }

  /* 4) Check ODD mortar layers in the Y-X plane (facing) */

  brickz = (z / brickdepth);
  ibrickz = (int) brickz;
  brickz -= (DBL) ibrickz;

  if (brickz < 0.0)
  {
    brickz += 1.0;
  }

  if ((brickz <= mortardepth) && (bricky > 0.5))
  {
    return(0.0);
  }

  /* 5) Check EVEN mortar layers in the X-Y plane (facing) */

  brickz = (z / brickdepth) + 0.5;
  ibrickz = (int) brickz;
  brickz -= (DBL) ibrickz;

  if (brickz < 0.0)
  {
    brickz += 1.0;
  }

  if ((brickz <= mortardepth) && (bricky <= 0.5))
  {
    return(0.0);
  }

  /* If we've gotten this far, color me brick. */

  return(1.0);
}


/*****************************************************************************
*
* FUNCTION
*
*   checker
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value exactly 0.0 or 1.0
*
* AUTHOR
*
*   POV-Team
*
* DESCRIPTION
*
* CHANGES
*   Oct 1994    : adapted from pigment by [CY]
*
******************************************************************************/

static DBL checker (VECTOR EPoint)
{
  int value;

  value = (int)(floor(EPoint[X]+Small_Tolerance) +
                floor(EPoint[Y]+Small_Tolerance) +
                floor(EPoint[Z]+Small_Tolerance));

  if (value & 1)
  {
    return (1.0);
  }
  else
  {
    return (0.0);
  }
}


#ifdef TrianglulairSquarePatch 

/*****************************************************************************
*
* FUNCTION
*
*   square 
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value exactly 0.0, 1.0, 2.0 or 3.0
*
* AUTHOR
*
*   J. Grimbert
*
* DESCRIPTION
*   Paving the XZ plan with 4 'colours', in square
*
* CHANGES
*
******************************************************************************/

static DBL square (VECTOR EPoint)
{
  int valueX,valueZ;

  valueX = (int)(floor(EPoint[X])); 
  valueZ = (int)(floor(EPoint[Z])); 

  if (valueX & 1)
  {
    if (valueZ & 1)
    {
       return (2.0);
    }
    else
    {
       return (3.0);
    }
  }
  else
  {
    if (valueZ & 1)
    {
       return (1.0);
    }
    else
    {
       return (0.0);
    }
  }
}

/*****************************************************************************
*
* FUNCTION
*
*   ternaire
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value exactly 0.0, 1.0, 2.0, 3.0, 4.0 or 5.0
*
* AUTHOR
*
*   J. Grimbert
*
* DESCRIPTION
*   Paving the XZ plan with 6 'colours', in triangle around the origin
*
* CHANGES
*
******************************************************************************/

/* 1.73205080756887729352 is sqrt(3) */
/* .86602540378443864676 is sqrt(3)/2 */
#define SQR3_2 .86602540378443864676
#define SQR3    1.73205080756887729352
static DBL ternaire (VECTOR EPoint)
{
DBL answer;
DBL x,z;
DBL xs,zs;
int a,b;
DBL k,slop1,slop2;
int mask;

x=EPoint[X];
z=EPoint[Z];

 xs = x-3.0*floor(x/3.0);
 zs = z-SQR3*floor(z/SQR3);

 /* xs,zs is in { [0.0, 3.0 [, [0.0, SQR3 [ } 
 ** but there is some symetry to simplify the testing
 */

 a = (int)floor(xs);
 xs -= a;
 b = (zs <SQR3_2 ? 0: 1);
 if (b)
 {
   zs = SQR3 - zs; /* mirror */
 } 

 k = 1.0 - xs;
 if ((xs != 0.0)&&( k != 0.0 )) /* second condition should never occurs */
 {
    slop1 = zs/xs;
    slop2 = zs/k; /* just in case */
   switch( (slop1<SQR3?1:0)+(slop2<SQR3?2:0))
    { 
     case 3:
      answer = 0.0;
      break;
     case 2:
      answer = 1.0;
       break;
     case 1: 
      answer = 3.0;
      break;
    }
 }
 else
 {
   answer = 1.0;
 }
 mask = (int) answer;
 answer = (mask & 1) ? fmod(answer+2.0*a,6.0): fmod(6.0+answer-2.0*a,6.0);
 if (b)
 {
   answer = 5.0 - answer;
 }

 return answer;
}
#endif


/*****************************************************************************
*
* FUNCTION
*
*   crackle
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Jim McElhiney
*
* DESCRIPTION
*
*   "crackle":
*
*   New colour function by Jim McElhiney,
*     CompuServe 71201,1326, aka mcelhiney@acm.org
*
*   Large scale, without turbulence, makes a pretty good stone wall.
*   Small scale, without turbulence, makes a pretty good crackle ceramic glaze.
*   Highly turbulent (with moderate displacement) makes a good marble, solving
*   the problem of apparent parallel layers in Perlin's method.
*   2 octaves of full-displacement turbulence make a great "drizzled paint"
*   pattern, like a 1950's counter top.
*   Rule of thumb:  put a single colour transition near 0 in your colour map.
*
*   Mathematically, the set crackle(p)=0 is a 3D Voronoi diagram of a field of
*   semirandom points, and crackle(p)>0 is distance from set along shortest path.
*   (A Voronoi diagram is the locus of points equidistant from their 2 nearest
*   neighbours from a set of disjoint points, like the membranes in suds are
*   to the centres of the bubbles).
*
*   All "crackle" specific source code and examples are in the public domain.
*
* CHANGES
*   Oct 1994    : adapted from pigment by [CY]
*
******************************************************************************/
#ifdef CracklePatch
static long IntPickInCube(int tvx, int tvy, int tvz, VECTOR  p1);

static DBL crackle (VECTOR EPoint, TPATTERN *TPat ) {
  int    i;
  long   thisseed;
  DBL    sum, minsum, minsum2, minsum3, tf;
  VECTOR minvec;
  VECTOR tv, dv, t1;
  int addx,addy,addz;

  VECTOR flo;
  int cvc;
  static int vali=0, vals[3];
  static int valid[125];

  DBL Metric;
  DBL Offset;

  int UseSquare;
  int UseUnity;
  int flox,floy,floz;
  /*int seed,temp;*/

  Metric = TPat->Vals.Crackle.Metric[X];
  Offset = TPat->Vals.Crackle.Offset;

  UseSquare = ( Metric == 2);
  UseUnity = ( Metric == 1);

  Assign_Vector(tv,EPoint);

  /*
   * Check to see if the input point is in the same unit cube as the last
   * call to this function, to use cache of cubelets for speed.
   */

  thisseed = PickInCube(tv, t1);

  if (thisseed != TPat->Vals.Crackle.lastseed)
  {
    /*
     * No, not same unit cube.  Calculate the random points for this new
     * cube and its 80 neighbours which differ in any axis by 1 or 2.
     * Why distance of 2?  If there is 1 point in each cube, located
     * randomly, it is possible for the closest random point to be in the
     * cube 2 over, or the one two over and one up.  It is NOT possible
     * for it to be two over and two up.  Picture a 3x3x3 cube with 9 more
     * cubes glued onto each face.
     */


    flo[X] = floor(tv[X] - EPSILON);
    flo[Y] = floor(tv[Y] - EPSILON);
    flo[Z] = floor(tv[Z] - EPSILON);
    
    Assign_Vector( (TPat->Vals.Crackle.lastcenter), flo );

    /* Now store a points for this cube and each of the 80 neighbour cubes. */

    vals[0]=25*(2+(-2))+5*(2+(-1))+2+(-1);
    vals[1]=25*(2+(-2))+5*(2+(-1))+2+(0);
    vals[2]=25*(2+(-2))+5*(2+(-1))+2+(1);

    flox = (int)flo[X];
    floy = (int)flo[Y];
    floz = (int)flo[Z];

    for (addx = -2; addx <= 2; addx++)
    {
      for (addy = -2; addy <= 2; addy++)
      {
	      for (addz = -2; addz <= 2; addz++)
	      {
	        /* For each cubelet in a 5x5 cube. */
          cvc = 25*(2+addx)+5*(2+addy)+2+addz;

          if ((abs(addx)==2)+(abs(addy)==2)+(abs(addz)==2) <= 1)
	        {
	          /* Yes, it's within a 3d knight move away. */

	          /*VAdd(sv, tv, add);*/
            /*
            sv[X] = tv[X] + (DBL)addx;
            sv[Y] = tv[Y] + (DBL)addy;
            sv[Z] = tv[Z] + (DBL)addz;
	          PickInCube(sv, t1);

	          TPat->Vals.Crackle.cv[cvc][X] = t1[X];
	          TPat->Vals.Crackle.cv[cvc][Y] = t1[Y];
	          TPat->Vals.Crackle.cv[cvc][Z] = t1[Z];
            */
#define INLINE_PICK_IN_CUBE 0
#if INLINE_PICK_IN_CUBE
            /* do our own PickInCube and use as much integer math as possible */
#ifdef NoiseTranslateFixPatch
            seed = Hash3d((flox+addx)&0xFFF,(floy+addy)&0xFFF,(floz+addz)&0xFFF);
#else
            seed = Hash3d(flox+addx,floy+addy,floz+addz);
#endif
            temp = POV_GET_OLD_RAND(); /* save current seed */
            POV_SRAND(seed);
            TPat->Vals.Crackle.cv[cvc][X] = flox+addx + FRAND();
            TPat->Vals.Crackle.cv[cvc][Y] = floy+addy + FRAND();
            TPat->Vals.Crackle.cv[cvc][Z] = floz+addz + FRAND();
            POV_SRAND(temp);  /* restore */
#else
	          IntPickInCube(flox+addx,floy+addy,floz+addz, t1);

	          TPat->Vals.Crackle.cv[cvc][X] = t1[X];
	          TPat->Vals.Crackle.cv[cvc][Y] = t1[Y];
	          TPat->Vals.Crackle.cv[cvc][Z] = t1[Z];
#endif
	          valid[cvc]=1;
	        }
	        else {
            valid[cvc]=0;
	        }
	      }
      }
    }

    TPat->Vals.Crackle.lastseed = thisseed;
  }

  cvc=125;
  /*
   * Find the 2 points with the 2 shortest distances from the input point.
   * Loop invariant:  minsum is shortest dist, minsum2 is 2nd shortest
   */

  /* Set up the loop so the invariant is true:  minsum <= minsum2 */

  VSub(dv, TPat->Vals.Crackle.cv[vals[0]], tv);  
  if ( UseSquare ) {
      minsum  = VSumSqr(dv);
	  if ( Offset ) minsum += Offset*Offset;
  }
  else if ( UseUnity ) {
	  minsum = dv[X] + dv[Y] + dv[Z];
	  if ( Offset ) minsum += Offset;
  }
  else {
	  minsum = pow( fabs( dv[X] ), Metric ) +
		       pow( fabs( dv[Y] ), Metric ) +
			   pow( fabs( dv[Z] ), Metric );
	  if ( Offset ) minsum += pow( Offset, Metric );
  }
  Assign_Vector( &minvec, TPat->Vals.Crackle.cv+vals[0] );
  VSub(dv, TPat->Vals.Crackle.cv[vals[1]], tv);  
  if ( UseSquare ) {
      minsum2  = VSumSqr(dv);
	  if ( Offset ) minsum2 += Offset*Offset;
  }
  else if ( UseUnity ) {
	  minsum2 = dv[X] + dv[Y] + dv[Z];
	  if ( Offset ) minsum2 += Offset;
  }
  else {
	  minsum2 = pow( fabs( dv[X] ), Metric ) +
		       pow( fabs( dv[Y] ), Metric ) +
			   pow( fabs( dv[Z] ), Metric );
	  if ( Offset ) minsum2 += pow( Offset, Metric );
  }
  VSub(dv, TPat->Vals.Crackle.cv[vals[2]], tv);  
  if ( UseSquare ) {
      minsum3  = VSumSqr(dv);
	  if ( Offset ) minsum3 += Offset*Offset;
  }
  else if ( UseUnity ) {
	  minsum3 = dv[X] + dv[Y] + dv[Z];
	  if ( Offset ) minsum3 += Offset;
  }
  else {
	  minsum3 = pow( fabs( dv[X] ), Metric ) +
		       pow( fabs( dv[Y] ), Metric ) +
			   pow( fabs( dv[Z] ), Metric );
	  if ( Offset ) minsum3 += pow( Offset, Metric );
  }

  if (minsum2 < minsum)
  {
    tf = minsum; minsum = minsum2; minsum2 = tf;
    Assign_Vector( &minvec, TPat->Vals.Crackle.cv+vals[1] );
  }
  if (minsum3 < minsum)
  {
    tf = minsum; minsum = minsum3; minsum3 = tf;
    Assign_Vector( &minvec, TPat->Vals.Crackle.cv+vals[2] );
  }
  if ( minsum3 < minsum2 ) {
	  tf = minsum2; minsum2=minsum3; minsum3= tf;
  }

  /* Loop for the 81 cubelets to find closest and 2nd closest. */

  for (i = vals[2]+1; i < cvc; i++) if (valid[i])
  {
    VSub(dv, TPat->Vals.Crackle.cv[i], tv);

	if ( UseSquare ) {
	    sum  = VSumSqr(dv);
		if ( Offset ) sum += Offset*Offset;
	}
    else if ( UseUnity ) {
	    sum = dv[X] + dv[Y] + dv[Z];
	    if ( Offset ) sum += Offset;
    }
    else {
	    sum = pow( fabs( dv[X] ), Metric ) +
		      pow( fabs( dv[Y] ), Metric ) +
			  pow( fabs( dv[Z] ), Metric );
	    if ( Offset ) sum += pow( Offset, Metric );
    }

    if (sum < minsum)
    {
      minsum3 = minsum2;
      minsum2 = minsum;
      minsum = sum;
      Assign_Vector( &minvec, TPat->Vals.Crackle.cv+i );
    }
    else if (sum < minsum2) 
    {
	    minsum3 = minsum2;
	minsum2 = sum;
    }
	else if ( sum < minsum3 ) {
		minsum3 = sum;
	}
  }

  if ( TPat->Vals.Crackle.IsSolid ) {
	  tf = Noise( minvec );
  }
  else if (UseSquare) {
      tf = TPat->Vals.Crackle.Form[X]*sqrt(minsum) + 
		   TPat->Vals.Crackle.Form[Y]*sqrt(minsum2) +
		   TPat->Vals.Crackle.Form[Z]*sqrt(minsum3); 
  }
  else if ( UseUnity ) {
      tf = TPat->Vals.Crackle.Form[X]*minsum + 
		   TPat->Vals.Crackle.Form[Y]*minsum2 +
		   TPat->Vals.Crackle.Form[Z]*minsum3; 
  }
  else {
      tf = TPat->Vals.Crackle.Form[X]*pow(minsum, 1.0/Metric) + 
		   TPat->Vals.Crackle.Form[Y]*pow(minsum2, 1.0/Metric) + 
		   TPat->Vals.Crackle.Form[Y]*pow(minsum3, 1.0/Metric); 
  }

 
  return max(min(tf, 1.), 0.);
}

/*****************************************************************************
*
* FUNCTION
*
*   IntPickInCube(tvx,tvy,tvz, p1)
*    a version of PickInCube that takes integers for input
*
* INPUT
*
*   ?
*
* OUTPUT
*   
* RETURNS
*
*   long integer hash function used, to speed up cacheing.
*   
* AUTHOR
*
*   original PickInCube by Jim McElhiney
*   this integer one modified by Nathan Kopp
*   
* DESCRIPTION
*
*   A subroutine to go with crackle.
*
*   Pick a random point in the same unit-sized cube as tv, in a
*   predictable way, so that when called again with another point in
*   the same unit cube, p1 is picked to be the same.
*
* CHANGES
*
******************************************************************************/

static long IntPickInCube(int tvx, int tvy, int tvz, VECTOR  p1)
{
  int seed, temp;

#ifdef NoiseTranslateFixPatch
  seed = Hash3d(tvx&0xFFF,tvy&0xFFF,tvz&0xFFF);
#else
  seed = Hash3d(tvx,tvy,tvz);
#endif
  temp = POV_GET_OLD_RAND(); /* save current seed */
  POV_SRAND(seed);

  p1[X] = tvx + FRAND();
  p1[Y] = tvy + FRAND();
  p1[Z] = tvz + FRAND();

  POV_SRAND(temp);  /* restore */

  return((long)seed);
}




#else
static DBL crackle (VECTOR EPoint)
{
  int    i;
  long   thisseed;
  DBL    sum, minsum, minsum2, tf;
  VECTOR sv, tv, dv, t1, add;

  static int cvc;
  static long lastseed = 0x80000000;
  static VECTOR cv[81];

  Assign_Vector(tv,EPoint);

  /*
   * Check to see if the input point is in the same unit cube as the last
   * call to this function, to use cache of cubelets for speed.
   */

  thisseed = PickInCube(tv, t1);

  if (thisseed != lastseed)
  {
    /*
     * No, not same unit cube.  Calculate the random points for this new
     * cube and its 80 neighbours which differ in any axis by 1 or 2.
     * Why distance of 2?  If there is 1 point in each cube, located
     * randomly, it is possible for the closest random point to be in the
     * cube 2 over, or the one two over and one up.  It is NOT possible
     * for it to be two over and two up.  Picture a 3x3x3 cube with 9 more
     * cubes glued onto each face.
     */

    /* Now store a points for this cube and each of the 80 neighbour cubes. */

    cvc = 0;

    for (add[X] = -2.0; add[X] < 2.5; add[X] +=1.0)
    {
      for (add[Y] = -2.0; add[Y] < 2.5; add[Y] += 1.0)
      {
        for (add[Z] = -2.0; add[Z] < 2.5; add[Z] += 1.0)
        {
          /* For each cubelet in a 5x5 cube. */

          if ((fabs(add[X])>1.5)+(fabs(add[Y])>1.5)+(fabs(add[Z])>1.5) <= 1.0)
          {
            /* Yes, it's within a 3d knight move away. */

            VAdd(sv, tv, add);

            PickInCube(sv, t1);

            cv[cvc][X] = t1[X];
            cv[cvc][Y] = t1[Y];
            cv[cvc][Z] = t1[Z];
            cvc++;
          }
        }
      }
    }

    lastseed = thisseed;
  }

  /*
   * Find the 2 points with the 2 shortest distances from the input point.
   * Loop invariant:  minsum is shortest dist, minsum2 is 2nd shortest
   */

  /* Set up the loop so the invariant is true:  minsum <= minsum2 */

  VSub(dv, cv[0], tv);  minsum  = VSumSqr(dv);
  VSub(dv, cv[1], tv);  minsum2 = VSumSqr(dv);

  if (minsum2 < minsum)
  {
    tf = minsum; minsum = minsum2; minsum2 = tf;
  }

  /* Loop for the 81 cubelets to find closest and 2nd closest. */

  for (i = 2; i < cvc; i++)
  {
    VSub(dv, cv[i], tv);

    sum = VSumSqr(dv);

    if (sum < minsum)
    {
      minsum2 = minsum;
      minsum = sum;
    }
    else
    {
      if (sum < minsum2)
      {
        minsum2 = sum;
      }
    }
  }

  /* Crackle value is absolute value of diff in dist to closest 2 points. */

  tf = sqrt(minsum2) - sqrt(minsum);      /* minsum is known <= minsum2 */

  /*
   * Note that the theoretical range of this function is 0 to root 3.
   * In practice, it rarely exceeds 0.9, and only very rarely 1.0
   */

  return min(tf, 1.);
}
#endif

/*****************************************************************************
*
* FUNCTION
*
*   gradient
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   
* OUTPUT
*   
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   Gradient Pattern - gradient based on the fractional values of
*   x, y or z, based on whether or not the given directional vector is
*   a 1.0 or a 0.0.
*   The basic concept of this is from DBW Render, but Dave Wecker's
*   only supports simple Y axis gradients.
*
* CHANGES
*   Oct 1994    : adapted from pigment by [CY]
*
******************************************************************************/

static DBL gradient (VECTOR EPoint, TPATTERN *TPat)
{
  register int i;
  register DBL temp;
  DBL value = 0.0;

  for (i=X; i<=Z; i++)
  {
    if (TPat->Vals.Gradient[i] != 0.0)
    {
      temp = fabs(EPoint[i]);

      value += fmod(temp,1.0);
    }
  }

  /* Clamp to 1.0. */

  value = ((value > 1.0) ? fmod(value, 1.0) : value);

  return(value);
}



/*****************************************************************************
*
* FUNCTION
*
*   granite
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   
* OUTPUT
*   
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   Granite - kind of a union of the "spotted" and the "dented" textures,
*   using a 1/f fractal noise function for color values. Typically used
*   with small scaling values. Should work with colour maps for pink granite.
*
* CHANGES
*   Oct 1994    : adapted from pigment by [CY]
*
******************************************************************************/

static DBL granite (VECTOR EPoint)
{
  register int i;
  register DBL temp, noise = 0.0, freq = 1.0;
  VECTOR tv1,tv2;

  VScale(tv1,EPoint,4.0);

  for (i = 0; i < 6 ; freq *= 2.0, i++)
  {
    VScale(tv2,tv1,freq);
    temp = 0.5 - Noise (tv2);

    temp = fabs(temp);

    noise += temp / freq;
  }

  return(noise);
}



/*****************************************************************************
*
* FUNCTION
*
*   leopard
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Scott Taylor
*
* DESCRIPTION
*
* CHANGES
*   Jul 1991 : Creation.
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

static DBL leopard (VECTOR EPoint)
{
  register DBL value, temp1, temp2, temp3;

  /* This form didn't work with Zortech 386 compiler */
  /* value = Sqr((sin(x)+sin(y)+sin(z))/3); */
  /* So we break it down. */

  temp1 = sin(EPoint[X]);
  temp2 = sin(EPoint[Y]);
  temp3 = sin(EPoint[Z]);

  value = Sqr((temp1 + temp2 + temp3) / 3.0);

  return(value);
}



/*****************************************************************************
*
* FUNCTION
*
*   mandel
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   submitted by user, name lost (sorry)
*
* DESCRIPTION
* The mandel pattern computes the standard Mandelbrot fractal pattern and
* projects it onto the X-Y plane.  It uses the X and Y coordinates to compute
* the Mandelbrot set.
*
* CHANGES
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

/* NK fractal - various changes */

static DBL fractal_exterior_color(TPATTERN *TPat, int iters,
                                  DBL a, DBL b)
{
    switch(TPat->Vals.Fractal.exterior_type)
    {
      case 0:
          return  (DBL)TPat->Vals.Fractal.efactor;
      case 1:
          return (DBL)iters / (DBL)TPat->Vals.Fractal.Iterations;
      case 2:
          return a * (DBL)TPat->Vals.Fractal.efactor;
      case 3:
          return b * (DBL)TPat->Vals.Fractal.efactor;
      case 4:
          return a*a * (DBL)TPat->Vals.Fractal.efactor;
      case 5:
          return b*b * (DBL)TPat->Vals.Fractal.efactor;
      case 6:
          return sqrt(a*a+b*b) * (DBL)TPat->Vals.Fractal.efactor;
    }
    return 0;
}

static DBL fractal_interior_color(TPATTERN *TPat, int iters,
                                  DBL a, DBL b, DBL mindist2)
{
    switch(TPat->Vals.Fractal.interior_type)
    {
      case 0:
          return  (DBL)TPat->Vals.Fractal.ifactor;
      case 1:
          return sqrt(mindist2) * (DBL)TPat->Vals.Fractal.ifactor;
      case 2:
          return a * (DBL)TPat->Vals.Fractal.ifactor;
      case 3:
          return b * (DBL)TPat->Vals.Fractal.ifactor;
      case 4:
          return a*a * (DBL)TPat->Vals.Fractal.ifactor;
      case 5:
          return b*b * (DBL)TPat->Vals.Fractal.ifactor;
      case 6:
          return a*a+b*b * (DBL)TPat->Vals.Fractal.ifactor;
    }
    return 0;
}

static DBL mandel (VECTOR EPoint, TPATTERN *TPat)
{
  int it_max, col;
  DBL a, b, cf, a2, b2, x, y, dist2, mindist2;

  a = x = EPoint[X]; a2 = Sqr(a);
  b = y = EPoint[Y]; b2 = Sqr(b);
  mindist2 = a2+b2;

  it_max = TPat->Vals.Fractal.Iterations;

  for (col = 0; col < it_max; col++)
  {
    b  = 2.0 * a * b + y;
    a  = a2 - b2 + x;

    a2 = Sqr(a);
    b2 = Sqr(b);
    dist2 = a2+b2;

    if(dist2 < mindist2) mindist2 = dist2;
    if(dist2 > 4.0)
    {
        cf = fractal_exterior_color(TPat, col, a, b);
        break;
    }
  }

  if(col == it_max)
      cf = fractal_interior_color(TPat, col, a, b, mindist2);

  return(cf);
}

static DBL mandel3 (VECTOR EPoint, TPATTERN *TPat)
{
  int it_max, col;
  DBL a, b, cf, a2, b2, x, y, dist2, mindist2;

  a = x = EPoint[X]; a2 = Sqr(a);
  b = y = EPoint[Y]; b2 = Sqr(b);
  mindist2 = a2+b2;

  it_max = TPat->Vals.Fractal.Iterations;

  for (col = 0; col < it_max; col++)
  {
      b = 3.0*a2*b - b2*b + y;
      a = a2*a - 3.0*a*b2 + x;

      a2 = Sqr(a);
      b2 = Sqr(b);
      dist2 = a2+b2;

      if(dist2 < mindist2) mindist2 = dist2;
      if(dist2 > 4.0)
      {
          cf = fractal_exterior_color(TPat, col, a, b);
          break;
      }
  }

  if(col == it_max)
      cf = fractal_interior_color(TPat, col, a, b, mindist2);

  return(cf);
}


static DBL mandel4 (VECTOR EPoint, TPATTERN *TPat)
{
  int it_max, col;
  DBL a, b, cf, a2, b2, x, y, dist2, mindist2;

  a = x = EPoint[X]; a2 = Sqr(a);
  b = y = EPoint[Y]; b2 = Sqr(b);
  mindist2 = a2+b2;

  it_max = TPat->Vals.Fractal.Iterations;

  for (col = 0; col < it_max; col++)
  {
      b = 4.0 * (a2*a*b - a*b2*b) + y;
      a = a2*a2 - 6.0*a2*b2 + b2*b2 + x;

      a2 = Sqr(a);
      b2 = Sqr(b);
      dist2 = a2+b2;

      if(dist2 < mindist2) mindist2 = dist2;
      if(dist2 > 4.0)
      {
          cf = fractal_exterior_color(TPat, col, a, b);
          break;
      }
  }

  if(col == it_max)
      cf = fractal_interior_color(TPat, col, a, b, mindist2);

  return(cf);
}

static DBL julia (VECTOR EPoint, TPATTERN *TPat)
{
  int it_max, col;
  DBL a, b, cf, a2, b2, dist2, mindist2,
      cr = TPat->Vals.Fractal.Coord[U], ci = TPat->Vals.Fractal.Coord[V];

  a = EPoint[X]; a2 = Sqr(a);
  b = EPoint[Y]; b2 = Sqr(b);
  mindist2 = a2+b2;

  it_max = TPat->Vals.Fractal.Iterations;

  for (col = 0; col < it_max; col++)
  {
    b  = 2.0 * a * b + ci;
    a  = a2 - b2 + cr;

    a2 = Sqr(a);
    b2 = Sqr(b);
    dist2 = a2+b2;

    if(dist2 < mindist2) mindist2 = dist2;
    if(dist2 > 4.0)
    {
        cf = fractal_exterior_color(TPat, col, a, b);
        break;
    }
  }

  if(col == it_max)
      cf = fractal_interior_color(TPat, col, a, b, mindist2);

  return(cf);
}

static DBL julia3 (VECTOR EPoint, TPATTERN *TPat)
{
  int it_max, col;
  DBL a, b, cf, a2, b2, dist2, mindist2,
      cr = TPat->Vals.Fractal.Coord[U], ci = TPat->Vals.Fractal.Coord[V];

  a = EPoint[X]; a2 = Sqr(a);
  b = EPoint[Y]; b2 = Sqr(b);
  mindist2 = a2+b2;

  it_max = TPat->Vals.Fractal.Iterations;

  for (col = 0; col < it_max; col++)
  {
    b = 3.0*a2*b - b2*b + ci;
    a = a2*a - 3.0*a*b2 + cr;

    a2 = Sqr(a);
    b2 = Sqr(b);
    dist2 = a2+b2;

    if(dist2 < mindist2) mindist2 = dist2;
    if(dist2 > 4.0)
    {
        cf = fractal_exterior_color(TPat, col, a, b);
      break;
    }
  }

  if(col == it_max)
      cf = fractal_interior_color(TPat, col, a, b, mindist2);

  return(cf);
}

static DBL julia4 (VECTOR EPoint, TPATTERN *TPat)
{
  int it_max, col;
  DBL a, b, cf, a2, b2, dist2, mindist2,
      cr = TPat->Vals.Fractal.Coord[U], ci = TPat->Vals.Fractal.Coord[V];

  a = EPoint[X]; a2 = Sqr(a);
  b = EPoint[Y]; b2 = Sqr(b);
  mindist2 = a2+b2;

  it_max = TPat->Vals.Fractal.Iterations;

  for (col = 0; col < it_max; col++)
  {
    b = 4.0 * (a2*a*b - a*b2*b) + ci;
    a = a2*a2 - 6.0*a2*b2 + b2*b2 + cr;

    a2 = Sqr(a);
    b2 = Sqr(b);
    dist2 = a2+b2;

    if(dist2 < mindist2) mindist2 = dist2;
    if(dist2 > 4.0)
    {
        cf = fractal_exterior_color(TPat, col, a, b);
        break;
    }
  }

  if(col == it_max)
      cf = fractal_interior_color(TPat, col, a, b, mindist2);

  return(cf);
}


static DBL magnet1m (VECTOR EPoint, TPATTERN *TPat)
{
  int it_max, col;
  DBL a, b, cf, a2, b2, x, y, tmp, tmp1r, tmp1i, tmp2r, tmp2i, dist2, mindist2;

  x = EPoint[X];
  y = EPoint[Y];
  a = a2 = 0;
  b = b2 = 0;
  mindist2 = 10000;

  it_max = TPat->Vals.Fractal.Iterations;

  for (col = 0; col < it_max; col++)
  {
      tmp1r = a2-b2 + x-1;
      tmp1i = 2*a*b + y;
      tmp2r = 2*a + x-2;
      tmp2i = 2*b + y;
      tmp = tmp2r*tmp2r + tmp2i*tmp2i;
      a = (tmp1r*tmp2r + tmp1i*tmp2i) / tmp;
      b = (tmp1i*tmp2r - tmp1r*tmp2i) / tmp;
      b2 = b*b;
      b = 2*a*b;
      a = a*a-b2;

      a2 = Sqr(a);
      b2 = Sqr(b);
      dist2 = a2+b2;

      if(dist2 < mindist2) mindist2 = dist2;
      tmp1r = a-1;
      if(dist2 > 10000.0 || tmp1r*tmp1r+b2 < 1/10000.0)
      {
          cf = fractal_exterior_color(TPat, col, a, b);
          break;
      }
  }

  if(col == it_max)
      cf = fractal_interior_color(TPat, col, a, b, mindist2);

  return(cf);
}


static DBL magnet1j (VECTOR EPoint, TPATTERN *TPat)
{
  int it_max, col;
  DBL a, b, cf, a2, b2, tmp, tmp1r, tmp1i, tmp2r, tmp2i, dist2, mindist2,
      cr = TPat->Vals.Fractal.Coord[U], ci = TPat->Vals.Fractal.Coord[V];

  a = EPoint[X]; a2 = Sqr(a);
  b = EPoint[Y]; b2 = Sqr(b);
  mindist2 = a2+b2;

  it_max = TPat->Vals.Fractal.Iterations;

  for (col = 0; col < it_max; col++)
  {
      tmp1r = a2-b2 + cr-1;
      tmp1i = 2*a*b + ci;
      tmp2r = 2*a + cr-2;
      tmp2i = 2*b + ci;
      tmp = tmp2r*tmp2r + tmp2i*tmp2i;
      a = (tmp1r*tmp2r + tmp1i*tmp2i) / tmp;
      b = (tmp1i*tmp2r - tmp1r*tmp2i) / tmp;
      b2 = b*b;
      b = 2*a*b;
      a = a*a-b2;

      a2 = Sqr(a);
      b2 = Sqr(b);
      dist2 = a2+b2;

      if(dist2 < mindist2) mindist2 = dist2;
      tmp1r = a-1;
      if(dist2 > 10000.0 || tmp1r*tmp1r+b2 < 1/10000.0)
      {
          cf = fractal_exterior_color(TPat, col, a, b);
          break;
      }
  }

  if(col == it_max)
      cf = fractal_interior_color(TPat, col, a, b, mindist2);

  return(cf);
}


static DBL magnet2m (VECTOR EPoint, TPATTERN *TPat)
{
  int it_max, col;
  DBL a, b, cf, a2, b2, x, y, tmp, tmp1r, tmp1i, tmp2r, tmp2i,
      c1r, c2r, c1c2r, c1c2i, dist2, mindist2;

  x = EPoint[X];
  y = EPoint[Y];
  a = a2 = 0;
  b = b2 = 0;
  mindist2 = 10000;

  c1r = x-1; c2r = x-2;
  c1c2r = c1r*c2r-y*y;
  c1c2i = (c1r+c2r)*y;

  it_max = TPat->Vals.Fractal.Iterations;

  for (col = 0; col < it_max; col++)
  {
      tmp1r = a2*a-3*a*b2 + 3*(a*c1r-b*y) + c1c2r;
      tmp1i = 3*a2*b-b2*b + 3*(a*y+b*c1r) + c1c2i;
      tmp2r = 3*(a2-b2) + 3*(a*c2r-b*y) + c1c2r + 1;
      tmp2i = 6*a*b + 3*(a*y+b*c2r) + c1c2i;
      tmp = tmp2r*tmp2r + tmp2i*tmp2i;
      a = (tmp1r*tmp2r + tmp1i*tmp2i) / tmp;
      b = (tmp1i*tmp2r - tmp1r*tmp2i) / tmp;
      b2 = b*b;
      b = 2*a*b;
      a = a*a-b2;

      a2 = Sqr(a);
      b2 = Sqr(b);
      dist2 = a2+b2;

      if(dist2 < mindist2) mindist2 = dist2;
      tmp1r = a-1;
      if(dist2 > 10000.0 || tmp1r*tmp1r+b2 < 1/10000.0)
      {
          cf = fractal_exterior_color(TPat, col, a, b);
          break;
      }
  }

  if(col == it_max)
      cf = fractal_interior_color(TPat, col, a, b, mindist2);

  return(cf);
}


static DBL magnet2j (VECTOR EPoint, TPATTERN *TPat)
{
  int it_max, col;
  DBL a, b, cf, a2, b2, tmp, tmp1r, tmp1i, tmp2r, tmp2i, c1r,c2r,c1c2r,c1c2i,
      cr = TPat->Vals.Fractal.Coord[U], ci = TPat->Vals.Fractal.Coord[V],
      dist2, mindist2;

  a = EPoint[X]; a2 = Sqr(a);
  b = EPoint[Y]; b2 = Sqr(b);
  mindist2 = a2+b2;

  c1r = cr-1, c2r = cr-2;
  c1c2r = c1r*c2r-ci*ci;
  c1c2i = (c1r+c2r)*ci;

  it_max = TPat->Vals.Fractal.Iterations;

  for (col = 0; col < it_max; col++)
  {
      tmp1r = a2*a-3*a*b2 + 3*(a*c1r-b*ci) + c1c2r;
      tmp1i = 3*a2*b-b2*b + 3*(a*ci+b*c1r) + c1c2i;
      tmp2r = 3*(a2-b2) + 3*(a*c2r-b*ci) + c1c2r + 1;
      tmp2i = 6*a*b + 3*(a*ci+b*c2r) + c1c2i;
      tmp = tmp2r*tmp2r + tmp2i*tmp2i;
      a = (tmp1r*tmp2r + tmp1i*tmp2i) / tmp;
      b = (tmp1i*tmp2r - tmp1r*tmp2i) / tmp;
      b2 = b*b;
      b = 2*a*b;
      a = a*a-b2;

      a2 = Sqr(a);
      b2 = Sqr(b);
      dist2 = a2+b2;

      if(dist2 < mindist2) mindist2 = dist2;
      tmp1r = a-1;
      if(dist2 > 10000.0 || tmp1r*tmp1r+b2 < 1/10000.0)
      {
          cf = fractal_exterior_color(TPat, col, a, b);
          break;
      }
  }

  if(col == it_max)
      cf = fractal_interior_color(TPat, col, a, b, mindist2);

  return(cf);
}

/* NK ---- */

/*****************************************************************************
*
* FUNCTION
*
*   marble
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

static DBL marble (VECTOR EPoint, TPATTERN *TPat)
{
  register DBL turb_val;
  TURB *Turb;

  if ((Turb=Search_For_Turb(TPat->Warps)) != NULL)
  {
    turb_val = Turb->Turbulence[X] * Turbulence(EPoint,Turb);
  }
  else
  {
    turb_val = 0.0;
  }

  return(EPoint[X] + turb_val);
}



/*****************************************************************************
*
* FUNCTION
*
*   onion
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Scott Taylor
*
* DESCRIPTION
*
* CHANGES
*   Jul 1991 : Creation.
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

static DBL onion (VECTOR EPoint)
{
  /* The variable noise is not used as noise in this function */

  register DBL noise;

/*
   This ramp goes 0-1,1-0,0-1,1-0...

   noise = (fmod(sqrt(Sqr(x)+Sqr(y)+Sqr(z)),2.0)-1.0);

   if (noise<0.0) {noise = 0.0-noise;}
*/

  /* This ramp goes 0-1, 0-1, 0-1, 0-1 ... */

  noise = (fmod(sqrt(Sqr(EPoint[X])+Sqr(EPoint[Y])+Sqr(EPoint[Z])), 1.0));

  return(noise);
}



/*****************************************************************************
*
* FUNCTION
*
*   radial
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Chris Young -- new in vers 2.0
*
* DESCRIPTION
*
* CHANGES
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

static DBL radial (VECTOR EPoint)
{
  register DBL value;

  if ((fabs(EPoint[X])<0.001) && (fabs(EPoint[Z])<0.001))
  {
    value = 0.25;
  }
  else
  {
    value = 0.25 + (atan2(EPoint[X],EPoint[Z]) + M_PI) / TWO_M_PI;
  }

  return(value);
}


#ifdef PolaricalPatch
/*****************************************************************************
*
* FUNCTION
*
*   polarical
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Krzysztof Garus
*
* DESCRIPTION
*
* CHANGES
*
*
******************************************************************************/

static DBL polarical (VECTOR EPoint)
{
  register DBL value;
  DBL radius = sqrt(Sqr(EPoint[X])+Sqr(EPoint[Z]));

  if ((radius<0.001) && (fabs(EPoint[Y])<0.001))
  {
    value = 0;
  }
  else
  {
    value = atan2(radius,-EPoint[Y]) / M_PI;
    
  }

  return(value);
}
#endif
/*****************************************************************************
*
* FUNCTION
*
*   spiral1
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*   Spiral whirles around z-axis.
*   The number of "arms" is defined in the TPat.
*
* CHANGES
*   Aug 1994 : Creation.
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

static DBL spiral1(VECTOR EPoint, TPATTERN *TPat)
{
  DBL rad, phi, turb_val;
  DBL x = EPoint[X];
  DBL y = EPoint[Y];
  DBL z = EPoint[Z];
  TURB *Turb;

  if ((Turb=Search_For_Turb(TPat->Warps)) != NULL)
  {
    turb_val = Turb->Turbulence[X] * Turbulence(EPoint,Turb);
  }
  else
  {
    turb_val = 0.0;
  }

  /* Get distance from z-axis. */

  rad = sqrt(x * x + y * y);

  /* Get angle in x,y-plane (0...2 PI). */

  if (rad == 0.0)
  {
    phi = 0.0;
  }
  else
  {
    if (x < 0.0)
    {
      phi = 3.0 * M_PI_2 - asin(y / rad);
    }
    else
    {
      phi = M_PI_2 + asin(y / rad);
    }
  }

  return(z + rad + (DBL)TPat->Vals.Arms * phi / TWO_M_PI + turb_val);
}



/*****************************************************************************
*
* FUNCTION
*
*   spiral2
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*   Spiral whirles around z-axis.
*   The number of "arms" is defined in the TPat.
*
* CHANGES
*   Aug 1994 : Creation.
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/


static DBL spiral2(VECTOR EPoint, TPATTERN *TPat)
{
  DBL rad, phi, turb_val;
  DBL x = EPoint[X];
  DBL y = EPoint[Y];
  DBL z = EPoint[Z];
  TURB *Turb;

  if ((Turb=Search_For_Turb(TPat->Warps)) != NULL)
  {
    turb_val = Turb->Turbulence[X] * Turbulence(EPoint,Turb);
  }
  else
  {
    turb_val = 0.0;
  }

  /* Get distance from z-axis. */

  rad = sqrt(x * x + y * y);

  /* Get angle in x,y-plane (0...2 PI) */

  if (rad == 0.0)
  {
    phi = 0.0;
  }
  else
  {
    if (x < 0.0)
    {
      phi = 3.0 * M_PI_2 - asin(y / rad);
    }
    else
    {
      phi = M_PI_2 + asin(y / rad);
    }
  }

  turb_val = Triangle_Wave(z + rad + (DBL)TPat->Vals.Arms * phi / TWO_M_PI +
                           turb_val);

  return(Triangle_Wave(rad) + turb_val);
}



/*****************************************************************************
*
* FUNCTION
*
*   wood
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/


static DBL wood (VECTOR EPoint, TPATTERN *TPat)
{
  register DBL length;
  VECTOR WoodTurbulence;
  VECTOR point;
  DBL x=EPoint[X];
  DBL y=EPoint[Y];
  TURB *Turb;

  if ((Turb=Search_For_Turb(TPat->Warps)) != NULL)
  {
    DTurbulence (WoodTurbulence, EPoint,Turb);
    point[X] = cycloidal((x + WoodTurbulence[X]) * Turb->Turbulence[X]);
    point[Y] = cycloidal((y + WoodTurbulence[Y]) * Turb->Turbulence[Y]);
  }
  else
  {
    point[X] = 0.0;
    point[Y] = 0.0;
  }
  point[Z] = 0.0;

  point[X] += x;
  point[Y] += y;

  /* point[Z] += z; Deleted per David Buck --  BP 7/91 */

  VLength (length, point);

  return(length);
}


/*****************************************************************************
*
* FUNCTION
*
*   hexagon
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value exactly 0.0, 1.0 or 2.0
*
* AUTHOR
*
*   Ernest MacDougal Campbell III
*   
* DESCRIPTION
*
*   TriHex pattern -- Ernest MacDougal Campbell III (EMC3) 11/23/92
*
*   Creates a hexagon pattern in the XZ plane.
*
*   This algorithm is hard to explain.  First it scales the point to make
*   a few of the later calculations easier, then maps some points to be
*   closer to the Origin.  A small area in the first quadrant is subdivided
*   into a 6 x 6 grid.  The position of the point mapped into that grid
*   determines its color.  For some points, just the grid location is enough,
*   but for others, we have to calculate which half of the block it's in
*   (this is where the atan2() function comes in handy).
*
* CHANGES
*   Nov 1992 : Creation.
*   Oct 1994 : adapted from pigment by [CY]
*
******************************************************************************/

#define xfactor 0.5;         /* each triangle is split in half for the grid */
#define zfactor 0.866025404; /* sqrt(3)/2 -- Height of an equilateral triangle */

static DBL hexagon (VECTOR EPoint)
{
  int xm, zm;
  int brkindx;
  DBL xs, zs, xl, zl, value = 0.0;
  DBL x=EPoint[X];
  DBL z=EPoint[Z];


  /* Keep all numbers positive.  Also, if z is negative, map it in such a
   * way as to avoid mirroring across the x-axis.  The value 5.196152424
   * is (sqrt(3)/2) * 6 (because the grid is 6 blocks high)
   */

  x = fabs(x);

  /* Avoid mirroring across x-axis. */

  z = z < 0.0 ? 5.196152424 - fabs(z) : z;

  /* Scale point to make calcs easier. */

  xs = x/xfactor;
  zs = z/zfactor;

  /* Map points into the 6 x 6 grid where the basic formula works. */

  xs -= floor(xs/6.0) * 6.0;
  zs -= floor(zs/6.0) * 6.0;

  /* Get a block in the 6 x 6 grid. */

  xm = (int) FLOOR(xs) % 6;
  zm = (int) FLOOR(zs) % 6;

  switch (xm)
  {
    /* These are easy cases: Color depends only on xm and zm. */

    case 0:
    case 5:

      switch (zm)
      {
        case 0:
        case 5: value = 0; break;

        case 1:
        case 2: value = 1; break;

        case 3:
        case 4: value = 2; break;
      }

      break;

    case 2:
    case 3:

      switch (zm)
      {
        case 0:
        case 1: value = 2; break;

        case 2:
        case 3: value = 0; break;

        case 4:
        case 5: value = 1; break;
      }

      break;

    /* These cases are harder.  These blocks are divided diagonally
     * by the angled edges of the hexagons.  Some slope positive, and
     * others negative.  We flip the x value of the negatively sloped
     * pieces.  Then we check to see if the point in question falls
     * in the upper or lower half of the block.  That info, plus the
     * z status of the block determines the color.
     */

    case 1:
    case 4:

      /* Map the point into the block at the origin. */

      xl = xs-xm;
      zl = zs-zm;

      /* These blocks have negative slopes so we flip it horizontally. */

      if (((xm + zm) % 2) == 1)
      {
        xl = 1.0 - xl;
      }

      /* Avoid a divide-by-zero error. */

      if (xl == 0.0)
      {
        xl = 0.0001;
      }

      /* Is the angle less-than or greater-than 45 degrees? */

      brkindx = (zl / xl) < 1.0;

      /* was...
       * brkindx = (atan2(zl,xl) < (45 * M_PI_180));
       * ...but because of the mapping, it's easier and cheaper,
       * CPU-wise, to just use a good ol' slope.
       */

      switch (brkindx)
      {
        case TRUE:

          switch (zm)
          {
            case 0:
            case 3: value = 0; break;

            case 2:
            case 5: value = 1; break;

            case 1:
            case 4: value = 2; break;
          }

          break;

        case FALSE:

          switch (zm)
          {
            case 0:
            case 3: value = 2; break;

            case 2:
            case 5: value = 0; break;

            case 1:
            case 4: value = 1; break;
          }

          break;
      }
  }

  value = fmod(value, 3.0);

  return(value);
}

/* In addition to clipping the value to 
   lie between 0.0 to 1.0, it also fudges 1.0-value.
 */

#define CLIP_DENSITY(r) if((r)<0.0){(r)=1.0;}else{if((r)>1.0){(r)=0.0;}else{(r)=1.0-(r);}}

static DBL planar_pattern (VECTOR EPoint)
{
  register DBL value;

  value = fabs(EPoint[Y]);
  CLIP_DENSITY(value);

  return(value);
}

static DBL spherical (VECTOR EPoint)
{
  register DBL value;

  VLength(value, EPoint);
  CLIP_DENSITY(value);

  return(value);
}

static DBL boxed (VECTOR EPoint)
{
  register DBL value;

  value = max(fabs(EPoint[X]), max(fabs(EPoint[Y]), fabs(EPoint[Z])));
  CLIP_DENSITY(value);

  return(value);
}

static DBL cylindrical (VECTOR EPoint)
{
  register DBL value;

  value = sqrt(Sqr(EPoint[X]) + Sqr(EPoint[Z]));
  CLIP_DENSITY(value);

  return(value);
}



/*****************************************************************************
*
* FUNCTION
*
*   PickInCube(tv, p1)
*
* INPUT
*
*   ?
*
* OUTPUT
*   
* RETURNS
*
*   long integer hash function used, to speed up cacheing.
*   
* AUTHOR
*
*   Jim McElhiney
*   
* DESCRIPTION
*
*   A subroutine to go with crackle.
*
*   Pick a random point in the same unit-sized cube as tv, in a
*   predictable way, so that when called again with another point in
*   the same unit cube, p1 is picked to be the same.
*
* CHANGES
*
******************************************************************************/

#ifdef CracklePatch
long PickInCube(VECTOR tv, VECTOR  p1)
#else
static long PickInCube(VECTOR tv, VECTOR  p1)
#endif
{
  int seed, temp;
  VECTOR flo;

  /*
   * This uses floor() not FLOOR, so it will not be a mirror
   * image about zero in the range -1.0 to 1.0. The viewer
   * won't see an artefact around the origin.
   */

  flo[X] = floor(tv[X] - EPSILON);
  flo[Y] = floor(tv[Y] - EPSILON);
  flo[Z] = floor(tv[Z] - EPSILON);

#ifdef NoiseTranslateFixPatch
  seed = Hash3d((int)flo[X]&0xFFF, (int)flo[Y]&0xFFF, (int)flo[Z]&0xFFF);
#else
  seed = Hash3d((int)flo[X], (int)flo[Y], (int)flo[Z]);
#endif
  temp = POV_GET_OLD_RAND(); /* save current seed */
  
  POV_SRAND(seed);

  p1[X] = flo[X] + FRAND();
  p1[Y] = flo[Y] + FRAND();
  p1[Z] = flo[Z] + FRAND();

  POV_SRAND(temp);  /* restore */

  return((long)seed);
}



/*****************************************************************************
*
* FUNCTION
*
*   Evaluate_Pattern
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*   Intersection - intersection structure
*   
* OUTPUT
*   
* RETURNS
*
*   DBL result usual 0.0 to 1.0 but may be 2.0 in hexagon
*   
* AUTHOR
*
*   adapted from Add_Pigment by Chris Young
*   
* DESCRIPTION
*
* CHANGES
*   added parameter Intersection   -hdf- May 98
*  Removed Warp_EPoint call - moved it outside
*
******************************************************************************/

DBL Evaluate_TPat (TPATTERN *TPat, VECTOR EPoint, INTERSECTION *Intersection)
{
  DBL value = 0.0;

  /* NK 19 Nov 1999 removed Warp_EPoint call */

  switch (TPat->Type)
  {
    case AGATE_PATTERN:    value = agate    (EPoint, TPat);   break;

    case BOZO_PATTERN:
    case SPOTTED_PATTERN:
    case BUMPS_PATTERN:    value = Noise    (EPoint);         break;

    case BRICK_PATTERN:    value = brick    (EPoint, TPat);   break;
    case CHECKER_PATTERN:  value = checker  (EPoint);         break;
#ifdef TrianglulairSquarePatch 
    case SQUARE_PATTERN:   value = square   (EPoint);         break;
    case TERNAIRE_PATTERN: value = ternaire (EPoint);         break;
#endif
#ifdef CracklePatch
    case CRACKLE_PATTERN:  value = crackle  (EPoint, TPat);   break;
#else
    case CRACKLE_PATTERN:  value = crackle  (EPoint);         break;
#endif
    case GRADIENT_PATTERN: value = gradient (EPoint, TPat);   break;
    case GRANITE_PATTERN:  value = granite  (EPoint);         break;
    case HEXAGON_PATTERN:  value = hexagon  (EPoint);         break;
    case LEOPARD_PATTERN:  value = leopard  (EPoint);         break;
    case MAGNET1M_PATTERN: value = magnet1m (EPoint, TPat);   break;
    case MAGNET1J_PATTERN: value = magnet1j (EPoint, TPat);   break;
    case MAGNET2M_PATTERN: value = magnet2m (EPoint, TPat);   break;
    case MAGNET2J_PATTERN: value = magnet2j (EPoint, TPat);   break;
    case MANDEL_PATTERN:   value = mandel   (EPoint, TPat);   break;
    case MANDEL3_PATTERN:  value = mandel3  (EPoint, TPat);   break;
    case MANDEL4_PATTERN:  value = mandel4  (EPoint, TPat);   break;
    case JULIA_PATTERN:    value = julia    (EPoint, TPat);   break;
    case JULIA3_PATTERN:   value = julia3   (EPoint, TPat);   break;
    case JULIA4_PATTERN:   value = julia4   (EPoint, TPat);   break;
    case MARBLE_PATTERN:   value = marble   (EPoint, TPat);   break;
    case ONION_PATTERN:    value = onion    (EPoint);         break;
    case RADIAL_PATTERN:   value = radial   (EPoint);         break;
#ifdef PolaricalPatch
    case POLARICAL_PATTERN:value = polarical(EPoint);         break;
#endif    
	  case SPIRAL1_PATTERN:  value = spiral1  (EPoint, TPat);   break;
    case SPIRAL2_PATTERN:  value = spiral2  (EPoint, TPat);   break;
    case WOOD_PATTERN:     value = wood     (EPoint, TPat);   break;
#ifdef CellsPatch
    case CELLS_PATTERN:value = cells    (EPoint);         break;
#endif
#ifdef VanSicklePatternPatch
    case BLOTCHES_PATTERN: value = blotches    (EPoint);         break;
    case BANDS_PATTERN:value = banded    (EPoint, TPat);   break;
    case SHEET_PATTERN:value = sheet    (EPoint, TPat);   break;
#endif

    case WAVES_PATTERN:    value = waves_pigm    (EPoint, TPat);   break;
    case RIPPLES_PATTERN:  value = ripples_pigm  (EPoint, TPat);   break;
    case WRINKLES_PATTERN: value = wrinkles_pigm (EPoint);   break;
    case DENTS_PATTERN:    value = dents_pigm    (EPoint);   break;
    case QUILTED_PATTERN:  value = quilted_pigm  (EPoint, TPat);   break;

    case PLANAR_PATTERN:      value = planar_pattern (EPoint);      break;
    case BOXED_PATTERN:       value = boxed          (EPoint);      break;
    case SPHERICAL_PATTERN:   value = spherical      (EPoint);      break;
    case CYLINDRICAL_PATTERN: value = cylindrical    (EPoint);      break;
    case DENSITY_FILE_PATTERN:value = density_file (EPoint, TPat);  break;
#ifdef SolidPatternPatch
    case SOLID_PATTERN       :value = SolidPat     (EPoint, TPat);  break; /*Chris Huff solid pattern*/
#endif
#ifdef ClothPatternPatch
    case CLOTH_PATTERN       :value = ClothPat(EPoint);        break;/*Chris Huff cloth pattern*/
    case CLOTH2_PATTERN       :value = Cloth2Pat(EPoint);        break;/*Chris Huff cloth2 pattern*/
#endif
#ifdef TorodialPatch
    case TOROIDAL_SPIRAL_PATTERN :value = ToroidalSpiral(EPoint, TPat);   break;/*Chris Huff toroidalSpiral pattern*/
#endif
#ifdef BlobPatternPatch
    case BLOB_PATTERN:        value = blob_pattern (EPoint, TPat);  break;/*Chris Huff blob pattern*/
#endif
#ifdef ObjectPatternPatch
    case OBJECT_PATTERN:      value = object(EPoint, TPat);         break;/*Chris Huff object pattern*/
#endif
#ifdef ProximityPatch
    case PROXIMITY_PATTERN:   value = proximity(EPoint, TPat);      break;/*Chris Huff proximity pattern*/
#endif
/** poviso: July 96  R.S. **/
#ifdef POVISO
    case FUNCTION_PATTERN:  value = func_pat  (EPoint, TPat);   break;
#endif
/** --- **/
    /* -hdf- Apr 98 */
    case SLOPE_PATTERN:    
		#if defined (EvalPatternPatch) || defined(EvalPigmentPatch)
      if (!Intersection)
        Error("Cannot use slope pattern for eval_pigment, eval_pattern, or sky_sphere.\n");
      	#endif
      value = slope (EPoint, TPat, Intersection);   
      break;

    
    /* NK 1998 */
    case IMAGE_PATTERN:    value = image_pattern(EPoint, TPat); break;
    /* NK ---- */

#ifdef PigmentPatternPatch
    case PIGMENT_PATTERN: value = pigment_pattern(EPoint, TPat, Intersection); break;
#endif

    default: Error("Problem in Evaluate_TPat.");
  }

  if (TPat->Frequency !=0.0)
  {
    value = fmod(value * TPat->Frequency + TPat->Phase, 1.00001);
  }

  /* allow negative Frequency */

  if (value < 0.0)
  {
    value -= floor(value);
  }

  switch (TPat->Wave_Type)
  {
    case RAMP_WAVE:
      break;

    case SINE_WAVE:
      value = (1.0+cycloidal(value))*0.5;
      break;

    case TRIANGLE_WAVE:
      value = Triangle_Wave(value);
      break;

    case SCALLOP_WAVE:
      value = fabs(cycloidal(value*0.5));
      break;

    case CUBIC_WAVE:
      value = Sqr(value)*((-2.0 * value) + 3.0);
      break;

    case POLY_WAVE:
      value = pow(value, TPat->Exponent);
      break;

#ifdef AtanWavePatch
    case ATAN_WAVE:
      value = (atan(value*M_PI_2)/M_PI_2)*0.5+0.5;
      break;
#endif

#ifdef SplineWavePatch
    case SPLINE_WAVE:
    {
    	EXPRESS tmp;
        value = Get_Spline_Val(TPat->spline_wave, value, tmp);
    }
    break;
#endif
    default: Error("Unknown Wave Type %d.",TPat->Wave_Type);
   }

  return(value);
}


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Init_TPat_Fields (TPATTERN *Tpat)
{
  Tpat->Type       = NO_PATTERN;
  Tpat->Wave_Type  = RAMP_WAVE;
  Tpat->Flags      = NO_FLAGS;
  Tpat->References = 1;
  Tpat->Exponent   = 1.0;
  Tpat->Frequency  = 1.0;
  Tpat->Phase      = 0.0;
  Tpat->Warps      = NULL;
  Tpat->Next       = NULL;
  Tpat->Blend_Map  = NULL;
#ifdef SplineWavePatch
  Tpat->spline_wave  = NULL;
#endif  
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Copy_TPat_Fields (TPATTERN *New, TPATTERN  *Old)
{
  /* is this necessary, or even wanted? */
  /*
  if ( New->Type == CRACKLE_PATTERN )
  {
    if (New->Vals.Crackle.cv)
      POV_FREE(New->Vals.Crackle.cv);
  }
  */
  *New = *Old;
  
  /* Copy warp chain */
  New->Warps = Copy_Warps(Old->Warps);

  New->Blend_Map = Copy_Blend_Map(Old->Blend_Map);

  /* Note, cannot copy Old->Next because we don't know what kind of
     thing this is.  It must be copied by Copy_Pigment, Copy_Tnormal etc.
  */

  /* NK 1998 - added IMAGE_PATTERN */
  if ((Old->Type == BITMAP_PATTERN) || (Old->Type == IMAGE_PATTERN))
  {
     New->Vals.Image = Copy_Image(Old->Vals.Image);
  }

#ifdef SplineWavePatch
  if(Old->spline_wave != NULL)
      New->spline_wave = Copy_Spline(Old->spline_wave);
#endif
  
#ifdef ObjectPatternPatch
  if (Old->Type == OBJECT_PATTERN)
  {
    if(Old->Vals.Object != NULL)
    {
      New->Vals.Object = (OBJECT*)Copy_Object(Old->Vals.Object);
    }
  }
#endif

#ifdef ProximityPatch
  if (Old->Type == PROXIMITY_PATTERN)
  {
    if(Old->Vals.Proximity.proxObject != NULL)
    {
      New->Vals.Proximity.proxObject = (OBJECT*)Copy_Object(Old->Vals.Proximity.proxObject);
    }
  }
#endif

#ifdef BlobPatternPatch
  if(Old->Type == BLOB_PATTERN ||
     Old->Type == BLOB_PIGMENT)
  {
    if(Old->Vals.Blob.blob_dat != NULL)
    {
        BLOB_PATTERN_DATA * currentComponent = Old->Vals.Blob.blob_dat;
        BLOB_PATTERN_DATA * nextComponent = currentComponent->next;/*tPat->blob_dat->next;*/
        BLOB_PATTERN_DATA * destComponent = NULL;
      
        while(currentComponent != NULL)/*Chris Huff blob pattern*/
        {
            BLOB_PATTERN_DATA * componentCopy = (BLOB_PATTERN_DATA *)POV_MALLOC(sizeof(BLOB_PATTERN_DATA), "spherical blob pattern component");
            nextComponent = currentComponent->next;
          
            /*START: copy component*/
            *componentCopy = *currentComponent;
            if(currentComponent->transform)
	            componentCopy->transform = Copy_Transform(currentComponent->transform);
	            
            if(currentComponent->pigment)
	            componentCopy->pigment = Copy_Pigment(currentComponent->pigment);
/*            if(currentComponent->pattern)
	            Copy_TPat_Fields(componentCopy->pattern, currentComponent->pattern);*/
            if(currentComponent->blob)
	            componentCopy->blob = (BLOB *)Copy_Object((OBJECT *)currentComponent->blob);
            componentCopy->next = NULL;
            /*END: copy component*/
          
            /*add in new component*/
            if(destComponent == NULL)/*This is the first copy*/
            {
                New->Vals.Blob.blob_dat = componentCopy;
                destComponent = New->Vals.Blob.blob_dat;
            }
            else
            {
                destComponent->next = componentCopy;
                destComponent = destComponent->next;
            }
          
            /*move to next component*/
            currentComponent = nextComponent;
        }
    }
  }
#endif
  if (Old->Type == DENSITY_FILE_PATTERN)
  {
     New->Vals.Density_File = Copy_Density_File(Old->Vals.Density_File);
  }

#ifdef CracklePatch
  if (Old->Type == CRACKLE_PATTERN)
  {
    New->Vals.Crackle.cv =(VECTOR*) POV_MALLOC( 125*sizeof(VECTOR), "crackle cache");
  }

#endif
#ifdef PigmentPatternPatch
  if (Old->Type == PIGMENT_PATTERN )
  {
    New->Vals.Pigment = Copy_Pigment(Old->Vals.Pigment);
  }
#endif
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Destroy_TPat_Fields(TPATTERN *Tpat)
{
  Destroy_Warps(Tpat->Warps);
  Destroy_Blend_Map(Tpat->Blend_Map);
  /* Note, cannot destroy Tpat->Next nor pattern itself because we don't
     know what kind of thing this is.  It must be destroied by Destroy_Pigment, etc.
  */

  /* NK 1998 - added IMAGE_PATTERN */
  if ((Tpat->Type == BITMAP_PATTERN))/* || (Tpat->Type == IMAGE_PATTERN))*/
  {
     Destroy_Image(Tpat->Vals.Image);
  }
  
#ifdef SplineWavePatch
  if(Tpat->spline_wave != NULL)
      Destroy_Spline(Tpat->spline_wave);
#endif
  if (Tpat->Type == DENSITY_FILE_PATTERN)
  {
     Destroy_Density_File(Tpat->Vals.Density_File);
  }
#ifdef ObjectPatternPatch
  if (Tpat->Type == OBJECT_PATTERN)
  {
    if(Tpat->Vals.Object != NULL)
    {
        Destroy_Object((OBJECT *)Tpat->Vals.Object);
    }
  }
#endif
#ifdef ProximityPatch
  if (Tpat->Type == PROXIMITY_PATTERN)
  {
    if(Tpat->Vals.Proximity.proxObject != NULL)
    {
        Destroy_Object((OBJECT *)Tpat->Vals.Proximity.proxObject);
    }
  }
#endif
#ifdef BlobPatternPatch
  if (Tpat->Type == BLOB_PATTERN ||
      Tpat->Type == BLOB_PIGMENT)
  {
    if(Tpat->Vals.Blob.blob_dat != NULL)/*Chris Huff blob pattern*/
    {
        BLOB_PATTERN_DATA * currentComponent = Tpat->Vals.Blob.blob_dat;
        BLOB_PATTERN_DATA * nextComponent = NULL;/*tPat->blob_dat->next;*/
        while(currentComponent != NULL)
        {
          nextComponent = currentComponent->next;
        
          if(currentComponent->pigment != NULL)
              Destroy_Pigment(currentComponent->pigment);
/*          if(currentComponent->pattern != NULL)
              Destroy_TPat_Fields(currentComponent->pattern);*/
          if(currentComponent->blob != NULL)
              Destroy_Object((OBJECT *)currentComponent->blob);
          POV_FREE(currentComponent->transform);
          POV_FREE(currentComponent);
        
          currentComponent = nextComponent;
        }
    }
  }
#endif
#ifdef CracklePatch
  if ( Tpat->Type == CRACKLE_PATTERN )
  {
    if ( Tpat->Vals.Crackle.cv )
    POV_FREE( Tpat->Vals.Crackle.cv );
    Tpat->Vals.Crackle.cv = NULL;
  }
#endif
#ifdef PigmentPatternPatch
  if (Tpat->Type == PIGMENT_PATTERN )
  {
    Destroy_Pigment(Tpat->Vals.Pigment);
    Tpat->Vals.Pigment = NULL;
  }
#endif
}




/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

TURB *Create_Turb()
{
  TURB *New;

  New = (TURB *)POV_MALLOC(sizeof(TURB),"turbulence struct");

  Make_Vector(New->Turbulence, 0.0, 0.0, 0.0);

  New->Octaves = 6;
  New->Omega = 0.5;
  New->Lambda = 2.0;

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

#if 0   /* Unused function [AED] */
static TURB *Copy_Turb(TURB *Old)
{
  TURB *New;

  if (Old != NULL)
  {
    New = Create_Turb();

    *New = *Old;
  }
  else
  {
    New=NULL;
  }

  return(New);
}
#endif


/*****************************************************************************
*
* FUNCTION
*
*   Translate_Tpattern
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Translate_Tpattern(TPATTERN *Tpattern,VECTOR Vector)
{
  TRANSFORM Trans;

  if (Tpattern != NULL)
  {
    Compute_Translation_Transform (&Trans, Vector);

    Transform_Tpattern (Tpattern, &Trans);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Tpattern
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Rotate_Tpattern(TPATTERN *Tpattern,VECTOR Vector)
{
  TRANSFORM Trans;

  if (Tpattern != NULL)
  {
    Compute_Rotation_Transform (&Trans, Vector);

    Transform_Tpattern (Tpattern, &Trans);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Tpattern
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Scale_Tpattern(TPATTERN *Tpattern,VECTOR Vector)
{
  TRANSFORM Trans;

  if (Tpattern != NULL)
  {
    Compute_Scaling_Transform (&Trans, Vector);

    Transform_Tpattern (Tpattern, &Trans);
  }
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Tpattern
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Transform_Tpattern(TPATTERN *Tpattern,TRANSFORM *Trans)
{
  WARP *Temp;

  if (Tpattern != NULL)
  {
    if (Tpattern->Warps == NULL)
    {
      Tpattern->Warps=Create_Warp(TRANSFORM_WARP);
    }
    else
    {
      if (Tpattern->Warps->Warp_Type != TRANSFORM_WARP)
      {
        Temp=Tpattern->Warps;

        Tpattern->Warps=Create_Warp(TRANSFORM_WARP);

        Tpattern->Warps->Next_Warp=Temp;
      }
    }

    Compose_Transforms (&( ((TRANS *)(Tpattern->Warps))->Trans), Trans);
  }
}


/*****************************************************************************
*
* FUNCTION
*
*   ripples_pigm
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*   
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*   
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION   : Note this pattern is only used for pigments and textures.
*                 Normals have a specialized pattern for this.
*
* CHANGES
*   Nov 1994 : adapted from normal by [CY]
*
******************************************************************************/

static DBL ripples_pigm (VECTOR EPoint, TPATTERN *TPat)
{
  register unsigned int i;
  register DBL length, index;
  DBL scalar =0.0;
  VECTOR point;

  for (i = 0 ; i < Number_Of_Waves ; i++)
  {
    VSub (point, EPoint, Wave_Sources[i]);
    VLength (length, point);

    if (length == 0.0)
      length = 1.0;

    index = length * TPat->Frequency + TPat->Phase;

    scalar += cycloidal(index);
  }

  scalar = 0.5*(1.0+(scalar / (DBL)Number_Of_Waves));

  return(scalar);
}


/*****************************************************************************
*
* FUNCTION
*
*   waves_pigm
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION   : Note this pattern is only used for pigments and textures.
*                 Normals have a specialized pattern for this.
*
* CHANGES
*   Nov 1994 : adapted from normal by [CY]
*
******************************************************************************/

static DBL waves_pigm (VECTOR EPoint, TPATTERN *TPat)
{
  register unsigned int i;
  register DBL length, index;
  DBL scalar = 0.0;
  VECTOR point;

  for (i = 0 ; i < Number_Of_Waves ; i++)
  {
    VSub (point, EPoint, Wave_Sources[i]);
    VLength (length, point);

    if (length == 0.0)
    {
      length = 1.0;
    }

    index = length * TPat->Frequency * frequency[i] + TPat->Phase;

    scalar += cycloidal(index)/frequency[i];
  }

  scalar = 0.2*(2.5+(scalar / (DBL)Number_Of_Waves));

  return(scalar);
}


/*****************************************************************************
*
* FUNCTION
*
*   dents_pigm
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION   : Note this pattern is only used for pigments and textures.
*                 Normals have a specialized pattern for this.
*
* CHANGES
*   Nov 1994 : adapted from normal by [CY]
*
******************************************************************************/

static DBL dents_pigm (VECTOR EPoint)
{
  DBL noise;

  noise = Noise (EPoint);

  return(noise * noise * noise);
}



/*****************************************************************************
*
* FUNCTION
*
*   wrinkles_pigm
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   
* OUTPUT
*   
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION   : Note this pattern is only used for pigments and textures.
*                 Normals have a specialized pattern for this.
*
* CHANGES
*   Nov 1994 : adapted from normal by [CY]
*
******************************************************************************/


static DBL wrinkles_pigm (VECTOR EPoint)
{
  register int i;
  DBL lambda = 2.0;
  DBL omega = 0.5;
  DBL value;
  VECTOR temp;

  value = Noise(EPoint);

  for (i = 1; i < 10; i++)
  {
    VScale(temp,EPoint,lambda);

    value += omega * Noise(temp);

    lambda *= 2.0;

    omega *= 0.5;
  }

  return(value/2.0);
}



/*****************************************************************************
*
* FUNCTION
*
*   quilted_pigm
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Dan Farmer & Chris Young
*   
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static DBL quilted_pigm (VECTOR EPoint, TPATTERN *TPat)
{
  VECTOR value;
  DBL t;

  value[X] = EPoint[X]-FLOOR(EPoint[X])-0.5;
  value[Y] = EPoint[Y]-FLOOR(EPoint[Y])-0.5;
  value[Z] = EPoint[Z]-FLOOR(EPoint[Z])-0.5;

  t = sqrt(value[X]*value[X]+value[Y]*value[Y]+value[Z]*value[Z]);

  t = quilt_cubic(t, TPat->Vals.Quilted.Control0, TPat->Vals.Quilted.Control1);

  value[X] *= t;
  value[Y] *= t;
  value[Z] *= t;

  return((fabs(value[X])+fabs(value[Y])+fabs(value[Z]))/3.0);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

#define INV_SQRT_3_4 1.154700538
DBL quilt_cubic(DBL t,DBL p1,DBL p2)
{
 DBL it=(1-t);
 DBL itsqrd=it*it;
/* DBL itcubed=it*itsqrd; */
 DBL tsqrd=t*t;
 DBL tcubed=t*tsqrd;
 DBL val;

/* Originally coded as...

 val= (DBL)(itcubed*n1+(tcubed)*n2+3*t*(itsqrd)*p1+3*(tsqrd)*(it)*p2);

 re-written by CEY to optimise because n1=0 n2=1 always.
 
*/

 val = (tcubed + 3.0*t*itsqrd*p1 + 3.0*tsqrd*it*p2) * INV_SQRT_3_4;
 
 return(val);
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Search_Blend_Map (DBL value,BLEND_MAP *Blend_Map,BLEND_MAP_ENTRY **Prev,BLEND_MAP_ENTRY  **Cur)
{
  BLEND_MAP_ENTRY *P, *C;
  int Max_Ent=Blend_Map->Number_Of_Entries-1;

  /* if greater than last, use last. */

  if (value >= Blend_Map->Blend_Map_Entries[Max_Ent].value)
  {
    P = C = &(Blend_Map->Blend_Map_Entries[Max_Ent]);
  }
  else
  {
    P = C = &(Blend_Map->Blend_Map_Entries[0]);

    while (value > C->value)
    {
      P = C++;
    }
  }

  if (value == C->value)
  {
    P = C;
  }

  *Prev = P;
  *Cur  = C;
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static TURB *Search_For_Turb(WARP *Warps)
{
  WARP* Temp=Warps;

  if (Temp!=NULL)
  {
    while (Temp->Next_Warp != NULL)
    {
      Temp=Temp->Next_Warp;
    }

    if (Temp->Warp_Type != CLASSIC_TURB_WARP)
    {
       Temp=NULL;
    }
  }

  return ((TURB *)Temp);
}


/*****************************************************************************
*
* FUNCTION
*
*   density_file
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
* CHANGES
*
*   Dec 1996 : Creation.
*
******************************************************************************/

static DBL density_file(VECTOR EPoint, TPATTERN *TPat)
{
  int x, y, z;
  int x1, y1, z1;
  int x2, y2, z2;
  DBL xx, yy, zz;
  DBL xi, yi, zi;
  DBL f111, f112, f121, f122, f211, f212, f221, f222;
  DBL density = 0.0;
  DENSITY_FILE_DATA *Data;

  if ((TPat->Vals.Density_File != NULL) &&
      ((Data = TPat->Vals.Density_File->Data) != NULL))
  {
    if ((EPoint[X] >= 0.0) && (EPoint[X] < 1.0) &&
        (EPoint[Y] >= 0.0) && (EPoint[Y] < 1.0) &&
        (EPoint[Z] >= 0.0) && (EPoint[Z] < 1.0))
    {
      switch (TPat->Vals.Density_File->Interpolation)
      {
        case NO_INTERPOLATION:

          x = (int)(EPoint[X] * (DBL)Data->Sx);
          y = (int)((EPoint[Y] )* (DBL)Data->Sy); 
          z = (int)(EPoint[Z] * (DBL)Data->Sz);

          if ((x < 0) || (x >= Data->Sx) ||
              (y < 0) || (y >= Data->Sy) ||
              (z < 0) || (z >= Data->Sz))
          {
            density = 0.0;
          }
          else
          {
            density = (DBL)Data->Density[z][y][x] / 255.0;
          }

          break;

        case TRILINEAR_INTERPOLATION:

          xx = EPoint[X] * (DBL)(Data->Sx - 1);
          yy = (EPoint[Y]) * (DBL)(Data->Sy - 1);
          zz = EPoint[Z] * (DBL)(Data->Sz - 1);

          x1 = (int)xx;
          y1 = (int)yy;
          z1 = (int)zz;

          x2 = x1 + 1;
          y2 = y1 + 1;
          z2 = z1 + 1;

          xx -= floor(xx);
          yy -= floor(yy);
          zz -= floor(zz);

          xi = 1.0 - xx;
          yi = 1.0 - yy;
          zi = 1.0 - zz;

          f111 = (DBL)Data->Density[z1][y1][x1] / 255.0;
          f112 = (DBL)Data->Density[z1][y1][x2] / 255.0;
          f121 = (DBL)Data->Density[z1][y2][x1] / 255.0;
          f122 = (DBL)Data->Density[z1][y2][x2] / 255.0;
          f211 = (DBL)Data->Density[z2][y1][x1] / 255.0;
          f212 = (DBL)Data->Density[z2][y1][x2] / 255.0;
          f221 = (DBL)Data->Density[z2][y2][x1] / 255.0;
          f222 = (DBL)Data->Density[z2][y2][x2] / 255.0;

          density = f111 * zi * yi * xi +
                    f112 * zi * yi * xx +
                    f121 * zi * yy * xi +
                    f122 * zi * yy * xx +
                    f211 * zz * yi * xi +
                    f212 * zz * yi * xx +
                    f221 * zz * yy * xi +
                    f222 * zz * yy * xx;

          break;
      }
    }
    else
    {
      density = 0.0;
    }

/*
    fprintf(stderr, "x = %3d, y = %3d, z = %3d, density = %5.4f\n", x, y, z, density);
*/
  }
  return(density);
}


/*****************************************************************************
*
* FUNCTION
*
*   Create_Density_File
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Create a density file structure.
*
* CHANGES
*
*   Dec 1996 : Creation.
*
******************************************************************************/

DENSITY_FILE *Create_Density_File()
{
  DENSITY_FILE *New;

  New = (DENSITY_FILE *)POV_MALLOC(sizeof(DENSITY_FILE), "density file");

  New->Interpolation = NO_INTERPOLATION;

  New->Data = (DENSITY_FILE_DATA *)POV_MALLOC(sizeof(DENSITY_FILE_DATA), "density file data");

  New->Data->References = 1;

  New->Data->Name = NULL;

  New->Data->Sx =
  New->Data->Sy =
  New->Data->Sz = 0;

  New->Data->Density = NULL;

  return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Density_File
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Copy a density file structure.
*
* CHANGES
*
*   Dec 1996 : Creation.
*
******************************************************************************/

DENSITY_FILE *Copy_Density_File(DENSITY_FILE *Old)
{
  DENSITY_FILE *New;

  if (Old != NULL)
  {
    New = (DENSITY_FILE *)POV_MALLOC(sizeof(DENSITY_FILE), "density file");

    *New = *Old;

    New->Data->References++;
  }
  else          /* tw */
    New = NULL; /* tw */

  return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Density_File
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Destroy a density file structure.
*
* CHANGES
*
*   Dec 1996 : Creation.
*
******************************************************************************/

void Destroy_Density_File(DENSITY_FILE *Density_File)
{
	int y, z;
	if (Density_File != NULL)
	{
		if ((--(Density_File->Data->References)) == 0)
		{
			POV_FREE(Density_File->Data->Name);
			for (z = 0; z < Density_File->Data->Sz; z++)
			{
				for (y = 0; y < Density_File->Data->Sy; y++)
					POV_FREE(Density_File->Data->Density[z][y]);
				POV_FREE(Density_File->Data->Density[z]);
			}
			if(Density_File->Data->Density!= NULL)  /*YS aug 2000 bug fix*/
				POV_FREE(Density_File->Data->Density);
			POV_FREE(Density_File->Data);
		}
		POV_FREE(Density_File);
	}
}


void Read_Density_File(DENSITY_FILE *df)
{
  unsigned char ***map;
  int y, z, sx, sy, sz;
  FILE *file;

  if (df == NULL)
  {
     return;
  }
  
  /* Allocate and read density file. */

  if ((df != NULL) && (df->Data->Name != NULL))
  {
    if ((file = Locate_File(df->Data->Name, READ_BINFILE_STRING, ".df3", ".DF3",NULL,TRUE)) == NULL)
    {
      Error("Cannot read media density file.\n");
    }
    
    sx = df->Data->Sx = readushort(file);
    sy = df->Data->Sy = readushort(file);
    sz = df->Data->Sz = readushort(file);

    map = (unsigned char ***)POV_MALLOC(sz*sizeof(unsigned char **), "media density file data");

    for (z = 0; z < sz; z++)
    {
      map[z] = (unsigned char **)POV_MALLOC(sy*sizeof(unsigned char *), "media density file data");

      for (y = 0; y < sy; y++)
      {
        map[z][y] = (unsigned char *)POV_MALLOC(sx*sizeof(unsigned char), "media density file data");

        fread(map[z][y], sizeof(unsigned char), (size_t)sx, file);
      }
    }

    df->Data->Density = map;

    if (file != NULL)		/* -hdf99- */
    {
      fclose(file);
    }
  }
}

#ifdef SolidPatternPatch
 /*Chris Huff 7/20/99 solid pattern*/
static DBL SolidPat(VECTOR EPoint, TPATTERN *TPat)
{
	return TPat->Vals.SolidVal;
}
#endif
#ifdef ClothPatternPatch
/*Chris Huff cloth pattern*/
static DBL ClothPat(VECTOR EPoint)
{
	DBL xVal = EPoint[X];
	DBL zVal = EPoint[Z];
	xVal = ((xVal > 1.0) ? fmod(xVal, 1.0) : xVal);
	zVal = ((zVal > 1.0) ? fmod(zVal, 1.0) : zVal);
/*	DBL xVal = ((EPoint[X] > 1.0) ? fmod(EPoint[X], 1.0) : EPoint[X]);
	DBL zVal = ((EPoint[Z] > 1.0) ? fmod(EPoint[Z], 1.0) : EPoint[Z]);*/

	if(checker(EPoint))
		return fabs(xVal);/*fabs(cycloidal(xVal/2));*/
	else
		return fabs(zVal);/*fabs(cycloidal(zVal/2));*/
}

/*Chris Huff cloth2 pattern*/
static DBL Cloth2Pat(VECTOR EPoint)
{
	DBL xVal = fabs(EPoint[X]);
	DBL zVal = fabs(EPoint[Z]);
	xVal = ((xVal > 1.0) ? fmod(xVal, 1.0) : xVal);
	zVal = ((zVal > 1.0) ? fmod(zVal, 1.0) : zVal);

	if(checker(EPoint))
		return (fabs(xVal)/2);/*fabs(cycloidal(xVal/2));*/
	else
		return ((fabs(zVal)/2)+0.5);/*fabs(cycloidal(zVal/2));*/
}
#endif
#ifdef TorodialPatch
/*Chris Huff torodial pattern*/
static DBL ToroidalSpiral(VECTOR EPoint, TPATTERN *TPat)
{
    DBL x = EPoint[X];
    DBL y = EPoint[Y];
    DBL z = EPoint[Z];
	DBL longitude = atan2(x, y);
	DBL latitude = atan2(sqrt(x*x + y*y)-1.0, z);

	return fmod(((latitude + longitude*(TPat->Vals.SolidVal))/2*M_PI), 1.0);
}
#endif
static unsigned short readushort(FILE *infile)
{
  int i0, i1 = 0; /* To quiet warnings */

  if ((i0  = fgetc(infile)) == EOF || (i1  = fgetc(infile)) == EOF)
  {
    Error("Error reading density_file\n");
  }

  return (unsigned short)((((unsigned short)i0) << 8) | ((unsigned short)i1));
}

/** poviso: May 97 R.S. **/
#ifdef POVISO
/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR  R.Suzuki
*
* DESCRIPTION
*
* CHANGES    May 97 
*
******************************************************************************/



static DBL func_pat (VECTOR EPoint, TPATTERN *TPat)
{
  DBL value;
  VECTOR V1;
  FUNCTION *TFunc;

  Assign_Vector(V1,EPoint);
  TFunc=(FUNCTION*)TPat->Vals.Function;
  if (TFunc!=NULL) value = (TFunc->iso_func)(TFunc, V1);
  value = ((value > 1.0) ? fmod(value, 1.0) : value);
  return(value);
}

#endif
/** --- **/

#ifdef PigmentPatternPatch
static DBL pigment_pattern (VECTOR EPoint, TPATTERN *TPat, INTERSECTION *Inter)
{
  DBL value;
  COLOUR Col;
  int colour_found=FALSE;

  if (TPat->Vals.Pigment)
  {
    colour_found = Compute_Pigment(Col,TPat->Vals.Pigment,EPoint,Inter);
  }

  if(!colour_found)
  {
    value = 0.0;
  }
  else
  {
    value = GREY_SCALE(Col);
  }

  return(value);
}
#endif

/*****************************************************************************
*
* FUNCTION
*
*   slope
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*             is evaluated.
*   TPat   -- Texture pattern struct
*   Intersection - intersection struct
*
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0, 0.0 if normal is NULL
*
* AUTHOR
*
*   -hdf-
*
* DESCRIPTION   :
*
*   calculates the surface slope from surface normal vector
*
* CHANGES
*   Apr 1998 : written by H.-D. Fink
*   May 1998 : modified by M.C. Andrews - now combines slope and 'gradient'.
*
******************************************************************************/

static DBL slope (VECTOR EPoint, TPATTERN *TPat, INTERSECTION *Intersection)
{
  DBL value, value1, value2;

  if (Intersection == NULL) return 0.0; /* just in case ... */

  if (TPat->Vals.Slope.Slope_Base > 0)
    /* short case 1: slope vector in x, y or z direction */
    value1 = Intersection->PNormal[TPat->Vals.Slope.Slope_Base - 1];
  else if (TPat->Vals.Slope.Slope_Base < 0)
    /* short case 2: slope vector in negative x, y or z direction */
    value1 = -Intersection->PNormal[-TPat->Vals.Slope.Slope_Base - 1];
  else
    /* projection slope onto normal vector */
    VDot(value1, Intersection->PNormal, TPat->Vals.Slope.Slope_Vector);

  /* Clamp to 1.0. */
  /* should never be necessary since both vectors are normalized */
  if      (value1 >  1.0) value1 =  1.0;
  else if (value1 < -1.0) value1 = -1.0;

  value1 = (value1 + 1.0) * 0.5;	/* normalize to [0..1] interval */

  if (!TPat->Vals.Slope.Altit_Len) return value1; /* no altitude defined */

  /* Calculate projection of Epoint along altitude vector */
  if (TPat->Vals.Slope.Altit_Base > 0)
    /* short case 1: altitude vector in x, y or z direction */
    value2 = EPoint[TPat->Vals.Slope.Altit_Base - 1];
  else if (TPat->Vals.Slope.Altit_Base < 0)
    /* short case 2: altitude vector in negative x, y or z direction */
    value2 = -EPoint[-TPat->Vals.Slope.Altit_Base - 1];
  else
    /* projection of Epoint along altitude vector */
    VDot(value2, EPoint, TPat->Vals.Slope.Altit_Vector);

  /* If set, use offset and scalings for slope and altitude. */
  if (0.0 != TPat->Vals.Slope.Slope_Mod[V])
  {
    value1 = (value1 - TPat->Vals.Slope.Slope_Mod[U]) / TPat->Vals.Slope.Slope_Mod[V];
  }
  if (0.0 != TPat->Vals.Slope.Altit_Mod[V])
  {
    value2 = (value2 - TPat->Vals.Slope.Altit_Mod[U]) / TPat->Vals.Slope.Altit_Mod[V];
  }

  value = TPat->Vals.Slope.Slope_Len * value1 + TPat->Vals.Slope.Altit_Len * value2;

  /* Clamp to 1.0. */
  value = (value < 0.0) ? 1.0 + fmod(value, 1.0) : fmod(value, 1.0);
  return value;

}

#ifdef CellsPatch
/*****************************************************************************
*
* FUNCTION
*
*   cells
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   John VanSickle
*
* DESCRIPTION
*
*   "cells":
*
*   New colour function by John VanSickle,
*     vansickl@erols.com
*
*   Assigns a pseudorandom value to each unit cube.  The value for the cube in
*   which the evaluted point lies is returned.
*
*   All "cells" specific source code and examples are in the public domain.
*
* CHANGES
*   Oct 1994    : original crackle code adapted by [CY]
*   Jul 1999    : adapted for a different pattern by [JV]
*
******************************************************************************/

static DBL cells (VECTOR EPoint)
{
  int    temp,seed;
  DBL    tf;

  /* select a random value based on the cube from which this came. */

#ifdef NoiseTranslateFixPatch
  seed = Hash3d((int)EPoint[X]&0xFFF, (int)EPoint[Y]&0xFFF, (int)EPoint[Z]&0xFFF);
#else
  seed = Hash3d((int)EPoint[X], (int)EPoint[Y], (int)EPoint[Z]);
#endif
  temp = POV_GET_OLD_RAND(); /* save current seed */
  
  POV_SRAND(seed);

  tf = FRAND();

  POV_SRAND(temp);  /* restore */

  return min(tf,1.);
}
#endif

#ifdef VanSicklePatternPatch
/*****************************************************************************
*
* FUNCTION
*
*   blotches
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
* OUTPUT
*
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*
* AUTHOR
*
*   John VanSickle, based on code by Jim McElhiney
*
* DESCRIPTION
*
*   "blotches":
*
*   New colour function by John VanSickle,
*     vansickl@erols.com
*
*   Assigns a pseudorandom point to each unit cube, and another float from 0 to
*   1 to each cube.  The nearest pseudorandom point to the evaluated point is
*   selected, and its associated pseudorandom value is returned.  It creates
*   regions of uniform value.
*
*   All "blotches" specific source code and examples are in the public domain.
*
* CHANGES
*   Oct 1994    : original crackle code adapted by [CY]
*   Jul 1999    : adapted for a different pattern by [JV]
*
******************************************************************************/

static DBL blotches (VECTOR EPoint)
{
  int    i,j,temp,seed;
  long   thisseed;
  DBL    sum, minsum,  tf;
  VECTOR sv, tv, dv, t1, add;

  static int bvc;
  static long lastbeed = 0x80000000;
  static VECTOR bv[81];

  Assign_Vector(tv,EPoint);

  /*
   * Check to see if the input point is in the same unit cube as the last
   * call to this function, to use cache of cubelets for speed.
   */

  thisseed = PickInCube(tv, t1);

  if (thisseed != lastbeed)
  {
    /*
     * No, not same unit cube.  Calculate the random points for this new
     * cube and its 80 neighbours which differ in any axis by 1 or 2.
     * Why distance of 2?  If there is 1 point in each cube, located
     * randomly, it is possible for the closest random point to be in the
     * cube 2 over, or the one two over and one up.  It is NOT possible
     * for it to be two over and two up.  Picture a 3x3x3 cube with 9 more
     * cubes glued onto each face.
     */

    /* Now store a points for this cube and each of the 80 neighbour cubes. */

    bvc = 0;

    for (add[X] = -2.0; add[X] < 2.5; add[X] +=1.0)
    {
      for (add[Y] = -2.0; add[Y] < 2.5; add[Y] += 1.0)
      {
        for (add[Z] = -2.0; add[Z] < 2.5; add[Z] += 1.0)
        {
          /* For each cubelet in a 5x5 cube. */

          if ((fabs(add[X])>1.5)+(fabs(add[Y])>1.5)+(fabs(add[Z])>1.5) <= 1.0)
          {
            /* Yes, it's within a 3d knight move away. */

            VAdd(sv, tv, add);

            PickInCube(sv, t1);

            bv[bvc][X] = t1[X];
            bv[bvc][Y] = t1[Y];
            bv[bvc][Z] = t1[Z];
            bvc++;
          }
        }
      }
    }

    lastbeed = thisseed;
  }

  /*
   * Find the point with the shortest distance from the input point.
   * Loop invariant:  minsum is shortest dist
   */

  VSub(dv, bv[0], tv);  minsum  = VSumSqr(dv);
  j=0;

  /* Loop for the 81 cubelets to find closest. */

  for (i = 1; i < bvc; i++)
  {
    VSub(dv, bv[i], tv);

    sum = VSumSqr(dv);

    if (sum < minsum) {
      minsum = sum;
      j=i;
    }
  }

  /* select a random value based on the cube from which this came. */

#ifdef NoiseTranslateFixPatch
  seed = Hash3d((int)bv[j][X]&0xFFF, (int)bv[j][Y]&0xFFF, (int)bv[j][Z]&0xFFF);
#else
  seed = Hash3d((int)bv[j][X], (int)bv[j][Y], (int)bv[j][Z]);
#endif
  temp = POV_GET_OLD_RAND(); /* save current seed */
  
  POV_SRAND(seed);

  /* The first three values were used for the random point, and shouldn't
     be used for the value of the region */

  FRAND();
  FRAND();
  FRAND();

  tf = FRAND();

  POV_SRAND(temp);  /* restore */

  return min(tf,1.);
}

/*****************************************************************************
*
* FUNCTION
*
*   bands
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   
* OUTPUT
*   
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*   
* AUTHOR
*
*   John VanSickle, based on the gradient code
*   
* DESCRIPTION
*
*   banded Pattern - just like gradient, but is not mirrored around the origin,
*   and the bands are of uniform width, regardless of the direction of the
*   vector.
*
* CHANGES
*   Oct 1994    : adapted from pigment by [CY]
*   Jul 1999    : adapted from gradient by [JV]
*
******************************************************************************/

/* ¡MUY IMPORTANTE! This function expects the Vals.Gradient vector in TPat to
 * be normalized (ie, unit length).  If the vector is greater or less than
 * unit length, there will be a scaling effect on the pattern.  I could have
 * divided by the magnitude of the vector, but since this function will be
 * called more than once by the renderer, it is more appropriate for the
 * parser to normalize the vector (where it need be done only once). */

static DBL banded (VECTOR EPoint, TPATTERN *TPat)
{
  register DBL value;

  value = EPoint[X]*TPat->Vals.Gradient[X]
        + EPoint[Y]*TPat->Vals.Gradient[Y]+EPoint[Z]*TPat->Vals.Gradient[Z];

  return(value-floor(value));
}

/*****************************************************************************
*
* FUNCTION
*
*   sheet
*
* INPUT
*
*   EPoint -- The point in 3d space at which the pattern
*   is evaluated.
*   
* OUTPUT
*   
* RETURNS
*
*   DBL value in the range 0.0 to 1.0
*   
* AUTHOR
*
*   John VanSickle, based on the gradient code
*   
* DESCRIPTION
*
*   Takes the dot product of the evaluated point and a specified vector.
*   If the product is <0, 0 is returned.  If the product is >1, 1 is returned.
*   Otherwise the product is returned.  This is basically a non-repeating,
*   non-mirrored gradient, but like the banded pattern above, the width of
*   the band is independent of direction of the vector.
*
* CHANGES
*   Oct 1994    : adapted from pigment by [CY]
*   Jul 1999    : adapted from gradient by [JV]
*
******************************************************************************/

/* ¡MUY IMPORTANTE! This function expects the Vals.Gradient vector in TPat to
 * be normalized (ie, unit length).  If the vector is greater or less than
 * unit length, there will be a scaling effect on the pattern.  I could have
 * divided by the magnitude of the vector, but since this function will be
 * called more than once by the renderer, it is more appropriate for the
 * parser to normalize the vector (where it need be done only once). */

static DBL sheet (VECTOR EPoint, TPATTERN *TPat)
{
  register DBL value;

  value = EPoint[X]*TPat->Vals.Gradient[X]
        + EPoint[Y]*TPat->Vals.Gradient[Y]+EPoint[Z]*TPat->Vals.Gradient[Z];

  return(value>1.0 ? 1.0 : (value<0.0? 0.0:value));
}
#endif
#ifdef BlobPatternPatch
/*Chris Huff-blob pattern*/
DBL blob_comp_strength(VECTOR EPoint, BLOB_PATTERN_DATA * comp)
{
	VECTOR TPoint;
	DBL distance = 0;
	DBL strength = comp->strength;
	DBL radius = comp->radius;
	DBL falloff = comp->falloff;
	DBL fieldVal = 0;
	MInvTransPoint (TPoint, EPoint, comp->transform);
	/*Calculate distance*/
	switch(comp->type)
	{
	    case 0:
		{/*spherical component*/
			VDist(distance, TPoint, comp->center);
        	if(distance < radius)/*Clip to the component shape*/
        	{
        	    if(comp->inverse == TRUE)
        	    {
            		fieldVal = strength*eval_density_func(1-(distance/radius), falloff, comp->function);
        	    }
        	    else
        	    {
            		fieldVal = strength*eval_density_func(distance/radius, falloff, comp->function);
        	    }
        	}
		}
		break;
	    case 1:
		{/*cylinderical component*/
			/*calculate closest point on axis of cylinder to evaluation point*/
			DBL x1 = comp->center[X];
			DBL x2 = comp->pointB[X];
			DBL x3 = TPoint[X];
			DBL y1 = comp->center[Y];
			DBL y2 = comp->pointB[Y];
			DBL y3 = TPoint[Y];
			DBL z1 = comp->center[Z];
			DBL z2 = comp->pointB[Z];
			DBL z3 = TPoint[Z];
			DBL dist = ((x3 - x1)*(x2 - x1) + (y3 - y1)*(y2 - y1) + (z3 - z1)*(z2 - z1));
			
			VECTOR nearestPt;
			VECTOR cylAxis;
			DBL len;
			VSub(cylAxis, comp->pointB, comp->center);
			VLength(len, cylAxis);
			dist = dist/(len*len);
						
			if(dist <= 0)
			{
				VDist(distance, TPoint, comp->center);
			}
			else
			{
				if(dist <= 1)
				{
					VScale(nearestPt, cylAxis, dist);
					VAddEq(nearestPt, comp->center);
					VDist(distance, TPoint, nearestPt);
				}
				else
				{
					VDist(distance, TPoint, comp->pointB);
				}
			}
        	if(distance < radius)/*Clip to the component shape*/
        	{
        	    if(comp->inverse == TRUE)
        	    {
            		fieldVal = strength*eval_density_func(1-(distance/radius), falloff, comp->function);
        	    }
        	    else
        	    {
            		fieldVal = strength*eval_density_func(distance/radius, falloff, comp->function);
        	    }
        	}
		}
		break;
	    case 2:
		{/*box component*/
		    VECTOR halfSize;
		    VECTOR center;
		    VHalf(center, comp->center, comp->pointB);
		    
		    Assign_Vector(halfSize, comp->pointB);
		    VSubEq(halfSize, comp->center);
		    VInverseScaleEq(halfSize, 2);
		    
		    distance = max(fabs(TPoint[X]-center[X])/halfSize[X],
		               max(fabs(TPoint[Y]-center[Y])/halfSize[Y],
		                   fabs(TPoint[Z]-center[Z])/halfSize[Z]));
        	if(distance < 1)
        	{/*Clip to the component shape*/
        	    if(comp->inverse == TRUE)
        	    {
            		fieldVal = strength*eval_density_func(1-distance, falloff, comp->function);
        	    }
        	    else
        	    {
            		fieldVal = strength*eval_density_func(distance, falloff, comp->function);
        	    }
        	}
		}
		break;
	    case 3:
		{/*pigment component*/
		    COLOUR tempCol;
		    int colour_found;
		    
            if (comp->pigment)
            {
                colour_found = Compute_Pigment (tempCol, comp->pigment, TPoint, NULL);
            }

            if(!colour_found)
            {
                distance = 0.0;
            }
            else
            {
                distance = GREY_SCALE(tempCol);
            }
    	    if(comp->inverse == TRUE)
    	    {
        		fieldVal = strength*eval_density_func(1-distance, falloff, comp->function);
    	    }
    	    else
    	    {
        		fieldVal = strength*eval_density_func(distance, falloff, comp->function);
    	    }
		}
		break;
	    case 4:
		{/*blob component*/
		    fieldVal = calculate_blob_field_value((BLOB*)comp->blob, TPoint);
/*    	    if(comp->inverse == TRUE)
    	    {
        		fieldVal = 1-fieldVal;
    	    }*/
		}
		break;
	    default:
		{/*spherical component*/
			VDist(distance, TPoint, comp->center);
        	if(distance < radius)/*Clip to the component shape*/
        	{
        		fieldVal = strength*eval_density_func(distance/radius, falloff, comp->function);
        	}
		}
	}
    return fieldVal;
}
static DBL blob_pattern (VECTOR EPoint, TPATTERN *TPat)
{
	DBL totalVal = 0;
	BLOB_PATTERN_DATA * currentComponent = NULL;
	DBL max_density = TPat->Vals.Blob.max_density;
	DBL threshold = TPat->Vals.Blob.blob_threshold;
	
	if(TPat->Vals.Blob.blob_dat == NULL)
		return 0;/*If the list of components is empty, return 0*/
	
	currentComponent = TPat->Vals.Blob.blob_dat;
	while(currentComponent != NULL)
	{
		totalVal += blob_comp_strength(EPoint, currentComponent);
		currentComponent = currentComponent->next;
	}
	
	
	if(totalVal < threshold) totalVal = threshold;
	else if(totalVal > max_density) totalVal = max_density;
	
	if((max_density - threshold) != 0 && (totalVal - threshold) != 0)
		totalVal = (totalVal - threshold)/(max_density - threshold);
    else
        totalVal = 0;
	
	return totalVal;
}
/*Chris Huff proximity and blob patterns*/
DBL eval_density_func(DBL val, DBL falloff, int func)
{
	DBL temp = 0;
	switch(func)/*decide on density function and calculate the density*/
	{
		case 0:
			if(falloff == 1)
				temp = 1 - val;
			else
			{
				if(falloff == 2)
					temp = Sqr(1 - Sqr(val));
				else
					temp = pow(1 - pow(val, falloff), falloff);
			}
			break;
		case 1:
			if(falloff == 1)
				temp = val;
			else
				if(falloff == 2)
					temp = 1/Sqr(val);
				else
					temp = 1/pow(val, falloff);
			break;
		case 2:
			if(falloff == 1)
				temp = 1 - val;
			else
				if(falloff == 2)
					temp = 1 - Sqr(val);
				else
					temp = 1 - pow(val, falloff);
			break;
		case 3:
			if(falloff == 1)
				temp = val;
			else
				if(falloff == 2)
					temp = Sqr(val);
				else
					temp = pow(val, falloff);
			break;
		case 4:/*No filter*/
			temp = val;
			break;
		default:
		temp = Sqr(1 - Sqr(val));
	}
	return temp;
}
#endif

#ifdef ProximityPatch
static DBL proximity(VECTOR EPoint, TPATTERN *TPat)
{
   INTERSECTION Intersect;
   RAY Ray;
   int k=0;
   int attempts=0;
   int insideObjBBox=0;
   DBL temp = 0;
   VECTOR tempVect;
   VECTOR resVect;
   DBL result = 0;
   DBL x,y,z,r,t;
   VECTOR minExtent;
   VECTOR maxExtent;
   Assign_Vector(Ray.Initial, EPoint);
   if(Test_Flag(TPat->Vals.Proximity.proxObject, INFINITE_FLAG))
       return 0;

   Make_min_max_from_BBox(minExtent, maxExtent, TPat->Vals.Proximity.proxObject->BBox)
   if( (EPoint[X] > minExtent[X])&&(EPoint[Y] > minExtent[Y])&&(EPoint[Z] > minExtent[Z])&&
      (EPoint[X] < maxExtent[X])&&(EPoint[Y] < maxExtent[Y])&&(EPoint[Z] < maxExtent[Z]))
   {
     insideObjBBox = 1;
   }
   
   Make_Vector(tempVect,0,0,0);
   Make_Vector(resVect,0,0,0);
   Initialize_Ray_Containers( &Ray );
   
   /*detect whether the point is inside the object or not*/
   if(Inside_Object(EPoint, TPat->Vals.Proximity.proxObject))
   {
	   	if(!((TPat->Vals.Proximity.sides == 0)||(TPat->Vals.Proximity.sides == 2))) 
   			return 0;
   }
   else 
   {
	   	if(!((TPat->Vals.Proximity.sides == 1)||(TPat->Vals.Proximity.sides == 2))) 
   			return 0;
   }
   while((k<TPat->Vals.Proximity.samples)&&(attempts < TPat->Vals.Proximity.sample_bailout))
   {
       attempts++;
       switch(TPat->Vals.Proximity.sampleMthd)
       {
           case 0:
           {
           /*pick a random vector in the bounding box of the sample object*/
               x = FRAND()*TPat->Vals.Proximity.proxObject->BBox.Lengths[X];
               x += TPat->Vals.Proximity.proxObject->BBox.Lower_Left[X];
               y = FRAND()*TPat->Vals.Proximity.proxObject->BBox.Lengths[Y];
               y += TPat->Vals.Proximity.proxObject->BBox.Lower_Left[Y];
               z = FRAND()*TPat->Vals.Proximity.proxObject->BBox.Lengths[Z];
               z += TPat->Vals.Proximity.proxObject->BBox.Lower_Left[Z];
               Make_Vector( Ray.Direction, x, y, z);
               VSubEq(Ray.Direction, EPoint);
           } break;
           
           case 1:
           {
           /* Pick a random vector with length <= 1 using the "trig method"
             described in the comp.graphics.algorithms FAQ. 
             Chris Huff: I got this code from the WyzPov reflection blur*/
               z = (FRAND() * 2) - 1;
               t = FRAND() * M_PI * 2;
               r = sqrt(1 - z*z);
               x = r * cos(t);
               y = r * sin(t);
               Make_Vector( Ray.Direction, x, y, z);
           } break;
           
           
           case 2:
           if(insideObjBBox)
           {
           /* Pick a random vector with length <= 1 using the "trig method"
             described in the comp.graphics.algorithms FAQ. 
             Chris Huff: I got this code from the WyzPov reflection blur*/
               z = (FRAND() * 2) - 1;
               t = FRAND() * M_PI * 2;
               r = sqrt(1 - z*z);
               x = r * cos(t);
               y = r * sin(t);
               Make_Vector( Ray.Direction, x, y, z);
           }
           else
           {
           /*pick a random vector in the bounding box of the sample object*/
               x = FRAND()*TPat->Vals.Proximity.proxObject->BBox.Lengths[X];
               x += TPat->Vals.Proximity.proxObject->BBox.Lower_Left[X];
               y = FRAND()*TPat->Vals.Proximity.proxObject->BBox.Lengths[Y];
               y += TPat->Vals.Proximity.proxObject->BBox.Lower_Left[Y];
               z = FRAND()*TPat->Vals.Proximity.proxObject->BBox.Lengths[Z];
               z += TPat->Vals.Proximity.proxObject->BBox.Lower_Left[Z];
               Make_Vector( Ray.Direction, x, y, z);
               VSubEq(Ray.Direction, EPoint);
           } break;
           case 3:/*planar sampling*/
           {
               z = (FRAND() * 2) - 1;
               t = FRAND() * M_PI * 2;
               r = sqrt(1 - z*z);
               x = r * cos(t);
               y = r * sin(t);
               Make_Vector( Ray.Direction, x, y, 0);   
           } break;
       }
       
       VAddEq(Ray.Direction, TPat->Vals.Proximity.sampleWeight);
       VNormalizeEq(Ray.Direction);
       
       if( Intersection( &Intersect, TPat->Vals.Proximity.proxObject, &Ray ) )
       {
/*	       VDist(temp, EPoint, Intersect.IPoint);*/
	       switch(TPat->Vals.Proximity.proxCalcMthd)
	       {
	           case 0:
	               VDist(temp, EPoint, Intersect.IPoint);
	               result += temp;
	           break;
	           case 1:
	           {
	               VDist(temp, EPoint, Intersect.IPoint);
	               if((temp < result)||(result == 0))/*the last part is necessary because result == 0 at first*/
	                   result = temp;
	           }
	           break;
	           case 2:
	           {
	               VSub(tempVect, Intersect.IPoint, EPoint);
	               VAddEq(resVect, tempVect);
	           }
	           break;

	           
	           case 3:
	               VDist(temp, EPoint, Intersect.IPoint);
	               result += 1/(1+pow(temp, TPat->Vals.Proximity.falloff));
	           break;
	           
	           default:
	           result += temp;
	       }
	       k++;
	   }
   }
   
   switch(TPat->Vals.Proximity.proxCalcMthd)
   {
       case 0:
	       result /= k;/*divide by total samples to get average*/
       break;
       case 1:
         {;}
       break;
       case 2:
         VLength(result, resVect);
	       break;
       case 3:
	       result /= k;/*divide by total to get average*/
	       break;
       
       default:
         result /= k;/*divide by total to get average*/
   }

   
   result /= TPat->Vals.Proximity.proxDist;
   if(result>TPat->Vals.Proximity.max_density)
     result = TPat->Vals.Proximity.max_density;
	
   return result;
}

#endif


#ifdef ObjectPatternPatch
static DBL object(VECTOR EPoint, TPATTERN *TPat)
{
   if(TPat->Vals.Object != NULL)
   {
       if(Inside_Object(EPoint, TPat->Vals.Object))
        return 1.0;
       else
        return 0.0;
   }
   else
    return 0.0;
}

/*
 * The Following Is Not Needed For OpenVMS.
 */
#if !defined(VMS) || !defined(DECC)
#  pragma peephole off 
#endif

#endif
