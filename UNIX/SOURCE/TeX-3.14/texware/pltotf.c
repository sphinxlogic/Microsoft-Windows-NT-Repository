#include "../common/extra.h"
#define bufsize 60 
#define maxheaderbytes 100 
#define maxparamwords 30 
#define maxligsteps 5000 
#define maxkerns 10000 
#define hashsize 15077 
#define noptions 1 
typedef unsigned char byte  ; 
typedef schar ASCIIcode  ; 
typedef struct {
    byte b0 ; 
  byte b1 ; 
  byte b2 ; 
  byte b3 ; 
} fourbytes  ; 
typedef integer fixword  ; 
typedef integer headerindex  ; 
typedef short indx  ; 
typedef short pointer  ; 
text plfile  ; 
file_ptr /* of  unsigned char */ tfmfile  ; 
char tfmname[FILENAMESIZE + 1], plname[FILENAMESIZE + 1]  ; 
ASCIIcode xord[256]  ; 
integer line  ; 
integer goodindent  ; 
integer indent  ; 
integer level  ; 
boolean leftln, rightln  ; 
integer limit  ; 
integer loc  ; 
char buffer[bufsize + 1]  ; 
boolean inputhasended  ; 
schar charsonline  ; 
ASCIIcode curchar  ; 
short start[89]  ; 
ASCIIcode dictionary[601]  ; 
schar startptr  ; 
short dictptr  ; 
ASCIIcode curname[21]  ; 
schar namelength  ; 
schar nameptr  ; 
schar nhash[101]  ; 
schar curhash  ; 
byte equiv[89]  ; 
byte curcode  ; 
fourbytes curbytes  ; 
integer fractiondigits[8]  ; 
byte headerbytes[maxheaderbytes + 1]  ; 
headerindex headerptr  ; 
fixword designsize  ; 
fixword designunits  ; 
boolean sevenbitsafeflag  ; 
fourbytes ligkern[maxligsteps + 1]  ; 
short nl  ; 
short minnl  ; 
fixword kern[maxkerns + 1]  ; 
integer nk  ; 
fourbytes exten[256]  ; 
short ne  ; 
fixword param[maxparamwords + 1]  ; 
integer np  ; 
boolean checksumspecified  ; 
short bchar  ; 
fixword memory[1033]  ; 
pointer memptr  ; 
pointer link[1033]  ; 
pointer charwd[256]  ; 
pointer charht[256]  ; 
pointer chardp[256]  ; 
pointer charic[256]  ; 
schar chartag[256]  ; 
unsigned short charremainder[257]  ; 
fixword nextd  ; 
byte index[1033]  ; 
byte excess  ; 
byte c  ; 
boolean lkstepended  ; 
integer krnptr  ; 
boolean sevenunsafe  ; 
fixword delta  ; 
integer ligptr  ; 
integer hash[hashsize + 1]  ; 
schar class[hashsize + 1]  ; 
short ligz[hashsize + 1]  ; 
integer hashptr  ; 
integer hashlist[hashsize + 1]  ; 
integer h, hh  ; 
indx tt  ; 
short xligcycle, yligcycle  ; 
byte bc  ; 
byte ec  ; 
byte lh  ; 
short lf  ; 
boolean notfound  ; 
fixword tempwidth  ; 
integer j  ; 
pointer p  ; 
schar q  ; 
integer parptr  ; 
struct {
    short rr ; 
  byte cc ; 
} labeltable[257]  ; 
short labelptr  ; 
short sortptr  ; 
short lkoffset  ; 
short t  ; 
boolean extralocneeded  ; 
boolean verbose  ; 

#include "pltotf.h"
void initialize ( ) 
{integer k  ; 
  schar h  ; 
  headerindex d  ; 
  byte c  ; 
  getoptstruct longoptions[noptions + 1]  ; 
  integer getoptreturnval  ; 
  integer optionindex  ; 
  if ( ( argc < 3 ) || ( argc > noptions + 3 ) ) 
  {
    (void) fprintf(stdout, "%s\n", "Usage: pltotf [-verbose] <property list file> <tfm file>." ) ; 
    uexit ( 1 ) ; 
  } 
  verbose = false ; 
  {
    longoptions [ 0 ] .name = "verbose" ; 
    longoptions [ 0 ] .hasarg = 0 ; 
    longoptions [ 0 ] .flag = addressofint ( verbose ) ; 
    longoptions [ 0 ] .val = 1 ; 
    longoptions [ 1 ] .name = 0 ; 
    longoptions [ 1 ] .hasarg = 0 ; 
    longoptions [ 1 ] .flag = 0 ; 
    longoptions [ 1 ] .val = 0 ; 
    do {
	getoptreturnval = getoptlongonly ( argc , gargv , "" , longoptions , 
      addressofint ( optionindex ) ) ; 
      if ( getoptreturnval != -1 ) 
      {
	if ( getoptreturnval == 63 ) 
	uexit ( 1 ) ; 
      } 
    } while ( ! ( getoptreturnval == -1 ) ) ; 
  } 
  if ( verbose ) 
  (void) fprintf(stdout, "%s\n", "This is PLtoTF, C Version 3.4" ) ; 
  argv ( optind , plname ) ; 
  reset ( plfile , plname ) ; 
  argv ( optind + 1 , tfmname ) ; 
  rewrite ( tfmfile , tfmname ) ; 
  {register integer for_end; k = 0 ; for_end = 127 ; if ( k <= for_end) do 
    xord [ chr ( k ) ] = 127 ; 
  while ( k++ < for_end ) ; } 
  xord [ ' ' ] = 32 ; 
  xord [ '!' ] = 33 ; 
  xord [ '"' ] = 34 ; 
  xord [ '#' ] = 35 ; 
  xord [ '$' ] = 36 ; 
  xord [ '%' ] = 37 ; 
  xord [ '&' ] = 38 ; 
  xord [ '\'' ] = 39 ; 
  xord [ '(' ] = 40 ; 
  xord [ ')' ] = 41 ; 
  xord [ '*' ] = 42 ; 
  xord [ '+' ] = 43 ; 
  xord [ ',' ] = 44 ; 
  xord [ '-' ] = 45 ; 
  xord [ '.' ] = 46 ; 
  xord [ '/' ] = 47 ; 
  xord [ '0' ] = 48 ; 
  xord [ '1' ] = 49 ; 
  xord [ '2' ] = 50 ; 
  xord [ '3' ] = 51 ; 
  xord [ '4' ] = 52 ; 
  xord [ '5' ] = 53 ; 
  xord [ '6' ] = 54 ; 
  xord [ '7' ] = 55 ; 
  xord [ '8' ] = 56 ; 
  xord [ '9' ] = 57 ; 
  xord [ ':' ] = 58 ; 
  xord [ ';' ] = 59 ; 
  xord [ '<' ] = 60 ; 
  xord [ '=' ] = 61 ; 
  xord [ '>' ] = 62 ; 
  xord [ '?' ] = 63 ; 
  xord [ '@' ] = 64 ; 
  xord [ 'A' ] = 65 ; 
  xord [ 'B' ] = 66 ; 
  xord [ 'C' ] = 67 ; 
  xord [ 'D' ] = 68 ; 
  xord [ 'E' ] = 69 ; 
  xord [ 'F' ] = 70 ; 
  xord [ 'G' ] = 71 ; 
  xord [ 'H' ] = 72 ; 
  xord [ 'I' ] = 73 ; 
  xord [ 'J' ] = 74 ; 
  xord [ 'K' ] = 75 ; 
  xord [ 'L' ] = 76 ; 
  xord [ 'M' ] = 77 ; 
  xord [ 'N' ] = 78 ; 
  xord [ 'O' ] = 79 ; 
  xord [ 'P' ] = 80 ; 
  xord [ 'Q' ] = 81 ; 
  xord [ 'R' ] = 82 ; 
  xord [ 'S' ] = 83 ; 
  xord [ 'T' ] = 84 ; 
  xord [ 'U' ] = 85 ; 
  xord [ 'V' ] = 86 ; 
  xord [ 'W' ] = 87 ; 
  xord [ 'X' ] = 88 ; 
  xord [ 'Y' ] = 89 ; 
  xord [ 'Z' ] = 90 ; 
  xord [ '[' ] = 91 ; 
  xord [ '\\' ] = 92 ; 
  xord [ ']' ] = 93 ; 
  xord [ '^' ] = 94 ; 
  xord [ '_' ] = 95 ; 
  xord [ '`' ] = 96 ; 
  xord [ 'a' ] = 97 ; 
  xord [ 'b' ] = 98 ; 
  xord [ 'c' ] = 99 ; 
  xord [ 'd' ] = 100 ; 
  xord [ 'e' ] = 101 ; 
  xord [ 'f' ] = 102 ; 
  xord [ 'g' ] = 103 ; 
  xord [ 'h' ] = 104 ; 
  xord [ 'i' ] = 105 ; 
  xord [ 'j' ] = 106 ; 
  xord [ 'k' ] = 107 ; 
  xord [ 'l' ] = 108 ; 
  xord [ 'm' ] = 109 ; 
  xord [ 'n' ] = 110 ; 
  xord [ 'o' ] = 111 ; 
  xord [ 'p' ] = 112 ; 
  xord [ 'q' ] = 113 ; 
  xord [ 'r' ] = 114 ; 
  xord [ 's' ] = 115 ; 
  xord [ 't' ] = 116 ; 
  xord [ 'u' ] = 117 ; 
  xord [ 'v' ] = 118 ; 
  xord [ 'w' ] = 119 ; 
  xord [ 'x' ] = 120 ; 
  xord [ 'y' ] = 121 ; 
  xord [ 'z' ] = 122 ; 
  xord [ '{' ] = 123 ; 
  xord [ '|' ] = 124 ; 
  xord [ '}' ] = 125 ; 
  xord [ '~' ] = 126 ; 
  line = 0 ; 
  goodindent = 0 ; 
  indent = 0 ; 
  level = 0 ; 
  limit = 0 ; 
  loc = 0 ; 
  leftln = true ; 
  rightln = true ; 
  inputhasended = false ; 
  charsonline = 0 ; 
  startptr = 1 ; 
  start [ 1 ] = 0 ; 
  dictptr = 0 ; 
  {register integer for_end; h = 0 ; for_end = 100 ; if ( h <= for_end) do 
    nhash [ h ] = 0 ; 
  while ( h++ < for_end ) ; } 
  {register integer for_end; d = 0 ; for_end = 18 * 4 - 1 ; if ( d <= 
  for_end) do 
    headerbytes [ d ] = 0 ; 
  while ( d++ < for_end ) ; } 
  headerbytes [ 8 ] = 11 ; 
  headerbytes [ 9 ] = 85 ; 
  headerbytes [ 10 ] = 78 ; 
  headerbytes [ 11 ] = 83 ; 
  headerbytes [ 12 ] = 80 ; 
  headerbytes [ 13 ] = 69 ; 
  headerbytes [ 14 ] = 67 ; 
  headerbytes [ 15 ] = 73 ; 
  headerbytes [ 16 ] = 70 ; 
  headerbytes [ 17 ] = 73 ; 
  headerbytes [ 18 ] = 69 ; 
  headerbytes [ 19 ] = 68 ; 
  {register integer for_end; d = 48 ; for_end = 59 ; if ( d <= for_end) do 
    headerbytes [ d ] = headerbytes [ d - 40 ] ; 
  while ( d++ < for_end ) ; } 
  designsize = 10 * 1048576L ; 
  designunits = 1048576L ; 
  sevenbitsafeflag = false ; 
  headerptr = 18 * 4 ; 
  nl = 0 ; 
  minnl = 0 ; 
  nk = 0 ; 
  ne = 0 ; 
  np = 0 ; 
  checksumspecified = false ; 
  bchar = 256 ; 
  charremainder [ 256 ] = 32767 ; 
  {register integer for_end; c = 0 ; for_end = 255 ; if ( c <= for_end) do 
    {
      charwd [ c ] = 0 ; 
      charht [ c ] = 0 ; 
      chardp [ c ] = 0 ; 
      charic [ c ] = 0 ; 
      chartag [ c ] = 0 ; 
      charremainder [ c ] = 0 ; 
    } 
  while ( c++ < for_end ) ; } 
  memory [ 0 ] = 2147483647L ; 
  memory [ 1 ] = 0 ; 
  link [ 1 ] = 0 ; 
  memory [ 2 ] = 0 ; 
  link [ 2 ] = 0 ; 
  memory [ 3 ] = 0 ; 
  link [ 3 ] = 0 ; 
  memory [ 4 ] = 0 ; 
  link [ 4 ] = 0 ; 
  memptr = 4 ; 
  hashptr = 0 ; 
  yligcycle = 256 ; 
  {register integer for_end; k = 0 ; for_end = hashsize ; if ( k <= for_end) 
  do 
    hash [ k ] = 0 ; 
  while ( k++ < for_end ) ; } 
} 
void showerrorcontext ( ) 
{integer k  ; 
  (void) fprintf(stdout, "%s%ld%s\n", " (line " , (long)line , ")." ) ; 
  if ( ! leftln ) 
  (void) Fputs(stdout, "..." ) ; 
  {register integer for_end; k = 1 ; for_end = loc ; if ( k <= for_end) do 
    (void) putc(buffer [ k ] , stdout);
  while ( k++ < for_end ) ; } 
  (void) fprintf(stdout, "%c\n", ' ' ) ; 
  if ( ! leftln ) 
  (void) Fputs(stdout, "   " ) ; 
  {register integer for_end; k = 1 ; for_end = loc ; if ( k <= for_end) do 
    (void) putc(' ' , stdout);
  while ( k++ < for_end ) ; } 
  {register integer for_end; k = loc + 1 ; for_end = limit ; if ( k <= 
  for_end) do 
    (void) putc(buffer [ k ] , stdout);
  while ( k++ < for_end ) ; } 
  if ( rightln ) 
  (void) fprintf(stdout, "%c\n", ' ' ) ; 
  else
  (void) fprintf(stdout, "%s\n", "..." ) ; 
  charsonline = 0 ; 
} 
void fillbuffer ( ) 
{leftln = rightln ; 
  limit = 0 ; 
  loc = 0 ; 
  if ( leftln ) 
  {
    if ( line > 0 ) 
    readln ( plfile ) ; 
    line = line + 1 ; 
  } 
  if ( eof ( plfile ) ) 
  {
    limit = 1 ; 
    buffer [ 1 ] = ')' ; 
    rightln = false ; 
    inputhasended = true ; 
  } 
  else {
      
    while ( ( limit < bufsize - 1 ) && ( ! eoln ( plfile ) ) ) {
	
      limit = limit + 1 ; 
      read ( plfile , buffer [ limit ] ) ; 
    } 
    buffer [ limit + 1 ] = ' ' ; 
    rightln = eoln ( plfile ) ; 
    if ( leftln ) 
    {
      while ( ( loc < limit ) && ( buffer [ loc + 1 ] == ' ' ) ) loc = loc + 1 
      ; 
      if ( loc < limit ) 
      {
	if ( level == 0 ) 
	if ( loc == 0 ) 
	goodindent = goodindent + 1 ; 
	else {
	    
	  if ( goodindent >= 10 ) 
	  {
	    if ( charsonline > 0 ) 
	    (void) fprintf(stdout, "%c\n", ' ' ) ; 
	    (void) Fputs(stdout, "Warning: Indented line occurred at level zero" ) ; 
	    showerrorcontext () ; 
	  } 
	  goodindent = 0 ; 
	  indent = 0 ; 
	} 
	else if ( indent == 0 ) 
	if ( loc % level == 0 ) 
	{
	  indent = loc / level ; 
	  goodindent = 1 ; 
	} 
	else goodindent = 0 ; 
	else if ( indent * level == loc ) 
	goodindent = goodindent + 1 ; 
	else {
	    
	  if ( goodindent >= 10 ) 
	  {
	    if ( charsonline > 0 ) 
	    (void) fprintf(stdout, "%c\n", ' ' ) ; 
	    (void) fprintf(stdout, "%s%s%ld", "Warning: Inconsistent indentation; " ,             "you are at parenthesis level " , (long)level ) ; 
	    showerrorcontext () ; 
	  } 
	  goodindent = 0 ; 
	  indent = 0 ; 
	} 
      } 
    } 
  } 
} 
void getkeywordchar ( ) 
{while ( ( loc == limit ) && ( ! rightln ) ) fillbuffer () ; 
  if ( loc == limit ) 
  curchar = 32 ; 
  else {
      
    curchar = xord [ buffer [ loc + 1 ] ] ; 
    if ( curchar >= 97 ) 
    curchar = curchar - 32 ; 
    if ( ( ( curchar >= 48 ) && ( curchar <= 57 ) ) ) 
    loc = loc + 1 ; 
    else if ( ( ( curchar >= 65 ) && ( curchar <= 90 ) ) ) 
    loc = loc + 1 ; 
    else if ( curchar == 47 ) 
    loc = loc + 1 ; 
    else if ( curchar == 62 ) 
    loc = loc + 1 ; 
    else curchar = 32 ; 
  } 
} 
void getnext ( ) 
{while ( loc == limit ) fillbuffer () ; 
  loc = loc + 1 ; 
  curchar = xord [ buffer [ loc ] ] ; 
  if ( curchar >= 97 ) 
  if ( curchar <= 122 ) 
  curchar = curchar - 32 ; 
  else {
      
    if ( curchar == 127 ) 
    {
      {
	if ( charsonline > 0 ) 
	(void) fprintf(stdout, "%c\n", ' ' ) ; 
	(void) Fputs(stdout, "Illegal character in the file" ) ; 
	showerrorcontext () ; 
      } 
      curchar = 63 ; 
    } 
  } 
  else if ( ( curchar <= 41 ) && ( curchar >= 40 ) ) 
  loc = loc - 1 ; 
} 
void skiptoendofitem ( ) 
{integer l  ; 
  l = level ; 
  while ( level >= l ) {
      
    while ( loc == limit ) fillbuffer () ; 
    loc = loc + 1 ; 
    if ( buffer [ loc ] == ')' ) 
    level = level - 1 ; 
    else if ( buffer [ loc ] == '(' ) 
    level = level + 1 ; 
  } 
  if ( inputhasended ) 
  {
    if ( charsonline > 0 ) 
    (void) fprintf(stdout, "%c\n", ' ' ) ; 
    (void) Fputs(stdout, "File ended unexpectedly: No closing \")\"" ) ; 
    showerrorcontext () ; 
  } 
  curchar = 32 ; 
} 
void finishtheproperty ( ) 
{while ( curchar == 32 ) getnext () ; 
  if ( curchar != 41 ) 
  {
    if ( charsonline > 0 ) 
    (void) fprintf(stdout, "%c\n", ' ' ) ; 
    (void) Fputs(stdout, "Junk after property value will be ignored" ) ; 
    showerrorcontext () ; 
  } 
  skiptoendofitem () ; 
} 
void lookup ( ) 
{schar k  ; 
  short j  ; 
  boolean notfound  ; 
  curhash = curname [ 1 ] ; 
  {register integer for_end; k = 2 ; for_end = namelength ; if ( k <= 
  for_end) do 
    curhash = ( curhash + curhash + curname [ k ] ) % 101 ; 
  while ( k++ < for_end ) ; } 
  notfound = true ; 
  while ( notfound ) {
      
    if ( curhash == 0 ) 
    curhash = 100 ; 
    else curhash = curhash - 1 ; 
    if ( nhash [ curhash ] == 0 ) 
    notfound = false ; 
    else {
	
      j = start [ nhash [ curhash ] ] ; 
      if ( start [ nhash [ curhash ] + 1 ] == j + namelength ) 
      {
	notfound = false ; 
	{register integer for_end; k = 1 ; for_end = namelength ; if ( k <= 
	for_end) do 
	  if ( dictionary [ j + k - 1 ] != curname [ k ] ) 
	  notfound = true ; 
	while ( k++ < for_end ) ; } 
      } 
    } 
  } 
  nameptr = nhash [ curhash ] ; 
} 
void entername ( byte v ) 
{schar k  ; 
  {register integer for_end; k = 1 ; for_end = namelength ; if ( k <= 
  for_end) do 
    curname [ k ] = curname [ k + 20 - namelength ] ; 
  while ( k++ < for_end ) ; } 
  lookup () ; 
  nhash [ curhash ] = startptr ; 
  equiv [ startptr ] = v ; 
  {register integer for_end; k = 1 ; for_end = namelength ; if ( k <= 
  for_end) do 
    {
      dictionary [ dictptr ] = curname [ k ] ; 
      dictptr = dictptr + 1 ; 
    } 
  while ( k++ < for_end ) ; } 
  startptr = startptr + 1 ; 
  start [ startptr ] = dictptr ; 
} 
void getname ( ) 
{loc = loc + 1 ; 
  level = level + 1 ; 
  curchar = 32 ; 
  while ( curchar == 32 ) getnext () ; 
  if ( ( curchar > 41 ) || ( curchar < 40 ) ) 
  loc = loc - 1 ; 
  namelength = 0 ; 
  getkeywordchar () ; 
  while ( curchar != 32 ) {
      
    if ( namelength == 20 ) 
    curname [ 1 ] = 88 ; 
    else namelength = namelength + 1 ; 
    curname [ namelength ] = curchar ; 
    getkeywordchar () ; 
  } 
  lookup () ; 
  if ( nameptr == 0 ) 
  {
    if ( charsonline > 0 ) 
    (void) fprintf(stdout, "%c\n", ' ' ) ; 
    (void) Fputs(stdout, "Sorry, I don't know that property name" ) ; 
    showerrorcontext () ; 
  } 
  curcode = equiv [ nameptr ] ; 
} 
byte getbyte ( ) 
{register byte Result; integer acc  ; 
  ASCIIcode t  ; 
  do {
      getnext () ; 
  } while ( ! ( curchar != 32 ) ) ; 
  t = curchar ; 
  acc = 0 ; 
  do {
      getnext () ; 
  } while ( ! ( curchar != 32 ) ) ; 
  if ( t == 67 ) 
  if ( ( curchar >= 33 ) && ( curchar <= 126 ) && ( ( curchar < 40 ) || ( 
  curchar > 41 ) ) ) 
  acc = xord [ buffer [ loc ] ] ; 
  else {
      
    {
      if ( charsonline > 0 ) 
      (void) fprintf(stdout, "%c\n", ' ' ) ; 
      (void) Fputs(stdout, "\"C\" value must be standard ASCII and not a paren" ) ; 
      showerrorcontext () ; 
    } 
    do {
	getnext () ; 
    } while ( ! ( ( curchar == 40 ) || ( curchar == 41 ) ) ) ; 
  } 
  else if ( t == 68 ) 
  {
    while ( ( curchar >= 48 ) && ( curchar <= 57 ) ) {
	
      acc = acc * 10 + curchar - 48 ; 
      if ( acc > 255 ) 
      {
	{
	  {
	    if ( charsonline > 0 ) 
	    (void) fprintf(stdout, "%c\n", ' ' ) ; 
	    (void) Fputs(stdout, "This value shouldn't exceed 255" ) ; 
	    showerrorcontext () ; 
	  } 
	  do {
	      getnext () ; 
	  } while ( ! ( ( curchar == 40 ) || ( curchar == 41 ) ) ) ; 
	} 
	acc = 0 ; 
	curchar = 32 ; 
      } 
      else getnext () ; 
    } 
    {
      if ( ( curchar > 41 ) || ( curchar < 40 ) ) 
      loc = loc - 1 ; 
    } 
  } 
  else if ( t == 79 ) 
  {
    while ( ( curchar >= 48 ) && ( curchar <= 55 ) ) {
	
      acc = acc * 8 + curchar - 48 ; 
      if ( acc > 255 ) 
      {
	{
	  {
	    if ( charsonline > 0 ) 
	    (void) fprintf(stdout, "%c\n", ' ' ) ; 
	    (void) Fputs(stdout, "This value shouldn't exceed '377" ) ; 
	    showerrorcontext () ; 
	  } 
	  do {
	      getnext () ; 
	  } while ( ! ( ( curchar == 40 ) || ( curchar == 41 ) ) ) ; 
	} 
	acc = 0 ; 
	curchar = 32 ; 
      } 
      else getnext () ; 
    } 
    {
      if ( ( curchar > 41 ) || ( curchar < 40 ) ) 
      loc = loc - 1 ; 
    } 
  } 
  else if ( t == 72 ) 
  {
    while ( ( ( curchar >= 48 ) && ( curchar <= 57 ) ) || ( ( curchar >= 65 ) 
    && ( curchar <= 70 ) ) ) {
	
      if ( curchar >= 65 ) 
      curchar = curchar - 7 ; 
      acc = acc * 16 + curchar - 48 ; 
      if ( acc > 255 ) 
      {
	{
	  {
	    if ( charsonline > 0 ) 
	    (void) fprintf(stdout, "%c\n", ' ' ) ; 
	    (void) Fputs(stdout, "This value shouldn't exceed \"FF" ) ; 
	    showerrorcontext () ; 
	  } 
	  do {
	      getnext () ; 
	  } while ( ! ( ( curchar == 40 ) || ( curchar == 41 ) ) ) ; 
	} 
	acc = 0 ; 
	curchar = 32 ; 
      } 
      else getnext () ; 
    } 
    {
      if ( ( curchar > 41 ) || ( curchar < 40 ) ) 
      loc = loc - 1 ; 
    } 
  } 
  else if ( t == 70 ) 
  {
    if ( curchar == 66 ) 
    acc = 2 ; 
    else if ( curchar == 76 ) 
    acc = 4 ; 
    else if ( curchar != 77 ) 
    acc = 18 ; 
    getnext () ; 
    if ( curchar == 73 ) 
    acc = acc + 1 ; 
    else if ( curchar != 82 ) 
    acc = 18 ; 
    getnext () ; 
    if ( curchar == 67 ) 
    acc = acc + 6 ; 
    else if ( curchar == 69 ) 
    acc = acc + 12 ; 
    else if ( curchar != 82 ) 
    acc = 18 ; 
    if ( acc >= 18 ) 
    {
      {
	{
	  if ( charsonline > 0 ) 
	  (void) fprintf(stdout, "%c\n", ' ' ) ; 
	  (void) Fputs(stdout, "Illegal face code, I changed it to MRR" ) ; 
	  showerrorcontext () ; 
	} 
	do {
	    getnext () ; 
	} while ( ! ( ( curchar == 40 ) || ( curchar == 41 ) ) ) ; 
      } 
      acc = 0 ; 
    } 
  } 
  else {
      
    {
      if ( charsonline > 0 ) 
      (void) fprintf(stdout, "%c\n", ' ' ) ; 
      (void) Fputs(stdout, "You need \"C\" or \"D\" or \"O\" or \"H\" or \"F\" here" ) ; 
      showerrorcontext () ; 
    } 
    do {
	getnext () ; 
    } while ( ! ( ( curchar == 40 ) || ( curchar == 41 ) ) ) ; 
  } 
  curchar = 32 ; 
  Result = acc ; 
  return(Result) ; 
} 
void getfourbytes ( ) 
{integer c  ; 
  integer r  ; 
  integer q  ; 
  do {
      getnext () ; 
  } while ( ! ( curchar != 32 ) ) ; 
  r = 0 ; 
  curbytes .b0 = 0 ; 
  curbytes .b1 = 0 ; 
  curbytes .b2 = 0 ; 
  curbytes .b3 = 0 ; 
  if ( curchar == 72 ) 
  r = 16 ; 
  else if ( curchar == 79 ) 
  r = 8 ; 
  else {
      
    {
      if ( charsonline > 0 ) 
      (void) fprintf(stdout, "%c\n", ' ' ) ; 
      (void) Fputs(stdout, "An octal (\"O\") or hex (\"H\") value is needed here" ) ; 
      showerrorcontext () ; 
    } 
    do {
	getnext () ; 
    } while ( ! ( ( curchar == 40 ) || ( curchar == 41 ) ) ) ; 
  } 
  if ( r > 0 ) 
  {
    q = 256 / r ; 
    do {
	getnext () ; 
    } while ( ! ( curchar != 32 ) ) ; 
    while ( ( ( curchar >= 48 ) && ( curchar <= 57 ) ) || ( ( curchar >= 65 ) 
    && ( curchar <= 70 ) ) ) {
	
      if ( curchar >= 65 ) 
      curchar = curchar - 7 ; 
      c = ( r * curbytes .b0 ) + ( curbytes .b1 / q ) ; 
      if ( c > 255 ) 
      {
	curbytes .b0 = 0 ; 
	curbytes .b1 = 0 ; 
	curbytes .b2 = 0 ; 
	curbytes .b3 = 0 ; 
	if ( r == 8 ) 
	{
	  {
	    if ( charsonline > 0 ) 
	    (void) fprintf(stdout, "%c\n", ' ' ) ; 
	    (void) Fputs(stdout, "Sorry, the maximum octal value is O 37777777777" ) ; 
	    showerrorcontext () ; 
	  } 
	  do {
	      getnext () ; 
	  } while ( ! ( ( curchar == 40 ) || ( curchar == 41 ) ) ) ; 
	} 
	else {
	    
	  {
	    if ( charsonline > 0 ) 
	    (void) fprintf(stdout, "%c\n", ' ' ) ; 
	    (void) Fputs(stdout, "Sorry, the maximum hex value is H FFFFFFFF" ) ; 
	    showerrorcontext () ; 
	  } 
	  do {
	      getnext () ; 
	  } while ( ! ( ( curchar == 40 ) || ( curchar == 41 ) ) ) ; 
	} 
      } 
      else if ( curchar >= 48 + r ) 
      {
	{
	  if ( charsonline > 0 ) 
	  (void) fprintf(stdout, "%c\n", ' ' ) ; 
	  (void) Fputs(stdout, "Illegal digit" ) ; 
	  showerrorcontext () ; 
	} 
	do {
	    getnext () ; 
	} while ( ! ( ( curchar == 40 ) || ( curchar == 41 ) ) ) ; 
      } 
      else {
	  
	curbytes .b0 = c ; 
	curbytes .b1 = ( r * ( curbytes .b1 % q ) ) + ( curbytes .b2 / q ) ; 
	curbytes .b2 = ( r * ( curbytes .b2 % q ) ) + ( curbytes .b3 / q ) ; 
	curbytes .b3 = ( r * ( curbytes .b3 % q ) ) + curchar - 48 ; 
	getnext () ; 
      } 
    } 
  } 
} 
fixword getfix ( ) 
{register fixword Result; boolean negative  ; 
  integer acc  ; 
  integer intpart  ; 
  schar j  ; 
  do {
      getnext () ; 
  } while ( ! ( curchar != 32 ) ) ; 
  negative = false ; 
  acc = 0 ; 
  if ( ( curchar != 82 ) && ( curchar != 68 ) ) 
  {
    {
      if ( charsonline > 0 ) 
      (void) fprintf(stdout, "%c\n", ' ' ) ; 
      (void) Fputs(stdout, "An \"R\" or \"D\" value is needed here" ) ; 
      showerrorcontext () ; 
    } 
    do {
	getnext () ; 
    } while ( ! ( ( curchar == 40 ) || ( curchar == 41 ) ) ) ; 
  } 
  else {
      
    do {
	getnext () ; 
      if ( curchar == 45 ) 
      {
	curchar = 32 ; 
	negative = true ; 
      } 
      else if ( curchar == 43 ) 
      curchar = 32 ; 
    } while ( ! ( curchar != 32 ) ) ; 
    while ( ( curchar >= 48 ) && ( curchar <= 57 ) ) {
	
      acc = acc * 10 + curchar - 48 ; 
      if ( acc >= 2048 ) 
      {
	{
	  {
	    if ( charsonline > 0 ) 
	    (void) fprintf(stdout, "%c\n", ' ' ) ; 
	    (void) Fputs(stdout, "Real constants must be less than 2048" ) ; 
	    showerrorcontext () ; 
	  } 
	  do {
	      getnext () ; 
	  } while ( ! ( ( curchar == 40 ) || ( curchar == 41 ) ) ) ; 
	} 
	acc = 0 ; 
	curchar = 32 ; 
      } 
      else getnext () ; 
    } 
    intpart = acc ; 
    acc = 0 ; 
    if ( curchar == 46 ) 
    {
      j = 0 ; 
      getnext () ; 
      while ( ( curchar >= 48 ) && ( curchar <= 57 ) ) {
	  
	if ( j < 7 ) 
	{
	  j = j + 1 ; 
	  fractiondigits [ j ] = 2097152L * ( curchar - 48 ) ; 
	} 
	getnext () ; 
      } 
      acc = 0 ; 
      while ( j > 0 ) {
	  
	acc = fractiondigits [ j ] + ( acc / 10 ) ; 
	j = j - 1 ; 
      } 
      acc = ( acc + 10 ) / 20 ; 
    } 
    if ( ( acc >= 1048576L ) && ( intpart == 2047 ) ) 
    {
      {
	if ( charsonline > 0 ) 
	(void) fprintf(stdout, "%c\n", ' ' ) ; 
	(void) Fputs(stdout, "Real constants must be less than 2048" ) ; 
	showerrorcontext () ; 
      } 
      do {
	  getnext () ; 
      } while ( ! ( ( curchar == 40 ) || ( curchar == 41 ) ) ) ; 
    } 
    else acc = intpart * 1048576L + acc ; 
  } 
  if ( negative ) 
  Result = - (integer) acc ; 
  else Result = acc ; 
  return(Result) ; 
} 
pointer sortin ( pointer h , fixword d ) 
{register pointer Result; pointer p  ; 
  if ( ( d == 0 ) && ( h != 1 ) ) 
  Result = 0 ; 
  else {
      
    p = h ; 
    while ( d >= memory [ link [ p ] ] ) p = link [ p ] ; 
    if ( ( d == memory [ p ] ) && ( p != h ) ) 
    Result = p ; 
    else if ( memptr == 1032 ) 
    {
      {
	if ( charsonline > 0 ) 
	(void) fprintf(stdout, "%c\n", ' ' ) ; 
	(void) Fputs(stdout, "Memory overflow: more than 1028 widths, etc" ) ; 
	showerrorcontext () ; 
      } 
      (void) fprintf(stdout, "%s\n", "Congratulations! It's hard to make this error." ) ; 
      Result = p ; 
    } 
    else {
	
      memptr = memptr + 1 ; 
      memory [ memptr ] = d ; 
      link [ memptr ] = link [ p ] ; 
      link [ p ] = memptr ; 
      memory [ h ] = memory [ h ] + 1 ; 
      Result = memptr ; 
    } 
  } 
  return(Result) ; 
} 
integer mincover ( pointer h , fixword d ) 
{register integer Result; pointer p  ; 
  fixword l  ; 
  integer m  ; 
  m = 0 ; 
  p = link [ h ] ; 
  nextd = memory [ 0 ] ; 
  while ( p != 0 ) {
      
    m = m + 1 ; 
    l = memory [ p ] ; 
    while ( memory [ link [ p ] ] <= l + d ) p = link [ p ] ; 
    p = link [ p ] ; 
    if ( memory [ p ] - l < nextd ) 
    nextd = memory [ p ] - l ; 
  } 
  Result = m ; 
  return(Result) ; 
} 
fixword shorten ( pointer h , integer m ) 
{register fixword Result; fixword d  ; 
  integer k  ; 
  if ( memory [ h ] > m ) 
  {
    excess = memory [ h ] - m ; 
    k = mincover ( h , 0 ) ; 
    d = nextd ; 
    do {
	d = d + d ; 
      k = mincover ( h , d ) ; 
    } while ( ! ( k <= m ) ) ; 
    d = d / 2 ; 
    k = mincover ( h , d ) ; 
    while ( k > m ) {
	
      d = nextd ; 
      k = mincover ( h , d ) ; 
    } 
    Result = d ; 
  } 
  else Result = 0 ; 
  return(Result) ; 
} 
void setindices ( pointer h , fixword d ) 
{pointer p  ; 
  pointer q  ; 
  byte m  ; 
  fixword l  ; 
  q = h ; 
  p = link [ q ] ; 
  m = 0 ; 
  while ( p != 0 ) {
      
    m = m + 1 ; 
    l = memory [ p ] ; 
    index [ p ] = m ; 
    while ( memory [ link [ p ] ] <= l + d ) {
	
      p = link [ p ] ; 
      index [ p ] = m ; 
      excess = excess - 1 ; 
      if ( excess == 0 ) 
      d = 0 ; 
    } 
    link [ q ] = p ; 
    memory [ p ] = l + ( memory [ p ] - l ) / 2 ; 
    q = p ; 
    p = link [ p ] ; 
  } 
  memory [ h ] = m ; 
} 
void junkerror ( ) 
{{
    
    if ( charsonline > 0 ) 
    (void) fprintf(stdout, "%c\n", ' ' ) ; 
    (void) Fputs(stdout, "There's junk here that is not in parentheses" ) ; 
    showerrorcontext () ; 
  } 
  do {
      getnext () ; 
  } while ( ! ( ( curchar == 40 ) || ( curchar == 41 ) ) ) ; 
} 
void readfourbytes ( headerindex l ) 
{getfourbytes () ; 
  headerbytes [ l ] = curbytes .b0 ; 
  headerbytes [ l + 1 ] = curbytes .b1 ; 
  headerbytes [ l + 2 ] = curbytes .b2 ; 
  headerbytes [ l + 3 ] = curbytes .b3 ; 
} 
void readBCPL ( headerindex l , byte n ) 
{headerindex k  ; 
  k = l ; 
  while ( curchar == 32 ) getnext () ; 
  while ( ( curchar != 40 ) && ( curchar != 41 ) ) {
      
    if ( k < l + n ) 
    k = k + 1 ; 
    if ( k < l + n ) 
    headerbytes [ k ] = curchar ; 
    getnext () ; 
  } 
  if ( k == l + n ) 
  {
    {
      if ( charsonline > 0 ) 
      (void) fprintf(stdout, "%c\n", ' ' ) ; 
      (void) fprintf(stdout, "%s%ld%s", "String is too long; its first " , (long)n - 1 ,       " characters will be kept" ) ; 
      showerrorcontext () ; 
    } 
    k = k - 1 ; 
  } 
  headerbytes [ l ] = k - l ; 
  while ( k < l + n - 1 ) {
      
    k = k + 1 ; 
    headerbytes [ k ] = 0 ; 
  } 
} 
void checktag ( byte c ) 
{switch ( chartag [ c ] ) 
  {case 0 : 
    ; 
    break ; 
  case 1 : 
    {
      if ( charsonline > 0 ) 
      (void) fprintf(stdout, "%c\n", ' ' ) ; 
      (void) Fputs(stdout, "This character already appeared in a LIGTABLE LABEL" ) ; 
      showerrorcontext () ; 
    } 
    break ; 
  case 2 : 
    {
      if ( charsonline > 0 ) 
      (void) fprintf(stdout, "%c\n", ' ' ) ; 
      (void) Fputs(stdout, "This character already has a NEXTLARGER spec" ) ; 
      showerrorcontext () ; 
    } 
    break ; 
  case 3 : 
    {
      if ( charsonline > 0 ) 
      (void) fprintf(stdout, "%c\n", ' ' ) ; 
      (void) Fputs(stdout, "This character already has a VARCHAR spec" ) ; 
      showerrorcontext () ; 
    } 
    break ; 
  } 
} 
void printoctal ( byte c ) 
{(void) fprintf(stdout, "%c%ld%ld%ld", '\'' , (long)( c / 64 ) , (long)( ( c / 8 ) % 8 ) , (long)( c % 8 ) ) ; 
} 
boolean hashinput ( indx p , indx c ) 
{/* 30 */ register boolean Result; schar cc  ; 
  unsigned char zz  ; 
  unsigned char y  ; 
  integer key  ; 
  integer t  ; 
  if ( hashptr == hashsize ) 
  {
    Result = false ; 
    goto lab30 ; 
  } 
  y = ligkern [ p ] .b1 ; 
  t = ligkern [ p ] .b2 ; 
  cc = 0 ; 
  zz = ligkern [ p ] .b3 ; 
  if ( t >= 128 ) 
  zz = y ; 
  else {
      
    switch ( t ) 
    {case 0 : 
    case 6 : 
      ; 
      break ; 
    case 5 : 
    case 11 : 
      zz = y ; 
      break ; 
    case 1 : 
    case 7 : 
      cc = 1 ; 
      break ; 
    case 2 : 
      cc = 2 ; 
      break ; 
    case 3 : 
      cc = 3 ; 
      break ; 
    } 
  } 
  key = 256 * c + y + 1 ; 
  h = ( 1009 * key ) % hashsize ; 
  while ( hash [ h ] > 0 ) {
      
    if ( hash [ h ] <= key ) 
    {
      if ( hash [ h ] == key ) 
      {
	Result = false ; 
	goto lab30 ; 
      } 
      t = hash [ h ] ; 
      hash [ h ] = key ; 
      key = t ; 
      t = class [ h ] ; 
      class [ h ] = cc ; 
      cc = t ; 
      t = ligz [ h ] ; 
      ligz [ h ] = zz ; 
      zz = t ; 
    } 
    if ( h > 0 ) 
    h = h - 1 ; 
    else h = hashsize ; 
  } 
  hash [ h ] = key ; 
  class [ h ] = cc ; 
  ligz [ h ] = zz ; 
  hashptr = hashptr + 1 ; 
  hashlist [ hashptr ] = h ; 
  Result = true ; 
  lab30: ; 
  return(Result) ; 
} 
#ifdef notdef
indx f ( indx h , indx x , indx y ) 
{register indx Result; ; 
  return(Result) ; 
} 
#endif /* notdef */
indx eval ( indx x , indx y ) 
{register indx Result; integer key  ; 
  key = 256 * x + y + 1 ; 
  h = ( 1009 * key ) % hashsize ; 
  while ( hash [ h ] > key ) if ( h > 0 ) 
  h = h - 1 ; 
  else h = hashsize ; 
  if ( hash [ h ] < key ) 
  Result = y ; 
  else Result = f ( h , x , y ) ; 
  return(Result) ; 
} 
indx f ( indx h , indx x , indx y ) 
{register indx Result; switch ( class [ h ] ) 
  {case 0 : 
    ; 
    break ; 
  case 1 : 
    {
      class [ h ] = 4 ; 
      ligz [ h ] = eval ( ligz [ h ] , y ) ; 
      class [ h ] = 0 ; 
    } 
    break ; 
  case 2 : 
    {
      class [ h ] = 4 ; 
      ligz [ h ] = eval ( x , ligz [ h ] ) ; 
      class [ h ] = 0 ; 
    } 
    break ; 
  case 3 : 
    {
      class [ h ] = 4 ; 
      ligz [ h ] = eval ( eval ( x , ligz [ h ] ) , y ) ; 
      class [ h ] = 0 ; 
    } 
    break ; 
  case 4 : 
    {
      xligcycle = x ; 
      yligcycle = y ; 
      ligz [ h ] = 257 ; 
      class [ h ] = 0 ; 
    } 
    break ; 
  } 
  Result = ligz [ h ] ; 
  return(Result) ; 
} 
void outscaled ( fixword x ) 
{byte n  ; 
  unsigned short m  ; 
  if ( fabs ( x / ((double) designunits ) ) >= 16.0 ) 
  {
    (void) Fputs(stdout, "The relative dimension " ) ; 
    printreal ( x / ((double) 1048576L ) , 1 , 3 ) ; 
    (void) fprintf(stdout, "%s\n", " is too large." ) ; 
    (void) Fputs(stdout, "  (Must be less than 16*designsize" ) ; 
    if ( designunits != 1048576L ) 
    {
      (void) Fputs(stdout, " =" ) ; 
      printreal ( designunits / ((double) 65536L ) , 1 , 3 ) ; 
      (void) Fputs(stdout, " designunits" ) ; 
    } 
    (void) fprintf(stdout, "%c\n", ')' ) ; 
    x = 0 ; 
  } 
  if ( designunits != 1048576L ) 
  x = round ( ( x / ((double) designunits ) ) * 1048576.0 ) ; 
  if ( x < 0 ) 
  {
    putbyte ( 255 , tfmfile ) ; 
    x = x + 16777216L ; 
    if ( x <= 0 ) 
    x = 1 ; 
  } 
  else {
      
    putbyte ( 0 , tfmfile ) ; 
    if ( x >= 16777216L ) 
    x = 16777215L ; 
  } 
  n = x / 65536L ; 
  m = x % 65536L ; 
  putbyte ( n , tfmfile ) ; 
  putbyte ( m / 256 , tfmfile ) ; 
  putbyte ( m % 256 , tfmfile ) ; 
} 
void paramenter ( ) 
{namelength = 5 ; 
  curname [ 16 ] = 83 ; 
  curname [ 17 ] = 76 ; 
  curname [ 18 ] = 65 ; 
  curname [ 19 ] = 78 ; 
  curname [ 20 ] = 84 ; 
  entername ( 21 ) ; 
  namelength = 5 ; 
  curname [ 16 ] = 83 ; 
  curname [ 17 ] = 80 ; 
  curname [ 18 ] = 65 ; 
  curname [ 19 ] = 67 ; 
  curname [ 20 ] = 69 ; 
  entername ( 22 ) ; 
  namelength = 7 ; 
  curname [ 14 ] = 83 ; 
  curname [ 15 ] = 84 ; 
  curname [ 16 ] = 82 ; 
  curname [ 17 ] = 69 ; 
  curname [ 18 ] = 84 ; 
  curname [ 19 ] = 67 ; 
  curname [ 20 ] = 72 ; 
  entername ( 23 ) ; 
  namelength = 6 ; 
  curname [ 15 ] = 83 ; 
  curname [ 16 ] = 72 ; 
  curname [ 17 ] = 82 ; 
  curname [ 18 ] = 73 ; 
  curname [ 19 ] = 78 ; 
  curname [ 20 ] = 75 ; 
  entername ( 24 ) ; 
  namelength = 7 ; 
  curname [ 14 ] = 88 ; 
  curname [ 15 ] = 72 ; 
  curname [ 16 ] = 69 ; 
  curname [ 17 ] = 73 ; 
  curname [ 18 ] = 71 ; 
  curname [ 19 ] = 72 ; 
  curname [ 20 ] = 84 ; 
  entername ( 25 ) ; 
  namelength = 4 ; 
  curname [ 17 ] = 81 ; 
  curname [ 18 ] = 85 ; 
  curname [ 19 ] = 65 ; 
  curname [ 20 ] = 68 ; 
  entername ( 26 ) ; 
  namelength = 10 ; 
  curname [ 11 ] = 69 ; 
  curname [ 12 ] = 88 ; 
  curname [ 13 ] = 84 ; 
  curname [ 14 ] = 82 ; 
  curname [ 15 ] = 65 ; 
  curname [ 16 ] = 83 ; 
  curname [ 17 ] = 80 ; 
  curname [ 18 ] = 65 ; 
  curname [ 19 ] = 67 ; 
  curname [ 20 ] = 69 ; 
  entername ( 27 ) ; 
  namelength = 4 ; 
  curname [ 17 ] = 78 ; 
  curname [ 18 ] = 85 ; 
  curname [ 19 ] = 77 ; 
  curname [ 20 ] = 49 ; 
  entername ( 28 ) ; 
  namelength = 4 ; 
  curname [ 17 ] = 78 ; 
  curname [ 18 ] = 85 ; 
  curname [ 19 ] = 77 ; 
  curname [ 20 ] = 50 ; 
  entername ( 29 ) ; 
  namelength = 4 ; 
  curname [ 17 ] = 78 ; 
  curname [ 18 ] = 85 ; 
  curname [ 19 ] = 77 ; 
  curname [ 20 ] = 51 ; 
  entername ( 30 ) ; 
  namelength = 6 ; 
  curname [ 15 ] = 68 ; 
  curname [ 16 ] = 69 ; 
  curname [ 17 ] = 78 ; 
  curname [ 18 ] = 79 ; 
  curname [ 19 ] = 77 ; 
  curname [ 20 ] = 49 ; 
  entername ( 31 ) ; 
  namelength = 6 ; 
  curname [ 15 ] = 68 ; 
  curname [ 16 ] = 69 ; 
  curname [ 17 ] = 78 ; 
  curname [ 18 ] = 79 ; 
  curname [ 19 ] = 77 ; 
  curname [ 20 ] = 50 ; 
  entername ( 32 ) ; 
  namelength = 4 ; 
  curname [ 17 ] = 83 ; 
  curname [ 18 ] = 85 ; 
  curname [ 19 ] = 80 ; 
  curname [ 20 ] = 49 ; 
  entername ( 33 ) ; 
  namelength = 4 ; 
  curname [ 17 ] = 83 ; 
  curname [ 18 ] = 85 ; 
  curname [ 19 ] = 80 ; 
  curname [ 20 ] = 50 ; 
  entername ( 34 ) ; 
  namelength = 4 ; 
  curname [ 17 ] = 83 ; 
  curname [ 18 ] = 85 ; 
  curname [ 19 ] = 80 ; 
  curname [ 20 ] = 51 ; 
  entername ( 35 ) ; 
  namelength = 4 ; 
  curname [ 17 ] = 83 ; 
  curname [ 18 ] = 85 ; 
  curname [ 19 ] = 66 ; 
  curname [ 20 ] = 49 ; 
  entername ( 36 ) ; 
  namelength = 4 ; 
  curname [ 17 ] = 83 ; 
  curname [ 18 ] = 85 ; 
  curname [ 19 ] = 66 ; 
  curname [ 20 ] = 50 ; 
  entername ( 37 ) ; 
  namelength = 7 ; 
  curname [ 14 ] = 83 ; 
  curname [ 15 ] = 85 ; 
  curname [ 16 ] = 80 ; 
  curname [ 17 ] = 68 ; 
  curname [ 18 ] = 82 ; 
  curname [ 19 ] = 79 ; 
  curname [ 20 ] = 80 ; 
  entername ( 38 ) ; 
  namelength = 7 ; 
  curname [ 14 ] = 83 ; 
  curname [ 15 ] = 85 ; 
  curname [ 16 ] = 66 ; 
  curname [ 17 ] = 68 ; 
  curname [ 18 ] = 82 ; 
  curname [ 19 ] = 79 ; 
  curname [ 20 ] = 80 ; 
  entername ( 39 ) ; 
  namelength = 6 ; 
  curname [ 15 ] = 68 ; 
  curname [ 16 ] = 69 ; 
  curname [ 17 ] = 76 ; 
  curname [ 18 ] = 73 ; 
  curname [ 19 ] = 77 ; 
  curname [ 20 ] = 49 ; 
  entername ( 40 ) ; 
  namelength = 6 ; 
  curname [ 15 ] = 68 ; 
  curname [ 16 ] = 69 ; 
  curname [ 17 ] = 76 ; 
  curname [ 18 ] = 73 ; 
  curname [ 19 ] = 77 ; 
  curname [ 20 ] = 50 ; 
  entername ( 41 ) ; 
  namelength = 10 ; 
  curname [ 11 ] = 65 ; 
  curname [ 12 ] = 88 ; 
  curname [ 13 ] = 73 ; 
  curname [ 14 ] = 83 ; 
  curname [ 15 ] = 72 ; 
  curname [ 16 ] = 69 ; 
  curname [ 17 ] = 73 ; 
  curname [ 18 ] = 71 ; 
  curname [ 19 ] = 72 ; 
  curname [ 20 ] = 84 ; 
  entername ( 42 ) ; 
  namelength = 20 ; 
  curname [ 1 ] = 68 ; 
  curname [ 2 ] = 69 ; 
  curname [ 3 ] = 70 ; 
  curname [ 4 ] = 65 ; 
  curname [ 5 ] = 85 ; 
  curname [ 6 ] = 76 ; 
  curname [ 7 ] = 84 ; 
  curname [ 8 ] = 82 ; 
  curname [ 9 ] = 85 ; 
  curname [ 10 ] = 76 ; 
  curname [ 11 ] = 69 ; 
  curname [ 12 ] = 84 ; 
  curname [ 13 ] = 72 ; 
  curname [ 14 ] = 73 ; 
  curname [ 15 ] = 67 ; 
  curname [ 16 ] = 75 ; 
  curname [ 17 ] = 78 ; 
  curname [ 18 ] = 69 ; 
  curname [ 19 ] = 83 ; 
  curname [ 20 ] = 83 ; 
  entername ( 28 ) ; 
  namelength = 13 ; 
  curname [ 8 ] = 66 ; 
  curname [ 9 ] = 73 ; 
  curname [ 10 ] = 71 ; 
  curname [ 11 ] = 79 ; 
  curname [ 12 ] = 80 ; 
  curname [ 13 ] = 83 ; 
  curname [ 14 ] = 80 ; 
  curname [ 15 ] = 65 ; 
  curname [ 16 ] = 67 ; 
  curname [ 17 ] = 73 ; 
  curname [ 18 ] = 78 ; 
  curname [ 19 ] = 71 ; 
  curname [ 20 ] = 49 ; 
  entername ( 29 ) ; 
  namelength = 13 ; 
  curname [ 8 ] = 66 ; 
  curname [ 9 ] = 73 ; 
  curname [ 10 ] = 71 ; 
  curname [ 11 ] = 79 ; 
  curname [ 12 ] = 80 ; 
  curname [ 13 ] = 83 ; 
  curname [ 14 ] = 80 ; 
  curname [ 15 ] = 65 ; 
  curname [ 16 ] = 67 ; 
  curname [ 17 ] = 73 ; 
  curname [ 18 ] = 78 ; 
  curname [ 19 ] = 71 ; 
  curname [ 20 ] = 50 ; 
  entername ( 30 ) ; 
  namelength = 13 ; 
  curname [ 8 ] = 66 ; 
  curname [ 9 ] = 73 ; 
  curname [ 10 ] = 71 ; 
  curname [ 11 ] = 79 ; 
  curname [ 12 ] = 80 ; 
  curname [ 13 ] = 83 ; 
  curname [ 14 ] = 80 ; 
  curname [ 15 ] = 65 ; 
  curname [ 16 ] = 67 ; 
  curname [ 17 ] = 73 ; 
  curname [ 18 ] = 78 ; 
  curname [ 19 ] = 71 ; 
  curname [ 20 ] = 51 ; 
  entername ( 31 ) ; 
  namelength = 13 ; 
  curname [ 8 ] = 66 ; 
  curname [ 9 ] = 73 ; 
  curname [ 10 ] = 71 ; 
  curname [ 11 ] = 79 ; 
  curname [ 12 ] = 80 ; 
  curname [ 13 ] = 83 ; 
  curname [ 14 ] = 80 ; 
  curname [ 15 ] = 65 ; 
  curname [ 16 ] = 67 ; 
  curname [ 17 ] = 73 ; 
  curname [ 18 ] = 78 ; 
  curname [ 19 ] = 71 ; 
  curname [ 20 ] = 52 ; 
  entername ( 32 ) ; 
  namelength = 13 ; 
  curname [ 8 ] = 66 ; 
  curname [ 9 ] = 73 ; 
  curname [ 10 ] = 71 ; 
  curname [ 11 ] = 79 ; 
  curname [ 12 ] = 80 ; 
  curname [ 13 ] = 83 ; 
  curname [ 14 ] = 80 ; 
  curname [ 15 ] = 65 ; 
  curname [ 16 ] = 67 ; 
  curname [ 17 ] = 73 ; 
  curname [ 18 ] = 78 ; 
  curname [ 19 ] = 71 ; 
  curname [ 20 ] = 53 ; 
  entername ( 33 ) ; 
} 
void nameenter ( ) 
{equiv [ 0 ] = 0 ; 
  namelength = 8 ; 
  curname [ 13 ] = 67 ; 
  curname [ 14 ] = 72 ; 
  curname [ 15 ] = 69 ; 
  curname [ 16 ] = 67 ; 
  curname [ 17 ] = 75 ; 
  curname [ 18 ] = 83 ; 
  curname [ 19 ] = 85 ; 
  curname [ 20 ] = 77 ; 
  entername ( 1 ) ; 
  namelength = 10 ; 
  curname [ 11 ] = 68 ; 
  curname [ 12 ] = 69 ; 
  curname [ 13 ] = 83 ; 
  curname [ 14 ] = 73 ; 
  curname [ 15 ] = 71 ; 
  curname [ 16 ] = 78 ; 
  curname [ 17 ] = 83 ; 
  curname [ 18 ] = 73 ; 
  curname [ 19 ] = 90 ; 
  curname [ 20 ] = 69 ; 
  entername ( 2 ) ; 
  namelength = 11 ; 
  curname [ 10 ] = 68 ; 
  curname [ 11 ] = 69 ; 
  curname [ 12 ] = 83 ; 
  curname [ 13 ] = 73 ; 
  curname [ 14 ] = 71 ; 
  curname [ 15 ] = 78 ; 
  curname [ 16 ] = 85 ; 
  curname [ 17 ] = 78 ; 
  curname [ 18 ] = 73 ; 
  curname [ 19 ] = 84 ; 
  curname [ 20 ] = 83 ; 
  entername ( 3 ) ; 
  namelength = 12 ; 
  curname [ 9 ] = 67 ; 
  curname [ 10 ] = 79 ; 
  curname [ 11 ] = 68 ; 
  curname [ 12 ] = 73 ; 
  curname [ 13 ] = 78 ; 
  curname [ 14 ] = 71 ; 
  curname [ 15 ] = 83 ; 
  curname [ 16 ] = 67 ; 
  curname [ 17 ] = 72 ; 
  curname [ 18 ] = 69 ; 
  curname [ 19 ] = 77 ; 
  curname [ 20 ] = 69 ; 
  entername ( 4 ) ; 
  namelength = 6 ; 
  curname [ 15 ] = 70 ; 
  curname [ 16 ] = 65 ; 
  curname [ 17 ] = 77 ; 
  curname [ 18 ] = 73 ; 
  curname [ 19 ] = 76 ; 
  curname [ 20 ] = 89 ; 
  entername ( 5 ) ; 
  namelength = 4 ; 
  curname [ 17 ] = 70 ; 
  curname [ 18 ] = 65 ; 
  curname [ 19 ] = 67 ; 
  curname [ 20 ] = 69 ; 
  entername ( 6 ) ; 
  namelength = 16 ; 
  curname [ 5 ] = 83 ; 
  curname [ 6 ] = 69 ; 
  curname [ 7 ] = 86 ; 
  curname [ 8 ] = 69 ; 
  curname [ 9 ] = 78 ; 
  curname [ 10 ] = 66 ; 
  curname [ 11 ] = 73 ; 
  curname [ 12 ] = 84 ; 
  curname [ 13 ] = 83 ; 
  curname [ 14 ] = 65 ; 
  curname [ 15 ] = 70 ; 
  curname [ 16 ] = 69 ; 
  curname [ 17 ] = 70 ; 
  curname [ 18 ] = 76 ; 
  curname [ 19 ] = 65 ; 
  curname [ 20 ] = 71 ; 
  entername ( 7 ) ; 
  namelength = 6 ; 
  curname [ 15 ] = 72 ; 
  curname [ 16 ] = 69 ; 
  curname [ 17 ] = 65 ; 
  curname [ 18 ] = 68 ; 
  curname [ 19 ] = 69 ; 
  curname [ 20 ] = 82 ; 
  entername ( 8 ) ; 
  namelength = 9 ; 
  curname [ 12 ] = 70 ; 
  curname [ 13 ] = 79 ; 
  curname [ 14 ] = 78 ; 
  curname [ 15 ] = 84 ; 
  curname [ 16 ] = 68 ; 
  curname [ 17 ] = 73 ; 
  curname [ 18 ] = 77 ; 
  curname [ 19 ] = 69 ; 
  curname [ 20 ] = 78 ; 
  entername ( 9 ) ; 
  namelength = 8 ; 
  curname [ 13 ] = 76 ; 
  curname [ 14 ] = 73 ; 
  curname [ 15 ] = 71 ; 
  curname [ 16 ] = 84 ; 
  curname [ 17 ] = 65 ; 
  curname [ 18 ] = 66 ; 
  curname [ 19 ] = 76 ; 
  curname [ 20 ] = 69 ; 
  entername ( 10 ) ; 
  namelength = 12 ; 
  curname [ 9 ] = 66 ; 
  curname [ 10 ] = 79 ; 
  curname [ 11 ] = 85 ; 
  curname [ 12 ] = 78 ; 
  curname [ 13 ] = 68 ; 
  curname [ 14 ] = 65 ; 
  curname [ 15 ] = 82 ; 
  curname [ 16 ] = 89 ; 
  curname [ 17 ] = 67 ; 
  curname [ 18 ] = 72 ; 
  curname [ 19 ] = 65 ; 
  curname [ 20 ] = 82 ; 
  entername ( 11 ) ; 
  namelength = 9 ; 
  curname [ 12 ] = 67 ; 
  curname [ 13 ] = 72 ; 
  curname [ 14 ] = 65 ; 
  curname [ 15 ] = 82 ; 
  curname [ 16 ] = 65 ; 
  curname [ 17 ] = 67 ; 
  curname [ 18 ] = 84 ; 
  curname [ 19 ] = 69 ; 
  curname [ 20 ] = 82 ; 
  entername ( 12 ) ; 
  namelength = 9 ; 
  curname [ 12 ] = 80 ; 
  curname [ 13 ] = 65 ; 
  curname [ 14 ] = 82 ; 
  curname [ 15 ] = 65 ; 
  curname [ 16 ] = 77 ; 
  curname [ 17 ] = 69 ; 
  curname [ 18 ] = 84 ; 
  curname [ 19 ] = 69 ; 
  curname [ 20 ] = 82 ; 
  entername ( 20 ) ; 
  namelength = 6 ; 
  curname [ 15 ] = 67 ; 
  curname [ 16 ] = 72 ; 
  curname [ 17 ] = 65 ; 
  curname [ 18 ] = 82 ; 
  curname [ 19 ] = 87 ; 
  curname [ 20 ] = 68 ; 
  entername ( 51 ) ; 
  namelength = 6 ; 
  curname [ 15 ] = 67 ; 
  curname [ 16 ] = 72 ; 
  curname [ 17 ] = 65 ; 
  curname [ 18 ] = 82 ; 
  curname [ 19 ] = 72 ; 
  curname [ 20 ] = 84 ; 
  entername ( 52 ) ; 
  namelength = 6 ; 
  curname [ 15 ] = 67 ; 
  curname [ 16 ] = 72 ; 
  curname [ 17 ] = 65 ; 
  curname [ 18 ] = 82 ; 
  curname [ 19 ] = 68 ; 
  curname [ 20 ] = 80 ; 
  entername ( 53 ) ; 
  namelength = 6 ; 
  curname [ 15 ] = 67 ; 
  curname [ 16 ] = 72 ; 
  curname [ 17 ] = 65 ; 
  curname [ 18 ] = 82 ; 
  curname [ 19 ] = 73 ; 
  curname [ 20 ] = 67 ; 
  entername ( 54 ) ; 
  namelength = 10 ; 
  curname [ 11 ] = 78 ; 
  curname [ 12 ] = 69 ; 
  curname [ 13 ] = 88 ; 
  curname [ 14 ] = 84 ; 
  curname [ 15 ] = 76 ; 
  curname [ 16 ] = 65 ; 
  curname [ 17 ] = 82 ; 
  curname [ 18 ] = 71 ; 
  curname [ 19 ] = 69 ; 
  curname [ 20 ] = 82 ; 
  entername ( 55 ) ; 
  namelength = 7 ; 
  curname [ 14 ] = 86 ; 
  curname [ 15 ] = 65 ; 
  curname [ 16 ] = 82 ; 
  curname [ 17 ] = 67 ; 
  curname [ 18 ] = 72 ; 
  curname [ 19 ] = 65 ; 
  curname [ 20 ] = 82 ; 
  entername ( 56 ) ; 
  namelength = 3 ; 
  curname [ 18 ] = 84 ; 
  curname [ 19 ] = 79 ; 
  curname [ 20 ] = 80 ; 
  entername ( 57 ) ; 
  namelength = 3 ; 
  curname [ 18 ] = 77 ; 
  curname [ 19 ] = 73 ; 
  curname [ 20 ] = 68 ; 
  entername ( 58 ) ; 
  namelength = 3 ; 
  curname [ 18 ] = 66 ; 
  curname [ 19 ] = 79 ; 
  curname [ 20 ] = 84 ; 
  entername ( 59 ) ; 
  namelength = 3 ; 
  curname [ 18 ] = 82 ; 
  curname [ 19 ] = 69 ; 
  curname [ 20 ] = 80 ; 
  entername ( 60 ) ; 
  namelength = 3 ; 
  curname [ 18 ] = 69 ; 
  curname [ 19 ] = 88 ; 
  curname [ 20 ] = 84 ; 
  entername ( 60 ) ; 
  namelength = 7 ; 
  curname [ 14 ] = 67 ; 
  curname [ 15 ] = 79 ; 
  curname [ 16 ] = 77 ; 
  curname [ 17 ] = 77 ; 
  curname [ 18 ] = 69 ; 
  curname [ 19 ] = 78 ; 
  curname [ 20 ] = 84 ; 
  entername ( 0 ) ; 
  namelength = 5 ; 
  curname [ 16 ] = 76 ; 
  curname [ 17 ] = 65 ; 
  curname [ 18 ] = 66 ; 
  curname [ 19 ] = 69 ; 
  curname [ 20 ] = 76 ; 
  entername ( 70 ) ; 
  namelength = 4 ; 
  curname [ 17 ] = 83 ; 
  curname [ 18 ] = 84 ; 
  curname [ 19 ] = 79 ; 
  curname [ 20 ] = 80 ; 
  entername ( 71 ) ; 
  namelength = 4 ; 
  curname [ 17 ] = 83 ; 
  curname [ 18 ] = 75 ; 
  curname [ 19 ] = 73 ; 
  curname [ 20 ] = 80 ; 
  entername ( 72 ) ; 
  namelength = 3 ; 
  curname [ 18 ] = 75 ; 
  curname [ 19 ] = 82 ; 
  curname [ 20 ] = 78 ; 
  entername ( 73 ) ; 
  namelength = 3 ; 
  curname [ 18 ] = 76 ; 
  curname [ 19 ] = 73 ; 
  curname [ 20 ] = 71 ; 
  entername ( 74 ) ; 
  namelength = 4 ; 
  curname [ 17 ] = 47 ; 
  curname [ 18 ] = 76 ; 
  curname [ 19 ] = 73 ; 
  curname [ 20 ] = 71 ; 
  entername ( 76 ) ; 
  namelength = 5 ; 
  curname [ 16 ] = 47 ; 
  curname [ 17 ] = 76 ; 
  curname [ 18 ] = 73 ; 
  curname [ 19 ] = 71 ; 
  curname [ 20 ] = 62 ; 
  entername ( 80 ) ; 
  namelength = 4 ; 
  curname [ 17 ] = 76 ; 
  curname [ 18 ] = 73 ; 
  curname [ 19 ] = 71 ; 
  curname [ 20 ] = 47 ; 
  entername ( 75 ) ; 
  namelength = 5 ; 
  curname [ 16 ] = 76 ; 
  curname [ 17 ] = 73 ; 
  curname [ 18 ] = 71 ; 
  curname [ 19 ] = 47 ; 
  curname [ 20 ] = 62 ; 
  entername ( 79 ) ; 
  namelength = 5 ; 
  curname [ 16 ] = 47 ; 
  curname [ 17 ] = 76 ; 
  curname [ 18 ] = 73 ; 
  curname [ 19 ] = 71 ; 
  curname [ 20 ] = 47 ; 
  entername ( 77 ) ; 
  namelength = 6 ; 
  curname [ 15 ] = 47 ; 
  curname [ 16 ] = 76 ; 
  curname [ 17 ] = 73 ; 
  curname [ 18 ] = 71 ; 
  curname [ 19 ] = 47 ; 
  curname [ 20 ] = 62 ; 
  entername ( 81 ) ; 
  namelength = 7 ; 
  curname [ 14 ] = 47 ; 
  curname [ 15 ] = 76 ; 
  curname [ 16 ] = 73 ; 
  curname [ 17 ] = 71 ; 
  curname [ 18 ] = 47 ; 
  curname [ 19 ] = 62 ; 
  curname [ 20 ] = 62 ; 
  entername ( 85 ) ; 
  paramenter () ; 
} 
void readligkern ( ) 
{integer krnptr  ; 
  byte c  ; 
  {
    while ( level == 1 ) {
	
      while ( curchar == 32 ) getnext () ; 
      if ( curchar == 40 ) 
      {
	getname () ; 
	if ( curcode == 0 ) 
	skiptoendofitem () ; 
	else if ( curcode < 70 ) 
	{
	  {
	    if ( charsonline > 0 ) 
	    (void) fprintf(stdout, "%c\n", ' ' ) ; 
	    (void) Fputs(stdout, "This property name doesn't belong in a LIGTABLE list" ) ; 
	    showerrorcontext () ; 
	  } 
	  skiptoendofitem () ; 
	} 
	else {
	    
	  switch ( curcode ) 
	  {case 70 : 
	    {
	      while ( curchar == 32 ) getnext () ; 
	      if ( curchar == 66 ) 
	      {
		charremainder [ 256 ] = nl ; 
		do {
		    getnext () ; 
		} while ( ! ( ( curchar == 40 ) || ( curchar == 41 ) ) ) ; 
	      } 
	      else {
		  
		{
		  if ( ( curchar > 41 ) || ( curchar < 40 ) ) 
		  loc = loc - 1 ; 
		} 
		c = getbyte () ; 
		checktag ( c ) ; 
		chartag [ c ] = 1 ; 
		charremainder [ c ] = nl ; 
	      } 
	      if ( minnl <= nl ) 
	      minnl = nl + 1 ; 
	      lkstepended = false ; 
	    } 
	    break ; 
	  case 71 : 
	    if ( ! lkstepended ) 
	    {
	      if ( charsonline > 0 ) 
	      (void) fprintf(stdout, "%c\n", ' ' ) ; 
	      (void) Fputs(stdout, "STOP must follow LIG or KRN" ) ; 
	      showerrorcontext () ; 
	    } 
	    else {
		
	      ligkern [ nl - 1 ] .b0 = 128 ; 
	      lkstepended = false ; 
	    } 
	    break ; 
	  case 72 : 
	    if ( ! lkstepended ) 
	    {
	      if ( charsonline > 0 ) 
	      (void) fprintf(stdout, "%c\n", ' ' ) ; 
	      (void) Fputs(stdout, "SKIP must follow LIG or KRN" ) ; 
	      showerrorcontext () ; 
	    } 
	    else {
		
	      c = getbyte () ; 
	      if ( c >= 128 ) 
	      {
		if ( charsonline > 0 ) 
		(void) fprintf(stdout, "%c\n", ' ' ) ; 
		(void) Fputs(stdout, "Maximum SKIP amount is 127" ) ; 
		showerrorcontext () ; 
	      } 
	      else if ( nl + c >= maxligsteps ) 
	      {
		if ( charsonline > 0 ) 
		(void) fprintf(stdout, "%c\n", ' ' ) ; 
		(void) Fputs(stdout, "Sorry, LIGTABLE too long for me to handle" ) ; 
		showerrorcontext () ; 
	      } 
	      else {
		  
		ligkern [ nl - 1 ] .b0 = c ; 
		if ( minnl <= nl + c ) 
		minnl = nl + c + 1 ; 
	      } 
	      lkstepended = false ; 
	    } 
	    break ; 
	  case 73 : 
	    {
	      ligkern [ nl ] .b0 = 0 ; 
	      ligkern [ nl ] .b1 = getbyte () ; 
	      kern [ nk ] = getfix () ; 
	      krnptr = 0 ; 
	      while ( kern [ krnptr ] != kern [ nk ] ) krnptr = krnptr + 1 ; 
	      if ( krnptr == nk ) 
	      {
		if ( nk < maxkerns ) 
		nk = nk + 1 ; 
		else {
		    
		  {
		    if ( charsonline > 0 ) 
		    (void) fprintf(stdout, "%c\n", ' ' ) ; 
		    (void) Fputs(stdout, "Sorry, too many different kerns for me to handle"                     ) ; 
		    showerrorcontext () ; 
		  } 
		  krnptr = krnptr - 1 ; 
		} 
	      } 
	      ligkern [ nl ] .b2 = 128 + ( krnptr / 256 ) ; 
	      ligkern [ nl ] .b3 = krnptr % 256 ; 
	      if ( nl >= maxligsteps - 1 ) 
	      {
		if ( charsonline > 0 ) 
		(void) fprintf(stdout, "%c\n", ' ' ) ; 
		(void) Fputs(stdout, "Sorry, LIGTABLE too long for me to handle" ) ; 
		showerrorcontext () ; 
	      } 
	      else nl = nl + 1 ; 
	      lkstepended = true ; 
	    } 
	    break ; 
	  case 74 : 
	  case 75 : 
	  case 76 : 
	  case 77 : 
	  case 79 : 
	  case 80 : 
	  case 81 : 
	  case 85 : 
	    {
	      ligkern [ nl ] .b0 = 0 ; 
	      ligkern [ nl ] .b2 = curcode - 74 ; 
	      ligkern [ nl ] .b1 = getbyte () ; 
	      ligkern [ nl ] .b3 = getbyte () ; 
	      if ( nl >= maxligsteps - 1 ) 
	      {
		if ( charsonline > 0 ) 
		(void) fprintf(stdout, "%c\n", ' ' ) ; 
		(void) Fputs(stdout, "Sorry, LIGTABLE too long for me to handle" ) ; 
		showerrorcontext () ; 
	      } 
	      else nl = nl + 1 ; 
	      lkstepended = true ; 
	    } 
	    break ; 
	  } 
	  finishtheproperty () ; 
	} 
      } 
      else if ( curchar == 41 ) 
      skiptoendofitem () ; 
      else junkerror () ; 
    } 
    {
      loc = loc - 1 ; 
      level = level + 1 ; 
      curchar = 41 ; 
    } 
  } 
} 
void readcharinfo ( ) 
{byte c  ; 
  {
    c = getbyte () ; 
    if ( verbose ) 
    {
      if ( charsonline == 8 ) 
      {
	(void) fprintf(stdout, "%c\n", ' ' ) ; 
	charsonline = 1 ; 
      } 
      else {
	  
	if ( charsonline > 0 ) 
	(void) putc(' ' , stdout);
	charsonline = charsonline + 1 ; 
      } 
      printoctal ( c ) ; 
    } 
    while ( level == 1 ) {
	
      while ( curchar == 32 ) getnext () ; 
      if ( curchar == 40 ) 
      {
	getname () ; 
	if ( curcode == 0 ) 
	skiptoendofitem () ; 
	else if ( ( curcode < 51 ) || ( curcode > 56 ) ) 
	{
	  {
	    if ( charsonline > 0 ) 
	    (void) fprintf(stdout, "%c\n", ' ' ) ; 
	    (void) Fputs(stdout, "This property name doesn't belong in a CHARACTER list" ) 
	    ; 
	    showerrorcontext () ; 
	  } 
	  skiptoendofitem () ; 
	} 
	else {
	    
	  switch ( curcode ) 
	  {case 51 : 
	    charwd [ c ] = sortin ( 1 , getfix () ) ; 
	    break ; 
	  case 52 : 
	    charht [ c ] = sortin ( 2 , getfix () ) ; 
	    break ; 
	  case 53 : 
	    chardp [ c ] = sortin ( 3 , getfix () ) ; 
	    break ; 
	  case 54 : 
	    charic [ c ] = sortin ( 4 , getfix () ) ; 
	    break ; 
	  case 55 : 
	    {
	      checktag ( c ) ; 
	      chartag [ c ] = 2 ; 
	      charremainder [ c ] = getbyte () ; 
	    } 
	    break ; 
	  case 56 : 
	    {
	      if ( ne == 256 ) 
	      {
		if ( charsonline > 0 ) 
		(void) fprintf(stdout, "%c\n", ' ' ) ; 
		(void) Fputs(stdout, "At most 256 VARCHAR specs are allowed" ) ; 
		showerrorcontext () ; 
	      } 
	      else {
		  
		checktag ( c ) ; 
		chartag [ c ] = 3 ; 
		charremainder [ c ] = ne ; 
		exten [ ne ] .b0 = 0 ; 
		exten [ ne ] .b1 = 0 ; 
		exten [ ne ] .b2 = 0 ; 
		exten [ ne ] .b3 = 0 ; 
		while ( level == 2 ) {
		    
		  while ( curchar == 32 ) getnext () ; 
		  if ( curchar == 40 ) 
		  {
		    getname () ; 
		    if ( curcode == 0 ) 
		    skiptoendofitem () ; 
		    else if ( ( curcode < 57 ) || ( curcode > 60 ) ) 
		    {
		      {
			if ( charsonline > 0 ) 
			(void) fprintf(stdout, "%c\n", ' ' ) ; 
			(void) Fputs(stdout, "This property name doesn't belong in a VARCHAR list"                         ) ; 
			showerrorcontext () ; 
		      } 
		      skiptoendofitem () ; 
		    } 
		    else {
			
		      switch ( curcode - ( 57 ) ) 
		      {case 0 : 
			exten [ ne ] .b0 = getbyte () ; 
			break ; 
		      case 1 : 
			exten [ ne ] .b1 = getbyte () ; 
			break ; 
		      case 2 : 
			exten [ ne ] .b2 = getbyte () ; 
			break ; 
		      case 3 : 
			exten [ ne ] .b3 = getbyte () ; 
			break ; 
		      } 
		      finishtheproperty () ; 
		    } 
		  } 
		  else if ( curchar == 41 ) 
		  skiptoendofitem () ; 
		  else junkerror () ; 
		} 
		ne = ne + 1 ; 
		{
		  loc = loc - 1 ; 
		  level = level + 1 ; 
		  curchar = 41 ; 
		} 
	      } 
	    } 
	    break ; 
	  } 
	  finishtheproperty () ; 
	} 
      } 
      else if ( curchar == 41 ) 
      skiptoendofitem () ; 
      else junkerror () ; 
    } 
    if ( charwd [ c ] == 0 ) 
    charwd [ c ] = sortin ( 1 , 0 ) ; 
    {
      loc = loc - 1 ; 
      level = level + 1 ; 
      curchar = 41 ; 
    } 
  } 
} 
void readinput ( ) 
{byte c  ; 
  curchar = 32 ; 
  do {
      while ( curchar == 32 ) getnext () ; 
    if ( curchar == 40 ) 
    {
      getname () ; 
      if ( curcode == 0 ) 
      skiptoendofitem () ; 
      else if ( curcode > 12 ) 
      {
	{
	  if ( charsonline > 0 ) 
	  (void) fprintf(stdout, "%c\n", ' ' ) ; 
	  (void) Fputs(stdout, "This property name doesn't belong on the outer level" ) ; 
	  showerrorcontext () ; 
	} 
	skiptoendofitem () ; 
      } 
      else {
	  
	switch ( curcode ) 
	{case 1 : 
	  {
	    checksumspecified = true ; 
	    readfourbytes ( 0 ) ; 
	  } 
	  break ; 
	case 2 : 
	  {
	    nextd = getfix () ; 
	    if ( nextd < 1048576L ) 
	    {
	      if ( charsonline > 0 ) 
	      (void) fprintf(stdout, "%c\n", ' ' ) ; 
	      (void) Fputs(stdout, "The design size must be at least 1" ) ; 
	      showerrorcontext () ; 
	    } 
	    else designsize = nextd ; 
	  } 
	  break ; 
	case 3 : 
	  {
	    nextd = getfix () ; 
	    if ( nextd <= 0 ) 
	    {
	      if ( charsonline > 0 ) 
	      (void) fprintf(stdout, "%c\n", ' ' ) ; 
	      (void) Fputs(stdout, "The number of units per design size must be positive" ) 
	      ; 
	      showerrorcontext () ; 
	    } 
	    else designunits = nextd ; 
	  } 
	  break ; 
	case 4 : 
	  readBCPL ( 8 , 40 ) ; 
	  break ; 
	case 5 : 
	  readBCPL ( 48 , 20 ) ; 
	  break ; 
	case 6 : 
	  headerbytes [ 71 ] = getbyte () ; 
	  break ; 
	case 7 : 
	  {
	    while ( curchar == 32 ) getnext () ; 
	    if ( curchar == 84 ) 
	    sevenbitsafeflag = true ; 
	    else if ( curchar == 70 ) 
	    sevenbitsafeflag = false ; 
	    else {
		
	      if ( charsonline > 0 ) 
	      (void) fprintf(stdout, "%c\n", ' ' ) ; 
	      (void) Fputs(stdout, "The flag value should be \"TRUE\" or \"FALSE\"" ) ; 
	      showerrorcontext () ; 
	    } 
	    do {
		getnext () ; 
	    } while ( ! ( ( curchar == 40 ) || ( curchar == 41 ) ) ) ; 
	  } 
	  break ; 
	case 8 : 
	  {
	    c = getbyte () ; 
	    if ( c < 18 ) 
	    {
	      {
		if ( charsonline > 0 ) 
		(void) fprintf(stdout, "%c\n", ' ' ) ; 
		(void) Fputs(stdout, "HEADER indices should be 18 or more" ) ; 
		showerrorcontext () ; 
	      } 
	      do {
		  getnext () ; 
	      } while ( ! ( ( curchar == 40 ) || ( curchar == 41 ) ) ) ; 
	    } 
	    else if ( 4 * c + 4 > maxheaderbytes ) 
	    {
	      {
		if ( charsonline > 0 ) 
		(void) fprintf(stdout, "%c\n", ' ' ) ; 
		(void) Fputs(stdout, "This HEADER index is too big for my present table size" ) ; 
		showerrorcontext () ; 
	      } 
	      do {
		  getnext () ; 
	      } while ( ! ( ( curchar == 40 ) || ( curchar == 41 ) ) ) ; 
	    } 
	    else {
		
	      while ( headerptr < 4 * c + 4 ) {
		  
		headerbytes [ headerptr ] = 0 ; 
		headerptr = headerptr + 1 ; 
	      } 
	      readfourbytes ( 4 * c ) ; 
	    } 
	  } 
	  break ; 
	case 9 : 
	  {
	    while ( level == 1 ) {
		
	      while ( curchar == 32 ) getnext () ; 
	      if ( curchar == 40 ) 
	      {
		getname () ; 
		if ( curcode == 0 ) 
		skiptoendofitem () ; 
		else if ( ( curcode < 20 ) || ( curcode >= 51 ) ) 
		{
		  {
		    if ( charsonline > 0 ) 
		    (void) fprintf(stdout, "%c\n", ' ' ) ; 
		    (void) Fputs(stdout, "This property name doesn't belong in a FONTDIMEN list" ) 
		    ; 
		    showerrorcontext () ; 
		  } 
		  skiptoendofitem () ; 
		} 
		else {
		    
		  if ( curcode == 20 ) 
		  c = getbyte () ; 
		  else c = curcode - 20 ; 
		  if ( c == 0 ) 
		  {
		    {
		      if ( charsonline > 0 ) 
		      (void) fprintf(stdout, "%c\n", ' ' ) ; 
		      (void) Fputs(stdout, "PARAMETER index must not be zero" ) ; 
		      showerrorcontext () ; 
		    } 
		    skiptoendofitem () ; 
		  } 
		  else if ( c > maxparamwords ) 
		  {
		    {
		      if ( charsonline > 0 ) 
		      (void) fprintf(stdout, "%c\n", ' ' ) ; 
		      (void) Fputs(stdout, "This PARAMETER index is too big for my present table size"                       ) ; 
		      showerrorcontext () ; 
		    } 
		    skiptoendofitem () ; 
		  } 
		  else {
		      
		    while ( np < c ) {
			
		      np = np + 1 ; 
		      param [ np ] = 0 ; 
		    } 
		    param [ c ] = getfix () ; 
		    finishtheproperty () ; 
		  } 
		} 
	      } 
	      else if ( curchar == 41 ) 
	      skiptoendofitem () ; 
	      else junkerror () ; 
	    } 
	    {
	      loc = loc - 1 ; 
	      level = level + 1 ; 
	      curchar = 41 ; 
	    } 
	  } 
	  break ; 
	case 10 : 
	  readligkern () ; 
	  break ; 
	case 11 : 
	  bchar = getbyte () ; 
	  break ; 
	case 12 : 
	  readcharinfo () ; 
	  break ; 
	} 
	finishtheproperty () ; 
      } 
    } 
    else if ( ( curchar == 41 ) && ! inputhasended ) 
    {
      {
	if ( charsonline > 0 ) 
	(void) fprintf(stdout, "%c\n", ' ' ) ; 
	(void) Fputs(stdout, "Extra right parenthesis" ) ; 
	showerrorcontext () ; 
      } 
      loc = loc + 1 ; 
      curchar = 32 ; 
    } 
    else if ( ! inputhasended ) 
    junkerror () ; 
  } while ( ! ( inputhasended ) ) ; 
} 
void corrandcheck ( ) 
{short c  ; 
  integer hh  ; 
  integer ligptr  ; 
  byte g  ; 
  if ( nl > 0 ) 
  {
    if ( charremainder [ 256 ] < 32767 ) 
    {
      ligkern [ nl ] .b0 = 255 ; 
      ligkern [ nl ] .b1 = 0 ; 
      ligkern [ nl ] .b2 = 0 ; 
      ligkern [ nl ] .b3 = 0 ; 
      nl = nl + 1 ; 
    } 
    while ( minnl > nl ) {
	
      ligkern [ nl ] .b0 = 255 ; 
      ligkern [ nl ] .b1 = 0 ; 
      ligkern [ nl ] .b2 = 0 ; 
      ligkern [ nl ] .b3 = 0 ; 
      nl = nl + 1 ; 
    } 
    if ( ligkern [ nl - 1 ] .b0 == 0 ) 
    ligkern [ nl - 1 ] .b0 = 128 ; 
  } 
  sevenunsafe = false ; 
  {register integer for_end; c = 0 ; for_end = 255 ; if ( c <= for_end) do 
    if ( charwd [ c ] != 0 ) 
    switch ( chartag [ c ] ) 
    {case 0 : 
      ; 
      break ; 
    case 1 : 
      {
	ligptr = charremainder [ c ] ; 
	do {
	    if ( hashinput ( ligptr , c ) ) 
	  {
	    if ( ligkern [ ligptr ] .b2 < 128 ) 
	    {
	      if ( ligkern [ ligptr ] .b1 != bchar ) 
	      {
		g = ligkern [ ligptr ] .b1 ; 
		if ( charwd [ g ] == 0 ) 
		{
		  charwd [ g ] = sortin ( 1 , 0 ) ; 
		  (void) fprintf(stdout, "%s%c", "LIG character examined by" , ' ' ) ; 
		  printoctal ( c ) ; 
		  (void) fprintf(stdout, "%s\n", " had no CHARACTER spec." ) ; 
		} 
	      } 
	      {
		g = ligkern [ ligptr ] .b3 ; 
		if ( charwd [ g ] == 0 ) 
		{
		  charwd [ g ] = sortin ( 1 , 0 ) ; 
		  (void) fprintf(stdout, "%s%c", "LIG character generated by" , ' ' ) ; 
		  printoctal ( c ) ; 
		  (void) fprintf(stdout, "%s\n", " had no CHARACTER spec." ) ; 
		} 
	      } 
	      if ( ligkern [ ligptr ] .b3 >= 128 ) 
	      if ( ( c < 128 ) || ( c == 256 ) ) 
	      if ( ( ligkern [ ligptr ] .b1 < 128 ) || ( ligkern [ ligptr ] 
	      .b1 == bchar ) ) 
	      sevenunsafe = true ; 
	    } 
	    else if ( ligkern [ ligptr ] .b1 != bchar ) 
	    {
	      g = ligkern [ ligptr ] .b1 ; 
	      if ( charwd [ g ] == 0 ) 
	      {
		charwd [ g ] = sortin ( 1 , 0 ) ; 
		(void) fprintf(stdout, "%s%c", "KRN character examined by" , ' ' ) ; 
		printoctal ( c ) ; 
		(void) fprintf(stdout, "%s\n", " had no CHARACTER spec." ) ; 
	      } 
	    } 
	  } 
	  if ( ligkern [ ligptr ] .b0 >= 128 ) 
	  ligptr = nl ; 
	  else ligptr = ligptr + 1 + ligkern [ ligptr ] .b0 ; 
	} while ( ! ( ligptr >= nl ) ) ; 
      } 
      break ; 
    case 2 : 
      {
	g = charremainder [ c ] ; 
	if ( ( g >= 128 ) && ( c < 128 ) ) 
	sevenunsafe = true ; 
	if ( charwd [ g ] == 0 ) 
	{
	  charwd [ g ] = sortin ( 1 , 0 ) ; 
	  (void) fprintf(stdout, "%s%c", "The character NEXTLARGER than" , ' ' ) ; 
	  printoctal ( c ) ; 
	  (void) fprintf(stdout, "%s\n", " had no CHARACTER spec." ) ; 
	} 
      } 
      break ; 
    case 3 : 
      {
	if ( exten [ charremainder [ c ] ] .b0 > 0 ) 
	{
	  g = exten [ charremainder [ c ] ] .b0 ; 
	  if ( ( g >= 128 ) && ( c < 128 ) ) 
	  sevenunsafe = true ; 
	  if ( charwd [ g ] == 0 ) 
	  {
	    charwd [ g ] = sortin ( 1 , 0 ) ; 
	    (void) fprintf(stdout, "%s%c", "TOP piece of character" , ' ' ) ; 
	    printoctal ( c ) ; 
	    (void) fprintf(stdout, "%s\n", " had no CHARACTER spec." ) ; 
	  } 
	} 
	if ( exten [ charremainder [ c ] ] .b1 > 0 ) 
	{
	  g = exten [ charremainder [ c ] ] .b1 ; 
	  if ( ( g >= 128 ) && ( c < 128 ) ) 
	  sevenunsafe = true ; 
	  if ( charwd [ g ] == 0 ) 
	  {
	    charwd [ g ] = sortin ( 1 , 0 ) ; 
	    (void) fprintf(stdout, "%s%c", "MID piece of character" , ' ' ) ; 
	    printoctal ( c ) ; 
	    (void) fprintf(stdout, "%s\n", " had no CHARACTER spec." ) ; 
	  } 
	} 
	if ( exten [ charremainder [ c ] ] .b2 > 0 ) 
	{
	  g = exten [ charremainder [ c ] ] .b2 ; 
	  if ( ( g >= 128 ) && ( c < 128 ) ) 
	  sevenunsafe = true ; 
	  if ( charwd [ g ] == 0 ) 
	  {
	    charwd [ g ] = sortin ( 1 , 0 ) ; 
	    (void) fprintf(stdout, "%s%c", "BOT piece of character" , ' ' ) ; 
	    printoctal ( c ) ; 
	    (void) fprintf(stdout, "%s\n", " had no CHARACTER spec." ) ; 
	  } 
	} 
	{
	  g = exten [ charremainder [ c ] ] .b3 ; 
	  if ( ( g >= 128 ) && ( c < 128 ) ) 
	  sevenunsafe = true ; 
	  if ( charwd [ g ] == 0 ) 
	  {
	    charwd [ g ] = sortin ( 1 , 0 ) ; 
	    (void) fprintf(stdout, "%s%c", "REP piece of character" , ' ' ) ; 
	    printoctal ( c ) ; 
	    (void) fprintf(stdout, "%s\n", " had no CHARACTER spec." ) ; 
	  } 
	} 
      } 
      break ; 
    } 
  while ( c++ < for_end ) ; } 
  if ( charremainder [ 256 ] < 32767 ) 
  {
    c = 256 ; 
    {
      ligptr = charremainder [ c ] ; 
      do {
	  if ( hashinput ( ligptr , c ) ) 
	{
	  if ( ligkern [ ligptr ] .b2 < 128 ) 
	  {
	    if ( ligkern [ ligptr ] .b1 != bchar ) 
	    {
	      g = ligkern [ ligptr ] .b1 ; 
	      if ( charwd [ g ] == 0 ) 
	      {
		charwd [ g ] = sortin ( 1 , 0 ) ; 
		(void) fprintf(stdout, "%s%c", "LIG character examined by" , ' ' ) ; 
		printoctal ( c ) ; 
		(void) fprintf(stdout, "%s\n", " had no CHARACTER spec." ) ; 
	      } 
	    } 
	    {
	      g = ligkern [ ligptr ] .b3 ; 
	      if ( charwd [ g ] == 0 ) 
	      {
		charwd [ g ] = sortin ( 1 , 0 ) ; 
		(void) fprintf(stdout, "%s%c", "LIG character generated by" , ' ' ) ; 
		printoctal ( c ) ; 
		(void) fprintf(stdout, "%s\n", " had no CHARACTER spec." ) ; 
	      } 
	    } 
	    if ( ligkern [ ligptr ] .b3 >= 128 ) 
	    if ( ( c < 128 ) || ( c == 256 ) ) 
	    if ( ( ligkern [ ligptr ] .b1 < 128 ) || ( ligkern [ ligptr ] .b1 
	    == bchar ) ) 
	    sevenunsafe = true ; 
	  } 
	  else if ( ligkern [ ligptr ] .b1 != bchar ) 
	  {
	    g = ligkern [ ligptr ] .b1 ; 
	    if ( charwd [ g ] == 0 ) 
	    {
	      charwd [ g ] = sortin ( 1 , 0 ) ; 
	      (void) fprintf(stdout, "%s%c", "KRN character examined by" , ' ' ) ; 
	      printoctal ( c ) ; 
	      (void) fprintf(stdout, "%s\n", " had no CHARACTER spec." ) ; 
	    } 
	  } 
	} 
	if ( ligkern [ ligptr ] .b0 >= 128 ) 
	ligptr = nl ; 
	else ligptr = ligptr + 1 + ligkern [ ligptr ] .b0 ; 
      } while ( ! ( ligptr >= nl ) ) ; 
    } 
  } 
  if ( sevenbitsafeflag && sevenunsafe ) 
  (void) fprintf(stdout, "%s\n", "The font is not really seven-bit-safe!" ) ; 
  if ( hashptr < hashsize ) 
  {register integer for_end; hh = 1 ; for_end = hashptr ; if ( hh <= for_end) 
  do 
    {
      tt = hashlist [ hh ] ; 
      if ( class [ tt ] > 0 ) 
      tt = f ( tt , ( hash [ tt ] - 1 ) / 256 , ( hash [ tt ] - 1 ) % 256 ) ; 
    } 
  while ( hh++ < for_end ) ; } 
  if ( ( hashptr == hashsize ) || ( yligcycle < 256 ) ) 
  {
    if ( hashptr < hashsize ) 
    {
      (void) Fputs(stdout, "Infinite ligature loop starting with " ) ; 
      if ( xligcycle == 256 ) 
      (void) Fputs(stdout, "boundary" ) ; 
      else printoctal ( xligcycle ) ; 
      (void) Fputs(stdout, " and " ) ; 
      printoctal ( yligcycle ) ; 
      (void) fprintf(stdout, "%c\n", '!' ) ; 
    } 
    else
    (void) fprintf(stdout, "%s\n", "Sorry, I haven't room for so many ligature/kern pairs!" ) 
    ; 
    (void) fprintf(stdout, "%s\n", "All ligatures will be cleared." ) ; 
    {register integer for_end; c = 0 ; for_end = 255 ; if ( c <= for_end) do 
      if ( chartag [ c ] == 1 ) 
      {
	chartag [ c ] = 0 ; 
	charremainder [ c ] = 0 ; 
      } 
    while ( c++ < for_end ) ; } 
    nl = 0 ; 
    bchar = 256 ; 
    charremainder [ 256 ] = 32767 ; 
  } 
  if ( nl > 0 ) 
  {register integer for_end; ligptr = 0 ; for_end = nl - 1 ; if ( ligptr <= 
  for_end) do 
    if ( ligkern [ ligptr ] .b2 < 128 ) 
    {
      if ( ligkern [ ligptr ] .b0 < 255 ) 
      {
	{
	  c = ligkern [ ligptr ] .b1 ; 
	  if ( charwd [ c ] == 0 ) 
	  if ( c != bchar ) 
	  {
	    ligkern [ ligptr ] .b1 = 0 ; 
	    if ( charwd [ 0 ] == 0 ) 
	    charwd [ 0 ] = sortin ( 1 , 0 ) ; 
	    (void) fprintf(stdout, "%s%s%s", "Unused " , "LIG step" ,             " refers to nonexistent character " ) ; 
	    printoctal ( c ) ; 
	    (void) fprintf(stdout, "%c\n", '!' ) ; 
	  } 
	} 
	{
	  c = ligkern [ ligptr ] .b3 ; 
	  if ( charwd [ c ] == 0 ) 
	  if ( c != bchar ) 
	  {
	    ligkern [ ligptr ] .b3 = 0 ; 
	    if ( charwd [ 0 ] == 0 ) 
	    charwd [ 0 ] = sortin ( 1 , 0 ) ; 
	    (void) fprintf(stdout, "%s%s%s", "Unused " , "LIG step" ,             " refers to nonexistent character " ) ; 
	    printoctal ( c ) ; 
	    (void) fprintf(stdout, "%c\n", '!' ) ; 
	  } 
	} 
      } 
    } 
    else {
	
      c = ligkern [ ligptr ] .b1 ; 
      if ( charwd [ c ] == 0 ) 
      if ( c != bchar ) 
      {
	ligkern [ ligptr ] .b1 = 0 ; 
	if ( charwd [ 0 ] == 0 ) 
	charwd [ 0 ] = sortin ( 1 , 0 ) ; 
	(void) fprintf(stdout, "%s%s%s", "Unused " , "KRN step" , " refers to nonexistent character " ) 
	; 
	printoctal ( c ) ; 
	(void) fprintf(stdout, "%c\n", '!' ) ; 
      } 
    } 
  while ( ligptr++ < for_end ) ; } 
  if ( ne > 0 ) 
  {register integer for_end; g = 0 ; for_end = ne - 1 ; if ( g <= for_end) do 
    {
      {
	c = exten [ g ] .b0 ; 
	if ( c > 0 ) 
	if ( charwd [ c ] == 0 ) 
	{
	  exten [ g ] .b0 = 0 ; 
	  if ( charwd [ 0 ] == 0 ) 
	  charwd [ 0 ] = sortin ( 1 , 0 ) ; 
	  (void) fprintf(stdout, "%s%s%s", "Unused " , "VARCHAR TOP" ,           " refers to nonexistent character " ) ; 
	  printoctal ( c ) ; 
	  (void) fprintf(stdout, "%c\n", '!' ) ; 
	} 
      } 
      {
	c = exten [ g ] .b1 ; 
	if ( c > 0 ) 
	if ( charwd [ c ] == 0 ) 
	{
	  exten [ g ] .b1 = 0 ; 
	  if ( charwd [ 0 ] == 0 ) 
	  charwd [ 0 ] = sortin ( 1 , 0 ) ; 
	  (void) fprintf(stdout, "%s%s%s", "Unused " , "VARCHAR MID" ,           " refers to nonexistent character " ) ; 
	  printoctal ( c ) ; 
	  (void) fprintf(stdout, "%c\n", '!' ) ; 
	} 
      } 
      {
	c = exten [ g ] .b2 ; 
	if ( c > 0 ) 
	if ( charwd [ c ] == 0 ) 
	{
	  exten [ g ] .b2 = 0 ; 
	  if ( charwd [ 0 ] == 0 ) 
	  charwd [ 0 ] = sortin ( 1 , 0 ) ; 
	  (void) fprintf(stdout, "%s%s%s", "Unused " , "VARCHAR BOT" ,           " refers to nonexistent character " ) ; 
	  printoctal ( c ) ; 
	  (void) fprintf(stdout, "%c\n", '!' ) ; 
	} 
      } 
      {
	c = exten [ g ] .b3 ; 
	if ( charwd [ c ] == 0 ) 
	{
	  exten [ g ] .b3 = 0 ; 
	  if ( charwd [ 0 ] == 0 ) 
	  charwd [ 0 ] = sortin ( 1 , 0 ) ; 
	  (void) fprintf(stdout, "%s%s%s", "Unused " , "VARCHAR REP" ,           " refers to nonexistent character " ) ; 
	  printoctal ( c ) ; 
	  (void) fprintf(stdout, "%c\n", '!' ) ; 
	} 
      } 
    } 
  while ( g++ < for_end ) ; } 
  {register integer for_end; c = 0 ; for_end = 255 ; if ( c <= for_end) do 
    if ( chartag [ c ] == 2 ) 
    {
      g = charremainder [ c ] ; 
      while ( ( g < c ) && ( chartag [ g ] == 2 ) ) g = charremainder [ g ] ; 
      if ( g == c ) 
      {
	chartag [ c ] = 0 ; 
	(void) Fputs(stdout, "A cycle of NEXTLARGER characters has been broken at " ) ; 
	printoctal ( c ) ; 
	(void) fprintf(stdout, "%c\n", '.' ) ; 
      } 
    } 
  while ( c++ < for_end ) ; } 
  delta = shorten ( 1 , 255 ) ; 
  setindices ( 1 , delta ) ; 
  if ( delta > 0 ) 
  {
    (void) fprintf(stdout, "%s%s%s", "I had to round some " , "width" , "s by " ) ; 
    printreal ( ( ( ( delta + 1 ) / 2 ) / ((double) 1048576L ) ) , 1 , 7 ) ; 
    (void) fprintf(stdout, "%s\n", " units." ) ; 
  } 
  delta = shorten ( 2 , 15 ) ; 
  setindices ( 2 , delta ) ; 
  if ( delta > 0 ) 
  {
    (void) fprintf(stdout, "%s%s%s", "I had to round some " , "height" , "s by " ) ; 
    printreal ( ( ( ( delta + 1 ) / 2 ) / ((double) 1048576L ) ) , 1 , 7 ) ; 
    (void) fprintf(stdout, "%s\n", " units." ) ; 
  } 
  delta = shorten ( 3 , 15 ) ; 
  setindices ( 3 , delta ) ; 
  if ( delta > 0 ) 
  {
    (void) fprintf(stdout, "%s%s%s", "I had to round some " , "depth" , "s by " ) ; 
    printreal ( ( ( ( delta + 1 ) / 2 ) / ((double) 1048576L ) ) , 1 , 7 ) ; 
    (void) fprintf(stdout, "%s\n", " units." ) ; 
  } 
  delta = shorten ( 4 , 63 ) ; 
  setindices ( 4 , delta ) ; 
  if ( delta > 0 ) 
  {
    (void) fprintf(stdout, "%s%s%s", "I had to round some " , "italic correction" , "s by " ) ; 
    printreal ( ( ( ( delta + 1 ) / 2 ) / ((double) 1048576L ) ) , 1 , 7 ) ; 
    (void) fprintf(stdout, "%s\n", " units." ) ; 
  } 
} 
void main_body() {
    
  initialize () ; 
  nameenter () ; 
  readinput () ; 
  if ( verbose ) 
  (void) fprintf(stdout, "%c\n", '.' ) ; 
  corrandcheck () ; 
  lh = headerptr / 4 ; 
  notfound = true ; 
  bc = 0 ; 
  while ( notfound ) if ( ( charwd [ bc ] > 0 ) || ( bc == 255 ) ) 
  notfound = false ; 
  else bc = bc + 1 ; 
  notfound = true ; 
  ec = 255 ; 
  while ( notfound ) if ( ( charwd [ ec ] > 0 ) || ( ec == 0 ) ) 
  notfound = false ; 
  else ec = ec - 1 ; 
  if ( bc > ec ) 
  bc = 1 ; 
  memory [ 1 ] = memory [ 1 ] + 1 ; 
  memory [ 2 ] = memory [ 2 ] + 1 ; 
  memory [ 3 ] = memory [ 3 ] + 1 ; 
  memory [ 4 ] = memory [ 4 ] + 1 ; 
  labelptr = 0 ; 
  labeltable [ 0 ] .rr = -1 ; 
  {register integer for_end; c = bc ; for_end = ec ; if ( c <= for_end) do 
    if ( chartag [ c ] == 1 ) 
    {
      sortptr = labelptr ; 
      while ( labeltable [ sortptr ] .rr > toint ( charremainder [ c ] ) ) {
	  
	labeltable [ sortptr + 1 ] = labeltable [ sortptr ] ; 
	sortptr = sortptr - 1 ; 
      } 
      labeltable [ sortptr + 1 ] .cc = c ; 
      labeltable [ sortptr + 1 ] .rr = charremainder [ c ] ; 
      labelptr = labelptr + 1 ; 
    } 
  while ( c++ < for_end ) ; } 
  if ( bchar < 256 ) 
  {
    extralocneeded = true ; 
    lkoffset = 1 ; 
  } 
  else {
      
    extralocneeded = false ; 
    lkoffset = 0 ; 
  } 
  {
    sortptr = labelptr ; 
    if ( labeltable [ sortptr ] .rr + lkoffset > 255 ) 
    {
      lkoffset = 0 ; 
      extralocneeded = false ; 
      do {
	  charremainder [ labeltable [ sortptr ] .cc ] = lkoffset ; 
	while ( labeltable [ sortptr - 1 ] .rr == labeltable [ sortptr ] .rr ) 
	{
	  sortptr = sortptr - 1 ; 
	  charremainder [ labeltable [ sortptr ] .cc ] = lkoffset ; 
	} 
	lkoffset = lkoffset + 1 ; 
	sortptr = sortptr - 1 ; 
      } while ( ! ( lkoffset + labeltable [ sortptr ] .rr < 256 ) ) ; 
    } 
    if ( lkoffset > 0 ) 
    while ( sortptr > 0 ) {
	
      charremainder [ labeltable [ sortptr ] .cc ] = charremainder [ 
      labeltable [ sortptr ] .cc ] + lkoffset ; 
      sortptr = sortptr - 1 ; 
    } 
  } 
  if ( charremainder [ 256 ] < 32767 ) 
  {
    ligkern [ nl - 1 ] .b2 = ( charremainder [ 256 ] + lkoffset ) / 256 ; 
    ligkern [ nl - 1 ] .b3 = ( charremainder [ 256 ] + lkoffset ) % 256 ; 
  } 
  lf = 6 + lh + ( ec - bc + 1 ) + memory [ 1 ] + memory [ 2 ] + memory [ 3 ] + 
  memory [ 4 ] + nl + lkoffset + nk + ne + np ; 
  putbyte ( ( lf ) / 256 , tfmfile ) ; 
  putbyte ( ( lf ) % 256 , tfmfile ) ; 
  putbyte ( ( lh ) / 256 , tfmfile ) ; 
  putbyte ( ( lh ) % 256 , tfmfile ) ; 
  putbyte ( ( bc ) / 256 , tfmfile ) ; 
  putbyte ( ( bc ) % 256 , tfmfile ) ; 
  putbyte ( ( ec ) / 256 , tfmfile ) ; 
  putbyte ( ( ec ) % 256 , tfmfile ) ; 
  putbyte ( ( memory [ 1 ] ) / 256 , tfmfile ) ; 
  putbyte ( ( memory [ 1 ] ) % 256 , tfmfile ) ; 
  putbyte ( ( memory [ 2 ] ) / 256 , tfmfile ) ; 
  putbyte ( ( memory [ 2 ] ) % 256 , tfmfile ) ; 
  putbyte ( ( memory [ 3 ] ) / 256 , tfmfile ) ; 
  putbyte ( ( memory [ 3 ] ) % 256 , tfmfile ) ; 
  putbyte ( ( memory [ 4 ] ) / 256 , tfmfile ) ; 
  putbyte ( ( memory [ 4 ] ) % 256 , tfmfile ) ; 
  putbyte ( ( nl + lkoffset ) / 256 , tfmfile ) ; 
  putbyte ( ( nl + lkoffset ) % 256 , tfmfile ) ; 
  putbyte ( ( nk ) / 256 , tfmfile ) ; 
  putbyte ( ( nk ) % 256 , tfmfile ) ; 
  putbyte ( ( ne ) / 256 , tfmfile ) ; 
  putbyte ( ( ne ) % 256 , tfmfile ) ; 
  putbyte ( ( np ) / 256 , tfmfile ) ; 
  putbyte ( ( np ) % 256 , tfmfile ) ; 
  if ( ! checksumspecified ) 
  {
    curbytes .b0 = bc ; 
    curbytes .b1 = ec ; 
    curbytes .b2 = bc ; 
    curbytes .b3 = ec ; 
    {register integer for_end; c = bc ; for_end = ec ; if ( c <= for_end) do 
      if ( charwd [ c ] > 0 ) 
      {
	tempwidth = memory [ charwd [ c ] ] ; 
	if ( designunits != 1048576L ) 
	tempwidth = round ( ( tempwidth / ((double) designunits ) ) * 
	1048576.0 ) ; 
	tempwidth = tempwidth + ( c + 4 ) * 4194304L ; 
	curbytes .b0 = ( curbytes .b0 + curbytes .b0 + tempwidth ) % 255 ; 
	curbytes .b1 = ( curbytes .b1 + curbytes .b1 + tempwidth ) % 253 ; 
	curbytes .b2 = ( curbytes .b2 + curbytes .b2 + tempwidth ) % 251 ; 
	curbytes .b3 = ( curbytes .b3 + curbytes .b3 + tempwidth ) % 247 ; 
      } 
    while ( c++ < for_end ) ; } 
    headerbytes [ 0 ] = curbytes .b0 ; 
    headerbytes [ 1 ] = curbytes .b1 ; 
    headerbytes [ 2 ] = curbytes .b2 ; 
    headerbytes [ 3 ] = curbytes .b3 ; 
  } 
  headerbytes [ 4 ] = designsize / 16777216L ; 
  headerbytes [ 5 ] = ( designsize / 65536L ) % 256 ; 
  headerbytes [ 6 ] = ( designsize / 256 ) % 256 ; 
  headerbytes [ 7 ] = designsize % 256 ; 
  if ( ! sevenunsafe ) 
  headerbytes [ 68 ] = 128 ; 
  {register integer for_end; j = 0 ; for_end = headerptr - 1 ; if ( j <= 
  for_end) do 
    putbyte ( headerbytes [ j ] , tfmfile ) ; 
  while ( j++ < for_end ) ; } 
  index [ 0 ] = 0 ; 
  {register integer for_end; c = bc ; for_end = ec ; if ( c <= for_end) do 
    {
      putbyte ( index [ charwd [ c ] ] , tfmfile ) ; 
      putbyte ( index [ charht [ c ] ] * 16 + index [ chardp [ c ] ] , tfmfile 
      ) ; 
      putbyte ( index [ charic [ c ] ] * 4 + chartag [ c ] , tfmfile ) ; 
      putbyte ( charremainder [ c ] , tfmfile ) ; 
    } 
  while ( c++ < for_end ) ; } 
  {register integer for_end; q = 1 ; for_end = 4 ; if ( q <= for_end) do 
    {
      putbyte ( 0 , tfmfile ) ; 
      putbyte ( 0 , tfmfile ) ; 
      putbyte ( 0 , tfmfile ) ; 
      putbyte ( 0 , tfmfile ) ; 
      p = link [ q ] ; 
      while ( p > 0 ) {
	  
	outscaled ( memory [ p ] ) ; 
	p = link [ p ] ; 
      } 
    } 
  while ( q++ < for_end ) ; } 
  if ( extralocneeded ) 
  {
    putbyte ( 255 , tfmfile ) ; 
    putbyte ( bchar , tfmfile ) ; 
    putbyte ( 0 , tfmfile ) ; 
    putbyte ( 0 , tfmfile ) ; 
  } 
  else {
      register integer for_end; sortptr = 1 ; for_end = lkoffset ; if ( 
  sortptr <= for_end) do 
    {
      t = labeltable [ labelptr ] .rr ; 
      if ( bchar < 256 ) 
      {
	putbyte ( 255 , tfmfile ) ; 
	putbyte ( bchar , tfmfile ) ; 
      } 
      else {
	  
	putbyte ( 254 , tfmfile ) ; 
	putbyte ( 0 , tfmfile ) ; 
      } 
      putbyte ( ( t + lkoffset ) / 256 , tfmfile ) ; 
      putbyte ( ( t + lkoffset ) % 256 , tfmfile ) ; 
      do {
	  labelptr = labelptr - 1 ; 
      } while ( ! ( labeltable [ labelptr ] .rr < t ) ) ; 
    } 
  while ( sortptr++ < for_end ) ; } 
  if ( nl > 0 ) 
  {register integer for_end; ligptr = 0 ; for_end = nl - 1 ; if ( ligptr <= 
  for_end) do 
    {
      putbyte ( ligkern [ ligptr ] .b0 , tfmfile ) ; 
      putbyte ( ligkern [ ligptr ] .b1 , tfmfile ) ; 
      putbyte ( ligkern [ ligptr ] .b2 , tfmfile ) ; 
      putbyte ( ligkern [ ligptr ] .b3 , tfmfile ) ; 
    } 
  while ( ligptr++ < for_end ) ; } 
  if ( nk > 0 ) 
  {register integer for_end; krnptr = 0 ; for_end = nk - 1 ; if ( krnptr <= 
  for_end) do 
    outscaled ( kern [ krnptr ] ) ; 
  while ( krnptr++ < for_end ) ; } 
  if ( ne > 0 ) 
  {register integer for_end; c = 0 ; for_end = ne - 1 ; if ( c <= for_end) do 
    {
      putbyte ( exten [ c ] .b0 , tfmfile ) ; 
      putbyte ( exten [ c ] .b1 , tfmfile ) ; 
      putbyte ( exten [ c ] .b2 , tfmfile ) ; 
      putbyte ( exten [ c ] .b3 , tfmfile ) ; 
    } 
  while ( c++ < for_end ) ; } 
  {register integer for_end; parptr = 1 ; for_end = np ; if ( parptr <= 
  for_end) do 
    {
      if ( parptr == 1 ) 
      {
	if ( param [ 1 ] < 0 ) 
	{
	  param [ 1 ] = param [ 1 ] + 1073741824L ; 
	  putbyte ( ( param [ 1 ] / 16777216L ) + 192 , tfmfile ) ; 
	} 
	else putbyte ( param [ 1 ] / 16777216L , tfmfile ) ; 
	putbyte ( ( param [ 1 ] / 65536L ) % 256 , tfmfile ) ; 
	putbyte ( ( param [ 1 ] / 256 ) % 256 , tfmfile ) ; 
	putbyte ( param [ 1 ] % 256 , tfmfile ) ; 
      } 
      else outscaled ( param [ parptr ] ) ; 
    } 
  while ( parptr++ < for_end ) ; } 
} 
