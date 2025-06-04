/****************************************************************************
*		isofunc.c -- internal functions for iso surfaces
*
*	This module calculates internal functions for POVISO patch.
*	By Ryoichi SUZUKI, rsuzuki@etl.go.jp.
*
*****************************************************************************/

/*
 * Modification For OpenVMS By Robert Alan Byer <byer@mail.ourservers.net>
 * Feb. 13, 2001
 */

#include "frame.h"
#include "isofunc.h"
#include "isosrf.h"
#include "vector.h"		
#include "povproto.h"
#include "f_expr.h"	
#include "texture.h" /* Added MCB 01-06-00 */

#ifdef IsoPigmentPatch
#include "pigment.h" /* Added MCB 12-28-98 */
#endif
#define F3D(x,y,z) (f3d[x+y*ixmax+z*ixmax*iymax+3])
#define zmax(i,imax) (((i)<0)?(imax-1):((i) % (imax))) 
static float intpd2[4][4];

#define x XYZ[X]
#define y XYZ[Y]
#define z XYZ[Z]

#ifdef IsoPigmentPatch

static DBL iso_pigment(FUNCTION *Func, VECTOR XYZ) /* Added MCB 12-28-98 */
{
	COLOUR Col;
	VECTOR v1;
	if (Func->Pigment == NULL) 
		return 0.0;
	Assign_Vector(v1,XYZ);
	Compute_Pigment(Col,Func->Pigment,v1,NULL); /* last Entry Null cause its Intersections for slope based */
	return ((Col[RED]+Col[GREEN]/255.)*0.996093);
}
#endif

static DBL imp_func(FUNCTION *Func, VECTOR XYZ)
{
	Assign_Vector(func_XYZ,XYZ);
	evaluate_function( Func );
	return(*calc_stack);
}

static DBL R(FUNCTION *Func,VECTOR XYZ)
{
	return( sqrt(x*x + y*y + z*z ) );	
}

static DBL TH(FUNCTION *Func,VECTOR XYZ)
{
	return( atan2(x,z) );	
}

static DBL PH(FUNCTION *Func,VECTOR XYZ)
{
	return( atan2(sqrt(x*x + z*z ),y) );	
}

static DBL sphere(FUNCTION *Func,VECTOR XYZ)
{
	return( -P0+sqrt(x*x + y*y + z*z ) );	
}


static DBL rounded_box(FUNCTION *Func,VECTOR XYZ)
{
	DBL x2,y2,z2,x3,y3,z3;
	if (Func->isosf) 
	{
		x2=Func->isosf->bounds[1][X] -P0;
		y2=Func->isosf->bounds[1][Y] -P0;
		z2=Func->isosf->bounds[1][Z] -P0;
		x3= (x<x2)? 0 : (x-x2);
		y3= (y<y2)? 0 : (y-y2);
		z3= (z<z2)? 0 : (z-z2);
		x2=Func->isosf->bounds[0][X] +P0;
		y2=Func->isosf->bounds[0][Y] +P0;
		z2=Func->isosf->bounds[0][Z] +P0;
		x= (x>x2)? 0: (x2-x);
		y= (y>y2)? 0: (y2-y);
		z= (z>z2)? 0: (z2-z);
		x=max(x,x3); y=max(y,y3); z=max(z,z3);
	} 
	else 
	{
		x2 = P1 -P0;
		y2= P2 -P0;
		z2= P3 -P0;
		x3= (x<x2)? 0 : (x-x2);					
		y3= (y<y2)? 0 : (y-y2);
		z3= (z<z2)? 0 : (z-z2);
		x2=P0-P1;
		y2=P0-P2;
		z2=P0-P3;
		x= (x>x2)? 0: (x2-x);						
		y= (y>y2)? 0: (y2-y);
		z= (z>z2)? 0: (z2-z);
		x=max(x,x3); y=max(y,y3); z=max(z,z3);
	}
	return( -P0+ sqrt(x*x+y*y+z*z)-1e-6);
}

static DBL torus(FUNCTION *Func,VECTOR XYZ)
{
	x=sqrt(x*x+z*z)-P0;
	return -P1+sqrt(x*x+y*y);
}

	/*	Superquadric Ellipsoid	*/
static DBL superellipsoid(FUNCTION *Func,VECTOR XYZ)
{
	DBL p=2/P0, n=1/P1;
	return 1- pow((pow( (pow(fabs(x), p) + pow(fabs(y),p)), P0*n) + pow(fabs(z),2*n)),P1*.5);
}


static DBL helix1(FUNCTION *Func,VECTOR XYZ)
{
	DBL r,r2,temp,th,ph,x2;
	/* helix														*
	*	 p[0] : number of helix							*
	*	 p[1] : frequency										*
	*	 p[2] : minor radius									*
	*	 p[3] : major radius									*
	*	 p[4] : shape parameter							*
	*	 p[5] : cross section								*
	*	 p[5] 	= 1:	circle						 				*
	*				= 2:	diamond								*
	*				< 1:	rectangle(rounded) 				*
	*	 p[6] : rotation angle for p[5]<1				*/

	r=sqrt(x*x+z*z);
	if ((x==0)&&(z==0)) 
		x=0.000001;
	th=atan2(z,x);
	th=fmod(th*P0+y*P1*P0, M_2PI);
	if (th<0) 
		th+=M_2PI;
	z=(th-M_PI)/P4/(P1*P0);

	x=r-P3;
	if (P5==1)
		r2=sqrt(x*x+z*z);
	else
	{
		if (P6!=0)	
		{	
			th=cos(P6*M_PI180); ph=sin(P6*M_PI180);
			x2=x*th-z*ph;
			z=x*ph+z*th;
			x=x2;
		}
		if (P5!=0)
		{
			temp= 2./P5;
			r2= pow( (pow(fabs(x),temp)+pow(fabs(z),temp)), P5*.5);
		}
		else 
			r2= max( fabs(x), fabs(z));
	} 
	return(-P2+min((P3+r),r2));
}

static DBL helix2(FUNCTION *Func,VECTOR XYZ)
{
	DBL th,ph,x2,z2,r2,temp;
	/* helical shape	for (minor radius>major radius	*
	*		cross section	 p[5] same as NFUNCTION = 6			*/
	th=y*P1;
	ph=cos(th);
	th=sin(th);
	x2=x-P3*ph;
	z2=z-P3*th;
	x=x2*ph+z2*th;
	z=(-x2*th+z2*ph);

	if (P5==1)	 
		return(sqrt(x*x+z*z) - P2 );
	if (P5!=0)
	{
		temp= 2./P5;
		r2= pow( (pow(fabs(x),temp)+pow(fabs(z),temp)), P5*.5);
	}
	else 
		r2= max( fabs(x), fabs(z));
	return(r2-P2);
}

static DBL spiral(FUNCTION *Func,VECTOR XYZ)
{
	DBL r,r2,th,temp;
	/* spiral shape								*
	*														 */
	r=sqrt(x*x+z*z);
	if ((x==0)&&(z==0)) 
		x=0.000001;
	th=atan2(z,x);
	r=r+P0*th/M_2PI;
	r2= fmod(r, P0)-P0*0.5;

	if (P5==1)
		r2= sqrt(r2*r2+y*y);
	else if (P5!=0)
	{
		temp= 2/P5;
		r2= pow( (pow(fabs(r2),temp)+pow(fabs(y),temp)), 1./temp);
	}
	else 
		r2= max( fabs(r2), fabs(y));

	r=sqrt(x*x+y*y+z*z);
	return(-min(P2-r,P1-min(r2,r)) );
}


static DBL mesh1(FUNCTION *Func,VECTOR XYZ)
{
	DBL th,ph,r,r2,temp;
	th=M_PI/P0;
	ph=M_PI/P1;
	r = fmod(x, P0*2); 
	if (r<0) 
		r+=P0*2;
	r= fabs(r-P0)*P2;
	r2= (y-cos(z*ph)*P3)*P4;
	temp= -sqrt( r2*r2 + r*r);

	r = fmod(x-P0, P0*2); 
	if (r<0) 
		r+=P0*2;
	r= fabs(r-P0)*P2;
	r2= (y+cos(z*ph)*P3)*P4;
	temp= max(-sqrt( r2*r2 + r*r), temp);

	r = fmod(z, P1*2); 
	if (r<0) 
		r+=P1*2;
	r= fabs(r-P1)*P2;
	r2= (y+cos(x*th)*P3)*P4;
	temp= max(-sqrt( r2*r2 + r*r), temp);

	r = fmod(z-P1, P1*2); 
	if (r<0) 
		r+=P1*2;
	r= fabs(r-P1)*P2;
	r2= (y-cos(x*th)*P3)*P4;
	return( -max(-sqrt( r2*r2 + r*r), temp));
}
/****************************************************************************
*								i_argbr.c			 Algebraic Surfaces
*
*	This module calculates potential function for "isosurface".
*	This file was written by Ryoichi SUZUKI, rsuzuki@etl.go.jp.
*	These functions are based on the following article written by 
*	Tore Nordstrand.
*		 Subject: Algebraic Surfaces (long)
*		 Date: 5 Feb 1996 06:54:57 GMT
*		 From: nfytn@alf.uib.no (Tore)
*		 Newsgroups: comp.graphics.rendering.raytracing
*
*****************************************************************************/

#define ROT2D(p,d,ang) if (p>0) {x2=sqrt(x2+z*z)- d; th=ang*M_PI180; \
		if (th!=0){ x= x2*cos(th)-y*sin(th); y= x2*sin(th)+y*cos(th);} else x=x2; \
		x2=x*x; y2=y*y;}

/*Algebraic cylinders */
static DBL Algbr_Cyl1(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, th;
	ROT2D(P2,P3,P4)
	x=fabs(x);
	r=-( x2*x - x2 +y2 );
	return(- min(P1, max(P0*r,-P1)) );
}

static DBL Algbr_Cyl2(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, th;
	ROT2D(P2,P3,P4)
	r=-( 2*x2*x2 -3*x2*y +y2 -2*y2*y +y2*y2 );
	return( min(P1, max(P0*r,-P1)) );
}

static DBL Algbr_Cyl3(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, th;
	ROT2D(P2,P3,P4)
	r=-( x2*x2 +x2*y2 -2*x2*y -x*y2 +y2 );
	return( min(P1, max(P0*r,-P1)) );
}

static DBL Algbr_Cyl4(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, th;
	ROT2D(P2,P3,P4)
	r=-( x2*x2 +y2*y2 +2*x2*y2 +3*x2*y -y2*y );
	return( min(P1, max(P0*r,-P1)) );
}



static DBL Bicorn(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, r2,x2=x*x, y2=y*y, z2=z*z;
	r = P1*P1; 
	r2=(x2 + z2 + 2*P1*y - r);
	r = ( y2*(r - (x2 + z2)) - r2*r2);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Bifolia(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, z2=z*z;
	r=x2+y*y+z2;
	r= -(r*r - P1*(x2 + z2)*y);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Boy_surface(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, r2,ph,x2=x*x, y2=y*y, z2=z*z;
	r2=1-z; ph=x2+y2;
	r= P1*(64*r2*r2*r2*z2*z- 48*r2*r2*z2*(3*x2+3*y2+2*z2)+
	12*r2*z*(27*ph*ph-24*z2*ph+ 36*sqrt(2)*y*z*(y2-3*x2)+4*z2*z2)+
	(9*x2+9*y2-2*z2)*(-81*ph*ph-72*z2*ph+ 
	108*sqrt(2)*x*z*(x2-3*y2)+4*z2*z2) );
	return( min(10., max(P0*r,-10.)) );
}

static DBL Ovals_of_Cassini(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, r2,x2=x*x, y2=y*y, z2=z*z;
	r2 = (x2 + y2 + z2 + P1*P1);
	r	= -(r2*r2 - P3*P1*P1*(x2 + z2) - P2*P2 );
	return( min(10., max(P0*r,-10.)) );
}

static DBL Cubic_saddle(FUNCTION *Func, VECTOR XYZ)
{
	DBL r;
	r=-(x*x*x - y*y*y - z);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Cushion(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, z2=z*z;
	r= (z2*x2 - z2*z2 - 2*z*x2 + 2*z2*z + x2 - z2
	-(x2 - z)*(x2 - z) - y2*y2 - 2*x2*y2 - y2*z2 + 2*y2*z + y2);			 
	return( min(10., max(P0*r,-10.)) );
}

static DBL Devils_curve(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, z2=z*z;
	r=-(x2*x2 + 2*x2*z2 - 0.36*x2 - y2*y2 + 0.25*y2 + z2*z2);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Devils_curve2D(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, th;
	ROT2D(P3,P4,P5)
	r=-( x2 * (x2 - P1*P1) -	y2 * (y2 - P2*P2) );
	return( min(10., max(P0*r,-10.)) );
}

static DBL Dupin_Cyclid(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, r2,x2=x*x, y2=y*y, z2=z*z, ph,th, p1,p2,p3,p4;
	r2=P5*P5;
	ph=P2*P2; th=P4*P4;
	p1= ph - th - (P3 + P1)*(P3 + P1);
	p2= ph - th - (P3 - P1)*(P3 - P1);
	p3= P3*P3; p4=P1*P1;
	r=-( p1*p2* (x2*x2+y2*y2+z2*z2)+ 2*(p1*p2* (x2*y2+x2*z2+y2*z2))+
	2*r2*((-th-p3 + ph+p4)* (2*x*P3+2*y*P4-r2)-4*P4*ph*y)*
	(x2+y2+z2)+ 4*r2*r2*(P3*x+P4*y)
	*(-r2+P4*y+P3*x)+ 4*r2*r2*p4*y2+ r2*r2*r2*r2);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Folium_surface(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, z2=z*z;
	r=((y2 + z2) * (1+(P2 - 4*P1)*x)+x2*(1 + P2));
	return( min(10., max(P0*r,-10.)) );
}

static DBL Folium_surface2D(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, z2=z*z, th;
	ROT2D(P3,P4,P5)
	r = -( y2	* (1 + (P2 - 4*P1)*x) + x2*(1 + P2));
	return( min(10., max(P0*r, -10.)) );
}

static DBL Torus_gumdrop(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, z2=z*z;
	r=-( 4*(x2*x2 + (y2 + z2)*(y2 + z2)) + 17 * x2 * (y2 + z2) -
	20 * (x2 + y2 + z2) + 17);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Hunt_surface(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, z2=z*z;
	r=-( 4*(x2+y2+z2-13)*(x2+y2+z2-13)*(x2+y2+z2-13) + 
	27*(3*x2+y2-4*z2-12)*(3*x2+y2-4*z2-12)	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Hyperbolic_torus(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, ph,th,x2=x*x, y2=y*y, z2=z*z;
	ph=P1*P1; th=P2*P2;
	r=-( x2*x2 + 2*x2*y2 - 2*x2*z2 - 2*(ph+th)*x2 + y2*y2 -
	2*y2*z2 + 2*(ph-th)*y2 + z2*z2 + 2*(ph+th)*z2 + (ph-th)*(ph-th));
	return( min(10., max(P0*r,-10.)) );
}

static DBL Kampyle_of_Eudoxus(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x;
	r=-( (y*y + z*z) - P2*P2 * x2*x2 +	P2*P2 * P1*P1 * x2	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Kampyle_of_Eudoxus2D(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, th;
	ROT2D(P3,P4,P5)
	r=-( y2	- P2*P2 * x2*x2 +		P2*P2 * P1*P2 * x2	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Klein_Bottle(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, z2=z*z;
	r=-( (x2+y2+z2+2*y-1)*((x2+y2+z2-2*y-1)*(x2+y2+z2-2*y-1)-8*z2)+
	16*x*z*(x2+y2+z2-2*y-1) );
	return( min(10., max(P0*r,-10.)) );
}

static DBL Kummer_Surface_V1(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, z2=z*z;
	r=-( x2*x2+y2*y2+z2*z2-x2-y2-z2-x2*y2-x2*z2-y2*z2+1	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL	Kummer_Surface_V2(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, z2=z*z;
	r=-( x2*x2+y2*y2+z2*z2+P1*(x2+y2+z2)+P2*(x2*y2+x2*z2+y2*z2)+P3*x*y*z-1 );
	return( min(10., max(P0*r,-10.)) );
}

static DBL Lemniscate_of_Gerono(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, z2=z*z;
	r=-(	x2*x2 - x2 + y2 + z2	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Lemniscate_of_Gerono2D(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, th;
	ROT2D(P3,P4,P5)
	r=-( y2 - P2*P2 * P1*P1 * x2 +	 P2*P2*x2*x2	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Paraboloid(FUNCTION *Func, VECTOR XYZ)
{
	DBL r;
	r=-( x*x - y + z*z);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Parabolic_Torus(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, ph,th, x2=x*x, y2=y*y, z2=z*z;
	ph=P1*P1; th=P2*P2;
	r=-(	x2*x2 + 2*x2*y2 - 2*x2*z - (ph+th)*x2 + y2*y2 - 2*y2*z +
	(ph-th)*y2 + z2 + (ph+th)*z +	(ph-th)* (ph-th)	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Piriform(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x;
	r=-( (x2*x2 - x2*x) + y*y + z*z	);
	return( -min(10., max(P0*r,-10.)) );
}

static DBL Piriform2D(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y,th;
	ROT2D(P4,P5,P6)
	r=-( y*y - P1 * P3* P3 * x2*x -	P2 * P3*P3 * x2*x2	);			 
	return( min(10., max(P0*r,-10.)) );
}

static DBL Quartic_paraboloid(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, z2=z*z;
	r=-( x2*x2 + z2*z2 - y	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Quartic_saddle(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, z2=z*z;
	r=-( x2*x2 - z2*z2 - y	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Quartic_Cylinder(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, z2=z*z;
	r=-( (x2 + z2) * y*y + P2*P2 * (x2 + z2) - P2*P2 * P1*P1	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Steiners_Roman(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, z2=z*z;
	r=-( x2*y2 + x2*z2 + y2*z2 + x*y*z	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Strophoid(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, r2,x2=x*x;
	r2=P3*P3;
	r=-((P2 - x)*(y*y + z*z) - r2*P1*x2 - r2*x2*x );
	return( min(10., max(P0*r,-10.)) );
}

static DBL Strophoid2D(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, r2,x2=x*x, y2=y*y, th;
	ROT2D(P4,P5,P6)
	r2=P3*P3;
	r=-( (P2 - x)*y2	- r2*P1*x2 -	r2*x2*x	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Glob(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x;
	r= ( 0.5*x2*x2*x + 0.5*x2*x2 - (y*y + z*z)	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Pillow(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, z2=z*z;
	r=( x2*x2 + y2*y2 + z2*z2 - (x2 + y2 + z2)	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Crossed_Trough(FUNCTION *Func, VECTOR XYZ)
{
	DBL r;
	r=( x*x * z*z - y	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Witch_of_Agnesi(FUNCTION *Func, VECTOR XYZ)
{
	DBL r;
	r=-( P1 * (y - 1) + (x*x + z*z) * y	 );
	return( min(10., max(P0*r,-10.)) );
}

static DBL Witch_of_Agnesi2D(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, th;
	ROT2D(P3,P4,P5)
	r=-( P1*P1 * y + x2 * y - P2	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Mitre(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, z2=z*z;
	r=-( 4*x2*(x2 + y2 + z2) - y2*(1 - y2 - z2)	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Odd(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, z2=z*z;
	r=( z2*x2 - z2*z2 - 2*z*x2 + 2*z2*z + x2 - z2 - 
	(x2 - z)*(x2 - z) - y2*y2 - 2*y2*x2 - y2*z2 + 2*y2*z + y2	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Heart(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, x2=x*x, y2=y*y, z2=z*z;
	r=-( (2*x2+y2+z2-1)*(2*x2+y2+z2-1)*(2*x2+y2+z2-1)-
	0.1*x2*z2*z-y2*z2*z	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Torus2(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, ph,th,x2=x*x, y2=y*y, z2=z*z;
	ph=P1*P1; th=P2*P2;
	r=-( x2*x2 + y2*y2 + z2*z2 + 2*x2*y2 + 2*x2*z2 + 2*y2*z2
	-2* (ph + th)* x2 + 2* (ph - th)* y2 -2* (ph + th)* z2 + 
	(ph - th)*(ph - th)	);
	return( min(10., max(P0*r,-10.)) );
}

static DBL Nodal_cubic(FUNCTION *Func, VECTOR XYZ)
{
	DBL r;
	r=-( y*y*y + z*z*z - 6*y*z );
	return( min(10., max(P0*r,-10.)) );
}

static DBL Umbrella(FUNCTION *Func, VECTOR XYZ)
{
	DBL r;
	r=-( x*x - y*z*z );
	return( min(10., max(P0*r,-10.)) );
}

static DBL Enneper(FUNCTION *Func, VECTOR XYZ)
{
	DBL r, r2,x2=x*x, y2=y*y, z2=z*z;
	if (fabs(z)<0.2) 
		z=0.2;
	r =((y2-x2)/(2*z)+2*z2/9+2/3);
	r2=((y2-x2)/(4*z)-(1/4)*(x2+y2+(8/9)*z2)+2/9);
	r=-( r*r*r -6*r2*r2) ;
	return( min(10., max(P0*r,-10.)) );
}

/****************************************************************************
*								i_nfunc.c for internal functions
*
*	This module calculates potential function for POVISO patch.
*	By Ryoichi SUZUKI, rsuzuki@etl.go.jp.
*
*****************************************************************************/
static DBL func_0(FUNCTION *Func,VECTOR XYZ)
{
	/*	 sphere	*/
	return(sqrt(x*x * P0*P0 + y*y * P1*P1 + z*z * P2*P2));	
}

static DBL func_1(FUNCTION *Func,VECTOR XYZ)
{
	DBL r,r2,y2,z2,temp;
	/*	 blob	*/
	y2=y*y; z2=z*z;
	r2= P0*0.5;
	r=((x+r2)*(x+r2)+y2+z2)*P2*P2;
	if (r>1.) 
		r=1.;
	temp=( (x - r2)*(x - r2)+y2+z2)*P4*P4;
	if (temp>1) 
		temp=1.;
	return(-(P1 *(1.-r) *(1.-r)	+P3 *(1.-temp)*(1.-temp) ));
}

static DBL func_2(FUNCTION *Func,VECTOR XYZ)
{
	DBL r,x2,y2,z2;
	x2=x*x; y2=y*y; z2=z*z;
	/*	f= f1 + f2 + f3 */
	r=exp(-(x2*P0+y2*P0+z2)*P1) + exp(-(x2*P0+y2+z2*P0)*P1) +
	exp(-(x2+y2*P0+z2*P0)*P1);
	return(P3-r*P2);
}

static DBL func_2b(FUNCTION *Func,VECTOR XYZ)
{
	DBL r,x2,y2,z2;
	x2=x*x; y2=y*y; z2=z*z;
	/*	f= f1 + f2 */
	r=exp(-(x2+y2+z2)*P1) + exp(-((x-P0)*(x-P0)+y2+z2)*P1);
	return(P3-r*P2);
}

static DBL func_3(FUNCTION *Func,VECTOR XYZ)
{
	DBL r,x2,y2,z2;
	x2=x*x; y2=y*y; z2=z*z;
	/*	f= max (f1, f2, f3) */
	r=max(exp(-(x2*P0+y2*P0+z2)*P1), exp(-(x2*P0+y2+z2*P0)*P1));
	r=max(r, exp(-(x2+y2*P0+z2*P0)*P1));
	return(P3-r*P2);
}

static DBL func_4(FUNCTION *Func,VECTOR XYZ)
{
	DBL r,x2,y2,z2;
	x2=x*x; y2=y*y; z2=z*z;
	/* f= min (f1, f2, f3) */
	r=min(exp(-(x2*P0+y2*P0+z2)*P1), exp(-(x2*P0+y2+z2*P0)*P1));
	r=min(r, exp(-(x2+y2*P0+z2*P0)*P1));
	return(P3-r*P2);
}

static DBL func_5(FUNCTION *Func,VECTOR XYZ)
{
	DBL r,x2,y2,z2;
	x2=x*x; y2=y*y; z2=z*z;
	r=exp(-(x2+y2+z2)*P3)*P4				 -exp(-(x2*P0+y2*P0+z2)*P1)
	-exp(-(x2*P0+y2+z2*P0)*P1)	 -exp(-(x2+y2*P0+z2*P0)*P1);
	return(-r*P2);
}

static DBL func_6(FUNCTION *Func,VECTOR XYZ)
{
	DBL r,r2,temp,th,ph,x2;
	/* helix												*
	*	 p[0] : number of helix					*
	*	 p[1] : frequency								*
	*	 p[2] : minor radius							*
	*	 p[3] : major radius							*
	*	 p[4] : shape parameter					*
	*	 p[5] : cross section						*
	*	 p[5] 	= 1:	circle								*
	*				= 2:	diamond						*
	*				< 1:	rectangle(rounded) 		*
	*	 p[6] : rotation angle for p[5]<1		*/

	r=sqrt(x*x+z*z);
	if ((x==0)&&(z==0)) 
		x=0.000001;
	th=atan2(z,x);
	th=fmod(th*P0+y*P1*P0, M_2PI);
	if (th<0) 
		th+=M_2PI;
	z=(th-M_PI)/P4/(P1*P0);

	x=r-P3;
	if (P5==1)
		r2=sqrt(x*x+z*z);
	else
	{
		if (P6!=0)
		{	
			th=cos(P6*M_PI180); ph=sin(P6*M_PI180);
			x2=x*th-z*ph;
			z=x*ph+z*th;
			x=x2;
		}
		if (P5!=0)
		{
			temp= 2./P5;
			r2= pow( (pow(fabs(x),temp)+pow(fabs(z),temp)), P5*.5);
		}
		else 
			r2= max( fabs(x), fabs(z));
	} 
	return(-P2+min((P3+r),r2));
}

static DBL func_7(FUNCTION *Func,VECTOR XYZ)
{
	DBL th,ph,x2,z2,r2,temp;
	/* helical shape	for (minor radius>major radius	*
	*		cross section	 p[5] same as NFunc = 6			*/
	th=y*P1;
	ph=cos(th);
	th=sin(th);
	x2=x-P3*ph;
	z2=z-P3*th;
	x=x2*ph+z2*th;
	z=(-x2*th+z2*ph);

	if (P5==1)
		return(P2 - sqrt(x*x+z*z));
	if (P5!=0)
	{
		temp= 2./P5;
		r2= pow( (pow(fabs(x),temp)+pow(fabs(z),temp)), P5*.5);
	}
	else 
		r2= max( fabs(x), fabs(z));
	return(-P2+r2);
}

static DBL func_8(FUNCTION *Func,VECTOR XYZ)
{
	DBL r,r2,th,temp;
	/* spiral shape*/

	r=sqrt(x*x+z*z);
	if ((x==0)&&(z==0)) 
		x=0.000001;
	th=atan2(z,x);
	r=r+P0*th/M_2PI;
	r2= fmod(r, P0)-P0*0.5;

	if (P5==1)	
		r2= sqrt(r2*r2+y*y);
	else if (P5!=0)
	{
		temp= 2/P5;
		r2= pow( (pow(fabs(r2),temp)+pow(fabs(y),temp)), 1./temp);
	}
	else 
		r2= max( fabs(r2), fabs(y));
	r=sqrt(x*x+y*y+z*z);
	return(-min(P2-r,P1-min(r2,r)) );
}

static DBL func_9(FUNCTION *Func,VECTOR XYZ)
{
	/*	cilindrical shape */
	DBL y2, temp;
	y2=y*y;
	temp=(P0+P1*y+P2*y2 + P3*y2*y +P4*y2*y2);
	if (temp<-5.) 
		temp=-5.;
	return (-temp+sqrt(x*x+z*z));
}

static DBL func_10(FUNCTION *Func,VECTOR XYZ)
{
	/*	2-D distribution */
	return(-( P0* cos(P1*x) * cos(P2*z) * exp(- P3*(x*x+z*z)) -y));
}

static DBL func_11(FUNCTION *Func,VECTOR XYZ)
{
	DBL r, th,temp;
	/*	well known function in quantum mechanics */
	if ((x==0)&&(z==0)) 
		x=1e-6;
	r=sqrt(x*x+z*z);
	th=cos(atan2(r,y));
	r=sqrt(r*r+y*y)*2.;
	temp=r*r*exp(-r*0.33333333)*(3.*th*th-1);
	return (temp*temp-12.0)*(-0.1);
}

static DBL func_12(FUNCTION *Func,VECTOR XYZ)
{
	DBL r,r2, temp,th,ph;
	r2=sqrt(x*x+z*z);
	if ((x==0)&&(z==0)) 
		x=0.000001;
	ph=atan2(x,z);
	temp=atan2(r2 - P0, y);
	r=sqrt((r2-P0)*(r2-P0)+y*y);
	th=fmod(ph*P1+temp*P2, M_2PI);
	if (th<0) 
		th+=M_2PI;
	temp=atan2((th-M_PI)*P8, r-P5);
	temp=cos(temp*P7+ph*P9)*P3+P4;
	r2=P4+P5-P0-r2;
	th=th - M_PI;
	temp=temp-min(sqrt((r-P5)*(r-P5)+th*th*P6), (r+P5));
	return(-max(r2,temp));
}

static DBL func_13(FUNCTION *Func,VECTOR XYZ)
{
	DBL r,th,temp;
	if ((x==0)&&(z==0)) 
		x=0.0001;
	th=atan2(z,x);
	r=sqrt( (x + P0*0.25)*(x + P0*0.25) + z*z);
	temp= cos(th*0.5) * P0*0.5 -sqrt((r- P0*0.75)*(r - P0*0.75)+y*y);
	temp= min(z, temp);
	r= P0*0.5	 - sqrt( (x - P0*0.5)*(x - P0*0.5) + z*z + y*y) ;
	return(-(DBL)max(temp,r));
}

static DBL func_14(FUNCTION *Func,VECTOR XYZ)
{
	DBL th,ph,r,r2,temp;
	th=M_PI/P0;
	ph=M_PI/P1;
	r = fmod(x, P0*2);			
	if (r<0) 
		r+=P0*2;
	r= fabs(r-P0)*P2;
	r2= (y-cos(z*ph)*P3)*P4;
	temp= -sqrt( r2*r2 + r*r);

	r = fmod(x-P0, P0*2); 
	if (r<0) 
		r+=P0*2;
	r= fabs(r-P0)*P2;
	r2= (y+cos(z*ph)*P3)*P4;
	temp= max(-sqrt( r2*r2 + r*r), temp);

	r = fmod(z, P1*2);		
	if (r<0) 
		r+=P1*2;
	r= fabs(r-P1)*P2;
	r2= (y+cos(x*th)*P3)*P4;
	temp= max(-sqrt( r2*r2 + r*r), temp);

	r = fmod(z-P1, P1*2); 
		if (r<0) r+=P1*2;
	r= fabs(r-P1)*P2;
	r2= (y-cos(x*th)*P3)*P4;
	return( -max(-sqrt( r2*r2 + r*r), temp));
}

static DBL func_16(FUNCTION *Func,VECTOR XYZ)
{
	DBL x2,y2,z2,r,r2;
	int i;
	y2=y*y;
	r2=P1+P2*y+P3*y2+P4*y2*y+	P5*y2*y2;
	r=-10000;
	for (i=0;i<(int)P0;i++)
	{
		x2 = x - r2*sin(2*M_PI/P0*i);
		z2 = z - r2*cos(2*M_PI/P0*i);
		r= max(r,-sqrt( x2*x2 + z2*z2 ) );
	}
	return(-r);
}

/*dfs--*/
static DBL hex_x(FUNCTION *Func,VECTOR XYZ)
{
	DBL x1,y1,x2,y2, th;
	x1=fabs(fmod(fabs(x), sqrt(3))-sqrt(3)/2);
	y1=fabs(fmod(fabs(y), 3)-1.5);
	x2=sqrt(3)/2-x1;
	y2=1.5-y1;
	if ((x1*x1+y1*y1)>(x2*x2+y2*y2)) 
	{
		x1=x2; 
		y1=y2;
	}
	if ((x1==0)&&(y1==0)) 
		x=0.000001;
	th=atan2(y1,x1);
	if (th<M_PI/6) 
		return(x1);
	else
	{
		x1=cos(M_PI/3)*x1+sin(M_PI/3)*y1;
		return(x1);
	}
}

static DBL hex_y(FUNCTION *Func,VECTOR XYZ)
{
	DBL x1,y1,x2,y2, th;
	x1=fabs(fmod(fabs(x), sqrt(3))-sqrt(3)/2);
	y1=fabs(fmod(fabs(y), 3)-1.5);
	x2=sqrt(3)/2-x1;
	y2=1.5-y1;
	if ((x1*x1+y1*y1)>(x2*x2+y2*y2)) 
	{
		x1=x2; 
		y1=y2;
	}
	if ((x1==0)&&(y1==0)) 
		x=0.000001;
	th=atan2(y1,x1);
	if (th<M_PI/6) 
		return(y1);
	else
	{
		y1=-sin(M_PI/3)*x1+cos(M_PI/3)*y1;
		return(fabs(y1));
	}
}
/*--dfs*/

/* "Michael C. Andrews" <M.C.Andrews@reading.ac.uk>*/
/*For those of you who asked for the source code to the cloud isosurface*/
static DBL ridge(FUNCTION *Func, VECTOR EPoint)
{
	int i;
	DBL Lambda, Omega, l, o;
	DBL value, v, resid, tot = 1.0, off, ridge, scale, rscale;
	VECTOR temp;
	int Octaves;

	Lambda = l = P0;
	Octaves = (int)P1;
	Omega = o = P2;
	off = P3;
	ridge = P4;

	rscale = 1.0 / max(ridge, 1.0 - ridge);
	scale = 1.0 / max(off, 1.0 - off);
	resid = P1 - (DBL)Octaves;

	v = fabs(Noise(EPoint) - ridge) * rscale;
	value = (v - off);

	for (i = 2; i <= Octaves; i++)
	{
		VScale(temp,EPoint,l);
		v = fabs(Noise(temp) - ridge) * rscale;
		value += o * (v - off);
		tot += o;
		l *= Lambda;
		o *= Omega;
	}
	if (0.0 != resid)
	{
		VScale(temp,EPoint,l);
		v = fabs(Noise(temp) - ridge) * rscale;
		value += o * (v - off) * resid;
		tot += o * resid;
	}

	return (value * scale / tot);
}
/* "Michael C. Andrews" <M.C.Andrews@reading.ac.uk>*/

/*----------i_dat3d---------------------------------*/
static float intp3(float t,float *fa,float *fb,float *fc,float *fd)
{
	float b,d,e,f;
	b=(*fc - *fa)*(const float).5;
	d=(*fd-*fb)*(const float).5;
	e=(const float)2.*((*fb)-(*fc))+b+d;
	f=-(const float)3.*((*fb)-(*fc))-(const float)2.*b-d;
	return (float)( ((e*t + f)*t + b)*t + (*fb) );
}

static DBL data_3D_3(FUNCTION *Func, VECTOR XYZ)
{
	SNGL *f3d;
	int ix, iy, iz;
	int i,j, ii,jj, ixmax,iymax,izmax;

	x=fabs(x); y=fabs(y); z=fabs(z);
	ix=(int)x; iy=(int)y; iz=(int)z;
	if (!Func->Lib->pClientData) return -1.e6;
	f3d=(SNGL*)Func->Lib->pClientData;
	ixmax=(int)f3d[0];
	iymax=(int)f3d[1];
	izmax=(int)f3d[2];

	x=x-(DBL)ix;
	y=y-(DBL)iy;
	z=z-(DBL)iz;

	for (i=0;i<4;i++) 
	{
		ii=zmax(i+ix-1,ixmax);
		for (j=0;j<4;j++)
		{
			jj=zmax(j+iy-1,iymax);
			intpd2[i][j]= intp3( (float)z, &F3D(ii,jj,zmax(-1+iz,izmax)), &F3D(ii,jj,zmax(	 iz,izmax)),
							 &F3D(ii,jj,zmax( 1+iz,izmax)), &F3D(ii,jj,zmax( 2+iz,izmax)) );
		}
	}

	for (i=0;i<4;i++) 
		intpd2[0][i]= intp3( (float)y, &intpd2[i][0], &intpd2[i][1], &intpd2[i][2], &intpd2[i][3]);
	return P0* intp3((float)x, &intpd2[0][0], &intpd2[0][1], &intpd2[0][2], &intpd2[0][3]);
}


static DBL data_2D_3(FUNCTION *Func, VECTOR XYZ)
{
	SNGL *f3d;
	int ix, iz;
	int i, ii, ixmax, iymax, izmax;

	x=fabs(x); z=fabs(z);
	ix=(int)x; iz=(int)z;

	if (!Func->Lib->pClientData) 
		return -1.e6;
	f3d=(SNGL*)Func->Lib->pClientData;
	ixmax=(int)f3d[0];
	iymax=(int)f3d[1];
	izmax=(int)f3d[2];

	x=x-(DBL)ix;
	z=z-(DBL)iz;

	for (i=0;i<4;i++) 
	{
		ii=zmax(i+ix-1,ixmax);
		intpd2[0][i]= intp3( (float)z, &F3D(ii,0,zmax(-1+iz,izmax)), &F3D(ii,0,zmax(	 iz,izmax)),
				 &F3D(ii,0,zmax( 1+iz,izmax)), &F3D(ii,0,zmax( 2+iz,izmax)));
	}
	return	y-P0* intp3((float)x, &intpd2[0][0], &intpd2[0][1], &intpd2[0][2], &intpd2[0][3]);
}

static DBL data_3D_1(FUNCTION *Func, VECTOR XYZ)
{
	SNGL *f3d;
	int ix, iy, iz;
	int i0,i1,j0,j1,k0,k1, ixmax,iymax,izmax;

	x=fabs(x); y=fabs(y); z=fabs(z);
	ix=(int)x; iy=(int)y; iz=(int)z;

	if (!Func->Lib->pClientData) 
		return -1.e6;
	f3d=(SNGL*)Func->Lib->pClientData;

	ixmax=(int)f3d[0];
	iymax=(int)f3d[1];
	izmax=(int)f3d[2];

	x=x-(DBL)ix;
	y=y-(DBL)iy;
	z=z-(DBL)iz;

	i0=zmax(ix,ixmax);
 	j0=zmax(iy,iymax);
 	k0=zmax(iz,izmax);

 	i1=zmax(ix+1,ixmax);
 	j1=zmax(iy+1,iymax);
 	k1=zmax(iz+1,izmax);

	return P0* ( ((F3D(i0,j0,k0)*(1.-z)+F3D(i0,j0,k1)*z)*(1.-y)+
									 (F3D(i0,j1,k0)*(1.-z)+F3D(i0,j1,k1)*z)*	y	)*(1.-x)+
									((F3D(i1,j0,k0)*(1.-z)+F3D(i1,j0,k1)*z)*(1.-y)+
									 (F3D(i1,j1,k0)*(1.-z)+F3D(i1,j1,k1)*z)*	y	)*	x );
}


static DBL data_2D_1(FUNCTION *Func, VECTOR XYZ)
{
	SNGL *f3d;
	int ix, iz;
	int i0,i1,j0,j1, ixmax,iymax,izmax;

	x=fabs(x);	z=fabs(z);
	ix=(int)x;	iz=(int)z;

	if (!Func->Lib->pClientData) 
		return -1.e6;
	f3d=(SNGL*)Func->Lib->pClientData;

	ixmax=(int)f3d[0];
	iymax=(int)f3d[1];
	izmax=(int)f3d[2];

	x=x-(DBL)ix;
	z=z-(DBL)iz;

	i0=zmax(ix,ixmax);
	j0=zmax(iz,izmax);
	i1=zmax(ix+1,ixmax);
	j1=zmax(iz+1,izmax);

	return y- P0* ( ( F3D(i0,0,j0)*(1.-z)+ F3D(i0,0,j1)*	z	)*(1.-x)+
						 ( F3D(i1,0,j0)*(1.-z)+ F3D(i1,0,j1)*	z	)*	x );
}


static DBL odd_sphere(FUNCTION *Func, VECTOR XYZ)
{
	DBL r;
	r= P2*sqrt(x*x+y*y+z*z);
	return (r-P1+data_3D_3(Func, XYZ));
}


/**********************************************************
	Kenton Musgraves Ridged Multifractal
	Added MCB 01-06-00
	P0 = H
	P1 = lacunarity
	P2 = octaves
	P3 = offset
	P4 = gain
 ***********************************************************/
#define SNoise(a) (Noise(a)*2.0-1.0)

static DBL ridgedMF(FUNCTION *Func, VECTOR XYZ)
{
	DBL *ea,freq,signal,weight,result;
	int i;
	VECTOR V1;
	V1[X]=x;V1[Y]=y;V1[Z]=z;
	if (Func->cache == NULL) 
	{
		ea =(DBL*) POV_MALLOC((P2 + 1)*sizeof(DBL), "exponent array");
		freq = 1.0;
		for (i=0; i<=P2;i++)
		{
			ea[i]= pow(freq,-P0);
			freq *= P1;
		}
		Func->cache = (void *)ea;
	} 
	else 
	{
		ea = (DBL *)Func->cache;
	}
	signal = SNoise( V1 );
	if (signal < 0.0 ) 
		signal = -signal;
	signal = P3 - signal;
	signal *= signal;
	result = signal;
	weight = 1.0;

	for (i=1; i<P2; i++) 
	{
		V1[X] *= P1;
		V1[Y] *= P1;
		V1[Z] *= P1;
		weight = signal * P4;
		if (weight > 1.0)
			weight = 1.0;
		if (weight < 0.0)
			weight = 0.0;
		signal = SNoise( V1 );
		if (signal < 0.0 )
			signal = -signal;
		signal = P3 - signal;
		signal *= signal;
		signal *= weight;
		result += signal * ea[i];
	}
	return (result);
}
 
/*dfs--heteroMF*/
/*
 * "Hetero-multifractal"
 *
 * P0 = H
 * P1 = lacunarity
 * P2 = octaves
 * P3 = offset
 */

static DBL heteroMF(FUNCTION *Func, VECTOR XYZ)
{
	DBL *ea,freq,signal,inc,rem;
	int p;
	int i;
	VECTOR V1;
	V1[X]=x;V1[Y]=y;V1[Z]=z;
	if (Func->cache == NULL) 
	{
		ea =(DBL*) POV_MALLOC((P2 + 1)*sizeof(DBL), "exponent array");
		freq = 1.0;
		for (i=0; i<=P2;i++)
		{
			ea[i]= pow(freq,-P0);
			freq *= P1;
		}
		Func->cache = (void *)ea;
	} 
	else 
	{
		ea = (DBL *)Func->cache;
	}
	signal = SNoise( V1 )+P3;
	V1[X] *= P1;
	V1[Y] *= P1;
	V1[Z] *= P1;
	for (i=1; i<P2; i++){
		 p=(int) P4;
		 /* make a noisy increment and scale it by f^(-H) */
		 inc = (SNoise( V1 ) + P3) * ea[i];
		 /* scale the increment by (the current signal)^P4 at V1
				so that P4=0 gives 'straight' 1/f, P4=1 gives heterogenous
				fractal, etc */
		 while (p>0){
				inc *= signal;
				p--;
		 }
		 signal += inc;
		 /* go to next 'octave' */
		 V1[X] *= P1;
		 V1[Y] *= P1;
		 V1[Z] *= P1;
	
	}

	rem = P2 - (int) P2;
	if (rem) {
			/* do something with fraction part of octave */
			inc = (SNoise(V1) + P3) * ea[i];
			signal += rem * inc * signal;
	}

	return signal;
}
/*--dfs*/

/*---------- for internal functions ----------------------*/

typedef struct iso_funcs ISO_FUNCS;
struct iso_funcs {	ISO_FUNC func;	 char* funcname;};

ISO_FUNCS func_table[]={
	imp_func, "imp_func",
	R, "R",
	TH, "TH",
	PH, "PH",
	sphere, "sphere",
	helix1, "helix1",
	helix2, "helix2",
	spiral, "spiral",
	mesh1, "mesh1",
	rounded_box, "rounded_box",
	torus, "torus",
	superellipsoid,"superellipsoid",
/*Algebra (instead of a lib) */
	Algbr_Cyl1,"Algbr_Cyl1",
	Algbr_Cyl2,"Algbr_Cyl2",
	Algbr_Cyl3,"Algbr_Cyl3",
	Algbr_Cyl4,"Algbr_Cyl4",
	Bicorn,"Bicorn",
	Bifolia,"Bifolia",
	Boy_surface,"Boy_surface",
	Ovals_of_Cassini,"Ovals_of_Cassini",
	Cubic_saddle,"Cubic_saddle",
	Cushion,"Cushion",
	Devils_curve,"Devils_curve",
	Devils_curve2D,"Devils_curve2D",
	Dupin_Cyclid,"Dupin_Cyclid",
	Folium_surface,"Folium_surface",
	Folium_surface2D,"Folium_surface2D",
	Torus_gumdrop,"Torus_gumdrop",
	Hunt_surface,"Hunt_surface",
	Hyperbolic_torus,"Hyperbolic_torus",
	Kampyle_of_Eudoxus,"Kampyle_of_Eudoxus",
	Kampyle_of_Eudoxus2D,"Kampyle_of_Eudoxus2D",
	Klein_Bottle,"Klein_Bottle",
	Kummer_Surface_V1,"Kummer_Surface_V1",
	Kummer_Surface_V2,"Kummer_Surface_V2",
	Lemniscate_of_Gerono,"Lemniscate_of_Gerono",
	Lemniscate_of_Gerono2D,"Lemniscate_of_Gerono2D",
	Paraboloid,"Paraboloid",
	Parabolic_Torus,"Parabolic_Torus",
	Piriform,"Piriform",
	Piriform2D,"Piriform2D",
	Quartic_paraboloid,"Quartic_paraboloid",
	Quartic_saddle,"Quartic_saddle",
	Quartic_Cylinder,"Quartic_Cylinder",
	Steiners_Roman,"Steiners_Roman",
	Strophoid,"Strophoid",
	Strophoid2D,"Strophoid2D",
	Glob,"Glob",
 Pillow,"Pillow",
	Crossed_Trough,"Crossed_Trough",
 Witch_of_Agnesi,"Witch_of_Agnesi",
 Witch_of_Agnesi2D ,"Witch_of_Agnesi2D",
	Mitre,"Mitre",
	Odd,"Odd",
	Heart,"Heart",
	Torus2,"Torus2",
	Nodal_cubic,"Nodal_cubic",
	Umbrella,"Umbrella",
	Enneper,"Enneper",
	/*i_Nfunc (instead of a lib) */
	func_0,"func_0",
	func_1,"func_1",
	func_2,"func_2",
	func_2b,"func_2b",
	func_3,"func_3",
	func_4,"func_4",
	func_5,"func_5",
	func_6,"func_6",
	func_7,"func_7",
	func_8,"func_8",
	func_9,"func_9",
	func_10,"func_10",
	func_11,"func_11",
	func_12,"func_12",
	func_13,"func_13",
	func_14,"func_14",
	func_16,"func_16",
/*dfs--*/
	hex_x, "hex_x",
	hex_y, "hex_y",
/*--dfs*/
/* "Michael C. Andrews" <M.C.Andrews@reading.ac.uk>*/
	ridge,"ridge",
/*i_dat3d (instead of a lib) */

	data_3D_3,"data_3D_3",
	data_2D_3,"data_2D_3",
	data_3D_1,"data_3D_1",
	data_2D_1,"data_2D_1",
	odd_sphere,"odd_sphere",
	 #ifdef IsoPigmentPatch
	iso_pigment,"iso_pigment",		/* Added MCB 12-28-98 */
#endif
	ridgedMF,"ridgedMF",			/* Added MCB 01-06-00 */
	/*dfs--*/
	heteroMF,"heteroMF",
	/*--dfs*/
	NULL,""
};

int pov_stricmp (char *s1,char *s2);

ISO_FUNC find_iso_func(char * fname)
{
	short i;
	for (i = 0 ; func_table[i].func!=NULL ; i++)
	{
		if (pov_stricmp (func_table[i].funcname, fname) == 0)
			return (func_table[i].func);
	}
	return (0);
}

/*
 * The following is not needed for OpenVMS.
 */
#if !defined(VMS) || !defined(DECC)
#  pragma peephole off 
#endif

