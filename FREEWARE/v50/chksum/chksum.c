/*                                                                      */
/* This program is free software; you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation; either version 2 of the License, or    */
/* (at your option) any later version.                                  */
/*                                                                      */
/* This program is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* chksum - Calculates file(s) checksum according to several standards	*/
/* Written J.Lauret, <jlauret@mail.chem.sunysb.edu>			*/
/*                                                                      */


#include "chksum.h"
#include "md5.h"
		
/* Implemented Methods						*/
static unsigned int method=0;
int compute_BSD_checksum(FILE *,char *,int);	/* 2 */
int compute_SYSV_checksum(FILE *,char *,int);	/* 2 */
int compute_CRC_checksum(FILE *,char *);	/* 1 */
int compute_MD5_checksum(FILE *,char *);


/*
 * Computes the BSD, SysV or CRC checksum of a file or files.
 */
int main (int argc,char **argv)
{
  void			lhelp();
  int			Process();
  int			i;

  int			errflg = 0;
  int			numfiles=0;
  char			filin[LONG];
  char			*q;
#ifdef VMS
  unsigned long		context=0;
  unsigned int		status;
  char			wild_file[LONG]="\0";
  char			prev_file[LONG]="\0";
  DDESCR(next_file_d);
  DDESCR(wild_file_d);
  IIDESCR(next_file_d,filin,LONG);
#endif

  /* Get default method from environment				*/
  q = getenv("CHKSUM_OPTION");
  if(q){
    switch (*q){
    case 'b': 
      method = 0;
      break;
    case 'r':
      method = 1;
      break;
    case 's':
      method = 2;
      break;
    case 'o':
      method = 3;
      break;
    case 'c':
      method = 4;
      break;
    case 'm':
      method = 5;
      break;
    default:
      (void) fprintf(stderr,"chksum: Environment [%s] unknown\n",q);
    }
  }  
  

  /* Scan args (simple way)						*/
  for(i = 1 ; i < argc ; i++) {
    /* Test for options							*/
    if ( strcmp(argv[i],"-h") == 0 ){
      lhelp();
      return EXIT_SUCCESS;
    } else if ( strcmp(argv[i],"-b") == 0){
      method = 0;
    } else if ( strcmp(argv[i],"-r") == 0){
      method = 1;
    } else if ( strcmp(argv[i],"-s") == 0){
      method = 2;
    } else if ( strcmp(argv[i],"-o") == 0){
      method = 3;
    } else if ( strcmp(argv[i],"-c") == 0){
      method = 4;
    } else if ( strcmp(argv[i],"-m") == 0){
      method = 5;
    } else {
#ifdef VMS
      /* Wildcard => Expand it (but no version number)			*/
      (void *) strcpy(wild_file,argv[i]);
      IDESCR(wild_file_d,wild_file);

      NEXT_FILE:
       status = lib$find_file(&wild_file_d,&next_file_d,&context,0,0,0,0);
       if( OK(status) ){
	 q = strrchr(filin,';');
	 if(q) *q= '\0';
	 if ( strcmp(prev_file,filin) == 0 ){
	   goto NEXT_FILE;
	 }
	 (void *) strcpy(prev_file,filin);
#else
	 (void *) strcpy(filin,argv[i]);
#endif
	 numfiles++;
	 errflg += Process(filin);

#ifdef VMS
       goto NEXT_FILE;
     }
     (void) lib$find_file_end(&context);
#endif
    }
  }


  /* Small non-ellegant twick for stdin read				*/
  if( numfiles == 0){
    numfiles = 1;
    errflg += Process("(stdin)");
  }


  /* Here we preserve the spirit of CHKSUM but this will produce a VMS	*/
  /* shell printed error message as well so we avoid returning explicit	*/
  /* 0 under any OS.							*/
  return (errflg==0?EXIT_SUCCESS:errflg);
}


int Process(char *filin)
{
  register FILE *f;
  register int	status;
  
  
  /* Same common code							*/
  status = 0;
  if( strcmp(filin,"(stdin)") == 0){
    f = stdin;
  } else {
    if ( ( f = fopen(filin,"r") ) == NULL) {
      (void) fprintf (stderr, "chksum :: Error %d on open %s\n",errno,filin);
      return 10;
    }
  }

  if(method <= 1){
    status += compute_BSD_checksum(f,filin,method);
  } else if (method <= 3){
    status += compute_SYSV_checksum(f,filin,method-2);
  } else if (method == 4){
    status += compute_CRC_checksum(f,filin);
  } else if (method == 5){
    status += compute_MD5_checksum(f,filin);
  }
  if ( fclose (f) != 0){
    (void) fprintf(stderr,"chksum: Error %d while closing %s\n",errno,filin);
    status += 10;
  }
  
  return status;
}


int compute_BSD_checksum(FILE *f, char *flnm, int Control)
{
  register unsigned int	sum;
  register long		nbytes;
  register int		c;

  sum	= 0;
  nbytes= 0;

  while ((c = getc(f)) != EOF) {
    nbytes++;
    if (sum&01){
      sum = (sum>>1) + 0x8000;
    } else {
      sum >>= 1;
    }
    sum += c;
    sum &= 0xFFFF;
  }
  if (ferror (f)) {
    (void) fprintf (stderr, "chksum: read error on %s\n",flnm);
    return 1;
  } else {
    if(Control == 0){
      (void) fprintf (stdout,"%05u %6ld %s\n", sum, (nbytes+BUFSIZ-1)/BUFSIZ,flnm);
    } else {
      (void) fprintf (stdout,"%05u %6ld %s\n", sum, (nbytes+1024-1)/1024,flnm);
    }
    return 0;
  }
}


int compute_SYSV_checksum(FILE *f,char *flnm,int Control)
{
  unsigned char		buf[8192];
  register int		bytes_read;
  register unsigned long checksum = 0;
  register int		i;
  long int		total_bytes = 0;
  int			fl=fileno(f);


  while ((bytes_read = read (fl, buf, sizeof(buf) )) > 0){
    for (i = 0; i < bytes_read; i++) checksum += buf[i];
    total_bytes += bytes_read;
  }

  if (bytes_read < 0){
    (void) fprintf(stderr,"chksum: read error on %s\n",flnm);
    return 1;
  } else {
    if(Control == 0){
      (void) fprintf(stdout,"%05lu %6ld %s\n", checksum % 0xffff, (total_bytes + 512 - 1) / 512,flnm);
    } else {
      (void) fprintf(stdout,"%05lu %6ld %s\n", checksum % 0xffff, (total_bytes + 1024 - 1) / 1024,flnm);
    }
    return 0;
  }
}



/* This part of the code has been copied from the Unix cksum program	*/
/* written by Q. Frank Xia, qx@math.columbia.edu. cksum is Copyright 	*/
/* (C) 92, 1995-1999 Free Software Foundation, Inc.			*/

# define BUFLEN (1 << 16)
# define BIT(x)	( (unsigned long)1 << (x) )
# define SBIT	BIT(31)
# define GEN     (BIT(26)|BIT(23)|BIT(22)|BIT(16)|BIT(12)|BIT(11)|BIT(10)\
                |BIT(8) |BIT(7) |BIT(5) |BIT(4) |BIT(2) |BIT(1) |BIT(0));
static unsigned long const crctab[256] =
{
  0x0,
  0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B,
  0x1A864DB2, 0x1E475005, 0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6,
  0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,
  0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 0x5F15ADAC,
  0x5BD4B01B, 0x569796C2, 0x52568B75, 0x6A1936C8, 0x6ED82B7F,
  0x639B0DA6, 0x675A1011, 0x791D4014, 0x7DDC5DA3, 0x709F7B7A,
  0x745E66CD, 0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039,
  0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5, 0xBE2B5B58,
  0xBAEA46EF, 0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033,
  0xA4AD16EA, 0xA06C0B5D, 0xD4326D90, 0xD0F37027, 0xDDB056FE,
  0xD9714B49, 0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,
  0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 0xE13EF6F4,
  0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D, 0x34867077, 0x30476DC0,
  0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 0x2E003DC5,
  0x2AC12072, 0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16,
  0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA, 0x7897AB07,
  0x7C56B6B0, 0x71159069, 0x75D48DDE, 0x6B93DDDB, 0x6F52C06C,
  0x6211E6B5, 0x66D0FB02, 0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1,
  0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
  0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B,
  0xBB60ADFC, 0xB6238B25, 0xB2E29692, 0x8AAD2B2F, 0x8E6C3698,
  0x832F1041, 0x87EE0DF6, 0x99A95DF3, 0x9D684044, 0x902B669D,
  0x94EA7B2A, 0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E,
  0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2, 0xC6BCF05F,
  0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34,
  0xDC3ABDED, 0xD8FBA05A, 0x690CE0EE, 0x6DCDFD59, 0x608EDB80,
  0x644FC637, 0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB,
  0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 0x5C007B8A,
  0x58C1663D, 0x558240E4, 0x51435D53, 0x251D3B9E, 0x21DC2629,
  0x2C9F00F0, 0x285E1D47, 0x36194D42, 0x32D850F5, 0x3F9B762C,
  0x3B5A6B9B, 0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF,
  0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623, 0xF12F560E,
  0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65,
  0xEBA91BBC, 0xEF68060B, 0xD727BBB6, 0xD3E6A601, 0xDEA580D8,
  0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,
  0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 0xAE3AFBA2,
  0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B, 0x9B3660C6, 0x9FF77D71,
  0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD, 0x81B02D74,
  0x857130C3, 0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640,
  0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C, 0x7B827D21,
  0x7F436096, 0x7200464F, 0x76C15BF8, 0x68860BFD, 0x6C47164A,
  0x61043093, 0x65C52D24, 0x119B4BE9, 0x155A565E, 0x18197087,
  0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,
  0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D,
  0x2056CD3A, 0x2D15EBE3, 0x29D4F654, 0xC5A92679, 0xC1683BCE,
  0xCC2B1D17, 0xC8EA00A0, 0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB,
  0xDBEE767C, 0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18,
  0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4, 0x89B8FD09,
  0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662,
  0x933EB0BB, 0x97FFAD0C, 0xAFB010B1, 0xAB710D06, 0xA6322BDF,
  0xA2F33668, 0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4
};
		

int compute_CRC_checksum(FILE *f,char *flnm)
{
  unsigned char buf[BUFLEN];
  unsigned long crc = 0;
  long 		length = 0;
  long 		bytes_read;
  

  
  while ((bytes_read = fread (buf, 1, BUFLEN, f)) > 0){
    unsigned char *cp = buf;

    length += bytes_read;
    while (bytes_read--)
      crc = (crc << 8) ^ crctab[((crc >> 24) ^ *(cp++)) & 0xFF];
  }
   
  if (ferror (f)) {
    (void) fprintf (stderr, "chksum: read error on %s\n",flnm);
    return 1;
  } else {
    bytes_read = length;
    while (bytes_read > 0){
      crc = (crc << 8) ^ crctab[((crc >> 24) ^ bytes_read) & 0xFF];
      bytes_read >>= 8;
    }
    crc = ~crc & 0xFFFFFFFF;
    (void) fprintf(stdout,"%lu %ld %s\n", crc, length,flnm);
    return 0;
  }
}

/* This routine has been developped based on the md5sum program from 
   Ulrich Drepper <drepper@gnu.ai.mit.edu>. Note that the original program
   is distributed under the terms of the GNU Gpl as for this software	*/
   
int compute_MD5_checksum(FILE *f,char *flnm)
{
  unsigned char md5buffer[16];
  int		err,i;
  

  err = md5_stream (f, md5buffer);
   
  if (err) {
    (void) fprintf (stderr, "chksum: md5 stream error %d on %s\n",err,flnm);
    return 1;
  } else {
    for (i = 0; i < 16; ++i){
      (void) fprintf(stdout,"%02x", md5buffer[i]);
    }
    (void) fprintf(stdout," %s\n",flnm);
    return 0;
  }
}



void lhelp()
{
  printf("\nCHKSUM Version %s. Copyright © 1999-2001 Jérôme LAURET\n",VERSION);
#include "chksum_help.c"
}


