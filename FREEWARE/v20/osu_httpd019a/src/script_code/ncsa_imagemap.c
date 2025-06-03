/*
** mapper 1.2
** 7/26/93 Kevin Hughes, kevinh@pulua.hcc.hawaii.edu
** "macmartinized" polygon code copyright 1992 by Eric Haines, erich@eye.com
** All suggestions, help, etc. gratefully accepted!
**
** 1.1 : Better formatting, added better polygon code.
** 1.2 : Changed isname(), added config file specification.
**
** 11/13/93: Rob McCool, robm@ncsa.uiuc.edu
**
** Rewrote configuration stuff for NCSA /htbin script
**
** 12/05/93: Rob McCool, robm@ncsa.uiuc.edu
** 
** Made CGI/1.0 compliant.
**
** 06/27/94: Chris Hyams, cgh@rice.edu
**          Based on an idea by Rick Troth (troth@rice.edu)
**
** 04/05/95: David Jones, vman+@osu.edu
**	    Converted to DECthreads CGI environment.
**
** Imagemap configuration file in PATH_INFO.  Backwards compatible.
**
**  Old-style lookup in imagemap table:
**    <a href="http://foo.edu/cgi-bin/imagemap/oldmap">
**
**  New-style specification of mapfile relative to DocumentRoot:
**    <a href="http://foo.edu/cgi-bin/imagemap/path/for/new.map">
**
**  New-style specification of mapfile in user's public HTML directory:
**    <a href="http://foo.edu/cgi-bin/imagemap/~username/path/for/new.map">
**
** 07/11/94: Craig Milo Rogers, Rogers@ISI.Edu
**
** Added the "point" datatype.  The nearest point wins.  Overrides "default".
**
** 08/28/94: Carlos Varela, cvarela@ncsa.uiuc.edu
**
** Fixed bug:  virtual URLs are now understood.
** Better error reporting when not able to open configuration file.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#ifndef VAXC
#include <sys/stat.h>
#else
#include <stat.h>
#endif

#ifdef NOCGILIB
#define cgi_init(a,b)  1
#define cgi_info(a) getenv(strcpy(&cgi_info_buf[4], a)-4)
static char cgi_info_buf[64] = { 'W', 'W', 'W', '_' };
#define cgi_printf printf
#else
#include "cgilib.h"
#endif
#ifndef CONF_FILE
#define CONF_FILE "/www_root/conf/imagemap.conf"
#endif

#define MAXLINE 500
#define MAXVERTS 100
#define X 0
#define Y 1
/*
 * Not sure what function getline is supposed to do.  Assume it works
 * like fgets (same arguments) but concatentates lines that end in backslash.
 * Return 0 on success.
 */
static int getline ( char *buf, int bufsize, FILE *fp )
{
    int i, len;
    char *cur;
    for ( i = 0; i < bufsize; ) {
	cur = fgets ( &buf[i], bufsize - i, fp );
	if ( !cur ) return (i > 0) ? 0 : -1;
	i += strlen ( cur );
	if ( i > 1 ) if ( (buf[i-1] == '\n') && (buf[i-2] == '\\') ) {
	    buf[i-1] = '\0';
	    buf[i-2] = ' ';	/* ensure line break counts as whitespace */
	    i = i - 1;
	    continue;
	}
	break;
    }
    return 0;
}

int isname(char);
void servererr(char *), sendmesg(char *);	/* forward references */
int pointincircle(double point[2], double coords[MAXVERTS][2]);
int pointinpoly(double point[2], double coords[MAXVERTS][2]);
int pointinrect(double point[2], double coords[MAXVERTS][2]);

int main(int argc, char **argv)
{
    char input[MAXLINE], *mapname, def[MAXLINE], conf[MAXLINE];
    double testpoint[2], pointarray[MAXVERTS][2];
    int status, i, j, k;
    FILE *fp;
    char *t, *query_string;
    double dist, mindist;
    int sawpoint = 0;
    status = cgi_init ( argc, argv );
    if ( (status&1) == 0 ) exit ( status );

    mapname=cgi_info("PATH_INFO");

    if((!mapname) || (!mapname[0]))
        servererr("No map name given. Please read the <A HREF=\"http://hoohoo.ncsa.uiuc.edu/docs/setup/admin/Imagemap.html\">instructions</A>.<P>");


    mapname++;
    query_string = cgi_info ( "QUERY_STRING" );
    if ( !query_string ) query_string = "";
    if(!(t = strchr(query_string,',')))
        servererr("Your client doesn't support image mapping properly.");
    *t++ = '\0';
    testpoint[X] = (double) atoi(query_string);
    testpoint[Y] = (double) atoi(t);

    /*
     * if the mapname contains a '/', it represents a unix path -
     * we get the translated path, and skip reading the configuration file.
     */
    if (strchr(mapname,'/')) {
      strcpy(conf,cgi_info("PATH_TRANSLATED"));
      goto openconf;
    }
    
    if ((fp = fopen(CONF_FILE, "r")) == NULL)
        servererr(strcat("Couldn't open configuration file:", CONF_FILE));

    while(!(getline(input,MAXLINE,fp))) {
        char confname[MAXLINE];
        if((input[0] == '#') || (!input[0]))
            continue;
        for(i=0;isname(input[i]) && (input[i] != ':');i++)
            confname[i] = input[i];
        confname[i] = '\0';
        if(!strcmp(confname,mapname))
            goto found;
    }
    /*
     * if mapname was not found in the configuration file, it still
     * might represent a file in the server root directory -
     * we get the translated path, and check to see if a file of that
     * name exists, jumping to the opening of the map file if it does.
     */
    if(feof(fp)) {
      struct stat sbuf;
      strcpy(conf,cgi_info("PATH_TRANSLATED"));
      if (!stat(conf,&sbuf) && ((sbuf.st_mode & S_IFMT) == S_IFREG))
	goto openconf;
      else
	servererr("Map not found in configuration file.");
    }
    
  found:
    fclose(fp);
    while(isspace(input[i]) || input[i] == ':') ++i;

    for(j=0;input[i] && isname(input[i]);++i,++j)
        conf[j] = input[i];
    conf[j] = '\0';

  openconf:
    if(!(fp=fopen(conf,"r")))
        servererr(strcat("Couldn't open configuration file:", conf));

    while(!(getline(input,MAXLINE,fp))) {
        char type[MAXLINE];
        char url[MAXLINE];
        char num[10];

        if((input[0] == '#') || (!input[0]))
            continue;

        type[0] = '\0';url[0] = '\0';

        for(i=0;isname(input[i]) && (input[i]);i++)
            type[i] = input[i];
        type[i] = '\0';

        while(isspace(input[i])) ++i;
        for(j=0;input[i] && isname(input[i]);++i,++j)
            url[j] = input[i];
        url[j] = '\0';

        if(!strcmp(type,"default") && !sawpoint) {
            strcpy(def,url);
            continue;
        }

        k=0;
        while (input[i]) {
            while (isspace(input[i]) || input[i] == ',')
                i++;
            j = 0;
            while (isdigit(input[i]))
                num[j++] = input[i++];
            num[j] = '\0';
            if (num[0] != '\0')
                pointarray[k][X] = (double) atoi(num);
            else
                break;
            while (isspace(input[i]) || input[i] == ',')
                i++;
            j = 0;
            while (isdigit(input[i]))
                num[j++] = input[i++];
            num[j] = '\0';
            if (num[0] != '\0')
                pointarray[k++][Y] = (double) atoi(num);
            else {
                fclose(fp);
                servererr("Missing y value.");
            }
        }
        pointarray[k][X] = -1;
        if(!strcmp(type,"poly"))
            if(pointinpoly(testpoint,pointarray))
                sendmesg(url);
        if(!strcmp(type,"circle"))
            if(pointincircle(testpoint,pointarray))
                sendmesg(url);
        if(!strcmp(type,"rect"))
            if(pointinrect(testpoint,pointarray))
                sendmesg(url);
        if(!strcmp(type,"point")) {
	    /* Don't need to take square root. */
	    dist = ((testpoint[X] - pointarray[0][X])
		    * (testpoint[X] - pointarray[0][X]))
		   + ((testpoint[Y] - pointarray[0][Y])
		      * (testpoint[Y] - pointarray[0][Y]));
	    /* If this is the first point, or the nearest, set the default. */
	    if ((! sawpoint) || (dist < mindist)) {
		mindist = dist;
	        strcpy(def,url);
	    }
	    sawpoint++;
	}
    }
    if(def[0])
        sendmesg(def);
    servererr("No default specified.");
}

void sendmesg(char *url)
{
  if (strchr(url, ':'))   /*** It is a full URL ***/
    cgi_printf("Location: ");
  else                    /*** It is a virtual URL ***/
    cgi_printf("Location: http://%s:%s", cgi_info("SERVER_NAME"), 
           cgi_info("SERVER_PORT"));

    cgi_printf("%s%c%c",url,10,10);
    cgi_printf("This document has moved <A HREF=\"%s\">here</A>%c",url,10);
    exit(1);
}

int pointinrect(double point[2], double coords[MAXVERTS][2])
{
        return ((point[X] >= coords[0][X] && point[X] <= coords[1][X]) &&
        (point[Y] >= coords[0][Y] && point[Y] <= coords[1][Y]));
}

int pointincircle(double point[2], double coords[MAXVERTS][2])
{
        int radius1, radius2;

        radius1 = ((coords[0][Y] - coords[1][Y]) * (coords[0][Y] -
        coords[1][Y])) + ((coords[0][X] - coords[1][X]) * (coords[0][X] -
        coords[1][X]));
        radius2 = ((coords[0][Y] - point[Y]) * (coords[0][Y] - point[Y])) +
        ((coords[0][X] - point[X]) * (coords[0][X] - point[X]));
        return (radius2 <= radius1);
}

int pointinpoly(double point[2], double pgon[MAXVERTS][2])
{
        int i, numverts, inside_flag, xflag0;
        int crossings;
        double *p, *stop;
        double tx, ty, y;

        for (i = 0; pgon[i][X] != -1 && i < MAXVERTS; i++)
                ;
        numverts = i;
        crossings = 0;

        tx = point[X];
        ty = point[Y];
        y = pgon[numverts - 1][Y];

        p = (double *) pgon + 1;
        if ((y >= ty) != (*p >= ty)) {
                if ((xflag0 = (pgon[numverts - 1][X] >= tx)) ==
                (*(double *) pgon >= tx)) {
                        if (xflag0)
                                crossings++;
                }
                else {
                        crossings += (pgon[numverts - 1][X] - (y - ty) *
                        (*(double *) pgon - pgon[numverts - 1][X]) /
                        (*p - y)) >= tx;
                }
        }

        stop = pgon[numverts];

        for (y = *p, p += 2; p < stop; y = *p, p += 2) {
                if (y >= ty) {
                        while ((p < stop) && (*p >= ty))
                                p += 2;
                        if (p >= stop)
                                break;
                        if ((xflag0 = (*(p - 3) >= tx)) == (*(p - 1) >= tx)) {
                                if (xflag0)
                                        crossings++;
                        }
                        else {
                                crossings += (*(p - 3) - (*(p - 2) - ty) *
                                (*(p - 1) - *(p - 3)) / (*p - *(p - 2))) >= tx;
                        }
                }
                else {
                        while ((p < stop) && (*p < ty))
                                p += 2;
                        if (p >= stop)
                                break;
                        if ((xflag0 = (*(p - 3) >= tx)) == (*(p - 1) >= tx)) {
                                if (xflag0)
                                        crossings++;
                        }
                        else {
                                crossings += (*(p - 3) - (*(p - 2) - ty) *
                                (*(p - 1) - *(p - 3)) / (*p - *(p - 2))) >= tx;
                        }
                }
        }
        inside_flag = crossings & 0x01;
        return (inside_flag);
}

void servererr(char *msg)
{
    cgi_printf("Content-type: text/html%c%c",10,10);
    cgi_printf("<title>Mapping Server Error</title>");
    cgi_printf("<h1>Mapping Server Error</h1>");
    cgi_printf("This server encountered an error:<p>");
    cgi_printf("%s", msg);
    exit(-1);
}

int isname(char c)
{
        return (!isspace(c));
}


