/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#ifndef __JED_BUFFER_H_  
#define  __JED_BUFFER_H_

#ifndef sequent
#include <stdlib.h>
#endif

#include "slang.h"
#include "keymap.h"
#include "undo.h"

/* 
#define sprintf simple_sprintf
extern char *simple_sprintf(char *, char *, ...);
*/

typedef struct Line
  {
      struct Line *next;               /* pointer to next line */
      struct Line *prev;               /* pointer to prev line */
      unsigned char *data;             /* data for the line */
      int len;                         /* actual length of line */
      int space;                       /* space allocated for line */
  } Line;

/* This is the price we pay for a linked list approach.  With straight
   buffer gap, this would be an integer.  Sigh. */
typedef struct Mark
  {
     Line *line;                      /* line that marker points at */
     int point;                       /* offset from beginning */
     unsigned int n;		       /* line number in buffer */
     struct Mark *next;
     unsigned int flags;	       /* visible mark if non-zero */
#define MARK_INVALID 0x1
#define VISIBLE_MARK 0x2
  }
Mark;

extern unsigned int LineNum;	       /* current line number */
extern unsigned int Max_LineNum;       /* max line number */

typedef struct Narrow_Type
{
   struct Narrow_Type *next;
   unsigned int nup, ndown;	       /* (relative) lines above this narrow */
   Line *beg, *end;		       /* pointers to lines to linkup with */
   Line *beg1, *end1;		       /* beg and end before narrow */
} Narrow_Type;


#ifdef HAS_ABBREVS
#include "abbrev.h"
#endif

typedef struct Buffer
  {
     Line *beg;			       /* Top line of buffer */
     Line *end;			       /* Bottom line */
     Line *line;		       /* current line */
     int point;			       /* current offset */
     unsigned int linenum;	       /* current line number */
     unsigned int max_linenum;	       /* lines in buffer */

     char file[256];		       /* filename sans dir */
     char dir[256];		       /* directory of file */
     char name[50];		       /* name of this buffer */
     unsigned int flags;	       /* flags  (autosave, etc...) */
     Narrow_Type *narrow;	       /* info for use by widen */
     unsigned int nup;		       /* lines above narrow (absolute) */
     unsigned int ndown;	       /* lines below narrow */
     Mark *marks;
     Mark *spots;
     Mark *user_marks;
     unsigned int modes;	       /* c-mode, wrap, etc... */
     SLKeyMap_List_Type *keymap;       /* keymap attached to this buffer */
     struct Buffer *next;	       /*  */
     struct Buffer *prev;
     char mode_str[13];
     int hits;			       /* number of hits on buffer since 
                                           last autosave.  A hit is the number
					   of times the buffer was hit on at top level  */
     unsigned long m_time;	       /* time when buffer first modified */
     unsigned long c_time;	       /* time when buffer first created or  */
				       /* when file visited */
     Undo_Type *undo;		       /* pointer to undo ring */
     /* local variables */
     int tab;			       /* tab setting for buffer */
     int sd;			       /* selective display */
     
#define SPOT_ARRAY_SIZE 4
     
     Mark spot_array[SPOT_ARRAY_SIZE];
     Mark mark_array[SPOT_ARRAY_SIZE];
     int mark_ptr;
     int spot_ptr;
     int vis_marks;		       /* number of visible marks */
     char status_line[80];
     SLang_Name_Type *par_sep;		       /* paragraph sep function */
     SLang_Name_Type *indent_hook;
     SLang_Name_Type *wrap_hook;
     
#ifdef HAS_ABBREVS
     Abbrev_Table_Type *abbrev_table;
#endif
  } Buffer;

extern char Default_Status_Line[80];

/* flags */
#define BUFFER_TRASHED 0x01

/* This flag cannot be used with the AUTO_SAVE_JUST_SAVE flag */
#define AUTO_SAVE_BUFFER 0x02
/* these two flags are to tell user that the buffer and the file on disk
   have been modified--- see update_marks and main editor loop */
#define FILE_MODIFIED 0x04
#define READ_ONLY 0x08
#define OVERWRITE_MODE 0x10
#define UNDO_ENABLED 0x20

/* skip this buffer if looking for a pop up one. */
#define BURIED_BUFFER 0x40

/* Instead of autosaving saving the buffer, just save it.  This flag
 * is only used when SIGHUP or something like that hits.  It is also
 * used when exiting the editor.  It will cause the buffer to be silently 
 * saved.  It is possible that I need another flag for this.
 */
#define AUTO_SAVE_JUST_SAVE 0x80
#define NO_BACKUP_FLAG  0x100
#define BINARY_FILE  0x200
#define ADD_CR_ON_WRITE_FLAG 0x400

#define ABBREV_MODE 0x800

extern char *Read_Only_Error;
#define CHECK_READ_ONLY\
    if (CBuf->flags & READ_ONLY) { msg_error(Read_Only_Error); return(1);}
    
#define CHECK_READ_ONLY_VOID\
    if (CBuf->flags & READ_ONLY) { msg_error(Read_Only_Error); return;}

				 

#define NO_MODE 0x00
#define WRAP_MODE 0x01
#define C_MODE 0x02
#define LANG_MODE 0x04		       /* to be a replacement for C_MODE */
#define SL_MODE 0x08		       /* S-Lang mode (ored with C_MODE) */
#define F_MODE 0x10		       /* Fortran mode */
#define TEX_MODE 0x20		       /* ored with TEXT_MODE */

typedef struct User_Variable_Type
  {
      int wrap_column;
      int tab;				/* tab size, if 0 display as ^I */
      int c_indent;			/* indent level past brace */
      int c_brace;			/* indent of brace */
      int max_hits;			/* autosave interval */
  }
User_Variable_Type;

/* These are buffer local variables that slang can access */
typedef struct 
{
   int tab;			       /* tab width */
   int sd;			       /* selective display */
} Buffer_Local_Type;

extern Buffer_Local_Type Buffer_Local;


extern User_Variable_Type User_Vars;

extern Buffer *CBuf;
extern Line *CLine;


extern int bob(void);
extern int eob(void);                  /* point to end of buffer */
extern int bol(void);
extern int eol(void);

extern int bobp(void);
extern int eobp(void);
extern int eolp(void);
extern int bolp(void);

extern int prevline(int *);
extern int nextline(int *);

extern int forwchars(int *);
extern int backwchars(int *);
extern void goto_line(int *);

extern Line *make_line1(int);
extern unsigned char *make_line(int);
extern unsigned char *remake_line(int);

extern Buffer *make_buffer(void);
extern void uniquely_name_buffer(char *);
extern void buffer_filename(char *, char *);
extern Buffer *find_file_buffer(char *);
extern Buffer *find_buffer(char *);
extern int delete_line(void);
extern void delete_buffer(Buffer *);
extern int switch_to_buffer(Buffer *);
extern int get_percent(void);
extern int what_line(void);
extern int erase_buffer(void);
extern void mark_buffer_modified(int *);
extern Line *dup_line(Line *);
extern void free_line(Line *);
extern void check_buffers(void);
extern int buffer_exists(Buffer *);
extern int Point;
extern int Number_Zero;
extern int Number_One;
extern int Number_Two;
extern int Number_Ten;
/*
extern unsigned char Chg_UCase_Lut[256];
extern unsigned char Chg_LCase_Lut[256];
#define UPPER_CASE(x) (Chg_UCase_Lut[(unsigned char) (x)])
#define LOWER_CASE(x) (Chg_LCase_Lut[(unsigned char) (x)])
#define CHANGE_CASE(x) (((x) == Chg_LCase_Lut[(unsigned char) (x)]) ?\
			Chg_UCase_Lut[(unsigned char) (x)] : Chg_LCase_Lut[(unsigned char) (x)])
*/
extern void mark_undo_boundary(Buffer *);
extern void delete_undo_ring(Buffer *);

extern int Batch;		       /* JED used in batch mode. */
extern void touch_screen(void);
extern void check_line(void);
extern Buffer *MiniBuffer;
#endif
