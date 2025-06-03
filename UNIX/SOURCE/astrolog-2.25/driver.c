/*
** Astrolog (Version 2.25) File: driver.c
*/

#include "astrolog.h"

char *filenameout, **extralines;
int prog = FALSE, extracount = 0;

/*
*******************************************************************************
** Option processing
*******************************************************************************
*/

displayswitches()
{
  printf("Astrolog command switches (version %s) (%s):\n", VERSION, DATE); 
  printf(" -H: Display this help message.\n");
  printf(" -O: Display available planets and other celestial objects.\n");
  printf(" -O0: Line -O but ignore any restrictions.\n");
  printf(" -R [<obj1> [<obj2> ..]: Restrict specific bodies from displays.\n");
  printf(" -R0 [<obj1> ..]: Like -R but restrict everything first.\n");
  printf(" -R[C,u,U]: Restrict all minor cusps, all uranians, or stars.\n");
  printf(" -C: Include non-angular house cusps in charts.\n");
  printf(" -u: Include transneptunian/uranian bodies in charts.\n");
  printf(" -U: Include locations of fixed background stars in charts.\n");
  printf(" -U[z,l,n,b]: Order by azimuth, altitude, name, or brightness.\n");
  printf(" -A [<0-18>]: Display available aspects or limit their number.\n");
  printf(" -Ao <orb1> [<orb2> ..]: Specify max orbs for each aspect.\n");
  printf(" -AO [..]: Like -Ao but suspend objects' max orb restrictions.\n\n");
  printf("Switches which affect how a chart is computed:\n");
  printf(" -c <value>: Select a different default system of houses.\n");
  printf("     (0 = Placidus, 1 = Koch, 2 = Equal, 3 = Campanus, ");
  printf("4 = Meridian,\n     5 = Regiomontanus, 6 = Porphry, ");
  printf("7 = Morinus, 8 = Topocentric, 9 = None.)\n");
  printf(" -s: Compute siderial instead of the normal tropical chart.\n");
  printf(" -s0: Display locations as right ascension instead of degrees.\n");
  printf(" -h [<objnum>]: Compute positions centered on specified object.\n");
  printf(" -p <month> <day> <year>: Cast progressed chart based for date.\n");
#ifdef TIME
  printf(" -pn: Cast progressed chart based on current date now.\n");
#endif
  printf(" -p0 <days>: Set no. of days to progress / day (default 365.25).\n");
  printf(" -x <1-360>: Cast harmonic chart based on specified factor.\n");
  printf(" -1 [<objnum>]: Cast chart with specified object on ascendant.\n");
  printf(" -3: Display objects in their zodiac decan positions.\n");
  printf(" -f: Display houses as sign positions (flip them).\n");
  printf(" -G: Display houses based on geographic location only.\n");
  printf(" -+ <days>: Cast chart for specified no. of days in the future.\n");
  printf(" -- <days>: Cast chart for specified no. of days in the past.\n\n");
  printf("Switches which determine in what format the chart is displayed:\n");
  printf(" -v: Display list of object positions (chosen by default).\n");
  printf(" -v0: Like -v but express velocities relative to average speed.\n");
  printf(" -w: Display chart on screen in a graphic house wheel format.\n");
  printf(" -w0: Like -w but show objects in houses 4..9 in reverse order.\n");
  printf(" -g: Display aspect and midpoint grid among planets.\n");
  printf(" -g0: Like -g but flag aspect configurations (e.g. Yod's) too.\n");
  printf(" -g0: For comparison charts, show midpoints instead of aspects.\n");
  printf(" -ga: Like -g but indicate applying instead of difference orbs.\n");
  printf(" -Z: Display planet locations with respect to the local horizon.\n");
  printf(" -S: Display x,y,z coordinate positions of planets.\n");
  printf(" -I: Display interpretation of planetary influences.\n");
  printf(" -L [<step>]: Display astro-graph locations of planetary angles.\n");
  printf(" -L0 [..]: Like -L but display list of latitude crossings too.\n");
  printf(" -d: Print all aspects and changes occurring in a day.\n");
  printf(" -d0: Like -d but print all aspects for the entire month.\n");
  printf(" -dp <month> <year>: Print aspects within progressed chart.\n");
  printf(" -e: Print all options for chart (i.e. -v-w-g0-Z-S-I-L0-d).\n\n");
  printf("Switches which affect how the chart parameters are obtained:\n");
#ifdef TIME
  printf(" -n: Compute chart for this exact moment using current time.\n");
#endif
  printf(" -a <month> <date> <year> <time> <zone> <long> <lat>:\n");
  printf("     Compute chart automatically given specified data.\n");
  printf(" -z: Assume Daylight time (change default zone appropriately).\n");
  printf(" -z <zone>: Change the default time zone (for -d-q-T-E options).\n");
  printf(" -l <long> <lat>: ");
  printf("Change the default longitude & latitude.\n");
  printf(" -q <month> <date> <year>: Compute chart for noon on date.\n");
  printf(" -q0 <month> <date> <year> <time>: Like -q but include time too.\n");
  printf(" -i <file>: Compute chart based on info in file.\n");
  printf(" -o <file> [..]: Write parameters of current chart to file.\n");
  printf(" -o0 <file> [..]: Like -o but output planet/house positions.\n\n");
  printf("Other features or major modes:\n");
  printf(" -r <file1> <file2>: Compute a relationship synastry chart.\n");
  printf(" -rc <file1> <file2>: Compute a composite chart.\n");
  printf(" -rm <file1> <file2>: Compute a time space midpoint chart.\n");
  printf(" -r0 <file1> <file2>: Keep the charts separate in comparison.\n");
#ifdef TIME
  printf(" -t <file>: Compute current house transits for particular chart.\n");
#endif
  printf(" -T <file> <month> <year>: ");
  printf("Compute all transits in month for chart.\n");
  printf(" -T0 <..>: Like -T but include transits of the Moon as well.\n");
  printf(" -Tp <file> <month> <year>: ");
  printf("Compute all progressions in month for chart.\n");
  printf(" -E <month> <year>: Display ephemeris for given month.\n");
  printf(" -E0 <..>: Like -E but include Chiron and the asteroids as well.\n");
#ifdef X11
  xdisplayswitches();
#endif
}

printobjects(all)
int all;
{
  int i, j;
  double Off;
  if (!cusp)
    for (i = objects+1; i <= objects+4; i++)
      ignore[i] = TRUE;
  if (!uranian)
    for (i = objects+5; i <= BASE; i++)
      ignore[i] = TRUE;
  printf("Astrolog planets and objects:\n");
  printf("No. Name       Rule Co-Rule Fall Co-Fall Exalt Debilitate\n\n");
  if (all || universe) {
    Off = processinput(TRUE);
    caststar(siderial ? 0.0 : Off);
  }
  for (i = 1; i <= BASE; i++) if (all || !ignore[i]) {
    printf("%2d %s", i, objectname[i]);
    printtab(' ', 12-stringlen(objectname[i]));
    if (i <= objects) {
      if (ruler1[i]) {
	j = ruler2[i];
	printf("%c%c%c  %c%c%c     ", SIGNAM(ruler1[i]),
	       j ? signname[j][0] : ' ', j ? signname[j][1] : ' ',
	       j ? signname[j][2] : ' ');
	printf("%c%c%c  %c%c%c     ", SIGNAM(mod12(ruler1[i]+6)),
	       j ? signname[mod12(j+6)][0] : ' ',
	       j ? signname[mod12(j+6)][1] : ' ',
	       j ? signname[mod12(j+6)][2] : ' ');
	printf("%c%c%c   %c%c%c", SIGNAM(exalt[i]),
	       SIGNAM(mod12(exalt[i]+6)));
      }
    } else if (i <= objects+4)
      printf("Minor House Cusp #%d", i-objects);
    else
      printf("Uranian #%d", i-objects-4);
    putchar('\n');
  }
  if (all || universe)
    for (i = BASE+1; i <= total; i++) if (all | !ignore[i]) {
      j = BASE+starname[i-BASE];
      printf("%2d %s", i, objectname[j]);
      printtab(' ', 12-stringlen(objectname[j]));
      printf("Star #%2d    ", i-BASE);
      printminute(planet[j]);
      printf("  ");
      printaltitude(planetalt[j]);
      printf(" %5.2f\n", starbright[j-BASE]);
    }
}

printaspects()
{
  int i;
  printf("Astrolog aspects:\nNo. Angle    Orb       Abbrev. Name");
  printf("          Description of glyph\n\n");
  for (i = 1; i <= ASPECTS; i++) {
    printf("%2d %6.2f +/- %1.0f degrees (%s) %s", i,
	   aspectangle[i], aspectorb[i], aspectabbrev[i], aspectname[i]);
    printtab(' ', 15-stringlen(aspectname[i]));
    printf("%s\n", aspectglyph[i]);
  }
}

#define NEXTDEFAULT while(getc(data) != '=');

int inputdefaults()
{
  char name[STRING];
  int i;
  filename = DEFAULT_INFOFILE;
  data = fopen(filename, "r");
  if (data == NULL) {
    sprintf(name, "%s%s", DEFAULT_DIR, filename);
    data = fopen(name, "r");
    if (data == NULL)
      return FALSE;
  }
  NEXTDEFAULT; fscanf(data, "%lf", &defzone);
  NEXTDEFAULT; fscanf(data, "%lf", &deflong);
  NEXTDEFAULT; fscanf(data, "%lf", &deflat);
  NEXTDEFAULT; fscanf(data, "%d",  &aspects);
  NEXTDEFAULT; fscanf(data, "%d",  &housesystem);
  NEXTDEFAULT;
  for (i = 1; i <= objects; i++)
    fscanf(data, "%d", &ignore[i]);
  NEXTDEFAULT;
  for (i = 1; i <= ASPECTS; i++)
    fscanf(data, "%lf", &aspectorb[i]);
  NEXTDEFAULT;
  for (i = 1; i <= objects; i++)
    fscanf(data, "%lf", &objectinf[i]);
  for (i = 1; i <= SIGNS; i++)
    fscanf(data, "%lf", &houseinf[i]);
  for (i = 1; i <= ASPECTS; i++)
    fscanf(data, "%lf", &aspectinf[i]);
  fclose(data);
  return TRUE;
}

outputdata()
{
  int i, j;
  double k;
  data = fopen(filenameout, "w");
  if (data == NULL) {
    fprintf(stderr, "File %s can not be created.\n", filenameout);
    exit(1);
  }
  if (!(todisplay & 4096)) {
    if (Mon < 1) {
      fprintf(stderr, "\nCan't output chart with no time/space to file.\n");
      fclose(data);
      exit(1);
    }
    fprintf(data, "%.0f\n%.0f\n%.0f\n%.2f\n%.2f\n%.2f\n%.2f\n",
	    Mon, Day, Yea, Tim, Zon, Lon, Lat);
  } else {
    for (i = 1; i <= objects; i++) {
      j = (int) planet[i];
      fprintf(data, "%c%c%c: %2d %2d %10.7f\n", OBJNAM(i),
	      j%30, j/30+1, (planet[i]-floor(planet[i]))*60.0);
      k = planetalt[i];
      fprintf(data, "[%c]: %3d %12.8f\n",
	      ret[i] >= 0.0 ? 'D' : 'R', (int)(sgn(k)*
              floor(dabs(k))), (k-(double)(int)k)*60.0);
    }
    for (i = 1; i <= SIGNS/2; i++) {
      j = (int) house[i];
      fprintf(data, "H_%c: %2d %2d %10.7f\n",
	      'a'+i-1, j%30, j/30+1, (house[i]-floor(house[i]))*60.0);
    }
  }
  for (i = 1; i < extracount; i++) {
    extralines++;
    fprintf(data, "%s\n", extralines[1]);
  }
  fclose(data);
}

action()
{
  int i;
  if (!cusp)
    for (i = objects+1; i <= objects+4; i++)
      ignore[i] = TRUE;
  if (!uranian)
    for (i = objects+5; i <= BASE; i++)
      ignore[i] = TRUE;
  if (!universe)
    for (i = BASE+1; i <= total; i++)
      ignore[i] = TRUE;
  if (operation & 2)
    printtransit(prog);
  else if (operation & 4)
    printephemeris();
  else {
    if (!relation) {
      if (!autom)
	inputdata("tty");
      Mon = M; Day = D; Yea = Y; Tim = F; Zon = X; Lon = L5; Lat = LA;
      castchart(TRUE);
    } else
      castrelation();
#ifdef X11
    if (operation & 8)
      xaction();
    else
#endif
      printchart(prog);
  }
  if (operation & 1)
    outputdata();
}

#ifndef SWITCHES
#define MAXSWITCHES 20
int inputswitches(argv)
char argv[MAXSWITCHES][10];
{
  int argc = 0;
  printf("** Astrolog version %s (cruiser1@milton.u.washington.edu) **\n",
    VERSION); 
  printf("Enter all the switch parameters below. (Enter -H for help.)\n");
  printf("Press return after each switch or number parameter input.\n");
  printf("Input a '.' on a line by itself when done.\n\n");
  do {
    argc++;
    printf("Input parameter string #%2d: ", argc);
    if (gets(argv[argc]) == (char *) NULL) {
      printf("\nAstrolog terminated.\n");
      exit(1);
    }
  } while (argc < MAXSWITCHES && (argv[argc][0] != '.' || argv[argc][1] != 0));
  printf("\n");
  return argc;
}
#endif


/*
*******************************************************************************
** Main program
*******************************************************************************
*/

main(argc, argv)
int argc;
char **argv;
{
#ifdef TIME
  struct tm curtime;
  int curtimer;
#endif
  int pos, i, j;
  double k;
  char c;
#ifndef SWITCHES
  char strings[MAXSWITCHES][10];
  char *pointers[MAXSWITCHES];
  inputdefaults();
  for (i = 0; i < MAXSWITCHES; i++)
    pointers[i] = strings[i];
  argc = inputswitches(strings);
  argv = pointers;
#else
  inputdefaults();
#endif
  argc--; argv++;
  while (argc) {
    pos = 1;
    if (argv[0][0] == '-')
      pos++;
    switch (argv[0][pos-1]) {
    case 'H':
      displayswitches();
      exit(0);
    case 'O':
      printobjects(argv[0][pos] == '0');
      exit(0);
    case 'R':
      if (argv[0][pos] == '0')
	for (i = 1; i <= total; i++)
	  ignore[i] = TRUE;
      else if (argv[0][pos] == 'C')
	for (i = objects+1; i <= objects+4; i++)
	  ignore[i] = TRUE;
      else if (argv[0][pos] == 'u')
	for (i = objects+5; i <= BASE; i++)
	  ignore[i] = TRUE;
      else if (argv[0][pos] == 'U')
	for (i = BASE+1; i <= total; i++)
	  ignore[i] = TRUE;
      else if (argc <= 1 || (!atoi(argv[1]))) {
	for (i = 11; i <= 15; i++)
	  ignore[i] = 1-ignore[i];
	ignore[17] = 1-ignore[17]; ignore[20] = 1-ignore[20];
      }
      while (argc > 1 && (i = atoi(argv[1])))
	if (i < 1 || i > total) {
	  fprintf(stderr, "Bad value %d to switch -R\n", i);
	  exit(1);
	} else {
	  ignore[i] = 1-ignore[i];
	  argc--; argv++;
	}
      break;
    case 'u':
      uranian = TRUE;
      break;
    case 'U':
      if (argv[0][pos] == 'n' || argv[0][pos] == 'b' ||
	  argv[0][pos] == 'z' || argv[0][pos] == 'l')
	universe = argv[0][pos];
      else
	universe = TRUE;
      break;
    case 'C':
      cusp = TRUE;
      break;
    case 'A':
      if (argc <= 1) {
	printaspects();
	exit(0);
      }
      if (argv[0][pos] != 'o' && argv[0][pos] != 'O') {
	aspects = atoi(argv[1]);
	if (aspects < 0 || aspects > ASPECTS) {
	  fprintf(stderr, "Bad value %d to switch -A\n", aspects);
	  exit(1);
	}
	argc--; argv++;
      } else {
	if (argv[0][pos] == 'O')
	  orb(0, 0, 0);
	i = 1;
	while (argc > 1 && ((k = atof(argv[1])) || argv[1][0] == '0'))
	  if (k < -180.0 || k > 180.0 || i > ASPECTS) {
	    fprintf(stderr, "Bad value %.1f to switch -Ao\n", k);
	    exit(1);
	  } else {
	    aspectorb[i++] = k;
	    argc--; argv++;
	  }
      }
      break;
    case 's':
      if (argv[0][pos] == '0')
	right = TRUE;
      else
	siderial = TRUE;
      break;
    case 'h':
      if (argc > 1 && (centerplanet = atoi(argv[1]))) {
	argc--; argv++;
      } else
	centerplanet = 1;
      if (centerplanet < 0 || centerplanet == 2 || (centerplanet >= THINGS &&
	  centerplanet < objects+5) || centerplanet > BASE) {
	fprintf(stderr, "Bad value %d to switch -h\n", centerplanet);
	exit(1);
      }
      for (i = 0; i <= 11; i++) {
	c = objectname[0][i]; objectname[0][i] = objectname[centerplanet][i];
	objectname[centerplanet][i] = c;
      }
      if (centerplanet < 2)
	centerplanet = 1-centerplanet;
      break;
    case 'c':
      if (argc <= 1)
	toofew("c");
      housesystem = atoi(argv[1]);
      if (housesystem < 0 || housesystem >= SYSTEMS) {
	fprintf(stderr, "Bad value %d to switch -c\n", housesystem);
	exit(1);
      }
      argc--; argv++;
      break;
    case 'x':
      if (argc <= 1)
	toofew("x");
      multiplyfactor = atoi(argv[1]);
      if (multiplyfactor < 1 || multiplyfactor > DEGREES) {
	fprintf(stderr, "Bad value %d to switch -x\n", multiplyfactor);
	exit(1);
      }
      argc--; argv++;
      break;
    case '1':
      if (argc > 1 && (onasc = atoi(argv[1]))) {
	argc--; argv++;
      } else
	onasc = 1;
      if (onasc < 1 || onasc > objects) {
	fprintf(stderr, "Bad value %d to switch -1\n", onasc);
	exit(1);
      }
      break;
    case 'f':
      flip = TRUE;
      break;
    case '3':
      decan = TRUE;
      break;
    case 'G':
      geodetic = TRUE;
      break;
    case 'p':
#ifdef TIME
      if (argv[0][pos] == 'n') {
	progress = TRUE;
	curtimer = (int) time((int *) 0);
	curtime = *localtime(&curtimer);
	Mon = (double) curtime.tm_mon + 1.0;
	Day = (double) curtime.tm_mday;
	Yea = (double) curtime.tm_year + 1900.0;
	Jdp = mdytojulian(Mon, Day, Yea);
	break;
      }
#endif
      if (argv[0][pos] == '0') {
	if (argc <= 1)
	  toofew("p0");
	progday = atof(argv[1]);
	if (progday == 0.0) {
	  fprintf(stderr, "Bad progression value %.2f to switch -p0\n",
		  progday);
	  exit(1);
	}
	argc--; argv++;
	break;
      }
      if (argc <= 3)
	toofew("p");
      progress = TRUE;
      Mon = atof(argv[1]);
      Day = atof(argv[2]);
      Yea = atof(argv[3]);
      if (Mon < 1.0 || Mon > 12.0 || Day < 1.0 || Day > 31.0) {
	fprintf(stderr, "Bad date value %.0f,%.0f,%.0f to switch -p\n",
		Mon, Day, Yea);
	exit(1);
      }
      Jdp = mdytojulian(Mon, Day, Yea);
      argc -= 3; argv += 3;
      break;
    case '+':
      if (argc <= 1)
	toofew("+");
      Delta = atof(argv[1]);
      argc--; argv++;
      break;
    case '-': case '\0':
      if (argc <= 1)
	toofew("-");
      Delta = -atof(argv[1]);
      argc--; argv++;
      break;
    case 'v':
      if (argv[0][pos] == '0')
	todisplay = todisplay | 32768;
      todisplay = todisplay | 1;
      break;
    case 'w':
      if (argv[0][pos] == '0')
	todisplay = todisplay | 2048;
      todisplay = todisplay | 2;
      break;
    case 'g':
      if (argv[0][pos] == '0')
	todisplay = todisplay | 8192;
      if (argv[0][pos] == 'a') {
	todisplay = todisplay | 65536;
	if (argv[0][pos+1] == '0')
	  todisplay = todisplay | 8192;
      }
      todisplay = todisplay | 4;
      break;
    case 'Z':
      todisplay = todisplay | 8;
      break;
    case 'S':
      todisplay = todisplay | 16;
      break;
    case 'I':
      todisplay = todisplay | 32;
      break;
    case 'L':
      if (argv[0][pos] == '0')
	todisplay = todisplay | 16384;
      if (argc > 1 && (i = atoi(argv[1]))) {
	graphstep = i;
	argc--; argv++;
      }
      if (graphstep < 1 || 160%graphstep > 0) {
	fprintf(stderr, "Bad value %d to switch -L\n", graphstep);
	exit(1);
      }
      todisplay = todisplay | 64;
      break;
    case 'd':
      if (argv[0][pos] == 'p') {
	if (argc <= 2)
	  toofew("dp");
	prog = TRUE;
	todisplay = todisplay | 256;
	Mon2 = atof(argv[1]);
	Yea2 = atof(argv[2]);
	if (Mon2 < 0.0 || Mon2 > 12.0) {
	  fprintf(stderr, "Bad date value %.0f to switch -dp\n", Mon2);
	  exit(1);
	}
	argc -= 2; argv += 2;
      } else if (argv[0][pos] == '0')
	todisplay = todisplay | 256;
      todisplay = todisplay | 128;
      break;
    case 'e':
      todisplay = 255 | 8192 | 16384;
      break;
#ifdef TIME
    case 'n':
      inputdata("now");
      break;
#endif
    case 'l':
      if (argc <= 2)
	toofew("l");
      deflong = atof(argv[1]);
      deflat  = atof(argv[2]);
      argc -= 2; argv += 2;
      break;
    case 'z':
      if (argc <= 1 || atoi(argv[1]) == 0)
	defzone--;
      else {
	defzone = atof(argv[1]);
	if (defzone < -24.0 || defzone > 24.0) {
	  fprintf(stderr, "Bad value %.0f to switch -z\n", defzone);
	  exit(1);
	}
	argc--; argv++;
      }
      break;
    case 'a':
      if (argc <= 7)
	toofew("a");
      autom = TRUE;
      M = atof(argv[1]); D = atof(argv[2]); Y = atof(argv[3]);
      F = atof(argv[4]); X = atof(argv[5]);
      L5 = atof(argv[6]); LA = atof(argv[7]);
      argc -= 7; argv += 7;
      break;
    case 'q':
      i = (argv[0][pos] == '0');
      if (argc <= 3+i)
	toofew("q");
      autom = TRUE;
      M = atof(argv[1]); D = atof(argv[2]); Y = atof(argv[3]);
      F = i ? atof(argv[4]) : 12.0; X = defzone; L5 = deflong; LA = deflat;
      argc -= 3+i; argv += 3+i;
      break;
    case 'i':
      if (argc <= 1)
	toofew("i");
      inputdata(argv[1]);
      argc--; argv++;
      break;
    case 'o':
      if (argv[0][pos] == '0')
	todisplay = todisplay | 4096;
      if (argc <= 1)
	toofew("o");
      operation = operation | 1;
      filenameout = argv[1];
      extralines = argv;
      do {
	argc--; argv++;
	extracount++;
      } while (argc > 1 && argv[1][0] != '-');
      break;
    case 'r':
      if (argc <= 2)
	toofew("r");
      if (argv[0][pos] == 'c')
	relation = 2;
      else if (argv[0][pos] == 'm')
	relation = 3;
      else if (argv[0][pos] == '0')
	relation = -1;
      else
	relation = 1;
      filename = argv[1]; filename2 = argv[2];
      argc -= 2; argv += 2;
      break;
#ifdef TIME
    case 't':
      if (argc <= 1)
	toofew("t");
      relation = -1;
      filename = argv[1]; filename2 = "now";
      argc--; argv++;
      break;
#endif
    case 'T':
      if (argc <= 3)
	toofew("T");
      if (argv[0][pos] == 'p') {
	prog = TRUE;
	todisplay = todisplay | 512;
      } else if (argv[0][pos] == '0')
	todisplay = todisplay | 512;
      operation = 2;
      filename = argv[1];
      Mon2 = atof(argv[2]);
      Yea2 = atof(argv[3]);
      if (Mon2 < 0.0 || Mon2 > 12.0) {
	fprintf(stderr, "Bad date value %.0f to switch -T\n", Mon2);
	exit(1);
      }
      argc -= 3; argv += 3;
      break;
    case 'E':
      if (argc <= 2)
	toofew("E");
      if (argv[0][pos] == '0')
	todisplay = todisplay | 1024;
      operation = 4;
      Mon2 = atof(argv[1]);
      Yea2 = atof(argv[2]);
      if (Mon2 < 0.0 || Mon2 > 12.0) {
	fprintf(stderr, "Bad date value %.0f to switch -E\n", Mon2);
	exit(1);
      }
      argc -= 2; argv += 2;
      break;
#ifdef X11
    case 'X':
      i = xprocess(argc, argv, pos);
      operation = 8;
      argc -= i; argv += i;
      break;
#endif
    default:
      fprintf(stderr, "Unknown switch '%s'\n", argv[0]);
      exit(1);
    }
    argc--; argv++;
  }
  action();
}

/**/
