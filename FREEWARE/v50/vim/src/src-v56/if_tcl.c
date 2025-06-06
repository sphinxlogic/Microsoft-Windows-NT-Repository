/* vi:set ts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Tcl extensions by Ingo Wilken <Ingo.Wilken@informatik.uni-oldenburg.de>
 * Last modification: Fri Mar 27 22:41:25 CET 1998
 * Requires Tcl 8.0 or higher.
 *
 *	Variables:
 *	::vim::current(buffer)	# Name of buffer command for current buffer.
 *	::vim::current(window)	# Name of window command for current window.
 *	::vim::range(start)		# Start of current range (line number).
 *	::vim::range(end)		# End of current range (line number).
 *	::vim::lbase			# Start of line/column numbers (1 or 0).
 *
 *	Commands:
 *  ::vim::command {cmd}		# Execute ex command {cmd}.
 *  ::vim::option {opt} [val]	# Get/Set option {opt}.
 *  ::vim::expr {expr}			# Evaluate {expr} using vim's evaluator.
 *  ::vim::beep					# Guess.
 *
 *  set buf [::vim::buffer {n}]	# Create Tcl command for buffer N.
 *  set bl [::vim::buffer list] # Get list of Tcl commands of all buffers.
 *  ::vim::buffer exists {n}	# True if buffer {n} exists.
 *
 *  set wl [::vim::window list] # Get list of Tcl commands of all windows.
 *
 *  set n [$win height]			# Report window height.
 *  $win height {n}				# Set window height to {n}.
 *  array set pos [$win cursor] # Get cursor position.
 *	$win cursor {row} {col}		# Set cursor position.
 *  $win cursor pos				# Set cursor position from array var "pos"
 *	$win delcmd {cmd}			# Register callback command for closed window.
 *  $win option {opt} [val]		# Get/Set vim option in context of $win.
 *  $win command {cmd}			# Execute ex command in context of $win.
 *  $win expr {expr}			# Evaluate vim expression in context of $win.
 *  set buf [$win buffer]		# Create Tcl command for window's buffer.
 *
 *	$buf name					# Reports file name in buffer.
 *	$buf number					# Reports buffer number.
 *	set l [$buf get {n}]		# Get buffer line {n} as a string.
 *	set L [$buf get {n} {m}]	# Get lines {n} through {m} as a list.
 *	$buf count					# Reports number of lines in buffer.
 *	$buf last					# Reports number of last line in buffer.
 *	$buf delete {n}				# Delete line {n}.
 *	$buf delete {n} {m}			# Delete lines {n} through {m}.
 *	$buf set {n} {l}			# Set line {n} to string {l}.
 *	$buf set {n} {m} {L}		# Set lines {n} through {m} from list {L}.
 *								# Delete/inserts lines as appropriate.
 *	$buf option {opt} [val]		# Get/Set vim option in context of $buf.
 *	$buf command {cmd}			# Execute ex command in context of $buf
 *	$buf expr {cmd}				# Evaluate vim expression in context of $buf.
 *	array set pos [$buf mark {m}]	# Get position of mark.
 *	$buf append {n} {str}		# Append string {str} to buffer,after line {n}.
 *	$buf insert {n} {str}		# Insert string {str} in buffer as line {n}.
 *	$buf delcmd {cmd}			# Register callback command for deleted buffer.
 *	set wl [$buf windows]		# Get list of Tcl commands for all windows of
 *								# this buffer.
TODO:
 *  ::vim::buffer new			#	create new buffer + Tcl command
 */

#include "vim.h"
#include <tcl.h>
#include <errno.h>
#include <string.h>

static struct
{
	Tcl_Interp *interp;
	unsigned int refcount;
	unsigned do_exit:1;
	unsigned valid:1;
	int range_start, range_end, lbase;
	char *curbuf, *curwin;
} tcl = {(Tcl_Interp *)NULL, 0, 0, 0};

#define VAR_RANGE1		"::vim::range(start)"
#define VAR_RANGE2		"::vim::range(begin)"
#define VAR_RANGE3		"::vim::range(end)"
#define VAR_CURBUF		"::vim::current(buffer)"
#define VAR_CURWIN		"::vim::current(window)"
#define VAR_LBASE		"::vim::lbase"
#define VAR_CURLINE		"line"
#define VAR_CURLNUM		"lnum"
#define VARNAME_SIZE	64

#define row2tcl(x)	((x) - (tcl.lbase==0))
#define row2vim(x)	((x) + (tcl.lbase==0))
#define col2tcl(x)	((x) + (tcl.lbase!=0))
#define col2vim(x)	((x) - (tcl.lbase!=0))


#define VIMOUT	((ClientData)1)
#define VIMERR	((ClientData)2)

/*
 *	List of Tcl interpreters who reference a vim window or buffer.
 *	Each buffer and window has it's own list in the tcl_ref struct member.
 *  We need this because Tcl can create sub-interpreters with the "interp"
 *  command, and each interpreter can reference all windows and buffers.
 */
struct ref
{
	struct ref *next;

	Tcl_Interp *interp;
	Tcl_Command cmd;	/* Tcl command that represents this object */
	Tcl_Obj *delcmd;	/* Tcl command to call when object is being deleted */
	void *vimobj;		/* Vim window or buffer (WIN* or BUF*) */
};
static char * tclgetbuffer _ANSI_ARGS_((Tcl_Interp *interp, BUF *buf));
static char * tclgetwindow _ANSI_ARGS_((Tcl_Interp *interp, WIN *win));
static int tclsetdelcmd _ANSI_ARGS_((Tcl_Interp *interp, struct ref *reflist, void *vimobj, Tcl_Obj *delcmd));
static int tclgetlinenum _ANSI_ARGS_ ((Tcl_Interp *interp, Tcl_Obj *obj, int *valueP, BUF *buf));
static WIN *tclfindwin _ANSI_ARGS_ ((BUF *buf));
static int tcldoexcommand _ANSI_ARGS_ ((Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[], int objn));
static int tclsetoption _ANSI_ARGS_ ((Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[], int objn));
static int tclvimexpr _ANSI_ARGS_ ((Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[], int objn));

static int vimerror _ANSI_ARGS_((Tcl_Interp *interp));
static void tclmsg _ANSI_ARGS_((char *text));
static void tclerrmsg _ANSI_ARGS_((char *text));
static void tclupdatevars _ANSI_ARGS_((void));

static struct ref refsdeleted;	/* dummy object for deleted ref list */

/****************************************************************************
  Tcl commands
 ****************************************************************************/

/*
 *	"exit" - replaces Tcl's standard "exit" command.
 *	Unfortunately, this is one of Tcl's design flaws - "exit" should have
 *	been implemented as returning TCL_EXIT to the application.
 */
/* ARGSUSED */
	static int
exitcmd(dummy, interp, objc, objv)
	ClientData dummy;
	Tcl_Interp *interp;
	int objc;
	Tcl_Obj *CONST objv[];
{
	int value = 0;
	char buf[32];

	switch (objc)
	{
		case 2:
			if (Tcl_GetIntFromObj(interp, objv[1], &value) != TCL_OK)
				break;
		case 1:
			sprintf(buf, "exit code %d", value);
			Tcl_SetResult(interp, buf, TCL_VOLATILE);
			tcl.do_exit = 1;
			break;
		default:
			Tcl_WrongNumArgs(interp, 1, objv, "?returnCode?");
	}
	return TCL_ERROR;
}

/*
 *	"::vim::beep" - what Vi[m] does best :-)
 */
/* ARGSUSED */
	static int
beepcmd(dummy, interp, objc, objv)
	ClientData dummy;
	Tcl_Interp *interp;
	int objc;
	Tcl_Obj *CONST objv[];
{
	if (objc != 1)
	{
		Tcl_WrongNumArgs(interp, 1, objv, NULL);
		return TCL_ERROR;
	}
	vim_beep();
	return TCL_OK;
}

/*
 *	"::vim::buffer list" - create a list of buffer commands.
 *	"::vim::bufffer {N}" - create buffer command for buffer N.
 *	"::vim::buffer new" - create a new buffer (not implemented)
 */
/* ARGSUSED */
	static int
buffercmd(dummy, interp, objc, objv)
	ClientData dummy;
	Tcl_Interp *interp;
	int objc;
	Tcl_Obj *CONST objv[];
{
	char *name;
	BUF *buf;
	Tcl_Obj *resobj;
	int err, n, idx;

	enum { BCMD_EXISTS, BCMD_LIST };
	static char *bcmdoptions[] =
	{
		"exists", "list", (char *)0
	};

	err = Tcl_GetIntFromObj(interp, objv[1], &n);
	if (err == TCL_OK)
	{
		if (objc != 2)
		{
			Tcl_WrongNumArgs(interp, 1, objv, "bufNumber");
			return TCL_ERROR;
		}
		for (buf = firstbuf; buf != NULL; buf = buf->b_next)
		{
			if (buf->b_fnum == n)
			{
				name = tclgetbuffer(interp, buf);
				if (name == NULL)
					return TCL_ERROR;
				Tcl_SetResult(interp, name, TCL_VOLATILE);
				return TCL_OK;
			}
		}
		Tcl_SetResult(interp, "invalid buffer number", TCL_STATIC);
		return TCL_ERROR;
	}
	Tcl_ResetResult(interp); /* clear error from Tcl_GetIntFromObj */

	err = Tcl_GetIndexFromObj(interp, objv[1], bcmdoptions, "option", 0, &idx);
	if (err != TCL_OK)
		return err;
	switch (idx)
	{
		case BCMD_LIST:
			if (objc != 2)
			{
				Tcl_WrongNumArgs(interp, 2, objv, "");
				err = TCL_ERROR;
				break;
			}
			for (buf = firstbuf; buf != NULL; buf = buf->b_next)
			{
				name = tclgetbuffer(interp, buf);
				if (name == NULL)
				{
					err = TCL_ERROR;
					break;
				}
				Tcl_AppendElement(interp, name);
			}
			break;

		case BCMD_EXISTS:
			if (objc != 3)
			{
				Tcl_WrongNumArgs(interp, 2, objv, "bufNumber");
				err = TCL_ERROR;
				break;
			}
			err = Tcl_GetIntFromObj(interp, objv[2], &n);
			if (err == TCL_OK)
			{
				buf = buflist_findnr(n);
				resobj = Tcl_NewIntObj(buf != NULL);
				Tcl_SetObjResult(interp, resobj);
			}
			break;

		default:
			Tcl_SetResult(interp, "not implemented yet", TCL_STATIC);
			err = TCL_ERROR;
	}
	return err;
}

/*
 * "::vim::window list" - create list of window commands.
 */
/* ARGSUSED */
	static int
windowcmd(dummy, interp, objc, objv)
	ClientData dummy;
	Tcl_Interp *interp;
	int objc;
	Tcl_Obj *CONST objv[];
{
	char *what, *string;
	WIN *win;

	if (objc != 2)
	{
		Tcl_WrongNumArgs(interp, 1, objv, "option");
		return TCL_ERROR;
	}
	what = Tcl_GetStringFromObj(objv[1], NULL);
	if (strcmp(what, "list") == 0)
	{
		for (win = firstwin; win != NULL; win = win->w_next)
		{
			string = tclgetwindow(interp, win);
			if (string == NULL)
				return TCL_ERROR;
			Tcl_AppendElement(interp, string);
		}
		return TCL_OK;
	}
	Tcl_SetResult(interp, "unknown option", TCL_STATIC);
	return TCL_ERROR;
}

/*
 * flags for bufselfcmd and winselfcmd to indicate outstanding actions.
 */
#define FL_UPDATE_SCREEN	(1<<0)
#define FL_UPDATE_CURBUF	(1<<1)
#define FL_ADJUST_CURSOR	(1<<2)

/*
 * This function implements the buffer commands.
 */
	static int
bufselfcmd(ref, interp, objc, objv)
	ClientData	ref;
	Tcl_Interp	*interp;
	int			objc;
	Tcl_Obj		*CONST objv[];
{
	int			opt, err, idx, flags;
	int			val1, val2, n, i;
	BUF			*buf, *savebuf;
	WIN			*win, *savewin;
	Tcl_Obj		*resobj;
	FPOS		*pos;
	char		*line;

	enum
	{
		BUF_APPEND, BUF_COMMAND, BUF_COUNT, BUF_DELCMD, BUF_DELETE, BUF_EXPR,
		BUF_GET, BUF_INSERT, BUF_LAST, BUF_MARK, BUF_NAME, BUF_NUMBER,
		BUF_OPTION, BUF_SET, BUF_WINDOWS
	};
	static char *bufoptions[] =
	{
		"append", "command", "count", "delcmd", "delete", "expr",
		"get", "insert", "last", "mark", "name", "number",
		"option", "set", "windows", (char *)0
	};

	if (objc < 2)
	{
		Tcl_WrongNumArgs(interp, 1, objv, "option ?arg ...?");
		return TCL_ERROR;
	}

	err = Tcl_GetIndexFromObj(interp, objv[1], bufoptions, "option", 0, &idx);
	if (err != TCL_OK)
		return err;

	buf = (BUF *)((struct ref *)ref)->vimobj;
	savebuf = curbuf;  curbuf = buf;
	savewin = curwin;  curwin = tclfindwin(buf);
	flags = 0;
	opt = 0;

	switch (idx)
	{
		case BUF_COMMAND:
			err = tcldoexcommand(interp, objc, objv, 2);
			flags |= FL_UPDATE_SCREEN;
			break;

		case BUF_OPTION:
			err = tclsetoption(interp, objc, objv, 2);
			flags |= FL_UPDATE_SCREEN;
			break;

		case BUF_EXPR:
			err = tclvimexpr(interp, objc, objv, 2);
			break;

		case BUF_NAME:
			/*
			 *	Get filename of buffer.
			 */
			if (objc != 2)
			{
				Tcl_WrongNumArgs(interp, 2, objv, NULL);
				err = TCL_ERROR;
				break;
			}
			if (buf->b_ffname)
				Tcl_SetResult(interp, (char *)buf->b_ffname, TCL_VOLATILE);
			else
				Tcl_SetResult(interp, "", TCL_STATIC);
			break;

		case BUF_LAST:
			/*
			 * Get line number of last line.
			 */
			opt = 1;
			/* fallthrough */
		case BUF_COUNT:
			/*
			 * Get number of lines in buffer.
			 */
			if (objc != 2)
			{
				Tcl_WrongNumArgs(interp, 2, objv, NULL);
				err = TCL_ERROR;
				break;
			}
			val1 = (int)buf->b_ml.ml_line_count;
			if (opt)
				val1 = row2tcl(val1);

			resobj = Tcl_NewIntObj(val1);
			Tcl_SetObjResult(interp, resobj);
			break;

		case BUF_NUMBER:
			/*
			 * Get buffer's number.
			 */
			if (objc != 2)
			{
				Tcl_WrongNumArgs(interp, 2, objv, NULL);
				err = TCL_ERROR;
				break;
			}
			resobj = Tcl_NewIntObj((int)buf->b_fnum);
			Tcl_SetObjResult(interp, resobj);
			break;

		case BUF_GET:
			if (objc != 3 && objc != 4)
			{
				Tcl_WrongNumArgs(interp, 2, objv, "lineNumber ?lineNumber?");
				err = TCL_ERROR;
				break;
			}
			err = tclgetlinenum(interp, objv[2], &val1, buf);
			if (err != TCL_OK)
				break;
			if (objc == 4)
			{
				err = tclgetlinenum(interp, objv[3], &val2, buf);
				if (err != TCL_OK)
					break;
				if (val1 > val2)
				{
					n = val1; val1 = val2; val2 = n;
				}
				Tcl_ResetResult(interp);

				for (n = val1; n <= val2 && err == TCL_OK; n++)
				{
					line = (char *)ml_get_buf(buf, (linenr_t)n, FALSE);
					if (line)
						Tcl_AppendElement(interp, line);
					else
						err = TCL_ERROR;
				}
			}
			else {	/* objc == 3 */
				line = (char *)ml_get_buf(buf, (linenr_t)val1, FALSE);
				Tcl_SetResult(interp, line, TCL_VOLATILE);
			}
			break;

		case BUF_SET:
			if (objc != 4 && objc != 5)
			{
				Tcl_WrongNumArgs(interp, 3, objv, "lineNumber ?lineNumber? stringOrList");
				err = TCL_ERROR;
				break;
			}
			err = tclgetlinenum(interp, objv[2], &val1, buf);
			if (err != TCL_OK)
				return TCL_ERROR;
			if (objc == 4)
			{
				/*
				 *	Replace one line with a string.
				 *		$buf set {n} {string}
				 */
				line = Tcl_GetStringFromObj(objv[3], NULL);
				if (u_savesub((linenr_t)val1) != OK)
				{
					Tcl_SetResult(interp, "cannot save undo information", TCL_STATIC);
					err = TCL_ERROR;
				}
				else
				if (ml_replace((linenr_t)val1, (char_u *)line, TRUE) != OK)
				{
					Tcl_SetResult(interp, "cannot replace line", TCL_STATIC);
					err = TCL_ERROR;
				}
				else
				{
					changed();
#ifdef SYNTAX_HL
					/* recompute syntax hl. for this line */
					syn_changed((linenr_t)val1);
#endif
					flags |= FL_UPDATE_CURBUF;
				}
				break;
			}
			else
			{
				/*
				 * Replace several lines with the elements of a Tcl list.
				 *		$buf set {n} {m} {list}
				 * If the list contains more than {m}-{n}+1 elements, they
				 * are * inserted after line {m}.  If the list contains fewer
				 * elements, * the lines from {n}+length({list}) through {m}
				 * are deleted.
				 */
				int lc;
				Tcl_Obj **lv;

				err = tclgetlinenum(interp, objv[3], &val2, buf);
				if (err != TCL_OK)
					break;
				err = Tcl_ListObjGetElements(interp, objv[4], &lc, &lv);
				if (err != TCL_OK)
					break;
				if (val1 > val2)
				{
					n = val1; val1 = val2; val2 = n;
				}

				n = val2-val1+1;	/* number of lines to be replaced */
				if (u_save((linenr_t)(val1-1), (linenr_t)(val2+1)) != OK)
				{
					Tcl_SetResult(interp, "cannot save undo information", TCL_STATIC);
					err = TCL_ERROR;
					break;
				}
				flags |= FL_UPDATE_CURBUF;

				for (i = 0; i < lc && val1 <= val2; i++)
				{
					line = Tcl_GetStringFromObj(lv[i], NULL);
					if (ml_replace((linenr_t)val1, (char_u *)line, TRUE) != OK)
						goto setListError;
					changed();
#ifdef SYNTAX_HL
					/* recompute syntax hl. for this line */
					syn_changed((linenr_t)val1);
#endif
					++val1;
				}
				if (i < lc)
				{
					/* append lines */
					do
					{
						line = Tcl_GetStringFromObj(lv[i], NULL);
						if (ml_append((linenr_t)(val1-1), (char_u *)line, 0, FALSE) != OK)
							goto setListError;
						changed();
#ifdef SYNTAX_HL
						/* recompute syntax hl. for this line */
						syn_changed((linenr_t)(val1-1));
#endif
						++val1;
						++i;
					} while (i < lc);
				}
				else if (val1 <= val2)
				{
					/* did not replace all lines, delete */
					i = val1;
					do
					{
						if (ml_delete((linenr_t)i, FALSE) != OK)
							goto setListError;
						changed();
						++val1;
					} while (val1 <= val2);
				}
				mark_adjust((linenr_t)val1, (linenr_t)val2, (long)MAXLNUM,
																(long)(lc-n));
				break;
	setListError:
				u_undo(1);	/* ??? */
				Tcl_SetResult(interp, "cannot set line(s)", TCL_STATIC);
				err = TCL_ERROR;
			}
			break;

		case BUF_DELETE:
			if (objc != 3  &&  objc != 4)
			{
				Tcl_WrongNumArgs(interp, 3, objv, "lineNumber ?lineNumber?");
				err = TCL_ERROR;
				break;
			}
			err = tclgetlinenum(interp, objv[2], &val1, buf);
			if (err != TCL_OK)
				break;
			val2 = val1;
			if (objc == 4)
			{
				err = tclgetlinenum(interp, objv[3], &val2, buf);
				if (err != TCL_OK)
					return err;
				if (val1 > val2)
				{
					i = val1; val1 = val2; val2 = i;
				}
			}
			n = val2-val1+1;
			if (u_savedel((linenr_t)val1, (long)n) != OK)
			{
				Tcl_SetResult(interp, "cannot save undo information", TCL_STATIC);
				err = TCL_ERROR;
				break;
			}
			for (i = 0; i < n; i++)
			{
				ml_delete((linenr_t)val1, FALSE);
				changed();
				err = vimerror(interp);
				if (err != TCL_OK)
					break;
			}
			if (err == TCL_OK)
				mark_adjust((linenr_t)val1, (linenr_t)val2, (long)MAXLNUM,
																	(long)-n);
			flags |= FL_ADJUST_CURSOR|FL_UPDATE_SCREEN;
			break;

		case BUF_MARK:
			if (objc != 3)
			{
				Tcl_WrongNumArgs(interp, 2, objv, "markName");
				err = TCL_ERROR;
				break;
			}
			line = Tcl_GetStringFromObj(objv[2], NULL);

			pos = NULL;
			if (line[0] != '\0'  &&  line[1] == '\0')
			{
				pos = getmark(line[0], FALSE);
			}
			if (pos == NULL)
			{
				Tcl_SetResult(interp, "invalid mark name", TCL_STATIC);
				err = TCL_ERROR;
				break;
			}
			err = vimerror(interp);
			if (err != TCL_OK)
				break;
			if (pos->lnum == 0)
			{
				Tcl_SetResult(interp, "mark not set", TCL_STATIC);
				err = TCL_ERROR;
			}
			else
			{
				char rbuf[64];
				sprintf(rbuf, "row %d column %d", (int)row2tcl(pos->lnum), (int)col2tcl(pos->col));
				Tcl_SetResult(interp, rbuf, TCL_VOLATILE);
			}
			break;

		case BUF_INSERT:
			opt = 1;
			/* fallthrough */
		case BUF_APPEND:
			if (objc != 4)
			{
				Tcl_WrongNumArgs(interp, 2, objv, "lineNum text");
				err = TCL_ERROR;
				break;
			}
			err = tclgetlinenum(interp, objv[2], &val1, buf);
			if (err != TCL_OK)
				break;
			if (opt)
				--val1;
			if (u_save((linenr_t)val1, (linenr_t)(val1+1)) != OK)
			{
				Tcl_SetResult(interp, "cannot save undo information", TCL_STATIC);
				err = TCL_ERROR;
				break;
			}

			line = Tcl_GetStringFromObj(objv[3], NULL);
			if (ml_append((linenr_t)val1, (char_u *)line, 0, FALSE) != OK)
			{
				Tcl_SetResult(interp, "cannot insert/append line", TCL_STATIC);
				err = TCL_ERROR;
				break;
			}
			changed();
			mark_adjust((linenr_t)(val1+1), (linenr_t)MAXLNUM, 1L, 0L);
			flags |= FL_UPDATE_SCREEN;
			break;

		case BUF_WINDOWS:
			/*
			 * Return list of window commands.
			 */
			if (objc != 2)
			{
				Tcl_WrongNumArgs(interp, 2, objv, NULL);
				err = TCL_ERROR;
				break;
			}
			Tcl_ResetResult(interp);
			for (win = firstwin; win != NULL; win = win->w_next)
			{
				if (win->w_buffer == buf)
				{
					line = tclgetwindow(interp, win);
					if (line != NULL)
						Tcl_AppendElement(interp, line);
					else
					{
						err = TCL_ERROR;
						break;
					}
				}
			}
			break;

		case BUF_DELCMD:
			/*
			 * Register deletion callback.
			 * TODO: Should be able to register multiple callbacks
			 */
			if (objc != 3)
			{
				Tcl_WrongNumArgs(interp, 2, objv, "command");
				err = TCL_ERROR;
				break;
			}
			err = tclsetdelcmd(interp, buf->tcl_ref, (void *)buf, objv[2]);
			break;

		default:
			Tcl_SetResult(interp, "not implemented yet", TCL_STATIC);
			err = TCL_ERROR;
	}

	if (flags & FL_UPDATE_CURBUF)
		update_curbuf(NOT_VALID);
	curbuf = savebuf;
	curwin = savewin;
	if (flags & FL_ADJUST_CURSOR)
		adjust_cursor();
	if (flags & FL_UPDATE_SCREEN)
		update_screen(NOT_VALID);

	return err;
}

/*
 * This function implements the window commands.
 */
static int
winselfcmd(ref, interp, objc, objv)
	ClientData ref;
	Tcl_Interp *interp;
	int objc;
	Tcl_Obj *CONST objv[];
{
	int err, idx, flags;
	int val1, val2;
	Tcl_Obj *resobj;
	WIN *savewin, *win;
	BUF *savebuf;
	char *str;

	enum
	{
		WIN_BUFFER, WIN_COMMAND, WIN_CURSOR, WIN_DELCMD, WIN_EXPR,
		WIN_HEIGHT, WIN_OPTION
	};
	static char *winoptions[] =
	{
		"buffer", "command", "cursor", "delcmd", "expr",
		"height", "option", (char *)0
	};

	if (objc < 2)
	{
		Tcl_WrongNumArgs(interp, 1, objv, "option ?arg ...?");
		return TCL_ERROR;
	}

	err = Tcl_GetIndexFromObj(interp, objv[1], winoptions, "option", 0,  &idx);
	if (err != TCL_OK)
		return TCL_ERROR;

	win = (WIN *)((struct ref *)ref)->vimobj;
	savewin = curwin;  curwin = win;
	savebuf = curbuf;  curbuf = win->w_buffer;
	flags = 0;

	switch (idx)
	{
		case WIN_OPTION:
			err = tclsetoption(interp, objc, objv, 2);
			flags |= FL_UPDATE_SCREEN;
			break;

		case WIN_COMMAND:
			err = tcldoexcommand(interp, objc, objv, 2);
			flags |= FL_UPDATE_SCREEN;
			break;

		case WIN_EXPR:
			err = tclvimexpr(interp, objc, objv, 2);
			break;

		case WIN_HEIGHT:
			if (objc == 3)
			{
				err = Tcl_GetIntFromObj(interp, objv[2], &val1);
				if (err != TCL_OK)
					break;
#ifdef USE_GUI
				need_mouse_correct = TRUE;
#endif
				win_setheight(val1);
				err = vimerror(interp);
				if (err != TCL_OK)
					break;
			}
			else
			if (objc != 2)
			{
				Tcl_WrongNumArgs(interp, 2, objv, "?value?");
				err = TCL_ERROR;
				break;
			}

			resobj = Tcl_NewIntObj((int)(win->w_height));
			Tcl_SetObjResult(interp, resobj);
			break;

		case WIN_BUFFER:
			if (objc != 2)
			{
				Tcl_WrongNumArgs(interp, 2, objv, NULL);
				err = TCL_ERROR;
				break;
			}
			str = tclgetbuffer(interp, win->w_buffer);
			if (str)
				Tcl_SetResult(interp, str, TCL_VOLATILE);
			else
				err = TCL_ERROR;
			break;

		case WIN_DELCMD:
			if (objc != 3)
			{
				Tcl_WrongNumArgs(interp, 2, objv, "command");
				err = TCL_ERROR;
				break;
			}
			err = tclsetdelcmd(interp, win->tcl_ref, (void *)win, objv[2]);
			break;

		case WIN_CURSOR:
			if (objc > 4)
			{
				Tcl_WrongNumArgs(interp, 2, objv, "?arg1 ?arg2??");
				err = TCL_ERROR;
				break;
			}
			if (objc == 2)
			{
				char buf[64];
				sprintf(buf, "row %d column %d", (int)row2tcl(win->w_cursor.lnum), (int)col2tcl(win->w_cursor.col));
				Tcl_SetResult(interp, buf, TCL_VOLATILE);
				break;
			}
			else if (objc == 3)
			{
				Tcl_Obj *part, *var;

				part = Tcl_NewStringObj("row", -1);
				var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_MSG);
				if (var == NULL)
				{
					err = TCL_ERROR;
					break;
				}
				err = tclgetlinenum(interp, var, &val1, win->w_buffer);
				if (err != TCL_OK)
					break;
				part = Tcl_NewStringObj("column", -1);
				var = Tcl_ObjGetVar2(interp, objv[2], part, TCL_LEAVE_ERR_MSG);
				if (var == NULL)
				{
					err = TCL_ERROR;
					break;
				}
				err = Tcl_GetIntFromObj(interp, var, &val2);
				if (err != TCL_OK)
					break;
			}
			else {	/* objc == 4 */
				err = tclgetlinenum(interp, objv[2], &val1, win->w_buffer);
				if (err != TCL_OK)
					break;
				err = Tcl_GetIntFromObj(interp, objv[3], &val2);
				if (err != TCL_OK)
					break;
			}
			/* TODO: should check column */
			win->w_cursor.lnum = val1;
			win->w_cursor.col = col2vim(val2);
			flags |= FL_UPDATE_SCREEN;
			break;

		default:
			Tcl_SetResult(interp, "not implemented yet", TCL_STATIC);
			break;
	}

	curwin = savewin;
	curbuf = savebuf;
	if (flags & FL_UPDATE_SCREEN)
		update_screen(NOT_VALID);

	return err;
}


/* ARGSUSED */
	static int
commandcmd(dummy, interp, objc, objv)
	ClientData dummy;
	Tcl_Interp *interp;
	int objc;
	Tcl_Obj *CONST objv[];
{
	int err;
	err = tcldoexcommand(interp, objc, objv, 1);
	update_screen(NOT_VALID);
	return err;
}

/* ARGSUSED */
	static int
optioncmd(dummy, interp, objc, objv)
	ClientData dummy;
	Tcl_Interp *interp;
	int objc;
	Tcl_Obj *CONST objv[];
{
	int err;

	err = tclsetoption(interp, objc, objv, 1);
	update_screen(NOT_VALID);
	return err;
}

/* ARGSUSED */
	static int
exprcmd(dummy, interp, objc, objv)
	ClientData dummy;
	Tcl_Interp *interp;
	int objc;
	Tcl_Obj *CONST objv[];
{
	return tclvimexpr(interp, objc, objv, 1);
}

/****************************************************************************
  Support functions for Tcl commands
 ****************************************************************************/

/*
 * Get a line number from 'obj' and convert it to vim's range.
 */
static int
tclgetlinenum(interp, obj, valueP, buf)
	Tcl_Interp *interp;
	Tcl_Obj *obj;
	int *valueP;
	BUF *buf;
{
	int err, i;

	enum { LN_BEGIN, LN_BOTTOM, LN_END, LN_FIRST, LN_LAST, LN_START, LN_TOP };

	static char *keyw[] =
	{
		"begin", "bottom", "end", "first", "last", "start", "top", (char *)0
	};

	err = Tcl_GetIndexFromObj(interp, obj, keyw, "", 0, &i);
	if (err == TCL_OK)
	{
		switch (i)
		{
			case LN_BEGIN:
			case LN_FIRST:
			case LN_START:
			case LN_TOP:
				*valueP = 1;
				break;
			case LN_BOTTOM:
			case LN_END:
			case LN_LAST:
				*valueP = buf->b_ml.ml_line_count;
				break;
		}
		return TCL_OK;
	}
	Tcl_ResetResult(interp);

	err = Tcl_GetIntFromObj(interp, obj, &i);
	if (err != TCL_OK)
		return err;
	i = row2vim(i);
	if (i < 1  ||  i > buf->b_ml.ml_line_count)
	{
		Tcl_SetResult(interp, "line number out of range", TCL_STATIC);
		return TCL_ERROR;
	}
	*valueP = i;
	return TCL_OK;
}

/*
 * Find the first window in the window list that displays the buffer.
 */
static WIN *
tclfindwin(buf)
	BUF *buf;
{
	WIN *win;

	for (win = firstwin; win != NULL; win = win->w_next)
	{
		if (win->w_buffer == buf)
			return win;
	}
	return curwin;	/* keep current window context */
}

/*
 * Do-it-all function for "::vim::command", "$buf command" and "$win command".
 */
static int
tcldoexcommand(interp, objc, objv, objn)
	Tcl_Interp *interp;
	int objc;
	Tcl_Obj *CONST objv[];
	int objn;
{
	int save1, save2, save3;
	int err, flag, nobjs;
	char *arg;

	nobjs = objc - objn;
	if (nobjs < 1 || nobjs > 2)
	{
		Tcl_WrongNumArgs(interp, objn, objv, "?-quiet? exCommand");
		return TCL_ERROR;
	}

	flag = 0;
	if (nobjs == 2)
	{
		arg = Tcl_GetStringFromObj(objv[objn], NULL);
		if (strcmp(arg, "-quiet") == 0)
			flag = 1;
		else
		{
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp, "unknown flag: ", arg, (char *)0);
			return TCL_ERROR;
		}
		++objn;
	}

	save1 = tcl.range_start;
	save2 = tcl.range_end;
	save3 = tcl.lbase;

	arg = Tcl_GetStringFromObj(objv[objn], NULL);
	if (flag)
		++emsg_off;
	do_cmdline((char_u *)arg, NULL, NULL, DOCMD_NOWAIT|DOCMD_VERBOSE);
	if (flag)
		--emsg_off;
	err = vimerror(interp);

	tcl.range_start = save1;
	tcl.range_end = save2;
	tcl.lbase = save3;
	tclupdatevars();

	return err;
}

/*
 * Do-it-all function for "::vim::option", "$buf option" and "$win option".
 */
static int
tclsetoption(interp, objc, objv, objn)
	Tcl_Interp *interp;
	int objc;
	Tcl_Obj *CONST objv[];
	int objn;
{
	int err, nobjs, idx;
	char_u *option;
	int isnum;
	long lval;
	char_u *sval;
	Tcl_Obj *resobj;

	enum { OPT_OFF, OPT_ON, OPT_TOGGLE };
	static char *optkw[] = { "off", "on", "toggle", (char *)0 };

	nobjs = objc - objn;
	if (nobjs != 1 && nobjs != 2)
	{
		Tcl_WrongNumArgs(interp, objn, objv, "vimOption ?value?");
		return TCL_ERROR;
	}

	option = (char_u *)Tcl_GetStringFromObj(objv[objn], NULL);
	++objn;
	isnum = get_option_value(option, &lval, &sval);
	err = TCL_OK;
	switch (isnum)
	{
		case 0:
			Tcl_SetResult(interp, (char *)sval, TCL_VOLATILE);
			vim_free(sval);
			break;
		case 1:
			resobj = Tcl_NewLongObj(lval);
			Tcl_SetObjResult(interp, resobj);
			break;
		default:
			Tcl_SetResult(interp, "unknown vimOption", TCL_STATIC);
			return TCL_ERROR;
	}
	if (nobjs == 2)
	{
		if (isnum)
		{
			sval = NULL;	/* avoid compiler warning */
			err = Tcl_GetIndexFromObj(interp, objv[objn], optkw, "", 0, &idx);
			if (err != TCL_OK)
			{
				Tcl_ResetResult(interp);
				err = Tcl_GetLongFromObj(interp, objv[objn], &lval);
			}
			else
			switch (idx)
			{
				case OPT_ON:
					lval = 1;
					break;
				case OPT_OFF:
					lval = 0;
					break;
				case OPT_TOGGLE:
					lval = !lval;
					break;
			}
		}
		else
			sval = (char_u *)Tcl_GetStringFromObj(objv[objn], NULL);
		if (err == TCL_OK)
		{
			set_option_value(option, lval, sval);
			err = vimerror(interp);
		}
	}
	return err;
}

/*
 * Do-it-all function for "::vim::expr", "$buf expr" and "$win expr".
 */
static int
tclvimexpr(interp, objc, objv, objn)
	Tcl_Interp *interp;
	int objc;
	Tcl_Obj *CONST objv[];
	int objn;
{
#ifdef WANT_EVAL
	char *expr, *str;
#endif
	int err;

	if (objc - objn != 1)
	{
		Tcl_WrongNumArgs(interp, objn, objv, "vimExpr");
		return TCL_ERROR;
	}

#ifdef WANT_EVAL
	expr = Tcl_GetStringFromObj(objv[objn], NULL);
	str = (char *)eval_to_string((char_u *)expr, NULL);
	if (str == NULL)
		Tcl_SetResult(interp, "invalid expression", TCL_STATIC);
	else
		Tcl_SetResult(interp, str, TCL_VOLATILE);
	err = vimerror(interp);
#else
	Tcl_SetResult(interp, "expressions disabled at compile time", TCL_STATIC);
	err = TCL_ERROR;
#endif

	return err;
}

/*
 * Check for internal vim errors.
 */
static int
vimerror(interp)
	Tcl_Interp *interp;
{
	if (got_int)
	{
		Tcl_SetResult(interp, "keyboard interrupt", TCL_STATIC);
		return TCL_ERROR;
	}
	else if (did_emsg)
	{
		Tcl_SetResult(interp, "vim error", TCL_STATIC);
		return TCL_ERROR;
	}
	return TCL_OK;
}

/*
 * Functions that handle the reference lists:
 *   delref() - callback for Tcl's DeleteCommand
 *   tclgetref() - find/create Tcl command for a WIN* or BUF* object
 *   tclgetwindow() - window frontend for tclgetref()
 *   tclgetbuffer() - buffer frontend for tclgetref()
 *   tclsetdelcmd() - add Tcl callback command to a vim object
 */
static void
delref(cref)
	ClientData cref;
{
	struct ref *ref = (struct ref *)cref;

	if (ref->delcmd)
	{
		Tcl_DecrRefCount(ref->delcmd);
		ref->delcmd = NULL;
	}
	ref->interp = NULL;
}

static char *
tclgetref(interp, refstartP, prefix, vimobj, proc)
	Tcl_Interp *interp;
	void **refstartP;			/* ptr to tcl_ref member of WIN/BUF struct */
	char *prefix;				/* "win" or "buf" */
	void *vimobj;				/* WIN* or BUF* */
	Tcl_ObjCmdProc *proc;		/* winselfcmd or bufselfcmd */
{
	struct ref *ref, *unused = NULL;
	static char name[VARNAME_SIZE];
	Tcl_Command cmd;

	ref = (struct ref *)(*refstartP);
	if (ref == &refsdeleted)
	{
		Tcl_SetResult(interp, "cannot created buffer/window command: object is being deleted", TCL_STATIC);
		return NULL;
	}

	while (ref != NULL)
	{
		if (ref->interp == interp)
			break;
		if (ref->interp == NULL)
			unused = ref;
		ref = ref->next;
	}

	if (ref)
		sprintf(name, "::vim::%s", Tcl_GetCommandName(interp, ref->cmd));
	else
	{
		if (unused)
			ref = unused;
		else
		{
			ref = (struct ref *)Tcl_Alloc(sizeof(struct ref));
#if 0	/* Tcl_Alloc either succeeds or does not return */
			if (!ref)
			{
				Tcl_SetResult(interp, "out of memory", TCL_STATIC);
				return NULL;
			}
#endif
		}
		sprintf(name, "::vim::%s_%lx", prefix, (long)vimobj);
		cmd = Tcl_CreateObjCommand(interp, name, proc,
			(ClientData)ref, (Tcl_CmdDeleteProc *)delref);
		if (!cmd)
			return NULL;

		ref->interp = interp;
		ref->cmd = cmd;
		ref->delcmd = NULL;
		ref->vimobj = vimobj;
		ref->next = (struct ref *)(*refstartP);
		(*refstartP) = (void *)ref;
	}
	return name;
}

static char *
tclgetwindow(interp, win)
	Tcl_Interp *interp;
	WIN *win;
{
	return tclgetref(interp, &(win->tcl_ref), "win", (void *)win, winselfcmd);
}

static char *
tclgetbuffer(interp, buf)
	Tcl_Interp *interp;
	BUF *buf;
{
	return tclgetref(interp, &(buf->tcl_ref), "buf", (void *)buf, bufselfcmd);
}

static int
tclsetdelcmd(interp, reflist, vimobj, delcmd)
	Tcl_Interp *interp;
	struct ref *reflist;
	void *vimobj;
	Tcl_Obj *delcmd;
{
	if (reflist == &refsdeleted)
	{
		Tcl_SetResult(interp, "cannot register callback command: buffer/window is already being deleted", TCL_STATIC);
		return TCL_ERROR;
	}

	while (reflist != NULL)
	{
		if (reflist->interp == interp && reflist->vimobj == vimobj)
		{
			if (reflist->delcmd)
				Tcl_DecrRefCount(reflist->delcmd);
			Tcl_IncrRefCount(delcmd);
			reflist->delcmd = delcmd;
			return TCL_OK;
		}
		reflist = reflist->next;
	}
	/* This should never happen.  Famous last word? */
	Tcl_SetResult(interp, "TCL FATAL ERROR: reflist corrupt!? Please report this to vim-dev@vim.org", TCL_STATIC);
	return TCL_ERROR;
}


/*******************************************
	I/O Channel
********************************************/

/* ARGSUSED */
	static int
channel_close(instance, interp)
	ClientData instance;
	Tcl_Interp *interp;
{
	int err = 0;

	/* currently does nothing */

	if (instance != VIMOUT && instance != VIMERR)
	{
		Tcl_SetErrno(EBADF);
		err = EBADF;
	}
	return err;
}

/* ARGSUSED */
	static int
channel_input(instance, buf, bufsiz, errptr)
	ClientData instance;
	char *buf;
	int bufsiz;
	int *errptr;
{

	/* input is currently not supported */

	Tcl_SetErrno(EINVAL);
	if (errptr)
		*errptr = EINVAL;
	return -1;
}

static int
channel_output(instance, buf, bufsiz, errptr)
	ClientData instance;
	char *buf;
	int bufsiz;
	int *errptr;
{
	char_u *str;
	int result;

	/* The buffer is not guaranteed to be 0-terminated, and we don't if
	 * there is enough room to add a '\0'.  So we have to create a copy
	 * of the buffer...
	 */
	str = vim_strnsave((char_u *)buf, bufsiz);
	if (!str)
	{
		Tcl_SetErrno(ENOMEM);
		if (errptr)
			*errptr = ENOMEM;
		return -1;
	}

	result = bufsiz;
	if (instance == VIMOUT)
		tclmsg((char *)str);
	else
	if (instance == VIMERR)
		tclerrmsg((char *)str);
	else
	{
		Tcl_SetErrno(EBADF);
		if (errptr)
			*errptr = EBADF;
		result = -1;
	}
	vim_free(str);
	return result;
}

/* ARGSUSED */
	static void
channel_watch(instance, mask)
	ClientData instance;
	int mask;
{
	Tcl_SetErrno(EINVAL);
}

/* ARGSUSED */
	static int
channel_gethandle(instance, direction, handleptr)
	ClientData instance;
	int direction;
	ClientData *handleptr;
{
	Tcl_SetErrno(EINVAL);
	return EINVAL;
}


static Tcl_ChannelType channel_type =
{
	"vimmessage",
	NULL,   /* blockmode */
	channel_close,
	channel_input,
	channel_output,
	NULL,   /* seek */
	NULL,   /* set option */
	NULL,   /* get option */
	channel_watch,
	channel_gethandle
};

/**********************************
  Interface to vim
 **********************************/

static void
tclupdatevars()
{
	char varname[VARNAME_SIZE];	/* must be writeable */
	char *name;

	strcpy(varname, VAR_RANGE1);
	Tcl_UpdateLinkedVar(tcl.interp, varname);
	strcpy(varname, VAR_RANGE2);
	Tcl_UpdateLinkedVar(tcl.interp, varname);
	strcpy(varname, VAR_RANGE3);
	Tcl_UpdateLinkedVar(tcl.interp, varname);

	strcpy(varname, VAR_LBASE);
	Tcl_UpdateLinkedVar(tcl.interp, varname);

	name = tclgetbuffer(tcl.interp, curbuf);
	strcpy(tcl.curbuf, name);
	strcpy(varname, VAR_CURBUF);
	Tcl_UpdateLinkedVar(tcl.interp, varname);

	name = tclgetwindow(tcl.interp, curwin);
	strcpy(tcl.curwin, name);
	strcpy(varname, VAR_CURWIN);
	Tcl_UpdateLinkedVar(tcl.interp, varname);
}


static void
tclinit(eap)
	EXARG *eap;
{
	char varname[VARNAME_SIZE];	/* must be writeable memory */
	char *name;

	if (!tcl.valid)
	{
		static Tcl_ChannelType ct1, ct2;
		Tcl_Channel ch1, ch2;

		tcl.interp = Tcl_CreateInterp();
		if (Tcl_Init(tcl.interp) == TCL_ERROR)
			return;
		tcl.do_exit = 0;
#if 0
		/* VIM sure is interactive */
		Tcl_SetVar(tcl.interp, "tcl_interactive", "1", TCL_GLOBAL_ONLY);
#endif

		/* replace stdout and stderr */
		ct1 = channel_type;
		ch1 = Tcl_CreateChannel(&ct1, "vimout", VIMOUT, TCL_WRITABLE);
		Tcl_RegisterChannel(tcl.interp, ch1);
		Tcl_SetStdChannel(ch1, TCL_STDOUT);
		Tcl_SetChannelOption(tcl.interp, ch1, "-buffering", "line");
		ct2 = channel_type;
		ch2 = Tcl_CreateChannel(&ct2, "vimerr", VIMERR, TCL_WRITABLE);
		Tcl_RegisterChannel(tcl.interp, ch2);
		Tcl_SetStdChannel(ch2, TCL_STDERR);
		Tcl_SetChannelOption(tcl.interp, ch2, "-buffering", "line");

		/* replace some standard Tcl commands */
		Tcl_DeleteCommand(tcl.interp, "exit");
		Tcl_CreateObjCommand(tcl.interp, "exit", exitcmd,
			(ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

		/* new commands, in ::vim namespace */
		Tcl_CreateObjCommand(tcl.interp, "::vim::buffer", buffercmd,
			(ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
		Tcl_CreateObjCommand(tcl.interp, "::vim::window", windowcmd,
		   (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
		Tcl_CreateObjCommand(tcl.interp, "::vim::command", commandcmd,
		   (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
		Tcl_CreateObjCommand(tcl.interp, "::vim::beep", beepcmd,
		   (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
		Tcl_CreateObjCommand(tcl.interp, "::vim::option", optioncmd,
		   (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
		Tcl_CreateObjCommand(tcl.interp, "::vim::expr", exprcmd,
		   (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

		/* "lbase" variable */
		tcl.lbase = 1;
		strcpy(varname, VAR_LBASE);
		Tcl_LinkVar(tcl.interp, varname, (char *)&tcl.lbase, TCL_LINK_INT);

		/* "range" variable */
		tcl.range_start = eap->line1;
		strcpy(varname, VAR_RANGE1);
		Tcl_LinkVar(tcl.interp, varname, (char *)&tcl.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
		strcpy(varname, VAR_RANGE2);
		Tcl_LinkVar(tcl.interp, varname, (char *)&tcl.range_start, TCL_LINK_INT|TCL_LINK_READ_ONLY);
		tcl.range_end   = eap->line2;
		strcpy(varname, VAR_RANGE3);
		Tcl_LinkVar(tcl.interp, varname, (char *)&tcl.range_end, TCL_LINK_INT|TCL_LINK_READ_ONLY);

		/* "current" variable */
		tcl.curbuf = Tcl_Alloc(VARNAME_SIZE);
		tcl.curwin = Tcl_Alloc(VARNAME_SIZE);
		name = tclgetbuffer(tcl.interp, curbuf);
		strcpy(tcl.curbuf, name);
		strcpy(varname, VAR_CURBUF);
		Tcl_LinkVar(tcl.interp, varname, (char *)&tcl.curbuf, TCL_LINK_STRING|TCL_LINK_READ_ONLY);
		name = tclgetwindow(tcl.interp, curwin);
		strcpy(tcl.curwin, name);
		strcpy(varname, VAR_CURWIN);
		Tcl_LinkVar(tcl.interp, varname, (char *)&tcl.curwin, TCL_LINK_STRING|TCL_LINK_READ_ONLY);

		tcl.valid = 1;
	}
	else
	{
		/* Interpreter already exists, just update variables */
		tcl.range_start = row2tcl(eap->line1);
		tcl.range_end = row2tcl(eap->line2);
		tclupdatevars();
	}

	Tcl_Preserve(tcl.interp); /* protect interpreter from deletion */
	++tcl.refcount;
}

static void
tclerrmsg(text)
	char *text;
{
	char *next;

	while ((next=strchr(text, '\n')))
	{
		*next++ = '\0';
		EMSG(text);
		text = next;
	}
	if (*text)
		EMSG(text);
}

static void
tclmsg(text)
	char *text;
{
	char *next;

	while ((next=strchr(text, '\n')))
	{
		*next++ = '\0';
		MSG(text);
		text = next;
	}
	if (*text)
		MSG(text);
}

static int
tclexit(error)
	int error;
{
	int newerr;
	char *result;

	result = Tcl_GetStringResult(tcl.interp);
	if (error == TCL_OK)
	{
		tclmsg(result);
		newerr = OK;
	}
	else
	{
		tclerrmsg(result);
		newerr = FAIL;
	}
	if (--tcl.refcount == 0  &&  tcl.do_exit)
	{
		if (!Tcl_InterpDeleted(tcl.interp))
			Tcl_DeleteInterp(tcl.interp);
		Tcl_Free(tcl.curbuf);
		Tcl_Free(tcl.curwin);
		tcl.valid = 0;
		/* TODO: should call Tcl_UnlinkVar */
	}
	Tcl_Release(tcl.interp);

	return newerr;
}

int
do_tcl(eap)
	EXARG *eap;
{
	char *script = (char *)eap->arg;
	int err;

	tclinit(eap);
	err = Tcl_Eval(tcl.interp, script);
	return tclexit(err);
}

int
do_tclfile(eap)
	EXARG *eap;
{
	char *file = (char *)eap->arg;
	int err;

	tclinit(eap);
	err = Tcl_EvalFile(tcl.interp, file);
	return tclexit(err);
}

int
do_tcldo(eap)
	EXARG *eap;
{
	char *script, *line;
	int err, rs, re, lnum;
	char var_lnum[VARNAME_SIZE];	/* must be writeable memory */
	char var_line[VARNAME_SIZE];

	rs = eap->line1;
	re = eap->line2;
	script = (char *)eap->arg;
	strcpy(var_lnum, VAR_CURLNUM);
	strcpy(var_line, VAR_CURLINE);

	tclinit(eap);

	lnum = row2tcl(rs);
	Tcl_LinkVar(tcl.interp, var_lnum, (char *)&lnum, TCL_LINK_INT|TCL_LINK_READ_ONLY);
	err = TCL_OK;
	if (u_save((linenr_t)(rs-1), (linenr_t)(re+1)) != OK)
	{
		Tcl_SetResult(tcl.interp, "cannot save undo information", TCL_STATIC);
		err = TCL_ERROR;
	}
	while (err == TCL_OK  &&  rs <= re)
	{
		line = (char *)ml_get_buf(curbuf, (linenr_t)rs, FALSE);
		if (!line)
		{
			Tcl_SetResult(tcl.interp, "cannot get line", TCL_STATIC);
			err = TCL_ERROR;
			break;
		}
		Tcl_SetVar(tcl.interp, var_line, line, 0);
		err = Tcl_Eval(tcl.interp, script);
		if (err != TCL_OK)
			break;
		line = Tcl_GetVar(tcl.interp, var_line, 0);
		if (line)
		{
			if (ml_replace((linenr_t)rs, (char_u *)line, TRUE) != OK)
			{
				Tcl_SetResult(tcl.interp, "cannot replace line", TCL_STATIC);
				err = TCL_ERROR;
				break;
			}
			changed();
#ifdef SYNTAX_HL
			syn_changed((linenr_t)rs); /* recompute syntax hl. for this line */
#endif
		}
		++rs;
		++lnum;
		Tcl_UpdateLinkedVar(tcl.interp, var_lnum);
	}
	update_curbuf(NOT_VALID);

	Tcl_UnsetVar(tcl.interp, var_line, 0);
	Tcl_UnlinkVar(tcl.interp, var_lnum);
	if (err == TCL_OK)
		Tcl_ResetResult(tcl.interp);

	return tclexit(err);
}

static void
tcldelallrefs(ref)
	struct ref *ref;
{
	struct ref *next;
	int err;
	char *result;

	while (ref != NULL)
	{
		next = ref->next;
		if (ref->interp)
		{
			if (ref->delcmd)
			{
				err = Tcl_GlobalEvalObj(ref->interp, ref->delcmd);
				if (err != TCL_OK)
				{
					result = Tcl_GetStringResult(ref->interp);
					if (result)
						tclerrmsg(result);
				}
				Tcl_DecrRefCount(ref->delcmd);
				ref->delcmd = NULL;
			}
			Tcl_DeleteCommandFromToken(ref->interp, ref->cmd);
		}
		Tcl_Free((char *)ref);
		ref = next;
	}
}

void
tcl_buffer_free(buf)
	BUF *buf;
{
	struct ref *reflist;

	reflist = (struct ref*)(buf->tcl_ref);
	if (reflist != &refsdeleted)
	{
		buf->tcl_ref = (void *)&refsdeleted;
		tcldelallrefs(reflist);
		buf->tcl_ref = NULL;
	}
}

void
tcl_window_free(win)
	WIN *win;
{
	struct ref *reflist;

	reflist = (struct ref*)(win->tcl_ref);
	if (reflist != &refsdeleted)
	{
		win->tcl_ref = (void *)&refsdeleted;
		tcldelallrefs(reflist);
		win->tcl_ref = NULL;
	}
}
/* The End */
