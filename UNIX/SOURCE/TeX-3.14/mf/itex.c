#define EXTERN extern
#include "texd.h"

#ifdef INIMF
boolean getstringsstarted ( ) 
{/* 30 10 */ register boolean Result; unsigned char k, l  ; 
  ASCIIcode m, n  ; 
  strnumber g  ; 
  integer a  ; 
  boolean c  ; 
  poolptr = 0 ; 
  strptr = 0 ; 
  maxpoolptr = 0 ; 
  maxstrptr = 0 ; 
  strstart [ 0 ] = 0 ; 
  {register integer for_end; k = 0 ; for_end = 255 ; if ( k <= for_end) do 
    {
      if ( ( ( k < 32 ) || ( k > 126 ) ) ) 
      {
	{
	  strpool [ poolptr ] = 94 ; 
	  incr ( poolptr ) ; 
	} 
	{
	  strpool [ poolptr ] = 94 ; 
	  incr ( poolptr ) ; 
	} 
	if ( k < 64 ) 
	{
	  strpool [ poolptr ] = k + 64 ; 
	  incr ( poolptr ) ; 
	} 
	else if ( k < 128 ) 
	{
	  strpool [ poolptr ] = k - 64 ; 
	  incr ( poolptr ) ; 
	} 
	else {
	    
	  l = k / 16 ; 
	  if ( l < 10 ) 
	  {
	    strpool [ poolptr ] = l + 48 ; 
	    incr ( poolptr ) ; 
	  } 
	  else {
	      
	    strpool [ poolptr ] = l + 87 ; 
	    incr ( poolptr ) ; 
	  } 
	  l = k % 16 ; 
	  if ( l < 10 ) 
	  {
	    strpool [ poolptr ] = l + 48 ; 
	    incr ( poolptr ) ; 
	  } 
	  else {
	      
	    strpool [ poolptr ] = l + 87 ; 
	    incr ( poolptr ) ; 
	  } 
	} 
      } 
      else {
	  
	strpool [ poolptr ] = k ; 
	incr ( poolptr ) ; 
      } 
      g = makestring () ; 
      strref [ g ] = 127 ; 
    } 
  while ( k++ < for_end ) ; } 
  vstrcpy ( nameoffile + 1 , poolname ) ; 
  nameoffile [ 0 ] = ' ' ; 
  nameoffile [ strlen ( poolname ) + 1 ] = ' ' ; 
  if ( aopenin ( poolfile , MFPOOLPATH ) ) 
  {
    c = false ; 
    do {
	{ 
	if ( eof ( poolfile ) ) 
	{
	  ; 
	  (void) fprintf( stdout , "%s\n",  "! mf.pool has no check sum." ) ; 
	  aclose ( poolfile ) ; 
	  Result = false ; 
	  goto lab10 ; 
	} 
	read ( poolfile , m ) ; 
	read ( poolfile , n ) ; 
	if ( m == '*' ) 
	{
	  a = 0 ; 
	  k = 1 ; 
	  while ( true ) {
	      
	    if ( ( xord [ n ] < 48 ) || ( xord [ n ] > 57 ) ) 
	    {
	      ; 
	      (void) fprintf( stdout , "%s\n",                "! mf.pool check sum doesn't have nine digits." ) ; 
	      aclose ( poolfile ) ; 
	      Result = false ; 
	      goto lab10 ; 
	    } 
	    a = 10 * a + xord [ n ] - 48 ; 
	    if ( k == 9 ) 
	    goto lab30 ; 
	    incr ( k ) ; 
	    read ( poolfile , n ) ; 
	  } 
	  lab30: if ( a != 426472440L ) 
	  {
	    ; 
	    (void) fprintf( stdout , "%s\n",  "! mf.pool doesn't match; tangle me again." ) ; 
	    aclose ( poolfile ) ; 
	    Result = false ; 
	    goto lab10 ; 
	  } 
	  c = true ; 
	} 
	else {
	    
	  if ( ( xord [ m ] < 48 ) || ( xord [ m ] > 57 ) || ( xord [ n ] < 48 
	  ) || ( xord [ n ] > 57 ) ) 
	  {
	    ; 
	    (void) fprintf( stdout , "%s\n",  "! mf.pool line doesn't begin with two digits."             ) ; 
	    aclose ( poolfile ) ; 
	    Result = false ; 
	    goto lab10 ; 
	  } 
	  l = xord [ m ] * 10 + xord [ n ] - 48 * 11 ; 
	  if ( poolptr + l + stringvacancies > poolsize ) 
	  {
	    ; 
	    (void) fprintf( stdout , "%s\n",  "! You have to increase POOLSIZE." ) ; 
	    aclose ( poolfile ) ; 
	    Result = false ; 
	    goto lab10 ; 
	  } 
	  {register integer for_end; k = 1 ; for_end = l ; if ( k <= for_end) 
	  do 
	    {
	      if ( eoln ( poolfile ) ) 
	      m = ' ' ; 
	      else read ( poolfile , m ) ; 
	      {
		strpool [ poolptr ] = xord [ m ] ; 
		incr ( poolptr ) ; 
	      } 
	    } 
	  while ( k++ < for_end ) ; } 
	  readln ( poolfile ) ; 
	  g = makestring () ; 
	  strref [ g ] = 127 ; 
	} 
      } 
    } while ( ! ( c ) ) ; 
    aclose ( poolfile ) ; 
    Result = true ; 
  } 
  else {
      
    ; 
    (void) fprintf( stdout , "%s\n",  "! I can't read mf.pool." ) ; 
    aclose ( poolfile ) ; 
    Result = false ; 
    goto lab10 ; 
  } 
  lab10: ; 
  return(Result) ; 
} 
#endif /* INIMF */
#ifdef INIMF
void sortavail ( ) 
{halfword p, q, r  ; 
  halfword oldrover  ; 
  p = getnode ( 1073741824L ) ; 
  p = mem [ rover + 1 ] .hh .v.RH ; 
  mem [ rover + 1 ] .hh .v.RH = 262143L ; 
  oldrover = rover ; 
  while ( p != oldrover ) if ( p < rover ) 
  {
    q = p ; 
    p = mem [ q + 1 ] .hh .v.RH ; 
    mem [ q + 1 ] .hh .v.RH = rover ; 
    rover = q ; 
  } 
  else {
      
    q = rover ; 
    while ( mem [ q + 1 ] .hh .v.RH < p ) q = mem [ q + 1 ] .hh .v.RH ; 
    r = mem [ p + 1 ] .hh .v.RH ; 
    mem [ p + 1 ] .hh .v.RH = mem [ q + 1 ] .hh .v.RH ; 
    mem [ q + 1 ] .hh .v.RH = p ; 
    p = r ; 
  } 
  p = rover ; 
  while ( mem [ p + 1 ] .hh .v.RH != 262143L ) {
      
    mem [ mem [ p + 1 ] .hh .v.RH + 1 ] .hh .v.LH = p ; 
    p = mem [ p + 1 ] .hh .v.RH ; 
  } 
  mem [ p + 1 ] .hh .v.RH = rover ; 
  mem [ rover + 1 ] .hh .v.LH = p ; 
} 
#endif /* INIMF */
#ifdef INIMF
void primitive ( strnumber s , halfword c , halfword o ) 
{poolpointer k  ; 
  smallnumber j  ; 
  smallnumber l  ; 
  k = strstart [ s ] ; 
  l = strstart [ s + 1 ] - k ; 
  {register integer for_end; j = 0 ; for_end = l - 1 ; if ( j <= for_end) do 
    buffer [ j ] = strpool [ k + j ] ; 
  while ( j++ < for_end ) ; } 
  cursym = idlookup ( 0 , l ) ; 
  if ( s >= 256 ) 
  {
    flushstring ( strptr - 1 ) ; 
    hash [ cursym ] .v.RH = s ; 
  } 
  eqtb [ cursym ] .v.LH = c ; 
  eqtb [ cursym ] .v.RH = o ; 
} 
#endif /* INIMF */
