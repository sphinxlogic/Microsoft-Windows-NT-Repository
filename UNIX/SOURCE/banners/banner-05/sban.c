
/* Small Korean Banner */

#include <stdio.h>

#define TOTAL 60
#define BufSize 255

#define CW   2   /* character width */
#define CL   2   /* character length */ 

#define SD   1   /* shift down */ 
#define SR   1   /* shift right */ 
#define SB   1   /* shift below */

#define MC   15  /* max characters */

#define DFName 	"/ufo2/fcode/e880505/Bin/sban.dat"	/* data file name */

static char alpha[]={"abcdefghijklmnopqrstuvwxyzQWERTOP?!.,:_AZSXDCFVGB1234567890"};
static char c[]={"acdefgqrstvwxzQWERT"};  /* consonant */
static char v1[]={"ijklopuOP"}; 
static char v2[]={"bhny"};
static char v3[]={"m"};
static char s[]={"?!.,:_AZSXDCFVGB1234567890"};

struct FONT  {
	int   address;
	short type;
	}  a[TOTAL];  

int  b[BufSize];    /*  board  */
int  t[BufSize];    /*  taget  */

char board[2*CL+3][(CW*2+1)*MC+3]; 
char *receiver;   /*  special receiver  */
char *message;

short  word_counter ;

main(argc,argv)
int argc;
char *argv[];
{ 
	int number , station = 0; 
	
	if( argc != 2 && argc !=3 ) {Error(":)"); Quit();} 
	if(argc == 3 )  receiver = argv[2];
	make_font(); 
	message = argv[1];
	number=make_board(message);
	while(1){
		word_counter=0;
		clear_board();
		station=load_board(station,number);
		print_board(argc-2);
		fflush(stdout);
		if(station == number) break;
	}
}
clear_board()
{
	int i,j;

	for(i=0;i<2*CL+3; i++ ){
		for( j=0; j< (CW*2+1)*MC; j++)
			board[i][j] = ' ';
		board[i][j] = 0;
	}
}

contractor( index ,num)
int index;
int num;
{
	int i,j;
	int what, mwhat, twhat;
			
	switch( t[index] ){
		case 4:   /** symbolic  */
			return 0;
			break;
		case 1:    /* */
			if( index > 0 ) {
					if( t[index-1] ==2 ) return 2;
					else if( t[index-1] == 3) return 11;
					else return 1;
					}
			else return 1;
			break;
		case 2:   /** velar */ 
			return 3;
			break;
		case 3:   /*  case of m */
			return 10;
			break;
		case 0:   /*  consonant */
			if(index==0 ) return 0;
			if( index <num-1 ) { 
				what = contractor(index+1, num );
				if( what==1||what==2||what==3||what==10||what==11) return 0;
				if( what == 0 ) {
					twhat = t[index-1] ;
					if( twhat== 1 ) {
						mwhat = contractor(index-1, num );
						if( mwhat== 1) return 8;
						if(mwhat == 2) return 9;
						if( mwhat == 11 ) return 14;
						}
					if( twhat == 2) {
							return 6;
						}
					if( twhat == 3) {
						return 12;
						}
					if( twhat== 0) {
						mwhat = contractor( index-2);
						if(mwhat == 1) return 5;
						if(mwhat == 11) return 13;
						if(mwhat == 2) return 7;
						if(mwhat == 3) return 7;
						if(mwhat == 10) return 13;
						}
					} /* if */
				  else{ 
					mwhat = contractor( index-1, num );
					if(mwhat == 1 ) return 4;
					if(mwhat == 11) return 12;
					if(mwhat == 2 ) return 6;
					if(mwhat == 3 ) return 6;
					if(mwhat == 10) return 12;
					}
				}	/*  if */
			else { 
				twhat = t[index-1];
				if(twhat == 0) {
					mwhat = contractor(index-2, num);
					if( mwhat == 3 )  return 7;
					if( mwhat == 10) return 13;
					if( mwhat == 1 ) return 5;
					if( mwhat == 11) return 13;
					if(mwhat ==2 ) return 7;
					}
				mwhat = contractor( index-1 , num );
				if( mwhat == 3 ) return 6;
				if( mwhat == 10) return 12;
				if( mwhat == 1 ) return 8;
				if( mwhat == 11) return 14;
				if( mwhat == 2 ) return 9;
				}
		break;
		} /* swtich */
} /* end of procedure */



load_board(stop, num )
int stop;
int num;
{
	int i;
	int x=0 ,y=0 ;    /* location of a charcater */
	int tx, ty ;   /* location of a alphabet */ 
	short  what;
	
	for( i= stop ; i< num ; i++ ){ 
		what = contractor(i,num); 
		switch( what ){
			case 0:
				if(i!=stop) { x+=CW*2+1 ;  }
				tx=x; ty = y;
				word_counter++;
				if( word_counter > MC )  return i;
				break;
			case 1:
				tx = x+CW ;
				ty=y;
				break;
			case 2: 
			case 11:
				tx= x+CW;
				ty= y+SB ;
				break; 
			case 3: 
				tx = x; 
				ty = y+CL ;
				break;
			case 10:
				tx = x;
				ty = y+CL-1;
				break;
			case 4:
				tx = x ;
				ty = y+CL ;
				break; 
			case 12:
				tx=x;
				ty = y+CL+1;
				break;
			case 5:
				tx= x+CW ;
				ty = y+CL;
				break;
			case 13:
				tx= x+CW ;
				ty = y+CL+1;
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
			case 14:
				tx = x+ SR;
				ty = y+CL+1;
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
		return i;
}   /*  end of function */	

sculpture( x, y, index )
int x,y, index;
{
	FILE *fptr;
	char str[20],len;
	int i,j;

	if( NULL == (fptr=fopen(DFName,"r"))) { Error("8)"); Quit();}
	fseek(fptr, (long)(a[index].address), 0 );
	for(i=0; i< CL ; i++ ){
		fgets( str, CW+5, fptr);
		len = strlen( str );
		str[ len-1] =' '; len--;
		for(j=0 ; j <len ; j++ ){
			if( str[j] != ' ') board[y+i][x+j] = str[j] ; 
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
	space_check( limit );
		
	if(mode == 0 ) {
		for(i=0 ; i <= limit ; i++ ){
					puts( board[i]);
					fflush(stdout);
					}
	fflush(stdout);
				}
	else if( mode==1 )   for(i=0 ; i <= limit ; i++ )
							printf("%s:%s\n",receiver,board[i]);
}

space_check(max)
{
	int i,j;

	for(i=0;i<=max ; i++ )
		for( j=0; j< (CW*2+1)*MC-1; j++)
			if(board[i][j] == 'A')  board[i][j]=' ';
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
			for( j=i*(CW*2+1); j< (i+1)*(CW*2+1) ; j++) 
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

	for( j=i*(CW*2+1); j< (i+1)*(CW*2+1) ; j++) 
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
	else if( where(ch, v3) >-1 ) return 3;
	return 4;
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
	puts(str);
}
Quit()
{
	exit(0);
}

