/*
      File : ld.c

      Author : C S Palkar

      Function : Front end for 'ld' to be POSIX complient

      Details : The 'ld' command will be invoked it options
                scaned for validity and an equivalent command
                constructed for the native link editor, this
                command is passed to a deamon OR if possible
                execed.

      Date : 23 June 93
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PNULL(p)          (p*)NULL
#define RNULL             PNULL(rev_cell)
#define NNULL             PNULL(nat_cell)
#define GNULL             PNULL(gnu_cell)
#define SNULL             PNULL(char)

#define OPT_TAG           '-'       /* Option tag */
#define EQU               '='       /* EQU */
#define NL                "\n"      /* EOL */
#define OP_CHAR           '/'      /* native opt tag */
#define SP_CHAR           ' '       /* native arg sep */
#define CNULL             '\0'      /* string terminator */

#define PT_SEP            "/"      /* Path seperator */

#define ST_LIST           "["       /* Trial list start */
#define SP_LIST           "|"       /* Trial list end */
#define ED_LIST           "]"       /* Trial list end */

#define LIB_PPATH          "LIB" /* Lib path env var */
#define NO_LINK           "c"       /* Opt for no lnk */

#define CWD               "cwd"     /* Delmiter for path list */
#define HOST_CC           "link"   /* Host 'C' complier */
#define TMP_ENV           "TMP"     /* Env var for tmp dir */
#define TMP_DIR           "/tmp"    /* Default tmp dir */
#define option(a)   ((a) == OPT_TAG )

#ifdef DEAMON
#define   D_TMP           "tmp.ld"      /* Input of deamon */
#define   D_IN            "devsem.ini"  /* Input of deamon */
#define   D_OUT           "devsem.out"  /* Output of deamon */
#define   D_ERR           "devsem.err"  /* Error of deamon */
#endif

typedef enum { FALSE , TRUE } boolean;

typedef enum                 /* trail list flag */
{
   NO,                  /* No trail list */
   REV,                 /* Reserved list */
   OPEN,                /* Open     list */
   LIBN,                /* LIBNAME       */
   PPATH                 /* lib path      */
} t_info;

typedef struct               /* Trail list name and it native equivalent */
{
   char const *gnu_name;
   char const *nat_name;
} rev_cell , rev_list[];

typedef struct nat_dummy
{
   char const *nat_name;           /* Native option name */
   struct nat_dummy *after_ptr;    /* After pos for order */
   rev_cell *tran_list;            /* Ptr to list of reserved opts */
} nat_cell, nat_list[];

typedef struct 
{
   char const *opt_name;        /* Option name */
   t_info     trail_info;       /* Trail flag */
   rev_cell   *rev_lst;         /* Ptr to trail info */
   char const *opn_lst;         /* Disc list for Usage */
   nat_cell   *nat_ptr;         /* Ptr to equiv native option */
} gnu_cell, gnu_list[];

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
rev_list m_rev_lst =
{
	{ SNULL , SNULL }
};

nat_list nat_lst =
{
	{ "ALIGN"    , NNULL , RNULL },      /* 0 */
	{ "BASE"   , NNULL , RNULL },      /* 1 */
	{ "DEBUG"    , NNULL , RNULL },      /* 2 */
	{ "DEBUGTYPE"   , NNULL , RNULL },      /* 3 */
	{ "DLL"   , NNULL , RNULL },      /* 4 */
	{ "ENTRY"    , NNULL , RNULL },      /* 5 */
	{ "FIXED"   , NNULL , RNULL },      /* 6 */
	{ "FORCE"   , NNULL , RNULL },      /* 7 */
	{ "GPSIZE"    , NNULL , RNULL },      /* 8 */
	{ "HEAP"  , NNULL , RNULL },      /* 9 */
	{ "INCLUDE"   , NNULL , RNULL },      /* 10 */
	{ "MACHINE"   , NNULL , RNULL },      /* 11 */
	{ "MAP"   , NNULL , RNULL },      /* 12 */
	{ "OUT" , NNULL , RNULL },      /* 13 */
	{ "ROM"  , NNULL , RNULL },      /* 14 */
	{ "SECTION"   , NNULL , RNULL },      /* 15 */
	{ "STACK"    , NNULL , RNULL },      /* 16 */
	{ "SUBSYSTEM"    , NNULL , RNULL },      /* 17 */
	{ "VERBOSE"    , NNULL , RNULL },      /* 18 */
	{ "VERSION"    , NNULL , RNULL },      /* 19 */
	{ SNULL , NNULL , RNULL }
};

gnu_list gnu_lst =
{
	{ "a"           , NO , RNULL , SNULL ,  nat_lst + 6 },
	{ "e"           , OPEN , RNULL , "<entry-point>" ,  nat_lst + 5 },
	{ "f"           , OPEN , RNULL , "<pattern>" ,  NNULL },
	{ "l"           , LIBN , RNULL , "<lib-name>" ,  nat_lst },
	{ "m"           , NO , RNULL , SNULL ,  NNULL },
	{ "o"           , OPEN , RNULL , "<file>" ,  nat_lst + 13 },
	{ "r"           , NO , RNULL , SNULL ,  NNULL },
	{ "s"           , NO , RNULL , SNULL ,  NNULL },
	{ "t"           , NO , RNULL , SNULL ,  NNULL },
	{ "u"           , OPEN , RNULL , "<symbol>" ,  NNULL },
	{ "x"           , NO , RNULL , SNULL ,  NNULL },
	{ "z"           , NO , RNULL , SNULL ,  NNULL },
	{ "L"           , PPATH , RNULL , "<dir-path>" ,  nat_lst },
	{ "M"           , NO , RNULL , SNULL ,  NNULL },
	{ "N"           , NO , RNULL , SNULL ,  NNULL },
	{ "V"           , NO , RNULL , SNULL ,  NNULL },
	{ "VS"          , OPEN , RNULL , "<ver-num>" ,  nat_lst + 19 },
	{ SNULL         , NO , RNULL , SNULL , NNULL }
};

cmd_cell head_cell , *head_ptr , *end_ptr;
cmd_cell path_cell , *path_ptr , *pend_ptr;
int no_path = 1;

char *link_opts[] = /* Default linker options */
{
   "subsystem:posix",
   "entry:__PosixProcessStartup",
   SNULL
};

char *def_lib[] = /* Default library */
{
   "libcpsx.lib",
   "psxdll.lib",
   "ntdll.lib",
   "psxrtl.lib",
   SNULL
};

/*
       Function : display_rev_lst()

       Discription : To display reserved word(s) with option

       Called by : usage()
*/
void
#ifdef __STDC__
display_rev_lst( rev_list *rev_lst )
#else
display_rev_lst( rev_lst )
rev_cell *rev_lst;
#endif
{
   fputs(ST_LIST,stderr);

   while ( rev_lst->gnu_name != ( char const * ) NULL )
   {
      fputs(rev_lst->gnu_name,stderr);
      fputs(SP_LIST,stderr);
      fputs(NL,stderr);
      rev_lst++;
   }
}

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

   tmp = ptr;
   
   do
   {
      if ( ptr->optn == TRUE )
         fputc( OP_CHAR , fp );
      fputs(ptr->token,fp);
      fputs( NL , fp );
      ptr = ptr->nxt;
   }
   while ( ptr != tmp );
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
        char const *de_err)
#else
*deamon(temp_env, temp_path, de_in, de_out, de_err,de_tmp)
char const *temp_env;
char const *temp_path;
char const *de_in;
char const *de_out;
char const *de_err;
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
   int i;

   if ( !no_path )
   {
     fputs(LIB_PPATH,fdout);
     fputs( NL , fdout );
     print_list( path_ptr , fdout );
   }

   fprintf(fdout,"%s\n",CWD);

   i =  sizeof( buff );
   if ( getcwd( buff , ( size_t )i ) == CNULL )
      return 0;
   fprintf(fdout,"%s\n",buff);
   print_list( head_ptr , fdout );

   for ( i = 0 ; link_opts[i] != SNULL ; i++ )
   {
      fputc( OP_CHAR , fdout );
      fputs(link_opts[i],fdout);
      fputs( NL , fdout );
   }

   for ( i = 0 ; def_lib[i] != SNULL ; i++ )
   {
      fputs(def_lib[i],fdout);
      fputs( NL , fdout );
   }
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
usage(int argc, char *argv)
#else
usage(argc,argv)
   int argc;
   char *argv;
#endif
{
   gnu_cell *gnu_ptr;
   int new_opt;

   new_opt = TRUE;
   gnu_ptr = gnu_lst;

   if ( argc > 1 )
      return 0;

   fprintf(stderr,"\nUseage : %s ",argv);

   while ( gnu_ptr->opt_name != ( char const * ) NULL )
   {
      if ( new_opt == TRUE )
      {
         fputc(OPT_TAG,stderr);
         new_opt = FALSE;
      }
      fputs(gnu_ptr->opt_name,stderr);
      switch ( gnu_ptr->trail_info )
      {
         case NO   : new_opt = TRUE;
                break;
         case REV  : display_rev_lst( gnu_ptr->rev_lst );
                new_opt = TRUE;
                break;
         case LIBN :
         case PPATH :
         case OPEN : fputs(gnu_ptr->opn_lst,stderr);
                new_opt = TRUE;
                break;
         default   : fprintf(stderr,"ERROR : Internal error \n");
                break;
      }
      fputc(SP_CHAR,stderr);
      gnu_ptr++;
   }
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
init_list( cell , str , end_ptr )
cmd_ptr cell;
char *str; 
#endif
{
   cell->token = str;
   cell->optn = FALSE;
   cell->nxt = cell;
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

   Function : equiv_name()

   Discription : Searchs for equivalent name for native
            and returns pointer to base.

   Called by : process_opt()

*/

char
#ifdef __STDC__
*equiv_name( rev_cell *ptr , char *str )
#else
*equiv_name( ptr , str )
rev_cell *ptr; 
char *str;
#endif
{
   while ( ptr->gnu_name != ( char const * ) NULL )
      if ( strcmp(ptr->gnu_name,str) == 0 )
         return ( char * ) ptr->nat_name;
      else
         ptr++;

   return ( char * ) NULL;
}

/*

   Function : process_opt()

   Discription : Check if equivalent option exists

   Called by : main()

*/

int
#ifdef __STDC__
process_opt( char *ch_ptr , gnu_cell *gnu_ptr , int *arg_cnt , char **argv )
#else
process_opt( ch_ptr , gnu_ptr , arg_cnt , argv )
char *ch_ptr;
gnu_cell *gnu_ptr;
int *arg_cnt;
char **argv;
#endif
{
   cmd_ptr cell_ptr;
   char    *tmp_ptr;
   int      len;

   len = strlen(gnu_ptr->opt_name);

   if ( gnu_ptr->trail_info == PPATH || gnu_ptr->trail_info == LIBN )
   {
      if ( *( ch_ptr + len ) != CNULL )
         ch_ptr += len;
      else
         ch_ptr = argv[++*arg_cnt];

     if ( gnu_ptr->trail_info ==  PPATH )
     {
        if ( no_path )
        {
           no_path = 0;
           pend_ptr = path_ptr = init_list( &path_cell , ch_ptr );
        }
        else
        {
            if (( cell_ptr = new_cell(ch_ptr, FALSE )) == ( cmd_ptr ) NULL )
            {
               fprintf(stderr,"ERROR : No mem!\n");
               return 0;
            }
            pend_ptr = add_list( pend_ptr , cell_ptr );
        }
    }
    else
    {
       if (( tmp_ptr = (char *)malloc( strlen(ch_ptr) + 6 )) == SNULL )
       {
          fprintf(stderr,"ERROR : No mem!\n");
          return 0;
       }

       strcat(strcat(strcpy(tmp_ptr,"lib"),ch_ptr),".a");

       if (( cell_ptr = new_cell(tmp_ptr, FALSE )) == ( cmd_ptr ) NULL )
       {
          fprintf(stderr,"ERROR : No mem!\n");
          return 0;
       }
       end_ptr = add_list( end_ptr , cell_ptr );
    }
    return 1;
  }
   if (( cell_ptr = new_cell(gnu_ptr->nat_ptr->nat_name , TRUE )) == ( cmd_ptr ) NULL )
   {
      fprintf(stderr,"ERROR : No mem!\n");
      return 0;
   }

   end_ptr = add_list( end_ptr , cell_ptr );

   if ( gnu_ptr->trail_info == NO )
      return strlen(ch_ptr) == strlen(gnu_ptr->opt_name);

   if ( *( ch_ptr + len ) != CNULL )
      ch_ptr += len;
   else
      ch_ptr = argv[++*arg_cnt];

   if ( gnu_ptr->trail_info != OPEN )
      if (( ch_ptr = equiv_name(gnu_ptr->rev_lst,ch_ptr)) != ( char * ) NULL )
         return 0;

    tmp_ptr = ( char * )gnu_ptr->nat_ptr->nat_name;

    if ((tmp_ptr=(char*)malloc(strlen(ch_ptr)+strlen(tmp_ptr)+2)) == SNULL )
    {
       fprintf(stderr,"ERROR : No mem!\n");
       return 0;
    }
    
    strcat(strcat(strcpy(tmp_ptr,gnu_ptr->nat_ptr->nat_name ),":"),ch_ptr);

     cell_ptr->token = tmp_ptr;
     return 1;
}

/*

   Function : in_opt_list()

   Discription : Identify option and return ptr to table entry

   Called by : main()

*/

gnu_cell 
#ifdef __STDC__
*in_opt_list( char *ch_ptr )
#else
*in_opt_list( ch_ptr )
char *ch_ptr;
#endif
{
   gnu_cell *gnu_ptr;
   char *tmp_ptr;

   gnu_ptr = gnu_lst;

   while (( tmp_ptr = ( char * )gnu_ptr->opt_name ) != SNULL )
   {
      if (strncmp(ch_ptr,tmp_ptr,strlen(tmp_ptr))==0 )
         if ( gnu_ptr->nat_ptr != NNULL )
           return gnu_ptr ;
      gnu_ptr++;
   }
   return GNULL;
}

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
   char *arg_ptr, *ch_ptr;
   cmd_ptr tmp_ptr;
   gnu_cell *gnu_ptr;
   int i;

   if ( usage(argc,argv[0]))
      return 0;

#if DEAMON
   if((dm_ptr=deamon(TMP_ENV,TMP_DIR,D_IN,D_OUT,D_ERR,D_TMP))==(dm_struct*)NULL)
   {
      fprintf(stderr,"Cannot execute %s deamon missing\n",*argv[0]);
      return 0;
   }
#endif

   end_ptr = head_ptr = init_list( &head_cell , HOST_CC );

   i = 0;

   while ( ++i < argc )
   {
      arg_ptr = *( argv + i );

      if ( !option(*arg_ptr))
      {
         if (( tmp_ptr = new_cell(arg_ptr,FALSE)) == ( cmd_ptr ) NULL )
         {
            fprintf(stderr,"Insufficient memory EXITING!\n");
            return 0;
         }
         end_ptr = add_list( end_ptr , tmp_ptr );
      }   
      else
      {
         ch_ptr = arg_ptr;
         ch_ptr++;

         while ( *ch_ptr != ( char )NULL )
         {
            if (( gnu_ptr = in_opt_list(ch_ptr)) != ( gnu_cell * ) NULL )
               if ( process_opt(ch_ptr,gnu_ptr,&i,argv))
                  break;
            ch_ptr++;
         }
      }
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
