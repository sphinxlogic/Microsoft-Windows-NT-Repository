/*
** Astrolog (Version 2.25) File: formulas.c
**
** IMPORTANT: The planetary calculation routines used in this program
** have been Copyrighted and the core of this program is basically a
** conversion to C of the routines created by James Neely as listed in
** Michael Erlewine's 'Manual of Computer Programming for Astrologers',
** available from Matrix Software. The copyright gives us permission to
** use the routines for our own purposes but not to sell them or profit
** from them in any way.
*/

#include "astrolog.h"

double MC, Asc, Vtx, OB,
  A, R, B, Q, L, G, O, RA, R1, R2, AZ, FF, LO, OA,
  S, S1, S2, AP, IN, AN, XX, YY, ZZ, NU, BR, AB, C, P, T0[4];

double geo[TOTAL+1], geoalt[TOTAL+1], georet[TOTAL+1],
  helio[TOTAL+1], helioalt[TOTAL+1], helioret[TOTAL+1],
  planet1[TOTAL+1], planet2[TOTAL+1], planetalt1[TOTAL+1], planetalt2[TOTAL+1],
  house1[SIGNS+1], house2[SIGNS+1], ret1[TOTAL+1], ret2[TOTAL+1];


/*
*******************************************************************************
** Specific calculations
*******************************************************************************
*/

void juliantomdy(JD, mon, day, yea)
double JD, *mon, *day, *yea;
{
  double L, N, IT, JT, K, IK;
  L  = floor(JD+0.5)+68569.0;
  N  = floor(4.0*L/146097.0);
  L  = L-floor((146097.0*N+3.0)/4.0);
  IT = floor(4000.0*(L+1.0)/1461001.0);
  L  = L-floor(1461.0*IT/4.0)+31.0;
  JT = floor(80.0*L/2447.0);
  K  = L-floor(2447.0*JT/80.0);
  L  = floor(JT/11.0);
  JT = JT+2.0-12.0*L;
  IK = 100.0*(N-49.0)+IT+L;
  *mon = JT; *day = K; *yea = IK;
}

double mdytojulian(mon, day, yea)
double mon, day, yea;
{
  double im, j;
  im = 12.0*(yea+4800.0)+mon-3.0;
  j = (2.0*(im-floor(im/12.0)*12.0)+7.0+365.0*im)/12.0;
  j = floor(j)+day+floor(im/48.0)-32083.0;
  if (j > 2299171.0)
    j += floor(im/4800.0)-floor(im/1200.0)+38.0;
  return j;
}

double processinput(var)
int var;
{
  double Off, Ln;
  F = sgn(F)*floor(dabs(F))+(dabs(F)-floor(dabs(F)))*100.0/60.0+dectodeg(X);
  L5 = dectodeg(L5);
  LA = DTOR(dectodeg(LA));
  if (var) {
    JD = mdytojulian(M, D+Delta, Y);
    if (!progress)
      T = ((JD-2415020.0)+F/24.0-0.5)/36525.0;
    else
      T = (((Jdp-JD)/progday+JD)-2415020.0+F/24.0-0.5)/36525.0;
  }
  OB = DTOR(23.452294-0.0130125*T);
  Ln = mod((933060-6962911*T+7.5*T*T)/3600.0);
  Off = (259205536.0*T+2013816.0)/3600.0;
  Off = 17.23*sin(DTOR(Ln))+1.27*sin(DTOR(Off))-(5025.64+1.11*T)*T;
  Off = (Off-84038.27)/3600.0;
  SD = siderial ? Off : 0.0;
  return Off;
}

poltorec(A, R, X, Y)
double A, R, *X, *Y;
{
  if (A == 0.0)
    A = 1.7453E-09;
  *X = R*cos(A);
  *Y = R*sin(A);
}

rectopol(X, Y, A, R)
double X, Y, *A, *R;
{
  if (Y == 0.0)
    Y = 1.7453E-09;
  *R = sqrt(X*X+Y*Y);
  *A = atan(Y/X);
  if (*A < 0.0)
    *A += PI;
  if (Y < 0.0)
    *A += PI;
}

rectosph()
{
  A = B; R = 1.0;
  poltorec(A, R, &X, &Y);
  Q = Y; R = X; A = L;
  poltorec(A, R, &X, &Y);
  G = X; X = Y; Y = Q;
  rectopol(X, Y, &A, &R);
  A += O;
  poltorec(A, R, &X, &Y);
  Q = ASIN(Y);
  Y = X; X = G;
  rectopol(X, Y, &A, &R);
  if (A < 0.0)
    A += 2*PI;
  G = A;
}

void coorxform(azi, alt, tilt)
double *azi, *alt, tilt;
{
  double x, y, a1, l1;
  x = cos(*alt)*sin(*azi)*cos(tilt);
  y = sin(*alt)*sin(tilt);
  x -= y;
  a1 = cos(*alt);
  y = cos(*alt)*cos(*azi);
  l1 = atan(x/y);
  if (l1 < 0.0)
    l1 += PI;
  if (x < 0.0)
    l1 += PI;
  a1 = ASIN(a1*sin(*azi)*sin(tilt)+sin(*alt)*cos(tilt));
  *azi = l1; *alt = a1;
}

computevariables()
{
  RA = DTOR(mod((6.6460656+2400.0513*T+2.58E-5*T*T+F)*15.0-L5));
  R2 = RA; O = -OB; B = LA; A = R2; R = 1.0;
  poltorec(A, R, &X, &Y);
  X *= cos(O);
  rectopol(X, Y, &A, &R);
  MC = mod(SD+RTOD(A));
  L = R2;
  rectosph();
  AZ = mod(SD+mod(G+PI/2.0));
  L= R2+PI; B = PI/2.0-dabs(B);
  if (LA < 0.0)
    B = -B;
  rectosph();
  Vtx = mod(SD+RTOD(G+PI/2.0));
}


/*
*******************************************************************************
** House cusp calculations
*******************************************************************************
*/

int inhouseplace(point)
double point;
{
  int i = 0;
  do {
    i++;
  } while (!(i >= SIGNS ||
	     (point >= house[i] && point < house[mod12(i+1)]) ||
	     (house[i] > house[mod12(i+1)] &&
	      (point >= house[i] || point < house[mod12(i+1)]))));
  return i;
}

void houseplace()
{
  int i;
  for (i = 1; i <= total; i++)
    inhouse[i] = inhouseplace(planet[i]);
}

double midheaven()
{
  double MC;
  MC = atan(tan(RA)/cos(OB));
  if (MC < 0.0)
    MC += PI;
  if (RA > PI)
    MC += PI;
  return mod(RTOD(MC)+SD);
}

double ascendant()
{
  double Asc;
  Asc = atan(cos(RA)/(-sin(RA)*cos(OB)-tan(LA)*sin(OB)));
  if (Asc < 0.0)
    Asc += PI;
  if (cos(RA) < 0.0)
    Asc += PI;
  return mod(RTOD(Asc)+SD);
}

placidus_cusp()
{
  int i;
  X = -1.0;
  if (Y == 1.0)
    X = 1.0;
  for (i = 1; i <= 10; i++) {
    XX = ACOS(X*sin(R1)*tan(OB)*tan(LA));
    if (XX < 0.0)
      XX += PI;
    R2 = RA+(XX/FF);
    if (Y == 1.0)
      R2 = RA+PI-(XX/FF);
    R1 = R2;
  }
  LO = atan(tan(R1)/cos(OB));
  if (LO < 0.0)
    LO += PI;
  if (sin(R1) < 0.0)
    LO += PI;
  LO = RTOD(LO);
}

placidus()
{
  int i;
  Y = 0.0;
  house[4] = mod(MC+180.0-SD);
  house[1] = mod(Asc-SD);
  R1 = RA+DTOR(30.0);  FF=3.0; placidus_cusp(); house[5]=mod(LO+180.0);
  R1 = RA+DTOR(60.0);  FF=1.5; placidus_cusp(); house[6]=mod(LO+180.0);
  R1 = RA+DTOR(120.0); Y=1.0;  placidus_cusp(); house[2]=LO;
  R1 = RA+DTOR(150.0); FF=3.0; placidus_cusp(); house[3]=LO;
  for (i = 1; i <= SIGNS; i++) {
    house[i] = mod(house[i]+SD);
    if (i > 6)
      house[i] = mod(house[i-6]+180.0);
  }
}

koch()
{
  double A1, A2, A3, KN;
  int i;
  A1 = ASIN(sin(RA)*tan(LA)*tan(OB));
  for (i = 1; i <= SIGNS; i++) {
    D = mod(60.0+30.0*(double)i);
    A2 = D/90.0-1.0; KN = 1.0;
    if (D >= 180.0) {
      KN = -1.0;
      A2 = D/90.0-3.0;
    }
    A3 = DTOR(mod(RTOD(RA)+D+A2*RTOD(A1)));
    X = atan(sin(A3)/(cos(A3)*cos(OB)-KN*tan(LA)*sin(OB)));
    if (X < 0.0)
      X += PI;
    if (sin(A3) < 0.0)
      X += PI;
    house[i] = mod(RTOD(X)+SD);
  }
}

equal()
{
  int i;
  for (i = 1; i <= SIGNS; i++) {
    house[i] = mod(Asc-30.0+30.0*(double)i);
  }
}

campanus()
{
  double KO, DN;
  int i;
  for (i = 1; i <= SIGNS; i++) {
    KO = DTOR(60.000001+30.0*(double)i);
    DN = atan(tan(KO)*cos(LA));
    if (DN < 0.0)
      DN += PI;
    if (sin(KO) < 0.0)
      DN += PI;
    Y = sin(RA+DN);
    X = cos(RA+DN)*cos(OB)-sin(DN)*tan(LA)*sin(OB);
    X = atan(Y/X);
    if (X < 0.0)
      X += PI;
    if (Y < 0.0)
      X += PI;
    house[i] = mod(RTOD(X)+SD);
  }
}

meridian()
{
  int i;
  for (i = 1; i <= SIGNS; i++) {
    D = DTOR(60.0+30.0*(double)i);
    Y = sin(RA+D);
    X = atan(Y/(cos(RA+D)*cos(OB)));
    if (X < 0.0)
      X += PI;
    if (Y < 0.0)
      X += PI;
    house[i] = mod(RTOD(X)+SD);
  }
}

regiomontanus()
{
  int i;
  for (i = 1; i <= SIGNS; i++) {
    D = DTOR(60.0+30.0*i);
    Y = sin(RA+D);
    X = atan(Y/(cos(RA+D)*cos(OB)-sin(D)*tan(LA)*sin(OB)));
    if (X < 0.0)
      X += PI;
    if (Y < 0.0)
      X += PI;
    house[i] = mod(RTOD(X)+SD);
  }
}

porphyry()
{
  int i;
  X = Asc-MC;
  if (X < 0.0)
    X += 360;
  Y = X/3.0;
  for (i = 1; i <= 2; i++)
    house[i+4] = mod(180.0+MC+i*Y);
  X = mod(180.0+MC)-Asc;
  if (X < 0.0)
    X += 360;
  house[1]=Asc;
  Y = X/3.0;
  for (i = 1; i <= 3; i++)
    house[i+1] = mod(Asc+i*Y);
  for (i = 1; i <= 6; i++)
    house[i+6] = mod(house[i]+180.0);
}

morinus()
{
  int i;
  for (i = 1; i <= SIGNS; i++) {
    D = DTOR(60.0+30.0*(double)i);
    Y = sin(RA+D)*cos(OB);
    X = atan(Y/cos(RA+D));
    if (X < 0.0)
      X += PI;
    if (Y < 0.0)
      X += PI;
    house[i] = mod(RTOD(X)+SD);
  }
}

topocentric_cusp()
{
  X = atan(tan(LA)/cos(OA));
  Y = X+OB;
  LO = atan(cos(X)*tan(OA)/cos(Y));
  if (LO < 0.0)
    LO += PI;
  if (sin(OA) < 0.0)
    LO += PI;
}

topocentric()
{
  double TL, P1, P2, LT;
  int i;
  modulus = 2.0*PI;
  house[4] = mod(DTOR(MC+180.0-SD));
  TL = tan(LA); P1 = atan(TL/3.0); P2 = atan(TL/1.5); LT = LA;
  LA = P1; OA = mod(RA+DTOR(30.0)); topocentric_cusp(); house[5] = mod(LO+PI);
  LA = P2; OA = mod(OA+DTOR(30.0)); topocentric_cusp(); house[6] = mod(LO+PI);
  LA = LT; OA = mod(OA+DTOR(30.0)); topocentric_cusp(); house[1] = LO;
  LA = P2; OA = mod(OA+DTOR(30.0)); topocentric_cusp(); house[2] = LO;
  LA = P1; OA = mod(OA+DTOR(30.0)); topocentric_cusp(); house[3] = LO;
  LA = LT; modulus = DEGREES;
  for (i = 1; i <= 6; i++) {
    house[i] = mod(RTOD(house[i])+SD);
    house[i+6] = mod(house[i]+180.0);
  }
}

null()
{
  int i;
  for (i = 1; i <= SIGNS; i++)
    house[i] = mod((double)(i-1)*30.0+SD);
}

houses(housesystem)
int housesystem;
{
  switch (housesystem) {
  case  1: koch();          break;
  case  2: equal();         break;
  case  3: campanus();      break;
  case  4: meridian();      break;
  case  5: regiomontanus(); break;
  case  6: porphyry();      break;
  case  7: morinus();       break;
  case  8: topocentric();   break;
  case  9: null();          break;
  default: placidus();
  }
}


/*
*******************************************************************************
** Planetary position calculations
*******************************************************************************
*/

double readthree()
{
  S = readplanetdata(FALSE); S1 = readplanetdata(FALSE);
  S2 = readplanetdata(FALSE);
  return S = DTOR(S+S1*T+S2*T*T);
}

rectosph2()
{
  rectopol(X, Y, &A, &R); A += AP; poltorec(A, R, &X, &Y);
  D = X; X = Y; Y = 0.0; rectopol(X, Y, &A, &R);
  A += IN; poltorec(A, R, &X, &Y);
  G = Y; Y = X; X = D; rectopol(X, Y, &A, &R); A += AN;
  if (A < 0.0)
    A += 2.0*PI;
  poltorec(A, R, &X, &Y);
}

errorcorrect(ind)
int ind;
{
  double U, V, W;
  int IK, IJ, errorindex;
  errorindex = errorcount[ind];
  for (IK = 1; IK <= 3; IK++) {
    if (ind == 6 && IK == 3) {
      T0[3]=0;
      return;
    }
    if (IK == 3)
      errorindex--;
    readthree(); A = 0.0;
    for (IJ = 1; IJ <= errorindex; IJ++) {
      U = readplanetdata(FALSE); V = readplanetdata(FALSE);
      W = readplanetdata(FALSE);
      A = A+DTOR(U)*cos((V*T+W)*PI/180.0);
    }
    T0[IK] = RTOD(S+A);
  }
}

processplanet(ind)
int ind;
{
  X = XX; Y = YY; rectopol(X, Y, &A, &R);
  C = RTOD(A)+NU-BR;
  if (ind == 1 && AB == 1.0)
    C = mod(C+180.0);
  C = mod(C+SD); Y = ZZ; X = R; rectopol(X, Y, &A, &R);
  P = RTOD(A);
}

planets()
{
  double AU, E, EA, E1, XK, XW, YW, XH[BASE+1], YH[BASE+1], ZH[BASE+1];
  int ind, i;
  readplanetdata(TRUE);
  for (ind = 1; ind <= (uranian ? BASE : PLANETS+1);
       ind += (ind == 1 ? 2 : (ind != PLANETS+1 ? 1 : 10))) {
    modulus = 2.0*PI;
    EA = M = mod(readthree());
    E = RTOD(readthree());
    for (i = 1; i <= 5; i++)
      EA = M+E*sin(EA);
    AU = readplanetdata(FALSE);
    E1 = 0.01720209/(pow(AU,1.5)*(1.0-E*cos(EA)));
    XW = -AU*E1*sin(EA);
    YW = AU*E1*pow(1.0-E*E,0.5)*cos(EA);
    AP = readthree(); AN = readthree(); IN = readthree();
    X = XW; Y = YW; rectosph2();
    XH[ind] = X; YH[ind] = Y; ZH[ind] = G;
    modulus = DEGREES;
    if (ind > 1) {
      XW = XH[ind]-XH[1]; YW = YH[ind]-YH[1];
    }
    X = AU*(cos(EA)-E); Y = AU*sin(EA)*pow(1.0-E*E,0.5); rectosph2();
    XX = X; YY = Y; ZZ = G;
    if (ind >= 6 && ind <= 10) {
      errorcorrect(ind); XX += T0[2]; YY += T0[1]; ZZ += T0[3];
    }
    helioret[ind] = XK = (XX*YH[ind]-YY*XH[ind])/(XX*XX+YY*YY);
    spacex[ind] = XX; spacey[ind] = YY; spacez[ind] = ZZ;
    BR = 0.0; processplanet(ind); helio[ind] = C; helioalt[ind] = P; AB = 1.0;
    if (ind > 1) {
      XX -= spacex[1]; YY -= spacey[1]; ZZ -= spacez[1];
      XK = (XX*YW-YY*XW)/(XX*XX+YY*YY);
    }
    BR = 0.0057756*sqrt(XX*XX+YY*YY+ZZ*ZZ)*RTOD(XK);
    georet[ind] = XK; processplanet(ind); geo[ind] = C; geoalt[ind] = P;
    if (!centerplanet) {
      planet[ind] = helio[ind]; planetalt[ind] = helioalt[ind];
      ret[ind] = helioret[ind];
    } else {
      planet[ind] = geo[ind]; planetalt[ind] = geoalt[ind];
      ret[ind] = georet[ind];
    }
    if (todisplay & 32768)
      ret[ind] = DTOR(ret[ind]/helioret[ind]);
  }
  spacex[0] = spacey[0] = spacez[0] = 0.0;
  ind = centerplanet;
  if (ind) {
    for (i = 0; i <= BASE; i++) if (i != 2 && i != ind) {
      spacex[i] -= spacex[ind]; spacey[i] -= spacey[ind];
      spacez[i] -= spacez[ind];
    }
    spacex[ind] = spacey[ind] = spacez[ind] = 0.0;
    swapdoub(&spacex[0], &spacex[ind]);
    swapdoub(&spacey[0], &spacey[ind]);
    swapdoub(&spacez[0], &spacez[ind]);
    swapdoub(&spacex[1], &spacex[ind]);
    swapdoub(&spacey[1], &spacey[ind]);
    swapdoub(&spacez[1], &spacez[ind]);
  }
  if (ind > 2)
    for (i = 1; i <= (uranian ? BASE : PLANETS+1);
	 i += (i == 1 ? 2 : (i != PLANETS+1 ? 1 : 10))) {
      XX = spacex[i]; YY = spacey[i]; ZZ = spacez[i];
      AB = 0.0; BR = 0.0; processplanet(i);
      planet[i] = C; planetalt[i] = P;
      ret[i] = (XX*(YH[i]-YH[ind])-YY*(XH[i]-XH[ind]))/(XX*XX+YY*YY);
    }
}


/*
*******************************************************************************
** Lunar position calculations
*******************************************************************************
*/

lunar(moonlo, moonla, nodelo, nodela)
double *moonlo, *moonla, *nodelo, *nodela;
{
  double LL, G, N, G1, D, L, ML, L1, MB, TN, M = 3600.0;
  LL = 973563.0+1732564379.0*T-4.0*T*T;
  G = 1012395.0+6189.0*T;
  N = 933060.0-6962911.0*T+7.5*T*T;
  G1 = 1203586.0+14648523.0*T-37.0*T*T;
  D = 1262655.0+1602961611.0*T-5.0*T*T;
  L = (LL-G1)/M; L1 = ((LL-D)-G)/M; F = (LL-N)/M; D = D/M; Y = 2.0*D;
  ML = 22639.6*SIND(L)-4586.4*SIND(L-Y)+2369.9*SIND(Y)+769.0*SIND(2.0*L)-
    669.0*SIND(L1)-411.6*SIND(2.0*F)-212.0*SIND(2.0*L-Y)-206.0*SIND(L+L1-Y);
  ML += 192.0*SIND(L+Y)-165.0*SIND(L1-Y)+148.0*SIND(L-L1)-125.0*SIND(D)-
    110.0*SIND(L+L1)-55.0*SIND(2.0*F-Y)-45.0*SIND(L+2.0*F)+ 40.0*SIND(L-2.0*F);
  *moonlo = G = mod((LL+ML)/M+SD);
  MB = 18461.5*SIND(F)+1010.0*SIND(L+F)-999.0*SIND(F-L)-624.0*SIND(F-Y)+
    199.0*SIND(F+Y-L)-167.0*SIND(L+F-Y);
  MB += 117.0*SIND(F+Y)+62.0*SIND(2.0*L+F)-
    33.0*SIND(F-Y-L)-32.0*SIND(F-2.0*L)-30.0*SIND(L1+F-Y);
  *moonla = MB =
    sgn(MB)*((dabs(MB)/M)/DEGREES-floor((dabs(MB)/M)/DEGREES))*DEGREES;
  /*TN = N+5392.0*SIND(2.0*F-Y)-541.0*SIND(L1)-442.0*SIND(Y)+423.0*SIND(2.0*F)-
    291.0*SIND(2.0*L-2.0*F);
  TN = mod(TN/M);*/
  *nodelo = N = mod(N/M+SD);
  *nodela = 0.0;
}


/*
*******************************************************************************
** Star position calculations
*******************************************************************************
*/

int strcmp(s1, s2)
char *s1, *s2;
{
  int i = 0;
  while (s1[i] == s2[i] && s1[i] != 0)
    i++;
  return (s1[i] == s2[i] ? 0 : (s1[i] > s2[i] ? 1 : -1));
}

void caststar(SD)
double SD;
{
  int i, j;
  double x, y, z;
  readstardata(TRUE);
  for (i = 1; i <= stars; i++) {
    x = readstardata(FALSE); y = readstardata(FALSE); z = readstardata(FALSE);
    planet[BASE+i] = DTOR(mod(x*DEGREES/24.0+y*15.0/60.0+z*0.25/60.0-SD));
    x = readstardata(FALSE); y = readstardata(FALSE); z = readstardata(FALSE);
    planetalt[BASE+i] = DTOR(x+y/60.0+z/60.0/60.0);
    equtoecl(&planet[BASE+i], &planetalt[BASE+i]);
    planet[BASE+i] = RTOD(planet[BASE+i]);
    planetalt[BASE+i] = RTOD(planetalt[BASE+i]);
    starname[i] = i;
  }
  if (universe > 1) for (i = 2; i <= stars; i++) {
    j = i-1;
    if (universe == 'n') while (j > 0 &&
      strcmp(objectname[BASE+starname[j]],
	     objectname[BASE+starname[j+1]]) > 0) {
      swapint(&starname[j], &starname[j+1]);
      j--;
    } else if (universe == 'b') while (j > 0 &&
      starbright[starname[j]] > starbright[starname[j+1]]) {
      swapint(&starname[j], &starname[j+1]);
      j--;
    } else if (universe == 'z') while (j > 0 &&
      planet[BASE+starname[j]] > planet[BASE+starname[j+1]]) {
      swapint(&starname[j], &starname[j+1]);
      j--;
    } else if (universe == 'l') while (j > 0 &&
      planetalt[BASE+starname[j]] < planetalt[BASE+starname[j+1]]) {
      swapint(&starname[j], &starname[j+1]);
      j--;
    }
  }
}


/*
*******************************************************************************
** Calculate chart for specific time
*******************************************************************************
*/

double castchart(var)
int var;
{
  int i, k;
  double housetemp[SIGNS+1], Off = 0.0, j;
  if (M == -1.0) {
    MC = planet[18]; Asc = planet[19]; Vtx = planet[20];
  } else {
    Off = processinput(var); computevariables();
    if (geodetic)
      RA = DTOR(mod(-L5));
    MC = midheaven(); Asc = ascendant();
    houses(housesystem);
    for (i = 1; i <= total; i++)
      ret[i] = 1.0;
    planets();
    lunar(&planet[2], &planetalt[2], &planet[16], &planetalt[16]);
    ret[16] = -1.0;
    j = planet[2]-planet[1]; j = dabs(j) < 90.0 ? j : j - sgn(j)*DEGREES;
    planet[17] = mod(j+Asc);
    planet[18] = MC; planet[19] = Asc; planet[20] = Vtx;
    planet[21] = house[11]; planet[22] = house[12];
    planet[23] = house[2];  planet[24] = house[3];
  }
  if (universe)
    caststar(siderial ? 0.0 : Off);
  if (multiplyfactor > 1)
    for (i = 1; i <= total; i++)
      planet[i] = mod(planet[i] * (double)multiplyfactor);
  if (onasc) {
    j = planet[onasc]-Asc;
    for (i = 1; i <= SIGNS; i++)
      house[i] = mod(house[i]+j);
  }
  houseplace();
  if (flip) {
    for (i = 1; i <= total; i++) {
      k = inhouse[i];
      inhouse[i] = (int) (planet[i]/30.0)+1;
      planet[i] = (double)(k-1)*30.0+mindistance(house[k], planet[i])/
	mindistance(house[k], house[mod12(k+1)])*30.0;
    }
    for (i = 1; i <= SIGNS; i++) {
      k = inhouseplace((double) (i-1)*30.0);
      housetemp[i] = (double)(k-1)*30.0+mindistance(house[k],
        (double) (i-1)*30.0)/mindistance(house[k], house[mod12(k+1)])*30.0;
    }
    for (i = 1; i <= SIGNS; i++)
      house[i] = housetemp[i];
  }
  if (decan)
    for (i = 1; i <= total; i++) {
      k = (int) (planet[i]/30.0)+1;
      j = planet[i] - (double)((k-1)*30);
      k = mod12(k + 4*((int)floor(j/10.0)));
      j = (j - floor(j/10.0)*10.0)*3.0;
      planet[i] = (double)(k-1)*30.0+j;
      houseplace();
    }
  return T;
}

/**/
