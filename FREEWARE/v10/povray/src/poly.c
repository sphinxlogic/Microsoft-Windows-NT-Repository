/****************************************************************************
*                poly.c
*
*  This module implements the code for general 3 variable polynomial shapes
*
*  This file was written by Alexander Enzmann.  He wrote the code for
*  4th - 6th order shapes and generously provided us these enhancements.
*
*  from Persistence of Vision Raytracer
*  Copyright 1993 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other 
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file. If 
*  POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by leaving a message in CompuServe's Graphics Developer's
*  Forum.  The latest version of POV-Ray may be found there as well.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
*****************************************************************************/

#include "frame.h"
#include "vector.h"
#include "povproto.h"

/* Basic form of a quartic equation
   a00*x^4+a01*x^3*y+a02*x^3*z+a03*x^3+a04*x^2*y^2+
   a05*x^2*y*z+a06*x^2*y+a07*x^2*z^2+a08*x^2*z+a09*x^2+
   a10*x*y^3+a11*x*y^2*z+a12*x*y^2+a13*x*y*z^2+a14*x*y*z+
   a15*x*y+a16*x*z^3+a17*x*z^2+a18*x*z+a19*x+a20*y^4+
   a21*y^3*z+a22*y^3+a23*y^2*z^2+a24*y^2*z+a25*y^2+a26*y*z^3+
   a27*y*z^2+a28*y*z+a29*y+a30*z^4+a31*z^3+a32*z^2+a33*z+a34
*/

#define POLYNOMIAL_TOLERANCE 1.0e-4
#define COEFF_LIMIT 1.0e-20
#define BINOMSIZE 40

/* The following table contains the binomial coefficients up to 15 */
int binomials[15][15] =
  {
    {  
    1,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0
  }
  ,
    {  
    1,  1,  0,  0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0
  }
  ,
    {  
    1,  2,  1,  0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0
  }
  ,
    {  
    1,  3,  3,  1,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0
  }
  ,
    {  
    1,  4,  6,  4,   1,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0
  }
  ,
    {  
    1,  5, 10, 10,   5,   1,   0,   0,   0,   0,   0,  0,  0,  0,  0
  }
  ,
    {  
    1,  6, 15, 20,  15,   6,   1,   0,   0,   0,   0,  0,  0,  0,  0
  }
  ,
    {  
    1,  7, 21, 35,  35,  21,   7,   1,   0,   0,   0,  0,  0,  0,  0
  }
  ,
    {  
    1,  8, 28, 56,  70,  56,  28,   8,   1,   0,   0,  0,  0,  0,  0
  }
  ,
    {  
    1,  9, 36, 84, 126, 126,  84,  36,   9,   1,   0,  0,  0,  0,  0
  }
  ,
    {  
    1, 10, 45,120, 210, 252, 210, 120,  45,  10,   1,  0,  0,  0,  0
  }
  ,
    {  
    1, 11, 55,165, 330, 462, 462, 330, 165,  55,  11,  1,  0,  0,  0
  }
  ,
    {  
    1, 12, 66,220, 495, 792, 924, 792, 495, 220,  66, 12,  1,  0,  0
  }
  ,
    {  
    1, 13, 78,286, 715,1287,1716,1716,1287, 715, 286, 78, 13,  1,  0
  }
  ,
    {  
    1, 14, 91,364,1001,2002,3003,3432,3003,2002,1001,364, 91, 14,  1
  }
  };

DBL eqn_v[3][MAX_ORDER+1], eqn_vt[3][MAX_ORDER+1];

METHODS Poly_Methods =
  { 
  All_Poly_Intersections,
  Inside_Poly, Poly_Normal, Copy_Poly,
  Translate_Poly, Rotate_Poly,
  Scale_Poly, Transform_Poly, Invert_Poly, Destroy_Poly
};

extern long Ray_Poly_Tests, Ray_Poly_Tests_Succeeded;
extern unsigned int Options;
extern int Shadow_Test_Flag;

/* unused
static DBL evaluate_linear PARAMS((VECTOR *P, DBL *a));
static DBL evaluate_quadratic PARAMS((VECTOR *P, DBL *a));
*/

static int intersect PARAMS((RAY *Ray, int Order, DBL *Coeffs, int Sturm_Flag,
DBL *Depths));
static void normal0 PARAMS((VECTOR *Result, int Order, DBL *Coeffs,
VECTOR *IPoint));
static void normal1 PARAMS((VECTOR *Result, int Order, DBL *Coeffs,
VECTOR *IPoint));
static DBL inside PARAMS((VECTOR *IPoint, int Order, DBL *Coeffs));
static int intersect_linear PARAMS((RAY *ray, DBL *Coeffs, DBL *Depths));
static int intersect_quadratic PARAMS((RAY *ray, DBL *Coeffs, DBL *Depths));
static int factor_out PARAMS((int n, int i, int *c, int *s));
static long binomial PARAMS((int n, int r));
static void factor1 PARAMS((int n, int *c, int *s));

int All_Poly_Intersections(Object, Ray, Depth_Stack)
OBJECT *Object;
RAY *Ray;
ISTACK *Depth_Stack;
  {
  POLY *Poly = (POLY *) Object;
  DBL Depths[MAX_ORDER], len;
  VECTOR IPoint, dv;
  int cnt, i, j, Intersection_Found;
  RAY New_Ray;

  /* Transform the ray into the polynomial's space */
  if (Poly->Trans != NULL) 
    {
    MInvTransPoint(&New_Ray.Initial, &Ray->Initial, Poly->Trans);
    MInvTransDirection(&New_Ray.Direction, &Ray->Direction, Poly->Trans);
    }
  else 
    {
    New_Ray.Initial   = Ray->Initial;
    New_Ray.Direction = Ray->Direction;
    }

  VDot(len, New_Ray.Direction, New_Ray.Direction);
  if (len == 0.0)
    return 0;
  len = 1.0 / sqrt(len);
  VScaleEq(New_Ray.Direction, len);

  Intersection_Found = FALSE;
  Ray_Poly_Tests++;

  if (Poly->Order == 1)
    cnt = intersect_linear(&New_Ray, Poly->Coeffs, Depths);
  else if (Poly->Order == 2)
    cnt = intersect_quadratic(&New_Ray, Poly->Coeffs, Depths);
  else
    cnt = intersect(&New_Ray, Poly->Order, Poly->Coeffs, Poly->Sturm_Flag,
      Depths);
  if (cnt > 0) Ray_Poly_Tests_Succeeded++;

  for (i=0;i<cnt;i++) 
    {
    if (Depths[i] < POLYNOMIAL_TOLERANCE) goto l0;
    for (j=0;j<i;j++)
      if (Depths[i] == Depths[j]) goto l0;
    VScale(IPoint, New_Ray.Direction, Depths[i]);
    VAddEq(IPoint, New_Ray.Initial);
    /* Transform the point into world space */
    if (Poly->Trans != NULL)
      MTransPoint(&IPoint, &IPoint, Poly->Trans);

    VSub(dv, IPoint, Ray->Initial);
    VLength(len, dv);
    if (Point_In_Clip(&IPoint, Object->Clip))
      {
      push_entry(len,IPoint,Object,Depth_Stack);
      Intersection_Found = TRUE;
      }
l0:;
    }
  return (Intersection_Found);
  }

/* For speedup of low order polynomials, expand out the terms
   involved in evaluating the poly. */
/* unused
static DBL
evaluate_linear(P, a)
   VECTOR *P;
   DBL *a;
{
   return (a[0] * P->x) + (a[1] * P->y) + (a[2] * P->z) + a[3];
}

static DBL
evaluate_quadratic(P, a)
   VECTOR *P;
   DBL *a;
{
   DBL x, y, z;

   x = P->x; y = P->y; z = P->z;
   return  a[0] * x * x + a[1] * x * y + a[2] * x * z +
           a[3] * x     + a[4] * y * y + a[5] * y * z +
           a[6] * y     + a[7] * z * z + a[8] * z     +
           a[9];
}
*/
/* Remove all factors of i from n. */
static int
factor_out(n, i, c, s)
int n, i, *c, *s;
  {
  while (!(n % i)) 
    {
    n /= i;
    s[(*c)++] = i;
    }
  return n;
  }

/* Find all prime factors of n. (Note that n must be less than 2^15 */
static void
factor1(n, c, s)
int n, *c, *s;
  {
  int i,k;
  /* First factor out any 2s */
  n = factor_out(n, 2, c, s);
  /* Now any odd factors */
  k = (int)sqrt(n) + 1;
  for (i=3;n>1 && i<=k;i+=2)
    if (!(n%i)) 
    {
    n = factor_out(n, i, c, s);
    k = (int)sqrt(n)+1;
    }
  if (n>1)
    s[(*c)++] = n;
  }

/* Calculate the binomial coefficent of n,r. */
static
long
binomial(n, r)
int n, r;
  {
  int h,i,j,k,l;
  unsigned long result;
  static int stack1[BINOMSIZE], stack2[BINOMSIZE];
  if (n<0 || r<0 || r>n)
    result = 0L;
  else if (r==n)
    result = 1L;
  else if (r < 15 && n < 15)
    result = (long)binomials[n][r];
  else 
    {
    j = 0;
    for (i=r+1;i<=n;i++)
      stack1[j++] = i;
    for (i=2;i<=(n-r);i++) 
      {
      h = 0;
      factor1(i, &h, stack2);
      for (k=0;k<h;k++) 
        {
        for (l=0;l<j;l++)
          if (!(stack1[l] % stack2[k])) 
          {
          stack1[l] /= stack2[k];
          goto l1;
          }
        /* Error if we get here */
        if (Options & DEBUGGING) 
          {
          printf("Failed to factor\n");
          fflush(stdout);
          }
l1:;
        }
      }
    result=1;
    for (i=0;i<j;i++)
      result *= stack1[i];
    }
  return result;
  }

static DBL
inside(IPoint, Order, Coeffs)
VECTOR *IPoint;
int Order;
DBL *Coeffs;
  {
  DBL x[MAX_ORDER+1], y[MAX_ORDER+1];
  DBL z[MAX_ORDER+1], c, Result;
  int i, j, k, term;

  x[0] = 1.0;       y[0] = 1.0;       z[0] = 1.0;
  x[1] = IPoint->x; y[1] = IPoint->y; z[1] = IPoint->z;
  for (i=2;i<=Order;i++) 
    {
    x[i] = x[1] * x[i-1];
    y[i] = y[1] * y[i-1];
    z[i] = z[1] * z[i-1];
    }
  Result = 0.0;
  term = 0;
  for (i=Order;i>=0;i--)
    for (j=Order-i;j>=0;j--)
    for (k=Order-(i+j);k>=0;k--) 
    {
    if ((c = Coeffs[term]) != 0.0)
      Result += c * x[i] * y[j] * z[k];
    term++;
    }
  return Result;
  }

/* Intersection of a ray and an arbitrary polynomial function */
static int
intersect(ray, Order, Coeffs, Sturm_Flag, Depths)
RAY *ray;
int Order, Sturm_Flag;
DBL *Coeffs, *Depths;
  {
  DBL eqn[MAX_ORDER+1];
  DBL t[3][MAX_ORDER+1];
  VECTOR P, D;
  DBL val;
  int h, i, j, k, i1, j1, k1, term;
  int offset;

  /* First we calculate the values of the individual powers
      of x, y, and z as they are represented by the ray */
  P = ray->Initial;
  D = ray->Direction;
  for (i=0;i<3;i++) 
    {
    eqn_v[i][0]  = 1.0;
    eqn_vt[i][0] = 1.0;
    }
  eqn_v[0][1] = P.x;
  eqn_v[1][1] = P.y;
  eqn_v[2][1] = P.z;
  eqn_vt[0][1] = D.x;
  eqn_vt[1][1] = D.y;
  eqn_vt[2][1] = D.z;

  for (i=2;i<=Order;i++)
    for (j=0;j<3;j++) 
    {
    eqn_v[j][i]  = eqn_v[j][1] * eqn_v[j][i-1];
    eqn_vt[j][i] = eqn_vt[j][1] * eqn_vt[j][i-1];
    }

  for (i=0;i<=Order;i++)
    eqn[i] = 0.0;

  /* Now walk through the terms of the polynomial.  As we go
      we substitute the ray equation for each of the variables. */
  term = 0;
  for (i=Order;i>=0;i--) 
    {
    for (h=0;h<=i;h++)
      t[0][h] = binomial(i, h) *
      eqn_vt[0][i-h] *
      eqn_v[0][h];
    for (j=Order-i;j>=0;j--) 
      {
      for (h=0;h<=j;h++)
        t[1][h] = binomial(j, h) *
        eqn_vt[1][j-h] *
        eqn_v[1][h];
      for (k=Order-(i+j);k>=0;k--) 
        {
        if (Coeffs[term] != 0) 
          {
          for (h=0;h<=k;h++)
            t[2][h] = binomial(k, h) *
            eqn_vt[2][k-h] *
            eqn_v[2][h];

          /* Multiply together the three polynomials */
          offset = Order - (i + j + k);
          for (i1=0;i1<=i;i1++)
            for (j1=0;j1<=j;j1++)
            for (k1=0;k1<=k;k1++) 
            {
            h = offset + i1 + j1 + k1;
            val = Coeffs[term];
            val *= t[0][i1];
            val *= t[1][j1];
            val *= t[2][k1];
            eqn[h] += val;
            }
          }
        term++;
        }
      }
    }
  for (i=0,j=Order;i<=Order;i++)
    if (eqn[i] != 0.0)
      break;
    else
      j--;

  if (j > 4 || Sturm_Flag)
    return polysolve(j, &eqn[i], Depths);
  else if (j == 4)
    return solve_quartic(&eqn[i], Depths);
  else if (j==3)
    return solve_cubic(&eqn[i], Depths);
  else if (j==2)
    return solve_quadratic(&eqn[i], Depths);
  else
    return 0;
  }

/* Intersection of a ray and a quadratic */
static int
intersect_linear(ray, Coeffs, Depths)
RAY *ray;
DBL *Coeffs, *Depths;
  {
  DBL t0, t1, *a = Coeffs;

  t0 = a[0] * ray->Initial.x + a[1] * ray->Initial.y + a[2] * ray->Initial.z;
  t1 = a[0] * ray->Direction.x + a[1] * ray->Direction.y +
  a[2] * ray->Direction.z;
  if (fabs(t1) < EPSILON)
    return 0;
  Depths[0] = -(a[3] + t0) / t1;
  return 1;
  }

/* Intersection of a ray and a quadratic */
static int
intersect_quadratic(ray, Coeffs, Depths)
RAY *ray;
DBL *Coeffs, *Depths;
  {
  DBL x, y, z, x2, y2, z2;
  DBL xx, yy, zz, xx2, yy2, zz2;
  DBL *a, ac, bc, cc, d, t;

  x  = ray->Initial.x;
  y  = ray->Initial.y;
  z  = ray->Initial.z;
  xx = ray->Direction.x;
  yy = ray->Direction.y;
  zz = ray->Direction.z;
  x2 = x * x;  y2 = y * y;  z2 = z * z;
  xx2 = xx * xx;  yy2 = yy * yy;  zz2 = zz * zz;
  a = Coeffs;

  /*
      Determine the coefficients of t^n, where the line is represented
      as (x,y,z) + (xx,yy,zz)*t.
   */
  ac = (a[0]*xx2 + a[1]*xx*yy + a[2]*xx*zz + a[4]*yy2 + a[5]*yy*zz +
    a[7]*zz2);
  bc = (2*a[0]*x*xx + a[1]*(x*yy + xx*y) + a[2]*(x*zz + xx*z) +
    a[3]*xx + 2*a[4]*y*yy + a[5]*(y*zz + yy*z) + a[6]*yy +
    2*a[7]*z*zz + a[8]*zz);
  cc = a[0]*x2 + a[1]*x*y + a[2]*x*z + a[3]*x + a[4]*y2 +
  a[5]*y*z + a[6]*y + a[7]*z2 + a[8]*z + a[9];

  if (fabs(ac) < COEFF_LIMIT) 
    {
    if (fabs(bc) < COEFF_LIMIT)
      return 0;
    Depths[0] = -cc / bc;
    return 1;
    }

  /* Solve the quadratic formula & return results that are
      within the correct interval. */
  d = bc * bc - 4.0 * ac * cc;
  if (d < 0.0) return 0;
  d = sqrt(d);
  bc = -bc;
  t = 2.0 * ac;
  Depths[0] = (bc + d) / t;
  Depths[1] = (bc - d) / t;
  return 2;
  }

/* Normal to a polynomial (used for polynomials with order > 4) */
static void normal0(Result, Order, Coeffs, IPoint)
VECTOR *Result;
int Order;
DBL *Coeffs;
VECTOR *IPoint;
  {
  int i, j, k, term;
  DBL val, *a, x[MAX_ORDER+1], y[MAX_ORDER+1], z[MAX_ORDER+1];

  x[0] = 1.0; y[0] = 1.0; z[0] = 1.0;
  x[1] = IPoint->x;
  y[1] = IPoint->y;
  z[1] = IPoint->z;
  for (i=2;i<=Order;i++) 
    {
    x[i] = IPoint->x * x[i-1];
    y[i] = IPoint->y * y[i-1];
    z[i] = IPoint->z * z[i-1];
    }
  a = Coeffs;
  term = 0;
  Make_Vector(Result, 0, 0, 0);
  for (i=Order;i>=0;i--)
    for (j=Order-i;j>=0;j--)
    for (k=Order-(i+j);k>=0;k--) 
    {
    if (i >= 1) 
      {
      val = x[i-1] * y[j] * z[k];
      Result->x += i * a[term] * val;
      }
    if (j >= 1) 
      {
      val = x[i] * y[j-1] * z[k];
      Result->y += j * a[term] * val;
      }
    if (k >= 1) 
      {
      val = x[i] * y[j] * z[k-1];
      Result->z += k * a[term] * val;
      }
    term++;
    }
  }

/* Normal to a polynomial (for polynomials of order <= 4) */
static void
normal1(Result, Order, Coeffs, IPoint)
VECTOR *Result;
int Order;
DBL *Coeffs;
VECTOR *IPoint;
  {
  DBL *a, x, y, z, x2, y2, z2, x3, y3, z3;

  a = Coeffs;
  x = IPoint->x;
  y = IPoint->y;
  z = IPoint->z;

  if (Order == 1)
    /* Linear partial derivatives */
    Make_Vector(Result, a[0], a[1], a[2])
else if (Order == 2) 
  {
  /* Quadratic partial derivatives */
  Result->x = 2*a[0]*x+a[1]*y+a[2]*z+a[3];
  Result->y = a[1]*x+2*a[4]*y+a[5]*z+a[6];
  Result->z = a[2]*x+a[5]*y+2*a[7]*z+a[8];
  }
else if (Order == 3) 
  {
  x2 = x * x;  y2 = y * y;  z2 = z * z;
  /* Cubic partial derivatives */
  Result->x = 3*a[0]*x2 + 2*x*(a[1]*y + a[2]*z + a[3]) + a[4]*y2 +
  y*(a[5]*z + a[6]) + a[7]*z2 + a[8]*z + a[9];
  Result->y = a[1]*x2 + x*(2*a[4]*y + a[5]*z + a[6]) + 3*a[10]*y2 +
  2*y*(a[11]*z + a[12]) + a[13]*z2 + a[14]*z + a[15];
  Result->z = a[2]*x2 + x*(a[5]*y + 2*a[7]*z + a[8]) + a[11]*y2 +
  y*(2*a[13]*z + a[14]) + 3*a[16]*z2 + 2*a[17]*z + a[18];
  }
else 
  {
  /* Quartic partial derivatives */
    x2 = x * x;  y2 = y * y;  z2 = z * z;
  x3 = x * x2; y3 = y * y2; z3 = z * z2;
  Result->x = 4*a[ 0]*x3+3*x2*(a[ 1]*y+a[ 2]*z+a[ 3])+
  2*x*(a[ 4]*y2+y*(a[ 5]*z+a[ 6])+a[ 7]*z2+a[ 8]*z+a[ 9])+
  a[10]*y3+y2*(a[11]*z+a[12])+y*(a[13]*z2+a[14]*z+a[15])+
  a[16]*z3+a[17]*z2+a[18]*z+a[19];
  Result->y = a[ 1]*x3+x2*(2*a[ 4]*y+a[ 5]*z+a[ 6])+
  x*(3*a[10]*y2+2*y*(a[11]*z+a[12])+a[13]*z2+a[14]*z+a[15])+
  4*a[20]*y3+3*y2*(a[21]*z+a[22])+2*y*(a[23]*z2+a[24]*z+a[25])+
  a[26]*z3+a[27]*z2+a[28]*z+a[29];
  Result->z = a[ 2]*x3+x2*(a[ 5]*y+2*a[ 7]*z+a[ 8])+
  x*(a[11]*y2+y*(2*a[13]*z+a[14])+3*a[16]*z2+2*a[17]*z+a[18])+
  a[21]*y3+y2*(2*a[23]*z+a[24])+y*(3*a[26]*z2+2*a[27]*z+a[28])+
  4*a[30]*z3+3*a[31]*z2+2*a[32]*z+a[33];
  }
}

int Inside_Poly (IPoint, Object)
VECTOR *IPoint;
OBJECT *Object;
{
VECTOR New_Point;
DBL Result;

/* Transform the point into polynomial's space */
if (((POLY *)Object)->Trans != NULL)
MInvTransPoint(&New_Point, IPoint, ((POLY *)Object)->Trans);
else
  New_Point = *IPoint;

Result = inside(&New_Point, ((POLY *)Object)->Order, ((POLY *)Object)->Coeffs);
if (Result < Small_Tolerance)
return ((int)(1-((POLY *)Object)->Inverted));
else
  return ((int)((POLY *)Object)->Inverted);
}

/* Normal to a polynomial */
void Poly_Normal(Result, Object, IPoint)
VECTOR *Result, *IPoint;
OBJECT *Object;
{
DBL val;
VECTOR New_Point;
POLY *Shape = (POLY *)Object;

/* Transform the point into the polynomials space */
if (Shape->Trans != NULL)
MInvTransPoint(&New_Point, IPoint, Shape->Trans);
else
  New_Point = *IPoint;

if (((POLY *)Object)->Order > 4)
normal0(Result, Shape->Order, Shape->Coeffs, &New_Point);
else
  normal1(Result, Shape->Order, Shape->Coeffs, &New_Point);

/* Transform back to world space */
if (Shape->Trans != NULL)
MTransNormal(Result, Result, Shape->Trans);

/* Normalize (accounting for the possibility of a 0 length normal) */
VDot(val, *Result, *Result);
if (val > 0.0) 
  {
  val = 1.0 / sqrt(val);
  VScaleEq(*Result, val);
  }
else
  Make_Vector(Result, 1, 0, 0)
    }

  /* Make a copy of a polynomial object */
  void *Copy_Poly(Object)
    OBJECT *Object;
{
POLY *New;
int i;

New = Create_Poly (((POLY *)Object)->Order);
New->Sturm_Flag = ((POLY *)Object)->Sturm_Flag;
New->Inverted = ((POLY *)Object)->Inverted;

New->Trans = Copy_Transform(((POLY *)Object)->Trans);

for (i=0;i<term_counts(New->Order);i++)
New->Coeffs[i] = ((POLY *)Object)->Coeffs[i];
return (New);
}

void Translate_Poly (Object, Vector)
OBJECT *Object;
VECTOR *Vector;
{
TRANSFORM Trans;
Compute_Translation_Transform(&Trans, Vector);
Transform_Poly(Object, &Trans);
}

void Rotate_Poly (Object, Vector)
OBJECT *Object;
VECTOR *Vector;
{
TRANSFORM Trans;
Compute_Rotation_Transform(&Trans, Vector);
Transform_Poly(Object, &Trans);
}

void Scale_Poly (Object, Vector)
OBJECT *Object;
VECTOR *Vector;
{
TRANSFORM Trans;
Compute_Scaling_Transform(&Trans, Vector);
Transform_Poly(Object, &Trans);
}

void Transform_Poly(Object,Trans)
OBJECT *Object;
TRANSFORM *Trans;
{
if (((POLY *)Object)->Trans == NULL)
((POLY *)Object)->Trans = Create_Transform();
recompute_bbox(&Object->Bounds, Trans);
Compose_Transforms(((POLY *)Object)->Trans, Trans);
}

void Invert_Poly(Object)
OBJECT *Object;
{
((POLY *) Object)->Inverted = 1 - ((POLY *)Object)->Inverted;
}

POLY *Create_Poly(Order)
int Order;
{
POLY *New;
int i;

if ((New = (POLY *) malloc (sizeof (POLY))) == NULL)
MAError ("poly");

INIT_OBJECT_FIELDS(New,POLY_OBJECT, &Poly_Methods);
New->Order = Order;
New->Sturm_Flag = FALSE;
New->Trans = NULL;
New->Inverted = FALSE;
New->Coeffs = (DBL *)malloc(term_counts(Order) * sizeof(DBL));
if (New->Coeffs == NULL)
MAError("coefficients for POLY");
for (i=0;i<term_counts(Order);i++)
New->Coeffs[i] = 0.0;

return (New);
}

void Destroy_Poly(Object)
OBJECT *Object;
{
Destroy_Transform (((POLY *)Object)->Trans);
free (((POLY *)Object)->Coeffs);
free (Object);
}

