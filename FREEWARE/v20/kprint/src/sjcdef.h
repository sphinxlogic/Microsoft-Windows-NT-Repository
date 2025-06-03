#define SJC$_ABORT_JOB		1	/*  Abort current job in a queue */
#define SJC$_ADD_FILE		2	/*  Add a file to an open job */
#define SJC$_ALTER_JOB		3	/*  Alter parameters of a job */
#define SJC$_ALTER_QUEUE	4	/*  Alter parameters of a queue */
#define SJC$_ASSIGN_QUEUE	5	/*  Assign a logical queue to an execution queue */
#define SJC$_BATCH_CHECKPOINT	6	/*  Establish a checkpoint in a batch job */
#define SJC$_BATCH_SERVICE	7	/*  LOGINOUT communication */
#define SJC$_CLOSE_DELETE	8	/*  Close and delete an open job */
#define SJC$_CLOSE_JOB		9	/*  Close and enqueue an open job */
#define SJC$_CREATE_JOB		10	/*  Create an open job */
#define SJC$_CREATE_QUEUE	11	/*  Create a queue */
#define SJC$_DEASSIGN_QUEUE	12	/*  Deassign a logical queue */
#define SJC$_DEFINE_CHARACTERISTIC	13	/*  Create a characteristic definition */
#define SJC$_DEFINE_FORM	14	/*  Create a form definition */
#define SJC$_DELETE_CHARACTERISTIC	15	/*  Delete a characteristic definition */
#define SJC$_DELETE_FORM	16	/*  Delete a form definition */
#define SJC$_DELETE_JOB		17	/*  Delete a job */
#define SJC$_DELETE_QUEUE	18	/*  Delete a queue */
#define SJC$_ENTER_FILE		19	/*  Enter a single file job in a queue */
#define SJC$_MERGE_QUEUE	20	/*  Requeue all jobs in one queue to another */
#define SJC$_PAUSE_QUEUE	21	/*  Suspend processing from a queue */
#define SJC$_RESET_QUEUE	22	/*  Reset a queue */
#define SJC$_START_ACCOUNTING	23	/*  Start accounting manager */
#define SJC$_START_QUEUE	24	/*  Start processing from a queue	 */
#define SJC$_START_QUEUE_MANAGER	25	/*  Start system job queue manager */
#define SJC$_STOP_ACCOUNTING	26	/*  Stop accounting manager */
#define SJC$_STOP_QUEUE		27	/*  Stop processing from a queue */
#define SJC$_STOP_QUEUE_MANAGER	28	/*  Stop system job queue manager */
#define SJC$_SYNCHRONIZE_JOB	29	/*  Synchronize with job completion */
#define SJC$_WRITE_ACCOUNTING	30	/*  Write user accounting record */
#define SJC$_RESERVED_FUNC_1	31	/*  Reserved for Digital use (name may change) */
#define SJC$_RESERVED_FUNC_2	32	/*  Reserved for Digital use (name may change) */

#define SJC$_ACCOUNTING_MESSAGE	1	/*  User accounting message */
#define SJC$_ACCOUNTING_TYPES	2	/*  Mask of accounting record types */
#define SJC$_AFTER_TIME		3	/*  Hold until time */
#define SJC$_NO_AFTER_TIME	4	/*  Mask alignment data */
#define SJC$_ALIGNMENT_MASK	5
#define SJC$_ALIGNMENT_PAGES	6	/*  Number of alignment pages */
#define SJC$_BASE_PRIORITY	7	/*  Batch process base priority */
#define SJC$_BATCH		8	/*  Batch queue */
#define SJC$_NO_BATCH		9
#define SJC$_BATCH_INPUT	10	/*  LOGINOUT communication */
#define SJC$_BATCH_OUTPUT	11
#define SJC$_CHARACTERISTIC_NAME	12	/*  Printer characteristic */
#define SJC$_CHARACTERISTIC_NUMBER	13
#define SJC$_NO_CHARACTERISTICS	14
#define SJC$_CHECKPOINT_DATA	15	/*  Batch checkpoint string */
#define SJC$_NO_CHECKPOINT_DATA	16
#define SJC$_CLI		17	/*  Batch process CLI */
#define SJC$_NO_CLI		18
#define SJC$_CPU_DEFAULT	19	/*  Default CPU time limit */
#define SJC$_NO_CPU_DEFAULT	20
#define SJC$_CPU_LIMIT		21	/*  Maximum CPU time limit */
#define SJC$_NO_CPU_LIMIT	22
#define SJC$_CREATE_START	23	/*  Start queue after creation */
#define SJC$_DELETE_FILE	24	/*  Delete file after processing */
#define SJC$_NO_DELETE_FILE	25
#define SJC$_DESTINATION_QUEUE	26	/*  Destination queue name */
#define SJC$_DEVICE_NAME	27	/*  Output device name */
#define SJC$_DOUBLE_SPACE	28	/*  Double space output */
#define SJC$_NO_DOUBLE_SPACE	29
#define SJC$_ENTRY_NUMBER	30	/*  Job entry number */
#define SJC$_ENTRY_NUMBER_OUTPUT	31
#define SJC$_FILE_BURST		32	/*  Print file burst page */
#define SJC$_FILE_BURST_ONE	33
#define SJC$_NO_FILE_BURST	34
#define SJC$_FILE_COPIES	35	/*  Print n copies of file */
#define SJC$_FILE_FLAG		36	/*  Print file flag page */
#define SJC$_FILE_FLAG_ONE	37
#define SJC$_NO_FILE_FLAG	38
#define SJC$_FILE_IDENTIFICATION	39	/*  DVI, FID, DID of file */
#define SJC$_FILE_SETUP_MODULES	40	/*  Device control modules for file */
#define SJC$_NO_FILE_SETUP_MODULES	41
#define SJC$_FILE_SPECIFICATION	42	/*  File specification of file */
#define SJC$_FILE_TRAILER	43	/*  Print file trailer page */
#define SJC$_FILE_TRAILER_ONE	44
#define SJC$_NO_FILE_TRAILER	45
#define SJC$_FIRST_PAGE		46	/*  Starting page number */
#define SJC$_NO_FIRST_PAGE	47
#define SJC$_FORM_DESCRIPTION	48	/*  Textual description of form */
#define SJC$_FORM_LENGTH	49	/*  Form length in lines */
#define SJC$_FORM_MARGIN_BOTTOM	50	/*  Form bottom margin in lines */
#define SJC$_FORM_MARGIN_LEFT	51	/*  Form left margin in characters */
#define SJC$_FORM_MARGIN_RIGHT	52	/*  Form right margin in characters */
#define SJC$_FORM_MARGIN_TOP	53	/*  Form top margin in lines */
#define SJC$_FORM_NAME		54	/*  Printer form */
#define SJC$_FORM_NUMBER	55
#define SJC$_FORM_SETUP_MODULES	56	/*  Device control modules for form */
#define SJC$_NO_FORM_SETUP_MODULES	57
#define SJC$_FORM_SHEET_FEED	58	/*  Form is sheet fed */
#define SJC$_NO_FORM_SHEET_FEED	59
#define SJC$_FORM_STOCK		60	/*  Stock name for form */
#define SJC$_FORM_TRUNCATE	61	/*  Truncate long lines */
#define SJC$_NO_FORM_TRUNCATE	62
#define SJC$_FORM_WIDTH		63	/*  Form width in characters */
#define SJC$_FORM_WRAP		64	/*  Wrap long lines */
#define SJC$_NO_FORM_WRAP	65
#define SJC$_GENERIC_QUEUE	66	/*  Queue is a generic queue */
#define SJC$_NO_GENERIC_QUEUE	67
#define SJC$_GENERIC_SELECTION	68	/*  Queue can take work from generic queue */
#define SJC$_NO_GENERIC_SELECTION	69
#define SJC$_GENERIC_TARGET	70	/*  Possible execution queue for generic queue */
#define SJC$_HOLD		71	/*  Place job on hold */
#define SJC$_NO_HOLD		72
#define SJC$_JOB_BURST		73	/*  Print job burst page */
#define SJC$_NO_JOB_BURST	74
#define SJC$_JOB_COPIES		75	/*  Print n copies of entire job */
#define SJC$_JOB_FLAG		76	/*  Print job flag page */
#define SJC$_NO_JOB_FLAG	77
#define SJC$_JOB_LIMIT		78	/*  Number of concurrent jobs */
#define SJC$_JOB_NAME		79	/*  Identifying name of job */
#define SJC$_JOB_RESET_MODULES	80	/*  Device control modules between jobs */
#define SJC$_NO_JOB_RESET_MODULES	81
#define SJC$_JOB_SIZE_MAXIMUM	82	/*  Largest job accepted by printer */
#define SJC$_NO_JOB_SIZE_MAXIMUM	83
#define SJC$_JOB_SIZE_MINIMUM	84	/*  Smallest job accepted by printer */
#define SJC$_NO_JOB_SIZE_MINIMUM	85
#define SJC$_JOB_SIZE_SCHEDULING	86	/*  Schedule printer queues by size */
#define SJC$_NO_JOB_SIZE_SCHEDULING	87
#define SJC$_JOB_STATUS_OUTPUT	88	/*  Status message for submitted job */
#define SJC$_JOB_TRAILER	89	/*  Print job trailer page */
#define SJC$_NO_JOB_TRAILER	90
#define SJC$_LAST_PAGE		91	/*  Ending page number */
#define SJC$_NO_LAST_PAGE	92
#define SJC$_LIBRARY_SPECIFICATION	93	/*  Filename of device control library */
#define SJC$_NO_LIBRARY_SPECIFICATION	94
#define SJC$_LOG_DELETE		95	/*  Delete log file */
#define SJC$_NO_LOG_DELETE	96	/*  Log file's print queue */
#define SJC$_LOG_QUEUE		97
#define SJC$_LOG_SPECIFICATION	98	/*  File specification of log file */
#define SJC$_NO_LOG_SPECIFICATION	99
#define SJC$_LOG_SPOOL		100	/*  Print log file */
#define SJC$_NO_LOG_SPOOL	101
#define SJC$_LOWERCASE		102	/*  Print on lowercase printer */
#define SJC$_NO_LOWERCASE	103
#define SJC$_NEW_VERSION	104	/*  Create new version of file */
#define SJC$_NEXT_JOB		105	/*  Resume at next job */
#define SJC$_NOTE		106	/*  Flag page note text */
#define SJC$_NO_NOTE		107
#define SJC$_NOTIFY		108	/*  Notify user on completion */
#define SJC$_NO_NOTIFY		109
#define SJC$_OPERATOR_REQUEST	110	/*  Operator service text */
#define SJC$_NO_OPERATOR_REQUEST	111
#define SJC$_OWNER_UIC		112	/*  Queue owner UIC */
#define SJC$_PAGE_HEADER	113	/*  Print page headers */
#define SJC$_NO_PAGE_HEADER	114
#define SJC$_PAGE_SETUP_MODULES	115	/*  Device control modules for form page */
#define SJC$_NO_PAGE_SETUP_MODULES	116
#define SJC$_PAGINATE		117	/*  Paginate output with free form feeds */
#define SJC$_NO_PAGINATE	118
#define SJC$_PARAMETER_1	119	/*  Batch job parameters */
#define SJC$_PARAMETER_2	120
#define SJC$_PARAMETER_3	121
#define SJC$_PARAMETER_4	122
#define SJC$_PARAMETER_5	123
#define SJC$_PARAMETER_6	124
#define SJC$_PARAMETER_7	125
#define SJC$_PARAMETER_8	126
#define SJC$_NO_PARAMETERS	127
#define SJC$_PASSALL		128	/*  Print file passall */
#define SJC$_NO_PASSALL		129	/*  Job scheduling priority */
#define SJC$_PRIORITY		130
#define SJC$_PROCESSOR		131	/*  Filename of symbiont image */
#define SJC$_NO_PROCESSOR	132
#define SJC$_PROTECTION		133	/*  Queue protection mask */
#define SJC$_QUEUE		134	/*  Queue on which to operate */
#define SJC$_QUEUE_FILE_SPECIFICATION	135	/*  File specification of queue file */
#define SJC$_RELATIVE_PAGE	136	/*  Resume after forward or back space */
#define SJC$_REQUEUE		137	/*  Requeue job */
#define SJC$_RESTART		138	/*  Job can restart */
#define SJC$_NO_RESTART		139
#define SJC$_RETAIN_ALL_JOBS	140	/*  Retain completed jobs */
#define SJC$_RETAIN_ERROR_JOBS	141
#define SJC$_NO_RETAIN_JOBS	142
#define SJC$_SCSNODE_NAME	143	/*  Node name of execution node */
#define SJC$_SEARCH_STRING	144	/*  Resume after finding string */
#define SJC$_SWAP		145	/*  Swap batch processes */
#define SJC$_NO_SWAP		146
#define SJC$_TERMINAL		147	/*  Queue is a (generic) terminal queue */
#define SJC$_NO_TERMINAL	148
#define SJC$_TOP_OF_FILE	149	/*  Resume at top of file */
#define SJC$_USER_IDENTIFICATION	150	/*  Proxy user identification (reserved for Digital use only) */
#define SJC$_WSDEFAULT		151	/*  Working set default */
#define SJC$_NO_WSDEFAULT	152
#define SJC$_WSEXTENT		153	/*  Working set extent */
#define SJC$_NO_WSEXTENT	154
#define SJC$_WSQUOTA		155	/*  Working set quota */
#define SJC$_NO_WSQUOTA		156
#define SJC$_ACCOUNT_NAME	157	/*  Proxy account name (requires CMKRNL privilege) */
#define SJC$_UIC		158	/*  Proxy UIC (requires CMKRNL privilege) */
#define SJC$_USERNAME		159	/*  Proxy username (requires CMKRNL privilege) */
#define SJC$_BUFFER_COUNT	160	/*  Multibuffer count for queue file */
#define SJC$_EXTEND_QUANTITY	161	/*  Allocation and extend quantity for queue file */
#define SJC$_RESERVED_BOOLEAN_1	162	/*  Reserved for Digital use (name may change) */
#define SJC$_RESERVED_BOOLEAN_2	163	/*  Reserved for Digital use (name may change) */
#define SJC$_RESERVED_BOOLEAN_3	164	/*  Reserved for Digital use (name may change) */
#define SJC$_RESERVED_BOOLEAN_4	165	/*  Reserved for Digital use (name may change) */
#define SJC$_RESERVED_INPUT_1	166	/*  Reserved for Digital use (name may change) */
#define SJC$_RESERVED_INPUT_2	167	/*  Reserved for Digital use (name may change) */
#define SJC$_RESERVED_INPUT_3	168	/*  Reserved for Digital use (name may change) */
#define SJC$_RESERVED_INPUT_4	169	/*  Reserved for Digital use (name may change) */
#define SJC$_RESERVED_OUTPUT_1	170	/*  Reserved for Digital use (name may change) */
#define SJC$_RESERVED_OUTPUT_2	171	/*  Reserved for Digital use (name may change) */

#define SJC$M_ACCT_PROCESS		1
#define SJC$M_ACCT_IMAGE		2
#define SJC$M_ACCT_INTERACTIVE		4
#define SJC$M_ACCT_LOGIN_FAILURE	8
#define SJC$M_ACCT_SUBPROCESS		16
#define SJC$M_ACCT_DETACHED		32
#define SJC$M_ACCT_BATCH		64
#define SJC$M_ACCT_NETWORK		128
#define SJC$M_ACCT_PRINT		256
#define SJC$M_ACCT_MESSAGE		512
#define SJC$S_ACCOUNTING_TYPES		4
/*
macro SJC$V_ACCT_PROCESS	0,0,1,0 %;	!  Process termination
macro SJC$V_ACCT_IMAGE		0,1,1,0 %;	!  Image termination
macro SJC$V_ACCT_INTERACTIVE	0,2,1,0 %;	!  Interactive processes
macro SJC$V_ACCT_LOGIN_FAILURE	0,3,1,0 %;	!  Login failures
macro SJC$V_ACCT_SUBPROCESS	0,4,1,0 %;	!  Subprocesses
macro SJC$V_ACCT_DETACHED	0,5,1,0 %;	!  Detached processes
macro SJC$V_ACCT_BATCH		0,6,1,0 %;	!  Batch processes
macro SJC$V_ACCT_NETWORK	0,7,1,0 %;	!  Network processes
macro SJC$V_ACCT_PRINT		0,8,1,0 %;	!  Print job termination
macro SJC$V_ACCT_MESSAGE	0,9,1,0 %;	!  User messages
macro SJC$V_ACCT_UNUSED		0,10,22,0 %;
*/
#define SJC$S_ACCT_UNUSED	22
