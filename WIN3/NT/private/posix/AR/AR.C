/*
      File : ar.c

      Author : C S Palkar

      Function : Front end for 'ar' to be POSIX complient

      Details : The 'ar' command will be invoked it options
                scaned for validity and an equivalent command
                constructed for the native archiver, this
                command is passed to a deamon OR if possible
                execed.
      Limitations : 
         - Will be unable to replace/update OBJ's in LIB's
           since the capability of LIB is unknow with regard
           to this function.
         - Only one OBJ can be extracted at a time reason same
           as above.
         The solution for the above is to construct a batch file
         which will be executed by the deamon or execed. This
         can be implemented at a latter date.

      Date : 22 June 93
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PNULL(p)          (p*)NULL
#define GNULL             PNULL(cmd_cell)
#define SNULL             PNULL(char)

#define OPT_TAG           '-'       /* Option tag */
#define EQU               '='       /* EQU */
#define NL                "\n"      /* EOL */
#define OP_CHAR           '/'      /* native opt tag */
#define SP_CHAR           ' '       /* native arg sep */
#define CNULL             '\0'      /* string terminator */

#define PT_SEP            "/"      /* Path seperator */
#define CWD               "cwd"     /* Delmiter for path list */

#define ST_LIST           "["       /* Trial list start */
#define SP_LIST           "|"       /* Trial list end */
#define ED_LIST           "]"       /* Trial list end */
#define NO_FMTS           2         /* max no of formats */

#define DEL_OPT  'd'  /* delete obj */
#define EXT_OPT  'x'  /* extract obj */
#define ADD_OPT  'r'  /* add obj */
#define TAB_OPT  't'  /* tabulate obj */

#define VER_OPT  'v'  /* verbose obj */
#define IGN_OPT  'c'  /* Ignore err msg */
#define UPD_OPT  'u'  /* Update archive w.r.t time */


#define HOST_CC           "lib32"   /* Host 'C' complier */
#define TMP_ENV           "TMP"     /* Env var for tmp dir */
#define TMP_DIR           "/tmp"    /* Default tmp dir */
#define option(a)   ((a) == OPT_TAG )

#ifdef DEAMON
#define   D_TMP           "tmp.ar"      /* Input of deamon */
#define   D_IN            "devsem.ini"  /* Input of deamon */
#define   D_OUT           "devsem.out"  /* Output of deamon */
#define   D_ERR           "devsem.err"  /* Error of deamon */
#endif

typedef enum { FALSE , TRUE } boolean;
typedef enum { PNUL , DEL , ADD , EXT , TAB } p_flgs;
typedef enum { SNUL , LST , IGN , UPD } s_flgs;

typedef struct
{
   char c_opt;
   int  n_ops;
   p_flgs  flgs;
   char *f_str[ NO_FMTS ];
} p_opts;

typedef struct
{
   char c_opt;
   p_flgs  flgs;
   char *f_str;
} s_opts;

#if DEAMON
typedef struct
{
   char tmp_file[256];
   char out_file[256];
   FILE *de_in;
   FILE *de_out;
   FILE *de_err;
} dm_struct;
#endif

typedef struct dummy
{
   struct dummy *nxt;
   char *token;
        boolean optn;
} cmd_cell, *cmd_ptr;

/*
	Tables which will help translation
*/
p_opts pri_opts[] = {
                      { DEL_OPT , 1 , DEL , { "REMOVE:" , "OUT:" }},
                      { EXT_OPT , 2 , EXT , { "EXTRACT:" , "OUT:" }},
                      { ADD_OPT , 1 , ADD , { SNULL , "OUT:" }},
                      { TAB_OPT , 1 , TAB , { "LIST" , SNULL }},
                      { CNULL   , 0 , PNUL , { SNULL , SNULL }}
                    };

s_opts sec_opts[] = { 
                      { VER_OPT , LST , "VERBOSE" },
                      { IGN_OPT , IGN , SNULL  },
                      { UPD_OPT , UPD , SNULL },
                      { CNULL   , SNUL , SNULL }
                    };

p_opts *G_ptr;
p_flgs p_flag = PNUL;
int    s_cnt;
s_flgs s_flags[3] = { SNUL , SNUL , SNUL };

cmd_cell head_cell , *head_ptr , *end_ptr;

/*

   Function : print_list()

   Discription : Initialise linked list

   Called by : main()

*/

void
#ifdef __STDC__
print_list( cmd_ptr ptr , FILE * fp )
#else
print_list( ptr , fp )
cmd_ptr ptr;
FILE *fp;
#endif
{
   cmd_ptr tmp;
   int i;

   tmp = ptr;
   
   do
   {
      if ( ptr->optn == TRUE )
      {
         switch ( p_flag )
         {
            case DEL : fputc( OP_CHAR , fp );
                       fputs( G_ptr->f_str[0] , fp );
                       fputs(ptr->token,fp);
                       break;
            case EXT : fputc( OP_CHAR , fp );
                       fputs( G_ptr->f_str[0] , fp );
                       fputs(ptr->token,fp);
                       fputs( NL , fp );
            case ADD : fputc( OP_CHAR , fp );
                       fputs( G_ptr->f_str[1] , fp );
                       fputs(ptr->token,fp);
                       break;
            case TAB : fputc( OP_CHAR , fp );
                       fputs( G_ptr->f_str[0] , fp );
                       fputs( NL , fp );
            default  : fputs(ptr->token,fp);
          }
      }
      else
         fputs(ptr->token,fp);
      fputs( NL , fp );
      ptr = ptr->nxt;
   }
   while ( ptr != tmp );

   for ( i = 0 ; i < s_cnt ; i++ )
   {
     if ( s_flags[i] == LST )
     {
         fputc( OP_CHAR , fp );
         fputs( sec_opts->f_str  , fp );
         fputs( NL , fp );
     }
     if ( p_flag == DEL )
     {
         fputc( OP_CHAR , fp );
         fputs( G_ptr->f_str[1] , fp );
         fputs(ptr->nxt->token,fp);
         fputs( NL , fp );
     }
   }
}


#ifdef DEAMON
/*
       Function : deamon()

       Discription : To identify if deamon() is there and open associated
                     files.

       Called by : main()
*/
dm_struct
#ifdef __STDC__
*deamon(char const *temp_env,
        char const *temp_path,
        char const *de_in,
        char const *de_out,
        char const *de_err,
        char const *de_tmp)
#else
*deamon(temp_env, temp_path, de_in, de_out, de_err,de_tmp)
char const *temp_env;
char const *temp_path;
char const *de_in;
char const *de_out;
char const *de_err;
char const *de_tmp;
#endif
{
   static dm_struct dm_st;
   char *tmp_dir, t_buff[256];
   struct stat s_buff;
   int fd;

   if (( tmp_dir = getenv(temp_env)) == ( char * ) NULL )
      return(( dm_struct * ) NULL );

   strcpy( t_buff , tmp_dir );
   strcat( strcat ( t_buff , PT_SEP ) , de_in );
   strcpy( dm_st.out_file , t_buff );

   while ( stat(t_buff,&s_buff) == 0 )  /* Wait for DEAMON to get free */
     sleep(2);

   strcpy( t_buff , tmp_dir );
   strcat( strcat ( t_buff , PT_SEP ) , de_tmp );
   strcpy( dm_st.tmp_file , t_buff );

   if (( fd = creat( t_buff , ( mode_t ) 0777 )) != -1 )
   {
      close(fd);
      if (( dm_st.de_in = fopen( t_buff, "w" )) != ( FILE * ) NULL )
         return( &dm_st );
   }
   return(( dm_struct * ) NULL );

#if 0
   if (( dm_st.de_in = fopen( t_buff , "w" )) == ( FILE * ) NULL )
      return(( dm_struct * ) NULL );

   strcpy( t_buff , tmp_dir );
   strcat( strcat ( t_buff , PT_SEP ) , de_out );

   if (( dm_st.de_out = fopen( t_buff , "r" )) == ( FILE * ) NULL )
      return(( dm_struct * ) NULL );

   strcpy( t_buff , tmp_dir );
   strcat( strcat ( t_buff , PT_SEP ) , de_err );

   if (( dm_st.de_err = fopen( t_buff , "r" )) == ( FILE * ) NULL )
      return(( dm_struct * ) NULL );

   return( &dm_st );
#endif
}

/*
       Function : disp_deamon()

       Discription : To display messages retunred by deamon

       Called by : main()
*/
int
#ifdef __STDC__
disp_deamon(FILE *fderr)
#else
disp_deamon(fderr)
FILE *fderr;
#endif
{
   return 0;
}

/*
       Function : wait_deamon()

       Discription : To wait for deamon to finish job

       Called by : main()
*/
int
#ifdef __STDC__
wait_deamon(char *fdin)
#else
wait_deamon(fdin)
char *fdin;
#endif
{
   FILE *fd;

   while ((fd=fopen(fdin,"r")) != ( FILE *) NULL )
   {
      fclose(fd);
      sleep(1);
   }
   return 0;
}

/*
       Function : writ_deamon()

       Discription : To write to deamon I/P file the new command

       Called by : main()
*/
int
#ifdef __STDC__
writ_deamon(cmd_cell *head_ptr, FILE *fdout)
#else
writ_deamon(head_ptr, fdout)
cmd_cell *head_ptr;
FILE *fdout;
#endif
{
   char buff[64];
   size_t i;

   fprintf(fdout,"%s\n",CWD);
   i = ( size_t ) sizeof( buff );
   if ( getcwd( buff , i ) == CNULL )
      return 0;
   fprintf(fdout,"%s\n",buff);
   print_list( head_ptr , fdout );
   fclose( fdout );
   return 1;
}
#else
/*
       Function : exec_cmd()

       Discription : To exec the new command

       Called by : main()
*/
int
#ifdef __STDC__
exec_cmd(cmd_cell *head_ptr)
#else
exec_cmd(head_ptr)
cmd_cell *head_ptr;
#endif
{
   return 0;
}
#endif

/*
       Function : usage()

       Discription : To identify if if USAGE of command is correct

       Called by : main()
*/
int
#ifdef __STDC__
usage(int argc,char *argv)
#else
usage(argc,argv)
   int argc;
   char *argv;
#endif
{

   p_opts *p_ptr;
   s_opts *s_ptr;

   if ( argc > 1 )
      return 0;

   p_ptr = pri_opts;
   s_ptr = sec_opts;

   fprintf(stderr,"Useage : %s ",argv);

   fputc(OPT_TAG,stderr);
   fputs(ST_LIST,stderr);

   while ( p_ptr->c_opt != CNULL )
   {
      fputc(p_ptr->c_opt,stderr);
      p_ptr++;
      if ( p_ptr->c_opt != CNULL )
         fputs(SP_LIST,stderr);
   }

   fputs(ED_LIST,stderr);
   fputc(SP_CHAR,stderr);
   fputc(OPT_TAG,stderr);
   fputs(ST_LIST,stderr);

   while ( s_ptr->c_opt != CNULL )
   {
      fputc(s_ptr->c_opt,stderr);
      s_ptr++;
   }

   fputs(ED_LIST,stderr);
   fputc(SP_CHAR,stderr);

   fputs("archive [file(s) .... ]",stderr);
   fputs(NL,stderr);
   return 1;
}

/*

   Function : init_list()

   Discription : Initialise linked list

   Called by : main()

*/

cmd_ptr
#ifdef __STDC__
init_list( cmd_ptr cell , char * str )
#else
init_list( cell , str )
cmd_ptr cell;
char *str;
#endif
{
   cell->token = str;
   cell->optn = FALSE;
   cell->nxt = cell;
   end_ptr = cell->nxt;
   return( cell );
}

/*

   Function : del_list()

   Discription : Delete list

   Called by : main()

*/

void
#ifdef __STDC__
del_list( cmd_ptr ptr )
#else
del_list( ptr )
cmd_ptr ptr;
#endif
{
   cmd_ptr tmp, tmp2;

   if ( ptr == ptr->nxt )
      return;

   tmp = ptr;
   ptr = ptr->nxt;
   
   do
   {
      tmp2 = ptr->nxt;
      free( ptr );
      ptr = tmp2;
   }
   while ( ptr != tmp );
}


/*

   Function : new_cell()

   Discription : Create new node

   Called by : main()

*/

cmd_ptr
#ifdef __STDC__
new_cell( char *str, boolean optn )
#else
new_cell( str , optn )
char *str;
boolean optn;
#endif
{
   cmd_ptr ptr;

   if (( ptr = (cmd_ptr)malloc(sizeof(cmd_cell))) == (cmd_ptr) NULL )
      return (( cmd_ptr ) NULL );
   ptr->token = str;
   ptr->optn  = optn;
   return ptr;
}

/*

   Function : add_list()

   Discription : Add at current pos

   Called by : main()

*/

cmd_ptr
#ifdef __STDC__
add_list( cmd_ptr ptr , cmd_ptr node )
#else
add_list( ptr , node )
cmd_ptr ptr, node;
#endif
{
   node->nxt = ptr->nxt;
   ptr->nxt = node;
   return node;
}

/*

   Function : invalid_opts()

   Discription : Add at current pos

   Called by : main()

*/

int
#ifdef __STDC__
invalid_opts(char *str)
#else
invalid_opts(str)
   char * str;
#endif
{
  p_opts *p_ptr;
  s_opts *s_ptr;

  if ( *str == CNULL )
     return 0;

  if ( p_flag == PNUL )
  {
     p_ptr = pri_opts;
     while ( p_ptr->c_opt != CNULL )
     {
       if ( p_ptr->c_opt == *str )
       {
          p_flag = p_ptr->flgs;
          G_ptr = p_ptr;
          break;
       }
       else
          p_ptr++;
     }
     if ( p_flag == PNUL )
        return 1;
     str++;
  }

  while ( *str != CNULL )
  {
     s_ptr = sec_opts;
     while ( s_ptr->c_opt != CNULL )
     {
       if ( s_ptr->c_opt == *str )
       {
          s_flags[s_cnt++] = s_ptr->flgs;
          break;
       }
       else
          s_ptr++;
     }
     if ( s_ptr->c_opt == CNULL && !s_cnt )
       return 1;
     str++;
   }
   return 0;
}
/*----------------------------------------------------------------------*/
int
#ifdef __STDC__
main(int argc, char **argv)
#else
main(argc,argv)
   int argc;
   char ** argv;
#endif
{
#ifdef DEAMON
   dm_struct *dm_ptr;
#endif
   cmd_ptr tmp_ptr;
   boolean bull;
   int i;

   if ( usage(argc,argv[0]))
      return 0;
   i = 1;
   while ( i < argc && option(*argv[i]))
     if ( invalid_opts(argv[i]+1))
        return usage(1,argv[0]);
     else
        i++;
        
#if DEAMON
   if((dm_ptr=deamon(TMP_ENV,TMP_DIR,D_IN,D_OUT,D_ERR,D_TMP))==(dm_struct*)NULL)
   {
      fprintf(stderr,"Cannot execute %s deamon missing\n",*argv[0]);
      return 0;
   }
#endif

   head_ptr = init_list( &head_cell , HOST_CC );
   
   bull = p_flag == ADD || p_flag == TAB;
   if (( tmp_ptr = new_cell( argv[i] , bull )) == GNULL )
   {
       fprintf(stderr,"ERROR:Not enough memory.\n");
       return 0;
   }
  
   end_ptr = add_list ( end_ptr , tmp_ptr );
   i++;
   bull = p_flag == DEL || p_flag == EXT;

   while ( i < argc )
   {
      if (( tmp_ptr = new_cell( argv[i] , bull )) == GNULL )
      {
          fprintf(stderr,"ERROR:Not enough memory.\n");
          return 0;
      }
      end_ptr = add_list ( end_ptr , tmp_ptr );
      i++;
   }

#if DEAMON
   if ( !writ_deamon( head_ptr , dm_ptr->de_in ))
   {
      fprintf(stderr,"Unable to communicate with DEAMON. EXITING!\n");
      return 0;
   }
   rename( dm_ptr->tmp_file, dm_ptr->out_file );
   wait_deamon( dm_ptr->out_file );
   disp_deamon( dm_ptr->de_err );
#else
   exec_cmd( head_ptr );
#endif
   del_list( head_ptr );
   return 0;
}
