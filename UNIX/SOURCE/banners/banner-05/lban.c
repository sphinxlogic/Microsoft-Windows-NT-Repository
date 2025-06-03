/* Large Korean Banner */

#include <stdio.h>

#define TOTAL 37

#define CW   6   /* character width */
#define CL   3   /* character length */ 

#define SD   2   /* shift down */ 
#define SR   3   /* shift right */ 
#define SB   1   /* shift below */

#define MC   6  /* max characters */

#define DFName "/ufo2/fcode/e880505/Bin/lban.dat"	/* data file name */

static char alpha[]={"abcdefghijklmnopqrstuvwxyzQWERTOP?!.,"};
static char c[]={"acdefgqrstvwxzQWERT"};  /* consonant */
static char v1[]={"ijklopuOP"}; 
static char v2[]={"bhmny"};
static char s[]={"?!.,"};

struct FONT  {
	int   address;
	short type;
	}  a[TOTAL];  

int  b[MC*4];    /*  board  */
int  t[MC*4];    /*  taget  */

char board[2*CL+3][(CW*2-1)*MC+3]; 
char *receiver;

short  word_counter = 0;

main(argc,argv)
int argc;
char *argv[];
{ 
	int number ; 
	
	if( argc != 2 && argc !=3 ) {Error("?"); Quit();} 
	if(argc == 3 )  receiver = argv[2];
	make_font(); 
	clear_board();
	number=make_board(argv[1]);
	load_board(number);
	print_board(argc-2);
	fflush(stdout);
}
clear_board()
{
	int i,j;

	for(i=0;i<2*CL+3; i++ ){
		for( j=0; j< (CW*2-1)*MC; j++)
			board[i][j] = ' ';
		board[i][j] = 0;
	}
}

contractor( index ,num)
int index;
int num;
{
	int i,j;
	int what, mwhat;
			
	switch( t[index] ){
		case 3:
			return 0;
			break;
		case 1: 
			if( index > 0 ) {
					if( t[index-1] ==2 ) return 2;
					else return 1;
					}
			else return 1;
			break;
		case 2:
			return 3;
			break;
		case 0:
			if(index==0 ) return 0;
			if( index <num-1 ) { 
				what = contractor(index+1, num );
				if( what ==1 || what ==2 || what == 3 ) return 0;
				if( what == 0 ) {
					mwhat = t[index-1] ;
					if( mwhat== 1 ) {
						mwhat = contractor(index-1, num );
						if( mwhat== 1) return 8;
						if(mwhat == 2) return 9;
						}
					if( mwhat == 2) {
							return 6;
						}
					if( mwhat== 0) {
						mwhat = contractor( index-2);
						if(mwhat == 1) return 5;
						if(mwhat == 2) return 7;
						if(mwhat == 3) return 7;
						}
					} /* if */
				  else{ 
					mwhat = contractor( index-1, num );
					if(mwhat == 1 ) return 4;
					if(mwhat == 2 ) return 6;
					if(mwhat == 3 ) return 6;
					}
				}	/*  if */
			else { 
				what = t[index-1];
				if(what == 0) {
					mwhat = contractor(index-2, num);
					if( mwhat == 3 )  return 7;
					if( mwhat == 1 ) return 5;
					if(mwhat ==2 ) return 7;
					}
				what = contractor( index-1 , num );
				if( what == 3 ) return 6;
				if( what == 1 ) return 8;
				if( what == 2 ) return 9;
				}
		break;
		} /* swtich */
} /* end of procedure */



load_board( num )
int num;
{
	int i;
	int x=0 ,y=0 ;    /* location of a charcater */
	int tx, ty ;   /* location of a alphabet */ 
	short  what;
	
	for( i=0; i< num ; i++ ){ 
		what = contractor(i,num); 
		switch( what ){
			case 0:
				if(i!=0) { x+=CW*2-1 ;  }
				tx=x; ty = y;
				word_counter++;
				if( word_counter > MC ) {Error("Too many characters");Quit();}
				break;
			case 1:
				tx = x+CW ;
				ty=y;
				break;
			case 2: 
				tx= x+CW;
				ty= y+SB ;
				break; 
			case 3: 
				tx = x; 
				ty = y+CL ;
				break;
			case 4:
				tx = x ;
				ty = y+CL ;
				break; 
			case 5:
				tx= x+CW ;
				ty = y+CL;
				break;
			case 6:
				tx = x;
				ty = y+CL+SD ;
				break;
			case 7:
				tx = x+CW;
				ty = y+CL+SD;
				break;
			case 8:
				tx = x+ SR;
				ty = y+CL;
				break;
			case 9:
				tx = x +SR;
				ty= y+CL+SD;
				break;
			default:
				break;
				}  /* switch */
		sculpture( tx,ty, b[i] );
		}   /*  for */
}   /*  end of function */	

sculpture( x, y, index )
int x,y, index;
{
	FILE *fptr;
	char str[20],len;
	int i,j;

	if( NULL == (fptr=fopen(DFName,"r"))) { Error("!"); Quit();}
	fseek(fptr, (long)(a[index].address), 0 );
	for(i=0; i< CL ; i++ ){
		fgets( str, CW+5, fptr);
		len = strlen( str );
		str[ len-1] =' '; len--;
		for(j=0 ; j <len ; j++ ){
			board[y+i][x+j] = str[j] ; 
		}  /*  for */
	}  /* for */
	fclose(fptr);
}
compress(str)
char *str;
{
	int i,len ;
	len = strlen(str);
	for(i=0; i<len ; i++ )
		if( str[i] != ' ')  return 1;
	return 0;
}


print_board(mode)
short mode;
{
	int i,  limit;

	for(i=0; i < CL*2+3 ; i++ )
		if( compress(board[i]) ) limit=i;

	board_optimize( limit );
	board_compress( limit );
		
	if(mode == 0 ) {
puts("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
		for(i=0 ; i <= limit ; i++ ){
					puts( board[i]);
					fflush(stdout);
					}
puts("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	fflush(stdout);
				}
	else if( mode==1 )   for(i=0 ; i <= limit ; i++ )
							printf("%s:%s\n",receiver,board[i]);
}

board_compress( max )
int max;
{
	int i,j;
	int end;
	int sw = 0;
	char tablet[CL*3+2];

	end = strlen(board[0]);

	for(i=0 ; i<end-1 ; i++ ){
		for(j=0; j<= max ; j++ ) tablet[j]= board[j][i];
		tablet[j] = 0;
		if( strClen(tablet) ==0 ){ 
			sw++; 
			if (sw==3){
				exec_compress(i,max,end);
				sw--;
				end--;
				i--;  } /* if */
				}  /* if */
		else sw=0;
		}  /* for i */
}

exec_compress( i, max, end)
int i,max,end;
{
	int j,k;

	for(j=0 ; j<=max ; j++ ){
		for(k=i; k<end-1; k++ )
			board[j][k]=board[j][k+1];
		board[j][k] = 0 ;
		}
}
strClen(str)
char *str;
{
	int len, i;
	len=strlen(str);
	for(i=0 ; i<len ; i++)
		if(str[i]!=' ') return 1;
	return 0;
}
board_optimize( max )
int max;
{
	int i,j,k;
	int local_max;
	int delta ;

	for(i=0; i< word_counter ; i++ ){
		local_max= local_scan(i,max);
		if( local_max <= max-2) {
			delta = (max-local_max)/2 ;
			for( j=i*(CW*2-1); j< (i+1)*(CW*2-1) ; j++) 
				for(k=local_max ; k>=0 ; k--){
					board[k+delta][j] = board[k][j];
					board[k][j]=' ';
					}  /*  for k */
				} /*  if  */
		} /* for i */
}

local_scan(i,max)
int i;
int max;
{
	int j,k;
	int local=0;

	for( j=i*(CW*2-1); j< (i+1)*(CW*2-1) ; j++) 
		for(k=max; k>0 ; k--) 
			if( board[k][j] != ' ') { if(k>local) local=k ; break; }
	return local;
}
		
make_board(str)
char *str;
{
	int i,len,defect=0 ;

	len = strlen(str); 
	
	for(i=0; i<len ; i++ ) { 
		b[i+defect] = where( str[i], alpha );
		if( b[i+defect] == -1 ) defect--;
		else t[i+defect] = a[b[i+defect]].type;
		}
	return i+defect; 
}
make_font()
{
	int i;
	for(i=0; i<TOTAL ; i++ ) { 
		a[i].type = exist( alpha[i] );
		a[i].address = i*(CL*(CW+1)+1) ;
		}
}
	
exist( ch )
{
	if( where(ch, c ) >-1 )  return 0;
	else if( where( ch, v1) > -1 ) return 1;
	else if( where(ch, v2) >-1 ) return 2;
	return 3;
}
where( ch, str )
char ch;
char *str;
{
	int i, len;

	len=strlen(str);
	for(i=0; i<len ; i++ ) 
		if( ch == str[i] )  return i;
	return -1;
}
Error(str)
char *str;
{
}
Quit()
{
	puts("-------");
	exit(0);
}


