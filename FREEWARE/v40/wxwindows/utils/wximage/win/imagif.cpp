/*
 * File:	imagif.cc
 * Purpose:	Platform Independent GIF Image Class
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright: (c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 *
 */
#include "imagif.h"
#if WXIMA_SUPPORT_GIF

#include "imaiter.h"
#include "gifdecod.h"
#include <stdio.h>

// Force byte alignment (Borland)
#if defined  __BORLANDC__
#pragma option -a1
#elif defined _MSV_VER
#pragma option /Zp1
#endif

//#include <fstream.h>
//static ofstream log("x.log");

////////////////////////// AD - for interlace ///////////////////////////////
int interlaced, iypos, istep, iheight, ipass;
/////////////////////////////////////////////////////////////////////////////

#define GIFBUFTAM 16384

struct rgb { byte r,g,b; };

////////////////////////// AD - for transparency ///////////////////////////////
static struct {
  byte transpcolflag:1;
  byte userinputflag:1;
  byte dispmeth:3;
  byte res:3;
  byte delaytimehi;
  byte delaytimelo;
  byte transpcolindex;
} gifgce;

static struct {                 /* Logic Screen Descriptor  */
  char header[6];        /* Firma and version */
  USHORT scrwidth;
  USHORT scrheight;
  char pflds;
  char bcindx;
  char pxasrat;
} dscgif;

static struct {             /* Image Descriptor */
  USHORT l;
  USHORT t;
  USHORT w;
  USHORT h;
  byte   pf;
} image;

static struct {           /*   Tabla de colores  */
  SHORT colres; /* color resolution */
  SHORT sogct;  /* size of global color table */
  rgb paleta[256];    /*  paleta  */
} TabCol;

static ImaIter* iter;
static FILE *f;

static  int ibf = GIFBUFTAM+1;
static  byte buf[GIFBUFTAM];

int out_line(unsigned char *pixels, int linelen);

BOOL ImaGIF::readfile(char* ImageFileName)
{
  if (ImageFileName)
	 strcpy(filename, ImageFileName);
//  puts("Empezando... ");

  if ((f = fopen(filename, "rb"))==NULL) {
 //		printf("no se pudo abrir!");
	return FALSE;
  }

	fread((char*)&dscgif,/*sizeof(dscgif)*/13,1,f);
	 //if (strncmp(dscgif.header,"GIF8",3)!=0) {
	 if (strncmp(dscgif.header,"GIF8",4)!=0) {
//		puts("no es la firma!");
		return FALSE;
	 }

////////////////////////// AD - for interlace ///////////////////////////////

	 //TabCol.sogct = 1<<((dscgif.pflds & 0x0007) + 0x0001);
	 //TabCol.colres = (dscgif.pflds>>6) & 7 + 1;
	 TabCol.sogct = 1 << (dscgif.pflds & 0x07)+1;
	 TabCol.colres = ((int)(dscgif.pflds & 0x70) >> 3) + 1;

	 // Global colour map?

	 if (dscgif.pflds & 0x80)
	 {
		fread((char*)TabCol.paleta,/*sizeof(struct rgb)*/3*TabCol.sogct,1,f);
		//log << "Global colour map" << endl;
	 }

	 char ch;

loop:

	 if (fread(&ch, 1, 1, f) <= 0)
		goto done;

	 if (ch == '!')                     // extension
	 {
		unsigned char count;
		unsigned char fc;

		//log << "EXTENSION: ";

		if (fread(&fc, 1, 1, f) <= 0)   // function-code
			goto done;

		//log << hex << (int)fc << dec << endl;

		//////////////// AD - for transparency //////////////////////////
		if (fc == 0xF9)
		{
			//log << "Transparent" << endl;

			if (fread(&count, 1, 1, f) <= 0)
				goto done;
			if (fread(&gifgce, 1, /*sizeof(gifgce)*/4, f) != count)
				goto done;

			if (gifgce.transpcolflag)
				bgindex = gifgce.transpcolindex;

		///////////////////////////////////////////////////////////////////
		}

		while (fread(&count, 1, 1, f) && count)
		{
			//log << "Skipping " << count << " bytes" << endl;
			fseek(f, count, SEEK_CUR);
		}

		goto loop;
	 }

	 else if (ch == ',')                 // image
	 {
		fread((char*)&image,/*sizeof(image)*/9,1,f);
		//log << "Image header" << endl;

		// Local colour map?

		if (image.pf & 0x80)
		{
			TabCol.sogct = 1 << ((image.pf & 0x07) +1);
			fread((char*)TabCol.paleta,/*sizeof(struct rgb)*/3*TabCol.sogct,1,f);
			//log << "Local colour map" << endl;
		}

		Create(image.w, image.h, 8);

		if ((image.pf & 0x80) || (dscgif.pflds & 0x80))
		{
			unsigned char r[256], g[256], b[256];

			int i, has_white = 0;

			for (i=0; i < TabCol.sogct; i++)
			{
				r[i] = TabCol.paleta[i].r;
				g[i] = TabCol.paleta[i].g;
				b[i] = TabCol.paleta[i].b;

				if (RGB(r[i],g[i],b[i]) == 0xFFFFFF)
					has_white = 1;
			}

			// Make transparency colour black...
			if (bgindex != -1)
				r[bgindex] = g[bgindex] = b[bgindex] = 0;

			// Fill in with white // AD
			if (bgindex != -1)
			{
				while (i < 256)
				{
					has_white = 1;
					r[i] = g[i] = b[i] = 255;
					i++;
				}
			}

			// Force last colour to white...   // AD
			if ((bgindex != -1) && !has_white)
				r[255] = g[255] = b[255] = 255;

			SetColourMap((bgindex != -1 ? 256 : TabCol.sogct), r, g, b);
			//log << "Set colour map" << endl;
		}

		iter = new ImaIter(this);
		iter->upset();
		int badcode;
		ibf = GIFBUFTAM+1;
		GIFDecoder gifdec;

		interlaced = image.pf & 0x40;
		iheight = image.h;
		istep = 8;
		iypos = 0;
		ipass = 0;

		//if (interlaced) log << "Interlaced" << endl;

		gifdec.decoder(GetWidth(), badcode);
		delete iter;
		goto loop;
	 }

done:

	 fclose(f);

  return TRUE;
}

int get_byte()
{
  if (ibf>=GIFBUFTAM) {
	 fread(buf,GIFBUFTAM,1,f);
	 ibf = 0;
  }

  return buf[ibf++];
}

int out_line(unsigned char *pixels, int linelen)
{
////////////////////////// AD - for interlace ///////////////////////////////
  if (interlaced)
  {
	 iter->SetY(iheight-iypos-1);
	 iter->SetRow(pixels, linelen);

	 if ((iypos += istep) >= iheight)
	 {
		do
			{
				if (ipass++ > 0)
					istep /= 2;

				iypos = istep / 2;
			}
			  while (iypos > iheight);
	 }

	 return 0;
  }
  else
/////////////////////////////////////////////////////////////////////////////
  if (iter->ItOK()) {
	 iter->SetRow(pixels, linelen);
	 (void)iter->PrevRow();
	 return 0;
  }
  else {
//	 puts("chafeo");
	 return -1;
  }
}

#endif 	//	 WXIMA_SUPPORT_GIF
