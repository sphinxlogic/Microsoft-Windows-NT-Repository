/*
** Astrolog (Version 2.25) File: xcharts.c
** (Initially programmed 10/23-29/1991)
**
** IMPORTANT: the graphics database and chart display routines used in
** this program are Copyright (C) 1991-1992 by Walter D. Pullen. Permission
** is granted to freely use and distribute these routines provided one
** doesn't sell, restrict, or profit from them in any way. Modification
** is allowed provided these notices remain with any altered or edited
** versions of the program.
*/

#include "astrolog.h"

#ifdef X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define ISLEGAL(X, Y) \
  ((X) >= 0 && (X) < chartx && (Y) >= 0 && (Y) < chartx)
#define edge(X1, Y1, X2, Y2, O) \
  rect(X1, Y1, X2, Y2, 1, 1, O)
#define rectall(XSIZ, YSIZ, O) \
  rect(0, 0, chartx-1, charty-1, XSIZ, YSIZ, O)

char string[STRING];
double symbol[TOTAL+1], circx[360+1], circy[360+1];


/*
*******************************************************************************
** Main subprograms
*******************************************************************************
*/

int proper(i)
int i;
{
  int j = TRUE;
  if (modex == 'l')
    j = (i <= THINGS || i > objects+4);
  else if (modex == 'z' || modex == 'g')
    j = (i < THINGS || i > objects+4);
  else if (modex == 's')
    j = (i != 2 && (i < THINGS || i > objects+4));
  return j && !ignore[i];
}

drawobject(i, x, y)
int i, x, y;
{
  if (!label)
    return;
  if (modex != 's' && ((i == centerplanet && i > 2) ||
		       (centerplanet == 0 && i == 1)))
    i = 0;
  if (i <= objects)
    turtle(objectdraw[i], x, y, objectcolor[i]);
  else if (!xbitmap) {
    Xcolor(objectcolor[i]);
    sprintf(string, "%c%c%c", OBJNAM(i));
    XDrawImageString(disp, pixmap, gc, x-strlen(string)*FONT_X/2, y+FONT_Y/2,
		     string, strlen(string));
  }
}

fillsymbolring(symbol)
double *symbol;
{
  int i, j, k = 1, l, k1, k2;
  double orb = DEFORB*256.0/(double)charty*(double)scale/100.0, temp;
  for (l = 0; k && l < 100; l++) {
    k = 0;
    for (i = 1; i <= total; i++) if (proper(i)) {
      k1 = 1000; k2 = -1000;
      for (j = 1; j <= total; j++)
	if (proper(j) && i != j) {
	  temp = symbol[j]-symbol[i];
	  if (dabs(temp) > 180.0)
	    temp -= DEGREES*sgn(temp);
	  if (temp<k1 && temp>=0.0)
	    k1 = temp;
	  else if (temp>k2 && temp<=0.0)
	    k2 = temp;
	}
      if (k2>-orb && k1>orb) {
	k = 1; symbol[i] = mod(symbol[i]+orb*0.51+k2*0.49);
      } else if (k1<orb && k2<-orb) {
	k = 1; symbol[i] = mod(symbol[i]-orb*0.51+k1*0.49);
      } else if (k2>-orb && k1<orb) {
	k = 1; symbol[i] = mod(symbol[i]+(k1+k2)*0.5);
      }
    }
  }
}

#define POINT(U, R, S) ((int) ((R)*(double)(U)*(S)+0.5))
#define PX(A) COSD(180.0-(A)+Asc)
#define PY(A) SIND(180.0-(A)+Asc)

xchartwheel()
{
  int cx = chartx / 2, cy = charty / 2, unit, i, j, count = 0;
  double Asc, orb = DEFORB*256.0/(double)charty*(double)scale/100.0, temp;
  if (xbitmap)
    block(0, 0, chartx - 1, charty - 1, off);
  rectall(1, 1, hilite);
  unit = MIN(cx, cy);
  Asc = house[1];
  for (i = 0; i < 360; i++) {
    circx[i] = cos((double) i / DEGTORAD);
    circy[i] = sin((double) i / DEGTORAD);
  }
  circx[360] = circx[0]; circy[360] = circy[0];
  drawline(cx+POINT(unit, 0.99, PX(house[1])),
       cy+POINT(unit, 0.99, PY(house[1])),
       cx+POINT(unit, 0.99, PX(house[7])),
       cy+POINT(unit, 0.99, PY(house[7])), hilite, 1-xcolor);
  drawline(cx+POINT(unit, 0.99, PX(house[10])),
       cy+POINT(unit, 0.99, PY(house[10])),
       cx+POINT(unit, 0.99, PX(house[4])),
       cy+POINT(unit, 0.99, PY(house[4])), hilite, 1-xcolor);
  for (i = 0; i < 360; i += 5-xcolor*4) {
    temp = (double) i;
    drawline(cx+POINT(unit, 0.80, PX(temp)), cy+POINT(unit, 0.80, PY(temp)),
	     cx+POINT(unit, 0.75, PX(temp)), cy+POINT(unit, 0.75, PY(temp)),
	     i%5 ? gray : on, 0);
  }
  for (i = 0; i < 360; i++) {
    drawline(cx+POINT(unit, 0.95, circx[i]), cy+POINT(unit, 0.95, circy[i]),
	 cx+POINT(unit, 0.95, circx[i+1]), cy+POINT(unit, 0.95, circy[i+1]),
	 on, 0);
    drawline(cx+POINT(unit, 0.80, circx[i]), cy+POINT(unit, 0.80, circy[i]),
	 cx+POINT(unit, 0.80, circx[i+1]), cy+POINT(unit, 0.80, circy[i+1]),
	 on, 0);
    drawline(cx+POINT(unit, 0.75, circx[i]), cy+POINT(unit, 0.75, circy[i]),
	 cx+POINT(unit, 0.75, circx[i+1]), cy+POINT(unit, 0.75, circy[i+1]),
	 on, 0);
    drawline(cx+POINT(unit, 0.65, circx[i]), cy+POINT(unit, 0.65, circy[i]),
	 cx+POINT(unit, 0.65, circx[i+1]), cy+POINT(unit, 0.65, circy[i+1]),
	 on, 0);
  }
  for (i = 1; i <= SIGNS; i++) {
    temp = (double) (i-1)*30.0;
    drawline(cx+POINT(unit, 0.95, PX(temp)),
	 cy+POINT(unit, 0.95, PY(temp)),
	 cx+POINT(unit, 0.80, PX(temp)),
	 cy+POINT(unit, 0.80, PY(temp)), on, 0);
    drawline(cx+POINT(unit, 0.75, PX(house[i])),
	 cy+POINT(unit, 0.75, PY(house[i])),
	 cx+POINT(unit, 0.65, PX(house[i])),
	 cy+POINT(unit, 0.65, PY(house[i])), on, 0);
    if (xcolor && i%3 != 1)
      drawline(cx, cy, cx+POINT(unit, 0.65, PX(house[i])),
	       cy+POINT(unit, 0.65, PY(house[i])), gray, 1);
    temp += 15.0;
    turtle(signdraw[i], cx+POINT(unit, 0.875, PX(temp)),
	   cy+POINT(unit, 0.875, PY(temp)), elemcolor[(i-1)%4]);
    temp = midpoint(house[i], house[mod12(i+1)]);
    turtle(housedraw[i], cx+POINT(unit, 0.70, PX(temp)),
	   cy+POINT(unit, 0.70, PY(temp)), elemcolor[(i-1)%4]);
  }
  for (i = 1; i <= total; i++) {
    symbol[i] = planet[i];
  }
  fillsymbolring(symbol);
  for (i = 1; i <= total; i++) if (proper(i)) {
    temp = symbol[i];
    drawline(cx+POINT(unit, 0.52, PX(planet[i])),
	     cy+POINT(unit, 0.52, PY(planet[i])),
	     cx+POINT(unit, 0.56, PX(temp)),
	     cy+POINT(unit, 0.56, PY(temp)),
	     ret[i] < 0.0 ? gray : on, (ret[i] < 0.0 ? 1 : 0) - xcolor);
    drawobject(i, cx+POINT(unit, 0.60, PX(temp)),
	       cy+POINT(unit, 0.60, PY(temp)));
    drawpoint(cx+POINT(unit, 0.50, PX(planet[i])),
	      cy+POINT(unit, 0.50, PY(planet[i])), objectcolor[i]);
  }
  if (bonus)
    return;
  creategrid(FALSE);
  for (j = total; j >= 2; j--)
    for (i = j-1; i >= 1; i--)
      if (gridname[i][j] && proper(i) && proper(j))
	drawline(cx+POINT(unit, 0.48, PX(planet[i])),
		 cy+POINT(unit, 0.48, PY(planet[i])),
		 cx+POINT(unit, 0.48, PX(planet[j])),
		 cy+POINT(unit, 0.48, PY(planet[j])),
		 aspectcolor[gridname[i][j]], abs(grid[i][j]/20));
}

xchartwheelrelation()
{
  int cx = chartx / 2, cy = charty / 2, unit, i, j;
  double Asc, temp;
  if (xbitmap)
    block(0, 0, chartx - 1, charty - 1, off);
  rectall(1, 1, hilite);
  unit = MIN(cx, cy);
  Asc = house1[1];
  for (i = 0; i < 360; i++) {
    circx[i] = cos((double) i / DEGTORAD);
    circy[i] = sin((double) i / DEGTORAD);
  }
  circx[360] = circx[0]; circy[360] = circy[0];
  drawline(cx+POINT(unit, 0.99, PX(house1[1])),
       cy+POINT(unit, 0.99, PY(house1[1])),
       cx+POINT(unit, 0.99, PX(house1[7])),
       cy+POINT(unit, 0.99, PY(house1[7])), hilite, 1-xcolor);
  drawline(cx+POINT(unit, 0.99, PX(house1[10])),
       cy+POINT(unit, 0.99, PY(house1[10])),
       cx+POINT(unit, 0.99, PX(house1[4])),
       cy+POINT(unit, 0.99, PY(house1[4])), hilite, 1-xcolor);
  for (i = 0; i < 360; i += 5-xcolor*4) {
    temp = (double) i;
    drawline(cx+POINT(unit, 0.82, PX(temp)), cy+POINT(unit, 0.82, PY(temp)),
	     cx+POINT(unit, 0.78, PX(temp)), cy+POINT(unit, 0.78, PY(temp)),
	     i%5 ? gray : on, 0);
  }
  for (i = 0; i < 360; i++) {
    drawline(cx+POINT(unit, 0.95, circx[i]), cy+POINT(unit, 0.95, circy[i]),
	 cx+POINT(unit, 0.95, circx[i+1]), cy+POINT(unit, 0.95, circy[i+1]),
	 on, 0);
    drawline(cx+POINT(unit, 0.82, circx[i]), cy+POINT(unit, 0.82, circy[i]),
	 cx+POINT(unit, 0.82, circx[i+1]), cy+POINT(unit, 0.82, circy[i+1]),
	 on, 0);
    drawline(cx+POINT(unit, 0.78, circx[i]), cy+POINT(unit, 0.78, circy[i]),
	 cx+POINT(unit, 0.78, circx[i+1]), cy+POINT(unit, 0.78, circy[i+1]),
	 on, 0);
    drawline(cx+POINT(unit, 0.70, circx[i]), cy+POINT(unit, 0.70, circy[i]),
	 cx+POINT(unit, 0.70, circx[i+1]), cy+POINT(unit, 0.70, circy[i+1]),
	 on, 0);
  }
  for (i = 1; i <= SIGNS; i++) {
    temp = (double) (i-1)*30.0;
    drawline(cx+POINT(unit, 0.95, PX(temp)),
	 cy+POINT(unit, 0.95, PY(temp)),
	 cx+POINT(unit, 0.82, PX(temp)),
	 cy+POINT(unit, 0.82, PY(temp)), on, 0);
    drawline(cx+POINT(unit, 0.78, PX(house1[i])),
	 cy+POINT(unit, 0.78, PY(house1[i])),
	 cx+POINT(unit, 0.70, PX(house1[i])),
	 cy+POINT(unit, 0.70, PY(house1[i])), on, 0);
    if (xcolor && i%3 != 1)
      drawline(cx, cy, cx+POINT(unit, 0.70, PX(house1[i])),
	       cy+POINT(unit, 0.70, PY(house1[i])), gray, 1);
    temp += 15.0;
    turtle(signdraw[i], cx+POINT(unit, 0.885, PX(temp)),
	   cy+POINT(unit, 0.885, PY(temp)), elemcolor[(i-1)%4]);
    temp = midpoint(house1[i], house1[mod12(i+1)]);
    turtle(housedraw[i], cx+POINT(unit, 0.74, PX(temp)),
	   cy+POINT(unit, 0.74, PY(temp)), elemcolor[(i-1)%4]);
  }
  for (i = 1; i <= total; i++) {
    symbol[i] = planet2[i];
  }
  fillsymbolring(symbol);
  for (i = 1; i <= total; i++) if (proper(i)) {
    temp = symbol[i];
    drawline(cx+POINT(unit, 0.58, PX(planet2[i])),
	     cy+POINT(unit, 0.58, PY(planet2[i])),
	     cx+POINT(unit, 0.61, PX(temp)),
	     cy+POINT(unit, 0.61, PY(temp)),
	     ret2[i] < 0.0 ? gray : on, (ret2[i] < 0.0 ? 1 : 0) - xcolor);
    drawobject(i, cx+POINT(unit, 0.65, PX(temp)),
	       cy+POINT(unit, 0.65, PY(temp)));
    drawpoint(cx+POINT(unit, 0.56, PX(planet2[i])),
	      cy+POINT(unit, 0.56, PY(planet2[i])), objectcolor[i]);
    drawpoint(cx+POINT(unit, 0.43, PX(planet2[i])),
	      cy+POINT(unit, 0.43, PY(planet2[i])), objectcolor[i]);
    drawline(cx+POINT(unit, 0.45, PX(planet2[i])),
	     cy+POINT(unit, 0.45, PY(planet2[i])),
	     cx+POINT(unit, 0.54, PX(planet2[i])),
	     cy+POINT(unit, 0.54, PY(planet2[i])),
	     ret2[i] < 0.0 ? gray : on, 2-xcolor);
  }
  for (i = 1; i <= total; i++) {
    symbol[i] = planet1[i];
  }
  fillsymbolring(symbol);
  for (i = 1; i <= total; i++) if (proper(i)) {
    temp = symbol[i];
    drawline(cx+POINT(unit, 0.45, PX(planet1[i])),
	     cy+POINT(unit, 0.45, PY(planet1[i])),
	     cx+POINT(unit, 0.48, PX(temp)),
	     cy+POINT(unit, 0.48, PY(temp)),
	     ret1[i] < 0.0 ? gray : on, (ret1[i] < 0.0 ? 1 : 0) - xcolor);
    drawobject(i, cx+POINT(unit, 0.52, PX(temp)),
	       cy+POINT(unit, 0.52, PY(temp)));
    drawpoint(cx+POINT(unit, 0.43, PX(planet1[i])),
	      cy+POINT(unit, 0.43, PY(planet1[i])), objectcolor[i]);
  }
  if (bonus)
    return;
  createrelationgrid(FALSE);
  for (j = total; j >= 1; j--)
    for (i = total; i >= 1; i--)
      if (gridname[i][j] && proper(i) && proper(j))
	drawline(cx+POINT(unit, 0.41, PX(planet1[j])),
		 cy+POINT(unit, 0.41, PY(planet1[j])),
		 cx+POINT(unit, 0.41, PX(planet2[i])),
		 cy+POINT(unit, 0.41, PY(planet2[i])),
		 aspectcolor[gridname[i][j]], abs(grid[i][j]/20));
}

#define GLOBECALC \
if (modex == 'g') { \
  x1 = mod(x1+(double)deg); \
  if (tilt != 0.0) { \
    x1 = DTOR(x1); y1 = DTOR(90.0-y1); \
    coorxform(&x1, &y1, tilt / DEGTORAD); \
    x1 = mod(RTOD(x1)); y1 = 90.0-RTOD(y1); } \
  o = x1 > 180.0; \
  v = cy + (int) ((double)(ry+1)*-COSD(y1)); \
  u = cx + (int) ((double)(rx+1)*-COSD(x1)*SIND(y1)); \
} else { \
  if (bonus ? y1 < 90.0 : y1 > 90.0) \
    o = 1; \
  j = bonus ? 90.0+x1+deg : 270.0-x1-deg; \
  v = cy + (int) (SIND(y1)*(double)(ry+1)*SIND(j)); \
  u = cx + (int) (SIND(y1)*(double)(rx+1)*COSD(j)); \
}

drawglobe(deg)
int deg;
{
  char *nam, *loc, *lin;
  int X[TOTAL+1], Y[TOTAL+1], M[TOTAL+1], N[TOTAL+1],
    cx = chartx / 2, cy = charty / 2, rx, ry, lon, lat, unit = 12*scale/100,
    x, y, m, n, u, v, o = 0, i, J, k, l;
  double planet1[TOTAL+1], planet2[TOTAL+1], x1, y1, j;
  color c;
  if (xbitmap)
    block(0, 0, chartx - 1, charty - 1, off);
  rx = cx-1; ry = cy-1;
  while (readworlddata(&nam, &loc, &lin)) {
    i = nam[0]-'0';
    c = (modex == 'g' && bonus) ? gray : (i ? rainbowcolor[i] : maincolor[6]);
    lon = (loc[0] == '+' ? 1 : -1)*
      ((loc[1]-'0')*100 + (loc[2]-'0')*10 + (loc[3]-'0'));
    lat = (loc[4] == '+' ? 1 : -1)*((loc[5]-'0')*10 + (loc[6]-'0'));
    x = 180-lon;
    y = 90-lat;
    x1 = (double) x; y1 = (double) y; GLOBECALC; m = u; n = v;
    for (i = 0; lin[i]; i++) {
      if (lin[i] == 'L' || lin[i] == 'H' || lin[i] == 'G')
	x--;
      else if (lin[i] == 'R' || lin[i] == 'E' || lin[i] == 'F')
	x++;
      if (lin[i] == 'U' || lin[i] == 'H' || lin[i] == 'E')
	y--;
      else if (lin[i] == 'D' || lin[i] == 'G' || lin[i] == 'F')
	y++;
      if (x > 359)
	x = 0;
      else if (x < 0)
	x = 359;
      x1 = (double) x; y1 = (double) y; GLOBECALC;
      if (!o)
	drawline(m, n, u, v, c, 0);
      else
	o = 0;
      m = u; n = v;
    }
  }
  m = cx+rx+1; n = cy;
  for (i = 0; i <= 360; i++) {
    u = cx+(rx+1)*COSD(i); v = cy+(ry+1)*SIND(i);
    u = MIN(u, cx+rx); v = MIN(v, cy+ry);
    drawline(m, n, u, v, on, 0); m = u; n = v;
  }
  if (modex != 'g' || !bonus)
    return;
  j = Lon;
  if (j < 0.0)
    j += DEGREES;
  for (i = 1; i <= total; i++) {
    planet1[i] = DTOR(planet[i]);
    planet2[i] = DTOR(planetalt[i]);
    ecltoequ(&planet1[i], &planet2[i]);
  }
  for (i = 1; i <= total; i++) if (proper(i)) {
    x1 = planet1[18]-planet1[i];
    if (x1 < 0.0)
      x1 += 2.0*PI;
    if (x1 > PI)
      x1 -= 2.0*PI;
    x1 = mod(180.0-j-RTOD(x1));
    y1 = 90.0-RTOD(planet2[i]);
    GLOBECALC;
    X[i] = o ? -1000 : u; Y[i] = v; M[i] = X[i]; N[i] = Y[i]+unit/2;
  }
  for (i = 1; i <= total; i++) if (proper(i)) {
    k = l = chartx+charty;
    for (J = 1; J < i; J++) if (proper(J)) {
      k = MIN(k, abs(M[i]-M[J])+abs(N[i]-N[J]));
      l = MIN(l, abs(M[i]-M[J])+abs(N[i]-unit-N[J]));
    }
    if (k < unit || l < unit)
      if (k < l)
	N[i] -= unit;
  }
  for (i = total; i >= 1; i--) if (X[i] >= 0 && proper(i))
    drawobject(i, M[i], N[i]);
  for (i = total; i >= 1; i--) if (X[i] >= 0 && proper(i)) {
    if (!xbitmap)
      Xcolor(objectcolor[i]);
    drawspot(X[i], Y[i], objectcolor[i]);
  }
}

drawleyline(l1, f1, l2, f2, o)
double l1, f1, l2, f2;
bit o;
{
  l1 = mod(l1); l2 = mod(l2);
  f1 = 90.0-ASIN(f1)/(PI/2.0)*90.0; f2 = 90.0-ASIN(f2)/(PI/2.0)*90.0;
  drawline2((int) (l1*(double)(scale/100)+0.5)+1,
	    (int) (f1*(double)(scale/100)+0.5)+1,
	    (int) (l2*(double)(scale/100)+0.5)+1,
	    (int) (f2*(double)(scale/100)+0.5)+1, o);
}

drawleylines()
{
  color icosa, dodeca;
  float off = 0.0, phi, h, h1, h2, r, i;
  icosa = aspectcolor[5]; dodeca = hilite;
  phi = (sqrt(5.0)+1.0)/2.0;
  h = 1.0/(phi*2.0-1.0);
  for (i = off; i < DEGREES+off; i += 72.0) {
    drawleyline(i, h, i+72.0, h, icosa);
    drawleyline(i-36.0, -h, i+36.0, -h, icosa);
    drawleyline(i, h, i, 1.0, icosa);
    drawleyline(i+36.0, -h, i+36.0, -1.0, icosa);
    drawleyline(i, h, i+36.0, -h, icosa);
    drawleyline(i, h, i-36.0, -h, icosa);
  }
  r = 1.0/sqrt(3.0)/phi/cos(DTOR(54.0));
  h2 = sqrt(1.0-r*r); h1 = h2/(phi*2.0+1.0);
  for (i = off; i < DEGREES+off; i += 72.0) {
    drawleyline(i-36.0, h2, i+36.0, h2, dodeca);
    drawleyline(i, -h2, i+72.0, -h2, dodeca);
    drawleyline(i+36.0, h2, i+36.0, h1, dodeca);
    drawleyline(i, -h2, i, -h1, dodeca);
    drawleyline(i+36.0, h1, i+72.0, -h1, dodeca);
    drawleyline(i+36.0, h1, i, -h1, dodeca);
  }
}

drawworld()
{
  char *nam, *loc, *lin;
  int lon, lat, x, y, xold, yold, i;
  color c;
  if (chartx > BITMAPX) {
    fprintf(stderr, "Scale factor of %d too large for worldmap.\n", scale);
    exit(1);
  }
  if (xbitmap)
    block(0, 0, chartx - 1, charty - 1, off);
  rectall(1, 1, hilite);
  while (readworlddata(&nam, &loc, &lin)) {
    i = nam[0]-'0';
    c = modex == 'l' ? on : (i ? rainbowcolor[i] : maincolor[6]);
    lon = (loc[0] == '+' ? 1 : -1)*
      ((loc[1]-'0')*100 + (loc[2]-'0')*10 + (loc[3]-'0'));
    lat = (loc[4] == '+' ? 1 : -1)*((loc[5]-'0')*10 + (loc[6]-'0'));
    xold = x = 180-lon;
    yold = y = 91-lat;
    for (i = 0; lin[i]; i++) {
      if (lin[i] == 'L' || lin[i] == 'H' || lin[i] == 'G')
	x--;
      else if (lin[i] == 'R' || lin[i] == 'E' || lin[i] == 'F')
	x++;
      if (lin[i] == 'U' || lin[i] == 'H' || lin[i] == 'E')
	y--;
      else if (lin[i] == 'D' || lin[i] == 'G' || lin[i] == 'F')
	y++;
      if (x > 360) {
	x = 1;
	xold = 0;
      }
      drawline(xold*scale/100, yold*scale/100,
	       x*scale/100, y*scale/100, c, 0);
      if (x < 1)
	x = 360;
      xold = x; yold = y;
    }
  }
}

drawline2(xold, yold, xnew, ynew, o)
int xold, yold, xnew, ynew;
bit o;
{
  int xmid, ymid, i;
  if (xold < 0) {
    drawpoint(xnew, ynew, o);
    return;
  }
  xmid = 180*scale/100;
  if (dabs((double)(xnew-xold)) < (double) xmid) {
    drawline(xold, yold, xnew, ynew, o, 0);
    return;
  }
  i = xold < xmid ? xold+chartx-xnew-2 : xnew+chartx-xold-2;
  ymid = yold+(int)((double)(ynew-yold)*
    (xold < xmid ? (double)(xold-1) : (double)(chartx-xold-2))/(double)i);
  drawline(xold, yold, xold < xmid ? 1 : chartx-2, ymid, o, 0);
  drawline(xnew < xmid ? 1 : chartx-2, ymid, xnew, ynew, o, 0);
}

fillsymbolline(symbol)
double *symbol;
{
  double orb = DEFORB*1.35*(double)scale/100.0,
    max = DEGREES*(double)scale/100.0, temp;
  int i, j, k = 1, l, k1, k2;
  for (l = 0; k && l < 100; l++) {
    k = 0;
    for (i = 1; i <= TOTAL*2; i++) {
      if (proper((i+1)/2) && symbol[i] >= 0.0) {
	k1 = max-symbol[i]; k2 = -symbol[i];
	for (j = 1; j <= THINGS*2; j++) {
	  if (proper((j+1)/2) && i != j) {
	    temp = symbol[j]-symbol[i];
	    if (temp<k1 && temp>=0.0)
	      k1 = temp;
	    else if (temp>k2 && temp<=0.0)
	      k2 = temp;
	  }
	}
	if (k2>-orb && k1>orb) {
	  k = 1; symbol[i] = symbol[i]+orb*0.51+k2*0.49;
	} else if (k1<orb && k2<-orb) {
	  k = 1; symbol[i] = symbol[i]-orb*0.51+k1*0.49;
	} else if (k2>-orb && k1<orb) {
	  k = 1; symbol[i] = symbol[i]+(k1+k2)*0.5;
	}
      }
    }
  }
}

xchartastrograph()
{
  double planet1[TOTAL+1], planet2[TOTAL+1],
    end1[TOTAL*2+1], end2[TOTAL*2+1],
    symbol1[TOTAL*2+1], symbol2[TOTAL*2+1],
    lon = deflong, longm, x, y, z, ad, oa, am, od, dm, lat;
  int unit, lat1 = -60, lat2 = 75, y1, y2, xold1, xold2, i, j, k;
  unit = scale/100;
  y1 = (91-lat1)*scale/100.0;
  y2 = (91-lat2)*scale/100.0;
  block(1, 1, chartx-2, y2-1, off);
  block(1, charty-2, chartx-2, y1+1, off);
  drawline(0, charty/2, chartx-2, charty/2, hilite, 4);
  drawline(1, y2, chartx-2, y2, on, 0);
  drawline(1, y1, chartx-2, y1, on, 0);
  for (i = 1; i <= total*2; i++)
    end1[i] = end2[i] = -1000.0;
  for (i = lat1; i <= lat2; i += 5) {
    j = (91-i)*scale/100.0;
    k = 2+(i/10*10 == i ? 1 : 0)+(i/30*30 == i ? 2 : 0);
    drawline(1, j, k, j, hilite, 0);
    drawline(chartx-2, j, chartx-1-k, j, hilite, 0);
  }
  for (i = -180; i < 180; i += 5) {
    j = (180-i)*scale/100.0;
    k = 2+(i/10*10 == i ? 1 : 0)+(i/30*30 == i ? 2 : 0)+(i/90*90 == i ? 1 : 0);
    drawline(j, y2+1, j, y2+k, hilite, 0);
    drawline(j, y1-1, j, y1-k, hilite, 0);
  }
  for (i = 1; i <= total; i++) {
    planet1[i] = DTOR(planet[i]);
    planet2[i] = DTOR(planetalt[i]);
    ecltoequ(&planet1[i], &planet2[i]);
  }
  if (lon < 0.0)
    lon += DEGREES;
  for (i = 1; i <= total; i++) if (proper(i)) {
    x = planet1[18]-planet1[i];
    if (x < 0.0)
      x += 2.0*PI;
    if (x > PI)
      x -= 2.0*PI;
    z = lon+RTOD(x);
    if (z > 180.0)
      z -= DEGREES;
    j = (int) ((180.0-z)*(double)scale/100.0);
    drawline(j, y1+unit*4, j, y2-unit*1, elemcolor[1], 0);
    end2[i*2-1] = (double) j;
    y = RTOD(planet2[i]);
    k = (int) ((91.0-y)*(double)scale/100.0);
    block(j-1, k-1, j+1, k+1, hilite);
    block(j, k, j, k, off);
    if (!bonus) {
      j += 180*scale/100;
      if (j > chartx-2)
	j -= (chartx-2);
      end1[i*2-1] = (double) j;
      drawline(j, y1+unit*2, j, y2-unit*2, elemcolor[3], 0);
    }
  }
  longm = DTOR(mod(RTOD(planet1[18])+lon));
  if (!bonus) for (i = 1; i <= total; i++) if (proper(i)) {
    xold1 = xold2 = -1000;
    for (lat = (double) lat1; lat <= (double) lat2;
	 lat += 100.0/(double)scale) {
      j = (int) ((91.0-lat)*(double)scale/100.0);
      ad = tan(planet2[i])*tan(DTOR(lat));
      if (ad*ad > 1.0)
	ad = 1000.0;
      else {
	ad = ASIN(ad);
	oa = planet1[i]-ad;
	if (oa < 0.0)
	  oa += 2.0*PI;
	am = oa-PI/2.0;
	if (am < 0.0)
	  am += 2.0*PI;
	z = longm-am;
	if (z < 0.0)
	  z += 2.0*PI;
	if (z > PI)
	  z -= 2.0*PI;
	z = RTOD(z);
	k = (int) ((180.0-z)*(double)scale/100.0);
	drawline2(xold1, j+1, k, j, elemcolor[0]);
	if (lat == (double) lat1) {
	  drawline(k, y1, k, y1+unit*4, elemcolor[0], 0);
	  end2[i*2] = (double) k;
	}
	xold1 = k;
      }
      if (ad == 1000.0) {
	if (xold1 >= 0) {
	  drawline2(xold1, j+1, xold2, j+1, gray);
	  lat = 90.0;
	}
      } else {
	od = planet1[i]+ad;
	dm = od+PI/2.0;
	z = longm-dm;
	if (z < 0.0)
	  z += 2.0*PI;
	if (z > PI)
	  z -= 2.0*PI;
	z = RTOD(z);
	k = (int) ((180.0-z)*(double)scale/100.0);
	drawline2(xold2, j+1, k, j, elemcolor[2]);
	if (xold2 < 0)
	  drawline2(xold1, j, k, j, elemcolor[2]);
	if (lat == (double) lat1)
	  drawline(k, y1, k, y1+unit*2, elemcolor[2], 0);
	xold2 = k;
      }
    }
    if (ad != 1000.0) {
      drawline(xold1, y2, xold1, y2-unit*1, elemcolor[0], 0);
      drawline(k, y2, k, y2-unit*2, elemcolor[2], 0);
      end1[i*2] = (double) k;
    }
    if (!xbitmap)
      Xcolor(on);
  }
  for (i = 1; i <= total*2; i++) {
    symbol1[i] = end1[i];
    symbol2[i] = end2[i];
  }
  fillsymbolline(symbol1);
  fillsymbolline(symbol2);
  for (i = 1; i <= total*2; i++) {
    j = (i+1)/2;
    if (proper(j)) {
      if ((turtlex = (int) symbol1[i]) > 0) {
	drawline((int) end1[i], y2-unit*2, (int) symbol1[i], y2-unit*4,
		 ret[j] ? gray : on, (ret[i] < 0.0 ? 1 : 0) - xcolor);
	drawobject(j, turtlex, y2-unit*10);
      }
      if ((turtlex = (int) symbol2[i]) > 0) {
	drawline((int) end2[i], y1+unit*4, (int) symbol2[i], y1+unit*8,
		 ret[j] ? gray : on, (ret[i] < 0.0 ? 1 : 0) - xcolor);
	drawobject(j, turtlex, y1+unit*14);
	turtle(objectdraw[i & 1 ? 18 : 19], (int) symbol2[i],
	       y1+unit*24, objectcolor[j]);
      }
    }
  }
}

xchartaspectgrid()
{
  int unit, siz, x, y, i, j, k;
  if (chartx > BITMAPX) {
    fprintf(stderr, "Scale factor of %d too large for aspect grid.\n", scale);
    exit(1);
  }
  unit = CELLSIZE*scale/100; siz = objects*unit;
  if (xbitmap)
    block(0, 0, chartx - 1, charty - 1, off);
  creategrid(TRUE+bonus);
  for (y = 1, j = 0; y <= objects; y++) {
    do {
      j++;
    } while (ignore[j] && j <= total);
    drawline(0, y*unit, siz, y*unit, gray, 1-xcolor);
    drawline(y*unit, 0, y*unit, siz, gray, 1-xcolor);
    if (j <= total) for (x = 1, i = 0; x <= objects; x++) {
      do {
	i++;
      } while (ignore[i] && i <= total);
      if (i <= total) {
	turtlex = x*unit-unit/2;
	turtley = y*unit-unit/2 - (scale > 200 ? 5 : 0);
	if (bonus ? x > y : x < y)
	  turtle(aspectdraw[gridname[i][j]], turtlex,
		 turtley, aspectcolor[gridname[i][j]]);
	else if (bonus ? x < y : x > y)
	  turtle(signdraw[gridname[i][j]], turtlex,
		 turtley, elemcolor[(gridname[i][j]-1)%4]);
	else {
	  edge((y-1)*unit, (y-1)*unit, y*unit, y*unit, hilite);
	  drawobject(i, turtlex, turtley);
	}
	if (!xbitmap && scale > 200 && label) {
	  k = abs(grid[i][j]);
	  if (bonus ? x > y : x < y) {
	    if (gridname[i][j])
	      sprintf(string, "%c%d %d%d'", k != grid[i][j] ? '-' : '+',
		      k/60, (k%60)/10, (k%60)%10);
	    else
	      sprintf(string, "");
	  } else if (bonus ? x < y : x > y)
	    sprintf(string, "%d%d %d%d'",
		    (k/60)/10, (k/60)%10, (k%60)/10, (k%60)%10);
	  else {
	    Xcolor(elemcolor[(gridname[i][j]-1)%4]);
	    sprintf(string, "%c%c%c %d%d", SIGNAM(gridname[i][j]), k/10, k%10);
	  }
	  XDrawImageString(disp, pixmap, gc, x*unit-unit/2-FONT_X*3, y*unit-3,
			   string, strlen(string));
	}
      }
    }
  }
  rectall(1, 1, hilite);
}

xchartrelationaspectgrid()
{
  int unit, siz, x, y, i, j, k, l;
  if (chartx > BITMAPX) {
    fprintf(stderr, "Scale factor of %d too large for aspect grid.\n", scale);
    exit(1);
  }
  unit = CELLSIZE*scale/100; siz = (objects+1)*unit;
  if (xbitmap)
    block(0, 0, chartx - 1, charty - 1, off);
  createrelationgrid(TRUE+bonus);
  for (y = 0, j = -1; y <= objects; y++) {
    do {
      j++;
    } while (ignore[j] && j <= total);
    drawline(0, (y+1)*unit, siz, (y+1)*unit, gray, 1-xcolor);
    drawline((y+1)*unit, 0, (y+1)*unit, siz, gray, 1-xcolor);
    edge(0, y*unit, unit, (y+1)*unit, hilite);
    edge(y*unit, 0, (y+1)*unit, unit, hilite);
    if (j <= total) for (x = 0, i = -1; x <= objects; x++) {
      do {
	i++;
      } while (ignore[i] && i <= total);
      if (i <= total) {
	turtlex = x*unit+unit/2;
	turtley = y*unit+unit/2 - (!xbitmap && scale > 200 ? 5 : 0);
	if (y == 0 || x == 0) {
	  if (x+y > 0)
	    drawobject(j == 0 ? i : j, turtlex, turtley);
	} else {
	  if (!bonus)
	    turtle(aspectdraw[gridname[i][j]], turtlex, turtley,
		   aspectcolor[gridname[i][j]]);
	  else
	    turtle(signdraw[gridname[i][j]], turtlex, turtley,
		   elemcolor[(gridname[i][j]-1)%4]);
	}
	if (!xbitmap && scale > 200 && label) {
	  if (y == 0 || x == 0) {
	    if (x+y > 0) {
	      k = (int)((y == 0 ? planet2[i] : planet1[j])/30.0)+1;
	      l = (int)((y == 0 ? planet2[i] : planet1[j])-(double)(k-1)*30.0);
	      Xcolor(elemcolor[(k-1)%4]);
	      sprintf(string, "%c%c%c %d%d", SIGNAM(k), l/10, l%10);
	    } else {
	      Xcolor(hilite);
	      sprintf(string, "1v 2->");
	    }
	  } else {
	    k = abs(grid[i][j]);
	    if (!bonus)
	      if (gridname[i][j])
		sprintf(string, "%c%d %d%d'", k != grid[i][j] ? '-' : '+',
			k/60, (k%60)/10, (k%60)%10);
	      else
		sprintf(string, "");
	    else
	      sprintf(string, "%d%d %d%d'",
		      (k/60)/10, (k/60)%10, (k%60)/10, (k%60)%10);
	  }
	  XDrawImageString(disp, pixmap, gc, x*unit+unit/2-FONT_X*3,
			   (y+1)*unit-3, string, strlen(string));
	}
      }
    }
  }
  rectall(1, 1, hilite);
}

drawspot(x, y, o)
int x, y;
bit o;
{
  drawpoint(x, y, o);
  drawpoint(x, y-1, o);
  drawpoint(x-1, y, o);
  drawpoint(x+1, y, o);
  drawpoint(x, y+1, o);
}

xcharthorizon()
{
  double lon, lat,
    lonz[TOTAL+1], latz[TOTAL+1], azi[TOTAL+1], alt[TOTAL+1];
  int x[TOTAL+1], y[TOTAL+1], m[TOTAL+1], n[TOTAL+1],
    cx = chartx / 2, cy = charty / 2, unit = 12*scale/100,
    x1, y1, x2, y2, xs, ys, i, j, k, l;
  if (xbitmap)
    block(0, 0, chartx - 1, charty - 1, off);
  rectall(1, 1, hilite);
  x1 = y1 = unit/2; x2 = chartx-x1; y2 = charty-y1;
  xs = x2-x1; ys = y2-y1;
  for (i = 0; i < 180; i += 5) {
    j = y1+(int)((double)(i*ys)/180.0);
    k = 2+(i/10*10 == i ? 1 : 0)+(i/30*30 == i ? 2 : 0);
    drawline(x1+1, j, x1+1+k, j, hilite, 0);
    drawline(x2-1, j, x2-1-k, j, hilite, 0);
  }
  for (i = 0; i < 360; i += 5) {
    j = x1+(int)((double)(i*xs)/DEGREES);
    k = 2+(i/10*10 == i ? 1 : 0)+(i/30*30 == i ? 2 : 0);
    drawline(j, y1+1, j, y1+1+k, hilite, 0);
    drawline(j, y2-1, j, y2-1-k, hilite, 0);
  }
  drawline(cx, y1, cx, y2, gray, 1);
  drawline((cx+x1)/2, y1, (cx+x1)/2, y2, gray, 1);
  drawline((cx+x2)/2, y1, (cx+x2)/2, y2, gray, 1);
  edge(x1, y1, x2, y2, on);
  drawline(x1, cy, x2, cy, on, 1);
  lon = DTOR(mod(Lon)); lat = DTOR(Lat);
  for (i = 1; i <= total; i++) {
    lonz[i] = DTOR(planet[i]); latz[i] = DTOR(planetalt[i]);
    ecltoequ(&lonz[i], &latz[i]);
  }
  for (i = 1; i <= total; i++) if (proper(i)) {
    lonz[i] = DTOR(mod(RTOD(lonz[18]-lonz[i]+PI/2.0)));
    equtolocal(&lonz[i], &latz[i], PI/2.0-lat);
    azi[i] = DEGREES-RTOD(lonz[i]); alt[i] = RTOD(latz[i]);
    x[i] = x1+(int)((double)xs*(mod(90.0-azi[i]))/DEGREES+0.5);
    y[i] = y1+(int)((double)ys*(90.0-alt[i])/180.0+0.5);
    m[i] = x[i]; n[i] = y[i]+unit/2;
  }
  for (i = 1; i <= total; i++) if (proper(i)) {
    k = l = chartx+charty;
    for (j = 1; j < i; j++) if (proper(j)) {
      k = MIN(k, abs(m[i]-m[j])+abs(n[i]-n[j]));
      l = MIN(l, abs(m[i]-m[j])+abs(n[i]-unit-n[j]));
    }
    if (k < unit || l < unit)
      if (k < l)
	n[i] -= unit;
  }
  for (i = total; i >= 1; i--) if (proper(i))
    drawobject(i, m[i], n[i]);
  for (i = total; i >= 1; i--) if (proper(i)) {
    if (!xbitmap)
      Xcolor(objectcolor[i]);
    if (!bonus || i > BASE)
      drawpoint(x[i], y[i], objectcolor[i]);
    else
      drawspot(x[i], y[i], objectcolor[i]);
  }
}

double angle(x, y)
double x, y;
{
  double a;
  a = atan(y/x);
  if (a < 0.0)
    a += PI;
  if (y < 0.0)
    a += PI;
  return mod(RTOD(a));
}

xchartspace()
{
  int x[TOTAL+1], y[TOTAL+1], m[TOTAL+1], n[TOTAL+1],
    cx = chartx / 2, cy = charty / 2, unit = 12*scale/100, i, j, k, l;
  double sx, sy, sz = 30.0, x1, y1, a;
  if (scale < 200)
    sz = 90.0;
  else if (scale > 200)
    sz = 6.0;
  sx = (double)cx/sz; sy = (double)cy/sz;
  if (xbitmap)
    block(0, 0, chartx - 1, charty - 1, off);
  for (i = 0; i <= total; i++) if (proper(i)) {
    if (centerplanet == 0)
      j = i < 2 ? 1-i : i;
    else if (centerplanet == 1)
      j = i;
    else
      j = i == 0 ? centerplanet : (i == centerplanet ? 0 : i);
    x1 = spacex[j]; y1 = spacey[j];
    x[i] = cx-(int)(x1*sx); y[i] = cy+(int)(y1*sy);
    m[i] = x[i]; n[i] = y[i]+unit/2;
  }
  for (i = 0; i <= total; i++) if (proper(i)) {
    k = l = chartx+charty;
    for (j = 0; j < i; j++) if (proper(j)) {
      k = MIN(k, abs(m[i]-m[j])+abs(n[i]-n[j]));
      l = MIN(l, abs(m[i]-m[j])+abs(n[i]-unit-n[j]));
    }
    if (k < unit || l < unit)
      if (k < l)
	n[i] -= unit;
  }
  a = mod(angle(spacex[3], spacey[3])-planet[3]);
  for (i = 0; i < SIGNS; i++) {
    k = cx+2*(int)((double)cx*COSD((double)i*30.0+a));
    l = cy+2*(int)((double)cy*SIND((double)i*30.0+a));
    drawclip(cx, cy, k, l, gray, 1);
  }
  for (i = total; i >= 0; i--) if (proper(i) && ISLEGAL(m[i], n[i]))
    drawobject(i, m[i], n[i]);
  for (i = total; i >= 0; i--) if (proper(i) && ISLEGAL(x[i], y[i])) {
    if (!xbitmap)
      Xcolor(objectcolor[i]);
    if (!bonus || i > BASE)
      drawpoint(x[i], y[i], objectcolor[i]);
    else
      drawspot(x[i], y[i], objectcolor[i]);
  }
  rectall(1, 1, hilite);
}

void xchart()
{
  int i, j, k, l;
  switch (modex) {
  case 'c':
    if (relation != -1)
      xchartwheel();
    else
      xchartwheelrelation();
    break;
  case 'l':
    drawworld();
    xchartastrograph();
    break;
  case 'a':
    if (relation != -1)
      xchartaspectgrid();
    else
      xchartrelationaspectgrid();
    break;
  case 'z':
    xcharthorizon();
    break;
  case 's':
    xchartspace();
    break;
  case 'w':
    drawworld();
    if (bonus)
      drawleylines();
    break;
  case 'p':
  case 'g':
    drawglobe(degree);
    break;
  }
  if (!xbitmap) {
    Xcolor(on);
    if (xtext && modex != 'w' && modex != 'p' && modex != 'g') {
      if (Mon == -1)
	sprintf(string, "(no time or space)");
      else if (relation == 2)
	sprintf(string, "(composite)");
      else {
	i = (int) Mon; l = (int) ((Tim-floor(Tim))*100.0+0.1);
	sprintf(string, "%2.0f %c%c%c %.0f %2.0f:%d%d (%.2f GMT) %s",
		Day, monthname[i][0], monthname[i][1], monthname[i][2], Yea,
		floor(Tim), l/10, l%10, -Zon, stringlocation(Lon, Lat, 100.0));
      }
      XDrawImageString(disp, pixmap, gc, (chartx-strlen(string)*FONT_X)/2,
		       charty-3, string, strlen(string));
    }
  }
}
#endif

/**/
