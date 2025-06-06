/*
    TiMidity -- MIDI to WAVE converter and player
    Copyright (C) 1999,2000 Masanao Izumo <mo@goice.co.jp>
    Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

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

    w32g_c.c: written by Daisuke Aoki <dai@y7.net>
                         Masanao Izumo <mo@goice.co.jp>
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifndef NO_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include "timidity.h"
#include "common.h"
#include "output.h"
#include "instrum.h"
#include "playmidi.h"
#include "readmidi.h"
#include "controls.h"
#include "miditrace.h"
#include "strtab.h"
#include "aq.h"
#include "timer.h"

#include "w32g.h"
#include "w32g_subwin.h"

#include <process.h>


volatile int w32g_play_active;
volatile int w32g_restart_gui_flag = 0;
int w32g_current_volume[MAX_CHANNELS];
int w32g_current_expression[MAX_CHANNELS];
static int mark_apply_setting = 0;
PanelInfo *Panel = NULL;
static void CanvasUpdateInterval(void);
static void ctl_panel_refresh(void);

//****************************************************************************/
// Control funcitons

static int ctl_open(int using_stdin, int using_stdout);
static void ctl_close(void);
static void ctl_pass_playing_list(int number_of_files, char *list_of_files[]);
static void ctl_event(CtlEvent *e);
static int ctl_read(int32 *valp);
static int cmsg(int type, int verbosity_level, char *fmt, ...);

#define ctl w32gui_control_mode

ControlMode ctl=
{
    "Win32 GUI interface", 'w',
    1,1,0,
    CTLF_AUTOSTART | CTLF_LIST_LOOP | CTLF_DRAG_START,
    ctl_open,
    ctl_close,
    ctl_pass_playing_list,
    ctl_read,
    cmsg,
    ctl_event
};

static int ctl_open(int using_stdin, int using_stdout)
{
    if(ctl.opened)
	return 0;
    ctl.opened = 1;
    set_trace_loop_hook(CanvasUpdateInterval);

    /* Initialize Panel */
    Panel = (PanelInfo *)safe_malloc(sizeof(PanelInfo));
    memset((void *)Panel,0,sizeof(PanelInfo));
    Panel->changed = 1;

    return w32g_open();
}

static void ctl_close(void)
{
    if(ctl.opened)
    {
	w32g_close();
	ctl.opened = 0;
	free(Panel);
    }
}

static void PanelReset(void)
{
    int i, j;

    Panel->reset_panel = 0;
    Panel->multi_part = 0;
    Panel->wait_reset = 0;
    Panel->cur_time = 0;
    Panel->cur_time_h = 0;
    Panel->cur_time_m = 0;
    Panel->cur_time_s = 0;
    Panel->cur_time_ss = 0;
    for(i = 0; i < MAX_W32G_MIDI_CHANNELS; i++)
    {
	Panel->v_flags[i] = 0;
	Panel->cnote[i] = 0;
	Panel->cvel[i] = 0;
	Panel->ctotal[i] = 0;
	Panel->c_flags[i] = 0;
	for(j = 0; j < 4; j++)
	    Panel->xnote[i][j] = 0;
//	Panel->channel[i].panning = 64;
	Panel->channel[i].panning = -1;
	Panel->channel[i].sustain = 0;
	Panel->channel[i].expression = 0;
	Panel->channel[i].volume = 0;
//	Panel->channel[i].pitchbend = 0x2000;
	Panel->channel[i].pitchbend = -2;
    }
    Panel->titlename[0] = '\0';
    Panel->filename[0] = '\0';
    Panel->titlename_setflag = 0;
    Panel->filename_setflag = 0;
    Panel->cur_voices = 0;
    Panel->voices = voices;
    Panel->upper_voices = 0;
  //  Panel->master_volume = 0;
    Panel->aq_ratio = 0;
    Panel->changed = 1;
}

static void CanvasUpdateInterval(void)
{
    static double lasttime;
    double t;

    if(CanvasGetMode() == CANVAS_MODE_MAP)
    {
	t = get_current_calender_time();
	if(t - lasttime > 0.05)
	{
	    CanvasReadPanelInfo(0);
	    CanvasUpdate(0);
	    CanvasPaint();
	    lasttime = t;
	}
    }
}

static int ctl_drop_file(HDROP hDrop)
{
    StringTable st;
    int i, n, len;
    char buffer[BUFSIZ];
    char **files;
    int prevnfiles;

    w32g_get_playlist_index(NULL, &prevnfiles, NULL);

    init_string_table(&st);
    n = DragQueryFile(hDrop,0xffffffffL, NULL, 0);
    for(i = 0; i < n; i++)
    {
	DragQueryFile(hDrop, i, buffer, sizeof(buffer));
	if(is_directory(buffer))
	    directory_form(buffer);
	len = strlen(buffer);
	put_string_table(&st, buffer, strlen(buffer));
    }
    DragFinish(hDrop);

    if((files = make_string_array(&st)) == NULL)
	n = 0;
    else
    {
	n = w32g_add_playlist(n, files, 1,
			      ctl.flags & CTLF_AUTOUNIQ,
			      ctl.flags & CTLF_AUTOREFINE);
	free(files[0]);
	free(files);
    }
    if(n > 0)
    {
	ctl_panel_refresh();
	if(ctl.flags & CTLF_DRAG_START)
	{
	    w32g_goto_playlist(prevnfiles, !(ctl.flags & CTLF_NOT_CONTINUE));
	    return RC_LOAD_FILE;
	}
    }
    return RC_NONE;
}

static int ctl_load_file(char *fileptr)
{
    StringTable st;
    int len, n;
    char **files;
    char buffer[BUFSIZ];
    char *basedir;

    init_string_table(&st);
    n = 0;
    basedir = fileptr;
    fileptr += strlen(fileptr) + 1;
    while(*fileptr)
    {
	snprintf(buffer, sizeof(buffer), "%s\\%s", basedir, fileptr);
	if(is_directory(buffer))
	    directory_form(buffer);
	len = strlen(buffer);
	put_string_table(&st, buffer, len);
	n++;
	fileptr += strlen(fileptr) + 1;
    }

    if(n == 0)
    {
	put_string_table(&st, basedir, strlen(basedir));
	n++;
    }

    files = make_string_array(&st);
    n = w32g_add_playlist(n, files, 1,
			  ctl.flags & CTLF_AUTOUNIQ,
			  ctl.flags & CTLF_AUTOREFINE);
    free(files[0]);
    free(files);

    if(n > 0)
	ctl_panel_refresh();
    w32g_lock_open_file = 0;
    return RC_NONE;
}

static int ctl_load_files_and_play(argc_argv_t *argc_argv, int playflag)
{
    StringTable st;
    int i, n, len;
    char buffer[BUFSIZ];
    char **files;
    int prevnfiles;

	if(argc_argv==NULL)
	    return RC_NONE;
    
	w32g_get_playlist_index(NULL, &prevnfiles, NULL);

    init_string_table(&st);
	n = argc_argv->argc;
    for(i = 0; i < n; i++)
    {
	strcpy(buffer,(argc_argv->argv)[i]);
	if(is_directory(buffer))
	    directory_form(buffer);
	len = strlen(buffer);
	put_string_table(&st, buffer, strlen(buffer));
    }
#if 1
	for(i=0;i<argc_argv->argc;i++){
		free(argc_argv->argv[i]);
	}
	free(argc_argv->argv);
	argc_argv->argv = NULL;
	argc_argv->argc = 0;
#endif
    if((files = make_string_array(&st)) == NULL)
	n = 0;
    else
    {
	n = w32g_add_playlist(n, files, 1,
			      ctl.flags & CTLF_AUTOUNIQ,
			      ctl.flags & CTLF_AUTOREFINE);
	free(files[0]);
	free(files);
    }
    if(n > 0)
    {
	ctl_panel_refresh();
	if(playflag)
	{
	    w32g_goto_playlist(prevnfiles, !(ctl.flags & CTLF_NOT_CONTINUE));
	    return RC_LOAD_FILE;
	}
    }
    return RC_NONE;
}

static int ctl_load_playlist(char *fileptr)
{
    StringTable st;
    int n;
    char **files;
    char buffer[BUFSIZ];
    char *basedir;

    init_string_table(&st);
    n = 0;
    basedir = fileptr;
    fileptr += strlen(fileptr) + 1;
    while(*fileptr)
    {
	snprintf(buffer, sizeof(buffer), "@%s\\%s", basedir, fileptr);
	put_string_table(&st, buffer, strlen(buffer));
	n++;
	fileptr += strlen(fileptr) + 1;
    }

    if(n == 0)
    {
	buffer[0] = '@';
	strncpy(buffer + 1, basedir, sizeof(buffer) - 1);
	put_string_table(&st, buffer, strlen(buffer));
	n++;
    }

    files = make_string_array(&st);
    n = w32g_add_playlist(n, files, 1,
			  ctl.flags & CTLF_AUTOUNIQ,
			  ctl.flags & CTLF_AUTOREFINE);
    free(files[0]);
    free(files);

    if(n > 0)
	ctl_panel_refresh();
    w32g_lock_open_file = 0;
    return RC_NONE;
}

static int ctl_save_playlist(char *fileptr)
{
    FILE *fp;
    char *filename;
    int i, nfiles;

    if((fp = fopen(fileptr, "w")) == NULL)
    {
	w32g_lock_open_file = 0;
	cmsg(CMSG_FATAL, VERB_NORMAL, "%s: %s", fileptr, strerror(errno));
	w32g_lock_open_file = 0;
	return RC_NONE;
    }

    w32g_get_playlist_index(NULL, &nfiles, NULL);
    for(i = 0; i < nfiles; i++)
    {
	fputs(w32g_get_playlist(i), fp);
	fputs("\n", fp);
    }

    fclose(fp);
    w32g_lock_open_file = 0;
    return RC_NONE;
}

static int ctl_delete_playlist(int offset)
{
    int selected, nfiles, cur, pos;

    w32g_get_playlist_index(&selected, &nfiles, &cur);
    pos = cur + offset;
    if(pos < 0 || pos >= nfiles)
	return RC_NONE;
    if(w32g_delete_playlist(pos))
    {
	ctl_panel_refresh();
	if(w32g_play_active && selected == pos)
	    return RC_LOAD_FILE;
    }
    return RC_NONE;
}

static int ctl_uniq_playlist(void)
{
    int n, stop;
    n = w32g_uniq_playlist(&stop);
    if(n > 0)
    {
	ctl_panel_refresh();
	if(stop)
	    return RC_STOP;
    }
    return RC_NONE;
}

static int ctl_refine_playlist(void)
{
    int n, stop;
    n = w32g_refine_playlist(&stop);
    if(n > 0)
    {
	ctl_panel_refresh();
	if(stop)
	    return RC_STOP;
    }
    return RC_NONE;
}

static int w32g_ext_control(int rc, int32 value)
{
    int i;

    switch(rc)
    {
      case RC_EXT_DROP:
	return ctl_drop_file((HDROP)value);
      case RC_EXT_LOAD_FILE:
	return ctl_load_file((char *)value);
      case RC_EXT_LOAD_FILES_AND_PLAY:
	return ctl_load_files_and_play((argc_argv_t *)value, 1);
      case RC_EXT_LOAD_PLAYLIST:
	return ctl_load_playlist((char *)value);
      case RC_EXT_SAVE_PLAYLIST:
	return ctl_save_playlist((char *)value);
      case RC_EXT_MODE_CHANGE:
	CanvasChange(value);
	break;
      case RC_EXT_APPLY_SETTING:
	if(w32g_play_active) {
	    mark_apply_setting = 1;
	    return RC_STOP;
	}
	PrefSettingApplyReally();
	mark_apply_setting = 0;
	break;
      case RC_EXT_DELETE_PLAYLIST:
	return ctl_delete_playlist(value);
      case RC_EXT_UPDATE_PLAYLIST:
	w32g_update_playlist();
	break;
      case RC_EXT_UNIQ_PLAYLIST:
	return ctl_uniq_playlist();
      case RC_EXT_REFINE_PLAYLIST:
	return ctl_refine_playlist();
      case RC_EXT_JUMP_FILE:
	if(w32g_goto_playlist(value, !(ctl.flags & CTLF_NOT_CONTINUE)))
	    return RC_LOAD_FILE;
      case RC_EXT_ROTATE_PLAYLIST:
	w32g_rotate_playlist(value);
	ctl_panel_refresh();
	break;
      case RC_EXT_CLEAR_PLAYLIST:
	w32g_clear_playlist();
	ctl_panel_refresh();
	return RC_STOP;
      case RC_EXT_OPEN_DOC:
	w32g_setup_doc(value);
	w32g_open_doc(0);
	break;
    }
    return RC_NONE;
}

static int ctl_read(int32 *valp)
{
    int rc;

    rc = w32g_get_rc(valp, play_pause_flag);
    if(rc >= RC_EXT_BASE)
	return w32g_ext_control(rc, *valp);
    return rc;
}

static int cmsg(int type, int verbosity_level, char *fmt, ...)
{
    char buffer[BUFSIZ];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    if((type==CMSG_TEXT || type==CMSG_INFO || type==CMSG_WARNING) &&
       ctl.verbosity<verbosity_level)
	return 0;
    if(type == CMSG_FATAL)
	w32g_msg_box(buffer, "TiMidity Error", MB_OK);
    PutsConsoleWnd(buffer);
    PutsConsoleWnd("\n");
    return 0;
}

static void ctl_panel_refresh(void)
{
    MPanelReadPanelInfo(0);
    MPanelUpdate();
    MPanelPaint();
}

static void ctl_master_volume(int mv)
{
    Panel->master_volume = mv;
    Panel->changed = 1;
    ctl_panel_refresh();
}

static void ctl_pass_playing_list(int number_of_files, char *list_of_files[])
{
    int rc;
    int32 value;
    extern void timidity_init_aq_buff(void);
    int errcnt;

    w32g_add_playlist(number_of_files, list_of_files, 0,
		      ctl.flags & CTLF_AUTOUNIQ,
		      ctl.flags & CTLF_AUTOREFINE);
    w32g_play_active = 0;
    errcnt = 0;

    if(play_mode->fd != -1 &&
       w32g_nvalid_playlist() && (ctl.flags & CTLF_AUTOSTART))
	rc = RC_LOAD_FILE;
    else
	rc = RC_NONE;

    while(1)
    {
	if(rc == RC_NONE)
	{
	    if(play_mode->fd != -1)
	    {
		aq_flush(1);
		play_mode->close_output();
	    }
	    rc = w32g_get_rc(&value, 1);
	}

      redo:
	switch(rc)
	{
	  case RC_NONE:
	    Sleep(1000);
	    break;

	  case RC_LOAD_FILE: /* Play playlist.selected */
	    if(w32g_nvalid_playlist())
	    {
		int selected;
		w32g_get_playlist_index(&selected, NULL, NULL);
		w32g_play_active = 1;
		if(play_mode->fd == -1)
		{
		    int err;
		    if(play_mode->open_output() == -1)
		    {
			ctl.cmsg(CMSG_FATAL, VERB_NORMAL,
				 "Couldn't open %s (`%c') %s",
				 play_mode->id_name,
				 play_mode->id_character,
				 play_mode->name ? play_mode->name : "");
			break;
		    }
		    aq_setup();
		    timidity_init_aq_buff();
		}
		w32g_setcur_playlist();
		if(play_mode->id_character == 'l')
		    w32g_show_console();
		if(!DocWndIndependent){
			w32g_setup_doc(selected);
		    w32g_open_doc(1);
		}
		{
			char *p = w32g_get_playlist(selected);
			if(Panel!=NULL && p!=NULL)
				strcpy(Panel->filename,p);
		}
		rc = play_midi_file(w32g_get_playlist(selected));

		if(ctl.flags & CTLF_NOT_CONTINUE)
		    w32g_update_playlist(); /* Update mark of error */
		if(rc == RC_ERROR)
		{
		    int nfiles;
		    errcnt++;
		    w32g_get_playlist_index(NULL, &nfiles, NULL);
		    if(errcnt >= nfiles)
			w32g_msg_box("No MIDI file to play",
				     "TiMidity Warning", MB_OK);
		}
		else
		    errcnt = 0;
		w32g_play_active = 0;
		goto redo;
	    }
	    break;

	  case RC_ERROR:
	  case RC_TUNE_END:
	    if(play_mode->id_character != 'd' ||
	       (ctl.flags & CTLF_NOT_CONTINUE))
		break;
	    /* FALLTHROUGH */
	  case RC_NEXT:
	    if(!w32g_nvalid_playlist())
	    {
		if(ctl.flags & CTLF_AUTOEXIT) {
		    if(play_mode->fd != -1)
			aq_flush(0);
		    return;
		}
		break;
	    }
	    if(w32g_next_playlist(!(ctl.flags & CTLF_NOT_CONTINUE)))
	    {
		rc = RC_LOAD_FILE;
		goto redo;
	    }
	    else
	    {
		/* end of list */
		if(ctl.flags & CTLF_AUTOEXIT){
		    if(play_mode->fd != -1)
			aq_flush(0);
		    return;
		}
		if((ctl.flags & CTLF_LIST_LOOP) && w32g_nvalid_playlist())
		{
		    w32g_first_playlist(!(ctl.flags & CTLF_NOT_CONTINUE));
		    rc = RC_LOAD_FILE;
		    goto redo;
		}
	    }
	    break;

	  case RC_REALLY_PREVIOUS:
	    if(w32g_prev_playlist(!(ctl.flags & CTLF_NOT_CONTINUE)))
	    {
		rc = RC_LOAD_FILE;
		goto redo;
	    }
	    break;

	  case RC_QUIT:
	    if(play_mode->fd != -1)
		aq_flush(1);
	    return;

	  case RC_CHANGE_VOLUME:
	    amplification += value;
	    ctl_master_volume(amplification);
	    break;

	  case RC_TOGGLE_PAUSE:
	    play_pause_flag = !play_pause_flag;
	    break;

	  default:
	    if(rc >= RC_EXT_BASE)
	    {
		rc = w32g_ext_control(rc, value);
		if(rc != RC_NONE)
		    goto redo;
	    }
	    break;
	}

	if(mark_apply_setting)
	    PrefSettingApplyReally();
	rc = RC_NONE;
    }
}


static void ctl_channel_note(int ch, int note, int vel)
{
    if (vel == 0) {
	if (note == Panel->cnote[ch])
	    Panel->v_flags[ch] = FLAG_NOTE_OFF;
   	Panel->cvel[ch] = 0;
    } else if (vel > Panel->cvel[ch]) {
	Panel->cvel[ch] = vel;
	Panel->cnote[ch] = note;
	Panel->ctotal[ch] = ( vel * Panel->channel[ch].volume *
			     Panel->channel[ch].expression ) >> 14;
//	   	Panel->channel[ch].expression / (127*127);
	Panel->v_flags[ch] = FLAG_NOTE_ON;
    }
    Panel->changed = 1;
}

static void ctl_note(int status, int ch, int note, int vel)
{
    int32 i, n;

    if(!ctl.trace_playing)
	return;
    if(ch < 0 || ch >= MAX_W32G_MIDI_CHANNELS)
	return;

    if(status != VOICE_ON)
	vel = 0;

    switch(status) {
      case VOICE_SUSTAINED:
      case VOICE_DIE:
      case VOICE_FREE:
      case VOICE_OFF:
	n = note;
	i = 0;
	if(n<0) n = 0;
	if(n>127) n = 127;
	while(n >= 32){
	    n -= 32;
	    i++;
	}
	Panel->xnote[ch][i] &= ~(((int32)1) << n);
	break;
      case VOICE_ON:
	n = note;
	i = 0;
	if(n<0) n = 0;
	if(n>127) n = 127;
	while(n >= 32){
	    n -= 32;
	    i++;
	}
	Panel->xnote[ch][i] |= ((int32)1) << n;
	break;
    }
    ctl_channel_note(ch, note, vel);
}

static void ctl_volume(int ch, int val)
{
    if(ch >= MAX_W32G_MIDI_CHANNELS)
	return;
    if(!ctl.trace_playing)
	return;

    Panel->channel[ch].volume = val;
    ctl_channel_note(ch, Panel->cnote[ch], Panel->cvel[ch]);
}

static void ctl_expression(int ch, int val)
{
    if(ch >= MAX_W32G_MIDI_CHANNELS)
	return;
    if(!ctl.trace_playing)
	return;

    Panel->channel[ch].expression = val;
    ctl_channel_note(ch, Panel->cnote[ch], Panel->cvel[ch]);
}

static void ctl_current_time(int secs, int nvoices)
{
    int32 centisecs = secs * 100;

    Panel->cur_time = centisecs;
    Panel->cur_time_h = centisecs/100/60/60;
    centisecs %= 100*60*60;
    Panel->cur_time_m = centisecs/100/60;
    centisecs %= 100*60;
    Panel->cur_time_s = centisecs/100;
    centisecs %= 100;
    Panel->cur_time_ss = centisecs;
    Panel->cur_voices = nvoices;
    Panel->changed = 1;
}

static void display_aq_ratio(void)
{
    static int last_rate = -1;
    int rate, devsiz;

    if((devsiz = aq_get_dev_queuesize()) == 0)
	return;

    rate = (int)(((double)(aq_filled() + aq_soft_filled()) / devsiz)
		 * 100 + 0.5);
    if(rate > 999)
	rate = 1000;
    Panel->aq_ratio = rate;
    if(last_rate != rate) {
   	last_rate = Panel->aq_ratio = rate;
	Panel->changed = 1;
    }
}

static void ctl_total_time(int tt)
{
    int32 centisecs = tt/(play_mode->rate/100);

    Panel->total_time = centisecs;
    Panel->total_time_h = centisecs/100/60/60;
    centisecs %= 100*60*60;
    Panel->total_time_m = centisecs/100/60;
    centisecs %= 100*60;
    Panel->total_time_s = centisecs/100;
    centisecs %= 100;
    Panel->total_time_ss = centisecs;
    Panel->changed = 1;
    ctl_current_time(0, 0);
}

static void ctl_program(int ch, int val)
{
    if(ch < 0 || ch >= MAX_W32G_MIDI_CHANNELS)
	return;
    if(!ctl.trace_playing)
	return;
    if(!IS_CURRENT_MOD_FILE)
	val += progbase;

    Panel->channel[ch].program = val;
    Panel->c_flags[ch] |= FLAG_PROG;
    Panel->changed = 1;
}

static void ctl_panning(int ch, int val)
{
    if(ch >= MAX_W32G_MIDI_CHANNELS)
	return;
    if(!ctl.trace_playing)
	return;
    Panel->channel[ch].panning = val;
    Panel->c_flags[ch] |= FLAG_PAN;
    Panel->changed = 1;
}

static void ctl_sustain(int ch, int val)
{
    if(ch >= MAX_W32G_MIDI_CHANNELS)
	return;
    if(!ctl.trace_playing)
	return;
    Panel->channel[ch].sustain = val;
    Panel->c_flags[ch] |= FLAG_SUST;
    Panel->changed = 1;
}

static void ctl_pitch_bend(int ch, int val)
{
    if(ch >= MAX_W32G_MIDI_CHANNELS)
	return;
    if(!ctl.trace_playing)
	return;

    Panel->channel[ch].pitchbend = val;
//    Panel->c_flags[ch] |= FLAG_BENDT;
    Panel->changed = 1;
}

static void ctl_reset(void)
{
    int i;

    if(!ctl.trace_playing)
	return;

    PanelReset();
    CanvasReadPanelInfo(0);
    CanvasUpdate(0);
    CanvasPaint();

    for(i = 0; i < MAX_W32G_MIDI_CHANNELS; i++)
    {
	if(ISDRUMCHANNEL(i))
	    ctl_program(i, channel[i].bank);
	else
	    ctl_program(i, channel[i].program);
	ctl_volume(i, channel[i].volume);
	ctl_expression(i, channel[i].expression);
	ctl_panning(i, channel[i].panning);
	ctl_sustain(i, channel[i].sustain);
	if(channel[i].pitchbend == 0x2000 &&
	   channel[i].modulation_wheel > 0)
	    ctl_pitch_bend(i, -1);
	else
	    ctl_pitch_bend(i, channel[i].pitchbend);
	ctl_channel_note(i, Panel->cnote[i], 0);
    }
    Panel->changed = 1;
}

static void ctl_maxvoices(int v)
{
    Panel->voices = v;
    Panel->changed = 1;
}

static void ctl_event(CtlEvent *e)
{
    switch(e->type)
    {
      case CTLE_NOW_LOADING:
	PanelReset();
	CanvasReset();
	CanvasClear();
	CanvasReadPanelInfo(1);
	CanvasUpdate(1);
	CanvasPaintAll();
	MPanelReset();
	MPanelReadPanelInfo(1);
	MPanelUpdateAll();
	MPanelPaintAll();
	MPanelStartLoad((char *)e->v1);
	break;
      case CTLE_LOADING_DONE:
	break;
      case CTLE_PLAY_START:
	w32g_ctle_play_start((int)e->v1 / play_mode->rate);
	break;
      case CTLE_PLAY_END:
	MainWndScrollbarProgressUpdate(-1);
	break;
      case CTLE_TEMPO:
	break;
      case CTLE_METRONOME:
	break;
      case CTLE_CURRENT_TIME: {
	  int sec;
	  if(midi_trace.flush_flag)
	      return;
	  if(ctl.trace_playing)
	      sec = (int)e->v1;
	  else
	  {
	      sec = current_trace_samples();
	      if(sec < 0)
		  sec = (int)e->v1;
	      else
		  sec = sec / play_mode->rate;
	  }
	  ctl_current_time(sec, (int)e->v2);
	  display_aq_ratio();
	  MainWndScrollbarProgressUpdate(sec);
	  ctl_panel_refresh();
	}
	break;
      case CTLE_NOTE:
	ctl_note((int)e->v1, (int)e->v2, (int)e->v3, (int)e->v4);
	break;
      case CTLE_MASTER_VOLUME:
	ctl_master_volume((int)e->v1);
	break;
      case CTLE_PROGRAM:
	ctl_program((int)e->v1, (int)e->v2);
	break;
      case CTLE_VOLUME:
	ctl_volume((int)e->v1, (int)e->v2);
	break;
      case CTLE_EXPRESSION:
	ctl_expression((int)e->v1, (int)e->v2);
	break;
      case CTLE_PANNING:
	ctl_panning((int)e->v1, (int)e->v2);
	break;
      case CTLE_SUSTAIN:
	ctl_sustain((int)e->v1, (int)e->v2);
	break;
      case CTLE_PITCH_BEND:
	ctl_pitch_bend((int)e->v1, (int)e->v2);
	break;
      case CTLE_MOD_WHEEL:
	ctl_pitch_bend((int)e->v1, e->v2 ? -2 : 0x2000);
	break;
      case CTLE_CHORUS_EFFECT:
	break;
      case CTLE_REVERB_EFFECT:
	break;
      case CTLE_LYRIC:
	default_ctl_lyric((uint16)e->v1);
	break;
      case CTLE_REFRESH:
      	if(CanvasGetMode() == CANVAS_MODE_KEYBOARD)
	{
	    CanvasReadPanelInfo(0);
	    CanvasUpdate(0);
	    CanvasPaint();
      	}

	break;
      case CTLE_RESET:
	ctl_reset();
	break;
      case CTLE_SPEANA:
	break;
      case CTLE_PAUSE:
	if(w32g_play_active)
	{
	    MainWndScrollbarProgressUpdate((int)e->v2);
	    if(!(int)e->v1)
		ctl_reset();
	    ctl_current_time((int)e->v2, 0);
	    ctl_panel_refresh();
	}
	break;
      case CTLE_MAXVOICES:
	ctl_maxvoices((int)e->v1);
	break;
    }
}
