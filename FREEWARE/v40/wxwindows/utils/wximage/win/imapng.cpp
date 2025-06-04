/*
 * File:	imajpg.cc
 * Purpose:	Platform Independent JPEG Image Class
 * Author:	Alejandro Aguilar Sierra
 * Created:	1995
 * Copyright: (c) 1995, Alejandro Aguilar Sierra <asierra@servidor.unam.mx>
 *
 *      logjmp
 */
#include "imapng.h"
#if WXIMA_SUPPORT_PNG
   
extern "C" {
#include "png.h"
}

#include "imaiter.h"

#include <wx_dialg.h>

void
ima_png_error(png_struct *png_ptr, char *message)
{
//        wxMessageBox(message, "PNG error");

	longjmp(png_ptr->jmpbuf, 1);
}

BOOL
ImaPNG::readfile(char * ImageFileName)
{
  int number_passes;

  if (ImageFileName)
	 strcpy(filename, ImageFileName);

	FILE *fp;
	png_struct *png_ptr;
   png_info *info_ptr;
	ImaIter iter(this);

	/* open the file */
	fp = fopen(filename, "rb");
	if (!fp)
		return FALSE;

	/* allocate the necessary structures */
	png_ptr = new (png_struct);
	if (!png_ptr)
	{
		fclose(fp);
		return FALSE;
	}

	info_ptr = new (png_info);
	if (!info_ptr)
	{
		fclose(fp);
		delete(png_ptr);
		return FALSE;
	}
	/* set error handling */
	if (setjmp(png_ptr->jmpbuf))
	{
		png_read_destroy(png_ptr, info_ptr, (png_info *)0);
		fclose(fp);
		delete(png_ptr);
		delete(info_ptr);

		/* If we get here, we had a problem reading the file */
		return FALSE;
	}
        //png_set_error(ima_png_error, NULL);

	/* initialize the structures, info first for error handling */
	png_info_init(info_ptr);
	png_read_init(png_ptr);

	/* set up the input control */
	png_init_io(png_ptr, fp);

	/* read the file information */
	png_read_info(png_ptr, info_ptr);

	/* allocate the memory to hold the image using the fields
		of png_info. */
	png_color_16 my_background={ 0, 31, 127, 255, 0 };

	if (info_ptr->valid & PNG_INFO_bKGD)
    {
		png_set_background(png_ptr, &(info_ptr->background),
			PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
        if ( info_ptr->num_palette > 0 )
            bgindex = info_ptr->background.index;
    }
	else   {
		png_set_background(png_ptr, &my_background,
			PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);
	 }

	/* tell libpng to strip 16 bit depth files down to 8 bits */
	if (info_ptr->bit_depth == 16)
		png_set_strip_16(png_ptr);

	int pixel_depth=(info_ptr->pixel_depth<24) ? info_ptr->pixel_depth: 24;
	Create(info_ptr->width, info_ptr->height, pixel_depth,
		info_ptr->color_type);

	if (info_ptr->num_palette>0)
	  SetColourMap((int)info_ptr->num_palette, (rgb_color_struct*)info_ptr->palette);

	int row_stride = info_ptr->width * ((pixel_depth+7)>>3);
 //	printf("P = %d D = %d RS= %d ", info_ptr->num_palette, info_ptr->pixel_depth,row_stride);
//	printf("CT = %d TRS = %d BD= %d ", info_ptr->color_type, info_ptr->valid & PNG_INFO_tRNS,info_ptr->bit_depth);

	byte *row_pointers = new byte[row_stride];

	/* turn on interlace handling */
	if (info_ptr->interlace_type)
		number_passes = png_set_interlace_handling(png_ptr);
	else
		number_passes = 1;
//	printf("NP = %d ", number_passes);

	for (int pass=0; pass< number_passes; pass++) {
	iter.upset();
	int y=0;
	do	{
//	  (unsigned char *)iter.GetRow();
	  if (info_ptr->interlace_type)  {
		 if (pass>0)
			iter.GetRow(row_pointers, row_stride);
		 png_read_row(png_ptr, row_pointers, NULL);
	  }
	  else
		 png_read_row(png_ptr, row_pointers, NULL);

	  iter.SetRow(row_pointers, row_stride);
	  y++;
	} while(iter.PrevRow());
//	printf("Y=%d ",y);
	}
	delete[] row_pointers;

	/* read the rest of the file, getting any additional chunks
		in info_ptr */
	png_read_end(png_ptr, info_ptr);

	/* clean up after the read, and free any memory allocated */
	png_read_destroy(png_ptr, info_ptr, (png_info *)0);

	/* free the structures */
	delete(png_ptr);
	delete(info_ptr);

	/* close the file */
	fclose(fp);

	/* that's it */
	return TRUE;
}


/* write a png file */

BOOL ImaPNG::savefile(char * ImageFileName)
{
  if (ImageFileName)
	 strcpy(filename, ImageFileName);

  ImaIter iter(this);
  FILE *fp;
   png_struct *png_ptr;
	png_info *info_ptr;

   /* open the file */
	fp = fopen(filename, "wb");
	if (!fp)
		return FALSE;

	/* allocate the necessary structures */
	png_ptr = new (png_struct);
	if (!png_ptr)
	{
		fclose(fp);
		return FALSE;
	}

	info_ptr = new (png_info);
	if (!info_ptr)
	{
		fclose(fp);
		delete(png_ptr);
		return FALSE;
	}

	/* set error handling */
	if (setjmp(png_ptr->jmpbuf))
	{
		png_write_destroy(png_ptr);
		fclose(fp);
		delete(png_ptr);
		delete(info_ptr);

		/* If we get here, we had a problem reading the file */
		return FALSE;
	}
        //png_set_error(ima_png_error, NULL);

//	printf("writig pg %s ", filename);
   /* initialize the structures */
	png_info_init(info_ptr);
	png_write_init(png_ptr);
            
	int row_stride = GetWidth() * ((GetDepth()+7)>>3);
	/* set up the output control */
   png_init_io(png_ptr, fp);

	/* set the file information here */
	info_ptr->width = GetWidth();
	info_ptr->height = GetHeight();
	info_ptr->pixel_depth = GetDepth();
	info_ptr->channels = (GetDepth()>8) ? 3: 1;
	info_ptr->bit_depth = GetDepth()/info_ptr->channels;
	info_ptr->color_type = GetColorType();
	info_ptr->compression_type = info_ptr->filter_type = info_ptr->interlace_type=0;
	info_ptr->valid = 0;
	info_ptr->rowbytes = row_stride;


// printf("P = %d D = %d RS= %d GD= %d CH= %d ", info_ptr->pixel_depth, info_ptr->bit_depth, row_stride, GetDepth(), info_ptr->channels);
	/* set the palette if there is one */
	if ((GetColorType() & COLORTYPE_PALETTE) && GetColourMap())
	{
//	  printf("writing paleta[%d %d %x]",GetColorType() ,COLORTYPE_PALETTE, GetColourMap());
	  info_ptr->valid |= PNG_INFO_PLTE;
	  info_ptr->palette = new png_color[256];
	  info_ptr->num_palette = 256;
	  for (int i=0; i<256; i++)
		 GetColourMap()->GetRGB(i, &info_ptr->palette[i].red, &info_ptr->palette[i].green, &info_ptr->palette[i].blue);
	}  
//	  printf("Paleta [%d %d %x]",GetColorType() ,COLORTYPE_PALETTE, GetColourMap());


   /* optional significant bit chunk */
//   info_ptr->valid |= PNG_INFO_sBIT;
//   info_ptr->sig_bit = true_bit_depth;

	/* optional gamma chunk */
//   info_ptr->valid |= PNG_INFO_gAMA;
//   info_ptr->gamma = gamma;

	/* other optional chunks */

   /* write the file information */
   png_write_info(png_ptr, info_ptr);

   /* set up the transformations you want.  Note that these are
      all optional.  Only call them if you want them */

	/* shift the pixels up to a legal bit depth and fill in
      as appropriate to correctly scale the image */
//   png_set_shift(png_ptr, &(info_ptr->sig_bit));

	/* pack pixels into bytes */
//   png_set_packing(png_ptr);

	/* flip bgr pixels to rgb */
//   png_set_bgr(png_ptr);

   /* swap bytes of 16 bit files to most significant bit first */
//   png_set_swap(png_ptr);

   /* get rid of filler bytes, pack rgb into 3 bytes */
//   png_set_rgbx(png_ptr);

/* If you are only writing one row at a time, this works */

	byte *row_pointers = new byte[row_stride];
  	iter.upset();
	do	{
//	  (unsigned char *)iter.GetRow();
	  iter.GetRow(row_pointers, row_stride);
	  png_write_row(png_ptr, row_pointers);
	} while(iter.PrevRow());
        
        delete[] row_pointers;

/* write the rest of the file */
   png_write_end(png_ptr, info_ptr);

	/* clean up after the write, and free any memory allocated */
   png_write_destroy(png_ptr);

   /* if you malloced the palette, free it here */
   if (info_ptr->palette)
		delete[] (info_ptr->palette);

	/* free the structures */
	delete(png_ptr);
	delete(info_ptr);

	/* close the file */
	fclose(fp);

	/* that's it */
	return TRUE;
}


#endif // WXIMA_SUPPORT_PNG

