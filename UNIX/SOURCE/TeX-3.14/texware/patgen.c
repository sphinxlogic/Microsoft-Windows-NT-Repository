#include "../common/extra.h"
/* 22 9999 */ 
#define triesize 55000L 
#define triecsize 26000 
#define maxops 4080 
#define maxval 7 
#define maxdot 15 
#define maxlen 50 
typedef char textchar  ; 
typedef schar asciicode  ; 
typedef unsigned char qindex  ; 
typedef integer valtype  ; 
typedef integer dottype  ; 
typedef integer optype  ; 
typedef integer wordindex  ; 
typedef integer triepointer  ; 
typedef integer triecpointer  ; 
typedef struct {
    asciicode ch ; 
  triepointer lh, rh ; 
} trienode  ; 
typedef struct {
    dottype dot ; 
  valtype val ; 
  optype op ; 
} opword  ; 
dottype patstart, patfinish  ; 
valtype hyphstart, hyphfinish  ; 
integer goodwt, badwt, thresh  ; 
file_ptr /* of  textchar */ dictionary, patterns, pattmp, outfile  ; 
char fname[FILENAMESIZE + 1]  ; 
asciicode xord[256]  ; 
textchar xchr[128]  ; 
asciicode triec[triesize + 1]  ; 
triepointer triel[triesize + 1], trier[triesize + 1]  ; 
boolean trietaken[triesize + 1]  ; 
asciicode triecc[triecsize + 1]  ; 
triecpointer triecl[triecsize + 1], triecr[triecsize + 1]  ; 
boolean triectaken[triecsize + 1]  ; 
opword ops[maxops + 1]  ; 
trienode trieq[129]  ; 
qindex qmax  ; 
qindex qmaxthresh  ; 
triepointer triemax  ; 
triepointer triebmax  ; 
triepointer triecount  ; 
optype opcount  ; 
asciicode pat[maxdot + 1]  ; 
dottype patlen  ; 
triecpointer triecmax, triecbmax, trieccount  ; 
triecpointer trieckmax  ; 
integer patcount  ; 
integer goodpatcount, badpatcount  ; 
integer goodcount, badcount, misscount  ; 
integer levelpatterncount  ; 
boolean moretocome  ; 
asciicode word[maxlen + 1]  ; 
asciicode dots[maxlen + 1]  ; 
integer dotw[maxlen + 1]  ; 
valtype hval[maxlen + 1]  ; 
boolean nomore[maxlen + 1]  ; 
wordindex wlen  ; 
integer wordwt  ; 
boolean wtchg  ; 
textchar buf[81]  ; 
schar bufptr  ; 
asciicode gooddot, baddot  ; 
boolean procesp, hyphp  ; 
dottype patdot  ; 
valtype hyphlevel  ; 
char filnam[9]  ; 
dottype dot1, dot2  ; 
boolean levelnomore[maxdot + 1]  ; 

#include "patgen.h"
void initialize ( ) 
{textchar i  ; 
  asciicode j  ; 
  if ( argc < 4 ) 
  {
    {
      (void) fprintf( stdout , "%s\n",        "Usage: patgen <dictionary file> <pattern file> <output file>" ) ; 
      uexit ( 1 ) ; 
    } 
  } 
  argv ( 3 , fname ) ; 
  rewrite ( outfile , fname ) ; 
  {register integer for_end; i = chr ( 0 ) ; for_end = chr ( 127 ) ; if ( i 
  <= for_end) do 
    xord [ i ] = 127 ; 
  while ( i++ < for_end ) ; } 
  xord [ '*' ] = 42 ; 
  xord [ '-' ] = 45 ; 
  xord [ '.' ] = 46 ; 
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
  xord [ 'a' ] = 65 ; 
  xord [ 'b' ] = 66 ; 
  xord [ 'c' ] = 67 ; 
  xord [ 'd' ] = 68 ; 
  xord [ 'e' ] = 69 ; 
  xord [ 'f' ] = 70 ; 
  xord [ 'g' ] = 71 ; 
  xord [ 'h' ] = 72 ; 
  xord [ 'i' ] = 73 ; 
  xord [ 'j' ] = 74 ; 
  xord [ 'k' ] = 75 ; 
  xord [ 'l' ] = 76 ; 
  xord [ 'm' ] = 77 ; 
  xord [ 'n' ] = 78 ; 
  xord [ 'o' ] = 79 ; 
  xord [ 'p' ] = 80 ; 
  xord [ 'q' ] = 81 ; 
  xord [ 'r' ] = 82 ; 
  xord [ 's' ] = 83 ; 
  xord [ 't' ] = 84 ; 
  xord [ 'u' ] = 85 ; 
  xord [ 'v' ] = 86 ; 
  xord [ 'w' ] = 87 ; 
  xord [ 'x' ] = 88 ; 
  xord [ 'y' ] = 89 ; 
  xord [ 'z' ] = 90 ; 
  {register integer for_end; j = 0 ; for_end = 127 ; if ( j <= for_end) do 
    xchr [ j ] = ' ' ; 
  while ( j++ < for_end ) ; } 
  xchr [ 42 ] = '*' ; 
  xchr [ 45 ] = '-' ; 
  xchr [ 46 ] = '.' ; 
  xchr [ 48 ] = '0' ; 
  xchr [ 49 ] = '1' ; 
  xchr [ 50 ] = '2' ; 
  xchr [ 51 ] = '3' ; 
  xchr [ 52 ] = '4' ; 
  xchr [ 53 ] = '5' ; 
  xchr [ 54 ] = '6' ; 
  xchr [ 55 ] = '7' ; 
  xchr [ 56 ] = '8' ; 
  xchr [ 57 ] = '9' ; 
  xchr [ 65 ] = 'a' ; 
  xchr [ 66 ] = 'b' ; 
  xchr [ 67 ] = 'c' ; 
  xchr [ 68 ] = 'd' ; 
  xchr [ 69 ] = 'e' ; 
  xchr [ 70 ] = 'f' ; 
  xchr [ 71 ] = 'g' ; 
  xchr [ 72 ] = 'h' ; 
  xchr [ 73 ] = 'i' ; 
  xchr [ 74 ] = 'j' ; 
  xchr [ 75 ] = 'k' ; 
  xchr [ 76 ] = 'l' ; 
  xchr [ 77 ] = 'm' ; 
  xchr [ 78 ] = 'n' ; 
  xchr [ 79 ] = 'o' ; 
  xchr [ 80 ] = 'p' ; 
  xchr [ 81 ] = 'q' ; 
  xchr [ 82 ] = 'r' ; 
  xchr [ 83 ] = 's' ; 
  xchr [ 84 ] = 't' ; 
  xchr [ 85 ] = 'u' ; 
  xchr [ 86 ] = 'v' ; 
  xchr [ 87 ] = 'w' ; 
  xchr [ 88 ] = 'x' ; 
  xchr [ 89 ] = 'y' ; 
  xchr [ 90 ] = 'z' ; 
} 
void initpatterntrie ( ) 
{asciicode c  ; 
  optype h  ; 
  {register integer for_end; c = 0 ; for_end = 127 ; if ( c <= for_end) do 
    {
      triec [ 1 + c ] = c ; 
      triel [ 1 + c ] = 0 ; 
      trier [ 1 + c ] = 0 ; 
    } 
  while ( c++ < for_end ) ; } 
  trietaken [ 1 ] = true ; 
  triebmax = 1 ; 
  triemax = 128 ; 
  triecount = 128 ; 
  qmaxthresh = 5 ; 
  triel [ 0 ] = triemax + 1 ; 
  trier [ triemax + 1 ] = 0 ; 
  {register integer for_end; h = 1 ; for_end = maxops ; if ( h <= for_end) do 
    ops [ h ] .val = 0 ; 
  while ( h++ < for_end ) ; } 
  opcount = 0 ; 
} 
triepointer firstfit ( ) 
{/* 40 41 */ register triepointer Result; triepointer s, t  ; 
  qindex q  ; 
  if ( qmax > qmaxthresh ) 
  t = trier [ triemax + 1 ] ; 
  else t = 0 ; 
  while ( true ) {
      
    t = triel [ t ] ; 
    s = t - trieq [ 1 ] .ch ; 
    if ( s + 128 > triesize ) 
    {
      (void) fprintf( stdout , "%s\n",  "Pattern trie too full!" ) ; 
      uexit ( 1 ) ; 
    } 
    while ( triebmax < s ) {
	
      triebmax = triebmax + 1 ; 
      trietaken [ triebmax ] = false ; 
      triec [ triebmax + 127 ] = 0 ; 
      triel [ triebmax + 127 ] = triebmax + 128 ; 
      trier [ triebmax + 128 ] = triebmax + 127 ; 
    } 
    if ( trietaken [ s ] ) 
    goto lab41 ; 
    {register integer for_end; q = qmax ; for_end = 2 ; if ( q >= for_end) do 
      if ( triec [ s + trieq [ q ] .ch ] > 0 ) 
      goto lab41 ; 
    while ( q-- > for_end ) ; } 
    goto lab40 ; 
    lab41: ; 
  } 
  lab40: ; 
  {register integer for_end; q = 1 ; for_end = qmax ; if ( q <= for_end) do 
    {
      t = s + trieq [ q ] .ch ; 
      triel [ trier [ t ] ] = triel [ t ] ; 
      trier [ triel [ t ] ] = trier [ t ] ; 
      triec [ t ] = trieq [ q ] .ch ; 
      triel [ t ] = trieq [ q ] .rh ; 
      trier [ t ] = trieq [ q ] .lh ; 
      if ( t > triemax ) 
      triemax = t ; 
    } 
  while ( q++ < for_end ) ; } 
  trietaken [ s ] = true ; 
  Result = s ; 
  return(Result) ; 
} 
void unpack ( triepointer s ) 
{asciicode c  ; 
  triepointer t  ; 
  qmax = 0 ; 
  {register integer for_end; c = 64 ; for_end = 90 ; if ( c <= for_end) do 
    {
      t = s + c ; 
      if ( triec [ t ] == c ) 
      {
	qmax = qmax + 1 ; 
	trieq [ qmax ] .ch = c ; 
	trieq [ qmax ] .rh = triel [ t ] ; 
	trieq [ qmax ] .lh = trier [ t ] ; 
	trier [ triel [ 0 ] ] = t ; 
	triel [ t ] = triel [ 0 ] ; 
	triel [ 0 ] = t ; 
	trier [ t ] = 0 ; 
	triec [ t ] = 0 ; 
      } 
    } 
  while ( c++ < for_end ) ; } 
  trietaken [ s ] = false ; 
} 
optype newtrieop ( valtype v , dottype d , optype n ) 
{/* 10 */ register optype Result; optype h  ; 
  h = ( ( n + 313 * d + 361 * v ) % maxops ) + 1 ; 
  while ( true ) {
      
    if ( ops [ h ] .val == 0 ) 
    {
      opcount = opcount + 1 ; 
      if ( opcount == maxops ) 
      {
	(void) fprintf( stdout , "%s\n",  "Too many outputs!" ) ; 
	uexit ( 1 ) ; 
      } 
      ops [ h ] .val = v ; 
      ops [ h ] .dot = d ; 
      ops [ h ] .op = n ; 
      Result = h ; 
      goto lab10 ; 
    } 
    if ( ( ops [ h ] .val == v ) && ( ops [ h ] .dot == d ) && ( ops [ h ] .op 
    == n ) ) 
    {
      Result = h ; 
      goto lab10 ; 
    } 
    if ( h > 1 ) 
    h = h - 1 ; 
    else h = maxops ; 
  } 
  lab10: ; 
  return(Result) ; 
} 
void insertpattern ( valtype val , dottype dot ) 
{dottype i  ; 
  triepointer s, t  ; 
  i = 1 ; 
  s = 1 + pat [ i ] ; 
  t = triel [ s ] ; 
  while ( ( t > 0 ) && ( i < patlen ) ) {
      
    i = i + 1 ; 
    t = t + pat [ i ] ; 
    if ( triec [ t ] != pat [ i ] ) 
    {
      if ( triec [ t ] == 0 ) 
      {
	triel [ trier [ t ] ] = triel [ t ] ; 
	trier [ triel [ t ] ] = trier [ t ] ; 
	triec [ t ] = pat [ i ] ; 
	triel [ t ] = 0 ; 
	trier [ t ] = 0 ; 
	if ( t > triemax ) 
	triemax = t ; 
      } 
      else {
	  
	t = t - pat [ i ] ; 
	unpack ( t ) ; 
	qmax = qmax + 1 ; 
	trieq [ qmax ] .ch = pat [ i ] ; 
	trieq [ qmax ] .rh = 0 ; 
	trieq [ qmax ] .lh = 0 ; 
	t = firstfit () ; 
	triel [ s ] = t ; 
	t = t + pat [ i ] ; 
      } 
      triecount = triecount + 1 ; 
    } 
    s = t ; 
    t = triel [ s ] ; 
  } 
  trieq [ 1 ] .rh = 0 ; 
  trieq [ 1 ] .lh = 0 ; 
  qmax = 1 ; 
  while ( i < patlen ) {
      
    i = i + 1 ; 
    trieq [ 1 ] .ch = pat [ i ] ; 
    t = firstfit () ; 
    triel [ s ] = t ; 
    s = t + pat [ i ] ; 
    triecount = triecount + 1 ; 
  } 
  trier [ s ] = newtrieop ( val , dot , trier [ s ] ) ; 
} 
void initcounttrie ( ) 
{asciicode c  ; 
  {register integer for_end; c = 0 ; for_end = 127 ; if ( c <= for_end) do 
    {
      triecc [ 1 + c ] = c ; 
      triecl [ 1 + c ] = 0 ; 
      triecr [ 1 + c ] = 0 ; 
    } 
  while ( c++ < for_end ) ; } 
  triectaken [ 1 ] = true ; 
  triecbmax = 1 ; 
  triecmax = 128 ; 
  trieccount = 128 ; 
  trieckmax = 4096 ; 
  triecl [ 0 ] = triecmax + 1 ; 
  triecr [ triecmax + 1 ] = 0 ; 
  patcount = 0 ; 
} 
triecpointer firstcfit ( ) 
{/* 40 41 */ register triecpointer Result; triecpointer a, b  ; 
  qindex q  ; 
  if ( qmax > 3 ) 
  a = triecr [ triecmax + 1 ] ; 
  else a = 0 ; 
  while ( true ) {
      
    a = triecl [ a ] ; 
    b = a - trieq [ 1 ] .ch ; 
    if ( b + 128 > trieckmax ) 
    {
      if ( trieckmax == triecsize ) 
      {
	(void) fprintf( stdout , "%s\n",  "Count trie too full!" ) ; 
	uexit ( 1 ) ; 
      } 
      (void) fprintf( stdout , "%ld%s",  (long)trieckmax / 1024 , "K " ) ; 
      if ( trieckmax + 4096 > triecsize ) 
      trieckmax = triecsize ; 
      else trieckmax = trieckmax + 4096 ; 
    } 
    while ( triecbmax < b ) {
	
      triecbmax = triecbmax + 1 ; 
      triectaken [ triecbmax ] = false ; 
      triecc [ triecbmax + 127 ] = 0 ; 
      triecl [ triecbmax + 127 ] = triecbmax + 128 ; 
      triecr [ triecbmax + 128 ] = triecbmax + 127 ; 
    } 
    if ( triectaken [ b ] ) 
    goto lab41 ; 
    {register integer for_end; q = qmax ; for_end = 2 ; if ( q >= for_end) do 
      if ( triecc [ b + trieq [ q ] .ch ] > 0 ) 
      goto lab41 ; 
    while ( q-- > for_end ) ; } 
    goto lab40 ; 
    lab41: ; 
  } 
  lab40: ; 
  {register integer for_end; q = 1 ; for_end = qmax ; if ( q <= for_end) do 
    {
      a = b + trieq [ q ] .ch ; 
      triecl [ triecr [ a ] ] = triecl [ a ] ; 
      triecr [ triecl [ a ] ] = triecr [ a ] ; 
      triecc [ a ] = trieq [ q ] .ch ; 
      triecl [ a ] = trieq [ q ] .rh ; 
      triecr [ a ] = trieq [ q ] .lh ; 
      if ( a > triecmax ) 
      triecmax = a ; 
    } 
  while ( q++ < for_end ) ; } 
  triectaken [ b ] = true ; 
  Result = b ; 
  return(Result) ; 
} 
void unpackc ( triecpointer b ) 
{asciicode c  ; 
  triecpointer a  ; 
  qmax = 0 ; 
  {register integer for_end; c = 64 ; for_end = 90 ; if ( c <= for_end) do 
    {
      a = b + c ; 
      if ( triecc [ a ] == c ) 
      {
	qmax = qmax + 1 ; 
	trieq [ qmax ] .ch = c ; 
	trieq [ qmax ] .rh = triecl [ a ] ; 
	trieq [ qmax ] .lh = triecr [ a ] ; 
	triecr [ triecl [ 0 ] ] = a ; 
	triecl [ a ] = triecl [ 0 ] ; 
	triecl [ 0 ] = a ; 
	triecr [ a ] = 0 ; 
	triecc [ a ] = 0 ; 
      } 
    } 
  while ( c++ < for_end ) ; } 
  triectaken [ b ] = false ; 
} 
triecpointer insertcpat ( wordindex fpos ) 
{register triecpointer Result; wordindex spos  ; 
  triecpointer a, b  ; 
  spos = fpos - patlen ; 
  spos = spos + 1 ; 
  b = 1 + word [ spos ] ; 
  a = triecl [ b ] ; 
  while ( ( a > 0 ) && ( spos < fpos ) ) {
      
    spos = spos + 1 ; 
    a = a + word [ spos ] ; 
    if ( triecc [ a ] != word [ spos ] ) 
    {
      if ( triecc [ a ] == 0 ) 
      {
	triecl [ triecr [ a ] ] = triecl [ a ] ; 
	triecr [ triecl [ a ] ] = triecr [ a ] ; 
	triecc [ a ] = word [ spos ] ; 
	triecl [ a ] = 0 ; 
	triecr [ a ] = 0 ; 
	if ( a > triecmax ) 
	triecmax = a ; 
      } 
      else {
	  
	a = a - word [ spos ] ; 
	unpackc ( a ) ; 
	qmax = qmax + 1 ; 
	trieq [ qmax ] .ch = word [ spos ] ; 
	trieq [ qmax ] .rh = 0 ; 
	trieq [ qmax ] .lh = 0 ; 
	a = firstcfit () ; 
	triecl [ b ] = a ; 
	a = a + word [ spos ] ; 
      } 
      trieccount = trieccount + 1 ; 
    } 
    b = a ; 
    a = triecl [ a ] ; 
  } 
  trieq [ 1 ] .rh = 0 ; 
  trieq [ 1 ] .lh = 0 ; 
  qmax = 1 ; 
  while ( spos < fpos ) {
      
    spos = spos + 1 ; 
    trieq [ 1 ] .ch = word [ spos ] ; 
    a = firstcfit () ; 
    triecl [ b ] = a ; 
    b = a + word [ spos ] ; 
    trieccount = trieccount + 1 ; 
  } 
  Result = b ; 
  patcount = patcount + 1 ; 
  return(Result) ; 
} 
void traversecounttrie ( triecpointer b , dottype i ) 
{asciicode c  ; 
  triecpointer a  ; 
  i = i + 1 ; 
  {register integer for_end; c = 64 ; for_end = 90 ; if ( c <= for_end) do 
    {
      a = b + c ; 
      if ( triecc [ a ] == c ) 
      {
	pat [ i ] = c ; 
	if ( i < patlen ) 
	traversecounttrie ( triecl [ a ] , i ) ; 
	else if ( goodwt * triecl [ a ] < thresh ) 
	{
	  insertpattern ( maxval , patdot ) ; 
	  badpatcount = badpatcount + 1 ; 
	} 
	else if ( goodwt * triecl [ a ] - badwt * triecr [ a ] >= thresh ) 
	{
	  insertpattern ( hyphlevel , patdot ) ; 
	  goodpatcount = goodpatcount + 1 ; 
	  goodcount = goodcount + triecl [ a ] ; 
	  badcount = badcount + triecr [ a ] ; 
	} 
	else moretocome = true ; 
      } 
    } 
  while ( c++ < for_end ) ; } 
} 
void collectcounttrie ( ) 
{goodpatcount = 0 ; 
  badpatcount = 0 ; 
  goodcount = 0 ; 
  badcount = 0 ; 
  moretocome = false ; 
  traversecounttrie ( 1 , 0 ) ; 
  (void) fprintf( stdout , "%ld%s%ld%s",  (long)goodpatcount , " good and " , (long)badpatcount ,   " bad patterns added" ) ; 
  levelpatterncount = levelpatterncount + goodpatcount ; 
  if ( moretocome ) 
  (void) fprintf( stdout , "%s\n",  " (more to come)" ) ; 
  else
  (void) fprintf( stdout , "%c\n",  ' ' ) ; 
  (void) fprintf( stdout , "%s%ld%s%ld%s",  "finding " , (long)goodcount , " good and " , (long)badcount ,   " bad hyphens" ) ; 
  if ( goodpatcount > 0 ) 
  {
    (void) Fputs( stdout ,  ", efficiency = " ) ; 
    printreal ( goodcount / ((double) ( goodpatcount + badcount / ((double) ( 
    thresh / ((double) goodwt ) ) ) ) ) , 1 , 2 ) ; 
    (void) putc('\n',  stdout );
  } 
  else
  (void) fprintf( stdout , "%c\n",  ' ' ) ; 
  (void) fprintf( stdout , "%s%ld%s%s%ld%s%ld%s\n",  "pattern trie has " , (long)triecount , " nodes, " ,   "trie_max = " , (long)triemax , ", " , (long)opcount , " outputs" ) ; 
} 
triepointer deletepatterns ( triepointer s ) 
{register triepointer Result; asciicode c  ; 
  triepointer t  ; 
  boolean allfreed  ; 
  optype h, n  ; 
  allfreed = true ; 
  {register integer for_end; c = 64 ; for_end = 90 ; if ( c <= for_end) do 
    {
      t = s + c ; 
      if ( triec [ t ] == c ) 
      {
	{
	  h = 0 ; 
	  ops [ 0 ] .op = trier [ t ] ; 
	  n = ops [ 0 ] .op ; 
	  while ( n > 0 ) {
	      
	    if ( ops [ n ] .val == maxval ) 
	    ops [ h ] .op = ops [ n ] .op ; 
	    else h = n ; 
	    n = ops [ h ] .op ; 
	  } 
	  trier [ t ] = ops [ 0 ] .op ; 
	} 
	if ( triel [ t ] > 0 ) 
	triel [ t ] = deletepatterns ( triel [ t ] ) ; 
	if ( ( triel [ t ] > 0 ) || ( trier [ t ] > 0 ) || ( ( t <= 128 ) && ( 
	t >= 1 ) ) ) 
	allfreed = false ; 
	else {
	    
	  triel [ trier [ triemax + 1 ] ] = t ; 
	  trier [ t ] = trier [ triemax + 1 ] ; 
	  triel [ t ] = triemax + 1 ; 
	  trier [ triemax + 1 ] = t ; 
	  triec [ t ] = 0 ; 
	  triecount = triecount - 1 ; 
	} 
      } 
    } 
  while ( c++ < for_end ) ; } 
  if ( allfreed ) 
  {
    trietaken [ s ] = false ; 
    s = 0 ; 
  } 
  Result = s ; 
  return(Result) ; 
} 
void deletebadpatterns ( ) 
{optype oldopcount  ; 
  triepointer oldtriecount  ; 
  triepointer t  ; 
  optype h  ; 
  oldopcount = opcount ; 
  oldtriecount = triecount ; 
  t = deletepatterns ( 1 ) ; 
  {register integer for_end; h = 1 ; for_end = maxops ; if ( h <= for_end) do 
    if ( ops [ h ] .val == maxval ) 
    {
      ops [ h ] .val = 0 ; 
      opcount = opcount - 1 ; 
    } 
  while ( h++ < for_end ) ; } 
  (void) fprintf( stdout , "%ld%s%ld%s\n",  (long)oldtriecount - triecount , " nodes and " , (long)oldopcount -   opcount , " outputs deleted" ) ; 
  qmaxthresh = 7 ; 
} 
void outputpatterns ( triepointer s , dottype patlen ) 
{asciicode c  ; 
  triepointer t  ; 
  optype h  ; 
  dottype d  ; 
  patlen = patlen + 1 ; 
  {register integer for_end; c = 64 ; for_end = 90 ; if ( c <= for_end) do 
    {
      t = s + c ; 
      if ( triec [ t ] == c ) 
      {
	pat [ patlen ] = c ; 
	h = trier [ t ] ; 
	if ( h > 0 ) 
	{
	  {register integer for_end; d = 0 ; for_end = patlen ; if ( d <= 
	  for_end) do 
	    hval [ d ] = 0 ; 
	  while ( d++ < for_end ) ; } 
	  do {
	      d = ops [ h ] .dot ; 
	    if ( hval [ d ] < ops [ h ] .val ) 
	    hval [ d ] = ops [ h ] .val ; 
	    h = ops [ h ] .op ; 
	  } while ( ! ( h == 0 ) ) ; 
	  if ( hval [ 0 ] > 0 ) 
	  (void) fprintf( outfile , "%ld",  (long)hval [ 0 ] ) ; 
	  {register integer for_end; d = 1 ; for_end = patlen ; if ( d <= 
	  for_end) do 
	    {
	      if ( pat [ d ] == 64 ) 
	      (void) putc( '.' ,  outfile );
	      else
	      (void) putc( xchr [ pat [ d ] ] ,  outfile );
	      if ( hval [ d ] > 0 ) 
	      (void) fprintf( outfile , "%ld",  (long)hval [ d ] ) ; 
	    } 
	  while ( d++ < for_end ) ; } 
	  (void) putc('\n',  outfile );
	} 
	if ( triel [ t ] > 0 ) 
	outputpatterns ( triel [ t ] , patlen ) ; 
      } 
    } 
  while ( c++ < for_end ) ; } 
} 
void readword ( ) 
{asciicode c  ; 
  integer bptr  ; 
  bufptr = 1 ; 
  while ( ! eoln ( dictionary ) && ( bufptr < 80 ) ) {
      
    buf [ bufptr ] = getc ( dictionary ) ; 
    bufptr = bufptr + 1 ; 
  } 
  buf [ bufptr ] = ' ' ; 
  readln ( dictionary ) ; 
  word [ 1 ] = 64 ; 
  wlen = 1 ; 
  bufptr = 1 ; 
  c = xord [ buf [ bufptr ] ] ; 
  if ( ( c <= 57 ) && ( c >= 48 ) ) 
  {
    wordwt = ( c - 48 ) ; 
    wtchg = true ; 
    bufptr = bufptr + 1 ; 
  } 
  do {
      c = xord [ buf [ bufptr ] ] ; 
    if ( c >= 65 ) 
    {
      wlen = wlen + 1 ; 
      word [ wlen ] = c ; 
      dots [ wlen ] = 0 ; 
      dotw [ wlen ] = wordwt ; 
    } 
    else if ( c >= 48 ) 
    dotw [ wlen ] = ( c - 48 ) ; 
    else dots [ wlen ] = c ; 
    bufptr = bufptr + 1 ; 
  } while ( ! ( buf [ bufptr ] == ' ' ) ) ; 
  wlen = wlen + 1 ; 
  word [ wlen ] = 64 ; 
  dotw [ wlen - 3 ] = 1 ; 
} 
void hyphenate ( ) 
{/* 30 */ wordindex spos, dpos, fpos  ; 
  triepointer t  ; 
  optype h  ; 
  valtype v  ; 
  {register integer for_end; spos = wlen - 3 ; for_end = 0 ; if ( spos >= 
  for_end) do 
    {
      nomore [ spos ] = false ; 
      hval [ spos ] = 0 ; 
      fpos = spos + 1 ; 
      t = 1 + word [ fpos ] ; 
      do {
	  h = trier [ t ] ; 
	while ( h > 0 ) {
	    
	  dpos = spos + ops [ h ] .dot ; 
	  v = ops [ h ] .val ; 
	  if ( ( v < maxval ) && ( hval [ dpos ] < v ) ) 
	  hval [ dpos ] = v ; 
	  if ( ( v >= hyphlevel ) ) 
	  if ( ( ( fpos - patlen ) <= ( dpos - patdot ) ) && ( ( dpos - patdot 
	  ) <= spos ) ) 
	  nomore [ dpos ] = true ; 
	  h = ops [ h ] .op ; 
	} 
	t = triel [ t ] ; 
	if ( t == 0 ) 
	goto lab30 ; 
	fpos = fpos + 1 ; 
	t = t + word [ fpos ] ; 
      } while ( ! ( triec [ t ] != word [ fpos ] ) ) ; 
      lab30: ; 
    } 
  while ( spos-- > for_end ) ; } 
} 
void changedots ( ) 
{wordindex dpos  ; 
  {register integer for_end; dpos = wlen - 3 ; for_end = 3 ; if ( dpos >= 
  for_end) do 
    {
      if ( hval [ dpos ] > 0 ) 
      if ( odd ( hval [ dpos ] ) ) 
      {
	if ( dots [ dpos ] == 45 ) 
	dots [ dpos ] = 42 ; 
	else if ( dots [ dpos ] == 0 ) 
	dots [ dpos ] = 46 ; 
      } 
      else {
	  
	if ( dots [ dpos ] == 46 ) 
	dots [ dpos ] = 0 ; 
	else if ( dots [ dpos ] == 42 ) 
	dots [ dpos ] = 45 ; 
      } 
      if ( dots [ dpos ] == 42 ) 
      goodcount = goodcount + dotw [ dpos ] ; 
      else if ( dots [ dpos ] == 46 ) 
      badcount = badcount + dotw [ dpos ] ; 
      else if ( dots [ dpos ] == 45 ) 
      misscount = misscount + dotw [ dpos ] ; 
    } 
  while ( dpos-- > for_end ) ; } 
} 
void outputhyphenatedword ( ) 
{wordindex dpos  ; 
  if ( wtchg ) 
  {
    (void) fprintf( pattmp , "%ld",  (long)wordwt ) ; 
    wtchg = false ; 
  } 
  if ( wlen < 6 ) 
  {
    {register integer for_end; dpos = 2 ; for_end = wlen - 1 ; if ( dpos <= 
    for_end) do 
      (void) putc( xchr [ word [ dpos ] ] ,  pattmp );
    while ( dpos++ < for_end ) ; } 
    (void) putc('\n',  pattmp );
  } 
  else {
      
    (void) putc( xchr [ word [ 2 ] ] ,  pattmp );
    {register integer for_end; dpos = 3 ; for_end = wlen - 3 ; if ( dpos <= 
    for_end) do 
      {
	(void) putc( xchr [ word [ dpos ] ] ,  pattmp );
	if ( dots [ dpos ] > 0 ) 
	(void) putc( xchr [ dots [ dpos ] ] ,  pattmp );
	if ( ( dotw [ dpos ] != wordwt ) && ( dpos < wlen - 3 ) ) 
	(void) fprintf( pattmp , "%ld",  (long)dotw [ dpos ] ) ; 
      } 
    while ( dpos++ < for_end ) ; } 
    (void) fprintf( pattmp , "%c%c\n",  xchr [ word [ wlen - 2 ] ] , xchr [ word [ wlen - 1 ] ]     ) ; 
  } 
} 
void doword ( ) 
{/* 22 30 */ wordindex spos, dpos, fpos  ; 
  triecpointer a  ; 
  boolean goodp  ; 
  {register integer for_end; dpos = wlen - 3 ; for_end = 3 ; if ( dpos >= 
  for_end) do 
    {
      spos = dpos - patdot ; 
      fpos = spos + patlen ; 
      if ( ( spos < 0 ) || ( fpos > wlen ) || nomore [ dpos ] ) 
      goto lab22 ; 
      if ( dots [ dpos ] == gooddot ) 
      goodp = true ; 
      else if ( dots [ dpos ] == baddot ) 
      goodp = false ; 
      else goto lab22 ; 
      spos = spos + 1 ; 
      a = 1 + word [ spos ] ; 
      while ( spos < fpos ) {
	  
	spos = spos + 1 ; 
	a = triecl [ a ] + word [ spos ] ; 
	if ( triecc [ a ] != word [ spos ] ) 
	{
	  a = insertcpat ( fpos ) ; 
	  goto lab30 ; 
	} 
      } 
      lab30: if ( goodp ) 
      triecl [ a ] = triecl [ a ] + dotw [ dpos ] ; 
      else triecr [ a ] = triecr [ a ] + dotw [ dpos ] ; 
      lab22: ; 
    } 
  while ( dpos-- > for_end ) ; } 
} 
void dodictionary ( ) 
{goodcount = 0 ; 
  badcount = 0 ; 
  misscount = 0 ; 
  wordwt = 1 ; 
  wtchg = false ; 
  argv ( 1 , fname ) ; 
  reset ( dictionary , fname ) ; 
  if ( odd ( hyphlevel ) ) 
  {
    gooddot = 45 ; 
    baddot = 0 ; 
  } 
  else {
      
    gooddot = 46 ; 
    baddot = 42 ; 
  } 
  if ( procesp ) 
  {
    initcounttrie () ; 
    (void) fprintf( stdout , "%s%ld%s%ld\n",  "processing dictionary with pat_len = " , (long)patlen ,     ", pat_dot = " , (long)patdot ) ; 
  } 
  if ( hyphp ) 
  {
    {
      filnam [ 1 ] = 'p' ; 
      filnam [ 2 ] = 'a' ; 
      filnam [ 3 ] = 't' ; 
      filnam [ 4 ] = 't' ; 
      filnam [ 5 ] = 'm' ; 
      filnam [ 6 ] = 'p' ; 
      filnam [ 7 ] = '.' ; 
    } 
    filnam [ 8 ] = chr ( hyphlevel + ord ( '0' ) ) ; 
    rewrite ( pattmp , filnam ) ; 
    (void) fprintf( stdout , "%s%ld\n",  "writing " , (long)filnam ) ; 
  } 
  while ( ! eof ( dictionary ) ) {
      
    readword () ; 
    hyphenate () ; 
    changedots () ; 
    if ( hyphp ) 
    outputhyphenatedword () ; 
    if ( procesp ) 
    doword () ; 
  } 
  (void) fprintf( stdout , "%c\n",  ' ' ) ; 
  (void) fprintf( stdout , "%ld%s%ld%s%ld%s\n",  (long)goodcount , " good, " , (long)badcount , " bad, " , (long)misscount ,   " missed" ) ; 
  printreal ( ( 100 * goodcount / ((double) ( goodcount + misscount ) ) ) , 1 
  , 2 ) ; 
  (void) Fputs( stdout ,  " %, " ) ; 
  printreal ( ( 100 * badcount / ((double) ( goodcount + misscount ) ) ) , 1 , 
  2 ) ; 
  (void) Fputs( stdout ,  " %, " ) ; 
  printreal ( ( 100 * misscount / ((double) ( goodcount + misscount ) ) ) , 1 
  , 2 ) ; 
  (void) fprintf( stdout , "%s\n",  " %" ) ; 
  if ( procesp ) 
  (void) fprintf( stdout , "%ld%s%ld%s%s%ld\n",  (long)patcount , " patterns, " , (long)trieccount ,   " nodes in count trie, " , "triec_max = " , (long)triecmax ) ; 
} 
void readpatterns ( ) 
{asciicode c  ; 
  levelpatterncount = 0 ; 
  argv ( 2 , fname ) ; 
  reset ( patterns , fname ) ; 
  while ( ! eof ( patterns ) ) {
      
    bufptr = 1 ; 
    while ( ! eoln ( patterns ) && ( bufptr < 80 ) ) {
	
      buf [ bufptr ] = getc ( patterns ) ; 
      bufptr = bufptr + 1 ; 
    } 
    buf [ bufptr ] = ' ' ; 
    readln ( patterns ) ; 
    levelpatterncount = levelpatterncount + 1 ; 
    patlen = 0 ; 
    bufptr = 1 ; 
    while ( buf [ bufptr ] != ' ' ) {
	
      c = xord [ buf [ bufptr ] ] ; 
      if ( c >= 65 ) 
      {
	patlen = patlen + 1 ; 
	pat [ patlen ] = c ; 
      } 
      else if ( c == 46 ) 
      {
	patlen = patlen + 1 ; 
	pat [ patlen ] = 64 ; 
      } 
      bufptr = bufptr + 1 ; 
    } 
    patdot = 0 ; 
    bufptr = 1 ; 
    while ( buf [ bufptr ] != ' ' ) {
	
      c = xord [ buf [ bufptr ] ] ; 
      if ( ( c <= 57 ) && ( c >= 48 ) ) 
      insertpattern ( c - 48 , patdot ) ; 
      else patdot = patdot + 1 ; 
      bufptr = bufptr + 1 ; 
    } 
  } 
  (void) fprintf( stdout , "%ld%s\n",  (long)levelpatterncount , " patterns read in" ) ; 
  (void) fprintf( stdout , "%s%ld%s%s%ld%s%ld%s\n",  "pattern trie has " , (long)triecount , " nodes, " ,   "trie_max = " , (long)triemax , ", " , (long)opcount , " outputs" ) ; 
} 
void main_body() {
    
  initialize () ; 
  initpatterntrie () ; 
  readpatterns () ; 
  procesp = true ; 
  hyphp = false ; 
  (void) Fputs( stdout ,  "hyph_start = " ) ; 
  hyphstart = inputint ( stdin ) ; 
  (void) Fputs( stdout ,  "hyph_finish = " ) ; 
  hyphfinish = inputint ( stdin ) ; 
  {register integer for_end; hyphlevel = hyphstart ; for_end = hyphfinish 
  ; if ( hyphlevel <= for_end) do 
    {
      levelpatterncount = 0 ; 
      {register integer for_end; patdot = 0 ; for_end = maxdot ; if ( patdot 
      <= for_end) do 
	levelnomore [ patdot ] = false ; 
      while ( patdot++ < for_end ) ; } 
      if ( hyphlevel > hyphstart ) 
      (void) fprintf( stdout , "%c\n",  ' ' ) ; 
      (void) Fputs( stdout ,  "pat_start = " ) ; 
      patstart = inputint ( stdin ) ; 
      (void) Fputs( stdout ,  "pat_finish = " ) ; 
      patfinish = inputint ( stdin ) ; 
      (void) Fputs( stdout ,  "good weight, bad weight, threshold: " ) ; 
      input3ints ( goodwt , badwt , thresh ) ; 
      {register integer for_end; patlen = patstart ; for_end = patfinish 
      ; if ( patlen <= for_end) do 
	{
	  patdot = patlen / 2 ; 
	  dot1 = patdot * 2 ; 
	  dot2 = patlen * 2 - 1 ; 
	  do {
	      patdot = dot1 - patdot ; 
	    dot1 = dot2 - dot1 ; 
	    if ( levelnomore [ patdot ] ) 
	    goto lab22 ; 
	    dodictionary () ; 
	    collectcounttrie () ; 
	    if ( ! moretocome ) 
	    levelnomore [ patdot ] = true ; 
	    lab22: ; 
	  } while ( ! ( patdot == patlen ) ) ; 
	  {register integer for_end; patdot = patlen ; for_end = 0 ; if ( 
	  patdot >= for_end) do 
	    if ( levelnomore [ patdot ] ) 
	    levelnomore [ patdot + 1 ] = true ; 
	  while ( patdot-- > for_end ) ; } 
	} 
      while ( patlen++ < for_end ) ; } 
      deletebadpatterns () ; 
      (void) fprintf( stdout , "%s%ld%s%ld\n",  "total of " , (long)levelpatterncount ,       " patterns at hyph_level " , (long)hyphlevel ) ; 
    } 
  while ( hyphlevel++ < for_end ) ; } 
  outputpatterns ( 1 , 0 ) ; 
  procesp = false ; 
  hyphp = true ; 
  hyphlevel = hyphfinish ; 
  (void) Fputs( stdout ,  "hyphenate word list? " ) ; 
  {
    buf [ 1 ] = getc ( stdin ) ; 
    readln ( stdin ) ; 
  } 
  if ( ( buf [ 1 ] == 'Y' ) || ( buf [ 1 ] == 'y' ) ) 
  dodictionary () ; 
  lab9999: ; 
} 
