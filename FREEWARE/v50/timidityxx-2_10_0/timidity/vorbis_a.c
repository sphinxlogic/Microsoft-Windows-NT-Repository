/*
    TiMidity++ -- MIDI to WAVE converter and player
    Copyright (C) 1999,2000 Masanao Izumo <mo@goice.co.jp>
    Copyright (C) 1995 Tuukka Toivonen <tt@cgs.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    ogg_a.c

    Functions to output Ogg Vorbis (*.ogg).
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */
#include <stdio.h>

#ifdef __W32__
#include <stdlib.h>
#include <io.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#ifndef NO_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include <fcntl.h>
#include <vorbis/modes.h>

#include "timidity.h"
#include "common.h"
#include "output.h"
#include "controls.h"
#include "instrum.h"
#include "playmidi.h"
#include "readmidi.h"

static int open_output(void); /* 0=success, 1=warning, -1=fatal error */
static void close_output(void);
static int output_data(char *buf, int32 bytes);
static int acntl(int request, void *arg);

/* export the playback mode */
#define dpm vorbis_play_mode

PlayMode dpm = {
    44100, PE_16BIT|PE_SIGNED, PF_PCM_STREAM,
    -1,
    {0,0,0,0,0},
    "Ogg Vorbis", 'v',
    NULL,
    open_output,
    close_output,
    output_data,
    acntl
};

static  ogg_stream_state os; /* take physical pages, weld into a logical
				stream of packets */
static  vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
static  vorbis_block     vb; /* local working space for packet->PCM decode */


/*************************************************************************/

static int ogg_output_open(const char *fname, const char *comment)
{
  int fd;
  static vorbis_info ogg_info;
  vorbis_info *vi; /* struct that stores all the static vorbis bitstream
		      settings */
  vorbis_comment vc; /* struct that stores all the user comments */


  if(strcmp(fname, "-") == 0) {
    fd = 1; /* data to stdout */
    if(comment == NULL)
      comment = "(stdout)";
  } else {
    /* Open the audio file */
    fd = open(fname, FILE_OUTPUT_MODE);
    if(fd < 0) {
      ctl->cmsg(CMSG_ERROR, VERB_NORMAL, "%s: %s",
		fname, strerror(errno));
      return -1;
    }
    if(comment == NULL)
      comment = fname;
  }

  /* choose an encoding mode */
  /* (mode 0: 44kHz stereo uncoupled, roughly 128kbps VBR) */
  memcpy(&ogg_info, &info_A, sizeof(ogg_info));
  if(dpm.encoding & PE_MONO)
    ogg_info.channels = 1;
  else
    ogg_info.channels = 2;
  ogg_info.rate = dpm.rate;

  vi = &ogg_info;

  /* add a comment */
  vorbis_comment_init(&vc);
  vorbis_comment_add(&vc, (char *)comment);

  /* set up the analysis state and auxiliary encoding storage */
  vorbis_analysis_init(&vd, vi);
  vorbis_block_init(&vd, &vb);

  /* set up our packet->stream encoder */
  /* pick a random serial number; that way we can more likely build
     chained streams just by concatenation */
  srand(time(NULL));
  ogg_stream_init(&os, rand());

  /* Vorbis streams begin with three headers; the initial header (with
     most of the codec setup parameters) which is mandated by the Ogg
     bitstream spec.  The second header holds any comment fields.  The
     third header holds the bitstream codebook.  We merely need to
     make the headers, then pass them to libvorbis one at a time;
     libvorbis handles the additional Ogg bitstream constraints */

  {
    ogg_packet header;
    ogg_packet header_comm;
    ogg_packet header_code;

    vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);
    ogg_stream_packetin(&os, &header); /* automatically placed in its own
					  page */
    ogg_stream_packetin(&os, &header_comm);
    ogg_stream_packetin(&os, &header_code);

    /* no need to write out here.  We'll get to that in the main loop */
  }

  return fd;
}

static int auto_ogg_output_open(const char *input_filename)
{
  char *output_filename = (char *)safe_malloc(strlen(input_filename) + 5);
  char *ext, *p;

  strcpy(output_filename, input_filename);
  if((ext = strrchr(output_filename, '.')) == NULL)
    ext = output_filename + strlen(output_filename);
  else {
    /* strip ".gz" */
    if(strcasecmp(ext, ".gz") == 0) {
      *ext = '\0';
      if((ext = strrchr(output_filename, '.')) == NULL)
	ext = output_filename + strlen(output_filename);
    }
  }

  /* replace '.' and '#' before ext */
  for(p = output_filename; p < ext; p++)
    if(*p == '.' || *p == '#')
      *p = '_';

  if(*ext && isupper(*(ext + 1)))
    strcpy(ext, ".OGG");
  else
    strcpy(ext, ".ogg");
  if((dpm.fd = ogg_output_open(output_filename, input_filename)) == -1) {
    free(output_filename);
    return -1;
  }
  if(dpm.name != NULL)
    free(dpm.name);
  dpm.name = output_filename;
  ctl->cmsg(CMSG_INFO, VERB_NORMAL, "Output %s", dpm.name);
  return 0;
}

static int open_output(void)
{
  int include_enc, exclude_enc;

  /********** Encode setup ************/

  include_enc = exclude_enc = 0;

  /* only 16 bit is supported */
  include_enc |= PE_16BIT|PE_SIGNED;
  exclude_enc |= PE_BYTESWAP;
  dpm.encoding = validate_encoding(dpm.encoding, include_enc, exclude_enc);

  if(dpm.name == NULL) {
    dpm.flag |= PF_AUTO_SPLIT_FILE;
    dpm.name = NULL;
  } else {
    dpm.flag &= ~PF_AUTO_SPLIT_FILE;
    if((dpm.fd = ogg_output_open(dpm.name, NULL)) == -1)
      return -1;
  }

  return 0;
}

static int output_data(char *readbuffer, int32 bytes)
{
  int i, j, ch = ((dpm.encoding & PE_MONO) ? 1 : 2);
  double **buffer;
  int16 *samples = (int16 *)readbuffer;
  int nsamples = bytes / (2 * ch);
  ogg_page   og; /* one Ogg bitstream page.  Vorbis packets are inside */
  ogg_packet op; /* one raw packet of data for decode */

  /* data to encode */

  /* expose the buffer to submit data */
  buffer = vorbis_analysis_buffer(&vd, nsamples);
      
  /* uninterleave samples */
  for(j = 0; j < ch; j++)
    for(i = 0; i < nsamples; i++)
      buffer[j][i] = samples[i*ch+j] * (1.0/32768.0);

  /* tell the library how much we actually submitted */
  vorbis_analysis_wrote(&vd, nsamples);

  /* vorbis does some data preanalysis, then divvies up blocks for
     more involved (potentially parallel) processing.  Get a single
     block for encoding now */
  while(vorbis_analysis_blockout(&vd, &vb) == 1) {

    /* analysis */
    vorbis_analysis(&vb, &op);

    /* weld the packet into the bitstream */
    ogg_stream_packetin(&os, &op);

    /* write out pages (if any) */
    while(ogg_stream_pageout(&os, &og) != 0) {
      write(dpm.fd, og.header, og.header_len);
      write(dpm.fd, og.body, og.body_len);
    }
  }
  return 0;
}

static void close_output(void)
{
  int eos = 0;
  ogg_page   og; /* one Ogg bitstream page.  Vorbis packets are inside */
  ogg_packet op; /* one raw packet of data for decode */

  if(dpm.fd < 0)
    return;

  /* end of file.  this can be done implicitly in the mainline,
     but it's easier to see here in non-clever fashion.
     Tell the library we're at end of stream so that it can handle
     the last frame and mark end of stream in the output properly */
  vorbis_analysis_wrote(&vd, 0);

  /* vorbis does some data preanalysis, then divvies up blocks for
     more involved (potentially parallel) processing.  Get a single
     block for encoding now */
  while(vorbis_analysis_blockout(&vd, &vb) == 1) {

    /* analysis */
    vorbis_analysis(&vb, &op);
      
    /* weld the packet into the bitstream */
    ogg_stream_packetin(&os, &op);

    /* write out pages (if any) */
    while(!eos){
      int result = ogg_stream_pageout(&os,&og);
      if(result == 0)
	break;
      write(dpm.fd, og.header, og.header_len);
      write(dpm.fd, og.body, og.body_len);

      /* this could be set above, but for illustrative purposes, I do
	 it here (to show that vorbis does know where the stream ends) */

      if(ogg_page_eos(&og))
	eos = 1;
    }
  }

  /* clean up and exit.  vorbis_info_clear() must be called last */

  ogg_stream_clear(&os);
  vorbis_block_clear(&vb);
  vorbis_dsp_clear(&vd);
  close(dpm.fd);
  dpm.fd = -1;
}

static int acntl(int request, void *arg)
{
  switch(request) {
  case PM_REQ_PLAY_START:
    if(dpm.flag & PF_AUTO_SPLIT_FILE)
      return auto_ogg_output_open(current_file_info->filename);
    break;
  case PM_REQ_PLAY_END:
    if(dpm.flag & PF_AUTO_SPLIT_FILE) {
      close_output();
      return 0;
    }
    break;
  case PM_REQ_DISCARD:
    return 0;
  }
  return -1;
}
