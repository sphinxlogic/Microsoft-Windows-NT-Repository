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

    w32g_utl.c: written by Daisuke Aoki <dai@y7.net>
                           Masanao Izumo <mo@goice.co.jp>
*/


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <stdio.h>
#ifndef NO_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include <ctype.h>

#include "timidity.h"
#include "common.h"
#include "instrum.h"
#include "playmidi.h"
#include "readmidi.h"
#include "output.h"
#include "controls.h"
#include "recache.h"
#ifdef SUPPORT_SOUNDSPEC
#include "soundspec.h"
#endif /* SUPPORT_SOUNDSPEC */
#include "wrd.h"
#include "w32g.h"
#include "w32g_utl.h"
#include <sys/stat.h>
#include "strtab.h"
#include "url.h"

extern int opt_default_mid;
extern int effect_lr_mode;
extern int effect_lr_delay_msec;
extern char def_instr_name[];
extern int opt_control_ratio;
extern char *opt_aq_max_buff;
extern char *opt_aq_fill_buff;
extern int opt_evil_mode;
extern int opt_buffer_fragments;
extern int32 opt_output_rate;
extern int PlayerLanguage;
extern volatile int data_block_bits;
extern volatile int data_block_num;
extern int DocWndIndependent;
extern int SeachDirRecursive;
extern int IniFileAutoSave;
extern int SecondMode;

//*****************************************************************************/
// ini

// INI file
CHAR *INI_INVALID = "INVALID PARAMETER";
CHAR *INI_SEC_PLAYER = "PLAYER";
CHAR *INI_SEC_TIMIDITY = "TIMIDITY";
#define INI_MAXLEN 1024

int
IniGetKeyInt32(char *section, char *key,int32 *n)
{
  CHAR buffer[INI_MAXLEN];
  GetPrivateProfileString
    (section,key,INI_INVALID,buffer,INI_MAXLEN-1,IniFile);
  if(strcasecmp(buffer,INI_INVALID)){
    *n =atol(buffer);
    return 0;
  } else
    return 1;
}

int
IniGetKeyInt32Array(char *section, char *key, int32 *n, int arraysize)
{
  int i;
  int ret = 0;
  CHAR buffer[INI_MAXLEN];
  char keybuffer[INI_MAXLEN];
  for(i=0;i<arraysize;i++){
    sprintf(keybuffer,"%s%d",key,i);
    GetPrivateProfileString
      (section,keybuffer,INI_INVALID,buffer,INI_MAXLEN-1,IniFile);
    if(strcasecmp(buffer,INI_INVALID))
      *n =atol(buffer);
    else
      ret++;
  }
  return ret;
}

int
IniGetKeyInt(char *section, char *key, int *n)
{
  return IniGetKeyInt32(section, key, (int32 *)n);
}

int
IniGetKeyFloat(char *section, char *key, FLOAT_T *n)
{
    CHAR buffer[INI_MAXLEN];
    GetPrivateProfileString(section, key, INI_INVALID, buffer,
			    INI_MAXLEN-1, IniFile);
    if(strcasecmp(buffer, INI_INVALID))
    {
	*n = (FLOAT_T)atof(buffer);
	return 0;
    }
    else
	return 1;
}

int
IniGetKeyChar(char *section, char *key, char *c)
{
  char buffer[64];
  if(IniGetKeyStringN(section,key,buffer,60))
    return 1;
  else {
    *c = buffer[0];
    return 0;
  }
}

int
IniGetKeyIntArray(char *section, char *key, int *n, int arraysize)
{
  return IniGetKeyInt32Array(section,key,(int32 *)n, arraysize);
}

int
IniGetKeyString(char *section, char *key,char *str)
{
  CHAR buffer[INI_MAXLEN];
  GetPrivateProfileString
    (section,key,INI_INVALID,buffer,INI_MAXLEN-1,IniFile);
  if(strcasecmp(buffer,INI_INVALID)){
    strcpy(str,buffer);
    return 0;
  } else
    return 1;
}

int
IniGetKeyStringN(char *section, char *key,char *str, int size)
{
  CHAR buffer[INI_MAXLEN];
  GetPrivateProfileString
    (section,key,INI_INVALID,buffer,INI_MAXLEN-1,IniFile);
  if(strcasecmp(buffer,INI_INVALID)){
    strncpy(str,buffer,size);
    return 0;
  }	else
    return 1;
}

int
IniPutKeyInt32(char *section, char *key,int32 *n)
{
  CHAR buffer[INI_MAXLEN];
  sprintf(buffer,"%ld",*n);
  WritePrivateProfileString
    (section,key,buffer,IniFile);
  return 0;
}

int
IniPutKeyInt32Array(char *section, char *key, int32 *n, int arraysize)
{
  int i;
  CHAR buffer[INI_MAXLEN];
  CHAR keybuffer[INI_MAXLEN];
  for(i=0;i<arraysize;i++){
    sprintf(buffer,"%ld",n[i]);
    sprintf(keybuffer,"%s%d",key,i);
    WritePrivateProfileString(section,keybuffer,buffer,IniFile);
  }
  return 0;
}

int
IniPutKeyInt(char *section, char *key, int *n)
{
  return IniPutKeyInt32(section, key, (int32 *)n);
}

int
IniPutKeyChar(char *section, char *key, char *c)
{
  char buffer[64];
  sprintf(buffer,"%c",*c);
  return IniPutKeyStringN(section,key,buffer,60);
}

int
IniPutKeyIntArray(char *section, char *key, int *n, int arraysize)
{
  return IniPutKeyInt32Array(section,key,(int32 *)n, arraysize);
}

int
IniPutKeyString(char *section, char *key, char *str)
{
  WritePrivateProfileString(section,key,str,IniFile);
  return 0;
}

int
IniPutKeyStringN(char *section, char *key, char *str, int size)
{
  WritePrivateProfileString(section,key,str,IniFile);
  return 0;
}

int
IniPutKeyFloat(char *section, char *key,FLOAT_T n)
{
    CHAR buffer[INI_MAXLEN];
    sprintf(buffer,"%f", (double)n);
    WritePrivateProfileString(section, key, buffer, IniFile);
    return 0;
}

// LoadIniFile() , SaveIniFile()
// ***************************************************************************
// Setting

#define SetFlag(flag) (!!(flag))

static long SetValue(int32 value, int32 min, int32 max)
{
  int32 v = value;
  if(v < min) v = min;
  else if( v > max) v = max;
  return v;
}

void
ApplySettingPlayer(SETTING_PLAYER *sp)
{
  InitMinimizeFlag = SetFlag(sp->InitMinimizeFlag);
  DebugWndStartFlag = SetFlag(sp->DebugWndStartFlag);
  ConsoleWndStartFlag = SetFlag(sp->ConsoleWndStartFlag);
  ListWndStartFlag = SetFlag(sp->ListWndStartFlag);
  TracerWndStartFlag = SetFlag(sp->TracerWndStartFlag);
  DocWndStartFlag = SetFlag(sp->DocWndStartFlag);
  WrdWndStartFlag = SetFlag(sp->WrdWndStartFlag);
  DebugWndFlag = SetFlag(sp->DebugWndFlag);
  ConsoleWndFlag = SetFlag(sp->ConsoleWndFlag);
  ListWndFlag = SetFlag(sp->ListWndFlag);
  TracerWndFlag = SetFlag(sp->TracerWndFlag);
  DocWndFlag = SetFlag(sp->DocWndFlag);
  WrdWndFlag = SetFlag(sp->WrdWndFlag);
  SoundSpecWndFlag = SetFlag(sp->SoundSpecWndFlag);
  SubWindowMax = SetValue(sp->SubWindowMax,1,10);
  strncpy(ConfigFile,sp->ConfigFile,MAXPATH + 31);
  ConfigFile[MAXPATH + 31] = '\0';
  strncpy(PlaylistFile,sp->PlaylistFile,MAXPATH + 31);
  PlaylistFile[MAXPATH + 31] = '\0';
  strncpy(PlaylistHistoryFile,sp->PlaylistHistoryFile,MAXPATH + 31);
  PlaylistHistoryFile[MAXPATH + 31] = '\0';
  strncpy(MidiFileOpenDir,sp->MidiFileOpenDir,MAXPATH + 31);
  MidiFileOpenDir[MAXPATH + 31] = '\0';
  strncpy(ConfigFileOpenDir,sp->ConfigFileOpenDir,MAXPATH + 31);
  ConfigFileOpenDir[MAXPATH + 31] = '\0';
  strncpy(PlaylistFileOpenDir,sp->PlaylistFileOpenDir,MAXPATH + 31);
  PlaylistFileOpenDir[MAXPATH + 31] = '\0';
  PlayerThreadPriority = sp->PlayerThreadPriority;
  GUIThreadPriority = sp->GUIThreadPriority;
  TraceGraphicFlag = SetFlag(sp->TraceGraphicFlag);
  // fonts ...
  SystemFontSize = sp->SystemFontSize;
  PlayerFontSize = sp->PlayerFontSize;
  WrdFontSize = sp->WrdFontSize;
  DocFontSize = sp->DocFontSize;
  ListFontSize = sp->ListFontSize;
  TracerFontSize = sp->TracerFontSize;
  strncpy(SystemFont,sp->SystemFont,255);
  SystemFont[255] = '\0';
  strncpy(PlayerFont,sp->PlayerFont,255);
  PlayerFont[255] = '\0';
  strncpy(WrdFont,sp->WrdFont,255);
  WrdFont[255] = '\0';
  strncpy(DocFont,sp->DocFont,255);
  DocFont[255] = '\0';
  strncpy(ListFont,sp->ListFont,255);
  ListFont[255] = '\0';
  strncpy(TracerFont,sp->TracerFont,255);
  TracerFont[255] = '\0';
  // Apply font functions ...

  PlayerLanguage = sp->PlayerLanguage;
  DocWndIndependent = sp->DocWndIndependent; 
  SeachDirRecursive = sp->SeachDirRecursive;
  IniFileAutoSave = sp->IniFileAutoSave;
  SecondMode = sp->SecondMode;
}

void
SaveSettingPlayer(SETTING_PLAYER *sp)
{
  sp->InitMinimizeFlag = SetFlag(InitMinimizeFlag);
  sp->DebugWndStartFlag = SetFlag(DebugWndStartFlag);
  sp->ConsoleWndStartFlag = SetFlag(ConsoleWndStartFlag);
  sp->ListWndStartFlag = SetFlag(ListWndStartFlag);
  sp->TracerWndStartFlag = SetFlag(TracerWndStartFlag);
  sp->DocWndStartFlag = SetFlag(DocWndStartFlag);
  sp->WrdWndStartFlag = SetFlag(WrdWndStartFlag);
  sp->DebugWndFlag = SetFlag(DebugWndFlag);
  sp->ConsoleWndFlag = SetFlag(ConsoleWndFlag);
  sp->ListWndFlag = SetFlag(ListWndFlag);
  sp->TracerWndFlag = SetFlag(TracerWndFlag);
  sp->DocWndFlag = SetFlag(DocWndFlag);
  sp->WrdWndFlag = SetFlag(WrdWndFlag);
  sp->SoundSpecWndFlag = SetFlag(SoundSpecWndFlag);
  sp->SubWindowMax = SetValue(SubWindowMax,1,10);
  strncpy(sp->ConfigFile,ConfigFile,MAXPATH + 31);
  (sp->ConfigFile)[MAXPATH + 31] = '\0';
  strncpy(sp->PlaylistFile,PlaylistFile,MAXPATH + 31);
  (sp->PlaylistFile)[MAXPATH + 31] = '\0';
  strncpy(sp->PlaylistHistoryFile,PlaylistHistoryFile,MAXPATH + 31);
  (sp->PlaylistHistoryFile)[MAXPATH + 31] = '\0';
  strncpy(sp->MidiFileOpenDir,MidiFileOpenDir,MAXPATH + 31);
  (sp->MidiFileOpenDir)[MAXPATH + 31] = '\0';
  strncpy(sp->ConfigFileOpenDir,ConfigFileOpenDir,MAXPATH + 31);
  (sp->ConfigFileOpenDir)[MAXPATH + 31] = '\0';
  strncpy(sp->PlaylistFileOpenDir,PlaylistFileOpenDir,MAXPATH + 31);
  (sp->PlaylistFileOpenDir)[MAXPATH + 31] = '\0';
  sp->PlayerThreadPriority = PlayerThreadPriority;
  sp->GUIThreadPriority = GUIThreadPriority;
  sp->WrdGraphicFlag = SetFlag(WrdGraphicFlag);
  sp->TraceGraphicFlag = SetFlag(TraceGraphicFlag);
  // fonts ...
  sp->SystemFontSize = SystemFontSize;
  sp->PlayerFontSize = PlayerFontSize;
  sp->WrdFontSize = WrdFontSize;
  sp->DocFontSize = DocFontSize;
  sp->ListFontSize = ListFontSize;
  sp->TracerFontSize = TracerFontSize;
  strncpy(sp->SystemFont,SystemFont,255);
  sp->SystemFont[255] = '\0';
  strncpy(sp->PlayerFont,PlayerFont,255);
  sp->PlayerFont[255] = '\0';
  strncpy(sp->WrdFont,WrdFont,255);
  sp->WrdFont[255] = '\0';
  strncpy(sp->DocFont,DocFont,255);
  DocFont[255] = '\0';
  strncpy(sp->ListFont,ListFont,255);
  sp->ListFont[255] = '\0';
  strncpy(sp->TracerFont,TracerFont,255);
  sp->TracerFont[255] = '\0';
  sp->PlayerLanguage = PlayerLanguage;
  sp->DocWndIndependent = DocWndIndependent; 
  sp->SeachDirRecursive = SeachDirRecursive;
  sp->IniFileAutoSave = IniFileAutoSave;
  sp->SecondMode = SecondMode;
}

extern int set_play_mode(char *cp);
extern int set_tim_opt(int c, char *optarg);
extern int set_ctl(char *cp);
extern int set_wrd(char *w);

#if defined(__W32__) && defined(SMFCONV)
extern int opt_rcpcv_dll;
#endif /* SMFCONV */
extern char *wrdt_open_opts;

static int is_device_output_ID(int id)
{
    return id == 'd' || id == 'n' || id == 'e';
}

void
ApplySettingTiMidity(SETTING_TIMIDITY *st)
{
    int i;
    char buffer[INI_MAXLEN];

    /* Player must be stopped.
     * DANGER to apply settings while playing.
     */
    amplification = SetValue(st->amplification, 0, MAX_AMPLIFICATION);
    antialiasing_allowed = SetFlag(st->antialiasing_allowed);
    if(st->buffer_fragments == -1)
	opt_buffer_fragments = -1;
    else
	opt_buffer_fragments = SetValue(st->buffer_fragments, 3, 1000);
    default_drumchannels = st->default_drumchannels;
    default_drumchannel_mask = st->default_drumchannel_mask;
    opt_modulation_wheel = SetFlag(st->opt_modulation_wheel);
    opt_portamento = SetFlag(st->opt_portamento);
    opt_nrpn_vibrato = SetFlag(st->opt_nrpn_vibrato);
    opt_channel_pressure = SetFlag(st->opt_channel_pressure);
    opt_trace_text_meta_event = SetFlag(st->opt_trace_text_meta_event);
    opt_overlap_voice_allow = SetFlag(st->opt_overlap_voice_allow);
    opt_default_mid = st->opt_default_mid;
    default_tonebank = st->default_tonebank;
    special_tonebank = st->special_tonebank;
    effect_lr_mode = st->effect_lr_mode;
    effect_lr_delay_msec = st->effect_lr_delay_msec;
    opt_reverb_control = st->opt_reverb_control;
    opt_chorus_control = st->opt_chorus_control;
    noise_sharp_type = st->noise_sharp_type;
    opt_evil_mode = st->opt_evil_mode;
    adjust_panning_immediately = SetFlag(st->adjust_panning_immediately);
    fast_decay = SetFlag(st->fast_decay);
#ifdef SUPPORT_SOUNDSPEC
    view_soundspec_flag = SetFlag(st->view_soundspec_flag);
    spectrigram_update_sec = st->spectrigram_update_sec;
#endif
    for(i = 0; i < MAX_CHANNELS; i++)
	default_program[i] = st->default_program[i];
    set_ctl(st->opt_ctl);
    opt_realtime_playing = SetFlag(st->opt_realtime_playing);
    reduce_voice_threshold = st->reduce_voice_threshold;
    set_play_mode(st->opt_playmode);
    strncpy(OutputName,st->OutputName,MAXPATH);
    if(OutputName[0] && !is_device_output_ID(play_mode->id_character))
	play_mode->name = OutputName;
    opt_output_rate = st->output_rate;
    if(st->output_rate)
	play_mode->rate = SetValue(st->output_rate,
				   MIN_OUTPUT_RATE, MAX_OUTPUT_RATE);
    else if(play_mode->rate == 0)
	play_mode->rate = DEFAULT_RATE;
    voices = SetValue(st->voices, 1, MAX_VOICES);
	auto_reduce_polyphony = st->auto_reduce_polyphony;
    quietchannels = st->quietchannels;
    if(opt_aq_max_buff)
	free(opt_aq_max_buff);
    if(opt_aq_fill_buff)
	free(opt_aq_fill_buff);
    strcpy(buffer, st->opt_qsize);
    opt_aq_max_buff = buffer;
    opt_aq_fill_buff = strchr(opt_aq_max_buff, '/');
    *opt_aq_fill_buff++ = '\0';
    opt_aq_max_buff = safe_strdup(opt_aq_max_buff);
    opt_aq_fill_buff = safe_strdup(opt_aq_fill_buff);
    modify_release = SetValue(st->modify_release, 0, MAX_MREL);
    allocate_cache_size = st->allocate_cache_size;
    if(output_text_code)
	free(output_text_code);
    output_text_code = safe_strdup(st->output_text_code);
    free_instruments_afterwards = st->free_instruments_afterwards;
    set_wrd(st->opt_wrd);
#if defined(__W32__) && defined(SMFCONV)
    opt_rcpcv_dll = st->opt_rcpcv_dll;
#endif /* SMFCONV */

    opt_control_ratio = st->control_ratio;
    if(opt_control_ratio)
	control_ratio = SetValue(opt_control_ratio, 1, MAX_CONTROL_RATIO);
    else
    {
	control_ratio = play_mode->rate / CONTROLS_PER_SECOND;
	control_ratio = SetValue(control_ratio, 1, MAX_CONTROL_RATIO);
    }
    data_block_bits = st->data_block_bits;
    data_block_num = st->data_block_num;
}

void
SaveSettingTiMidity(SETTING_TIMIDITY *st)
{
    int i, j;

    st->amplification = SetValue(amplification, 0, MAX_AMPLIFICATION);
    st->antialiasing_allowed = SetFlag(antialiasing_allowed);
    st->buffer_fragments = opt_buffer_fragments;
    st->control_ratio = SetValue(opt_control_ratio, 0, MAX_CONTROL_RATIO);
    st->default_drumchannels = default_drumchannels;
    st->default_drumchannel_mask = default_drumchannel_mask;
    st->opt_modulation_wheel = SetFlag(opt_modulation_wheel);
    st->opt_portamento = SetFlag(opt_portamento);
    st->opt_nrpn_vibrato = SetFlag(opt_nrpn_vibrato);
    st->opt_channel_pressure = SetFlag(opt_channel_pressure);
    st->opt_trace_text_meta_event = SetFlag(opt_trace_text_meta_event);
    st->opt_overlap_voice_allow = SetFlag(opt_overlap_voice_allow);
    st->opt_default_mid = opt_default_mid;
    st->default_tonebank = default_tonebank;
    st->special_tonebank = special_tonebank;
    st->effect_lr_mode = effect_lr_mode;
    st->effect_lr_delay_msec = effect_lr_delay_msec;
    st->opt_reverb_control = opt_reverb_control;
    st->opt_chorus_control = opt_chorus_control;
    st->noise_sharp_type = noise_sharp_type;
    st->opt_evil_mode = SetFlag(opt_evil_mode);
    st->adjust_panning_immediately = SetFlag(adjust_panning_immediately);
    st->fast_decay = SetFlag(fast_decay);
#ifdef SUPPORT_SOUNDSPEC
    st->view_soundspec_flag = SetFlag(view_soundspec_flag);
    st->spectrigram_update_sec = spectrigram_update_sec;
#endif
    for(i = 0; i < MAX_CHANNELS; i++)
    {
	if(def_instr_name[0])
	    st->default_program[i] = SPECIAL_PROGRAM;
	else
	    st->default_program[i] = default_program[i];
    }
    j = 0;
    st->opt_ctl[j++] = ctl->id_character;
    for(i = 1; i < ctl->verbosity; i++)
	st->opt_ctl[j++] = 'v';
    for(i = 1; i > ctl->verbosity; i--)
	st->opt_ctl[j++] = 'q';
    if(ctl->trace_playing)
	st->opt_ctl[j++] = 't';
    if(ctl->flags & CTLF_LIST_LOOP)
	st->opt_ctl[j++] = 'l';
    if(ctl->flags & CTLF_LIST_RANDOM)
	st->opt_ctl[j++] = 'r';
    if(ctl->flags & CTLF_LIST_SORT)
	st->opt_ctl[j++] = 's';
    if(ctl->flags & CTLF_AUTOSTART)
	st->opt_ctl[j++] = 'a';
    if(ctl->flags & CTLF_AUTOEXIT)
	st->opt_ctl[j++] = 'x';
    if(ctl->flags & CTLF_DRAG_START)
	st->opt_ctl[j++] = 'd';
    if(ctl->flags & CTLF_AUTOUNIQ)
	st->opt_ctl[j++] = 'u';
    if(ctl->flags & CTLF_AUTOREFINE)
	st->opt_ctl[j++] = 'R';
    if(ctl->flags & CTLF_NOT_CONTINUE)
	st->opt_ctl[j++] = 'C';
    st->opt_ctl[j] = '\0';
    st->opt_realtime_playing = SetFlag(opt_realtime_playing);
    st->reduce_voice_threshold = reduce_voice_threshold;
    j = 0;
    st->opt_playmode[j++] = play_mode->id_character;
    st->opt_playmode[j++] = ((play_mode->encoding & PE_MONO) ? 'M' : 'S');
    st->opt_playmode[j++] = ((play_mode->encoding & PE_SIGNED) ? 's' : 'u');
    st->opt_playmode[j++] = ((play_mode->encoding & PE_16BIT) ? '1' : '8');
    if(play_mode->encoding & PE_ULAW)
	st->opt_playmode[j++] = 'U';
    else if(play_mode->encoding & PE_ALAW)
	st->opt_playmode[j++] = 'A';
    else
	st->opt_playmode[j++] = 'l';
    if(play_mode->encoding & PE_BYTESWAP)
	st->opt_playmode[j++] = 'x';
    st->opt_playmode[j] = '\0';
    strncpy(st->OutputName,OutputName,sizeof(st->OutputName)-1);
    st->voices = SetValue(voices, 1, MAX_VOICES);
	st->auto_reduce_polyphony = auto_reduce_polyphony;
    st->quietchannels = quietchannels;
    snprintf(st->opt_qsize,sizeof(st->opt_qsize),"%s/%s",
	     opt_aq_max_buff,opt_aq_fill_buff);
    st->modify_release = SetValue(modify_release, 0, MAX_MREL);
    st->allocate_cache_size = allocate_cache_size;
    st->output_rate = opt_output_rate;
    if(st->output_rate == 0)
    {
	st->output_rate = play_mode->rate;
	if(st->output_rate == 0)
	    st->output_rate = DEFAULT_RATE;
    }
    st->output_rate = SetValue(st->output_rate,MIN_OUTPUT_RATE,MAX_OUTPUT_RATE);
    strncpy(st->output_text_code,output_text_code,sizeof(st->output_text_code)-1);
    st->free_instruments_afterwards = free_instruments_afterwards;
    st->opt_wrd[0] = wrdt->id;
    if(wrdt_open_opts)
	strncpy(st->opt_wrd + 1, wrdt_open_opts, sizeof(st->opt_wrd) - 2);
    else
	st->opt_wrd[1] = '\0';
#if defined(__W32__) && defined(SMFCONV)
    st->opt_rcpcv_dll = opt_rcpcv_dll;
#endif /* SMFCONV */
  st->data_block_bits = data_block_bits;
  st->data_block_num = data_block_num;
}






//****************************************************************************/
// ini & config

static char S_IniFile[MAXPATH + 32];
static char S_timidity_window_inifile[MAXPATH + 32];
static char S_ConfigFile[MAXPATH + 32];
static char S_PlaylistFile[MAXPATH + 32];
static char S_PlaylistHistoryFile[MAXPATH + 32];
static char S_MidiFileOpenDir[MAXPATH + 32];
static char S_ConfigFileOpenDir[MAXPATH + 32];
static char S_PlaylistFileOpenDir[MAXPATH + 32];
static char S_DocFileExt[256];
static char S_OutputName[MAXPATH + 32];
static char *OutputName;
static char S_SystemFont[256];
static char S_PlayerFont[256];
static char S_WrdFont[256];
static char S_DocFont[256];
static char S_ListFont[256];
static char S_TracerFont[256];
static char *SystemFont = S_SystemFont;
char *PlayerFont = S_PlayerFont;
static char *WrdFont = S_WrdFont;
static char *DocFont = S_DocFont;
static char *ListFont = S_ListFont;
static char *TracerFont = S_TracerFont;

//static HFONT hSystemFont = 0;
//static HFONT hPlayerFont = 0;
//static HFONT hWrdFont = 0;
//static HFONT hDocFont = 0;
//static HFONT hListFont = 0;
//static HFONT hTracerFont = 0;
static int SystemFontSize = 9;
static int PlayerFontSize = 16;
static int WrdFontSize = 16;
static int DocFontSize = 16;
static int ListFontSize = 16;
static int TracerFontSize = 16;

#define DEFAULT_DOCFILEEXT "doc;txt;hed"

SETTING_PLAYER *sp_default, *sp_current, *sp_temp;
SETTING_TIMIDITY *st_default, *st_current, *st_temp;
extern char *timidity_window_inifile;

void w32g_initialize(void)
{
    char buffer[MAXPATH + 1024];
    char *p;

    hInst = GetModuleHandle(0);
  
    IniFile = S_IniFile;
    ConfigFile = S_ConfigFile;
    PlaylistFile = S_PlaylistFile;
    PlaylistHistoryFile = S_PlaylistHistoryFile;
    MidiFileOpenDir = S_MidiFileOpenDir;
    ConfigFileOpenDir = S_ConfigFileOpenDir;
    PlaylistFileOpenDir = S_PlaylistFileOpenDir;
    DocFileExt = S_DocFileExt;
    OutputName = S_OutputName;
  
    IniFile[0] = '\0';
    ConfigFile[0] = '\0';
    PlaylistFile[0] = '\0';
    PlaylistHistoryFile[0] = '\0';
    MidiFileOpenDir[0] = '\0';
    ConfigFileOpenDir[0] = '\0';
    PlaylistFileOpenDir[0] = '\0';
    OutputName[0] = '\0';
    strcpy(DocFileExt,DEFAULT_DOCFILEEXT);
    strcpy(SystemFont,"�l�r ����");
    strcpy(PlayerFont,"�l�r ����");
    strcpy(WrdFont,"�l�r ����");
    strcpy(DocFont,"�l�r ����");
    strcpy(ListFont,"�l�r ����");
    strcpy(TracerFont,"�l�r ����");

    if(GetModuleFileName(hInst, buffer, MAXPATH))
    {
	if((p = pathsep_strrchr(buffer)) != NULL)
	{
	    p++;
	    *p = '\0';
	}
	else
	{
	    buffer[0] = '.';
	    buffer[1] = PATH_SEP;
	    buffer[2] = '\0';
	}
    }
    else
    {
	buffer[0] = '.';
	buffer[1] = PATH_SEP;
	buffer[2] = '\0';
    }
    strncpy(IniFile, buffer, MAXPATH);
    IniFile[MAXPATH] = '\0';
    strcat(IniFile,"timpp32g.ini");
	timidity_window_inifile = S_timidity_window_inifile;
	strncpy(timidity_window_inifile, buffer, 200);
    timidity_window_inifile[200] = '\0';
    strcat(timidity_window_inifile,"timidity_window.ini");
    st_default = (SETTING_TIMIDITY *)safe_malloc(sizeof(SETTING_TIMIDITY));
    sp_default = (SETTING_PLAYER *)safe_malloc(sizeof(SETTING_PLAYER));
    st_current = (SETTING_TIMIDITY *)safe_malloc(sizeof(SETTING_TIMIDITY));
    sp_current = (SETTING_PLAYER *)safe_malloc(sizeof(SETTING_PLAYER));
    st_temp = (SETTING_TIMIDITY *)safe_malloc(sizeof(SETTING_TIMIDITY));
    sp_temp = (SETTING_PLAYER *)safe_malloc(sizeof(SETTING_PLAYER));

    SaveSettingPlayer(sp_current);
    SaveSettingTiMidity(st_current);
    if(IniVersionCheck())
    {
	LoadIniFile(sp_current, st_current);
	ApplySettingPlayer(sp_current);
	ApplySettingTiMidity(st_current);
	w32g_has_ini_file = 1;
    }
    else
    {
	sprintf(buffer,
"Ini file is not found, or old format is found.\n"
"Do you want to initialize the ini file?\n\n"
"Ini file path: %s",
		IniFile);

	if(MessageBox(0, buffer, "TiMidity Notice", MB_YESNO) == IDYES)
	{
	    SaveIniFile(sp_current, st_current);
	    w32g_has_ini_file = 1;
	}
	else
	{
	    w32g_has_ini_file = 0;
	}
    }

    memcpy(sp_default, sp_current, sizeof(SETTING_PLAYER));
    memcpy(st_default, st_current, sizeof(SETTING_TIMIDITY));

    memcpy(sp_temp, sp_current, sizeof(SETTING_PLAYER));
    memcpy(st_temp, st_current, sizeof(SETTING_TIMIDITY));

    w32g_i_init();
}

int IniVersionCheck(void)
{
    char version[INI_MAXLEN];
    if(IniGetKeyStringN(INI_SEC_PLAYER,"IniVersion",version,sizeof(version)-1) == 0 &&
       strcmp(version, IniVersion) == 0)
	return 1; // UnChanged
    return 0;
}

void BitBltRect(HDC dst, HDC src, RECT *rc)
{
    BitBlt(dst, rc->left, rc->top,
	   rc->right - rc->left, rc->bottom - rc->top,
	   src, rc->left, rc->top, SRCCOPY);
}

#if 0
/*
 * TmColor
 */
TmColors tm_colors[TMCC_SIZE];

static COLORREF WeakHalfColor(COLORREF fc, COLORREF bc)
{
    return fc*1/3 + bc*2/3;
}

static COLORREF HalfColor(COLORREF fc, COLORREF bc)
{
    return fc*1/6 + bc*5/6;
}

void TmInitColor(void)
{
    int i;

    tm_colors[TMCC_BLACK].color	= TMCCC_BLACK;
    tm_colors[TMCC_WHITE].color	= TMCCC_WHITE;
    tm_colors[TMCC_RED].color	= TMCCC_RED;
    tm_colors[TMCC_BACK].color	= TMCCC_BACK;
    tm_colors[TMCC_LOW].color	= TMCCC_LOW;
    tm_colors[TMCC_MIDDLE].color= TMCCC_MIDDLE;
    tm_colors[TMCC_HIGH].color	= TMCCC_HIGH;

    tm_colors[TMCC_FORE_HALF].color = HalfColor(TMCCC_FORE,TMCCC_BACK);
    tm_colors[TMCC_FORE_WEAKHALF].color = WeakHalfColor(TMCCC_FORE,TMCCC_BACK);
    tm_colors[TMCC_LOW_HALF].color = HalfColor(TMCCC_LOW,TMCCC_BACK);
    tm_colors[TMCC_MIDDLE_HALF].color = HalfColor(TMCCC_MIDDLE,TMCCC_BACK);
    tm_colors[TMCC_HIGH_HALF].color = HalfColor(TMCCC_HIGH,TMCCC_BACK);

    for(i = 0; i < TMCC_SIZE; i++)
    {
	tm_colors[i].pen = CreatePen(PS_SOLID, 1, tm_colors[i].color);
	tm_colors[i].brush = CreateSolidBrush(tm_colors[i].color);
    }
}

void TmFreeColor(void)
{
    int i;
    for(i = 0; i < TMCC_SIZE; i++)
    {
	if(tm_colors[i].pen != NULL)
	{
	    DeleteObject(tm_colors[i].pen);
	    DeleteObject(tm_colors[i].brush);
	    tm_colors[i].pen = NULL;
	}
    }
}

void TmFillRect(HDC hdc, RECT *rc, int color)
{
    HPEN hPen = tm_colors[color].pen;
    HBRUSH hBrush = tm_colors[color].brush;
    HGDIOBJ hgdiobj_hpen, hgdiobj_hbrush;

    hgdiobj_hpen = SelectObject(hdc, hPen);
    hgdiobj_hbrush = SelectObject(hdc, hBrush);
    Rectangle(hdc, rc->left, rc->top, rc->right, rc->bottom);
    SelectObject(hdc, hgdiobj_hpen);
    SelectObject(hdc, hgdiobj_hbrush);
}
#endif

#ifndef S_ISDIR
#define S_ISDIR(mode)   (((mode)&0xF000) == 0x4000)
#endif /* S_ISDIR */
int is_directory(char *path)
{
	struct stat st;
	if(*path == '@') /* special identifire for playlist file */
		return 0;
	if(stat(path, &st) != -1)
		return S_ISDIR(st.st_mode);
	return GetFileAttributes(path) == FILE_ATTRIBUTE_DIRECTORY;
}

/* Return: 0: - not modified
 *         1: - modified
 */
int directory_form(char *buffer)
{
	int len;

	len = strlen(buffer);
	if(len == 0 || buffer[len - 1] == PATH_SEP)
		return 0;
	if(IS_PATH_SEP(buffer[len - 1]))
		len--;
	buffer[len++] = PATH_SEP;
	buffer[len] = '\0';
	return 1;
}

/* Return: 0: - not modified
 *         1: - modified
 */
int nodirectory_form(char *buffer)
{
	char *lastp = buffer + strlen(buffer);
	char *p = lastp;

	while(p > buffer && IS_PATH_SEP(*(p - 1)))
		p--;
	if(p == lastp)
		return 0;
	*p = '\0';
	return 1;
}

void SettingCtlFlag(SETTING_TIMIDITY *st, int c, int onoff)
{
    int n;
    char *opt;
    
    opt = st->opt_ctl + 1;
    n = strlen(opt);
    if(onoff)
    {
	if(strchr(opt, c) != NULL)
	    return; /* Already set */
	opt[n++] = c;
	opt[n] = '\0';
    }
    else
    {
	char *p;
	if((p = strchr(opt, c)) == NULL)
	    return; /* Already removed */
	while(*(p + 1))
	{
	    *p = *(p + 1);
	    p++;
	}
	*p = '\0';
    }
}




int IsAvailableFilename(char *filename)
{
	char *p = strrchr(filename,'.');
	if(p == NULL)
		return 0;
	if(	strcasecmp(p,".lzh")==0 ||
		strcasecmp(p,".zip")==0 ||
		strcasecmp(p,".gz")==0	||
		strcasecmp(p,".mid")==0 ||
		strcasecmp(p,".rcp")==0 ||
		strcasecmp(p,".r36")==0 ||
		strcasecmp(p,".g18")==0 ||
		strcasecmp(p,".g36")==0 ||
		strcasecmp(p,".mod")==0 ||
//		strcasecmp(p,".hqx")==0 ||
		strcasecmp(p,".tar")==0 ||
		strcasecmp(p,".tgz")==0 ||
		strcasecmp(p,".lha")==0 ||
		strcasecmp(p,".mime")==0 ||
		strcasecmp(p,".smf")==0)
		return 1;
//	if(url_check_type(filename)!=-1)
//		return 1;
	return 0;
}

/* ScanDirectoryFiles() works like UNIX find. */
#define SCANDIR_MAX_DEPTH 32
void ScanDirectoryFiles(char *basedir,
						int (* file_proc)(char *pathname, /* (const) */
										  void *user_val),
						void *user_val)
{
	char baselen;
	URL dir;

	static int depth = 0;
    static int stop_flag;	/* Stop scanning if true */
    static int error_disp;	/* Whether error is displayed or not */
	static char pathbuf[MAXPATH]; /* pathname buffer */

	if(depth == 0) /* Initialize variables at first recursive */
	{
		stop_flag = 0;
		error_disp = 0;
		strcpy(pathbuf, basedir);
	}
	else if(depth > SCANDIR_MAX_DEPTH) /* Avoid infinite recursive */
	{
		if(!error_disp)
		{
			/* Display this message at once */
			ctl->cmsg(CMSG_WARNING, VERB_NORMAL,
					  "%s: Directory is too deep",
					  basedir);
			error_disp = 1;
		}
		return; /* Skip scanning this directory */
	}

	directory_form(pathbuf);
	baselen = strlen(pathbuf);
	if(baselen > sizeof(pathbuf) - 16)
	{
		/* Ignore too long file name */
		return;
	}

	if((dir = url_dir_open(pathbuf)) == NULL)
	{
	    ctl->cmsg(CMSG_ERROR, VERB_NORMAL, "%s: Can't open directory", pathbuf);
		return;
	}

	if(file_proc(pathbuf, user_val))
	{
	    stop_flag = 1; /* Terminate */
		return;
	}

	while(!stop_flag &&
		  url_gets(dir, pathbuf + baselen, sizeof(pathbuf) - baselen - 1))
	{
		if(strcmp(pathbuf + baselen, ".") == 0 ||
		   strcmp(pathbuf + baselen, "..") == 0)
			continue;
		if(file_proc(pathbuf, user_val))
		{
			stop_flag = 1; /* Terminate */
			break;
		}
		if(is_directory(pathbuf))
		{
			/* into subdirectory */
			depth++;
			ScanDirectoryFiles(pathbuf, file_proc, user_val);
			depth--;
		}
	}
	url_close(dir);
}

#define EXPANDDIR_MAX_SIZE  100000	/* Limit of total bytes of the file names */
static int expand_dir_proc(char *filename, void *v)
{
	void **user_val = (void **)v;
	StringTable *st = (StringTable *)user_val[0];
	int *total_size = (int *)user_val[1];
	char *startdir  = (char *)user_val[2];

	if(IsAvailableFilename(filename))
	{
		if(*total_size > EXPANDDIR_MAX_SIZE)
		{
			ctl->cmsg(CMSG_ERROR, VERB_NORMAL, "%s: There are too many files.",
					  startdir);
			return 1; /* Terminate */
		}
		put_string_table(st, filename, strlen(filename));
		*total_size += strlen(filename);
	}
	return 0;
}

char **FilesExpandDir(int *p_nfiles, char **files)
{
	StringTable st;
	int i;

	init_string_table(&st);
	for(i = 0; i < *p_nfiles; i++)
	{
		void *user_val[3];
		int total_size;

		total_size = 0;
		user_val[0] = &st;
		user_val[1] = &total_size;
		user_val[2] = files[i];

		if(is_directory(files[i]))
			ScanDirectoryFiles(files[i], expand_dir_proc, user_val);
		else
		{
			int len = strlen(files[i]);
			put_string_table(&st, files[i], len);
		}
	}
	*p_nfiles = st.nstring;
    return make_string_array(&st);

}
