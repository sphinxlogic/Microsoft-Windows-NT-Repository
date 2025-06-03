/*
** Astrolog (Version 2.25) File: options.c
*/

#include "astrolog.h"

/*
*******************************************************************************
** Calculation subprograms
*******************************************************************************
*/

getaspect(planet1, planet2, i, j, acc)
double *planet1, *planet2;
int i, j, acc;
{
  int k;
  double l, m, n;
  gridname[i][j] = grid[i][j] = 0;
  l = mindistance(planet2[i], planet1[j]);
  for (k = aspects; k >= 1; k--) {
    m = l-aspectangle[k];
    if (dabs(m) < orb(i, j, k)) {
      gridname[i][j] = k;
      if (todisplay & 65536)
	m = sgn(ret[j]-ret[i])*sgn(planet1[j]-planet2[i])*
	  (dabs(planet1[j]-planet2[i])>180.0 ? -1.0 : 1.0)*sgn(m)*dabs(m);
      grid[i][j] = (int) (m*(!acc ? 10.0 : 60.0));
    }
  }
}

void creategrid(acc)
int acc;
{
  int i, j, k;
  double l;
  for (j = 1; j <= total; j++) if (!ignore[j])
    for (i = 1; i <= total; i++) if (!ignore[i])
      if (acc < 2 ? i < j : i > j)
	getaspect(planet, planet, i, j, acc);
      else if (acc < 2 ? i > j : i < j) {
	l = mod(midpoint(planet[i], planet[j])); k = (int) l;
	gridname[i][j] = k/30+1;
	grid[i][j] = !acc ? k%30 : (int)((l-(double)(k/30)*30.0)*60.0);
      } else {
	gridname[i][j] = (int)planet[j]/30+1;
	grid[i][j] = (int)(planet[j]-(double)(gridname[i][j]-1)*30.0);
      }
}

createrelationgrid(acc)
int acc;
{
  int i, j, k;
  double l;
  for (j = 1; j <= total; j++) if (!ignore[j])
    for (i = 1; i <= total; i++) if (!ignore[i])
      if (acc < 2)
	getaspect(planet1, planet2, i, j, acc);
      else {
	l = mod(midpoint(planet2[i], planet1[j])); k = (int) l;
	gridname[i][j] = k/30+1;
	grid[i][j] = (int)((l-(double)(k/30)*30.0)*60.0);
      }
}

displayrelationgrid()
{
  int i, j, k, temp;
  printf("AB>");
  for (i = 1; i < total; i++) if (!ignore[i])
    printf("|%c%c%c", OBJNAM(i));
  printf("\nV  ");
  for (i = 1; i < total; i++) if (!ignore[i])
    printf("|###");
  putchar('\n');
  for (j = 1; j < total; j++) if (!ignore[j])
    for (k = 1; k <= 3; k++) {
      if (k < 2)
	printf("---");
      else if (k == 2)
	printf("%c%c%c", OBJNAM(j));
      else
	printf("###");
      for (i = 1; i < total; i++) if (!ignore[i]) {
	putchar('|');
        if (k < 2)
	  printf("---");
	else if (k == 2) {
	  temp = gridname[i][j];
	  if (todisplay & 8192)
	    printf("%c%c%c", SIGNAM(temp));
	  else
	    printf("%s", aspectabbrev[temp]);
	} else
	  if (todisplay & 8192) {
	    temp = grid[i][j] / 60;
	    printf("%d%d'", temp/10, temp%10);
	  } else
	    if (gridname[i][j]) {
	      temp = grid[i][j];
	      if (temp < 100)
		printf("%d%c%d", abs(temp)/10,
		       temp < 0 ? ',' : '.', abs(temp)%10);
	      else if (temp < 1000)
		printf("%2d%c", abs(temp)/10, temp < 0 ? ',' : '.');
	      else
		printf("%3d", abs(temp)/10);
	    } else
	      printf("   ");
      }
      putchar('\n');
    }
}

#define degtodec(A) sgn(A)*(floor(dabs(A))+(dabs(A)-floor(dabs(A)))*60/100.0)

void castrelation()
{
  double zon, lon, lat, t1, t2, t;
  int i;
  inputdata(filename);
  zon = X; lon = L5; lat = LA;
  t1 = castchart(TRUE);
  for (i = 1; i <= SIGNS; i++) {
    house1[i] = house[i];
    inhouse1[i] = inhouse[i];
  }
  for (i = 1; i <= total; i++) {
    planet1[i] = planet[i];
    planetalt1[i] = planetalt[i];
    ret1[i] = ret[i];
  }
  inputdata(filename2);
  Mon = M; Day = D; Yea = Y; Tim = F; Zon = X; Lon = L5; Lat = LA;
  t2 = castchart(TRUE);
  for (i = 1; i <= SIGNS; i++) {
    house2[i] = house[i];
    inhouse2[i] = inhouse[i];
  }
  for (i = 1; i <= total; i++) {
    planet2[i] = planet[i];
    planetalt2[i] = planetalt[i];
    ret2[i] = ret[i];
  }
  if (relation <= 1)
    for (i = 1; i <= SIGNS; i++)
      house[i] = house1[i];
  else if (relation == 2) {   
    for (i = 1; i <= total; i++) {
      planet[i] = midpoint(planet1[i], planet2[i]);
      planetalt[i] = (planetalt1[i]+planetalt2[i])/2.0;
      ret[i] = (ret1[i]+ret2[i])/2.0;
    }
    for (i = 1; i <= SIGNS; i++)
      house[i] = midpoint(house1[i], house2[i]);
    for (i = 1; i <= SIGNS; i++)
      if (mindistance(house[10], mod(house[i]-(double)(i+2)*30.0)) > 90.0)
	house[i] = mod(house[i]+180.0);
  } else if (relation == 3) {
    T = (t1+t2)/2.0;
    t = (T*36525.0)+0.5; JD = floor(t)+2415020.0; F = (t-floor(t))*24.0;
    X = (dectodeg(zon)+dectodeg(Zon))/2.0; X = degtodec(X);
    F = dectodeg(F)-dectodeg(X); F = degtodec(F);
    if (F < 0.0) {
      F = dectodeg(F)+24.0; F = degtodec(F); JD -= 1.0;
    }
    juliantomdy(JD, &M, &D, &Y);
    L5 = (dectodeg(lon)+dectodeg(Lon))/2.0; L5 = degtodec(L5);
    LA = (dectodeg(lat)+dectodeg(Lat))/2.0; LA = degtodec(LA);
    Mon = M; Day = D; Yea = Y; Tim = F; Zon = X; Lon = L5; Lat = LA;
    castchart(FALSE);
  }
  houseplace();
}

printfield(string, field)
char *string;
int field;
{
  int i, j;
  j = stringlen(string);
  printtab(' ', field-j);
  for (i = 0; i < j && i < field; i++)
    putchar(string[i]);
}

displayinday(prog)
int prog;
{
  int time[MAXINDAY], source[MAXINDAY], aspect[MAXINDAY], dest[MAXINDAY],
    sign1[MAXINDAY], sign2[MAXINDAY], occurcount, divisions, div,
    i, j, k, s1, s2;
  double Day2, D1, D2, divsiz, d1, d2, e1, e2, f1, f2, g;
  divisions = prog ? 1 : DIVISIONS;
  divsiz = 24.0/ (double) divisions*60.0;
  if (todisplay & 256) {
    D1 = 1.0;
    if (prog && Mon2 == 0.0) {
      Mon2 = 1.0; D2 = 365.0-28.0+(double)dayinmonth(2, (int) Yea2);
    } else D2 = (double)
      dayinmonth((int) (prog ? Mon2 : Mon), (int) (prog ? Yea2 : Yea));
  } else
    D1 = D2 = Day;
  for (Day2 = D1; Day2 <= D2; Day2 += 1.0) {
    occurcount = 0;
    M = Mon; D = Day2; Y = Yea; F = 0.0; X = Zon; L5 = Lon; LA = Lat;
    if (progress = prog) {
      Jdp = mdytojulian(Mon2, D, Yea2);
      M = Mon; D = Day; Y = Yea; F = Tim; X = Zon; L5 = Lon; LA = Lat;
    }
    castchart(TRUE);
    for (i = 1; i <= SIGNS; i++) {
      house2[i] = house[i];
      inhouse2[i] = inhouse[i];
    }
    for (i = 1; i <= total; i++) {
      planet2[i] = planet[i];
      ret2[i] = ret[i];
    }
    for (div = 1; div <= divisions; div++) {
      M = Mon; D = Day2; Y = Yea; X = Zon; L5 = Lon; LA = Lat;
      F = 24.0*div/ (double) divisions;
      if (prog) {
	Jdp = mdytojulian(Mon2, D+1.0, Yea2);
	M = Mon; D = Day; Y = Yea; F = Tim; X = Zon; L5 = Lon; LA = Lat;
      }
      castchart(TRUE);
      for (i = 1; i <= SIGNS; i++) {
	house1[i] = house2[i]; inhouse1[i] = inhouse2[i];
	house2[i] = house[i];  inhouse2[i] = inhouse[i];
      }
      for (i = 1; i <= total; i++) {
	planet1[i] = planet2[i]; ret1[i] = ret2[i];
	planet2[i] = planet[i];  ret2[i] = ret[i];
      }
      for (i = 1; i <= total; i++) if (!ignore[i] &&
	(prog || i <= THINGS || i > objects+4)) {
	s1 = (int) floor(planet1[i] / 30.0);
	s2 = (int) floor(planet2[i] / 30.0);
	if (!ignore[i] && s1 != s2) {
	  source[occurcount] = i;
	  aspect[occurcount] = -1;
	  dest[occurcount] = s2+1;
	  time[occurcount] = (int) (mindistance(planet1[i],
            (double) (ret1[i] >= 0.0 ? s2 : s1) * 30.0) /
            mindistance(planet1[i], planet2[i])*divsiz) +
	    (int) ((double) (div-1)*divsiz);
	  sign1[occurcount] = sign2[occurcount] = s1+1;
	  occurcount++;
	}
	if (!ignore[i] && (ret1[i] < 0.0) != (ret2[i] < 0.0)) {
	  source[occurcount] = i;
	  aspect[occurcount] = -2;
	  dest[occurcount] = ret2[i] < 0.0;
	  time[occurcount] = (int) (dabs(ret1[i])/(dabs(ret1[i])+dabs(ret2[i]))
	    *divsiz) + (int) ((double) (div-1)*divsiz);
	  sign1[occurcount] = sign2[occurcount] = s1+1;
	  occurcount++;
	}
	for (j = i+1; j <= total; j++) if (!ignore[j] &&
	  (prog || j <= THINGS || j > objects+4))
	  for (k = 1; k <= aspects; k++) {
	    d1 = planet1[i]; d2 = planet2[i];
	    e1 = planet1[j]; e2 = planet2[j];
	    if (mindistance(d1, d2) < mindistance(e1, e2)) {
	      swapdoub(&d1, &e1);
	      swapdoub(&d2, &e2);
	    }
	    if (mindistance(e1, mod(d1-aspectangle[k])) <
		mindistance(e2, mod(d2+aspectangle[k]))) {
	      e1 = mod(e1+aspectangle[k]);
	      e2 = mod(e2+aspectangle[k]);
	    } else {
	      e1 = mod(e1-aspectangle[k]);
	      e2 = mod(e2-aspectangle[k]);
	    }
	    f1 = e1-d1;
	    if (dabs(f1) > 180.0)
	      f1 -= sgn(f1)*DEGREES;
	    f2 = e2-d2;
	    if (dabs(f2) > 180.0)
	      f2 -= sgn(f2)*DEGREES;
	    if (mindistance(midpoint(d1, d2), midpoint(e1, e2)) < 90.0 &&
		sgn(f1) != sgn(f2)) {
	      source[occurcount] = i;
	      aspect[occurcount] = k;
	      dest[occurcount] = j;
	      f1 = d2-d1;
	      if (dabs(f1) > 180.0)
		f1 -= sgn(f1)*DEGREES;
	      f2 = e2-e1;
	      if (dabs(f2) > 180.0)
		f2 -= sgn(f2)*DEGREES;
	      g = (dabs(d1-e1) > 180.0 ?
		   (d1-e1)-sgn(d1-e1)*DEGREES : d1-e1)/(f2-f1);
	      time[occurcount] = (int) (g*divsiz) +
		(int) ((double) (div-1)*divsiz);
	      sign1[occurcount] = (int) (mod(planet1[i]+
		sgn(planet2[i]-planet1[i])*
		(dabs(planet2[i]-planet1[i]) > 180.0 ? -1 : 1)*
		dabs(g)*mindistance(planet1[i], planet2[i]))/30.0)+1;
	      sign2[occurcount] = (int) (mod(planet1[j]+
		sgn(planet2[j]-planet1[j])*
		(dabs(planet2[j]-planet1[j]) > 180.0 ? -1 : 1)*
		dabs(g)*mindistance(planet1[j], planet2[j]))/30.0)+1;
	      occurcount++;
	    }
	  }
      }
    }
    for (i = 1; i < occurcount; i++) {
      j = i-1;
      while (j >= 0 && time[j] > time[j+1]) {
	swapint(&source[j], &source[j+1]);
	swapint(&aspect[j], &aspect[j+1]);
	swapint(&dest[j], &dest[j+1]);
	swapint(&time[j], &time[j+1]);
	swapint(&sign1[j], &sign1[j+1]); swapint(&sign2[j], &sign2[j+1]);
	j--;
      }
    }
    for (i = 0; i < occurcount; i++) {
      s1 = time[i]/60;
      s2 = time[i]-s1*60;
      s1 = mod12(s1);
      j = (int) Day2;
      if (prog) {
	g = Mon2;
	while (j > (k = dayinmonth((int) g, (int) Yea2))) {
	  j -= k;
	  g += 1.0;
	}
      }
      printf("%2.0f/%d%d/%4.0f ",
	     prog ? g : Mon, j/10, j%10, prog ? Yea2 : Yea);
      printf("%2d:%d%d%cm - ",
	     s1, s2/10, s2%10, time[i] < 12*60 ? 'a' : 'p');
      if (prog)
	printf("progr ");
      printfield(objectname[source[i]], 7);
      j = (ret1[source[i]] < 0.0)+(ret2[source[i]] < 0.0);
      printf(" %c%c%c%c%c", j < 1 ? '(' : (j > 1 ? '[' : '<'),
	     SIGNAM(sign1[i]), j < 1 ? ')' : (j > 1 ? ']' : '>'));
      if (aspect[i] == -1)
	printf(" --> ");
      else if (aspect[i] == -2)
	printf(" S/%c", dest[i] ? 'R' : 'D');
      else
	printf(" %s ", aspectabbrev[aspect[i]]);
      if (aspect[i] == -1) {
	printf("%s", signname[dest[i]]);
	if (source[i] == 1) {
	  if (dest[i] == 1)
	    printf(" (Vernal Equinox)");
	  else if (dest[i] == 4)
	    printf(" (Summer Solstice)");
	  else if (dest[i] == 7)
	    printf(" (Autumnal Equinox)");
	  else if (dest[i] == 10)
	    printf(" (Winter Solstice)");
	}
      } else if (aspect[i] > 0) {
	j = (ret1[dest[i]] < 0.0)+(ret2[dest[i]] < 0.0);
	printf("%c%c%c%c%c %s",
	       j < 1 ? '(' : (j > 1 ? '[' : '<'), SIGNAM(sign2[i]),
	       j < 1 ? ')' : (j > 1 ? ']' : '>'), objectname[dest[i]]);
	if (source[i] == 1 && dest[i] == 2) {
	  if (aspect[i] == 1)
	    printf(" (New Moon)");
	  else if (aspect[i] == 2)
	    printf(" (Full Moon)");
	  else if (aspect[i] == 3)
	    printf(" (Half Moon)");
	}
      }
      putchar('\n');
    }
  }
}

void printtransit(prog)
int prog;
{
  double planet3[TOTAL+1], house3[SIGNS+1], ret3[TOTAL+1];
  int time[MAXINDAY], source[MAXINDAY], aspect[MAXINDAY], dest[MAXINDAY],
    sign[MAXINDAY], isret[MAXINDAY], occurcount, div, i, j, k, s1, s2, s3;
  double M1, M2, divsiz, daysiz, d, e1, e2, f1, f2;
  inputdata(filename);
  Mon = M; Day = D; Yea = Y; Tim = F; Zon = X; Lon = L5; Lat = LA;
  castchart(TRUE);
  for (i = 1; i <= SIGNS; i++)
    house3[i] = house[i];
  for (i = 1; i <= total; i++) {
    planet3[i] = planet[i];
    ret3[i] = ret[i];
  }
  if (Mon2 == 0.0) {
    M1 = 1.0; M2 = 12.0;
  } else
    M1 = M2 = Mon2;
  for (Mon2 = M1; Mon2 <= M2; Mon2 += 1.0) {
    daysiz = (double) dayinmonth((int) Mon2, (int) Yea2)*24.0*60.0;
    divsiz = daysiz/ (double) DIVISIONS;
    M = Mon2; D = 1.0; Y = Yea2; F = 0.0;
    X = defzone; L5 = deflong; LA = deflat;
    if (progress = prog) {
      Jdp = mdytojulian(M, D, Y);
      M = Mon; D = Day; Y = Yea; F = Tim; X = Zon; L5 = Lon; LA = Lat;
    }
    castchart(TRUE);
    for (i = 1; i <= SIGNS; i++)
      house2[i] = house[i];
    for (i = 1; i <= objects; i++) {
      planet2[i] = planet[i];
      ret2[i] = ret[i];
    }
    for (div = 1; div <= DIVISIONS; div++) {
      occurcount = 0;
      M = Mon2; Y = Yea2; F = 0.0; X = defzone; L5 = deflong; LA = deflat;
      D = 1.0+(daysiz/24.0/60.0)*div/ (double) DIVISIONS;
      if (prog) {
	Jdp = mdytojulian(M, D, Y);
	M = Mon; D = Day; Y = Yea; F = Tim; X = Zon; L5 = Lon; LA = Lat;
      }
      castchart(TRUE);
      for (i = 1; i <= SIGNS; i++) {
	house1[i] = house2[i]; house2[i] = house[i];
      }
      for (i = 1; i <= objects; i++) {
	planet1[i] = planet2[i]; ret1[i] = ret2[i];
	planet2[i] = planet[i];  ret2[i] = ret[i];
      }
      for (i = 1; i <= total; i++) if (!ignore[i])
	for (j = 1; j <= BASE; j++) if (!ignore[j] &&
	  (j != 2 || (j == 2 && (todisplay & 512) > 0)) &&
	  (prog || j <= THINGS || j > objects+4))
	  for (k = 1; k <= aspects; k++) {
	    d = planet3[i]; e1 = planet1[j]; e2 = planet2[j];
	    if (mindistance(e1, mod(d-aspectangle[k])) <
		mindistance(e2, mod(d+aspectangle[k]))) {
	      e1 = mod(e1+aspectangle[k]);
	      e2 = mod(e2+aspectangle[k]);
	    } else {
	      e1 = mod(e1-aspectangle[k]);
	      e2 = mod(e2-aspectangle[k]);
	    }
	    f1 = e1-d;
	    if (dabs(f1) > 180.0)
	      f1 -= sgn(f1)*DEGREES;
	    f2 = e2-d;
	    if (dabs(f2) > 180.0)
	      f2 -= sgn(f2)*DEGREES;
	    if (mindistance(d, midpoint(e1, e2)) < 90.0 &&
		sgn(f1) != sgn(f2) && occurcount < MAXINDAY) {
	      source[occurcount] = j;
	      aspect[occurcount] = k;
	      dest[occurcount] = i;
	      time[occurcount] = (int) (dabs(f1)/(dabs(f1)+dabs(f2))*divsiz) +
		(int) ((double) (div-1)*divsiz);
	      sign[occurcount] = (int) (mod(
		mindistance(planet1[j], mod(d-aspectangle[k])) <
		mindistance(planet2[j], mod(d+aspectangle[k])) ?
		d-aspectangle[k] : d+aspectangle[k])/30.0)+1;
	      isret[occurcount] = (ret1[j] < 0.0)+(ret2[j] < 0.0);
	      occurcount++;
	    }
	  }
      for (i = 1; i < occurcount; i++) {
	j = i-1;
	while (j >= 0 && time[j] > time[j+1]) {
	  swapint(&source[j], &source[j+1]);
	  swapint(&aspect[j], &aspect[j+1]);
	  swapint(&dest[j], &dest[j+1]);
	  swapint(&time[j], &time[j+1]);
	  swapint(&sign[j], &sign[j+1]);
	  swapint(&isret[j], &isret[j+1]);
	  j--;
	}
      }
      for (i = 0; i < occurcount; i++) {
	s1 = time[i]/24/60;
	s3 = time[i]-s1*24*60;
	s2 = s3/60;
	s3 = s3-s2*60;
	printf("%2.0f/%d%d/%4.0f %2d:%d%d%cm - %s ",
	       Mon2, (s1+1)/10, (s1+1)%10, Yea2, mod12(s2),
	       s3/10, s3%10, s2 < 12 ? 'a' : 'p', !prog ? "trans" : "progr");
	printfield(objectname[source[i]], 7);
	j = (int) (planet3[dest[i]]/30.0)+1;
	printf(" %c%c%c%c%c ",
	       isret[i] < 1 ? '(' : (isret[i] > 1 ? '[' : '<'),
	       SIGNAM(sign[i]),
	       isret[i] < 1 ? ')' : (isret[i] > 1 ? ']' : '>'));
	printf("%s natal %c%c%c%c%c %s", aspectabbrev[aspect[i]],
	       ret3[dest[i]] >= 0.0 ? '(' : '[', SIGNAM(j),
	       ret3[dest[i]] >= 0.0 ? ')' : ']', objectname[dest[i]]);
	if (source[i] == 1 && aspect[i] == 1 && dest[i] == 1)
	  printf(" (Solar Return)");
	else if (source[i] == 2 && aspect[i] == 1 && dest[i] == 2)
	  printf(" (Lunar Return)");
	putchar('\n');
      }
    }
  }
}

void printephemeris()
{
  double M1, M2;
  int daysiz, i, j, k, s, d, m;
  k = todisplay & 1024 ? BASE : 10;
  if (Mon2 == 0.0) {
    M1 = 1.0; M2 = 12.0;
  } else
    M1 = M2 = Mon2;
  for (Mon2 = M1; Mon2 <= M2; Mon2 += 1.0) {
    daysiz = dayinmonth((int) Mon2, (int) Yea2);
    printf("Mo/Dy/Yr");
    for (j = 1; j <= k; j++) {
      if (!ignore[j] && (j <= THINGS || j > objects+4))
	printf("  %c%c%c%c ", OBJNAM(j), objectname[j][3] != 0 ?
	       objectname[j][3] : ' ');
    }
    putchar('\n');
    for (i = 1; i <= daysiz; i++) {
      M = Mon2; D = (double) i; Y = Yea2;
      F = 0.0; X = defzone; L5 = deflong; LA = deflat;
      castchart(TRUE);
      printf("%2d/%2d/%2d ", (int) Mon2, i, ((int) Yea2) % 100);
      for (j = 1; j <= k; j++)
	if (!ignore[j] && (j <= THINGS || j > objects+4)) {
	  s = (int) (planet[j]/30.0) + 1;
	  d = (int) planet[j] - (s-1)*30;
	  m = (int) ((planet[j]-floor(planet[j]))*60.0);
	  printf("%d%d%s%d%d%c", d/10, d%10, signabbrev[s], m/10, m%10,
		 ret[j] >= 0.0 ? ' ' : '.');
	}
      putchar('\n');
    }
    if (Mon2 < M2)
      putchar('\n');
  }
}

void printchart(prog)
{
  int todisp;
  todisp = todisplay & 255;
  if (todisp == 0)
    todisp = todisplay = todisplay | 1;
  if (todisp & 1) {
    chartlocation();
    if (todisp - (todisp & 1))
      printf("\n\n");
  }
  if (todisp & 2) {
    chartwheel();
    if (todisp - (todisp & 3))
      printf("\n\n");
  }
  if (todisp & 4) {
    if (relation != -1) {
      creategrid(FALSE);
      chartgrid();
      if (todisplay & 8192) {
	printf("\n");
	displaygrands();
      }
    } else {
      createrelationgrid(todisplay & 8192 ? 2 : FALSE);
      displayrelationgrid();
    }
    if (todisp - (todisp & 7))
      printf("\n\n");
  }
  if (todisp & 8) {
    charthorizon();
    if (todisp - (todisp & 15))
      printf("\n\n");
  }
  if (todisp & 16) {
    chartspace();
    if (todisp - (todisp & 31))
      printf("\n\n");
  }
  if (todisp & 32) {
    chartinfluence();
    if (todisp - (todisp & 63))
      printf("\n\n");
  }
  if (todisp & 64) {
    chartastrograph();
    if (todisp - (todisp & 127))
      printf("\n\n");
  }
  if (todisp & 128)
    displayinday(prog);
}

/**/
