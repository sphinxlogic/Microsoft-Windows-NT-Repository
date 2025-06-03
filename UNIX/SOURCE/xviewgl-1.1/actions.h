#include "patchlevel.h"
#include "my_types.h"
#include "my_stdarg.h"
char *read_data();	/* Get next data item */
BOOLEAN cmd_box(int argc, my_va_list ap, char *err_msg); /* Draw a box */
BOOLEAN cmd_break(int argc, my_va_list ap, char *err_msg);	/* Break out of immediately enclosing loop. */
BOOLEAN cmd_call(int argc, my_va_list ap, char *err_msg);	/* Run another text file and return */
BOOLEAN cmd_cfade(int argc, my_va_list ap, char *err_msg); /* Fade in image */
BOOLEAN cmd_cfree(int argc, my_va_list ap, char *err_msg); /* Clear clip reg. */
BOOLEAN cmd_cgetbuf(int argc, my_va_list ap, char *err_msg);	/* Get clip register from screen */
BOOLEAN cmd_chgcolor(int argc, my_va_list ap, char *err_msg); /* Change EGA cmap */
BOOLEAN cmd_circle(int argc, my_va_list ap, char *err_msg); /* Draw an ellipse */
BOOLEAN cmd_clearscr(int argc, my_va_list ap, char *err_msg); /* Clear screen */
BOOLEAN cmd_cload(int argc, my_va_list ap, char *err_msg); /* Load clip reg */
BOOLEAN cmd_closegl(int argc, my_va_list ap, char *err_msg);	/* Close active graphics library */
BOOLEAN cmd_color(int argc, my_va_list ap, char *err_msg); /* Set primary/2ndary colors */
BOOLEAN cmd_cycle(int argc, my_va_list ap, char *err_msg); /* Cycle through ccolors */
BOOLEAN cmd_data(int argc, my_va_list ap, char *err_msg);	/* provide immediate data */
BOOLEAN cmd_databegin(int argc, my_va_list ap, char *err_msg); /* Start reading from labelled data. */
BOOLEAN cmd_dataend(int argc, my_va_list ap, char *err_msg); /* End of labelled data.  Should never be executed. */
BOOLEAN cmd_dataskip(int argc, my_va_list ap, char *err_msg);	/* Skip n elements of data */
BOOLEAN cmd_dfree(int argc, my_va_list ap, char *err_msg);	/* Free difference buffer */
BOOLEAN cmd_dload(int argc, my_va_list ap, char *err_msg);	/* load difference buffer */
BOOLEAN cmd_edge(int argc, my_va_list ap, char *err_msg);	/* Turn on edge display */
BOOLEAN cmd_else(int argc, my_va_list ap, char *err_msg);	/* else (in if-else-endif) */
BOOLEAN cmd_endif(int argc, my_va_list ap, char *err_msg);	/* endif (in if-else-endif) */
BOOLEAN cmd_exec(int argc, my_va_list ap, char *err_msg); /* Execute command file */
BOOLEAN cmd_exit(int argc, my_va_list ap, char *err_msg); /* Finish processing */
BOOLEAN cmd_ffree(int argc, my_va_list ap, char *err_msg); /* Free font space */
BOOLEAN cmd_fgaps(int argc, my_va_list ap, char *err_msg); /* Set font spacing */
BOOLEAN cmd_fload(int argc, my_va_list ap, char *err_msg); /* Load a font */
BOOLEAN cmd_float(int argc, my_va_list ap, char *err_msg); /* Fly preserving bg */
BOOLEAN cmd_fly (int argc, my_va_list ap, char *err_msg); /* Move pic, trash bg */
BOOLEAN cmd_font(int argc, my_va_list ap, char *err_msg);	/* Select active font buffer */
BOOLEAN cmd_fstyle(int argc, my_va_list ap, char *err_msg);	/* Select font style */
BOOLEAN cmd_getcolor(int argc, my_va_list ap, char *err_msg);
BOOLEAN cmd_gosub(int argc, my_va_list ap, char *err_msg);	/* Call subroutine */
BOOLEAN cmd_goto(int argc, my_va_list ap, char *err_msg);	/* Go to label */
BOOLEAN cmd_if(int argc, my_va_list ap, char *err_msg);	/* start of if-else-endif */
BOOLEAN cmd_ifkey(int argc, my_va_list ap, char *err_msg);	/* Go to label if key was pressed */
BOOLEAN cmd_ifmem(int argc, my_va_list ap, char *err_msg);	/* Go to label if memory is available. */
BOOLEAN cmd_ifmouse(int argc, my_va_list ap, char *err_msg);	/* */
BOOLEAN cmd_ifvideo(int argc, my_va_list ap, char *err_msg);	/* Go to label if video mode is available. */
BOOLEAN cmd_int(int argc, my_va_list ap, char *err_msg);	/* IBM PC interrupt */
BOOLEAN cmd_line(int argc, my_va_list ap, char *err_msg); /* Draw line */
BOOLEAN cmd_link(int argc, my_va_list ap, char *err_msg); /* Link to new file */
BOOLEAN cmd_local(int argc, my_va_list ap, char *err_msg);	/* Create local variable */
BOOLEAN cmd_loop(int argc, my_va_list ap, char *err_msg); /* Loop to last mark */
BOOLEAN cmd_mark(int argc, my_va_list ap, char *err_msg); /* Mark loop start */
BOOLEAN cmd_merge(int argc, my_va_list ap, char *err_msg);	/* Merge lines into current source (??) */
BOOLEAN cmd_mode(int argc, my_va_list ap, char *err_msg);	/* Set video modde */
BOOLEAN cmd_mouse(int argc, my_va_list ap, char *err_msg);	/* */
BOOLEAN cmd_move(int argc, my_va_list ap, char *err_msg);	/* Copy portion of screen to another part of screen */
BOOLEAN cmd_noise(int argc, my_va_list ap, char *err_msg);	/* Make a noise */
BOOLEAN cmd_note(int argc, my_va_list ap, char *err_msg);	/* Play a note */
BOOLEAN cmd_offset(int argc, my_va_list ap, char *err_msg);	/* Offset all graphics coordinates by fixed amount */
BOOLEAN cmd_opengl(int argc, my_va_list ap, char *err_msg);	/* Open a graphics library */
BOOLEAN cmd_out(int argc, my_va_list ap, char *err_msg);	/* Send value out IBM PC port */
BOOLEAN cmd_palette(int argc, my_va_list ap, char *err_msg);	/* Select preg palette for current cmap */
BOOLEAN cmd_pan(int argc, my_va_list ap, char *err_msg);	/* EGA panning */
BOOLEAN cmd_pfade(int argc, my_va_list ap, char *err_msg);	/* Fadee preg to screen */
BOOLEAN cmd_pfree(int argc, my_va_list ap, char *err_msg);	/* Free preg */
BOOLEAN cmd_pgetbuf(int argc, my_va_list ap, char *err_msg);	/* Read preg from screen */
BOOLEAN cmd_pload(int argc, my_va_list ap, char *err_msg);	/* Load preg from file */
BOOLEAN cmd_pnewbuf(int argc, my_va_list ap, char *err_msg);	/* Create new preg */
BOOLEAN cmd_point(int argc, my_va_list ap, char *err_msg);	/* Draw point in current draw color */
BOOLEAN cmd_poke(int argc, my_va_list ap, char *err_msg);	/* Poke into PC memory */
BOOLEAN cmd_pokel(int argc, my_va_list ap, char *err_msg);	/* Poke into PC memory */
BOOLEAN cmd_pokew(int argc, my_va_list ap, char *err_msg);	/* Poke into PC memory */
BOOLEAN cmd_position(int argc, my_va_list ap, char *err_msg);	/* Position image in preg */
BOOLEAN cmd_psave(int argc, my_va_list ap, char *err_msg);	/* Save preg to file */
BOOLEAN cmd_psetbuf(int argc, my_va_list ap, char *err_msg);	/* Draw to preg instead of screen */
BOOLEAN cmd_putdff(int argc, my_va_list ap, char *err_msg);	/* Put diff buffer to screen */
BOOLEAN cmd_putup(int argc, my_va_list ap, char *err_msg);	/* Put image on screen */
BOOLEAN cmd_resetgl(int argc, my_va_list ap, char *err_msg);	/* Reset GL library */
BOOLEAN cmd_resetscr(int argc, my_va_list ap, char *err_msg);	/* */
BOOLEAN cmd_return(int argc, my_va_list ap, char *err_msg);	/* Return from subroutine */
BOOLEAN cmd_revpage(int argc, my_va_list ap, char *err_msg);	/* Swap drawing and visible pages */
BOOLEAN cmd_send(int argc, my_va_list ap, char *err_msg);	/* Send string to device */
BOOLEAN cmd_set(int argc, my_va_list ap, char *err_msg);	/* Set option */
BOOLEAN cmd_setcolor(int argc, my_va_list ap, char *err_msg);	/* */
BOOLEAN cmd_setpage(int argc, my_va_list ap, char *err_msg);	/* Set drawing and visible pages */
BOOLEAN cmd_setrgb(int argc, my_va_list ap, char *err_msg);	/* Set r, g, b values associated with a pixel */
BOOLEAN cmd_setupscr(int argc, my_va_list ap, char *err_msg);	/* Set up virtual screen for panning */
BOOLEAN cmd_spread(int argc, my_va_list ap, char *err_msg);	/* Fade from one cmap to another */
BOOLEAN cmd_text(int argc, my_va_list ap, char *err_msg);	/* Draw text on screen */
BOOLEAN cmd_tile(int argc, my_va_list ap, char *err_msg);	/* Tile screen with image */
BOOLEAN cmd_timer(int argc, my_va_list ap, char *err_msg);	/* Initialize timer (??) */
BOOLEAN cmd_tran(int argc, my_va_list ap, char *err_msg);	/* Control transparent mode */
BOOLEAN cmd_video(int argc, my_va_list ap, char *err_msg);	/* Set video mode */
BOOLEAN cmd_waitkey(int argc, my_va_list ap, char *err_msg);	/* Wait for keypress */
BOOLEAN cmd_when(int argc, my_va_list ap, char *err_msg);	/* Perform action on given key press */
BOOLEAN cmd_window(int argc, my_va_list ap, char *err_msg); /* Def clipping window */
