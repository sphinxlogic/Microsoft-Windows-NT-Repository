/*
 *   This file was automatically generated by cextract version 1.2.
 *   Manual editing now recommended, since I've done a whole lot of it.
 *
 *   Created: Thu May 14 15:44:40 1992
 *
 * $Header: /usr2/foxharp/src/pgf/vile/RCS/proto.h,v 1.207 1995/10/01 14:44:59 pgf Exp $
 *
 */

extern int main P(( int, char *[] ));
extern char * strmalloc P(( char * ));
extern void tidy_exit P(( int ));
extern int no_memory P(( char * ));
extern SIGT catchintr (DEFINE_SIG_ARGS);
#ifndef interrupted
extern int interrupted P(( void ));
#endif
extern void not_interrupted P(( void ));
extern void do_repeats P(( int *, int *, int * ));
extern int writeall P(( int, int, int, int, int ));
extern int rdonly P(( void ));
extern void charinit P(( void ));
#if OPT_RAMSIZE
extern char *reallocate P(( char *, unsigned ));
extern char *allocate P(( unsigned ));
extern void release P(( char * ));
#endif
#if OPT_MAP_MEMORY
extern void exit_program P(( int ));
#endif
extern char *strncpy0 P(( char *, char *, SIZE_T ));
extern void setup_handler P(( int, void (*) (DEFINE_SIG_ARGS) ));

/* screen-drivers */
#if OPT_XTERM >= 3
extern int xterm_mouse_t P(( int, int ));
extern int xterm_mouse_T P(( int, int ));
#endif
#if OPT_EVAL || OPT_COLOR
extern void spal P(( char * ));
#endif


/* basic.c */
extern int firstchar P(( LINE * ));
extern int nextchar P(( LINE *, int ));
extern int lastchar P(( LINE * ));
extern int getgoal P(( LINE * ));
extern int next_column P(( int, int ));
extern int gonmmark P(( int ));
extern int setmark P(( void ));
extern void swapmark P(( void ));
#if OPT_MOUSE
extern	int setwmark P(( int, int ));
extern	int setcursor P(( int, int ));
#endif
#if SMALLER	/* cancel 'neproto.h' */
extern int gotobob P(( int, int ));
extern int gotoeob P(( int, int ));
#endif

/* bind.c */
extern int startup P(( char *));
extern char * flook P(( char *, int ));
extern char * kcod2pstr P(( int, char * ));
extern int kcod2escape_seq P(( int, char * ));
#if DISP_X11
extern int insertion_cmd P(( int ));
#endif
extern int fnc2kcod P(( CMDFUNC * ));
#if DISP_X11
extern char * fnc2pstr P(( CMDFUNC * ));
#endif
extern CMDFUNC * engl2fnc P(( char * ));
extern CMDFUNC * kcod2fnc P(( int ));
#if OPT_EVAL
extern char * prc2engl P(( char * ));
#endif
extern char * kbd_engl P(( char *, char * ));
extern void kbd_alarm P(( void ));
extern void kbd_putc P(( int ));
extern void kbd_puts P(( char * ));
extern void kbd_erase P(( void ));
extern void kbd_init P(( void ));
extern void kbd_unquery P(( void ));
extern int kbd_complete P(( int, int, char *, int *, char *, SIZE_T ));
extern int kbd_engl_stat P(( char *, char * ));
extern void popdown_completions P(( void ));

/* buffer.c */
extern WINDOW *bp2any_wp P(( BUFFER * ));
extern void imply_alt P(( char *, int, int ));
extern BUFFER *find_b_file P(( char * ));
extern BUFFER * find_alt P(( void ));
extern BUFFER * find_bp P(( BUFFER * ));
extern void make_current P(( BUFFER * ));
extern int swbuffer P(( BUFFER * ));
extern void undispbuff P(( BUFFER *, WINDOW * ));
extern int tabstop_val P(( BUFFER * ));
extern int shiftwid_val P(( BUFFER * ));
extern int has_C_suffix P(( BUFFER * ));
extern void delink_bp P(( BUFFER * ));
extern int zotbuf P(( BUFFER * ));
extern int zotwp P(( BUFFER * ));
extern BUFFER *find_any_buffer P(( char * ));
extern int popupbuff P(( BUFFER * ));
extern void sortlistbuffers P(( void ));
#if OPT_UPBUFF
void updatelistbuffers P((void));
void update_scratch P(( char *, int (*func)(BUFFER *) ));
#else
#define updatelistbuffers()
#define update_scratch(name, func)
#endif
extern int addline P(( BUFFER *, char *, int ));
extern int add_line_at P(( BUFFER *, LINEPTR, char *, int ));
extern int any_changed_buf P(( BUFFER ** ));
extern int any_unread_buf P(( BUFFER ** ));
extern void set_bname P(( BUFFER *, char * ));
extern char * get_bname P(( BUFFER * ));
extern BUFFER * find_b_name P(( char * ));
extern BUFFER * bfind P(( char *, int ));
BUFFER * make_bp P(( char *, int, int ));
extern int bclear P(( BUFFER * ));
extern int bsizes P(( BUFFER * ));
extern void chg_buff P(( BUFFER *, int ));
extern void unchg_buff P(( BUFFER *, int ));
extern BUFFER *getfile2bp P(( char *, int ));

/* crypt.c */
#if	OPT_ENCRYPT
extern	int	ue_makekey P((char *, int));
extern	void	ue_crypt P((char *, int));
#endif	/* OPT_ENCRYPT */

/* csrch.c */

/* display.c */
extern int nu_width P(( WINDOW * ));
extern int col_limit P(( WINDOW * ));
extern int vtinit P(( void ));
extern int video_alloc P(( VIDEO ** ));
extern void vttidy P(( int ));
extern int update P(( int ));
extern void upmode P(( void ));
extern int offs2col P(( WINDOW *, LINEPTR, C_NUM ));
#if OPT_MOUSE
extern int col2offs P(( WINDOW *, LINEPTR, C_NUM ));
#endif
#ifdef WMDLINEWRAP
extern int line_height P(( WINDOW *, LINEPTR ));
#else
#define line_height(wp,lp) 1
#endif
#if defined(WMDLINEWRAP) || OPT_MOUSE
extern WINDOW *row2window P(( int ));
#endif
extern void hilite P((int, int, int, int));
extern void movecursor P(( int, int ));
extern void bottomleft P(( void ));
extern void mlerase P(( void ));
extern void mlsavec P(( int ));
extern void mlwrite P((char *, ... ));
extern void mlforce P((char *, ... ));
extern void mlprompt P((char *, ... ));
extern void mlerror P(( char * ));
extern void mlwarn P(( char *, ... ));
extern void dbgwrite P((char *, ... ));
extern char * lsprintf P((char *, char *, ... ));
extern void bputc P(( int ));
extern void bprintf P((char *, ... ));
#if !DISP_X11
extern void getscreensize P(( int *, int * ));
#if defined(SIGWINCH)
extern SIGT sizesignal (DEFINE_SIG_ARGS);
#endif
#endif
extern void newscreensize P(( int, int ));
#if OPT_WORKING
extern SIGT imworking (DEFINE_SIG_ARGS);
#endif
#if OPT_PSCREEN
extern	void	psc_putchar	P(( int ));
extern	void	psc_flush	P(( void ));
extern	void	psc_move	P(( int, int ));
extern	void	psc_eeol	P(( void ));
extern	void	psc_eeop	P(( void ));
extern	void	psc_rev		P(( int ));
#endif	/* OPT_PSCREEN */

/* eval.c */
extern char * l_itoa P(( int ));
extern int absol P(( int ));
extern int is_truem P(( char * ));
extern int is_falsem P(( char * ));
#if OPT_EVAL || DISP_X11
extern int stol P(( char * ));
#endif
#if OPT_EVAL
extern char * gtenv P(( char * ));
#endif
#if OPT_EVAL || !SMALLER
extern char * mkupper P(( char * ));
#endif
extern char * mklower P(( char * ));
extern char * mktrimmed P(( char * ));
#if OPT_EVAL
extern int set_variable P(( char * ));
#endif

/* exec.c */
extern int end_named_cmd P(( void ));
extern int more_named_cmd P(( void ));
extern int dobuf P(( BUFFER * ));
extern int dofile P(( char * ));
extern int execute P(( CMDFUNC *, int, int ));
#if OPT_PROCEDURES
extern int run_procedure P(( char * ));
#endif

/* file.c */
extern time_t file_modified P(( char * ));
#ifdef MDCHK_MODTIME
extern int ask_shouldchange P(( BUFFER * ));
extern int get_modtime P(( BUFFER *, time_t * ));
extern void set_modtime P(( BUFFER *, char * ));
extern int check_modtime P(( BUFFER *, char * ));
extern int check_visible_modtimes P(( void ));
#endif
extern int no_such_file P(( char * ));
extern int same_fname P(( char *, BUFFER *, int ));
extern int getfile P(( char *, int ));
extern int readin P((char *, int, BUFFER *, int ));
extern int bp2readin P(( BUFFER *, int ));
extern int slowreadf P(( BUFFER *, int * ));
extern void makename P(( char *, char * ));
extern int unqname P((char *, int));
extern int writeout P(( char *, BUFFER *, int, int ));
extern int writeregion P(( void ));
extern int kwrite P(( char *, int ));
extern int ifile P(( char *, int, FILE * ));
extern SIGT imdying (DEFINE_SIG_ARGS);
extern void markWFMODE P(( BUFFER * ));
#if OPT_ENCRYPT
extern int resetkey P(( BUFFER *, char * ));
#endif
#if SMALLER	/* cancel neproto.h */
extern int filesave P(( int, int ));
#endif

/* filec.c */
extern int mlreply_file P(( char *, TBUFF **, int, char * ));
extern int mlreply_dir P(( char *, TBUFF **, char * ));
extern char *filec_expand P(( void ));

/* fileio.c */
extern int ffropen P(( char * ));
extern int ffwopen P(( char *, int ));
extern int ffaccess P(( char *, int ));
extern int ffronly P(( char * ));
extern off_t ffsize P(( void ));
extern int ffexists P(( char * ));
#if !(SYS_MSDOS || SYS_WIN31) && !OPT_MAP_MEMORY
extern int ffread P(( char *, long ));
extern void ffseek P(( long ));
extern void ffrewind P(( void ));
#endif
extern int ffclose P(( void ));
extern int ffputline P(( char [], int, char * ));
extern int ffputc P(( int ));
extern int ffhasdata P(( void ));

/* finderr.c */
#if OPT_FINDERR
extern void set_febuff P(( char * ));
#endif

/* glob.c */
#if !SYS_UNIX
extern	int	glob_needed P((char **));
#endif
extern	char **	glob_string P((char *));
extern	int	glob_length P((char **));
extern	char **	glob_free   P((char **));

#if !SYS_UNIX
extern	void	expand_wild_args P(( int * , char ***));
#endif

extern	int	doglob P(( char * ));

/* globals.c */

/* history.c */
#if OPT_HISTORY
extern void hst_init P(( int ));
extern void hst_glue P(( int ));
extern void hst_append P(( char *, int ));
extern void hst_remove P(( char * ));
extern void hst_flush P(( void ));
extern int edithistory P(( char *, int *, int *, int, int (*func)( char *, int, int, int ), int ));
#else
#define hst_init(c)
#define hst_glue(c)
#define hst_append(p,c)
#define hst_remove(p)
#define hst_flush()
#endif

/* input.c */
extern int no_completion P(( int, char *, int * ));
extern int mlyesno P(( char * ));
extern int mlquickask P(( char *, char *, int * ));
extern int mlreply P(( char *, char *, int ));
extern int mlreply_reg P(( char *, char *, int *, int ));
extern int mlreply_reg_count P(( int, int *, int * ));
extern int mlreply_no_bs P(( char *, char *, int ));
extern int mlreply_no_opts P(( char *, char *, int ));
extern void incr_dot_kregnum P(( void ));
extern int mapped_keystroke P(( void ));
extern int keystroke P(( void ));
extern int keystroke8 P(( void ));
extern int keystroke_raw8 P(( void ));
extern int keystroke_avail P(( void ));
extern void unkeystroke P(( int ));
extern int tgetc P(( int ));
extern int tgetc_avail P(( void ));
extern int get_recorded_char P(( int ));
extern int kbd_seq P(( void ));
extern int screen_string P(( char *, int, CHARTYPE ));
extern int end_string P(( void ));
extern void set_end_string P(( int ));
extern int kbd_delimiter P(( void ));
extern int is_edit_char P(( int ));
extern void kbd_kill_response P(( char *, int *, int ));
extern int kbd_show_response P(( char *, char *, int, int, int ));
extern int kbd_is_pushed_back P(( void ));
extern void kbd_pushback P(( char *, int ));
extern int kbd_string P(( char *, char *, int, int, int, int (*func)(int,char*,int*) ));
extern int kbd_reply P(( char *, char *, int, int (*efunc)(char *,int,int,int), int, int, int (*cfunc)(int,char*,int*) ));
extern int dotcmdbegin P(( void ));
extern int dotcmdfinish P(( void ));
extern void dotcmdstop P(( void ));
extern int kbd_replaying P(( int ));
extern int kbm_started P(( int, int ));
extern int start_kbm P(( int, int, ITBUFF * ));

/* insert.c */
extern int ins P(( void ));
extern int inschar P(( int, int * ));
extern int previndent P(( int * ));
extern int indentlen P(( LINE * ));
#if OPT_EVAL
extern char *current_modename P(( void ));
#endif
#if SMALLER	/* cancel 'neproto.h' */
extern int newline P(( int, int ));
extern int wrapword P(( int, int ));
#endif

/* isearch.c */
#if SMALLER	/* cancel 'neproto.h' */
extern int forwhunt P(( int, int ));
extern int backhunt P(( int, int ));
#endif

/* lckfiles.c */
#if OPT_LCKFILES
extern int set_lock P(( char *, char *, int));
extern void release_lock P(( char * ));
#endif

/* line.c */
extern int do_report P(( L_NUM ));
extern LINEPTR lalloc P(( int, BUFFER * ));
extern void lfree P(( LINEPTR, BUFFER * ));
#if !OPT_MAP_MEMORY
extern void ltextfree P(( LINE *, BUFFER * ));
#endif
extern void lremove P(( BUFFER *, LINEPTR ));
#if SMALLER	/* cancel neproto.h */
extern int insspace P(( int, int ));
#endif
extern int lstrinsert P(( char *, int ));
extern int linsert P(( int, int ));
extern int lnewline P(( void ));
extern int ldelete P(( long, int ));
#if OPT_EVAL
extern char * getctext P(( CHARTYPE ));
extern int putctext P(( char * ));
#endif
extern void ksetup P(( void ));
extern void kdone P(( void ));
extern int kinsertlater P(( int ));
extern int kinsert P(( int ));
extern int index2reg P(( int ));
extern int reg2index P(( int ));
extern int index2ukb P(( int ));
extern void kregcirculate P(( int ));

/* map.c */
extern void addtosysmap P(( char *, int, int ));
extern int sysmapped_c P(( void ));
extern int sysmapped_c_avail P(( void ));
extern void mapungetc P(( int ));
extern int mapped_c P(( int, int ));
extern int mapped_c_avail P(( void ));
extern void abbr_check P(( int * ));

/* msgs.c */
#if OPT_POPUP_MSGS
void msg_putc P((int c));
void popup_msgs P((void));
void purge_msgs P((void));
#endif

/* modes.c */
extern int string_to_number P(( char *, int * ));
extern int adjvalueset P(( char *, int, int, VALARGS * ));
extern char *string_mode_val P(( VALARGS * ));
extern REGEXVAL *new_regexval P(( char *, int ));
extern void copy_mvals P(( int, struct VAL *, struct VAL * ));
#if OPT_UPBUFF
extern void save_vals P(( int, struct VAL *, struct VAL *, struct VAL * ));
#endif
extern void free_local_vals P(( struct VALNAMES *, struct VAL *, struct VAL * ));
extern int find_mode P(( char *, int, VALARGS * ));
extern int mode_eol P(( char *, int, int, int ));

/* npopen.c */
#if SYS_UNIX || SYS_MSDOS || SYS_WIN31 || SYS_OS2 || SYS_WINNT
extern FILE * npopen P(( char *, char * ));
extern void npclose P(( FILE * ));
extern int inout_popen P(( FILE **, FILE **, char * ));
extern int softfork P(( void ));
#endif
#if SYS_UNIX || SYS_OS2 || SYS_WINNT
extern int system_SHELL P(( char * ));
#endif
#if SYS_MSDOS || SYS_WIN31 || (SYS_OS2 && CC_CSETPP)
extern void npflush P(( void ));
#endif

/* oneliner.c */
extern int llineregion P(( void ));
extern int plineregion P(( void ));
extern int substregion P(( void ));
extern int subst_again_region P(( void ));

/* opers.c */
extern int operator P(( int, int, int (*)(void), char * ));

/* path.c */
#if OPT_MSDOS_PATH
extern char * is_msdos_drive P(( char * ));
#endif
#if OPT_VMS_PATH
extern int is_vms_pathname P(( char *, int ));
extern char * vms_pathleaf P(( char * ));
extern char * unix_pathleaf P(( char * ));
#endif
#if SYS_UNIX
extern char * home_path P(( char * ));
#endif
extern char * pathleaf P(( char * ));
extern char * pathcat P(( char *, char *, char * ));
extern char * last_slash P(( char * ));
extern char * shorten_path P(( char *, int ));
extern char * lengthen_path P(( char * ));
extern int is_pathname P(( char * ));
extern int maybe_pathname P(( char * ));
extern char * is_appendname P(( char * ));
extern int is_internalname P(( char * ));
extern int is_directory P(( char * ));
#if (SYS_UNIX||SYS_VMS||OPT_MSDOS_PATH) && OPT_PATHLOOKUP
extern char *parse_pathlist P(( char *, char * ));
#endif
#if OPT_MSDOS_PATH
extern char *sl_to_bsl P(( char * ));
extern void bsl_to_sl_inplace P(( char * ));
#endif

/* random.c */
extern int line_report P(( L_NUM ));
extern L_NUM line_count P(( BUFFER * ));
extern L_NUM line_no P(( BUFFER *, LINEPTR ));
#if OPT_EVAL
extern L_NUM getcline P(( void ));
#endif
extern void set_rdonly P(( BUFFER *, char * ));
extern int liststuff P(( char *, int, void (*)(int, void *), int, void * ));
extern int getccol P(( int ));
extern int getcol P(( MARK, int ));
extern int getoff P(( C_NUM, C_NUM * ));
extern int gocol P(( int ));
extern int is_user_fence P(( int, int * ));
extern int fmatchindent P(( int ));
extern void catnap P(( int, int ));
extern char * current_directory P(( int ));
#if OPT_EVAL
extern char * previous_directory P(( void ));
#endif
extern int set_directory P(( char * ));
extern void ch_fname P(( BUFFER *, char * ));

/* regexp.c */
extern regexp * regcomp P(( char *, int ));
extern int regexec P(( regexp *, char *, char *, int, int ));
extern int lregexec P(( regexp *, LINE *, int, int ));

/* region.c */
extern int killregion P(( void ));
extern int killregionmaybesave P(( int ));
extern int openregion P(( void ));
extern int stringrect P(( void ));
extern int shiftrregion P(( void ));
extern int shiftlregion P(( void ));
extern int detabline P(( void *, int, int ));
extern int detab_region P(( void ));
extern int entabline P(( void *, int, int ));
extern int entab_region P(( void ));
extern int trimline P(( void *, int, int ));
extern int trim_region P(( void ));
extern int blank_region P(( void ));
extern int yankregion P(( void ));
extern int flipregion P(( void ));
extern int lowerregion P(( void ));
extern int upperregion P(( void ));
#if NEEDED
extern int charprocreg P(( int (*)(int) ));
#endif
extern int getregion P(( REGION * ));
extern int get_fl_region P(( REGION * ));

/* search.c */
extern int fsearch P(( int, int, int, int ));
extern int scanner P((regexp *, int, int, int * ));
extern void attrib_matches P(( void ));
extern void regerror P(( char * ));
#if OPT_EVAL || UNUSED
extern int eq P(( int, int ));
#endif
extern int readpattern P(( char *, char *, regexp **, int, int ));
extern void scanboundry P(( int, MARK, int ));
extern int findpat P(( int, int, regexp *, int ));

/* spawn.c */
extern SIGT rtfrmshell (DEFINE_SIG_ARGS);
extern void pressreturn P(( void ));
extern int filterregion P(( void ));

/* tags.c */
#if OPT_TAGS
extern int cmdlinetag P(( char * ));
#endif /* OPT_TAGS */

/* termio.c */
extern void ttopen P(( void ));
extern void ttclose P(( void ));
extern void ttclean P(( int ));
extern void ttunclean P(( void ));
extern void ttputc P(( int ));
extern void ttflush P(( void ));
extern int ttgetc P(( void ));
extern int tttypahead P(( void ));
extern void ttmiscinit P(( void ));
extern int open_terminal P(( TERM * ));

/* undo.c */
extern void toss_to_undo P(( LINEPTR ));
extern void copy_for_undo P(( LINEPTR ));
extern void tag_for_undo P(( LINEPTR ));
extern void nounmodifiable P(( BUFFER * ));
extern void freeundostacks P(( BUFFER *, int ));
extern void mayneedundo P(( void ));
extern void dumpuline P(( LINEPTR ));

/* version.c */
extern void print_usage P(( void ));
extern char *getversion P(( void ));
extern char * non_filename P(( void ));

/* vmspipe.c */
#if SYS_VMS
extern FILE *vms_rpipe (char *cmd, int fd, char *input_file);
#endif

/* window.c */
extern int set_curwp P(( WINDOW * ));
#if OPT_EVAL
extern int getwpos P(( void ));
#endif
extern int delwp P(( WINDOW * ));
extern void copy_traits P(( W_TRAITS *, W_TRAITS * ));
extern WINDOW * wpopup P(( void ));
extern void shrinkwrap P(( void ));
extern void winit P(( int ));
#if SMALLER	/* cancel neproto.h */
extern int reposition P(( int, int ));
extern int resize P(( int, int ));
#endif

/* word.c */
extern int joinregion P(( void ));
extern int formatregion P(( void ));

extern void setchartype P(( void ));
extern int isnewwordf P(( void ));
extern int isnewwordb P(( void ));
extern int isnewviwordf P(( void ));
extern int isnewviwordb P(( void ));
extern int isendwordf P(( void ));
extern int isendviwordf P(( void ));
extern int toktyp P(( char * ));
extern char * tokval P(( char * ));
extern char * token P(( char *, char *, int ));
extern int ffgetline P(( int * ));
extern int macroize P(( TBUFF **, char *, char * ));
extern int macarg P(( char * ));
extern int macliteralarg P(( char * ));
extern void fmatch P(( int ));

/* tbuff.c */
TBUFF *	tb_alloc P(( TBUFF **, ALLOC_T ));
TBUFF *	tb_init P(( TBUFF **, int ));
void	tb_free P(( TBUFF ** ));
void	tb_stuff P(( TBUFF *, int ));
int	tb_get P(( TBUFF *, ALLOC_T ));
void	tb_unput P(( TBUFF * ));
TBUFF *	tb_append P(( TBUFF **, int ));
TBUFF *	tb_copy P(( TBUFF **, TBUFF * ));
TBUFF *	tb_bappend P(( TBUFF **, char *, ALLOC_T ));
TBUFF *	tb_sappend P(( TBUFF **, char * ));
TBUFF *	tb_scopy P(( TBUFF **, char * ));
void	tb_first P(( TBUFF * ));
int	tb_more P(( TBUFF * ));
int	tb_next P(( TBUFF * ));
void	tb_unnext P(( TBUFF * ));
int	tb_peek P(( TBUFF * ));
char *	tb_values P(( TBUFF * ));
ALLOC_T	tb_length P(( TBUFF * ));

/* itbuff.c */
ITBUFF * itb_alloc P(( ITBUFF **, ALLOC_T ));
ITBUFF * itb_init P(( ITBUFF **, int ));
void	 itb_free P(( ITBUFF ** ));
void	 itb_stuff P(( ITBUFF *, int ));
int	 itb_get P(( ITBUFF *, ALLOC_T ));
#if NEEDED
void	 itb_unput P(( ITBUFF * ));
#endif
ITBUFF * itb_append P(( ITBUFF **, int ));
ITBUFF * itb_copy P(( ITBUFF **, ITBUFF * ));
ITBUFF * itb_bappend P(( ITBUFF **, char *, ALLOC_T ));
ITBUFF * itb_sappend P(( ITBUFF **, char * ));
#if NEEDED
void	 itb_delete P(( ITBUFF	*, ALLOC_T ));
ITBUFF * itb_insert P(( ITBUFF	**, int ));
#endif
void	 itb_first P(( ITBUFF * ));
int	 itb_more P(( ITBUFF * ));
int	 itb_next P(( ITBUFF * ));
int	 itb_last P(( ITBUFF * ));
#if NEEDED
void	 itb_unnext P(( ITBUFF * ));
#endif
	
int	 itb_peek P(( ITBUFF * ));
ALLOC_T	 itb_length P(( ITBUFF * ));

/* tmp.c */
#if OPT_MAP_MEMORY
extern	void	tmp_cleanup	P(( void ));
extern	LINEPTR	l_allocate	P(( SIZE_T ));
extern	void	l_deallocate	P(( LINEPTR ));
extern	LINE *	l_reallocate	P(( LINEPTR, SIZE_T, BUFFER * ));
extern	int	l_truncated	P(( void ));
extern	void	l_region	P(( REGION * ));
extern	LINE *	l_ref		P(( LINEPTR ));
extern	LINEPTR	l_ptr		P(( LINE * ));
extern	void	set_lforw	P(( LINE *, LINE * ));
extern	void	set_lback	P(( LINE *, LINE * ));
extern	LINE *	lforw		P(( LINE * ));
extern	LINE *	lback		P(( LINE * ));
extern	void	lsetclear	P(( LINE * ));
extern	LINE *	lforw_p2r	P(( LINEPTR ));
extern	LINE *	lback_p2r	P(( LINEPTR ));
extern	LINEPTR	lforw_p2p	P(( LINEPTR ));
extern	LINEPTR	lback_p2p	P(( LINEPTR ));
extern	void	set_lforw_p2r	P(( LINE *, LINEPTR ));
extern	void	set_lback_p2r	P(( LINE *, LINEPTR ));
extern	void	set_lforw_p2p	P(( LINEPTR, LINEPTR ));
extern	void	set_lback_p2p	P(( LINEPTR, LINEPTR ));

#endif

#if NO_LEAKS
extern	void bind_leaks P(( void ));
extern	void onel_leaks P(( void ));
extern	void path_leaks P(( void ));
extern	void kbs_leaks P(( void ));
extern	void map_leaks P(( void ));
extern	void tmp_leaks P(( void ));
extern	void itb_leaks P(( void ));
extern	void tb_leaks P(( void ));
extern	void wp_leaks P(( void ));
extern	void bp_leaks P(( void ));
extern	void vt_leaks P(( void ));
extern	void ev_leaks P(( void ));
#endif

#if DISP_X11
#if XTOOLKIT
extern	void	own_selection		P(( void ));
extern	void	update_scrollbar	P(( WINDOW *uwp ));
#else	/* !XTOOLKIT */
extern	void	x_set_rv		P(( void ));
extern	void	x_setname		P(( char * ));
extern	void	x_set_wm_title		P(( char * ));
extern	void	x_setforeground		P(( char * ));
extern	void	x_setbackground		P(( char * ));
extern  void	x_set_geometry		P(( char * ));
extern	void	x_set_dpy		P(( char * ));
extern	int	x_key_events_ready	P(( void ));
#endif	/* !XTOOKIT */
extern	int	x_setfont		P(( char * ));
extern	char *	x_current_fontname	P(( void ));
extern	void	x_preparse_args		P(( int *, char *** ));
extern	void	x_putline		P(( int, char *, int ));
extern	int	x_typahead		P(( int ));
extern	int	x_on_msgline		P((void));
extern	void	x_move_events		P(( void ));
#if OPT_WORKING
extern	void	x_working		P(( void ));
#endif
#if NO_LEAKS
extern	void	x11_leaks		P(( void ));
#endif
extern void x_set_icon_name P(( char * ));
extern char * x_get_icon_name P(( void ));
extern void x_set_window_name P(( char * ));
extern char * x_get_window_name P(( void ));
extern	void	x_resize		P(( int cols, int rows ));
#endif	/* DISP_X11 */

#if SYS_MSDOS || SYS_OS2 || SYS_WINNT
/* ibmpc.c */
extern	void scwrite P(( int, int, int, char *, VIDEO_ATTR *, int, int ));
extern VIDEO *scread P(( VIDEO *, int ));
/* random.c */
extern char * curr_dir_on_drive P(( int ));
extern int curdrive P(( void ));
extern int setdrive P(( int ));
extern void update_dos_drv_dir P(( char * ));
# if CC_WATCOM
     extern int dos_crit_handler P(( unsigned, unsigned, unsigned *));
# else
     extern void dos_crit_handler P(( void ));
# endif
# if OPT_MS_MOUSE
     extern int ms_exists P(( void ));
     extern void ms_processing P(( void ));
# endif
#endif

#if SYS_UNIX
#if MISSING_EXTERN__FILBUF
extern	int	_filbuf	P(( FILE * ));
#endif
#if MISSING_EXTERN__FLSBUF
extern	int	_flsbuf	P(( int, FILE * ));
#endif
#if MISSING_EXTERN_ACCESS
extern	int	access	P(( char *, int ));
#endif
#if MISSING_EXTERN_ALARM
extern	UINT	alarm	P(( UINT ));
#endif
#if MISSING_EXTERN_ATOI
extern int	atoi	P((char *));
#endif
#if MISSING_EXTERN_BZERO
extern	void	bzero	P(( char *, int ));
#endif
#if MISSING_EXTERN_CHDIR
extern	int	chdir	P(( char * ));
#endif
#if MISSING_EXTERN_CLOSE
extern	int	close	P(( int ));
#endif
#if MISSING_EXTERN_DUP
extern	int	dup	P(( int ));
#endif
#if MISSING_EXTERN_EXECLP
extern	int	execlp	P(( char *, ... ));
#endif
#if MISSING_EXTERN_FCLOSE
extern	int	fclose	P(( FILE * ));
#endif
#if MISSING_EXTERN_FCLOSE
extern	int	fflush	P(( FILE * ));
#endif
#if MISSING_EXTERN_FGETC
extern	int	fgetc	P(( FILE * ));
#endif
#if !defined(fileno) && MISSING_EXTERN_FILENO
extern	int	fileno	P(( FILE * ));
#endif
#if MISSING_EXTERN_FORK
extern	int	fork	P(( void ));
#endif
#if MISSING_EXTERN_FPRINTF
extern	int	fprintf	P(( FILE *, const char *, ... ));
#endif
#if MISSING_EXTERN_FPUTC
extern	int	fputc	P(( int, FILE * ));
#endif
#if MISSING_EXTERN_FPUTS
extern	int	fputs	P(( const char *, FILE * ));
#endif
#if MISSING_EXTERN_FPUTS
extern	int	fputs	P(( const char *, FILE * ));
#endif
#if MISSING_EXTERN_FREAD
extern	int	fread	P(( char *, int, int, FILE * ));
#endif
#if MISSING_EXTERN_FREE
extern void	free	P((void *ptr));
#endif
#if MISSING_EXTERN_FSEEK
extern	int	fseek	P(( FILE *, long, int ));
#endif
#if MISSING_EXTERN_FWRITE
extern	int	fwrite	P(( const char *, SIZE_T, SIZE_T, FILE * ));
#endif
#if MISSING_EXTERN_GETENV
extern	char *	getenv	P(( const char * ));
#endif
#if HAVE_GETHOSTNAME && MISSING_EXTERN_GETHOSTNAME
extern	int	gethostname P((char *, int));
#endif
#if MISSING_EXTERN_GETPGRP
extern	int	getpgrp	P(( int ));
#endif
#if MISSING_EXTERN_GETPID
extern	int	getpid	P(( void ));
#endif
#if MISSING_EXTERN_GETUID
extern	int	getuid	P(( void ));
#endif
#if HAVE_GETCWD && MISSING_EXTERN_GETCWD
extern char *getcwd P(( char *, int ));
#endif
#if HAVE_GETWD && MISSING_EXTERN_GETWD
extern	char *	getwd	P(( char * ));
#endif
#if MISSING_EXTERN_IOCTL
extern	int	ioctl	P(( int, ULONG, caddr_t ));
#endif
#if MISSING_EXTERN_ISATTY
extern	int	isatty	P(( int ));
#endif
#if MISSING_EXTERN_KILL
extern	int	kill	P((int, int));
#endif
#if HAVE_KILLPG && MISSING_EXTERN_KILLPG
extern	int	killpg	P(( int, int ));
#endif
#if HAVE_LINK && MISSING_EXTERN_LINK
extern	int	link	P(( char *, char * ));
#endif
#if MISSING_EXTERN_LONGJMP
extern	void	longjmp	P((jmp_buf env, int val));
#endif
#if MISSING_EXTERN_LSTAT
extern	int	lstat P((const char *, struct stat *));
#endif
#if MISSING_EXTERN_MEMSET
extern	void	memset	P((char *, int ch, int n));
#endif
#if HAVE_MKDIR && MISSING_EXTERN_MKDIR
extern	int	mkdir	P(( char *, int ));
#endif
#if MISSING_EXTERN_MKTEMP
extern char *mktemp P(( char * ));
#endif
#if MISSING_EXTERN_OPEN
extern	int	open	P(( char *, int ));
#endif
#if MISSING_EXTERN_PERROR
extern	void	perror	P(( const char * ));
#endif
#if MISSING_EXTERN_PIPE
extern	int	pipe	P(( int * ));
#endif
#if MISSING_EXTERN_PRINTF
extern	int	printf	P(( const char *, ... ));
#endif
#if MISSING_EXTERN_PUTS
extern	int	puts	P(( const char * ));
#endif
#if MISSING_EXTERN_QSORT
#if ANSI_QSORT
extern void qsort P((void *, size_t, size_t , int (*compar)(const void *, const void *)));
#else
extern void qsort P((void *, size_t, size_t , int (*compar)(char**, char**)));
#endif
#endif
#if MISSING_EXTERN_READ
extern	int	read	P(( int, char *, SIZE_T ));
#endif
#if MISSING_EXTERN_READLINK
extern	int	readlink P((const char *, char *, size_t ));
#endif
#if HAVE_SELECT && MISSING_EXTERN_SELECT
extern	int	select	P(( int, fd_set*, fd_set*, fd_set*, struct timeval* ));
#endif
#if MISSING_EXTERN_SETBUF
extern	void	setbuf	P(( FILE *, char * ));
#endif
#if MISSING_EXTERN_SETBUFFER
extern	void	setbuffer P(( FILE *, char *, int ));
#endif
#if MISSING_EXTERN_SETITIMER
extern	int setitimer P((int, const struct itimerval *, struct itimerval *));
#endif
#if MISSING_EXTERN_SETJMP
extern	int	setjmp	P((jmp_buf env));
#endif
#if MISSING_EXTERN_SETPGRP
#if HAVE_BSD_SETPGRP
extern	int	setpgrp	P(( int, int ));
#else
extern	pid_t	setpgrp	P(( void ));
#endif
#endif
#if MISSING_EXTERN_SETSID
extern	pid_t	setsid	P(( void ));
#endif
#if MISSING_EXTERN_SETVBUF
#if SETVBUF_REVERSED
extern	int	setvbuf P(( FILE *, int, char *, size_t ));
#else
extern	int	setvbuf P(( FILE *, char *, int, size_t ));
#endif
#endif
#if MISSING_EXTERN_SLEEP
extern	int	sleep	P(( UINT ));
#endif
#if MISSING_EXTERN_SSCANF
extern	int	sscanf	P(( const char *, const char *, ... ));
#endif
#if MISSING_EXTERN_STRTOL
extern	long	strtol	P(( const char *, char **, int ));
#endif
#if MISSING_EXTERN_SYSTEM
extern	int	system	P(( const char * ));
#endif
#if MISSING_EXTERN_TIME
extern	long	time	P(( long * ));
#endif
#if MISSING_EXTERN_UNLINK
extern	int	unlink	P(( char * ));
#endif
#if HAVE_UTIME && MISSING_EXTERN_UTIME
extern	int	utime	P(( const char *, const struct utimbuf * ));
#endif
#if HAVE_UTIMES && MISSING_EXTERN_UTIMES
extern	int	utimes	P(( char *, struct timeval * ));
#endif
#if MISSING_EXTERN_WAIT
extern	int	wait	P(( int * ));
#endif
#if MISSING_EXTERN_WRITE
extern	int	write	P(( int, char *, int ));
#endif
#endif

#if CC_DJGPP
/* djhandl.c */
extern u_long was_ctrl_c_hit P(( void ));
extern void want_ctrl_c P(( int ));
extern void clear_hard_error P(( void ));
extern void hard_error_catch_setup P(( void ));
extern void hard_error_teardown P(( void ));
extern int did_hard_error_occur P(( void ));
#endif

#if OPT_SELECTIONS
/* select.c */
extern	void	free_attribs	P(( BUFFER * ));
extern	void	free_attrib	P(( BUFFER *, AREGION * ));
extern	int	assign_attr_id	P(( void ));
extern	void	find_release_attr P(( BUFFER *, REGION * ));
extern	int	sel_begin	P(( void ));
extern	int	sel_extend	P(( int, int ));
extern	void	sel_release	P(( void ));
extern	void	sel_reassert_ownership P(( BUFFER * ));
#if DISP_X11 && XTOOLKIT
extern	int	sel_yank	P(( int ));
extern	int	sel_attached	P(( void ));
extern	BUFFER *sel_buffer	P(( void ));
#endif
extern	int	sel_setshape	P(( REGIONSHAPE ));
extern	int	attributeregion P(( void ));
#endif /* OPT_SELECTIONS */

#if OPT_VMS_PATH
extern	char *	vms2unix_path   P((char *dst, const char *src));
extern	char *	unix2vms_path   P((char *dst, const char *src));
extern	char *	vms_path2dir    P((const char *src));
#endif
