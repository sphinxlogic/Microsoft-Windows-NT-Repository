/*		FILE WRITER				HTFWrite.h
**		===========
**
**	This version of the stream object just writes to a C file.
**	The file is assumed open and left open.
**
**	Bugs:
**		strings written must be less than buffer size.
*/

#include <HTUtils.h>
#include <LYCurses.h>
#include <HTFWriter.h>
#include <HTSaveToFile.h>

#ifdef WIN_EX
#include <HTParse.h>
#endif

#if _WIN_CC
extern int exec_command(char * cmd, int wait_flag); /* xsystem.c */
#endif

#include <HTFormat.h>
#include <UCDefs.h>
#include <HTAlert.h>
#include <HTFile.h>
#include <HTPlain.h>

#include <LYStrings.h>
#include <LYUtils.h>
#include <LYGlobalDefs.h>
#include <LYClean.h>
#include <GridText.h>
#include <LYExtern.h>
#include <LYexit.h>
#include <LYLeaks.h>
#include <LYKeymap.h>

#ifdef EXP_PERSISTENT_COOKIES 
#include <LYCookie.h>
#endif

PUBLIC char * WWW_Download_File=NULL; /* contains the name of the temp file
				      ** which is being downloaded into
				      */
PUBLIC BOOLEAN LYCancelDownload=FALSE;   /* exported to HTFormat.c in libWWW */

#ifdef VMS
PRIVATE char * FIXED_RECORD_COMMAND = NULL;
#ifdef USE_COMMAND_FILE 	     /* Keep this as an option. - FM	*/
#define FIXED_RECORD_COMMAND_MASK "@Lynx_Dir:FIXED512 %s"
#else
#define FIXED_RECORD_COMMAND_MASK "%s"
PUBLIC unsigned long LYVMS_FixedLengthRecords PARAMS((char *filename));
#endif /* USE_COMMAND_FILE */
#endif /* VMS */

PUBLIC HTStream* HTSaveToFile PARAMS((
	HTPresentation *       pres,
	HTParentAnchor *       anchor,
	HTStream *	       sink));

/*	Stream Object
**	-------------
*/
struct _HTStream {
	CONST HTStreamClass *	isa;

	FILE *			fp;		/* The file we've opened */
	char *			end_command;	/* What to do on _free.  */
	char *			remove_command; /* What to do on _abort. */
	char *			viewer_command; /* Saved external viewer */
	HTFormat		input_format;  /* Original pres->rep	 */
	HTFormat		output_format; /* Original pres->rep_out */
	HTParentAnchor *	anchor;     /* Original stream's anchor. */
	HTStream *		sink;	    /* Original stream's sink.	 */
#ifdef FNAMES_8_3
	BOOLEAN			idash; /* remember position to become '.'*/
#endif
};


/*_________________________________________________________________________
**
**			A C T I O N	R O U T I N E S
**  Bug:
**	All errors are ignored.
*/

/*	Character handling
**	------------------
*/
PRIVATE void HTFWriter_put_character ARGS2(HTStream *, me, char, c)
{
    if (me->fp) {
	putc(c, me->fp);
    }
}

/*	String handling
**	---------------
**
**	Strings must be smaller than this buffer size.
*/
PRIVATE void HTFWriter_put_string ARGS2(HTStream *, me, CONST char*, s)
{
    if (me->fp) {
	fputs(s, me->fp);
    }
}

/*	Buffer write.  Buffers can (and should!) be big.
**	------------
*/
PRIVATE void HTFWriter_write ARGS3(HTStream *, me, CONST char*, s, int, l)
{
    if (me->fp) {
	fwrite(s, 1, l, me->fp);
    }
}




/*	Free an HTML object
**	-------------------
**
**	Note that the SGML parsing context is freed, but the created
**	object is not,
**	as it takes on an existence of its own unless explicitly freed.
*/
PRIVATE void HTFWriter_free ARGS1(HTStream *, me)
{
    int len;
    char *path = NULL;
    char *addr = NULL;
    int status;
    BOOL use_gzread = NO;
    BOOLEAN found = FALSE;
#ifdef WIN_EX
    HANDLE cur_handle;

    cur_handle = GetForegroundWindow();
#endif

    if (me->fp)
	fflush(me->fp);
    if (me->end_command) {		/* Temp file */
	LYCloseTempFP(me->fp);
#ifdef VMS
	if (0 == strcmp(me->end_command, "SaveVMSBinaryFile")) {
	/*
	 *  It's a binary file saved to disk on VMS, which
	 *  we want to convert to fixed records format. - FM
	 */
#ifdef USE_COMMAND_FILE
	    LYSystem(FIXED_RECORD_COMMAND);
#else
	    LYVMS_FixedLengthRecords(FIXED_RECORD_COMMAND);
#endif /* USE_COMMAND_FILE */
	    FREE(FIXED_RECORD_COMMAND);

	    if (me->remove_command) {
		/* NEVER REMOVE THE FILE unless during an abort!*/
		FREE(me->remove_command);
	    }
	} else
#endif /* VMS */
	if (me->input_format == HTAtom_for("www/compressed")) {
	    /*
	     *	It's a compressed file supposedly cached to
	     *	a temporary file for uncompression. - FM
	     */
	    if (me->anchor->FileCache != NULL) {
		BOOL skip_loadfile = (BOOL) (me->viewer_command != NULL);
		/*
		 *  Save the path with the "gz" or "Z" suffix trimmed,
		 *  and remove any previous uncompressed copy. - FM
		 */
		StrAllocCopy(path, me->anchor->FileCache);
		if ((len = strlen(path)) > 3 &&
		    !strcasecomp(&path[len-2], "gz")) {
#ifdef USE_ZLIB
		    if (!skip_loadfile) {
			use_gzread = YES;
		    } else
#endif /* USE_ZLIB */
		    {
			path[len-3] = '\0';
			remove(path);
		    }
#ifdef BZIP2_PATH
		} else if (len > 4 && !strcasecomp(&path[len-3], "bz2")) {
		    path[len-4] = '\0';
		    remove(path);
#endif /* BZIP2_PATH */
		} else if (len > 2 && !strcasecomp(&path[len-1], "Z")) {
		    path[len-2] = '\0';
		    remove(path);
		}
		if (!use_gzread) {
		    if (!dump_output_immediately) {
			/*
			 *  Tell user what's happening. - FM
			 */
			_HTProgress(me->end_command);
		    }
		    /*
		     *	Uncompress it. - FM
		     */
		    if (me->end_command && me->end_command[0])
			LYSystem(me->end_command);
		    found = LYCanReadFile(me->anchor->FileCache);
		}
		if (found) {
		    /*
		     *	It's still there with the "gz" or "Z" suffix,
		     *	so the uncompression failed. - FM
		     */
		    if (!dump_output_immediately) {
			lynx_force_repaint();
			refresh();
		    }
		    HTAlert(ERROR_UNCOMPRESSING_TEMP);
		    LYRemoveTemp(me->anchor->FileCache);
		    FREE(me->anchor->FileCache);
		} else {
		    /*
		     *	Succeeded!  Create a complete address
		     *	for the uncompressed file and invoke
		     *	HTLoadFile() to handle it. - FM
		     */
#ifdef FNAMES_8_3
		    /*
		     *	Assuming we have just uncompressed e.g.
		     *	FILE-mpeg.gz -> FILE-mpeg, restore/shorten
		     *	the name to be fit for passing to an external
		     *	viewer, by renaming FILE-mpeg -> FILE.mpe  - kw
		     */
		    if (skip_loadfile) {
			char *new_path = NULL;
			char *the_dash = me->idash ? strrchr(path, '-') : 0;
			if (the_dash != 0) {
			    unsigned off = (the_dash - path);
			    StrAllocCopy(new_path, path);
			    new_path[off] = '.';
			    if (strlen(new_path + off) > 4)
				new_path[off + 4] = '\0';
			    if (rename(path, new_path) == 0) {
				FREE(path);
				path = new_path;
			    } else {
				FREE(new_path);
			    }
			}
		    }
#endif /* FNAMES_8_3 */
		    LYLocalFileToURL (&addr, path);
		    if (!use_gzread) {
			LYRenamedTemp(me->anchor->FileCache, path);
			StrAllocCopy(me->anchor->FileCache, path);
			StrAllocCopy(me->anchor->content_encoding, "binary");
		    }
		    FREE(path);
		    if (!skip_loadfile) {
			/*
			 *  Lock the chartrans info we may possibly have,
			 *  so HTCharsetFormat() will not apply the default
			 *  for local files. - KW
			 */
			if (HTAnchor_getUCLYhndl(me->anchor,
						 UCT_STAGE_PARSER) < 0 ) {
			    /*
			     *	If not yet set - KW
			     */
			    HTAnchor_copyUCInfoStage(me->anchor,
						     UCT_STAGE_PARSER,
						     UCT_STAGE_MIME,
						     UCT_SETBY_DEFAULT+1);
			}
			HTAnchor_copyUCInfoStage(me->anchor,
						 UCT_STAGE_PARSER,
						 UCT_STAGE_MIME, -1);
		    }
		    /*
		     *	Create a complete address for
		     *	the uncompressed file. - FM
		     */
		    if (!dump_output_immediately) {
			/*
			 *  Tell user what's happening. - FM
			 */
			_user_message(WWW_USING_MESSAGE, addr);
		    }

		    if (skip_loadfile) {
			/*
			 *  It's a temporary file we're passing to a
			 *  viewer or helper application.
			 *  Loading the temp file through HTLoadFile()
			 *  would result in yet another HTStream (created
			 *  with HTSaveAndExecute()) which would just
			 *  copy the temp file to another temp file
			 *  (or even the same!).  We can skip this
			 *  needless duplication by using the
			 *  viewer_command which has already been
			 *  determined when the HTCompressed stream was
			 *  created. - kw
			 */
			FREE(me->end_command);

			HTAddParam(&(me->end_command), me->viewer_command, 1, me->anchor->FileCache);
			HTEndParam(&(me->end_command), me->viewer_command, 1);

			if (!dump_output_immediately) {
			    /*
			     *	Tell user what's happening. - FM
			     */
			    HTProgress(me->end_command);
#ifndef WIN_EX
			    stop_curses();
#endif
			}
#if _WIN_CC
			exec_command(me->end_command, FALSE);
#else
			LYSystem(me->end_command);
#endif
			if (me->remove_command) {
			    /* NEVER REMOVE THE FILE unless during an abort!!!*/
			    FREE(me->remove_command);
			}
			if (!dump_output_immediately) {
#ifdef WIN_EX
			    if (focus_window) {
				HTInfoMsg("Set focus1");
				status = SetForegroundWindow(cur_handle);
			    }
#else
			    start_curses();
#endif
			}
		    } else
		    status = HTLoadFile(addr,
					me->anchor,
					me->output_format,
					me->sink);
		    if (dump_output_immediately &&
			me->output_format == HTAtom_for("www/present")) {
			FREE(addr);
			remove(me->anchor->FileCache);
			FREE(me->anchor->FileCache);
			FREE(me->remove_command);
			FREE(me->end_command);
			FREE(me->viewer_command);
			FREE(me);
			return;
		    }
		}
		FREE(addr);
	    }
	    if (me->remove_command) {
		/* NEVER REMOVE THE FILE unless during an abort!!!*/
		FREE(me->remove_command);
	    }
	} else if (strcmp(me->end_command, "SaveToFile")) {
	    /*
	     *	It's a temporary file we're passing to a
	     *	viewer or helper application. - FM
	     */
	    if (!dump_output_immediately) {
		/*
		 *  Tell user what's happening. - FM
		 */
		_HTProgress(me->end_command);
#ifndef WIN_EX
		stop_curses();
#endif
	    }
#if _WIN_CC
	    exec_command(me->end_command, FALSE);
#else
	    LYSystem(me->end_command);
#endif

	    if (me->remove_command) {
		/* NEVER REMOVE THE FILE unless during an abort!!!*/
		FREE(me->remove_command);
	    }
	    if (!dump_output_immediately) {
#ifdef WIN_EX
		if (focus_window) {
		    HTInfoMsg("Set focus2");
		    status = SetForegroundWindow(cur_handle);
		}
#else
		start_curses();
#endif
	    }
	} else {
	    /*
	     *	It's a file we saved to disk for handling
	     *	via a menu. - FM
	     */
	    if (me->remove_command) {
		/* NEVER REMOVE THE FILE unless during an abort!!!*/
		FREE(me->remove_command);
	    }
	    if (!dump_output_immediately) {
#ifdef WIN_EX
		if (focus_window) {
		    HTInfoMsg("Set focus3");
		    status = SetForegroundWindow(cur_handle);
		}
#else
	        start_curses();
#endif
	    }
	}
	FREE(me->end_command);
    }
    FREE(me->viewer_command);

    if (dump_output_immediately) {
	if (me->anchor->FileCache)
	    remove(me->anchor->FileCache);
	FREE(me);
#ifdef EXP_PERSISTENT_COOKIES 
	/* 
	 *  We want to save cookies picked up when in source 
	 *  mode.  ... 
	 */ 
	if (persistent_cookies) 
	    LYStoreCookies(LYCookieSaveFile); 
#endif /* EXP_PERSISTENT_COOKIES */ 
	exit_immediately(0);
    }

    FREE(me);
    return;
}

/*	Abort writing
**	-------------
*/
PRIVATE void HTFWriter_abort ARGS2(
	HTStream *,	me,
	HTError,	e GCC_UNUSED)
{
    CTRACE((tfp,"HTFWriter_abort called\n"));
    LYCloseTempFP(me->fp);
    FREE(me->viewer_command);
    if (me->end_command) {		/* Temp file */
	CTRACE((tfp, "HTFWriter: Aborting: file not executed or saved.\n"));
	FREE(me->end_command);
	if (me->remove_command) {
	    LYSystem(me->remove_command);
	    FREE(me->remove_command);
	}
    }

    FREE(WWW_Download_File);

    FREE(me);
}

/*	Structured Object Class
**	-----------------------
*/
PRIVATE CONST HTStreamClass HTFWriter = /* As opposed to print etc */
{
	"FileWriter",
	HTFWriter_free,
	HTFWriter_abort,
	HTFWriter_put_character,	HTFWriter_put_string,
	HTFWriter_write
};

/*	Subclass-specific Methods
**	-------------------------
*/
PUBLIC HTStream* HTFWriter_new ARGS1(FILE *, fp)
{
    HTStream* me;

    if (!fp)
	return NULL;

    me = typecalloc(HTStream);
    if (me == NULL)
	outofmem(__FILE__, "HTFWriter_new");
    me->isa = &HTFWriter;

    me->fp = fp;
    me->end_command = NULL;
    me->remove_command = NULL;
    me->anchor = NULL;
    me->sink = NULL;

    return me;
}

PRIVATE void chrcat ARGS2(
	char *,		result,
	int,		ch)
{
    result += strlen(result);
    *result++ = ch;
    *result = 0;
}

/*	Make system command from template
**	---------------------------------
**
**	See mailcap spec for description of template.
*/
PRIVATE char *mailcap_substitute ARGS3(
	HTParentAnchor *,	anchor,
	HTPresentation *,	pres,
	char *,			fnam)
{
    int pass;
    int skip;
    size_t need = 0;
    char *result = 0;
    char *s;
    char *repl;

    for (pass = 0; pass < 2; pass++) {
	for (s = pres->command; *s; s++) {
	    if (*s == '%') {
		repl = 0;
		skip = 0;
		if (s[1] == 't') {
		    repl = pres->rep->name;
		    skip = 1;
		} else if (s[1] == 's') {
		    repl = fnam;
		    skip = 1;
		} else if (!strncasecomp(s+1, "{charset}", 9)) {
		    repl = anchor->charset;
		    skip = 9;
		} else if (!strncasecomp(s+1, "{encoding}", 10)) {
		    repl = anchor->content_encoding;
		    skip = 10;
		}
		if (skip != 0) {
		    if (repl == 0)
			repl = "";
		    if (pass) {
			strcat(result, repl);
		    } else {
			need += strlen(repl);
		    }
		    s += skip;
		} else {
		    if (pass) {
			chrcat(result, *s);
		    } else {
			need++;
		    }
		}
	    } else {
		if (pass) {
		    chrcat(result, *s);
		} else {
		    need++;
		}
	    }
	}
	if (pass == 0) {
	    if ((result = malloc(need)) == 0)
		outofmem(__FILE__, "mailcap_substitute");
	    *result = 0;
	}
    }
#if defined(UNIX)
    /* if we don't have a "%s" token, expect to provide the file via stdin */
    if (strstr(pres->command, "%s") == 0) {
	char *prepend = 0;
	char *format = "( %s ) < %s";
	HTSprintf(&prepend, "( %s", pres->command); /* ...avoid quoting */
	HTAddParam(&prepend, format, 2, fnam); /* ...to quote if needed */
	FREE(result);
	result = prepend;
    }
#endif
    return result;
}

#ifndef VMS
#define REMOVE_COMMAND "/bin/rm -f %s"
#else
#define REMOVE_COMMAND "delete/noconfirm/nolog %s;"
#endif /* VMS */

/*	Take action using a system command
**	----------------------------------
**
**	originally from Ghostview handling by Marc Andreseen.
**	Creates temporary file, writes to it, executes system command
**	on end-document.  The suffix of the temp file can be given
**	in case the application is fussy, or so that a generic opener can
**	be used.
*/
PUBLIC HTStream* HTSaveAndExecute ARGS3(
	HTPresentation *,	pres,
	HTParentAnchor *,	anchor,
	HTStream *,		sink)
{
    char fnam[LY_MAXPATH];
    CONST char *suffix;
    HTStream* me;

    if (traversal) {
	LYCancelledFetch = TRUE;
	return(NULL);
    }
#if defined(EXEC_LINKS) || defined(EXEC_SCRIPTS)
    if (pres->quality == 999.0) { /* exec link */
	if (dump_output_immediately) {
	    LYCancelledFetch = TRUE;
	    return(NULL);
	}
	if (no_exec) {
	    HTAlert(EXECUTION_DISABLED);
	    return HTPlainPresent(pres, anchor, sink);
	}
	if (!local_exec) {
	    if (local_exec_on_local_files &&
		(LYJumpFileURL ||
		 !strncmp(anchor->address,"file://localhost",16))) {
		/* allow it to continue */
		;
	    } else {
		char *buf = 0;

		HTSprintf0(&buf, EXECUTION_DISABLED_FOR_FILE,
			   key_for_func(LYK_OPTIONS));
		HTAlert(buf);
		FREE(buf);
		return HTPlainPresent(pres, anchor, sink);
	    }
	}
    }
#endif /* EXEC_LINKS || EXEC_SCRIPTS */

    if (dump_output_immediately) {
	return(HTSaveToFile(pres, anchor, sink));
    }

    me = typecalloc(HTStream);
    if (me == NULL)
	outofmem(__FILE__, "HTSaveAndExecute");
    me->isa = &HTFWriter;
    me->input_format = pres->rep;
    me->output_format = pres->rep_out;
    me->anchor = anchor;
    me->sink = sink;

    if (LYCachedTemp(fnam, &(anchor->FileCache))) {
	/*  This used to be LYNewBinFile(fnam); changed to a different call
	 *  so that the open fp gets registered in the list keeping track of
	 *  temp files, equivalent to when LYOpenTemp() gets called below.
	 *  This avoids a file descriptor leak caused by LYCloseTempFP()
	 *  not being able to find the fp.  The ".bin" suffix is expected
	 *  to not be used, it's only for fallback in unusual error cases. - kw
	 */
	me->fp = LYOpenTempRewrite(fnam, ".bin", "wb");
    } else {
#if defined(WIN_EX) && !defined(__CYGWIN__)	/* 1998/01/04 (Sun) */
	if (!strncmp(anchor->address,"file://localhost",16)) {

	    /* 1998/01/23 (Fri) 17:38:26 */
	    unsigned char *cp, *view_fname;

#define IS_SJIS_HI1(hi) ((0x81<=hi)&&(hi<=0x9F))	/* 1st lev. */
#define IS_SJIS_HI2(hi) ((0xE0<=hi)&&(hi<=0xEF))	/* 2nd lev. */

	    me->fp = NULL;

	    view_fname = fnam + 3;
	    LYstrncpy(view_fname, anchor->address + 17, sizeof(fnam) - 5);
	    HTUnEscape(view_fname);

	    if (strchr(view_fname, ':')==NULL) {
		fnam[0] = windows_drive[0];
		fnam[1] = windows_drive[1];
		fnam[2] = '/';
		view_fname = fnam;
	    }

	    /* 1998/04/21 (Tue) 11:04:16 */
	    cp = view_fname;
	    while (*cp) {
		if (IS_SJIS_HI1(*cp) || IS_SJIS_HI2(*cp)) {
		    cp += 2;
		    continue;
		} else if (*cp == '/') {
		    *cp = '\\';
		}
		cp++;
	    }
	    if (strchr(view_fname, ' '))
		view_fname = (unsigned char *) quote_pathname(view_fname);

	    StrAllocCopy(me->viewer_command, pres->command);

	    me->end_command = mailcap_substitute(anchor, pres, view_fname);
	    me->remove_command = NULL;

	    return me;
	}
#endif
	/*
	 *  Check for a suffix.
	 *  Save the file under a suitably suffixed name.
	 */
	if (!strcasecomp(pres->rep->name, "text/html")) {
	    suffix = HTML_SUFFIX;
	} else if (!strcasecomp(pres->rep->name, "text/plain")) {
	    suffix = ".txt";
	} else if (!strcasecomp(pres->rep->name,
				"application/octet-stream")) {
	    suffix = ".bin";
	} else if (
	(suffix = HTFileSuffix(pres->rep, anchor->content_encoding)) == 0
		   || *suffix != '.')
	{
	    suffix = HTML_SUFFIX;
	}
	me->fp = LYOpenTemp(fnam, suffix, "wb");
    }

    if (!me->fp) {
	HTAlert(CANNOT_OPEN_TEMP);
	FREE(me);
	return NULL;
    }

    StrAllocCopy(me->viewer_command, pres->command);
    /*
     *	Make command to process file.
     */
    me->end_command = mailcap_substitute(anchor, pres, fnam);

    /*
     *	Make command to delete file.
     */
    me->remove_command = 0;
    HTAddParam(&(me->remove_command), REMOVE_COMMAND, 1, fnam);
    HTEndParam(&(me->remove_command), REMOVE_COMMAND, 1);

    StrAllocCopy(anchor->FileCache, fnam);
    return me;
}


/*	Format Converter using system command
**	-------------------------------------
*/

/* @@@@@@@@@@@@@@@@@@@@@@ */

/*	Save to a local file   LJM!!!
**	--------------------
**
**	usually a binary file that can't be displayed
**
**	originally from Ghostview handling by Marc Andreseen.
**	Asks the user if he wants to continue, creates a temporary
**	file, and writes to it.  In HTSaveToFile_Free
**	the user will see a list of choices for download
*/
PUBLIC HTStream* HTSaveToFile ARGS3(
	HTPresentation *,	pres,
	HTParentAnchor *,	anchor,
	HTStream *,		sink)
{
    HTStream * ret_obj;
    char fnam[LY_MAXPATH];
    CONST char * suffix;
    char *cp;
    int c = 0;
    BOOL IsBinary = TRUE;

    ret_obj = typecalloc(HTStream);
    if (ret_obj == NULL)
	outofmem(__FILE__, "HTSaveToFile");
    ret_obj->isa = &HTFWriter;
    ret_obj->remove_command = NULL;
    ret_obj->end_command = NULL;
    ret_obj->input_format = pres->rep;
    ret_obj->output_format = pres->rep_out;
    ret_obj->anchor = anchor;
    ret_obj->sink = sink;

    if (dump_output_immediately) {
	ret_obj->fp = stdout; /* stdout*/
	if (HTOutputFormat == HTAtom_for("www/download"))
	    goto Prepend_BASE;
	return ret_obj;
    }

    LYCancelDownload = FALSE;
    if (HTOutputFormat != HTAtom_for("www/download")) {
	if (traversal ||
	    (no_download && !override_no_download && no_disk_save)) {
	    if (!traversal) {
		HTAlert(CANNOT_DISPLAY_FILE);
	    }
	    LYCancelDownload = TRUE;
	    if (traversal)
		LYCancelledFetch = TRUE;
	    FREE(ret_obj);
	    return(NULL);
	}

	if (((cp = strchr(pres->rep->name, ';')) != NULL) &&
	    strstr((cp+1), "charset") != NULL) {
	    _user_message(MSG_DOWNLOAD_OR_CANCEL, pres->rep->name);
	} else if (*(pres->rep->name) != '\0')	{
	    _user_message(MSG_DOWNLOAD_OR_CANCEL, pres->rep->name);
	} else {
	    _statusline(CANNOT_DISPLAY_FILE_D_OR_C);
	}

	while (c != 'D' && c != 'C' && !LYCharIsINTERRUPT(c)) {
	    c = LYgetch_single();
#ifdef VMS
	    /*
	     *	'C'ancel on Control-C or Control-Y and
	     *	a 'N'o to the "really exit" query. - FM
	     */
	    if (HadVMSInterrupt) {
		HadVMSInterrupt = FALSE;
		c = 'C';
	    }
#endif /* VMS */
	}

	/*
	 *  Cancel on 'C', 'c' or Control-G or Control-C.
	 */
	if (c == 'C' || LYCharIsINTERRUPT(c)) {
	    _statusline(CANCELLING_FILE);
	    LYCancelDownload = TRUE;
	    FREE(ret_obj);
	    return(NULL);
	}
    }

    /*
     *	Set up a 'D'ownload.
     */
    if (LYCachedTemp(fnam, &(anchor->FileCache))) {
	/*  This used to be LYNewBinFile(fnam); changed to a different call
	 *  so that the open fp gets registered in the list keeping track of
	 *  temp files, equivalent to when LYOpenTemp() gets called below.
	 *  This avoids a file descriptor leak caused by LYCloseTempFP()
	 *  not being able to find the fp.  The ".bin" suffix is expected
	 *  to not be used, it's only for fallback in unusual error cases. - kw
	 */
	ret_obj->fp = LYOpenTempRewrite(fnam, ".bin", "wb");
    } else {
	/*
	 *  Check for a suffix.
	 *  Save the file under a suitably suffixed name.
	 */
	if (!strcasecomp(pres->rep->name, "text/html")) {
	    suffix = HTML_SUFFIX;
	} else if (!strcasecomp(pres->rep->name, "text/plain")) {
	    suffix = ".txt";
	} else if (!strcasecomp(pres->rep->name,
				    "application/octet-stream")) {
	    suffix = ".bin";
	} else if ((suffix = HTFileSuffix(pres->rep,
					  anchor->content_encoding)) == 0
		    || *suffix != '.') {
	    suffix = HTML_SUFFIX;
	}
	ret_obj->fp = LYOpenTemp(fnam, suffix, "wb");
    }

    if (!ret_obj->fp) {
	HTAlert(CANNOT_OPEN_OUTPUT);
	FREE(ret_obj);
	return NULL;
    }

    if (0==strncasecomp(pres->rep->name, "text/", 5) ||
	0==strcasecomp(pres->rep->name, "application/postscript") ||
	0==strcasecomp(pres->rep->name, "application/x-RUNOFF-MANUAL"))
	/*
	 *  It's a text file requested via 'd'ownload.
	 *  Keep adding others to the above list, 'til
	 *  we add a configurable procedure. - FM
	 */
	IsBinary = FALSE;

    /*
     *	Any "application/foo" or other non-"text/foo" types that
     *	are actually text but not checked, above, will be treated
     *	as binary, so show the type to help sort that out later.
     *	Unix folks don't need to know this, but we'll show it to
     *	them, too. - FM
     */
    HTUserMsg2(CONTENT_TYPE_MSG, pres->rep->name);

    StrAllocCopy(WWW_Download_File,fnam);

    /*
     *	Make command to delete file.
     */
    ret_obj->remove_command = 0;
    HTAddParam(&(ret_obj->remove_command), REMOVE_COMMAND, 1, fnam);
    HTEndParam(&(ret_obj->remove_command), REMOVE_COMMAND, 1);

#ifdef VMS
    if (IsBinary && UseFixedRecords) {
	StrAllocCopy(ret_obj->end_command, "SaveVMSBinaryFile");
	FIXED_RECORD_COMMAND = 0;
	HTAddParam(&FIXED_RECORD_COMMAND, FIXED_RECORD_COMMAND_MASK, 1, fnam);
	HTEndParam(&FIXED_RECORD_COMMAND, FIXED_RECORD_COMMAND_MASK, 1);

    } else {
#endif /* VMS */
    StrAllocCopy(ret_obj->end_command, "SaveToFile");
#ifdef VMS
    }
#endif /* VMS */

    _statusline(RETRIEVING_FILE);

    StrAllocCopy(anchor->FileCache, fnam);
Prepend_BASE:
    if (LYPrependBaseToSource &&
	!strncasecomp(pres->rep->name, "text/html", 9) &&
	!anchor->content_encoding) {
	/*
	 *  Add the document's base as a BASE tag at the top of the file,
	 *  so that any partial or relative URLs within it will be resolved
	 *  relative to that if no BASE tag is present and replaces it.
	 *  Note that the markup will be technically invalid if a DOCTYPE
	 *  declaration, or HTML or HEAD tags, are present, and thus the
	 *  file may need editing for perfection. - FM
	 *
	 *  Add timestamp (last reload).
	 */
	char *temp = NULL;

	if (anchor->content_base && *anchor->content_base) {
	    StrAllocCopy(temp, anchor->content_base);
	} else if (anchor->content_location && *anchor->content_location) {
	    StrAllocCopy(temp, anchor->content_location);
	}
	if (temp) {
	    LYRemoveBlanks(temp);
	    if (!is_url(temp)) {
		FREE(temp);
	    }
	}

	fprintf(ret_obj->fp,
		"<!-- X-URL: %s -->\n", anchor->address);
	if (anchor->date && *anchor->date) {
	    fprintf(ret_obj->fp,
		"<!-- Date: %s -->\n", anchor->date);
	    if (anchor->last_modified && *anchor->last_modified
			&& strcmp(anchor->last_modified, anchor->date)
			&& strcmp(anchor->last_modified, "Thu, 01 Jan 1970 00:00:01 GMT")) {
		fprintf(ret_obj->fp,
		    "<!-- Last-Modified: %s -->\n", anchor->last_modified);
	    }
	}
	fprintf(ret_obj->fp,
		"<BASE HREF=\"%s\">\n\n", (temp ? temp : anchor->address));
	FREE(temp);
    }
    if (LYPrependCharsetToSource &&
	!strncasecomp(pres->rep->name, "text/html", 9) &&
	!anchor->content_encoding) {
	/*
	 *  Add the document's charset as a META CHARSET tag
	 *  at the top of the file, so HTTP charset header
	 *  will not be forgotten when a document saved as local file.
	 *  We add this line only(!) if HTTP charset present. - LP
	 *  Note that the markup will be technically invalid if a DOCTYPE
	 *  declaration, or HTML or HEAD tags, are present, and thus the
	 *  file may need editing for perfection. - FM
	 */
	char *temp = NULL;

	if (anchor->charset && *anchor->charset) {
	    StrAllocCopy(temp, anchor->charset);
	    LYRemoveBlanks(temp);
	    fprintf(ret_obj->fp,
		"<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=%s\">\n\n",
		temp);
	}
	FREE(temp);
    }
    return ret_obj;
}

/*	Set up stream for uncompressing - FM
**	-------------------------------
*/
PUBLIC HTStream* HTCompressed ARGS3(
	HTPresentation *,	pres,
	HTParentAnchor *,	anchor,
	HTStream *,		sink)
{
    HTStream* me;
    HTFormat format;
    char *type = NULL;
    HTPresentation *Pres = NULL;
    int n, i;
    BOOL can_present = FALSE;
    char fnam[LY_MAXPATH];
    char temp[LY_MAXPATH];	/* actually stores just a suffix */
    CONST char *suffix;
    char *uncompress_mask = NULL;
    char *compress_suffix = "";
    CONST char *middle;

    /*
     *	Deal with any inappropriate invocations of this function,
     *	or a download request, in which case we won't bother to
     *	uncompress the file. - FM
     */
    CTRACE((tfp, "FIXME %s @%d\n", __FILE__, __LINE__));
    if (!(anchor && anchor->content_encoding && anchor->content_type)) {
	/*
	 *  We have no idea what we're dealing with,
	 *  so treat it as a binary stream. - FM
	 */
	format = HTAtom_for("application/octet-stream");
	me = HTStreamStack(format, pres->rep_out, sink, anchor);
	return me;
    }
    n = HTList_count(HTPresentations);
    CTRACE((tfp, "FIXME %s @%d\n", __FILE__, __LINE__));
    for (i = 0; i < n; i++) {
	Pres = (HTPresentation *)HTList_objectAt(HTPresentations, i);
	CTRACE((tfp, "FIXME %s @%d '%s', '%s' (%s)\n", __FILE__, __LINE__,
	    Pres->rep->name,
	    anchor->content_type,
	    anchor->content_encoding));
	if (!strcasecomp(Pres->rep->name, anchor->content_type) &&
	    Pres->rep_out == WWW_PRESENT) {
    CTRACE((tfp, "FIXME %s @%d\n", __FILE__, __LINE__));
	    /*
	     *	We have a presentation mapping for it. - FM
	     */
	    can_present = TRUE;
	    if (!strcasecomp(anchor->content_encoding, "x-gzip") ||
		!strcasecomp(anchor->content_encoding, "gzip")) {
		/*
		 *  It's compressed with the modern gzip. - FM
		 */
		StrAllocCopy(uncompress_mask, GZIP_PATH);
		StrAllocCat(uncompress_mask, " -d --no-name %s");
		compress_suffix = "gz";
#ifdef BZIP2_PATH
	    } else if (!strcasecomp(anchor->content_encoding, "x-bzip2") ||
		!strcasecomp(anchor->content_encoding, "bzip2")) {
		StrAllocCopy(uncompress_mask, BZIP2_PATH);
		StrAllocCat(uncompress_mask, " -d %s");
		compress_suffix = "bz2";
#endif /* BZIP2_PATH */
	    } else if (!strcasecomp(anchor->content_encoding, "x-compress") ||
		       !strcasecomp(anchor->content_encoding, "compress")) {
		/*
		 *  It's compressed the old fashioned Unix way. - FM
		 */
		StrAllocCopy(uncompress_mask, UNCOMPRESS_PATH);
		StrAllocCat(uncompress_mask, " %s");
		compress_suffix = "Z";
	    }
	    break;
	}
    }
    if (can_present == FALSE || 		 /* no presentation mapping */
	uncompress_mask == NULL ||		    /* not gzip or compress */
	strchr(anchor->content_type, ';') ||		   /* wrong charset */
	HTOutputFormat == HTAtom_for("www/download") || 	/* download */
	!strcasecomp(pres->rep_out->name, "www/download") ||	/* download */
	(traversal &&	   /* only handle html or plain text for traversals */
	 strcasecomp(anchor->content_type, "text/html") &&
	 strcasecomp(anchor->content_type, "text/plain"))) {
	/*
	 *  Cast the Content-Encoding to a Content-Type
	 *  and pass it back to be handled as that type. - FM
	 */
	if (strchr(anchor->content_encoding, '/') == NULL) {
	    /*
	     *  Use "x-" prefix, none of the types we are likely to
	     *  construct here are official.  That is we generate
	     *  "application/x-gzip" and so on. - kw
	     */
	    if (!strncasecomp(anchor->content_encoding, "x-", 2))
		StrAllocCopy(type, "application/");
	    else
		StrAllocCopy(type, "application/x-");
	    StrAllocCat(type, anchor->content_encoding);
	} else {
	    StrAllocCopy(type, anchor->content_encoding);
	}
	format = HTAtom_for(type);
	FREE(type)
	FREE(uncompress_mask);
	me = HTStreamStack(format, pres->rep_out, sink, anchor);
	return me;
    }

    /*
     *	Set up the stream structure for uncompressing and then
     *	handling based on the uncompressed Content-Type.- FM
     */
    me = typecalloc(HTStream);
    if (me == NULL)
	outofmem(__FILE__, "HTCompressed");
    me->isa = &HTFWriter;
    me->input_format = pres->rep;
    me->output_format = pres->rep_out;
    me->anchor = anchor;
    me->sink = sink;
#ifdef FNAMES_8_3
    me->idash = FALSE;
#endif

    /*
     *	Remove any old versions of the file. - FM
     */
    if (anchor->FileCache) {
	LYRemoveTemp(anchor->FileCache);
	FREE(anchor->FileCache);
    }

    /*
     *	Get a new temporary filename and substitute a suitable suffix. - FM
     */
    middle = NULL;
    if (!strcasecomp(anchor->content_type, "text/html")) {
	middle = HTML_SUFFIX;
	middle++;		/* point to 'h' of .htm(l) - kw */
    } else if (!strcasecomp(anchor->content_type, "text/plain")) {
	middle = "txt";
    } else if (!strcasecomp(anchor->content_type,
			    "application/octet-stream")) {
	middle = "bin";
    } else if ((suffix =
		HTFileSuffix(HTAtom_for(anchor->content_type), NULL)) &&
	       *suffix == '.') {
#if defined(VMS) || defined(FNAMES_8_3)
	if (strchr(suffix + 1, '.') == NULL)
#endif
	    middle = suffix + 1;
    }

    temp[0] = 0;		/* construct the suffix */
    if (middle) {
#ifdef FNAMES_8_3
	me->idash = TRUE;	/* remember position of '-'  - kw */
	strcat(temp, "-");	/* NAME-htm,  NAME-txt, etc. - hack for DOS */
#else
	strcat(temp, ".");	/* NAME.html, NAME-txt etc. */
#endif /* FNAMES_8_3 */
	strcat(temp, middle);
#ifdef VMS
	strcat(temp, "-");	/* NAME.html-gz, NAME.txt-gz, NAME.txt-Z etc.*/
#else
	strcat(temp, ".");	/* NAME-htm.gz (DOS), NAME.html.gz (UNIX)etc.*/
#endif /* VMS */
    }
    strcat(temp, compress_suffix);

    /*
     *	Open the file for receiving the compressed input stream. - FM
     */
    me->fp = LYOpenTemp (fnam, temp, "wb");
    if (!me->fp) {
	HTAlert(CANNOT_OPEN_TEMP);
	FREE(uncompress_mask);
	FREE(me);
	return NULL;
    }

    /*
     *	me->viewer_command will be NULL if the converter Pres found above
     *	is not for an external viewer but an internal HTStream converter.
     *	We also don't set it under conditions where HTSaveAndExecute would
     *	disallow execution of the command. - KW
     */
    if (!dump_output_immediately && !traversal
#if defined(EXEC_LINKS) || defined(EXEC_SCRIPTS)
	&& (Pres->quality != 999.0 ||
	    (!no_exec &&	/* allowed exec link or script ? */
	     (local_exec ||
	      (local_exec_on_local_files &&
	       (LYJumpFileURL ||
		!strncmp(anchor->address,"file://localhost",16))))))
#endif /* EXEC_LINKS || EXEC_SCRIPTS */
	) {
	StrAllocCopy(me->viewer_command, Pres->command);
    }

    /*
     *	Make command to process file. - FM
     */
#ifdef USE_ZLIB
    if (compress_suffix[0] == 'g' && /* must be gzip */
	!me->viewer_command) {
	/*
	 *  We won't call gzip externally, so we don't need to supply
	 *  a command for it. - kw
	 */
	StrAllocCopy(me->end_command, "");
    } else
#endif /* USE_ZLIB */
    {
	me->end_command = 0;
	HTAddParam(&(me->end_command), uncompress_mask, 1, fnam);
	HTEndParam(&(me->end_command), uncompress_mask, 1);
    }
    FREE(uncompress_mask);

    /*
     *	Make command to delete file. - FM
     */
    me->remove_command = 0;
    HTAddParam(&(me->remove_command), REMOVE_COMMAND, 1, fnam);
    HTEndParam(&(me->remove_command), REMOVE_COMMAND, 1);

    /*
     *	Save the filename and return the structure. - FM
     */
    StrAllocCopy(anchor->FileCache, fnam);
    return me;
}

/*	Dump output to stdout - LJM & FM
**	---------------------
**
*/
PUBLIC HTStream* HTDumpToStdout ARGS3(
	HTPresentation *,	pres GCC_UNUSED,
	HTParentAnchor *,	anchor,
	HTStream *,		sink GCC_UNUSED)
{
    HTStream * ret_obj;
    ret_obj = typecalloc(HTStream);
    if (ret_obj == NULL)
	outofmem(__FILE__, "HTDumpToStdout");
    ret_obj->isa = &HTFWriter;
    ret_obj->remove_command = NULL;
    ret_obj->end_command = NULL;
    ret_obj->anchor = anchor;

    ret_obj->fp = stdout; /* stdout*/
    return ret_obj;
}

#if defined(VMS) && !defined(USE_COMMAND_FILE)
#include <fab.h>
#include <rmsdef.h>		/* RMS status codes */
#include <iodef.h>		/* I/O function codes */
#include <fibdef.h>		/* file information block defs */
#include <atrdef.h>		/* attribute request codes */
#ifdef NOTDEFINED /*** Not all versions of VMS compilers have these.	 ***/
#include <fchdef.h>		/* file characteristics */
#include <fatdef.h>		/* file attribute defs */
#else		  /*** So we'll define what we need from them ourselves. ***/
#define FCH$V_CONTIGB	0x005			/* pos of cont best try bit */
#define FCH$M_CONTIGB	(1 << FCH$V_CONTIGB)	/* contig best try bit mask */
/* VMS I/O User's Reference Manual: Part I (V5.x doc set) */
struct fatdef {
    unsigned char	fat$b_rtype,	fat$b_rattrib;
    unsigned short	fat$w_rsize;
    unsigned long	fat$l_hiblk,	fat$l_efblk;
    unsigned short	fat$w_ffbyte;
    unsigned char	fat$b_bktsize,	fat$b_vfcsize;
    unsigned short	fat$w_maxrec,	fat$w_defext,	fat$w_gbc;
    unsigned	: 16, : 32, : 16;   /* 6 bytes reserved, 2 bytes not used */
    unsigned short	fat$w_versions;
};
#endif /* NOTDEFINED */

/* arbitrary descriptor without type and class info */
typedef struct dsc { unsigned short len, mbz; void *adr; } Desc;

extern unsigned long	sys$open(),  sys$qiow(),  sys$dassgn();

#define syswork(sts)	((sts) & 1)
#define sysfail(sts)	(!syswork(sts))


/*
 *  25-Jul-1995 - Pat Rankin (rankin@eql.caltech.edu)
 *
 *  Force a file to be marked as having fixed-length, 512 byte records
 *  without implied carriage control, and with best_try_contiguous set.
 */
PUBLIC unsigned long LYVMS_FixedLengthRecords ARGS1(char *, filename)
{
    struct FAB	    fab;		/* RMS file access block */
    struct fibdef   fib;		/* XQP file information block */
    struct fatdef   recattr;		/* XQP file "record" attributes */
    struct atrdef   attr_rqst_list[3];	/* XQP attribute request itemlist */

    Desc	    fib_dsc;
    unsigned short  channel,  iosb[4];
    unsigned long   fchars,  sts,  tmp;

    /* initialize file access block */
    fab = cc$rms_fab;
    fab.fab$l_fna = filename;
    fab.fab$b_fns = (unsigned char) strlen(filename);
    fab.fab$l_fop = FAB$M_UFO;	/* user file open; no further RMS processing */
    fab.fab$b_fac = FAB$M_PUT;	/* need write access */
    fab.fab$b_shr = FAB$M_NIL;	/* exclusive access */

    sts = sys$open(&fab);	/* channel in stv; $dassgn to close */
    if (sts == RMS$_FLK) {
	/* For MultiNet, at least, if the file was just written by a remote
	   NFS client, the local NFS server might still have it open, and the
	   failed access attempt will provoke it to be closed, so try again. */
	sts = sys$open(&fab);
    }
    if (sysfail(sts)) return sts;

    /* RMS supplies a user-mode channel (see FAB$L_FOP FAB$V_UFO doc) */
    channel = (unsigned short) fab.fab$l_stv;

    /* set up ACP interface structures */
    /* file information block, passed by descriptor; it's okay to start with
       an empty FIB after RMS has accessed the file for us */
    fib_dsc.len = sizeof fib;
    fib_dsc.mbz = 0;
    fib_dsc.adr = &fib;
    memset((void *)&fib, 0, sizeof fib);
    /* attribute request list */
    attr_rqst_list[0].atr$w_size = sizeof recattr;
    attr_rqst_list[0].atr$w_type = ATR$C_RECATTR;
    *(void **)&attr_rqst_list[0].atr$l_addr = (void *)&recattr;
    attr_rqst_list[1].atr$w_size = sizeof fchars;
    attr_rqst_list[1].atr$w_type = ATR$C_UCHAR;
    *(void **)&attr_rqst_list[1].atr$l_addr = (void *)&fchars;
    attr_rqst_list[2].atr$w_size = attr_rqst_list[2].atr$w_type = 0;
    attr_rqst_list[2].atr$l_addr = 0;
    /* file "record" attributes */
    memset((void *)&recattr, 0, sizeof recattr);
    fchars = 0; 	/* file characteristics */

    /* get current attributes */
    sts = sys$qiow(0, channel, IO$_ACCESS, iosb, (void(*)())0, 0,
		   &fib_dsc, 0, 0, 0, attr_rqst_list, 0);
    if (syswork(sts))
	sts = iosb[0];

    /* set desired attributes */
    if (syswork(sts)) {
	recattr.fat$b_rtype = FAB$C_SEQ | FAB$C_FIX;	/* org=seq, rfm=fix */
	recattr.fat$w_rsize = recattr.fat$w_maxrec = 512;   /* lrl=mrs=512 */
	recattr.fat$b_rattrib = 0;			/* rat=none */
	fchars |= FCH$M_CONTIGB;		/* contiguous-best-try */
	sts = sys$qiow(0, channel, IO$_DEACCESS, iosb, (void(*)())0, 0,
		       &fib_dsc, 0, 0, 0, attr_rqst_list, 0);
	if (syswork(sts))
	    sts = iosb[0];
    }

    /* all done */
    tmp = sys$dassgn(channel);
    if (syswork(sts))
	sts = tmp;
    return sts;
}
#endif /* VMS && !USE_COMMAND_FILE */
