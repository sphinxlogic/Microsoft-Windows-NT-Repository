/*
 * This file is part of the Seyon, Copyright (c) 1992-1993 by Muhammad M.
 * Saggaf. All rights reserved.
 *
 * See the file COPYING (1-COPYING) or the manual page seyon(1) for a full
 * statement of rights and permissions for this program.
 */

#include <stdio.h>
#include <X11/Intrinsic.h>

extern Boolean  do_script(),
                DoToggleCapture(),
                SeGetToggleState();
extern char    *str_strip_lead_end_space(),
               *str_strip_lead_space(),
               *str_strip_end_space(),
               *str_stripspc_copy(),
               *expand_fname(),
                itoa(),
               *mport(),
                MdmReadChar(),
                TermReadChar(),
                read_seyon_file();
extern long     mbaud();
extern int      open_modem(),
                trminp(),
                io_get_attr(),
                io_set_attr(),
                io_flush(),
                io_send_break(),
                lock_tty(),
                SeFork(),
                coninp(),
                read_mbyte(),
                dial_number(),
                redial(),
                readbyte(),
                MdmSetGetBaud(),
                MdmSetGetCSize(),
                MdmSetGetParity(),
                MdmSetGetStopBits(),
                MdmSetGetIStrip(),
                Online(),
                get_modem_fio();
extern FILE    *open_file(),
               *open_file_va();
extern void     send_break(),
                usleep(),
                show(),
                showf(),
                SeyonMessage(),
                SeyonMessagef(),
                close_modem(),
                StartTerminal(),
                KillTerminal(),
                restart_terminal(),
                getword(),
                se_warning(),
                se_warningf(),
                SeError(),
                SeErrorF(),
                SePError(),
                SePErrorF(),
                SeNotice(),
                SeNoticeF(),
                setup_signal_handlers(),
                exec_close_escript(),
                exec_close_script(),
                io_set_speed(),
                hangup(),
                s_exit(),
                unlock_tty(),
                SeSetUnsetToggle(),
                ProcRequest(),
                write_child_info(),
                writef_child_info(),
                exec_cmd(),
                ShellCommand(),
                ExecShellCommand(),
                sendbyte(),
                send_tbyte(),
                send_slowly(),
                sendf_slowly(),
                cancel_dial(),
                SeBeep(),
                Beep(),
                SeSetViewportDimFromMultiList(),
                get_dial_info(),
                exec_dial(),
                TopDial(),
                put_parameters(),
                get_parameters(),
                SeSetDialogValue(),
                xc_setflow(),
                set_rtscts(),
                RemoveCurrentItem(),
                DialCirculate(),
                DialerEnd(),
                read_file(),
                read_pipe_data(),
                write_pipe_data(),
                read_close_file(),
                dial(),
                sync_modem_attr(),
                PortToTty(),
                do_exit(),
                get_line(),
                getline(),
                lc_word(),
                toggle_flag(),
                cpmode(),
                set_onoff(),
                SetStatusMessage(),
                set_modem_fio(),
                set_modem_attr(),
                get_modem_attr(),
                mattach(),
                MdmIFlush(),
                MdmPurge(),
                DestroyShell(),
                DestroyShellCallBack(),
                DestroyParentPopup(),
                DismissPopup2(),
                EditFile(),
                ExecProcRequest(),
                ExecExit(),
                SePopupNoticeF(),
                SeSetViewportDimFromList(),
                SeSetViewportDimensions(),
                ScriptEnd(),
                SetStatusMessagef(),
                w_exit_up(),
                w_kill_up(),
                pre_process(),
                post_process(),
                ExecScript(),
                SeSetWidgetWidth(),
                SeSetWidgetHeight(),
                SeSetValue(),
                SeSetLabel(),
                SeAppMSleep(),
                restore_orig_mode(),
                GetWord(),
                FunMessage(),
                IdleGuard(),
                MdmPutString(),
                mprintf(),
                PopupError(),
                kill_w_child(),
                ReadCommentedFile(),
                FreeList(),
                UpdateStatusBox(),
                CenterShell(),
                CenterShellOnRoot(),
                PositionShell(),
                ResetFlag(),
                SetSensitiveOn(),
                set_tty_mode();
extern Widget   GetShell(),
                SePopupDialogGetString(),
                SeAddButton(),
                AddButton(),
                SeAddButtonWithClientData(),
                SeAddButtonWCD(),
                SeAddLabel(),
                SeAddToggle(),
                SeAddToggleWCD(),
                SeCreatePopup(),
                SeCreateTopLevelPopup(),
                SeAddDialog(),
                AddBox(),
                SeAddBox(),
                SeAddForm(),
                SeAddPaned(),
                AddPaned(),
                SeAddPopupWG(),
                SeAddPopup(),
                AddSimplePopup(),
                DisplayFile(),
                SeAddPopupOffset(),
                SeAddPopupUL(),
                SePopupNotice(),
                SePopupRadio(),
                PopupDialogGetValue(),
                SePopupDialogGetString(),
                SePopupDialogGetStringE();
extern Dimension SeWidgetWidth(),
                SeWidgetHeight();

/*
 *   Apprarently these two are not ANSI?
 */
#if __STDC__ == 1 && defined(SVR4)
extern int      kill(pid_t, int),
                fileno(FILE *);
#endif

#if HAVE_TERMIOS
extern struct termios oldmode,
                newmode;
#else
#if HAVE_TERMIO
extern struct termio oldmode,
                newmode;
#else
#if HAVE_SGTTYB
extern struct sgttyb oldmode,
                newmode;
#endif
#endif
#endif

/*
 * defined constants
 */

#define VERSION "2.0"
#define REVISION "6"

#define START_TERM                20
#define START_AUTO_ZM             21
#define SET_MESSAGE               22
#define KILL_TERM                 23
#define EXIT_PROGRAM              24
#define EXEC_SCRIPT               25
#define TOP_DIAL                  26
#define POPUP_ERROR               27
#define TOP_DIAL_START            28

#define SHELLPOS_HWFH             1

#define TIN_BUF 20				/* number of chars in a tiny buffer */
#define LIT_BUF 40				/* number of chars in a little buffer */
#define SM_BUF  80				/* number of chars in a small buffer */
#define REG_BUF 256				/* number of chars in a regular buffer */
#define LRG_BUF 1024			/* number of chars in a large buffer */

#define MDELAY  50000			/* delay of output to modem in command mode */

/*
 * Modem lines
 */

#define NUM_MDM_STAT 8

#define MDM_LE    0x001
#define MDM_DTR   0x002
#define MDM_RTS   0x004
#define MDM_ST    0x008
#define MDM_SR    0x010
#define MDM_CTS   0x020
#define MDM_DCD   0x040
#define MDM_RNG   0x080
#define MDM_DSR   0x100

/*
 * macros
 */

#define SSpc(str) str_strip_lead_end_space(str)

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

#define ret_if_up(w, up_fl) {if (up_fl) {SeBeep(w);	return;}}
#define ReturnIfTrue(flag) {if (flag) {Beep();	return;}}
#define ErrorIfBusy() {if(inhibit_child) {Beep(); \
  SeyonMessage("Busy with Another Process"); return;}}

#define GET_ARG() {getword(); if (eof_flag) return;}
#define GETTEST_ARG(cmd) {GET_ARG(); if (word[0] == '\0') { \
  SeErrorF("Missing argument to the command `%s'", cmd, "", ""); \
  eof_flag = 1; return;}}

#define PopupCentered(popup,parent) \
  {CenterShell(popup,parent); XtPopupSpringLoaded(popup);}
#define PopupCenteredOnRoot(popup) \
  {CenterShellOnRoot(popup);  XtPopupSpringLoaded(popup);}
#define SimpleError(message) {Beep(); SeyonMessage(message); return;}

/*
 * resource structure
 */

struct QueryResources {
  String          modem;
  String          script;

  String          defaultBPS;
  int             defaultBits;
  int             defaultParity;
  int             defaultStopBits;
  Boolean         stripHighBit;
  Boolean         backspaceTranslation;
  Boolean         metaKeyTranslation;
  Boolean         xonxoffFlowControl;
  Boolean         rtsctsFlowControl;
  String          newlineTranslation;

  String          dialPrefix;
  String          dialSuffix;
  String          dialCancelString;

  int             dialTimeOut;
  int             dialDelay;
  int             dialRepeat;

  String          connectString;
  String          noConnectString[4];

  Boolean         hangupBeforeDial;
  Boolean         dialDirAutoOpen;
  Boolean         dialAutoStart;
  Boolean         dialDirAutoClose;
  Boolean         dialDirAutoIconify;
  Boolean         beepOnConnect;
  String          dialDirFormat;
  String          defaultPhoneEntries;

  int             modemVMin;
  Boolean         ignoreModemDCD;
  Boolean         hayesHangup;
  Boolean         hangupConfirm;
  Boolean         exitConfirm;

  Boolean         zmodemAutoDownload;
  String          zmodemAutoDownloadCommand;

  String          defaultDirectory;
  String          scriptDirectory;
  String          startupFile;
  String          phoneFile;
  String          protocolsFile;
  String          captureFile;
  String          helpFile;

  int             modemStatusInterval;
  Boolean         idleGuard;
  int             idleGuardInterval;
  String          idleGuardString;
  Boolean         showFunMessages;
  int             funMessagesInterval;
  String         *funMessages;

  String          quickKey[10];
};

extern XtAppContext app_con;
extern struct QueryResources qres;

extern Widget   dialWidget;

extern Boolean  inhibit_child;
extern char     captureFile[REG_BUF],
                modem_port[REG_BUF],
               *lptr;
extern int      newlineTrMode;
extern int      child_pipe[2];
extern pid_t    w_child_pid;

extern int      quote_flag;
extern int      linkflag,
                eof_flag;
extern char     word[],
               *wptr,
                line[];
