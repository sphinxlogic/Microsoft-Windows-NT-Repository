#include "patchlevel.h"
/*
  glcomp.c: a ".gl" file composer/decomposer.
  
  To compose a new ".gl" file, enter each file's name in a listfile.
  Supply the listfile's name as the second argument.
  
  Originally programmed for TurboC.
  Should be portable enough though.
  
  Donated to the public domain by S.R.van den Berg on 1990-11-08
  (That means I will not support the code any further, and you can do
  with the code whatever you want (except copyright it :-)).
  Reachable on INTERNET as:
  berg%cip-s01.informatik.rwth-aachen.de@unido.bitnet
  Reachable on BITNET as:
  berg%cip-s01.informatik.rwth-aachen.de@unido
  */

#ifdef VMS
#include <file.h>
#include <stdlib.h>
#include <stat.h>
#else
#include <fcntl.h>
#include <sys/types.h>
#include <sys/file.h>
#ifdef STDLIB
#include <stdlib.h>
#else
#include "my_types.h"
#endif
#include <sys/stat.h>
#endif
#define  S_IFILE  (S_IFREG|S_IREAD|S_IWRITE)

#define SEEK_SET 0
#define	SEEK_CUR 1
#define SEEK_END 2

#define  MYBUFSIZ   8000
#define  MAXFILES 1024

#define  RECLEN   17

#define fread(fd,buf,size) (eof[fd]=read(fd,buf,size))
#define min(x,y) ((x>y)?y:x)

unsigned _stklen=1024,_heaplen=4;
typedef unsigned char uchar;
struct {
  long offset;char name[13];
} filerec[MAXFILES];
uchar buffer[MYBUFSIZ];
int in,out;
int eof[MAXFILES];

long fgetl()
{
  unsigned i;
  i=fgetw();return i|(long)(unsigned)fgetw()<<16;
}

main(argc,argv)char*argv[];
{
  int i;char *p;
  if(argc<2||argc>3)
    {
      putse("Usage: glcomp xxx.gl [listfile]\n");return 1;
    }
  if(0>(in=open(argv[argc-1],O_RDONLY)))
    {
    couldntfind:
      putse("Couldn't find ");putse(argv[argc-1]);return 2;
    }
  if(argc==2)
    {
      if(fgetw()/RECLEN>MAXFILES)
	{
	toomanyfiles:
	  putse("Too many files\n");return 3;
	}
      i=0;
      do
	{
	  filerec[i].offset=fgetl();fread(in,filerec[i].name,RECLEN-4);
	}
      while(*filerec[i++].name);
      i=0;
      do
	{
	  if(0>
	     (out=open(p=filerec[i].name,O_WRONLY|O_CREAT|O_TRUNC,S_IFILE)))
	    {
	    couldntopen:
	      putse("Couldn't open ");putse(p);return 4;
	    }
	  puts(p);puts("\n");lseek(in,filerec[i].offset,SEEK_SET);copyfw(fgetl());
	  close(out);
	}
      while(*filerec[++i].name);
    }
  else
    {
      long len;
      eof[in] = 1;
      for(i=0;;)
	{
	  if(i>MAXFILES)
	    goto toomanyfiles;
	  p=filerec[i++].name;
	  do
	    if(eof[in]<=0)
	      goto nomorefiles;
	  while((*p=fgetc())<=' ');
	  while((*++p=fgetc())>' ');
	  *p='\0';
	}
    nomorefiles:
      close(in);*p='\0';
      if(0>(out=open(p=argv[1],O_WRONLY|O_CREAT|O_TRUNC,S_IFILE))) {
        putse("Couldn't open ");putse(p);return 4;
      }
      fputw(i=i*RECLEN);write(out,filerec,i);
      i=0;
      puts("Reading:\n");
      do
	{
	  if(0>(in=open(p=filerec[i].name,O_RDONLY)))
	      {
		putse("Couldn't open ");putse(p);return 4;
	      }
	  puts(p);puts("\n");filerec[i].offset=lseek(out,0L,SEEK_CUR);
	  fputl(len=lseek(in,0L,SEEK_END));lseek(in,0L,SEEK_SET);copyfw(len);
	  close(in);
	}
      while(*filerec[++i].name);
      lseek(out,2L,SEEK_SET);i=0;
      do
	{
	  fputl(filerec[i].offset);write(out,filerec[i].name,RECLEN-4);
	}
      while(*filerec[i++].name);
    }
  return 0;
}

fgetc()
{
  uchar i;
  int l;
  l=fread(in,&i,1);
  if (l==1)
    return i;
  return l;
}

fgetw()
{
  int i;
  i=fgetc();return i|fgetc()<<8;
}

fputc(i)
{
  uchar j;
  j=i;write(out,&j,1);
}

fputw(i)
{
  fputc(i);fputc(i>>8);
}

fputl(i)long i;
{
  fputw(i);fputw(i>>16);
}

copyfw(i)long i;
{
  long j;
  while(i>0L)
    {
      j=i;
      i-=
	(unsigned)write(out,buffer,fread(in,buffer,(unsigned)min(i,(long)MYBUFSIZ)));
      if(i>=j)
	{
	  putse("Choking...  Need space...\n");close(in);close(out);abort();
	}
    }
}

#define STDOUT 1
#define STDERR 2

puts(a)char *a;
{
  putsfd(a,STDOUT);
}

putse(a)char *a;
{
  putsfd(a,STDERR);
}

putsfd(a,fd)register char *a;
{
  register char *b;
  b=a;
  while(*a) a++;
  write(fd,b,a-b);
}

_setupio()
{
}   /* Only use this if NO streams are used */
