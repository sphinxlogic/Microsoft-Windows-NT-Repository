/*
*
* GSQL - a Mosaic gateway to SQL applications.
* 
* Jason Ng, NCSA, 
* University of Illinois Urbana-Champaign 
* 1994
* 
* This is public domain software. There is one requirement: if you use this
* software, you should include a link in your forms or documents that
* would mention GSQL and NCSA, and would point to the Tutorial on GSQL,
* ie http://www.ncsa.uiuc.edu/SDG/People/jason/pub/gsql/starthere.html
*
* Modified by David Jones for scripting system used by OSU's VMS HTTP server:
*
*     Assumes program is run directly from URL and portion trailing URL is
*     name of form file (for security, form file name is translated against
*     rule file.  The generated form specifies a method of POST so we can
*     use the method to determine whether response is to be a form or
*     a query.
*
*     This file include cgilib.h and must be linked with cgilib.obj and
*     scriptlib.obj.
*
*     The command defined by SQLPROG is chained to with a do_command rather
*     than spawning a system call.
* 
* */


#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <descrip.h>
#include "cgilib.h"
#define printf cgi_printf
#define strdup(s) strcpy(malloc(strlen(s)+1),s)

#define SELECTKEYWORD "SELECT"

static int dbg = 0;
static int dd  = 0;

static char *sqlprogram = NULL;

static FILE * fstream = NULL; /* procfile file descriptor */

#define NM 101
#define NITEMS 2010
static int nmenus = 0;
static char *menuvname[NM];
static char *menutitle[NM];
static char *mitem[NITEMS][NM];
static int nitems[NM];
static int menutype[NM];

#define M_BUTTON   0
#define M_FIELD    1
#define M_SCROLL   2
#define M_PULLDOWN 3
#define M_RADIO    4
#define M_BUTTONSET 5

static int g_widgettypes[] =
{ M_FIELD, M_BUTTON, M_SCROLL, M_PULLDOWN, M_RADIO, M_BUTTONSET };
static char *g_widgetnames[] =
{ "FIELD", "BUTTON", "SCROLL", "PULLDOWN", "RADIO", "BUTTONSET" };
#define g_ndefwidtypes (sizeof(g_widgettypes)/sizeof(int))

#define LLEN 5000
newmenu(title, vname,type) char *title, *vname; char *type;
{
  int i,t, id;
  char mtype[50];
  /* fprintf(stderr, "NEWMENU [%s] [%s] type [%s]\n", title, vname, type);  */

  sscanf(type,"%s", mtype);
  for(t=(-1), i=0;i<g_ndefwidtypes;i++) 
    if(!strcmp(mtype,g_widgetnames[i])) { t = g_widgettypes[i]; break; }

  if(t==(-1)) return(-1);

  menuvname[nmenus] = strdup(vname);
  menutitle[nmenus] = strdup(title);
  menutype[nmenus] = t;
  nitems[nmenus] = 0;
  id = nmenus; nmenus++;
  return(id);
}

addmenuitem(id, itemname) int id; char *itemname;
{
  int n = nitems[id];
   /* fprintf(stderr,"added [%s] ",itemname);  */
  mitem[n][id] = strdup(itemname);
  nitems[id]++;
}

makeform () {
  int a;
  char buf[LLEN];

  printf("<H1> New Mosaic-SQL interface </H1> <HR>\n");
  if (getdirective("HEADING", buf))  
          printf("%s\n",buf);
  else
          printf("<H1> Type-2 SQL Forms <H2>\n",buf);

  printf("<FORM METHOD=\"POST\">\n");
  for (a=0; a< (int) getitemcount(); a++) {
          int itemid;
          char type[10];
     fetchitem (a , type, &itemid);
          if (!strcmp(type,"TEXT"))
         displaytext(itemid);
          else if (!strcmp(type,"MENU"))
         displaymenu(itemid);
          else if (!strcmp(type,"EXEC")) {
                        getscript(itemid, buf);
         execscript (buf);
                        }
          }
  printf(" <p> <INPUT TYPE=\"submit\" VALUE=\"Doit!\"> <P>\n");
  printf("</FORM>\n");

 if (dbg)  showmenus(); 
}

/* displays the menu specified by id - index into the menu structure */
displaymenu(id) int id;
{
          int j, maxsize;
          printf("<p> %s ", menutitle[id]);
         if     (menutype[id]==M_FIELD ) { /* input field */
            printf(" <INPUT NAME=\"%s\">\n", menuvname[id]);
         }
         else if (menutype[id]==M_BUTTON) { /* toggle button */
            printf("<INPUT TYPE=\"checkbox\" NAME=\"%s\" > \n", 
                                 menuvname[id]);
         }
         else if (menutype[id]==M_BUTTONSET || menutype[id]==M_RADIO) { /* buttons */
           for(j=0;j<nitems[id];j++) {
             if(menutype[id]==M_BUTTONSET)
                printf("<INPUT TYPE=\"checkbox\" NAME=\"%s\" VALUE=\"%s\"> %s\n", 
                                 menuvname[id], mitem[j][id] , mitem[j][id]);
             else if(menutype[id]==M_RADIO)
                printf("<INPUT TYPE=\"radio\" NAME=\"%s\" VALUE=\"%s\"> %s\n", 
                                 menuvname[id], mitem[j][id] , mitem[j][id]);
        }
         }
         else if (menutype[id]==M_SCROLL || menutype[id]==M_PULLDOWN) { /* menus */
           maxsize  = nitems[id] -1 ; if (maxsize<2) maxsize=2;
           if(menutype[id]==M_SCROLL)
              printf("<SELECT NAME=\"%s\" MULTIPLE>\n", menuvname[id], maxsize);
           else if(menutype[id]==M_PULLDOWN)
              printf("<SELECT NAME=\"%s\">\n", menuvname[id]);
           for(j=0;j<nitems[id];j++) {
                         if(j==0)
                   printf(" <OPTION SELECTED>  %s\n", mitem[j][id]);
                         else
                    printf(" <OPTION>  %s\n", mitem[j][id]);
                }
       printf("</SELECT><p>\n");
                }
}

showmenus() {
  int i,j;
  fprintf(stderr,"SHOWALL\n");
  for(i=0;i<nmenus;i++) {
        fprintf(stderr, "MENU title [%s] [%s][%d]\n", 
                                                menutitle[i],menuvname[i], menutype[i]);
         for(j=0;j<nitems[i];j++) {
                 fprintf(stderr, "\t [%s]\n", mitem[j][i]);
                }
        }
}

/* ARGS: arg[1] = procfilename 
         arg[2-..] user inputs from form output 
                        Minimally, procfilename is needed.
*/
old_main(ac,av) int ac; char**av; {

 int i;
 char * procfilename;

 printf("Content-type: text/html \n\n");

 /*
 printf("NEW STUFF "); fflush(stdout);
 printf("NARGS %d \n", ac); if (ac>3) { printf(" SHOULD BE 3 <p>"); }
 for(i=0;i<ac;i++) { printf("<b>%d</b> [%s]\n", i,av[i]); } printf("<p>");
 */

 if(ac<2) {
         printf("Usage: %s procfilename user-inputs...\n",av[0]); 
         exit(0); 
         }
 else if(ac==2) { 
          procfilename = av[1];
     parsefile1(procfilename);
     makeform();
     exit(0);
          }
  else {
         char buf[LLEN];
         procfilename = av[1];
         combineargs(ac,av,buf);
         /* printf("AFTER COMBINE args is <pre><tt> [%s]</tt></pre> \n", buf); */ 
    parseinputs(buf); /* buf: "v1=n1&v2=b2&...&vx=bx", b's can be empty */

    parsefile2(procfilename);
         mergeall();
   }
 }
main(ac,av) int ac; char**av; {

 int i, status;
 char * procfilename, *content_length, *search_arg, *content_l;
    /*
     * Initialize communication with server.
     */
    status = cgi_init ( ac, av );
    if ( (status&1) == 0 ) exit ( status );

    procfilename = cgi_info ( "PATH_TRANSLATED" );
    fprintf ( stderr, "procfile: '%s'\n", procfilename ? procfilename : "?" );

    search_arg = cgi_info ( "QUERY_STRING" );
    fprintf ( stderr, "query: '%s'\n", search_arg ? search_arg : "?" );

    cgi_printf("Content-type: text/html \n\n");

  if ( !search_arg ) search_arg = "";
  if ( (ac > 1) && (strcmp(av[1],"GET")==0) ) {
     /*
      * Get method is for retrieving forms.
      */
     parsefile1(procfilename);
     makeform();
     exit(0);
          }
  else if ( (ac>1) && (strcmp(av[1], "POST")==0) ) {
     /*
      * Post method is for submitted form.
      */
         char buf[LLEN]; int length;
	content_l = cgi_info ( "CONTENT_LENGTH" );
	if ( content_l ) {
	    length = atoi ( content_l );
	    length = cgi_read ( buf, (length > LLEN-1) ? LLEN-1 : length );
	} else {
	    /* Try to use query string. */
	    strncpy ( buf, search_arg, LLEN-1 );
	    length = LLEN-1;
	}
	buf[length] = '\0';
    parseinputs(buf); /* buf: "v1=n1&v2=b2&...&vx=bx", b's can be empty */

    parsefile2(procfilename);
         mergeall();
   }
   else {
     cgi_printf ( "Invalid request (%s), must be GET or POST\n",
		ac > 1 ? av[1] : "" );
     exit ( 0 );
   }
 }


 combineargs(ac,av,buf) 
 int ac;
 char *av[], *buf;
 {
    int i;

         /* av[1] is the procfilename - skip over */
         if(ac<3) return;
    strcpy(buf,av[2]);
         for(i=3;i<ac;i++) {
      strcat(buf,av[i]);
                 }
 }

 /* data structure to receive user inputs - conv to name-value pairs */
#define NU 200
 static char *u_vname[NU];
 static char *u_vval[NU];
 static int  u_vspecial[NU]; /* 1 if that varname starts with "_" */
 static int  n_u = 0;


 parseinputs (inbuf) char *inbuf;
 {
        char *s, *s1=inbuf;
        char *headv, *tailv;
        int i;

        for(i=0;i<NU;i++) { /* init */
          u_vname[i]    = u_vval[i] = NULL;
          u_vspecial[i] = 0;
          }

        i = 0;
        printf("<pre>");
   while(NULL!=( s = strtok(s1,"&"))) {
           {{
                char *b1, *b2;
                b1 = strchr(s,'='); if(b1==NULL) continue;
                *b1 = '\0';
                headv = strdup(s);
                b1++;
                if(b1) tailv = strdup(b1); 
           }}
           if(headv[0]=='_') u_vspecial[i] = 1; 
           u_vname[i] = strdup(headv);
           u_vval[i]  = strdup(tailv);
           if(dd) { printf(" <li> userinput [%s] [%s]",u_vname[i], u_vval[i]);
                     if (u_vspecial[i] ) printf(" <b> special </b>\n");
                                        }
           i++;
      s1=NULL;
           }
        n_u = i;

   printf("</pre>");
 }

/* parses only display and exec directives */
parsefile1 (filename) char *filename; 
{
 char buf[LLEN];
 char title[LLEN];
 char vname[LLEN];
 int id;

 if (NULL == (fstream= fopen(filename,"r"))) {
         printf("parsefile1: Cannot open procfile [%s]\n", filename); 
         exit(0);
         }

 while( getword(buf)) {
        if (buf[0]=='#') {  /* comment - ignore */
          flushline(); 
          }
        else if (!strcmp(buf,"TEXT")) {
          getline(buf);
          id = savetext(buf);
          registeritem ("TEXT", id);
          }
        else if (!strcmp(buf,"HEADING")) {
          getline(buf);
          savedirective("HEADING", buf);
          }
        else if (!strcmp(buf,"SHOW")) {
          getword(vname);
          if (dbg) fprintf(stderr,"VARIABLE [%s]\n", vname);
          getword(buf);
          getquotedline(title); 

          getline(buf); 
          id = newmenu(title,vname, buf);
          makemenu (id, buf);
          registeritem ("MENU", id);

          }
        else if (!strcmp(buf,"EXEC")) {
          getline(buf);
          id = savescript(buf);
          registeritem ("EXEC", id);
     }
   }
  fclose (fstream);
 }

/* will scan to last semi-colon, and truncate the rest of line */
getline(ss) char*ss;
{
          char *sp, sbuf[LLEN];

          ss[0] ='\0';
          while(1) {
        if (NULL==fgets(sbuf,LLEN,fstream)) { 
                          return(0); 
                          }
             if(sbuf[0] =='#') {
                          if(dbg) fprintf(stderr," found comment [%s]\n", sbuf);
                          continue;  /* ignore comment, get another line  */
                          }
             sp = strrchr(sbuf,';');
             if (sp) { /* found semicolon - concat and return  */ 
                          sp[0] = '\0'; /* chop off rest of line */
                     strcat(ss,sbuf);
                     return(1);
                     }
        /* semicolon not found, concat and get more lines */
                     strcat(ss,sbuf);
                }
}

/* look for and return the next quoted string "xxx"  */
getquotedline(ss) char*ss;
{
          char  sbuf[LLEN], *sp = sbuf;
          int c;

          ss[0] ='\0';

          while (EOF != (c = fgetc(fstream)) ) {
                        if (c== '\"')  /* opening quote */
                                break;
                        }

          while (EOF != (c = fgetc(fstream)) ) {

                        if (c== '\"') { /* closing quote */
                           *sp++ = '\0';
                                strcpy(ss,sbuf);
                                return(1);
                                }
                        else {
                           *sp++ = c; 
                           }
         }
}

flushline() { /* throw away rst of line in input stream */
        char  sbuf[LLEN];
   fgets(sbuf,LLEN,fstream);
        }

getword(ss) char*ss;
{
char *sp;
char sbuf[LLEN];

if( EOF== fscanf(fstream,"%s",sbuf) ) return(0);

if (sp = strrchr(sbuf,';')) { *sp = '\0';  } /* remove semicolon */ 

sscanf(sbuf,"%s", ss);
return(1); 
}


/* for each item, need to remove leading and trailing blanks */ 
/* Note that mline  must have a first word of the menu type keyword */ 
makemenu (menuid, mline ) int menuid; char *mline ; {
#define PATTERN  ","

 char *s, *s1;
 char *zz = mline;

 /* skip past first word  */
  while( zz && isspace(*zz) ) { zz++; }  if (zz==NULL) return; 
  while( zz && !isspace(*zz) ) { zz++; }  if (zz==NULL) return; 

 if (dbg) fprintf(stderr,"DOIT rest ID %d[%s]\n", menuid, zz);

 if (NULL!=(s1 = strstr(zz,"EXEC"))) {
        FILE *f;
        char buf[LLEN];
        execcommand((s1+4), &f); 
        while(NULL!=fgets(buf, LLEN, f)){
          addmenuitem(menuid, buf);
          }
   fclose(f);
        return;
        }

 s1=zz;
 while(NULL!=( s = strtok(s1,PATTERN))) {
        chopblanks(&s);
        addmenuitem(menuid, s);
   s1=NULL;
 }

}
chopblanks(ss) char **ss;
{
  char *sp = *ss;
  int n;
  while(isspace(*sp) ) sp++;
  *ss = sp;
  
  sp = *ss;
  n = strlen(*ss);
  sp = *ss+(n-1);
  while(isspace(*sp)) { *sp = '\0'; sp--; }
        
}

/* ================================================================== */
/* list of graphic objects */
#define NOBJ 200
static int obj_ref[NOBJ]; /* actually a unique reference int */ 
static int obj_type[NOBJ]; /* t - text, m - menu, e -exec */
static int n_obj = 0;

/* rets its ordering , or -1 if error */
int registeritem(type, ref) char *type; int ref;
{
        int t , n;

   if (!strcmp(type,"TEXT")) {
                t = 't';
                }
   else if (!strcmp(type,"MENU")) {
                t = 'm';
                }
   else if (!strcmp(type,"EXEC")) {
                t = 'e';
                }
   else { return(-1); }

        n = n_obj;
        obj_ref[n]  = ref;
        obj_type[n] = t;
        n_obj++;
        return (n);
}

getitemcount() { return (n_obj); }

fetchitem (which, type, ref )
int which; /* its ordering as ret from registeritem() */
int  *ref; /* output */
char *type; /* output */
{
   *ref = obj_ref[which];
   switch (obj_type[which]) {
                case 't': strcpy(type,"TEXT"); break;
                case 'm': strcpy(type,"MENU"); break;
                case 'e': strcpy(type,"EXEC"); break;
                default : strcpy(type,"NONE"); return(0);  break;
                }
   return(1);
                
}

/* ================================================================== */
/* storage for user's scripts and executables */
#define NEX 80
static char * d_ex[NEX];
static int   n_ex  = 0;

/* rets id of script , else -1 */
int savescript(ex) char *ex;
{
  int id;
  d_ex[n_ex] = strdup ( ex );
  id = n_ex;  n_ex++; 
  return (id);
}

getscript (id, ex) int id; char *ex;
{
  strcpy(ex, d_ex[id] );
}
execscript (ex) char *ex;
{
  char tfile[500];
  char buf[LLEN];
  FILE *f;

  tmpnam(tfile);
  sprintf(buf, "%s > %s ", ex, tfile);

  system( buf );
  f = fopen(tfile,"r");
  if(f==NULL) { printf("cannot open %s \n", tfile); return; }

  while(NULL!= fgets(buf, LLEN, f)) printf("%s",buf);
  fclose(f);
}     

execcommand (ex,fhandle) char *ex; FILE **fhandle;
{
  char tfile[500];
  char buf[LLEN];
  FILE *f;


  tmpnam(tfile);
  sprintf(buf, "%s > %s ", ex, tfile);

  system( buf );
  f = fopen(tfile,"r");
  if(f==NULL) { printf("cannot open %s \n", tfile); *fhandle = NULL; return; }
  *fhandle = f;

}

/* ================================================================== */
/* storage for user's text */
#define NTXT 80
static char * d_text[NTXT];
static int   n_txt = 0;

/* rets id of text, else -1 */
int savetext(text) char *text;
{
  int id;
  d_text[n_txt] = strdup ( text);
  id = n_txt;  n_txt++; 
  return (id);
}

gettext (id, text) int id; char *text;
{
  strcpy(text, d_text[id] );
}
displaytext ( id) int id;
{
  char text[LLEN];
  gettext(id, text);
  printf("%s", text);
}

/* ================================================================== */
/* defines from DEFINE */
/* sql directives and the expansion */

/* general sql directives */
static char * d_cmd[] = { "FROMLIST", "SELECTLIST", "WHERELIST", "SORTLIST", "HEADING" };
#define NW   ( sizeof(d_cmd)/sizeof(char *) )
static char * d_mean[NW];
static int   n_cmd = NW;

/* defines from DEFINE */
#define MDEF 40
static char * d_defvar[MDEF];
static char * d_defval[MDEF];
static int n_def = 0;

/* variables from SUB */
#define NV 200
static char * var_name[NV];
static char * var_mean[NV];
static char * var_kind[NV]; /* what list it's part of eg SELECT or WHERE */
static char * var_fmt[NV]; /* string to format output as */
static int   var_used[NV]; /* 1 if has user input */
static int n_var = 0;

parsefile2 (filename) char *filename; 
{
 char buf[LLEN], tbuf[LLEN];
 char vname[LLEN];
 int id, i;

 if (NULL == (fstream= fopen(filename,"r"))) {
         printf("parsefile2: Cannot open procfile [%s]\n", filename); 
         exit(0);
         }

for(i=0;i<NW;i++) {  d_mean[i] = NULL; }

for(i=0;i<NV;i++) { /* init */
  var_name[i] = var_mean[i] = var_kind[i] = NULL; 
  var_used[i] = 0;
  var_fmt[i] = NULL;
  }

 printf("<PRE>");
 while( getword(buf)) {
        if (buf[0]=='#') { 
          if(dbg) fprintf(stderr,"comment word [%s]\n", buf);
          flushline(); 
        }
        else if (!strcmp(buf,"HEADING")) {
          getline(buf);
          savedirective("HEADING", buf);
          }
        else if (!strcmp(buf,"SUB")) {
          getword(vname); 
          var_name[n_var] = strdup(vname);
          getword(buf); /* kind */
          var_kind[n_var] = strdup(buf);
          getword(buf); /* AS */
          getline(buf); /* meaning */
          var_mean[n_var] = strdup(buf);
          if(dd) printf("<li> FILE VAR [%s] [%s] [%s] \n", 
                        var_name[n_var], var_kind[n_var], var_mean[n_var]);
          n_var++;
          }
        else if (!strcmp(buf,"SQLPROG")) {
                getline(tbuf);
                sqlprogram = strdup(tbuf);
                }
        else if (!strcmp(buf,"DEFINE")) {
           getword(tbuf);
                sscanf(tbuf,"%s",buf);
           d_defvar[n_def] = strdup(buf);
           getline(tbuf);
                sscanf(tbuf,"%s",buf);
           d_defval[n_def] = strdup(buf);
           if(dd) printf("<li> usrdefine [%s] [%s]\n", d_defvar[n_def], d_defval[n_def]);
           n_def++;
          }
        else { /* save if is a sql directive */
           getline(tbuf);
                savedirective (buf, tbuf);

                }
  }
 printf("</PRE>");
  fclose (fstream);
}

/* ------------------------------------------------------------------ */
/* returns the meaning of a SUB variable */
lookupmean (var, meaning) char *var, *meaning;
{
        int i;
   for (i=0;i<n_var;i++) 
                if (!strcmp(var, var_name[i])) {
                  strcpy(meaning, var_mean[i]);
                  return(1);
                  }
   return(0);           
}
/* ------------------------------------------------------------------ */
/* updates the var_xxx structures with info from user */
/* doesn't account for duplicate vars for now */

mergeall() 
{
   
        int i, j, t, num;
        char *vname;
        char buf[LLEN]; /* contains all matches (comma-separated) for a variable */
        int match[200], nm;
        char sqlstr[LLEN];

        /* -- sub user inputs in the variables --- */
        /* some vars (arrays) can have multiple userinput values */
   for (i=0;i<n_var;i++) {
                vname = var_name[i];
                for (nm=0, t=0; t<n_u; t++) {
          if (!strcmp(vname, u_vname[t])) {
                                 if (strlen(u_vval[t])>0) { /* don't bother if empty value */
                                   var_used[i] = 1;
                                   match[nm] = t; nm++;
                                   }
                                 }
                    }
                if (nm>0) { /* build the match string */
                        buf[0]='\0';
                   for (j=0;j<nm;j++) {
                                t = match[j];
                      if (u_vspecial[t]) { /* look up its meaning */
                                        char tbuf[LLEN];
                                        if (!lookupmean (u_vval[t], tbuf)) {
                                                strcpy(tbuf,"Undefined"); 
                                                printf("<li>UNDEFINED [%s]\n", u_vval[t]);
                                                }
               else {
                                     if (j!=0) strcat(buf, ",");
                                strcat(buf, tbuf);
                                }
                              }
            else { /* just use the input */
                                   if (j!=0) strcat(buf, ",");
                              strcat(buf, u_vval[t]);
               }
            }
                   substitute(&var_mean[i], buf);
         }
          }

  /* --- build the sql string from the pieces ---  */

         sqlstr[0] ='\0';
         strcat (sqlstr," \" " );
         strcat( sqlstr, SELECTKEYWORD);

         num = 0;
         if( getdirective("SELECTLIST", buf)) {
       strcat(sqlstr,  buf );
                 num = 1;
            }
    for (i=0;i<n_var;i++) {
            if (var_used[i]) { 
                    if (!strcmp(var_kind[i],"SELECTLIST")) {
                                 if(num>0) strcat(sqlstr, ", ");
             strcat(sqlstr, var_mean[i]);
                                 num++;
                                 }
                         }
            }
         if (num==0) { /* yikes..no user-selected values, & no default select list */
                 printf(" <H1> You must select field(s) to display. </H1> "); 
                 return; 
                 }


         if( getdirective("FROMLIST", buf)) {
       strcat(sqlstr, " FROM ");
       strcat(sqlstr,  buf );
                 }

         num = 0;
    strcat(sqlstr, " WHERE ");
    for (i=0;i<n_var;i++) {
            if (var_used[i]) {
                    if (!strcmp(var_kind[i],"WHERELIST")) {
                                 if(num>0) strcat(sqlstr, " AND ");
             strcat(sqlstr, var_mean[i]);
                                 num++;
                  }
               }
            }
         if( getdirective("WHERELIST", buf)) {
       if (num>0) strcat(sqlstr, " AND  ");
       strcat(sqlstr,  buf );
            }

         if( getdirective("SORTLIST", buf)) {
       strcat(sqlstr,  buf );
            }

    strcat(sqlstr, " \" "); /* entire sql string is quoted */

         if(dd) printf("<b> SQL </b> <tt> %s </tt> <p> \n", sqlstr);

        stuffmenu("Here is the SQL Query ", sqlstr );

        strcat(sqlstr," ");
   for(i=0;i<n_def;i++) {
                 char zz[LLEN];
                 sprintf(zz," \"%s %s\" ",d_defvar[i], d_defval[i]);
                 strcat(sqlstr,zz);
                 }

  if (getdirective("HEADING", buf))  printf("%s\n",buf);
   printf("<P>");

        {{ char cmd[LLEN], symname[4]; int i, n, length;
	   $DESCRIPTOR(command,"");
	   $DESCRIPTOR(symbol,"C00"); $DESCRIPTOR(value,"");
                if (sqlprogram==0) {
                        printf("<H1> Sorry..I don't know what SQL program to run.");
                        printf("<p>SQLPROG is not specified! </H1>");
                        exit(0);
                        }
	    /*
	     * Break sqlstr into a series of DCL symbols of no more than
	     * 255 characters.
	     */
	    length = strlen ( sqlstr );
	    symbol.dsc$a_pointer = symname;
	    for ( n = i = 0; i < length; i += 255 ) {
		value.dsc$w_length = length - i;
		if ( value.dsc$w_length > 255 ) value.dsc$w_length = 255;
		value.dsc$a_pointer = &sqlstr[i];
		sprintf(symname,"C%02d", n++ );
		LIB$SET_SYMBOL ( &symbol, &value );
	    }
fprintf(stderr,"sql, %d parts: %s\n", n, sqlstr );
                sprintf(cmd,"%s 'C00''C01''C02''C03''C04'", sqlprogram );
                /* stuffmenu("Here is the cmd ", cmd); */

                fflush(stdout);
	cgi_set_dcl_env("WWW_");
	command.dsc$a_pointer = cmd;
	command.dsc$w_length = strlen(cmd);
fprintf(stderr,"cmd: %s\n", cmd );
	LIB$DO_COMMAND ( &command );
	printf("<p>Error chaining to SQLPROG\n");	/* should never get here */
        }}

}

savedirective(cmd, meaning) 
char * cmd , *meaning;
{
   int i;

        for(i=0;i<NW;i++)  
                if (!strcmp(cmd , d_cmd[i])) {
         d_mean[i] = strdup(meaning); 
                        return(1);
                        break;
                }
        return(0);
}

getdirective (cmd , meaning)
char * cmd , *meaning;
{
    int i;
    for (i=0;i<n_cmd;i++) 
            if (!strcmp(d_cmd[i], cmd)) {
                        if(NULL==d_mean[i]) return (0);
                        strcpy(meaning, d_mean[i]);
                        return(1);
              }
        return(0);
}


/* substitute all occurences of $ in x1 with x2 */
/* this means reallocing the string at x1 */
substitute(x1, x2) char **x1, *x2; {
        char buf[LLEN];
        char *orig = *x1;
        char *sp;

        /* printf("<p> sub [%s] inside [%s] ", x2, orig); */
        sp = strchr(orig,'$');
        if (sp==NULL) return;

        buf[0] = '\0';
   
        *sp ='\0';
   strcat(buf,orig);  /* first portion */

   strcat(buf,x2);    /* substituted portion */

   sp++;
        if(sp) {        /* end portion */
          strcat(buf,sp);
     }

   *x1 = strdup(buf);
        /* printf("new string is [%s] ", *x1); */
}

/* stuff some text into a menu */
stuffmenu(title, string) char *title, *string;
{
       int n;
                 char *stringbuf;
                 char buf[LLEN];
                 char *ss, *s2, *s1;


                 stringbuf = strdup(string);
                 /* remove quotes */
         s1 = strchr(stringbuf,  '\"'); if(s1) *s1 = ' ';
         s1 = strrchr(stringbuf,  '\"'); if(s1) *s1 = ' ';

                 printf("<FORM>");
            printf("<SELECT NAME=\"sql-query\">\n" );
            printf(" <OPTION>  %s \n", title);

       s1 = ss = stringbuf;
       while(1) {
                         s2 = strchr(s1, ' ');
                         if(s2==NULL) break;
                         n = s2 - ss;
          if (n > 80) {
                                strncpy(buf,ss,n); buf[n]='\0';
                    printf(" <OPTION>  %s\n", buf);
                                ss = ++s2; if(ss==NULL) break;
                                }
          else {
             s1 = s2+1;
                                }

          }
                 if (ss) printf(" <OPTION>  %s\n", ss); /* trailing text */
       printf("</SELECT><p>\n");
                 printf("</FORM>");

                 fflush(stdout);
}
