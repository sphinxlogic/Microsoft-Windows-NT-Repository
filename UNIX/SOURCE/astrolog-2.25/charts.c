/*
** Astrolog (Version 2.25) File: charts.c
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

int wheel[SIGNS][WHEELROWS];

/*
*******************************************************************************
** Display subprograms
*******************************************************************************
*/

void chartlocation()
{
  int i, j, k;
  int count = 0, elemode[4][3], elem[4], mo[3], pos = 0, abo = 0, lef = 0;
  for (i = 0; i < 4; i++)
    elem[i] = 0;
  for (j = 0; j < 3; j++)
    mo[j] = 0;
  for (i = 0; i < 4; i++)
    for (j = 0; j < 3; j++)
      elemode[i][j] = 0;
  for (i = 1; i <= total; i++) if (!ignore[i]) {
    count++;
    j = (int) (planet[i]/30.0) + 1;
    elemode[(j-1)%4][(j-1)%3]++;
    elem[(j-1)%4]++; mo[(j-1)%3]++;
    pos += (j & 1);
    j = inhouse[i];
    abo += (j >= 7);
    lef += (j < 4 || j >= 10);
  }
  printf("Astrolog (%s) chart ", VERSION);
  if (Mon == -1)
    printf("(no time or space)\n");
  else if (relation == 2)
    printf("(composite)\n");
  else {
    i = (int) ((dabs(Tim)-floor(dabs(Tim)))*100.0+0.5);
    j = (int) ((dabs(Zon)-floor(dabs(Zon)))*100.0+0.5);
    printf("for %d %s %d %.0f:%d%d (%c%.0f:%d%d GMT) ",
	   (int) Day, monthname[(int) Mon], (int) Yea, floor(Tim), 
	   i/10, i%10, Zon > 0.0 ? '-' : '+', dabs(Zon), j/10, j%10);
    printf("%s\n", stringlocation(Lon, Lat, 100.0));
  }
  printf("Body  Locat. Ret. Decl. Rul.      House  Rul. Veloc.    ");
  printf("%s Houses.\n\n", systemname[housesystem]);
  for (i = 1; i <= BASE; i++)
    if (i <= objects || (i > objects+4 && !ignore[i])) {
    printf("%c%c%c%c: ", OBJNAM(i), objectname[i][3] ? objectname[i][3] : ' ');
    printminute(planet[i]);
    printf(" %c ", ret[i] >= 0.0 ? ' ' : 'R');
    printaltitude(planetalt[i]);
    printf(" (%c)", dignify(i, (int) (planet[i]/30.0)+1));
    j = inhouse[i];
    printf(" [%2d%c%c house]", j, post[j][0], post[j][1]);
    printf(" [%c]", dignify(i, j));
    if (i != 2 && i < THINGS || i > objects+4)
      printf(" %c%5.3f", ret[i] < 0.0 ? '-' : '+', RTOD(dabs(ret[i])));
    else
      printf(" ______");
    if (i <= SIGNS) {
      printf("  -  House cusp %2d: ", i);
      printminute(house[i]);
    }
    if (i == SIGNS+2)
      printf("     Car Fix Mut TOT");
    else if (i > SIGNS+2 && i < SIGNS+7) {
      j = i-(SIGNS+2)-1;
      printf("  %c%c%c%3d %3d %3d %3d",
	     element[j][0], element[j][1], element[j][2],
	     elemode[j][0], elemode[j][1], elemode[j][2], elem[j]);
    } else if (i == SIGNS+7)
      printf("  TOT %2d %3d %3d %3d", mo[0], mo[1], mo[2], count);
    else if (i > objects)
      printf("  Uranian #%d", i-objects-4);
    switch (i-SIGNS-1) {
    case 1: printf("   +:%2d", pos);       break;
    case 2: printf("   -:%2d", count-pos); break;
    case 3: printf("   M:%2d", abo);       break;
    case 4: printf("   N:%2d", count-abo); break;
    case 5: printf("   A:%2d", lef);       break;
    case 6: printf("   D:%2d", count-lef); break;
    }
    putchar('\n');
  }
  if (universe) for (i = BASE+1; i <= total; i++) if (!ignore[i]) {
    j = BASE+starname[i-BASE];
    printf("%c%c%c%c: ", OBJNAM(j), objectname[j][3]);
    printminute(planet[j]);
    printf("   ");
    printaltitude(planetalt[j]);
    k = inhouse[j];
    printf("     [%2d%c%c house]", k, post[k][0], post[k][1]);
    printf("     ______  Star #%2d: %5.2f\n", i-BASE, starbright[j-BASE]);
  }
}

void chartgrid()
{
  int i, j, k, temp;
  for (j = 1; j <= total; j++) if (!ignore[j])
    for (k = 1; k <= 3; k++) {
      for (i = 1; i <= total; i++) if (!ignore[i]) {
	if (i > 1 && j+k > 2)
	  putchar('|');
	if (k > 1) {
	  if (i < j) {
	    if (k < 3)
	      printf("%s", aspectabbrev[gridname[i][j]]);
	    else {
	      if (gridname[i][j]) {
		if (grid[i][j] < 100)
		  printf("%d%c%d", abs(grid[i][j])/10,
			 grid[i][j] < 0 ? ',' : '.', abs(grid[i][j])%10);
		else if (grid[i][j] < 1000)
		  printf("%2d%c", abs(grid[i][j])/10,
			 grid[i][j] < 0 ? ',' : '.');
		else
		  printf("%3d", abs(grid[i][j])/10);
	      } else
		printf("   ");
	    }
	  } else if (i > j) {
	    if (k < 3)
	      printf("%d%d'", grid[i][j]/10, grid[i][j]%10);
	    else {
	      temp = gridname[i][j];
	      printf("%c%c%c", SIGNAM(temp));
	    }
	  } else {
	    if (k < 3)
	      printf("%c%c%c", OBJNAM(j));
	    else
	      printf("###");
	  }
	} else
	  if (j > 1)
	    printf("---");
      }
      if (j+k > 2)
	putchar('\n');
    }
}

printgrand(nam, i1, i2, i3, i4)
char nam;
int i1, i2, i3, i4;
{
  switch (nam) {
  case '.': printf("Stellium   "); break;
  case 't': printf("Grand Trine"); break;
  case 's': printf("T-Square   "); break;
  case 'y': printf("Yod        "); break;
  case 'g': printf("Grand Cross"); break;
  case 'c': printf("Cradle     "); break;
  default: ;
  }
  printf(" %s ", nam == '.' || nam == 't' || nam == 'g' ? "with" : "from");
  printf("%c%c%c: ", OBJNAM(i1));
  printminute(planet[i1]);
  printf(" %s %c%c%c: ", nam == '.' || nam == 't' ? "and" : "to ", OBJNAM(i2));
  printminute(planet[i2]);
  printf(" %s %c%c%c: ", nam == 'g' || nam == 'c' ? "to " : "and", OBJNAM(i3));
  printminute(planet[i3]);
  if (nam == 'g' || nam == 'c') {
    printf(" to %c%c%c: ", OBJNAM(i4));
    printminute(planet[i4]);
  }
  printf("\n");
}

void displaygrands()
{
  int count = 0, i, j, k, l;
  for (i = 1; i <= objects; i++) if (!ignore[i])
    for (j = 1; j <= objects; j++) if (j != i && !ignore[j])
      for (k = 1; k <= objects; k++) if (k != i && k != j && !ignore[k]) {
	if (i < j && j < k && gridname[i][j] == 1 &&
	    gridname[i][k] == 1 && gridname[j][k] == 1) {
	  count++;
	  printgrand('.', i, j, k, l);
	} else if (i < j && j < k && gridname[i][j] == 4 &&
	    gridname[i][k] == 4 && gridname[j][k] == 4) {
	  count++;
	  printgrand('t', i, j, k, l);
	} else if (j < k && gridname[j][k] == 2 &&
	    gridname[MIN(i, j)][MAX(i, j)] == 3 &&
	    gridname[MIN(i, k)][MAX(i, k)] == 3) {
	  count++;
	  printgrand('s', i, j, k, l);
	} else if (j < k && gridname[j][k] == 5 &&
	    gridname[MIN(i, j)][MAX(i, j)] == 6 &&
	    gridname[MIN(i, k)][MAX(i, k)] == 6) {
	  count++;
	  printgrand('y', i, j, k, l);
	}
	for (l = 1; l <= objects; l++) if (!ignore[l]) {
	  if (i < j && i < k && i < l && j < l && gridname[i][j] == 3 &&
	      gridname[MIN(j, k)][MAX(j, k)] == 3 &&
	      gridname[MIN(k, l)][MAX(k, l)] == 3 &&
	      gridname[i][l] == 3 && mindistance(planet[i], planet[k]) > 150.0
	      && mindistance(planet[j], planet[l]) > 150.0) {
	    count++;
	    printgrand('g', i, j, k, l);
	  } else if (i < l && gridname[MIN(i, j)][MAX(i, j)] == 5 &&
	      gridname[MIN(j, k)][MAX(j, k)] == 5 &&
	      gridname[MIN(k, l)][MAX(k, l)] == 5 &&
	      mindistance(planet[i], planet[l]) > 150.0) {
	    count++;
	    printgrand('c', i, j, k, l);
	  }
	}
      }
  if (!count)
    printf("No major configurations in aspect grid.\n");
}

printtab(chr, count)
char chr;
int count;
{
  int i;
  for (i = 0; i < count; i++)
    putchar(chr);
}

printwheelslot(house, row)
int house, row;
{
  int i;
  i = wheel[house-1][row];
  if (i) {
    printf(" %c%c%c ", OBJNAM(i));
    printminute(planet[i]);
    printf("%c ", ret[i] < 0.0 ? 'r' : ' ');
    printtab(' ', WHEELCOLS-14-1);
  } else
    printtab(' ', WHEELCOLS-1);
}

void chartwheel()
{
  int i, j, k, l, count = 0;
  for (i = 0; i < SIGNS; i++)
    for (j = 0; j < WHEELROWS; j++)
      wheel[i][j] = 0;
  for (i = 1; i <= total && count < 26; i++) {
    if (!ignore[i] && (i < 18 || i == 20 || i > objects+4))
      for (j = inhouse[i]-1; j < SIGNS; j = j < SIGNS ? (j+1)%SIGNS : j) {
	l = house[j+1] > house[mod12(j+2)];
	for (k = 0; k < WHEELROWS && wheel[j][k] > 0 &&
	     (planet[i] >= planet[wheel[j][k]] ||
	      (l && planet[i] < 180.0 && planet[wheel[j][k]] > 180.0)) &&
	     !(l && planet[i] > 180.0 && planet[wheel[j][k]] < 180.0); k++)
	  ;
	count++;
	if (wheel[j][k] <= 0) {
	  wheel[j][k] = i;
	  j = SIGNS;
	} else if (k < WHEELROWS && wheel[j][WHEELROWS-1] <= 0) {
	  for (l = WHEELROWS-1; l > k; l--)
	    wheel[j][l] = wheel[j][l-1];
	  wheel[j][k] = i;
	  j = SIGNS;
	}
      }
  }
  if (!(todisplay & 2048))
    for (i = 3; i < 9; i++)
      for (j = 0; j < WHEELROWS/2; j++) {
	k = WHEELROWS-1-j;
	l = wheel[i][j]; wheel[i][j] = wheel[i][k]; wheel[i][k] = l;
      }
  putchar('+'); printtab('-', WHEELCOLS-8); printf("<11>");
  printminute(house[11]); printtab('-', WHEELCOLS-11); printf("<10>");
  printminute(house[10]); printtab('-', WHEELCOLS-10); printf("<9>");
  printminute(house[9]); printtab('-', WHEELCOLS-4); printf("+\n");
  for (i = 0; i < WHEELROWS; i++) {
    for (j = 11; j >= 8; j--) {
      putchar('|'); printwheelslot(j, i);
    }
    printf("|\n");
  }
  printf("<12>"); printminute(house[12]); printtab('-', WHEELCOLS-11);
  putchar('|'); printtab('-', WHEELCOLS*2-1); putchar('|');
  printtab('-', WHEELCOLS-10); printminute(house[8]); printf("<8>\n");
  for (i = 0; i < WHEELROWS; i++) {
    putchar('|'); printwheelslot(12, i); putchar('|');
    if (i) {
      printtab(' ', WHEELCOLS-11);
      if (i == 1)
	printf("Astrolog (%s) chart", VERSION);
      else if (i == 2) {
	j = (int) Mon;
	printf("%2d %c%c%c ", (int) Day,
	       monthname[j][0], monthname[j][1], monthname[j][2]);
	k = (int) ((dabs(Tim)-floor(dabs(Tim)))*100.0+0.5);
	printf("%4d     %2.0f:%d%d", (int) Yea, floor(Tim), k/10, k%10);
      } else {
	printf("%c%d%d:", Zon > 0.0 ? '-' : '+',
	       (int)dabs(Zon)/10, (int)dabs(Zon)%10);
	j = (int) ((dabs(Zon)-floor(dabs(Zon)))*100.0+0.5);
	printf("%d%d %s", j/10, j%10, stringlocation(Lon, Lat, 100.0));
      }
      printtab(' ', WHEELCOLS-11);
    } else
      printtab(' ', WHEELCOLS*2-1);
    putchar('|'); printwheelslot(7, i); printf("|\n");
  }
  printf("<1>"); printminute(house[1]); printtab('-', WHEELCOLS-10);
  putchar('|'); printtab(' ', WHEELCOLS-11);
  printf("%s", systemname[housesystem]);
  printtab(' ', 14-stringlen(systemname[housesystem]));
  printf("Houses."); printtab(' ', WHEELCOLS-11); putchar('|');
  printtab('-', WHEELCOLS-10); printminute(house[7]); printf("<7>\n");
  for (i = 0; i < WHEELROWS; i++) {
    putchar('|'); printwheelslot(1, i); putchar('|');
    printtab(' ', WHEELCOLS*2-1); putchar('|'); printwheelslot(6, i);
    printf("|\n");
  }
  printf("<2>"); printminute(house[2]); printtab('-', WHEELCOLS-10);
  putchar('|'); printtab('-', WHEELCOLS*2-1); putchar('|');
  printtab('-', WHEELCOLS-10); printminute(house[6]); printf("<6>\n");
  for (i = 0; i < WHEELROWS; i++) {
    for (j = 2; j <= 5; j++) {
      putchar('|'); printwheelslot(j, i);
    }
    printf("|\n");
  }
  printf("+"); printtab('-', WHEELCOLS-4); printminute(house[3]);
  printf("<3>"); printtab('-', WHEELCOLS-10); printminute(house[4]);
  printf("<4>"); printtab('-', WHEELCOLS-10); printminute(house[5]);
  printf("<5>"); printtab('-', WHEELCOLS-7); printf("+\n");
}

void charthorizon()
{
  double lon, lat, sx, sy, vx, vy,
    lonz[TOTAL+1], latz[TOTAL+1], azi[TOTAL+1], alt[TOTAL+1];
  int i, j, k;
  lon = DTOR(mod(Lon)); lat = DTOR(Lat);
  for (i = 1; i <= total; i++) {
    lonz[i] = DTOR(planet[i]); latz[i] = DTOR(planetalt[i]);
    ecltoequ(&lonz[i], &latz[i]);
  }
  for (i = 1; i <= total; i++) if (i != 18) {
    lonz[i] = DTOR(mod(RTOD(lonz[18]-lonz[i]+lon)));
    lonz[i] = DTOR(mod(RTOD(lonz[i]-lon+PI/2.0)));
    equtolocal(&lonz[i], &latz[i], PI/2.0-lat);
    azi[i] = DEGREES-RTOD(lonz[i]); alt[i] = RTOD(latz[i]);
  }
  printf("Body Altitude Azimuth  Azi. Vector   %s Vector    Moon Vector\n\n",
	 centerplanet ? " Sun" : " Earth");
  for (k = 1; k <= total; k++) {
    i = k <= BASE ? k : BASE+starname[k-BASE];
    if (!ignore[i] && (i <= THINGS || i > objects+4)) {
      printf("%c%c%c%c: ", OBJNAM(i),
	     objectname[i][3] ? objectname[i][3] : ' ');
      printaltitude(alt[i]);
      j = (int) ((azi[i]-floor(azi[i]))*60.0);
      printf(" %3d %d%d'", (int) azi[i], j/10, j%10);
      sx = cos(DTOR(azi[i])); sy = sin(DTOR(azi[i]));
      if (dabs(sx) < dabs(sy)) {
	vx = dabs(sx / sy); vy = 1.0;
      } else {
	vy = dabs(sy / sx); vx = 1.0;
      }
      printf(" (%.2f%c %.2f%c)",
	     vy, sy < 0.0 ? 's' : 'n', vx, sx > 0.0 ? 'e' : 'w');
      vx = azi[1]-azi[i]; vy = azi[2]-azi[i];
      printf(" [%6.1f%6.1f] [%6.1f%6.1f]\n",
        dabs(vx) < 180.0 ? vx : sgn(vx)*(DEGREES-dabs(vx)), alt[1]-alt[i],
	dabs(vy) < 180.0 ? vy : sgn(vy)*(DEGREES-dabs(vy)), alt[2]-alt[i]);
    }
  }
}

void chartspace()
{
  double x, y, z;
  int i;
  printf("Body     Angle    X axis    Y axis    Z axis    Length\n");
  for (i = 0; i <= BASE; i++)
    if (!ignore[i] && i != 2 && (i < THINGS || i > objects+4)) {
      printf("%c%c%c%c: ", OBJNAM(i),
	     objectname[i][3] ? objectname[i][3] : ' ');
      x = spacex[i]; y = spacey[i]; z = spacez[i];
      printf("[%7.2f] [%7.2f] [%7.3f] [%7.3f] [%7.3f]",
	     planet[i], x, y, z, sqrt(x*x+y*y+z*z));
      printf("\n");
    }
}

double objectinf[] = {0, 30, 25, 10, 10, 10, 10, 10, 10, 10, 10,
		      5, 5, 5, 5, 5, 5, 5, 15, 20, 5};
double houseinf[]  = {0, 20, 0, 0, 10, 0, 0, 5, 0, 0, 15, 0, 0};
double aspectinf[] = {0.0, 1.0, 0.8, 0.8, 0.6, 0.6, 0.4, 0.4, 0.2, 0.2,
		      0.2, 0.2, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
int rules[SIGNS+1] = {0, 5, 4, 3, 2, 1, 3, 4, 10, 6, 7, 8, 9};

sortrank(value, rank)
double *value;
int *rank;
{
  int h, i, j, k;
  value[0] = -1.0;
  for (i = 1; i <= THINGS; i++)
    rank[i] = -1;
  for (h = 1, i = 0; h <= THINGS; h++) if (!ignore[h]) {
    i++;
    k = 0;
    for (j = 1; j <= THINGS; j++) if (!ignore[j])
      if (value[j] > value[k] && rank[j] < 0)
	k = j;
    rank[k] = i;
  }
}

void chartinfluence()
{
  double power[OBJECTS+1], power1[OBJECTS+1], power2[OBJECTS+1],
    total, total1, total2, x;
  int rank[OBJECTS+1], rank1[OBJECTS+1], rank2[OBJECTS+1], i, j, k, l;
  char c;
  for (i = 1; i <= objects; i++) {
    power1[i] = power2[i] = 0.0;
  }
  total = total1 = total2 = 0.0;
  for (i = 1; i <= THINGS; i++) {
    j = (int) (planet[i]/30.0)+1;
    power1[i] += objectinf[i];
    power1[i] += houseinf[inhouse[i]];
    c = dignify(i, j);
    switch (c) {
    case 'R': x = 20.0; break;
    case 'e': x = 10.0; break;
    default:  x =  0.0;
    }
    power1[i] += x;
    c = dignify(i, inhouse[i]);
    switch (c) {
    case 'R': x = 15.0; break;
    case 'e': x =  5.0; break;
    default:  x =  0.0;
    }
    power1[i] += x;
    if (i != rules[j])
      power1[rules[j]] += objectinf[i]/2.0;
    if (i != (j = rules[inhouse[i]]))
      power1[j] += objectinf[i]/2.0;
  }
  for (i = 1; i <= SIGNS; i++) {
    j = (int)(house[i]/30.0)+1;
    power1[rules[j]] += houseinf[i];
  }
  creategrid(TRUE);
  for (j = 1; j <= objects; j++) if (!ignore[j])
    for (i = 1; i <= objects; i++) if (!ignore[i] && i != j) {
      k = gridname[MIN(i, j)][MAX(i, j)];
      if (k) {
	l = grid[MIN(i, j)][MAX(i, j)];
	power2[j] += aspectinf[k]*objectinf[i]*
	  (1.0-dabs((double)l)/60.0/aspectorb[k]);
      }
    }
  for (i = 1; i <= THINGS; i++) if (!ignore[i]) {
    power[i] = power1[i]+power2[i]; total1 += power1[i]; total2 += power2[i];
  }
  total = total1+total2;
  sortrank(power1, rank1); sortrank(power2, rank2); sortrank(power, rank);
  printf("Name:    Position     Aspects     Total       Percent\n");
  for (i = 1; i <= THINGS; i++) if (!ignore[i]) {
    printf("%c%c%c%c: ", OBJNAM(i),
	   objectname[i][3] ? objectname[i][3] : ' ');
    printf("%6.1f (%2d) +%6.1f (%2d) =%7.1f (%2d) /%6.1f%%\n",
	   power1[i], rank1[i], power2[i], rank2[i],
	   power[i], rank[i], power[i]/total*100.0);
  }
  printf("Tot : %6.1f      +%6.1f      =%7.1f      / 100.0%%\n",
	 total1, total2, total);
}

void chartastrograph()
{
  double planet1[TOTAL+1], planet2[TOTAL+1], lat[MAXCROSS], lon[MAXCROSS],
    mc[TOTAL+1], ic[TOTAL+1], as[TOTAL+1], ds[TOTAL+1], as1[TOTAL+1],
    ds1[TOTAL+1], lo = Lon, longm, w, x, y, z, ad, oa, am, od, dm;
  int obj1[MAXCROSS], obj2[MAXCROSS], occurcount = 0, i, j, k, l, m, n;
  for (i = 1; i <= TOTAL; i++) {
    planet1[i] = DTOR(planet[i]);
    planet2[i] = DTOR(planetalt[i]);
    ecltoequ(&planet1[i], &planet2[i]);
  }
  printf("Object :");
  for (i = 1; i <= total; i++)
    if (!ignore[i] && (i <= THINGS || i > objects+4))
      printf(" %c%c%c", OBJNAM(i));
  printf("\n------ :");
  for (i = 1; i <= total; i++)
    if (!ignore[i] && (i <= THINGS || i > objects+4))
      printf(" ###");
  printf("\nMidheav: ");
  if (lo < 0.0)
    lo += DEGREES;
  for (i = 1; i <= total; i++)
    if (!ignore[i] && (i <= THINGS || i > objects+4)) {
    x = planet1[18]-planet1[i];
    if (x < 0.0)
      x += 2.0*PI;
    if (x > PI)
      x -= 2.0*PI;
    z = lo+RTOD(x);
    if (z > 180.0)
      z -= DEGREES;
    mc[i] = z;
    printf("%3.0f%c", dabs(z), z < 0.0 ? 'e' : 'w');
  }
  printf("\nNadir  : ");
  for (i = 1; i <= total; i++)
    if (!ignore[i] && (i <= THINGS || i > objects+4)) {
    z = mc[i] + 180.0;
    if (z > 180.0)
      z -= DEGREES;
    ic[i] = z;
    printf("%3.0f%c", dabs(z), z < 0.0 ? 'e' : 'w');
  }
  printf("\nZenith : ");
  for (i = 1; i <= total; i++)
    if (!ignore[i] && (i <= THINGS || i > objects+4)) {
      y = RTOD(planet2[i]);
      printf("%3.0f%c", dabs(y), y < 0.0 ? 's' : 'n');
      as[i] = ds[i] = as1[i] = ds1[i] = 1000.0;
    }
  printf("\n\n");
  longm = DTOR(mod(RTOD(planet1[18])+lo));
  for (j = 80; j >= -80; j -= graphstep) {
    printf("Asc@%2d%c: ", j >= 0 ? j : -j, j < 0 ? 's' : 'n');
    for (i = 1; i <= total; i++)
      if (!ignore[i] && (i <= THINGS || i > objects+4)) {
      ad = tan(planet2[i])*tan(DTOR(j));
      if (ad*ad > 1.0) {
	printf(" -- ");
	as1[i] = ds1[i] = ret2[i] = 1000.0;
      } else {
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
	as1[i] = as[i];
 	as[i] = z = RTOD(z);
	ret2[i] = ad;
	printf("%3.0f%c", dabs(z), z < 0.0 ? 'e' : 'w');
      }
    }
    printf("\nDsc@%2d%c: ", j >= 0 ? j : -j, j < 0 ? 's' : 'n');
    for (i = 1; i <= total; i++)
      if (!ignore[i] && (i <= THINGS || i > objects+4)) {
      ad = ret2[i];
      if (ad == 1000.0)
	printf(" -- ");
      else {
	od = planet1[i]+ad;
	dm = od+PI/2.0;
	z = longm-dm;
	if (z < 0.0)
	  z += 2.0*PI;
	if (z > PI)
	  z -= 2.0*PI;
	ds1[i] = ds[i];
	ds[i] = z = RTOD(z);
	printf("%3.0f%c", dabs(z), z < 0.0 ? 'e' : 'w');
      }
    }
    putchar('\n');
    if (todisplay & 16384)
      for (l = 1; l <= total; l++)
	if (!ignore[l] && (l <= THINGS || l > objects+4))
	for (k = 1; k <= total; k++)
	  if (!ignore[k] && (k <= THINGS || k > objects+4))
	  for (n = 0; n <= 1; n++) {
	    x = n ? ds1[l] : as1[l];
	    y = n ? ds[l] : as[l];
	    for (m = 0; m <= 1; m++) {
	      z = m ? ic[k] : mc[k];
	      if (occurcount < MAXCROSS &&
		  dabs(x-y) < 180.0 && sgn(z-x) != sgn(z-y)) {
		obj1[occurcount] = n ? -l : l;
		obj2[occurcount] = m ? -k : k;
		lat[occurcount] = (double)j+5.0*dabs(z-y)/dabs(x-y);
		lon[occurcount] = z;
		occurcount++;
	      }
	      w = m ? ds1[k] : as1[k];
	      z = m ? ds[k] : as[k];
	      if (occurcount < MAXCROSS && k > l &&
		  dabs(x-y)+dabs(w-z) < 180.0 && sgn(w-x) != sgn(z-y)) {
		obj1[occurcount] = n ? -l : l;
		obj2[occurcount] = 100+(m ? -k : k);
		lat[occurcount] = (double)j+5.0*
		  dabs(y-z)/(dabs(x-w)+dabs(y-z));
		lon[occurcount] = MIN(x, y)+dabs(x-y)*
		  dabs(y-z)/(dabs(x-w)+dabs(y-z));
		occurcount++;
	      }
	    }
	  }
  }
  if ((todisplay & 16384) == 0)
    return;
  putchar('\n');
  for (i = 1; i < occurcount; i++) {
    j = i-1;
    while (j >= 0 && lat[j] < lat[j+1]) {
      swapint(&obj1[j], &obj1[j+1]); swapint(&obj2[j], &obj2[j+1]);
      swapdoub(&lat[j], &lat[j+1]); swapdoub(&lon[j], &lon[j+1]);
      j--;
    }
  }
  for (i = 1; i < occurcount; i++) {
    j = abs(obj1[i]);
    printf("%c%c%c %s crosses ", OBJNAM(j),
	   obj1[i] > 0 ? "Ascendant " : "Descendant");
    j = abs(obj2[i] - (obj2[i] < 50 ? 0 : 100));
    printf("%c%c%c %s at ", OBJNAM(j),
	   obj2[i] < 50 ? (obj2[i] > 0 ? "Midheaven " :
	   "Nadir     ") : (obj2[i] > 100 ? "Ascendant " : "Descendant"));
    j = (int) ((dabs(lon[i])-floor(dabs(lon[i])))*60.0);
    printf("%3d %d%d'%c, ", (int) dabs(lon[i]),
	   j/10, j%10, lon[i] < 0.0 ? 'E' : 'W');
    j = (int) ((dabs(lat[i])-floor(dabs(lat[i])))*60.0);
    printf("%2d %d%d'%c\n", (int) dabs(lat[i]),
	   j/10, j%10, lat[i] < 0.0 ? 'S' : 'N');
  }
  if (!occurcount)
    printf("No latitude crossings.\n");
}

/**/
