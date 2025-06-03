/********************************************************************************************************************************/
/* Created  9-JUN-1988 18:14:11 by VAX-11 SDL X3.1-5      Source:  9-JUN-1988 18:13:51 FIZBUZ$USER:[SAUNDERS.BANNER.C]BANNER.SDL; */
/********************************************************************************************************************************/
 
/*** MODULE banner IDENT V1.0 ***/
/*                                                                          */
/* Structure definitions for the DECwindows Banner program;                 */
/*                                                                          */
/*                                                                          */
/* Structure definition for the info block used to keep track of            */
/*   all the banner resources. ;                                            */
/*                                                                          */

typedef struct {
    char *alarm;
    int hour;
    int minute;
    int canceled;
    } Bnr$Alarm;

typedef struct _Bnr$_Blk {
    int *drm_prt;
    volatile int (*alarm_list)[];
    volatile int  alarm_count;
    Widget setup_widget;
    Widget alarm_widget;
    Widget sound_widget;
    Widget dp_id;                  /* Display id                       */
    Widget top_widget;             /* top widget id.                   */
    Widget main_widget;            /* Main widget point (Main window)  */
    Widget clock_shell_widget;	   /* Spinning cube widget pointer     */
    Widget clock_widget;           /* Spinning cube widget pointer     */
    Widget message_shell_widget;   /* Message widget pointer     */
    Widget message_widget;         /* Message widget pointer     */
    Widget message_text_widget;	   /* The STEXT message widget */
    Widget cube_shell_widget;	   /* Spinning cube widget pointer     */
    Widget cube_widget;            /* Spinning cube widget pointer     */
    Widget globe_shell_widget;	   /* Spinning cube widget pointer     */
    Widget globe_widget;            /* Spinning cube widget pointer     */
    Widget hanoi_shell_widget;	   /* Spinning cube widget pointer     */
    Widget hanoi_widget;            /* Spinning cube widget pointer     */
    Widget cpu_shell_widget;	   /* Cpu widget pointer               */
    Widget cpu_widget;             /* Cpu widget pointer               */
    Widget link_shell_widget;	   /* link widget pointer           */
    Widget link_widget;             /* link widget pointer           */
    Widget mon_shell_widget;	   /* Monitor widget pointer           */
    Widget mon_widget;             /* Monitor widget pointer           */
    Widget clu_shell_widget;	   /* Monitor widget pointer           */
    Widget clu_widget;             /* Monitor widget pointer           */
    Widget qb_shell_widget;	   /* Monitor widget pointer           */
    Widget qb_widget;             /* Monitor widget pointer           */
    Widget qp_shell_widget;	   /* Monitor widget pointer           */
    Widget qp_widget;             /* Monitor widget pointer           */
    Widget process_shell_widget;   /* Process widget pointer           */
    Widget process_widget;         /* Process widget pointer           */
    Widget pe_shell_widget;	   /* LAVC monitor */
    Widget pe_widget;		   /* LAVC monitor */
    Widget lck_shell_widget;	   /* lck monitor */
    Widget lck_widget;		   /* lck monitor */
    Widget cwps_shell_widget;	   /* cwps monitor */
    Widget cwps_widget;		   /* cwps monitor */
    Widget sys_shell_widget;	   /* system monitor */
    Widget sys_widget;		   /* system monitor */
    Widget tp_shell_widget;	   /* system monitor */
    Widget tp_widget;		   /* system monitor */
    int clock_x;		   /* Window X position                */
    int clock_y;		   /* Window Y position                */
    int clock_height;
    int clock_width;
    int message_x;		   /* Window X position                */
    int message_y;		   /* Window Y position                */
    int message_height;
    int message_width;
    int cube_x;			   /* Window X position                */
    int cube_y;			   /* Window Y position                */
    int cube_height;
    int cube_width;
    int hanoi_x;			   /* Window X position                */
    int hanoi_y;			   /* Window Y position                */
    int hanoi_height;
    int hanoi_width;
    int globe_x;			   /* Window X position                */
    int globe_y;			   /* Window Y position                */
    int globe_height;
    int globe_width;
    int cpu_x;			   /* Window X position                */
    int cpu_y;			   /* Window Y position                */
    int cpu_height;
    int cpu_width;
    int link_x;			   /* Window X position                */
    int link_y;			   /* Window Y position                */
    int link_height;
    int link_width;
    int mon_x;			   /* Window X position                */
    int mon_y;			   /* Window Y position                */
    int mon_height;
    int mon_width;
    int clu_x;			   /* Window X position                */
    int clu_y;			   /* Window Y position                */
    int clu_height;
    int clu_width;
    int qb_x;			   /* Window X position                */
    int qb_y;			   /* Window Y position                */
    int qb_height;
    int qb_width;
    int qp_x;			   /* Window X position                */
    int qp_y;			   /* Window Y position                */
    int qp_height;
    int qp_width;
    int process_x;		   /* Window X position                */
    int process_y;		   /* Window Y position                */
    int process_height;
    int process_width;
    int pe_x;			   /* Window X position                */
    int pe_y;			   /* Window Y position                */
    int pe_height;
    int pe_width;
    int lck_x;			   /* Window X position                */
    int lck_y;			   /* Window Y position                */
    int lck_height;
    int lck_width;
    int cwps_x;			   /* Window X position                */
    int cwps_y;			   /* Window Y position                */
    int cwps_height;
    int cwps_width;
    int tp_x;			   /* Window X position                */
    int tp_y;			   /* Window Y position                */
    int tp_height;
    int tp_width;
    int sys_x;			   /* Window X position                */
    int sys_y;			   /* Window Y position                */
    int sys_height;
    int sys_width;
    int sys_ioratemax;
    int sysrespgfile;
    int sys_faultratemax;
    int sys_highlight;
    XFontStruct *font_ptr;	   /* Pointer to current font structure */
    int font_height;		   /* Current font height              */
    int font_width;		   /* Current font width               */
    int foreground;
    int background;
    int hwmrk;
    char *foreground_colour;
    char *background_colour;
    char *hwmrk_colour;
    char *fnt_template;
    char *fnt_name;
    char *fnt_type;
    char *fnt_size;
    char *title;
    char *disk_0_name;
    char *disk_1_name;
    char *disk_2_name;
    char *disk_3_name;
    char *disk_0;
    char *disk_1;
    char *disk_2;
    char *disk_3;
    char *qp_name;
    char *qb_name;
    char *queue_user;
    int tp_max_rate;
    int	tp_rate1_start;
    int	tp_rate1_end;
    int	tp_rate2_start;
    int	tp_rate2_end;
    int	tp_rate3_start;
    int	tp_rate3_end;
    int	tp_rate4_start;
    int last_iocnt;
    int cpu_count;
    int cpu_update;
    int clu_update;
    int qb_update;
    int qp_update;
    int monitor_update;
    int link_update;
    int pe_update;
    int lck_update;
    int cwps_update;
    int sys_update;
    int tp_update;
    int process_update;
    int message_init_state;
    int hwmrk_percent;
    int hwmrk_update;
    int current_day;		    /* Current date                     */
    int current_time;		    /* Current time                     */

    int ws_purged;		    /* Purge the working set            */
    int bell_volume;		    /* 0 - 100 Bell volume */
    int base_priority;		    /* Banner Base priority */
    int current_priority;	    /* Banner Current priority */
    int execute_priority;	    /* Banner Execution priority */

/*                                                                          */
/* Flags bits, to indecate what parts of Banner we should activate          */
/*                                                                          */
    struct  {
        unsigned border : 1;      /* Borders should be on             */
        unsigned window_border : 1;      /* Borders should be on             */
        unsigned cube : 1;        /* Cube should be active            */
        unsigned bars : 1;        /* We should draw seperators        */
        unsigned iocnt : 1;       /* Display IO counts                */
        unsigned link_window : 1;     /* Display linkitor                  */
        unsigned monitor : 1;     /* Display monitor                  */
        unsigned process_window : 1;     /* Display process                  */
        unsigned process_states : 1;  /* process states should be displayed */
	unsigned process_name : 1;
	unsigned process_user : 1;
	unsigned process_pid : 1;
	unsigned clu_window : 1;
	unsigned qb_window : 1;
	unsigned qp_window : 1;
	unsigned tp_window : 1;
        unsigned cpu_window : 1;  /* Display the CPU histogram        */
        unsigned cpu_multi_hist : 1;  /* Display the CPU histogram        */
        unsigned pe_window : 1;  /* Display the CPU histogram        */
        unsigned lck_window : 1;  /* Display the CPU histogram        */
        unsigned cwps_window : 1;  /* Display the CPU histogram        */
        unsigned sys_window : 1;  /* Display the sys histogram        */
        unsigned digital_clock : 1; /* Display a digital clock        */
        unsigned analog_clock : 1;  /* Display an analog clock         */
        unsigned world : 1;         /* Display the spinning world       */
        unsigned seconds : 1;       /* Display seconds on the clocks    */
        unsigned hanoi : 1;         /* Display hanoi                    */
        unsigned modes_window : 1;  /* Display the modes window        */
	unsigned seperate_windows : 1; /* Display in seperate windows */
        unsigned both_clocks : 1;      /* display both digital and analog */
        unsigned message_window : 1;
	unsigned bell : 1;
	unsigned icon_on : 1;
	unsigned freeiocnt : 1;
	unsigned freeblkcnt : 1;
	unsigned all_users : 1;
	unsigned jobs_on_hold : 1;
	unsigned stopped_queues : 1;
	unsigned int_jobs : 1;
	unsigned total_com_rate : 1;
	unsigned onef_comp_rate : 1;
	unsigned start_rate : 1;
	unsigned prep_rate : 1;
	unsigned abort_rate : 1;
	unsigned end_rate : 1;
	unsigned rem_start_rate : 1;
	unsigned rem_end_rate : 1;
	unsigned comp_rate1 : 1;
	unsigned comp_rate2 : 1;
	unsigned comp_rate3 : 1;
	unsigned comp_rate4 : 1;	
        } Param_Bits;
    } Bnr$_Blk;

/*                                                                          */
/* Runtime positioning for all the elements of the banner display           */
/*                                                                          */

typedef struct _Clk$_Blk {
    float    center;
    float    hour_hand;
    float    minute_hand;
    float    second_hand;
    float    hours;
    float    minutes;
    float    seconds;
    } Clk$_Blk;

typedef struct _Lck$_Blk {
    int status_bits;
    int spare;
    int atb;
    int loc;
    int loc_rate;
    int in;
    int in_rate;
    int out;
    int out_rate;
    int wtnotq;
    int wtnotq_rate;
    int dir;
    int dir_rate;
    int dlcksrch;
    int dlcksrch_rate;
    int dlckfind;
    int dlckfind_rate;
    int dlckmsg;
    int dlckmsg_rate;
    unsigned flg_lock : 1;
    unsigned flg_lock_in : 1;
    unsigned flg_lock_out : 1;
    unsigned flg_notqd : 1;
    unsigned flg_dir : 1;
    unsigned flg_dlcksrch : 1;
    unsigned flg_dlckfind : 1;
    unsigned flg_dlckmsg : 1;
    } Lck$_Blk;

typedef struct _cwps$_Blk {
    int msgs_in;
    int msgs_in_rate;
    int msgs_out;
    int msgs_out_rate;
    int bytes_in;
    int bytes_in_rate;
    int bytes_out;
    int bytes_out_rate;
    int getjpi_in;
    int getjpi_in_rate;
    int getjpi_out;
    int getjpi_out_rate;
    int pcntrl;
    int pcntrl_rate;
    int rsrc;
    int rsrc_rate;
    unsigned flg_msgs_in : 1;
    unsigned flg_msgs_out : 1;
    unsigned flg_bytes_in : 1;
    unsigned flg_bytes_out : 1;
    unsigned flg_getjpi_in : 1;
    unsigned flg_getjpi_out : 1;
    unsigned flg_pcntrl : 1;
    unsigned flg_rsrc : 1;
    } cwps$_Blk;

typedef struct _pem$_Blk {
    int status_bits;
    int spare;
    int atb;
    int xmt;
    int retry;
    int rcv;
    int pipefull;
    int badchk;
    int timeout;
    int miscerr;
    int rexmt_percent;
    int rercv_percent;
    int xmt_rate;
    int retry_rate;
    int rcv_rate;
    int pipefull_rate;
    int badchk_rate;
    int timeout_rate;
    int miscerr_rate;
    int kbyte;
    int kbyte_rate;
    } pem$_Blk;

typedef struct _pes$_Blk {
    int vc_cnt;
    int xmt_msg;
    int xmt_unseq;
    int xmt_seq;
    int xmt_ack;
    int xmt_rexmt;
    int xmt_cntl;
    int xmt_bytes;
    int xmt_noxch;
    int rcv_msg;
    int rcv_unseq;
    int rcv_seq;
    int rcv_ack;
    int rcv_rercv;
    int rcv_cntl;
    int rcv_bytes;
    int rcv_cache;
    int hs_tmo;
    int rcv_tr_short;
    int rcv_cc_short;
    int rcv_ill_ack;
    int rcv_ill_seq;
    int rcv_bad_chksum;
    int xmt_rcv_norch;
    int rcv_cc_bad_eco;
    int rcv_cc_authorize;
    int xmt_seq_tmo;
    int rcv_listen_tmo;
    int tr_dfq_empty;
    int tr_mfq_empty;
    int cc_dfq_empty;
    int cc_mfq_empty;
    int tr_pipe_quota;
    } pes$_Blk;


typedef struct _Lks$_Blk {
    int enqnew_loc;
    int enqnew_in;
    int enqnew_out;
    int enqcvt_loc;
    int enqcvt_in;
    int enqcvt_out;
    int deq_loc;
    int deq_in;
    int deq_out;
    int enqwait;
    int enqnotqd;
    int blk_loc;
    int blk_in;
    int blk_out;
    int dir_in;
    int dir_out;
    int dlckmsgs_in;
    int dlckmsgs_out;
    int dlcksrch;
    int dlckfnd;
    } Lks$_Blk;

typedef struct _Mon$_Blk {
    int spare;
    int atb;
    int freecnt;
    int mfycnt;
    int fault;
    int sysfault;
    int pageio;
    int bufio;
    int dirio;
    int dirio1;
    int dirio2;
    int dirio3;
    int ni_io_value;
    int freecnt_rate;
    int mfycnt_rate;
    int fault_rate;
    int sysfault_rate;
    int pageio_rate;
    int bufio_rate;
    int dirio_rate;
    int dirio1_rate;
    int dirio2_rate;
    int dirio3_rate;
    int ni_io_rate;
    unsigned dua0 : 1;      /* Dua0 should be on             */
    unsigned dua1 : 1;      /* Dua1 should be on             */
    unsigned dua2 : 1;      /* Dua2 should be on             */
    unsigned dua3 : 1;      /* Dua3 should be on             */
    unsigned pio : 1;       /* Page io should be on             */
    unsigned sysflt : 1;    /* system faults should be on             */
    unsigned ni_io : 1;    /* ni IO  should be on             */
    unsigned mp : 1;        /* mp io should be on             */
    unsigned fp : 1;        /* free pages should be on             */
    unsigned pf : 1;        /* free pages should be on             */
    } Mon$_Blk;

typedef struct _Sound$_Item {
    int		On;
    char	*DisplayName;
    int		*Sound;
    } Sound$_Item;

typedef struct _Son$_Blk {
    int			Volume;
    int			Time;
    Sound$_Item		Mwait;
    Sound$_Item		NewProcesses;
    Sound$_Item		LessProcesses;
    Sound$_Item		Disk1;
    Sound$_Item		Disk2;
    Sound$_Item		Disk3;
    Sound$_Item		Memory;
    Sound$_Item		PageFile;
    Sound$_Item		PendingIO;
    } Son$_Blk;

typedef struct _TextDisplay$_Line {
    unsigned	LastLine : 1;      
    unsigned	highlight : 1;      
    unsigned	text_changed : 1;
    unsigned	seen : 1;
    unsigned	Fill : 13;      
    short int	MaxNameSize;
    short int	MaxNamePoints;
    short int	MaxTextSize;
    short int	MaxTextPoints;
    char	Text[150];
    char	Text1[50];
    } TextDisplay$_Line;

#define clu_max_lines 50

typedef struct _Clu$_Blk {
    TextDisplay$_Line	Lines[clu_max_lines];
    } Clu$_Blk;

typedef struct _Link$_Blk {
    TextDisplay$_Line	Lines[clu_max_lines];
    } Link$_Blk;

typedef struct _Qp$_Blk {
    TextDisplay$_Line	Lines[clu_max_lines];
    int			Updated;
    int			Valid;
    int			CurrentLine;
    int			SavedLine;
    int			BackupLine;
    int			DoingQueue;
    int			QueueAlive;
    } Qp$_Blk, Qb$_Blk, Q$_Blk;

typedef struct _Hanoi$_MoveVec {
    int move;
    int num_disk;
    int from;
    int to;
    int using;
    } Hanoi$_MoveVec;

typedef struct _Display$_Line {
    unsigned	LineActive : 1;      
    unsigned	HalfLine : 1;      
    unsigned	LastLine : 1;      
    unsigned	highltitle : 1;      
    unsigned	TitleChanged : 1;      
    unsigned	highlname : 1;      
    unsigned	NameChanged : 1;      
    unsigned	Fill : 9;      
    char	*Title;
    char	*tval1;
    char	*tval2;
    char	*tval3;
    char	*tval4;
    short int	MaxNameSize;
    short int	MaxNamePoints;
    char	*Name;
    int		MaxVal;
    int		CurVal;
    int		LastVal;
    int		LastVal1;
    int		Hwmrk;
    Sound$_Item	*Sound;
    } Display$_Line;

typedef struct _Sys$_Blk {
    Display$_Line Disk1;
    Display$_Line Disk1Rate;
    Display$_Line Disk2;
    Display$_Line Disk2Rate;
    Display$_Line Disk3;
    Display$_Line Disk3Rate;
    Display$_Line FreeMem;
    Display$_Line ModMem;
    Display$_Line FreeCpu;
    Display$_Line Pgf;
    Display$_Line PgfRate;
    Display$_Line PgIO;
    Display$_Line RemIO;
    } Sys$_Blk;

typedef struct _Tp$_Blk {
    Display$_Line total_com_rate;
    Display$_Line onef_comp_rate;
    Display$_Line start_rate;
    Display$_Line prep_rate;
    Display$_Line abort_rate;
    Display$_Line end_rate;
    Display$_Line rem_start_rate;
    Display$_Line rem_end_rate;
    Display$_Line comp_rate1;
    Display$_Line comp_rate2;
    Display$_Line comp_rate3;
    Display$_Line comp_rate4;
    } Tp$_Blk;

