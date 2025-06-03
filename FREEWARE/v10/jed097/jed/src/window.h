/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#ifndef __JED_WINDOW_H_
#define __JED_WINDOW_H_
#include "buffer.h"
typedef struct Window_Type
  {
      int top;                  /* top row of window */
      int rows;                 /* number of rows */
      int width;
      int column;               /* starts at 1 */
      Mark beg;
      struct Window_Type *next;
      Buffer *buffer;
      Mark mark;               /* last cursor pos in window before switch */
      int trashed;              /* true if lines in window was changed */
      int flags;               /* Note that trashed should be a bit here */
  }
Window_Type;

extern Window_Type *JWindow;

extern Window_Type *create_window(int, int, int, int);
extern void touch_screen_for_buffer(Buffer *);
extern void touch_window_hard(Window_Type *, int);


extern int split_window(void);
extern int other_window(void);
extern int one_window(void);
extern int delete_window(void);
extern int enlarge_window(void);
extern void window_buffer(Buffer *);
extern void change_screen_size(int, int);
extern Window_Type *buffer_visible(Buffer *);
extern int Top_Window_Row;
#endif

