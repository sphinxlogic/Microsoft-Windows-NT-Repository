$	SAVE_VERIFY = "''F$ENVIRONMENT("VERIFY_IMAGE")'"*2 + 'F$VERIFY(0)'
$	ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
$	ON WARNING THEN GOTO ABNORMAL_EXIT
$	VERIFY_VAR = -
	  F$TRNLNM("''F$PARSE(F$ENVIRONMENT("PROCEDURE"),,,"NAME")'_VERIFY")
$	IF 0'VERIFY_VAR' .GT. 0 .OR. F$MODE() .EQS. "BATCH" THEN SET VERIFY
$!
$! Version designation for this procedure.  It should include a letter
$! prefix, a version number, and a baselevel suffix (e.g., "V01.02-003").
$!
$	VERSION = "V04.00-001"		!update this when edits are made
$!
$	GOTO END_OF_HEADER_COMMENTS

$!=============================================================================
$!
$! WATCH_MAIL.COM -- THIS SOFTWARE MAY BE FREELY DISTRIBUTED.
$!
$! IF EDITS ARE DESIRED OR NECESSARY, PLEASE KEEP ALL LINES PRECEDING THE
$! END_OF_HEADER_COMMENTS LABEL IN PLACE, UPDATING THEM AS NECESSARY TO
$! REFLECT THE CHANGES.  PLEASE ALSO APPEND YOUR INITIALS TO THE ORIGINAL
$! VERSION DESIGNATION AND UPDATE THE EDIT HISTORY.
$!
$! DAN SCHULLMAN, DIGITAL EQUIPMENT CORPORATION, MAYNARD MASSACHUSETTS, 1994
$!
$!
$! DESCRIPTION:
$!
$!	The purpose of this procedure is to automatically reply to new mail
$!	with a specified message for a specified time interval, and possibly
$!	also copy that mail to one or more other users.  For example, if you
$!	are on vacation or away on business, the replies can be used to let
$!	others know that you won't be able to respond to their mail for some
$!	period of time, and also possibly inform them of other people they
$!	might consider contacting in your absence.  The copies (if enabled)
$!	might be checked by someone else to see if something urgent comes up
$!	in your absence.
$!
$!	This procedure is invoked interactively by a user, during which time
$!	all necessary parameters are solicited and various other activities
$!	occur.  Assuming all goes well, a batch job is then submitted to do
$!	the "real" work, though initially it merely performs a "dry run" to
$!	help identify any problems that may exist while you are still around
$!	to fix them.  The parameter values, having been validated and stored
$!	in a file, are used by the batch job each time it runs, and may also
$!	be used to provide default values for subsequent interactive sessions.
$!
$!	The batch job, running at a specified time during the day, reads and
$!	revalidates the parameter values, resubmits itself to run at the next
$!	specified time, scans for new mail, sends replies to those addresses
$!	that are not specifically excluded from receiving them and for which
$!	a reply has not already been generated (if such information is being
$!	kept), and then copies the new mail as desired and moves it into the
$!	WATCH_MAIL mail folder.
$!
$!	For more information, see below.
$!
$!
$! USAGE:
$!
$!	Invoke the procedure interactively (e.g., @WATCH_MAIL), answering the
$!	questions and providing input files as requested.  A batch job will
$!	then be submitted to perform some additional checking and subsequent
$!	mail processing.
$!
$!	Have a nice vacation (or whatever)!
$!
$!	Upon your return, new mail that has been successfully processed by
$!	this procedure will be found in your WATCH_MAIL mail folder.  You may
$!	also have new mail that has come in since the last time the batch job
$!	ran, or to which the procedure has had difficulty replying.
$!
$!	See below for more information.  In particular, see the section on
$!	known restrictions and bugs.

$!
$! KNOWN RESTRICTIONS AND BUGS:
$!
$!	After your specified start time has been reached, this procedure
$!	will begin processing any new mail messages that exist in your mail
$!	file, regardless of when they were received.  If you have new mail
$!	which has gone unread for some time, and don't want this procedure
$!	to process such mail, you should read it, manually file it into your
$!	WATCH_MAIL folder, or otherwise dispose of it before the start time
$!	is reached.
$!
$!	Do not read your new mail while this procedure is running (vs.
$!	queued) in batch mode.  If you do, you will alter the numbering of
$!	your new mail messages, possibly causing this procedure to forward
$!	and file the wrong messages.
$!
$!	If MAIL does not file your new messages in chronological order
$!	(based on their timestamps), this procedure will not be able to
$!	process them or any other new mail you may receive, effectively
$!	disabling this procedure.  It will also generate and notify you
$!	of problem reports.  This is most apt to happen on an OpenVMS
$!	cluster where the clocks are out of synchronization amongst the
$!	systems, though it could also occur when you set the clocks back
$!	an hour for daylight savings time.  To reduce the risk of this
$!	occurring, try to minimize the clock skew within a cluster and/or
$!	the reception of mail during a "repeated" interval of time.
$!
$!	If this procedure processes a new mail message multiple times (e.g.,
$!	because of a clock skew problem) AND if you requested that it not
$!	maintain a history file, the procedure will send multiple replies
$!	for such a message.  To avoid this problem, use a history file.
$!
$!	This procedure runs periodically as an OpenVMS batch job to process
$!	your new mail.  The OpenVMS operating system and this procedure
$!	make every effort to see that such batch jobs are not lost, but
$!	problems can occur, resulting in your new mail going unprocessed.
$!	For example, you may not be allowed to run batch jobs from your
$!	account, or the disk on which this procedure is located may become
$!	unavailable, or a privileged user may stop the batch queue.  You
$!	can try to avoid some of these situations by not placing this
$!	procedure on a DFS-mounted disk, and by not specifying that the
$!	procedure run at a time when system personnel are apt to be doing
$!	system maintenance.
$!
$!	This procedure may terminate abnormally (e.g., on a BUFOVF error)
$!	if it attempts to process a mail message that contains a very long
$!	subject or address string.  Such a message and any that follow it
$!	cannot be processed, and those that precede it may be processed
$!	multiple times.  There is no workaround for this problem, other than
$!	to request that people not send you messages with such long header
$!	information.
$!
$!	If you receive mail from an address that contains a transport prefix
$!	(e.g., "SMTP%") AND Nmail is present on your system, this procedure
$!	will not be able to reply to the message.  There is no workaround
$!	for this problem.
$!
$!	This procedure will not run on OpenVMS AXP V1.5 or earlier systems,
$!	because of some checks that were designed for higher-numbered
$!	OpenVMS VAX version numbers.  You can either upgrade the operating
$!	system or fake the assignment of VMS_VERSION in this procedure.
$!
$!	If this procedure is located in a subdirectory below SYSCOMMON, it
$!	may fail to resubmit itself because it is unable to correctly
$!	identify the directory in which it is located.  The most likely
$!	cause of this problem is that you performed an image backup of your
$!	system disk and the system is now running from the backup copy.  If
$!	this is the case,  SHOW ENTRY/FILE  will display an erroneous file
$!	specification for the queued procedure.  To correct this situation,
$!	execute the following commands in the order shown.
$!
$!	    $ RENAME SYS$SYSDEVICE:[000000]VMS$COMMON.DIR; -
$!	    _$ SYS$SYSDEVICE:[000000]SYSCOMMON.DIR;
$!	    $ RENAME SYS$SYSDEVICE:[000000]SYSCOMMON.DIR; -
$!	    _$ SYS$SYSDEVICE:[000000]VMS$COMMON.DIR;

$!
$! PARAMETERS (general information):
$!
$!	Default values for the parameters may be taken from a parameter file
$!	created by a previous invocation of this procedure, or from "factory
$!	defaults" if no such file is provided.  When supplying parameter
$!	values, you can enter a question-mark (?) to find out what options
$!	are available to you, including such things as how to use the default
$!	or factory default value, how to return to the previous question, and
$!	how to get parameter-specific help, etc.
$!
$!	Time (and date) values are specified using the normal VMS syntax
$!	for such.  "Now" is also a supported designation, as are the names 
$!	of the days of the week optionally followed by a positive offset
$!	(e.g., "Thursday+17:00", "Wednesday+14-8:00").  The weekday for the
$!	start time or stop time is chosen so that it does not precede the
$!	current day or start day respectively.  The weekday names may be
$!	abbreviated to as few as three characters.  Regardless of the format
$!	used to specify a time value, the resulting absolute time is always
$!	displayed to help you verify that the desired time has been selected.
$!	Don't forget that times are specified using a twenty-four hour clock.
$!	The values are saved in the parameter file in their original form
$!	(e.g., "Saturday") as well as absolute form, with the original form
$!	being used to provide defaults for a subsequent session.
$!
$!	Files are specified using the normal VMS syntax for such.  Missing
$!	components of the specification are obtained from the default
$!	parameter value or from the factory default if the current default
$!	also lacks the component.  The net result of such file parsing is
$!	displayed whenever it and the supplied spec differ, and is also the
$!	value that is saved and subsequently used by the procedure.  If you
$!	specify a version number for a file and a higher-numbered version
$!	of that file currently exists, your specification will be challenged.
$!	If the file is an input file, you will be given the option of using
$!	it anyway or entering a different specification.  If the file is an
$!	output file, you will not be allowed to create a new version "under"
$!	an existing version, so you must either have the procedure delete all
$!	existing versions of the file or enter a different specification.  You
$!	also have the option of typing a Control-Z to abort the interaction.
$!	A version specification of ";" or ";0" indicates a desire to read the
$!	highest version or create the next higher version.  By default, all
$!	file specifications have no version number (i.e., ";") and all but
$!	the highest-numbered version of each of the created files is deleted.
$!	If space is a major concern, the creation of some output files may be
$!	totally suppressed by supplying a null file specification (specified
$!	interactively as one or more spaces) for them.

$!
$! PARAMETERS (listed in alphabetical order):
$!
$HELP__Batch_Queue:	HELP_BEGIN
  The name of the batch queue in which this procedure is to be run.  To
  reduce the risk of network errors and other problems, you should choose a
  queue (if your system has such a queue) that is not started until DECnet
  is fully operational.  The factory default is "Sys$Batch", unless modified
  by a VMS logical.
$			HELP_END
$HELP__Copy_To:		HELP_BEGIN
  The optional mail address specification (e.g., "User", "@Group") of people
  who are to receive copies of your new mail, presumably for the purpose of
  monitoring your mail in your absence in case something urgent comes up.  The
  use of Nmail (if your system has it) is recommended in this specification
  or in a distribution list that it specifies (e.g., "nm%User").  Be sure to
  verify the supplied specification, as illegal values will result in failure
  to copy your mail while you are away.  The factory default is null, meaning
  no copying is to be done.
$			HELP_END
$HELP__Exclude_File:	HELP_BEGIN
  The specification of an optional input file which contains a list of mail
  addresses (one per line) to which automatic replies are not to be sent.
  For example, you might not want replies sent to nodes that are mail gateways
  (e.g., DECWRL::*) or to addresses that distribute general information (news
  services, social clubs, training groups, etc.).  An address can be in one
  of the following forms:
    NODE::NAME     Exclude user NAME from NODE.
    *::NAME        Exclude user NAME from any node.
    NODE::*        Exclude all users from NODE.
    NAME           Same as *::NAME.
    *::*           Exclude all users from all nodes.
  Lines in the file which begin with an exclamation point (!) are ignored,
  allowing their use as comments.  If you do not want an exclude file, omit
  the file specification (by entering one or more spaces).  The factory
  default is "SYS$LOGIN:WM_EXCLUDE.DAT;".
$			HELP_END
$HELP__History_File:	HELP_BEGIN
  The specification of an optional output file used to record the addresses
  to which replies have been sent, in order to avoid sending replies to people
  that have already received them.  If you prefer to have a reply sent for
  every new message, regardless of whether or not the sender has already been
  notified of your unavailability, omit the file specification (by entering
  one or more spaces).  The factory default is "SYS$LOGIN:WM_HISTORY.DAT;".
$			HELP_END
$HELP__Log_File:	HELP_BEGIN
  The specification of an optional output file used to log the output from
  the batch jobs, primarily for the purpose of debugging or "checking up" on
  the procedure.  The factory default is "SYS$LOGIN:xxx.LOG;", where "xxx"
  is the file name of this procedure.
$			HELP_END
$HELP__Parameter_File:	HELP_BEGIN
  The specification of the output file in which the parameter values will be
  saved.  The file is used as input by the batch jobs or when restarting a
  batch job, and may also be used during subsequent interactive sessions to
  provide default values.  The factory default is "SYS$LOGIN:WM_PARAMS.DAT;".
$			HELP_END
$HELP__Reply_File:	HELP_BEGIN
  The specification of the input file containing the message which is sent as
  a response to new mail.  The message should typically mention why you are
  unavailable and who might be able to cover for you in your absence, but it
  should not contain sensitive information or references to such because the
  new mail may have originated outside of your company.  The message will be
  sent with a subject that indicates that it is an automatic response and
  includes your start and anticipated stop times.  The latter are to give
  others a time frame in which to interpret statements like "I'm on vacation
  for two weeks".  The factory default is "SYS$LOGIN:WM_REPLY.TXT;".
$			HELP_END
$HELP__Start_Time:	HELP_BEGIN
  The date and time when you want this procedure to start replying to new mail
  (e.g., "Now", "Friday+17:00", "6-Apr").  The factory default is "Tomorrow".
$			HELP_END
$HELP__Stop_Time:	HELP_BEGIN
  The date and time when you want this procedure to stop replying to new mail
  (e.g., "Saturday", "Monday+21-13:00", "4-Nov").  There is no factory default.
$			HELP_END
$HELP__Time_List:	HELP_BEGIN
  A list of times (in HH or HH:MM 24-hour format, separated by commas, and in
  ascending order), or "Every N" (where N is in hours and evenly divides 24),
  specifying the times during the day when the batch job is to run and scan for
  new mail.  Generally one would want the batch job to run several times during
  normal work hours and possibly once or more during the off hours.  Try to
  avoid having the job run when the system is apt to come down for backups or
  some other purpose, as the job might get deleted without being resubmitted.
  The factory default (unless modified by a VMS logical) specifies that the
  job run every three hours, with the starting time being chosen as a function
  of when the interactive session was started.
$			HELP_END
$HELP__Version_Limit:	HELP_BEGIN
  The number of versions of each of the created files that will be allowed to
  exist, with older versions being deleted (through the use of explicit PURGE
  commands and SET FILE/VERSION_LIMIT).  If no limit is desired, specify a
  zero (0) as the value.  The factory default is one (1), unless modified by
  a VMS logical.
$			HELP_END

$!
$! PROCEDURE ARGUMENTS (i.e., P1 to P8):
$!
$!	Normally you will not need to supply arguments to the procedure, as
$!	all required parameters are solicited interactively.  Any arguments
$!	which you do specify should be separated by spaces.  Values currently
$!	allowed are:
$!
$!	CREATE		Indicates a desire to create a new batch job, even
$!			when in batch mode.  This is the default when running
$!			interactively.  The procedure must be invoked by the
$!			DCL indirection operator (@) rather than be submitted.
$!
$!	CANCEL		Indicates a desire to cancel an existing batch job.
$!
$!	RESTART		Indicates a desire to restart a batch job using
$!			information from an existing parameter file.
$!
$!	JOBS		Indicates a desire to list all currently queued batch
$!			jobs, primarily for the purpose of seeing if it is
$!			safe to delete the version of the procedure which is
$!			being used to do the checking.  This option requires
$!			OPER privilege, while SETPRV, SYSPRV, GRPPRV, READALL,
$!			or BYPASS is useful if the current process would not
$!			otherwise have read access to queued procedure files.
$!
$!	<param-file>	The specification of the parameter file, for use in
$!			batch mode.  If supplied when running interactively,
$!			it merely affects the default when the specification
$!			is later solicited.
$!
$!	The following argument values should NOT be specified by you, as they
$!	are only for internal use by the procedure.
$!
$!	Ver=<version>	The version designation of the submitting procedure,
$!			in case there are version incompatibilities that must
$!			be dealt with.

$!
$! SYMBOLS:
$!
$!	Some DCL symbols may be used to alter the default behavior of this
$!	procedure, as follows:
$!
$!	WATCH_MAIL_EDIT
$!	EDIT
$!
$!		These symbols may be used to specify the command for editing
$!		(or creating) an input file, if such editing is requested.
$!		If WATCH_MAIL_EDIT is defined, its value is used.  Otherwise
$!		the value of EDIT or the normal VMS "EDIT" command is used.
$!
$!	Examples:
$!		$ E*DIT == "EDIT/EDT/COMMAND=SYS$LOGIN:EDTINI.EDT"

$!
$! LOGICALS:
$!
$!	Some VMS logicals may be used to alter the default behavior of this
$!	procedure, as follows:
$!
$!	WATCH_MAIL_DEFAULT_BATCH_QUEUE
$!	WATCH_MAIL_DEFAULT_TIME_LIST
$!	WATCH_MAIL_DEFAULT_VERSION_LIMIT
$!
$!		These logicals can be used to alter the "factory defaults"
$!		for the corresponding parameters.  This capability is
$!		primarily for the purpose of allowing system managers to
$!		tailor the default behavior of this procedure on their
$!		systems, though there is nothing that prevents individual
$!		users from defining logicals to alter their own defaults.
$!		If nothing else, users may want to be aware that the factory
$!		defaults can be changed.
$!
$!		If you wish to ensure that the normal factory defaults are
$!		used, regardless of the possible presence of VMS logicals
$!		that could alter those defaults, define each of the possible
$!		logicals to have a value of one or more spaces:
$!
$!			DEFINE <logical-name> " "
$!
$!		Note that all parameter values, whether entered explicitly
$!		by the user or defaulted from a previous value or factory
$!		default, are checked for validity.  In this manner the
$!		validity of alternative factory default values supplied using
$!		logicals is checked.
$!
$!		Also note that the factory defaults are only used in the
$!		absence of all other information, or if explicitly requested
$!		by the user.  After the first time this procedure has been
$!		used, default values will most likely be obtained from the
$!		parameter values which were saved during a previous
$!		interactive session.
$!
$!		Refer to the descriptions of the corresponding parameters for
$!		more information about their values and to determine what the
$!		defaults are in the absence of these logical definitions.
$!
$!	xxx_VERIFY  (where "xxx" is the file name of this procedure)
$!
$!		Command verification is normally disabled when this procedure
$!		runs interactively and enabled when it runs in batch mode,
$!		without regard to the verification state upon entry.  Full
$!		verification may be enabled by setting this logical to a
$!		positive numeric value before the procedure is invoked.  In
$!		batch mode without this logical being appropriately defined,
$!		verification is sometimes disabled (e.g., in some loops) in
$!		order to reduce the size of the log file that would otherwise
$!		result and also to reduce the "noise".
$!
$!	Examples:
$!		$ DEFINE/SYSTEM	WATCH_MAIL_DEFAULT_BATCH_QUEUE	"Sys$NetBatch"
$!		$ DEFINE/GROUP	WATCH_MAIL_DEFAULT_TIME_LIST	"7,13,18"
$!		$ DEFINE	WATCH_MAIL_VERIFY		"1"

$!
$! NOTES:
$!
$!	This procedure is capable of replying to Internet addresses.
$!
$!	During the interactive session with the procedure, you may abort it
$!	by entering a Control-Z whenever you are prompted for input.  You
$!	can also resort to Control-Y in an "emergency".
$!
$!	Normally you may create a newer version of the reply file after the
$!	batch job has been submitted, and have subsequent executions of the
$!	job generate replies using the newer version.  However, if the
$!	specification for the reply file includes a version number (other
$!	than ";0" or ";"), this cannot easily be done.
$!
$!	It is possible to edit the parameter file directly in order to affect
$!	a currently queued batch job.  However, this is not recommended, as
$!	it is safer and probably easier to reinvoke this procedure.
$!
$!	You may wish to create some generic reply messages and associated
$!	information for your "standard reasons" for being out of the office.
$!	For example, by invoking the procedure and answering the questions,
$!	you could create a parameter file of WM_PARAMS.SICK with a start
$!	time of "+2:00", a stop time of "Today+99-0:00", and a reply file
$!	of WM_REPLY.SICK, to be used by you whenever you're sick.  The job
$!	would start two hours after the interactive session and run for (up
$!	to) 99 days with your "I'm out sick" message.  You would then cancel
$!	the job when you returned to work.  You could have a similar set of
$!	files for a two week vacation with a start time of "Friday+18:00"
$!	and a stop time of "Monday+14-7:00".  Because of the way in which
$!	file specifications are defaulted, it's probably easiest to identify
$!	different sets of files by the file type (e.g., ".2WEEKS").  Be sure
$!	to make only generic time statements in your reply message (e.g.,
$!	"I'm on vacation for two weeks"), as the inclusion of specific dates
$!	would prevent reuse of the file.  The subject associated with the
$!	reply will indicate your absolute start and stop times and give
$!	others a frame of reference for interpreting your generic statements.
$!
$!	For a given user, only one WATCH_MAIL job should be queued at any
$!	point in time.  This procedure checks the batch queues looking for
$!	any jobs that are executing the same procedure or have the same job
$!	name.  A job which is running the same procedure (except possibly
$!	for the version number) must be deleted, or a new job must not be
$!	started.  A job which has the same name as this procedure but whose
$!	procedure has a file specification which differs from this one (by
$!	other than the version number) may be deleted, but need not be.  If
$!	the displayed different file specification leads you to believe that
$!	the other job is simply running a copy of this procedure which is
$!	located in a different directory or has a different file type, you
$!	should delete it.  When asked whether you wish to delete an existing
$!	job, in addition to "Yes" or "No", you may also type Control-Z to
$!	abort the interaction and do some further investigation of your own.
$!
$!	If you wish to cancel the job prior to the specified stop time (e.g.,
$!	you return earlier than expected), invoke this procedure with an
$!	argument of "CANCEL" (e.g., @WATCH_MAIL CANCEL).  It will inform you
$!	that a current job already exists and allow you to delete it.
$!
$!	The batch job submitted during an interactive session will begin
$!	executing as soon as possible, in hopes of identifying problems while
$!	you are still available to correct them.  Unless the first run time
$!	within the specified time interval has been reached, this first batch
$!	job will not process any mail, and will resubmit itself for that time.
$!	You should receive a broadcast message (if not disabled) indicating
$!	that the job is completed, without any mention of an error.  If the
$!	job does not start immediately, it may be because too many other batch
$!	jobs are already executing or because the batch queue is stopped.
$!
$!	If a batch job should for some reason "disappear" from the queue
$!	(e.g., because of problems with your login command file or as the
$!	result of a system failure), it can be requeued by invoking this
$!	procedure with an argument of "RESTART" (e.g., @WATCH_MAIL RESTART).
$!	Alternatively a new job can be started via a normal interactive
$!	session, though this will cause any existing history information to
$!	be ignored.  Both of these approaches require, however, that someone
$!	be around to notice the disappearance and restart the job (from your
$!	account)!
$!
$!	The batch jobs are submitted with the /RESTART qualifier on the SUBMIT
$!	command, in hopes that this will further reduce the chances of them
$!	disappearing.  A restarted job, however, may encounter some problems
$!	if the queue that it is in is allowed to start before DECnet is fully
$!	operational.  For examples, this may cause problems when trying to
$!	send replies over the network, or may result in replies being sent to
$!	one's own account because the user's node and/or cluster name is not
$!	yet known.  None of these problems should be fatal.
$!
$!	When using the "/EDIT" qualifier to edit an input file, you need only
$!	enter the qualifier (vs. also re-entering the file specification) if
$!	you wish to edit the file corresponding to the default specification.
$!	You may also abbreviate the qualifier to a single letter (e.g., "/E").
$!	Files created using this qualifier will have the currently specified
$!	version limit applied to them.
$!
$!	Mail sent from an address containing the string "POSTMASTER" or
$!	"-DAEMON" (regardless of alphabetic case) is never replied to, in the
$!	belief that it comes from some sort of mailer package (e.g., Nmail,
$!	Internet gateway).
$!
$!	When searching for a sender's address in the history or exclude
$!	files, "routing nodes" in the the addresses are ignored.  Similarly,
$!	such routing information is not included in the history file.  For
$!	example, a sender's address of "RR::NN:USER" could be matched by
$!	"SS::NN::USER", since the "RR" and "SS" specify nodes that the mail
$!	was routed through, but "NN::USER" is the "real" sender's address.
$!	Note, however, that no attempt is made to decipher Internet addresses
$!	and routing.
$!
$!	Nmail is optional software that provides a store-and-forward mail
$!	transport for use in networked environments.  If a mail message
$!	cannot be delivered to a particular address, and the reason for the
$!	failure is considered temporary (e.g., the node is unreachable, or
$!	there is insufficient disk space to receive the message), Nmail will
$!	make several attempts over a period of time to deliver the message.
$!
$!	If your system has Nmail installed, this procedure will use it to
$!	send the replies.  Otherwise, mail that cannot be replied to due to
$!	a network failure will remain in the NEWMAIL folder until the next
$!	time the batch job executes, at which time another attempt will be
$!	made to reply to it.
$!
$!	If you use the Copy_To feature, and if Nmail is installed on your
$!	system, it is recommended that the Nmail prefix be included with
$!	your mail address(es).  This procedure does not automatically add
$!	the prefix for Copy_To, since the address might specify multiple
$!	people or a distribution list.
$!
$!	Mail which is copied to other users may be received by those users
$!	in a different order than you received them in.  This may cause some
$!	confusion if a later message (e.g., "2 of 2") is received before an
$!	earlier one.
$!
$!	It appears impossible to detect errors when using MAIL to forward
$!	your mail for the Copy_To function.  Therefore, if you supply a bad
$!	address specification, a network link is down, etc., you will get
$!	no indication that an error has occurred and yet your mail will not
$!	have been copied as desired.  The use of Nmail with this feature
$!	would avoid some of these problems, and is another justification for
$!	its use.  Beware, however, that a bad Copy_To address can result in
$!	a looping condition caused by trying to copy Nmail error reports
$!	which were the result of a previously failed copy operation.
$!
$!	The limit on the number of versions of files created by this procedure
$!	is enforced through the use of SET FILE/VERSION_LIMIT and explicit
$!	PURGE commands.  The latter will display a message if any files are
$!	deleted, whereas the former will not.  The former also affects all
$!	creations of new versions of the file (e.g., editing of it) and is
$!	not restricted to operations that this procedure performs on it.
$!
$!	You may search the log file (if one was specified) to see if any new
$!	mail was found by issuing the DCL command:
$!		$ SEARCH <log-file> FROM:/WINDOW=(0,1)
$!
$!	If an error occurs while executing this procedure in batch mode, an
$!	attempt is made to mail you a message giving some indication of the
$!	problem.  Examining the log file (if one was specified, and if it has
$!	not been overwritten or deleted by subsequent batch jobs) may also
$!	be helpful.  You might also want to reread this documentation to help
$!	ensure that you are not doing something wrong.
$!
$!	One frequent cause of batch job failure is the presence of commands
$!	in the file(s) executed at login time (e.g., LOGIN.COM) that are not
$!	applicable to batch mode.  Such commands should be removed or executed
$!	conditionally (e.g., IF F$MODE() .NES. "BATCH" THEN ...).  A warning
$!	mail message cannot be sent for such failures, because the failure
$!	occurs before this procedure is executed.
$!
$!	Errors while extracting information about new mail from the mail
$!	file are ignored, except for logging some information about the
$!	problem.  If necessary information is lacking, the message is left
$!	unread (and not replied to).  Earlier versions of this procedure
$!	were deliberately coded to crash in such cases, in hopes of quickly
$!	identifying a need to update the procedure to handle a presumably
$!	new mail file format.  However, because some unrecognized fields
$!	have occasionally been encountered, a more defensive approach has
$!	instead been taken.
$!
$!	Some problems (e.g., errors decoding records in the mail file) may not
$!	terminate the execution of this procedure, but they are nevertheless
$!	considered serious.  In such cases, some information is maintained
$!	in a separate file for later use in debugging such problems.  If any
$!	such problem reports exist when running the procedure interactively,
$!	you will be asked if you wish to continue, as doing so may cause
$!	this and other information to be lost.  Also, if any problem reports
$!	exist when the procedure exits, and if it has not resubmitted itself,
$!	and if the reports are dated after the time of the last interactive
$!	session, you will be warned that they exist.  These reports are
$!	primarily for the maintainers of this procedure, whom you should
$!	notify of their existence.  If you wish to examine them yourself,
$!	use DUMP/RECORD, as they may contain binary data.  Alternatively,
$!	SEARCH them for "Problem:".  Also note that an initial version
$!	limit of five (5) is established when the first such file is created,
$!	regardless of the version limit you specified during the interactive
$!	session.  Once the file exists, you may manually change the limit
$!	(using SET FILE/VERSION_LIMIT), in order to retain more information.
$!	And after the problems are resolved, the problem reports should be
$!	deleted (or renamed) so that you do not continue to be reminded of
$!	their existence.
$!
$!	Errors while attempting to update the history file do not cause
$!	termination of the procedure.  However, any remaining replies that
$!	are generated during that same scan of the new mail will not be
$!	recorded in the history file.
$!
$!	The procedure, if not aborted manually or by a system failure, will
$!	remind you to check your WATCH_MAIL mail folder whenever a batch job
$!	had been queued and a new one is not submitted.  Normally this will
$!	be because the stop time has been reached, but it can also occur if
$!	the procedure finds and deletes (at your request) an existing job
$!	and does not start a new one or if the procedure gets an error while
$!	running in batch mode that prevents it from continuing.
$!
$!	The factory default time list is dependent on the time when the
$!	interactive session is started, as an attempt to keep multiple batch
$!	jobs from different users from being queued to run at the same times,
$!	which might result in an abnormal load on the system.
$!
$!	Normally the batch job should run at the times specified.  However,
$!	if the system is down at the time the job is supposed to run, the
$!	job will run as soon as the system is brought back up and the queue
$!	it is in is started.  Also, if a job would be requeued for a time
$!	that is too close to the current time, a minimum time interval will
$!	instead be used.  This is to avoid problems with having two jobs
$!	running at once, which would result in premature termination of the
$!	job and subsequent loss of mail replies.  The minimum time interval
$!	is less than an hour, and hopefully adequate to allow a job processing
$!	a lot of mail and running at a low priority enough time to complete
$!	before its successor starts.
$!
$!	This procedure reads your mail file directly, with access by the MAIL
$!	utility and other software allowed at the same time.  It is therefore
$!	dependent on the internal format of the mail file, and may not work
$!	correctly in future releases of VMS.  Since the procedure only reads
$!	the mail file, it cannot corrupt it.  A change to the format would
$!	probably cause the procedure to abort, though it's possible it might
$!	reply to, copy, and move the wrong messages depending on the nature
$!	of the change.
$!
$!	The procedure only processes new (i.e., unread) mail in the NEWMAIL
$!	folder.  If, for example, you refile messages into the NEWMAIL folder,
$!	they will be ignored.
$!
$!	The procedure assumes (normally safely) that a sequential read of
$!	new records in the NEWMAIL folder will be in chronological order, and
$!	that it can therefore determine what message numbers to supply to
$!	MAIL to have it read them later.  If one or more records are out of
$!	order, the procedure will log information about each one in a problem
$!	report and leave ALL messages unread (and not replied to), effectively
$!	disabling the functioning of the procedure.
$!
$!	You may place this procedure in a private or shared directory.  The
$!	advantage of a shared location is that it allows multiple people to
$!	use the procedure without incurring the disk space costs of multiple
$!	copies.  The procedure provides several features for facilitating the
$!	use and management of such a shared copy.
$!
$!	Whenever the procedure is invoked, either interactively or in batch
$!	mode, you will be notified if it appears to differ from the version
$!	that was previously used in batch mode.  This is to alert you to the
$!	fact that new features, etc. may be available.  You will get notified
$!	at the time of the interaction, as well as by mail during the initial
$!	execution of the batch job.  When this occurs, you may wish to consult
$!	the edit history and other comments within the procedure.
$!
$!	When the procedure (re)submits itself to the specified batch queue,
$!	the submitted version is the highest numbered one that exists at the
$!	time of submission (vs. using the same version as the submitter).
$!	This is done to facilitate the transition to a new version, and should
$!	not cause any problems as long as the resubmitted version does not
$!	introduce any new bugs and is compatible with the submitting version.
$!	You will receive a message whenever such a change occurs, just in case
$!	a problem should arise.
$!
$!	When phasing in a new version of this procedure, you must ensure that
$!	the older version is not deleted while there are jobs in the batch
$!	queue(s) that refer to it.  If this precaution is not taken and such
$!	jobs exist, the jobs will abort the next time they run and their log
$!	files (if present) will mention an "Error opening primary input file
$!	SYS$INPUT" and "File not found".  The way to check for such jobs is
$!	to invoke the version of the procedure file that you wish to delete
$!	with an argument of "JOBS" (e.g., @WATCH_MAIL;-1 JOBS) and see what
$!	it finds.  Jobs for which the specified start time has been reached
$!	will start using the new version within twenty-four hours, since they
$!	resubmit themselves at least once a day.  However, jobs may be queued
$!	to run well in advance of their start time, so merely waiting a day
$!	before deleting an older version will not ensure that there are no
$!	references to it.
$!
$!	Use of the JOBS option may result in security alarms for a process
$!	which lacks read access to queued procedure files.  While not harmful
$!	to the functioning of this procedure, such alarms may be a nuisance
$!	from a system management perspective.  Alternatives are to run the
$!	procedure from an account with more privileges or to manually peruse
$!	the output of  SHOW QUEUE/BATCH/ALL  to determine if any other users
$!	have queued the procedure of interest.
$!
$!	It is possible to "fake" an interactive session with this procedure
$!	from batch mode, in order to create a new job and related files,
$!	cancel or restart an existing job, etc.  For example, a system
$!	manager (using SUBMIT/USER) might create or restart jobs on behalf
$!	of other users.  To do so, submit another procedure that invokes
$!	this one with the desired mode argument (e.g., @WATCH_MAIL RESTART),
$!	and follow the invocation with data lines containing the answers
$!	for the expected interactive dialog.  Answer lines that begin with
$!	an exclamation point (!) will be ignored, but all other lines,
$!	including blank ones, will be treated as responses.  Note that
$!	wherever a YES/NO question would have been asked while running
$!	interactively, a "YES" is instead assumed while running in batch
$!	mode.  Also note that error handling and indication is somewhat
$!	unpredictable in this simulated interactive mode, due to its
$!	schizophrenic nature.  The procedure's exit status should indicate
$!	failure when an error occurs, but you may have to closely examine
$!	the log file for your procedure to determine the nature of the
$!	problem.

$!
$! EDIT HISTORY:
$!
$!	Author:
$!		DS	Dan Schullman
$!
$!	Modifiers (in alphabetical order by last name):
$!		WCM	Walter C. Miller
$!		MM	Martin Minow
$!		DM	Dick Munroe
$!
$!	Modifications:
$!	  04.00	 6NOV94	DS	Remove copyright for distribution as freeware.
$!				Add list of known restrictions, other comments.
$!	  03.06	18OCT91	DS	Changed version format to use double digits.
$!				Include problem dates in log file.
$!	  3.5	28JUL91 DS	Avoid crashing on unknown MAIL record fields,
$!				  and log the errors in a problem report.
$!				Allow type 4, 6, and 7 mail record fields.
$!				Alphabetized modifiers by last name.
$!				Added procedure name to aborted message.
$!				Changed forced error status to be severe.
$!				Changed to enable verification with a logical.
$!				Changed to initially enable symbol scoping.
$!				Ignore old messages in new mail folder.
$!				Require new mail records to be in order,
$!				  and log discrepancies in a problem report.
$!				Use F$GETQUI to get and check queue information.
$!				Claimed authorship (actually dates from V2.0).
$!				Added comments about security alarms with JOBS
$!				  and modified the code to try more privileges.
$!	  3.4	01OCT89	DS	Added /NOSELF and /NOCC when forwarding mail.
$!				Don't prepend auto-copy subject to itself.
$!				Removed /QUEUE from DELETE/QUEUE/ENTRY.
$!				Use file ID's (vs. specs) when comparing procs.
$!				Changed to allow a logical name for queue name.
$!	  3.3	03SEP88	DS	Changed to allow longer addresses and subjects.
$!				Added comment about parsing of QI_4 line.
$!				Added security-related code and comments.
$!				Fixed routing-removal and exclusion-checking
$!				  code to handle quoted addresses.
$!				Don't mention Copy_To verification if an error.
$!				Save image (as well as proc) verification state.
$!				Added weekday to display of resultant time.
$!				Changed to resubmit highest (vs. same) version.
$!				Allow factory-default syntax in parameter file.
$!				Save invocation time in parameter file.
$!				Allow specificaton of "Every N" for hour list.
$!				Made some minor code and comment cleanups.
$!				Changed most cluster references to node refs.
$!				Changed to allow times other than hours, and
$!				  renamed Hour_List to Time_List.
$!				Determine verify symbol name from proc spec.
$!				Allow job creation, etc. from batch mode.
$!				Check for valid YES/NO responses.
$!				Notify user if using a different procedure.
$!				Allow job to process mail near stop time.
$!				Fix removal of personal name from quoted addr.
$!				Reduce verification of queue-checking code.
$!				Include an invalid queue name in error message.
$!				Make saved invocation time closer to actual.
$!	  3.2	21JUN88 WCM	Fixed a problem caused by addition of "File: "
$!				  prefix in batch queue listing for SDC VMS V5.
$!	  3.1	05APR88	DS	Segment list of batch queues to avoid overflow.
$!	  3.0	03APR88	DS	Allow use with VMS V5.
$!				Shorten file-searching code.
$!	  2.8   28JAN88 WCM     Fixed a problem if Nmail didn't exist.
$!	  2.7	21NOV87 DM	Allow wildcarded name or node in exclude file.
$!	  2.6	04SEP87 DS	Fix problem with not finding batch job in queue.
$!				Don't mention Copy_To verification if deleting.
$!	  2.5	13JUL87	DS	Do not reply to a "postmaster" address.
$!	  2.4	24JUN87	DS	Added /EDIT syntax to edit/create input files.
$!				Changed the ordering of parameter solicitation.
$!				Added positive confirmation of value deletion.
$!				Added syntax to give general instructions.
$!				Submit first job for immediate execution.
$!				Perform first scan of mail at a specified hour.
$!				Rearranged and expanded header comments.
$!				Ignore unrecognized queue information.
$!				Tell user to verify Copy_To address(es).
$!	  2.3	16JUN87	DS	Made Version_Limit a parameter, and changed
$!				  default to one (1).
$!				Changed the ordering of parameter solicitation.
$!				No longer default output files to version ";1".
$!				Added some logicals to provide factory defaults.
$!	  2.2	07JUN87	DS	Allow multiple parameter files.
$!				Use SET FILE/VERSION to control versions.
$!				Include start and stop time in reply subjects.
$!				Allow deletion if higher output versions.
$!				Generate warning if higher input versions.
$!				Added CANCEL argument to cancel existing jobs.
$!				Display procedure arguments in batch or debug.
$!				Remind user to check mail if job not requeued.
$!				Include param file spec in history file.
$!				Show all symbols if error while batch or debug.
$!				Disallow remote execution.
$!				Provide online help capability.
$!				Don't requeue too close to current time.
$!				Added JOBS argument to list jobs on system.
$!				Miscellaneous cleanups to code and comments.
$!	  2.1	19MAY87	DS	Fixed test of error messages containing quotes.
$!				Fixed so history file not created if unwanted.
$!				Fixed truncation of time values.
$!				Fixed bug in generating Nmail prefix,
$!				  and changed algorithm for detecting Nmail.
$!				Check batch queues for job name as well as spec,
$!				  and properly handle rooted directory specs.
$!				Use SET SYMBOL/SCOPE to avoid user symbols.
$!				Expand various comments, and paginate file.
$!				Log deleted files when exiting batch mode.
$!				Include letter and baselevel in version code.
$!				Only show resultant spec if spec changes.
$!				Allow parameters to have quotes in them.
$!				Check for higher versions on output files.
$!				Changed forced error status to %X0800002A.
$!				Don't allow mail processing prior to start time.
$!				Compare stop time with start time in batch mode.
$!				Moved edit history after documentation.
$!				Moved version number to beginning of procedure.
$!				Changed names of error-reporting subroutines,
$!				  allow inclusion of a severity code,
$!				  and allow use when running interactively.
$!				Use /RESTART qualifier with the SUBMIT command.
$!				Delete orphaned temporary files.
$!				Postpone generation of param list and defaults.
$!				Check for node as well as cluster address.
$!				Ignore routing nodes on address comparisons.
$!				Do not include routing information in history.
$!				Mail reminder to user if batch job not requeued.
$!				Shorten mail scan by assuming folders in order.
$!				Made check for mailer daemons more generic.
$!				Don't ignore errors while reading param file.
$!				Increased use of substitution for debugging
$!				  and to reduce size of log files.
$!				Display sender and subject in batch mode.
$!				Allow comments in exclude file.
$!				Created symbols for various folder names.
$!				Created subroutine for constants used in errors.
$!				Revised instructions for entering parameters.
$!				Disable some verification in batch mode.
$!				Mention creation of history file.
$!				Check procedure arguments (P1 to P8).
$!				Added restart capability.
$!				Use /NOLOG qualifier with CLOSE.
$!				Create temp command file only if new mail found.
$!				Search history file before exclude file.
$!				Preserve entered value for Stop_Time.
$!				Allow weekday names as valid time values.
$!				Display resulting start and stop times.
$!	  2.0	16MAY87	DS	Complete rewrite of documentation and code!.
$!				Ask for parameters (vs. editing this file),
$!				  and save and read them from a parameter file.
$!				Validate input parameters.
$!				Allow/force existing job to be deleted.
$!				Added Copy_To feature.
$!				Added error-reporting subroutines,
$!				Eliminated spaces at beginning of lines.
$!				Converted code to upper case.
$!				Branch around header comments.
$!				List recent modifications first.
$!				Changed procedure verification logic.
$!				Shortened node-determination logic.
$!				Create symbols for temporary files.
$!				Determine mail file by invoking MAIL.
$!				Reduce frequent use of ON command.
$!				Prefix all logical names with "WM_".
$!				Use subject symbol for modularity.
$!				Do not use SET MESSAGE to suppress output.
$!				Change NL: to _NLA0:.
$!				Make default hour list time dependent.
$!	  1.5	06MAY87	WCM     Fixed problem with long sender names (ARPAnet).
$!				Read MAIL.MAI directly.
$!				Use Nmail automatically if available.
$!				Avoid use of old history file.
$!				Add name to history only after successful reply.
$!				Don't reply to Nmail or DECWRL daemons.
$!				Added exclusion ability.
$!				No longer required to submit.
$!				Use cluster node name on a cluster.
$!	  1.4	19MAR87	DS	Fixed F$SEARCH's for deleting .OUT files.
$!	  1.3	29MAY86	MM	Added history, Nmail support.
$!	  1.2	17SEP85	WCM	Removed looping if replying to WATCH_MAIL reply.
$!

$!=============================================================================
$!
$! Common initialization code for all modes of operation.
$!
$!-----------------------------------------------------------------------------
$END_OF_HEADER_COMMENTS:
$!
$! Remember approximate invocation time, in full and truncated to minutes.
$!
$	INVOKED = "''F$EDIT( F$TIME(), "TRIM" )'"
$	NOW = "''F$EXTRACT( 0, F$LOCATE(":",INVOKED)+3, INVOKED )'"
$!
$! Go define some constants that may be used in error handling, and also
$! disable the use of externally-defined symbols.
$!
$	GOSUB DEFINE_CONSTANTS
$!
$! Get information about this procedure, and make sure that it is not
$! invoked remotely (because the resubmissions won't work).  The format
$! of the procedure information must not be changed, as it is used by
$! subsequent invocations (and possibly also versions) of the procedure.
$!
$	IF F$PARSE(PROC_FILE,,,"NODE") .EQS. "" THEN GOTO INVOKED_LOCALLY
$	ERROR_MESSAGE = "E-NOTLOCAL, this procedure must be invoked locally."
$	GOTO SEND_MESSAGE_AND_ABORT
$INVOKED_LOCALLY:
$	PROC_INFO = "''THIS_NODE'''PROC_FILE' -- ''VERSION'"
$!
$! Check arguments to the procedure (i.e., P1 to P8), while preserving their
$! original alphabetic case for possible error display.  Some may already have
$! been checked when determining the operational mode.
$!
$	PARAMETER_FILE = DEFAULT_PARAMETER_FILE
$	SPEC = F$ELEMENT(1," ",ARGS)
$	PARSED_SPEC = "''F$PARSE(SPEC,DEFAULT_PARAMETER_FILE,,,"SYNTAX_ONLY")'"
$	IF SPEC .NES. "" THEN IF PARSED_SPEC .NES. "" THEN GOTO POSSIBLE_SPEC
$	IF MODE .NES. "BATCH" THEN GOTO CHECKED_PARAM_SPEC	!ok if absent
$	ERROR_MESSAGE = -
	  "E-NOPSPEC, file specification for parameter file is missing."
$	GOTO SEND_MESSAGE_AND_ABORT
$POSSIBLE_SPEC:
$	DUMMY_DIR = "[NO-SUCH-DIRECTORY]"
$	IF "''F$PARSE(SPEC,DUMMY_DIR,,"DIRECTORY")'" .EQS. DUMMY_DIR -
	  THEN IF F$SEARCH(PARSED_SPEC) .EQS. "" THEN GOTO CHECKED_PARAM_SPEC
$	PARAMETER_FILE = PARSED_SPEC
$	ARGS = F$EDIT(ARGS-SPEC,"COMPRESS")
$CHECKED_PARAM_SPEC:
$!
$	VERSION_KEYWORD = "Ver="
$	POS = 'F$LOCATE(VERSION_KEYWORD,ARGS)'
$	STRING = F$ELEMENT(0," ",F$EXTRACT(POS,999,ARGS))
$	SUBMITTED_BY_VERSION = "''F$EDIT(STRING-VERSION_KEYWORD,"UPCASE")'"
$	ARGS = F$EDIT(ARGS-STRING,"COMPRESS")
$!
$	ARGS = F$EDIT(ARGS,"TRIM")
$	IF ARGS .EQS. "" THEN GOTO ARGS_OK
$	ERROR_MESSAGE = "E-BADARGS, invalid procedure arguments (" + ARGS + ")."
$	GOTO SEND_MESSAGE_AND_ABORT
$ARGS_OK:
$!
$! Define (for various uses) a character not apt to be found in text strings.
$!
$	NUL[0,8] = 0
$!
$! Determine "full" procedure specification, translating concealed logicals
$! and eliminating any reference to a master or rooted directory.
$!
$	FULL_PROC_SPEC = "_" + NUL + "''F$PARSE(PROC_FILE,,,,"NO_CONCEAL")'" -
	  - ( NUL + "_" ) - NUL - ".][000000" - "]["
$!
$! Get the file ID for the procedure.
$!
$	PROC_FID = "''F$FILE_ATTRIBUTES ( FULL_PROC_SPEC, "FID" )'"
$!
$! If using a different version of the procedure than was previously used,
$! notify the user.  The comparison is based on the file ID, rather than the
$! procedure specification, since disc controller changes can result in a
$! different fully-translated spec.  A partially-translated specification
$! cannot be used because of differences between interactive and batch mode,
$! etc.
$!
$	PREVIOUS_INFO = ""
$	PREVIOUS_FID = ""
$	CURRENT_INFO = FULL_PROC_SPEC + " -- " + VERSION
$	OPEN/READ/ERROR=USING_DIFFERENT_PROC WM_VERSION 'VERSION_FILE'
$	READ/ERROR=USING_DIFFERENT_PROC WM_VERSION PREVIOUS_INFO
$	READ/ERROR=USING_DIFFERENT_PROC WM_VERSION PREVIOUS_FID
$	CLOSE WM_VERSION
$	IF PROC_FID .EQS. PREVIOUS_FID THEN GOTO USING_SAME_PROC
$USING_DIFFERENT_PROC:
$	CLOSE WM_VERSION
$	OPEN/WRITE WM_MESSAGE 'MESSAGE_FILE'
$	WO = "WRITE WM_MESSAGE"
$	W = "''WO' ""  "","
$	W "The version of ''PROC_NAME' that is currently executing appears to"
$	IF PREVIOUS_INFO .NES. "" THEN GOTO NOT_FIRST_USAGE
$	W "differ from the previously used version, or this is the first time"
$	W "that you have used the procedure.  If you are not a new user, you"
$	W "may wish to examine the edit history within the procedure to see"
$	W "what has changed."
$	WO ""
$	GOTO FIRST_USAGE
$NOT_FIRST_USAGE:
$	W "differ from the previously used version.  Assuming that it is"
$	W "compatible with the previously used version and that it does not"
$	W "introduce any new bugs, the changeover should not cause you any"
$	W """surprises"".  You may wish to examine the edit history within"
$	W "the procedure to see what has changed."
$	WO ""
$	WO "Previously used:  ''PREVIOUS_INFO'"
$FIRST_USAGE:
$	WO "Currently using:  ''CURRENT_INFO'"
$	CLOSE WM_MESSAGE
$	SPEC = MESSAGE_FILE
$	ERROR_MESSAGE = -
	  "I-DIFFPROC, using a different version of the procedure."
$	GOSUB SEND_MESSAGE_AND_FILE
$	ERROR_MESSAGE = ""		!reset error flag in case of a real one
$	IF F$MODE() .NES. "BATCH" THEN GOTO DO_NOT_UPDATE_PREVIOUS_INFO
$	SPEC = VERSION_FILE
$	VERSION_LIMIT = "1"
$	GOSUB CONTROL_VERSIONS
$	OPEN/WRITE WM_VERSION 'VERSION_FILE'
$	WRITE WM_VERSION CURRENT_INFO
$	WRITE WM_VERSION PROC_FID
$	CLOSE WM_VERSION
$DO_NOT_UPDATE_PREVIOUS_INFO:
$USING_SAME_PROC:
$!
$! Check the batch queues to see what other WATCH_MAIL jobs might be queued,
$! either by using the F$GETQUI function or by "parsing" the output from the
$! SHOW QUEUE command.  If the latter, also build a list of available queues
$! for later validation.
$!
$! NOTE that the parsing of the SHOW QUEUE output is very dependent on the
$! formatting, and keys off of the number of leading spaces as indicated
$! below.  Unrecognized lines are trapped via error handling and result in
$! some error messages being displayed, but are otherwise ignored.
$!
$!	 0	name of batch queue, or null line
$!	 2	job name (possibly followed by 18-space info on same line)
$!	 4	procedure specification preceded by "File: " (VMS V5 and later)
$!	 6	procedure specification
$!	18	user, entry, and status (on continuation line)
$!
$	BDISPLAY "Checking batch queues for ", -
	  "''F$ELEMENT(MODE.EQS."JOBS","/","an/all")' existing ", -
	  "''PROC_NAME' job''F$EXTRACT(0,MODE.EQS."JOBS","s")' on ''NOW'."
$	IF MODE .NES. "JOBS" THEN GOTO PRIVILEGES_OK
$	PRIVS = "OPER"
$	SAVE_PRIVS = "''F$SETPRV(PRIVS)'"
$	IF F$PRIVILEGE(PRIVS) THEN GOTO PRIVILEGES_OK
$	ERROR_MESSAGE = -
	  "E-NOPRIV, lacking necessary privileges (" + PRIVS + ")."
$	GOTO SEND_MESSAGE_AND_ABORT
$PRIVILEGES_OK:
$	JOBS_FOUND = 0
$	JOBS_REMAINING = 0
$	IF VMS_VERSION .GES. "5.0" THEN GOTO USE_GETQUI
$!
$	QUEUE_TEXT = "atch queue "	!"Batch queue" or "Generic batch queue"
$	BQLN = 1			!batch queue list number
$	BQLE = ","			!batch queue list entry
$	OPTIONS = "''F$ELEMENT( MODE.EQS."JOBS", "|", "|/ALL_ENTRIES" )'"
$	SHOW QUEUE/BATCH/FILES'OPTIONS'/OUTPUT='BATCHINFO_FILE'
$	OPEN/READ WM_BATCHINFO 'BATCHINFO_FILE'
$	READ_QO = "READ/END=QO_END WM_BATCHINFO LINE"
$	NEXT_JOB = "QO"
$	GOTO QO_ON
$QO_BAD:
$	STATUS = $STATUS
$	ERROR = F$MESSAGE(STATUS)
$	IF ERROR - "-USGOTO," .EQS. ERROR THEN GOTO ABNORMAL_EXIT_2
$	STRING = "-''PROC_NAME'-I-QINFO, "
$	DISPLAY STRING, -
	  F$FAO("unrecognized queue information with !SL leading space!%S",L_SP)
$	DISPLAY STRING,"  <" + F$EDIT(LINE,"TRIM") + ">."
$QO_ON:
$	ON WARNING THEN GOTO QO_BAD
$QO:
$	READ_QO
$	L_SP = 'F$LENGTH(LINE+NUL)' - 'F$LENGTH(F$EDIT(LINE+NUL,"TRIM"))'
$	GOTO QO_'L_SP'
$QO_0:
$	IF 'F$STRING( LINE - QUEUE_TEXT .EQS. LINE )' THEN GOTO QO
$	POS = 'F$STRING( F$LOCATE(QUEUE_TEXT,LINE) + F$LENGTH(QUEUE_TEXT) )'
$	QUEUE_NAME = "''F$ELEMENT(0,",",F$EXTRACT(POS,999,LINE))'"
$	GOTO BQLE_'F$ELEMENT(F$LENGTH(BQLE+QUEUE_NAME).LT.255,"|","FULL|OK")'
$BQLE_FULL:
$	BQLE_'BQLN' = BQLE
$	BQLN = 'F$STRING( BQLN + 1 )'
$	BQLE = ","
$BQLE_OK:
$	BQLE = BQLE + QUEUE_NAME + ","
$	GOTO QO
$QO_2:
$	JOB_NAME = "''F$EDIT(F$EXTRACT(0,18,LINE),"TRIM")'"
$	INFO = LINE
$	GOTO QO
$QO_18:
$	JOB_NAME = "''F$EDIT(INFO,"TRIM")'"
$	INFO = INFO + LINE
$	GOTO QO
$QO_4:
$	LINE = LINE - "File: "
$QO_6:
$	INFO = F$EDIT(INFO-JOB_NAME,"TRIM")
$	USER = F$ELEMENT(0," ",INFO)
$	ENTRY = "''F$ELEMENT(0," ",F$EDIT(INFO-USER,"TRIM"))'"
$	STATUS = F$EDIT(INFO-USER-ENTRY,"TRIM,COMPRESS")
$	FULL_JOB_SPEC = -
	  "''F$PARSE(F$ELEMENT(0," ",F$EDIT(LINE,"TRIM")),,,,"SYNTAX_ONLY")'"
$	GOTO GOT_JOB
$QO_END:
$	ON WARNING THEN GOTO ABNORMAL_EXIT
$	CLOSE WM_BATCHINFO
$	BQLE_'BQLN' = BQLE
$	GOTO NO_MORE_JOBS
$!
$USE_GETQUI:
$	DQ = "DISPLAY_QUEUE"
$	DJ = "DISPLAY_JOB"
$	DF = "DISPLAY_FILE"
$	DQO = "BATCH"
$	DQOF = DQO + ",FREEZE_CONTEXT"
$	DJO = "''F$ELEMENT( MODE.EQS."JOBS", "|", "|ALL_JOBS" )'"
$	DJOF = "''F$STRING(DJO+F$EXTRACT(0,DJO.NES."",",")+"FREEZE_CONTEXT")'"
$	OPEN/WRITE WM_BATCHINFO 'BATCHINFO_FILE'
$	WRITE_JI = "WRITE WM_BATCHINFO"
$	NEXT_JOB = "JI"
$	GQC = F$GETQUI("")
$QI:
$	QUEUE_NAME = "''F$GETQUI(DQ,"QUEUE_NAME","*",DQO)'"
$	IF 'F$STRING( QUEUE_NAME .EQS. "" )' THEN GOTO QI_END
$	GENERIC = 'F$STRING( F$GETQUI(DQ,"QUEUE_GENERIC","*",DQOF) .AND. 1 )'
$	QSHOW = "''F$ELEMENT(GENERIC,"|","B|Generic b")'" + "atch queue " -
	  + QUEUE_NAME
$JI:
$	INACCESSIBLE = "''F$GETQUI(DJ,"JOB_INACCESSIBLE",,DJO)'"
$	IF 'F$STRING( INACCESSIBLE .EQS. "" )' THEN GOTO QI
$	IF 'F$STRING( INACCESSIBLE .AND. 1 )' THEN GOTO JI
$	JOB_NAME = "''F$GETQUI(DJ,"JOB_NAME",,DJOF)'"
$	USER = "''F$GETQUI(DJ,"USERNAME",,DJOF)'"
$	ENTRY = "''F$GETQUI(DJ,"ENTRY_NUMBER",,DJOF)'"
$	VS = 'F$VERIFY(0)' + F$VERIFY(0'VERIFY_VAR' .GT. 0)	!usually off
$	STATUS = "(unknown status)"
$	IF F$GETQUI(DJ,"JOB_ABORTING",,DJOF)	THEN STATUS = "Aborting"
$	IF F$GETQUI(DJ,"JOB_EXECUTING",,DJOF)	THEN STATUS = "Executing"
$	IF F$GETQUI(DJ,"JOB_HOLDING",,DJOF)	THEN STATUS = "Holding"
$	IF F$GETQUI(DJ,"JOB_PENDING",,DJOF)	THEN STATUS = "Pending"
$	IF F$GETQUI(DJ,"JOB_REFUSED",,DJOF)	THEN STATUS = "Refused"
$	IF F$GETQUI(DJ,"JOB_RETAINED",,DJOF)	THEN STATUS = "Retained"
$	IF F$GETQUI(DJ,"JOB_STARTING",,DJOF)	THEN STATUS = "Starting"
$	IF F$GETQUI(DJ,"JOB_SUSPENDED",,DJOF)	THEN STATUS = "Suspended"
$	IF F$GETQUI(DJ,"JOB_TIMED_RELEASE",,DJOF) -
	  THEN STATUS = "Holding until " -
	    + F$EDIT(F$EXTRACT(0,17,F$GETQUI(DJ,"AFTER_TIME",,DJOF)),"TRIM")
$	V = 'F$VERIFY(0)' + F$VERIFY(VS)	!restore verify
$	STATUS = "''STATUS'"
$	FULL_JOB_SPEC = -
	  "''F$PARSE(F$GETQUI(DF,"FILE_SPECIFICATION"),,,,"SYNTAX_ONLY")'"
$	IF 'F$STRING( QSHOW .EQS. "" )' THEN GOTO NOT_FIRST_QUEUE_ENTRY
$	WRITE_JI ""
$	WRITE_JI QSHOW
$	QSHOW = ""
$NOT_FIRST_QUEUE_ENTRY:
$	WRITE_JI F$FAO("  !5UL  !AS  !AS  !AS",0+ENTRY,JOB_NAME,USER,STATUS)
$	WRITE_JI F$FAO("!11* !AS",FULL_JOB_SPEC)
$	GOTO GOT_JOB
$QI_END:
$	CLOSE WM_BATCHINFO
$	GQC = F$GETQUI("")
$	GOTO NO_MORE_JOBS
$!
$! Got batch job information.  If the job appears to specify this procedure
$! or a variation of it, determine what to do with it based on the operational
$! mode and possible input from the user.  Attempt to compare the current and
$! queued procedures based on their file ID's, since disc controller changes
$! result in a different fully-translated spec.  Note that the attempt to get
$! the file ID may result in a privilege error and corresponding security alarm
$! if the process lacks read access to the procedure file, especially when the
$! JOBS option is specified.  Such errors do not affect this procedure.
$!
$GOT_JOB:
$	DEFINE/USER SYS$OUTPUT _NLA0:
$	DEFINE/USER SYS$ERROR _NLA0:
$	SET NOON
$	SP = F$SETPRV("SYSPRV,GRPPRV,READALL,BYPASS")
$	JOB_FID = ""
$	JOB_FID = "''F$FILE_ATTRIBUTES( FULL_JOB_SPEC, "FID" )'"
$	IF 'F$STRING(JOB_FID.EQS.PROC_FID)' THEN FULL_JOB_SPEC = FULL_PROC_SPEC
$	P = "''F$SETPRV(SP)'"
$	SET ON
$	DEASSIGN/USER SYS$OUTPUT
$	DEASSIGN/USER SYS$ERROR
$	JOB_SPEC  = "''F$PARSE(";",FULL_JOB_SPEC,,,"SYNTAX_ONLY")'"
$	PROC_SPEC = "''F$PARSE(";",FULL_PROC_SPEC,,,"SYNTAX_ONLY")'"
$	IF JOB_NAME .NES. PROC_NAME THEN IF JOB_SPEC .NES. PROC_SPEC -
	  THEN GOTO 'NEXT_JOB'
$	IF STATUS - "Aborting" .NES. STATUS THEN GOTO 'NEXT_JOB'
$	IF MODE .NES. "JOBS" THEN GOTO CHECK_LESS_INFO
$	PROC_SPEC = FULL_PROC_SPEC
$	JOB_SPEC = FULL_JOB_SPEC
$CHECK_LESS_INFO:
$	BDISPLAY -
	  "A ''F$ELEMENT(JOB_NAME.NES.PROC_NAME,NUL,NUL+JOB_NAME+" ")'", -
	  "job ''F$ELEMENT(MODE.EQS."JOBS",NUL,NUL+"for "+USER+" ")'", -
	  "is in queue ",QUEUE_NAME," as entry ",ENTRY
$	IF JOB_SPEC .EQS. PROC_SPEC THEN GOTO SAME_JOB_SPEC
$	DISPLAY "  with a status of """,STATUS,""", but with a different"
$	DISPLAY "  file specification (",JOB_SPEC,")."
$	OPTIONS = "You may choose to delete it or continue anyway."
$	GOTO SEE_ABOUT_DELETING_JOB
$SAME_JOB_SPEC:
$	DISPLAY "  with a status of """,STATUS,"""."
$	OPTIONS = -
	  "You must choose to delete it or terminate the current interaction."
$SEE_ABOUT_DELETING_JOB:
$	IF MODE .EQS. "BATCH" .OR. MODE .EQS. "JOBS" THEN GOTO JOB_REMAINS
$	IF MODE .NES. "CANCEL" THEN DISPLAY OPTIONS
$	PROMPT = "Do you wish to delete the existing job"
$	GOSUB READ_YES_NO
$	IF .NOT. YES THEN GOTO JOB_REMAINS
$	DELETE_ENTRY = -
	  "DELETE''F$ELEMENT(VMS_VERSION.LTS."5.0","|","|/QUEUE")'/ENTRY="
$	DELETE_ENTRY 'ENTRY' 'QUEUE_NAME'
$	SUBMITTED = -1			!remember job cancelled
$	GOTO IGNORE_JOB
$JOB_REMAINS:
$	JOBS_REMAINING = 'F$STRING( JOBS_REMAINING + (JOB_SPEC.EQS.PROC_SPEC) )'
$IGNORE_JOB:
$	JOBS_FOUND = 'F$STRING( JOBS_FOUND + 1 )'
$	GOTO 'NEXT_JOB'
$!
$NO_MORE_JOBS:
$!
$! If invoked to cancel an existing job, see if one was found.  If in batch
$! mode, make sure there is only ourself in the queues, but as a sanity check
$! of the search algorithm make sure that we do find ourself.  Otherwise,
$! abort the interaction if any jobs which are running the same procedure
$! remain.  Also exit if just looking for all batch jobs.
$!
$	IF MODE .NES. "CANCEL" THEN GOTO CHECK_JOBS_REMAINING
$	IF 'JOBS_FOUND' .EQ. 0 -
	  THEN BDISPLAY "No existing ''PROC_NAME' job found."
$	GOTO CORRECT_JOB_COUNT
$CHECK_JOBS_REMAINING:
$	OK = 'JOBS_REMAINING' - 'F$STRING( MODE.EQS."BATCH" )'
$	IF 'OK' .EQ. 0 THEN IF MODE .NES. "JOBS" THEN GOTO CORRECT_JOB_COUNT
$	STRING = ""
$	IF MODE .EQS. "BATCH" -
	  THEN STRING = "!SL other !AS batch job!%S !AS queued."
$	IF MODE .EQS. "JOBS" -
	  THEN STRING = "!SL batch job!%S !+!AS using " + PROC_FILE
$	IF STRING .EQS. "" THEN GOTO ABNORMAL_EXIT
$	ERROR_MESSAGE = F$FAO( -
	  F$EXTRACT(MODE.EQS."JOBS",1,"EI")+"-OTHERJOBS, "+STRING, -
	    OK, PROC_NAME, F$ELEMENT(OK.NE.1,"/","is/are") )
$	OK = 'F$STRING( JOBS_FOUND - JOBS_REMAINING )'
$	IF MODE .EQS. "JOBS" THEN ERROR_MESSAGE = ERROR_MESSAGE -
	  + F$FAO( "!/  and !SL other!%S !AS using a different procedure.", -
	    OK, F$ELEMENT(OK.NE.1,"/","is/are") )
$	SPEC = BATCHINFO_FILE
$	GOSUB SEND_MESSAGE_AND_FILE
$	IF MODE .NES. "JOBS" THEN GOTO ABNORMAL_EXIT
$	ERROR_MESSAGE = ""		!reset error flag in case of a real one
$	GOTO NORMAL_EXIT
$CORRECT_JOB_COUNT:
$!
$! If no problem reports exist, check for and delete temporary files left
$! behind by a "dead" job.
$!
$	IF F$SEARCH(PROBLEM_FILE) .NES. "" THEN GOTO KEEP_OLD_TEMP_FILES
$	ORPHANS_FOUND = 0
$FIND_NEXT_TEMP_FILE:
$	SPEC = "''F$SEARCH(ORPHAN_FILES)'"
$	IF SPEC .EQS. "" THEN GOTO NO_MORE_TEMP_FILES
$	FILE_PID = "''F$ELEMENT(2,"_",F$PARSE(SPEC,,,"NAME"))'"
$	IF FILE_PID .EQS. OUR_PID THEN GOTO FIND_NEXT_TEMP_FILE
$	DEFINE/USER SYS$OUTPUT _NLA0:
$	DEFINE/USER SYS$ERROR _NLA0:
$	SET NOON
$	LINE = F$GETJPI(FILE_PID,"PID")		!test process existence
$	OK = $STATUS
$	SET ON
$	DEASSIGN/USER SYS$OUTPUT
$	DEASSIGN/USER SYS$ERROR
$	IF OK THEN GOTO FIND_NEXT_TEMP_FILE
$	IF .NOT. ORPHANS_FOUND -
	  THEN BDISPLAY "Deleting temporary files left behind by aborted jobs."
$	ORPHANS_FOUND = 1
$	DELETE/LOG 'SPEC'
$	GOTO FIND_NEXT_TEMP_FILE
$NO_MORE_TEMP_FILES:
$KEEP_OLD_TEMP_FILES:
$!
$! Exit now if invoked merely to cancel an existing job.
$!
$	IF MODE .EQS. "CANCEL" THEN GOTO NORMAL_EXIT
$!
$! If not in real batch mode, check to see if any problem reports exist,
$! and if so, ask the user if it's okay to continue.  Be careful about access
$! conflicts, because an existing job may have the file open.
$!
$	IF F$MODE() .EQS. "BATCH" THEN GOTO DO_NOT_QUERY_ABOUT_PROBLEM_REPORTS
$	IF F$SEARCH(PROBLEM_FILE) .EQS. "" -
	  THEN GOTO DO_NOT_QUERY_ABOUT_PROBLEM_REPORTS
$	DEFINE/USER SYS$OUTPUT _NLA0:
$	DEFINE/USER SYS$ERROR _NLA0:
$	SET NOON
$	CDT = ""
$	CDT = ", the most recent dated " + F$EDIT( F$EXTRACT( 0, 17, -
	  "''F$FILE_ATTRIBUTES(PROBLEM_FILE,"CDT")'" ), "TRIM" )
$	SET ON
$	DEASSIGN/USER SYS$OUTPUT
$	DEASSIGN/USER SYS$ERROR
$	BDISPLAY "One or more problem reports exist''CDT'."
$	DISPLAY "Running the procedure may cause this", -
	  " and other information to be lost."
$	PROMPT = "Do you wish to proceed"
$	GOSUB READ_YES_NO
$	IF .NOT. YES THEN GOTO ABNORMAL_EXIT
$DO_NOT_QUERY_ABOUT_PROBLEM_REPORTS:
$!
$! List of parameter values and question prompts (in solicitation order).
$!
$	PARAM_LIST = "" -
	  + "''F$ELEMENT(MODE.EQS."INTERACTIVE","/","/Parameter_File,")'" -
	  + "Time_List," -
	  + "Start_Time," -
	  + "Stop_Time," -
	  + "Copy_To," -
	  + "Version_Limit," -
	  + "Reply_File," -
	  + "Exclude_File," -
	  + "History_File," -
	  + "Log_File," -
	  + "Batch_Queue," -
	  + ""
$!
$! Generate "factory defaults" for input parameters.
$!
$	VALUE = "''F$EDIT(F$TRNLNM("WATCH_MAIL_DEFAULT_BATCH_QUEUE"),"TRIM")'"
$	DEFAULT_BATCH_QUEUE = -
	  "''F$ELEMENT( VALUE.EQS."", NUL, VALUE+NUL+"Sys$Batch" )'"
$	DEFAULT_COPY_TO = ""
$	DEFAULT_EXCLUDE_FILE = SPEC_PREFIX + "EXCLUDE.DAT;"
$	DEFAULT_HISTORY_FILE = SPEC_PREFIX + "HISTORY.DAT;"
$	VALUE = "''F$EDIT(F$TRNLNM("WATCH_MAIL_DEFAULT_TIME_LIST"),"TRIM")'"
$	DEFAULT_TIME_LIST = -
	  "''F$ELEMENT( VALUE.EQS."", NUL, VALUE+NUL+"Every 3" )'"
$	DEFAULT_LOG_FILE = "SYS$LOGIN:" + PROC_NAME + ".LOG;"
$	VALUE = "''F$EDIT(F$TRNLNM("WATCH_MAIL_DEFAULT_VERSION_LIMIT"),"TRIM")'"
$	DEFAULT_VERSION_LIMIT = "''F$ELEMENT( VALUE.EQS."",NUL,VALUE+NUL+"1" )'"
$	DEFAULT_REPLY_FILE = SPEC_PREFIX + "REPLY.TXT;"
$	DEFAULT_START_TIME = "Tomorrow"
$	DEFAULT_STOP_TIME = ""
$!
$! Define strings to be used in writing some of the "comments" in the parameter
$! file.  However, these strings must NOT be changed, as the code looks for the
$! comments which contain these strings... even in "ancient" parameter files.
$!
$	PARAM_PROC_KEYWORD = "! Parameters saved by "
$	PARAM_USER_KEYWORD = "! from information supplied by "
$!
$! Now dispatch according to mode of operation.
$!
$	GOTO 'MODE'

$!=============================================================================
$!
$! Interactive (and some Restart) code.
$!
$!-----------------------------------------------------------------------------
$INTERACTIVE:
$!
$! Initialize parameters to their "factory defaults", except for Parameter_File
$! which may already have been specified via a procedure argument.
$!
$	TEMP_LIST = PARAM_LIST - "Parameter_File,"
$GET_FACTORY_DEFAULT:
$	PARAM = "''F$ELEMENT(0,",",TEMP_LIST)'"
$	TEMP_LIST = TEMP_LIST - PARAM - ","
$	'PARAM' = DEFAULT_'PARAM'
$	IF TEMP_LIST .NES. "" THEN GOTO GET_FACTORY_DEFAULT
$!
$! Get new parameter values from the user.
$!
$	BDISPLAY "Enter ""?"" (a question mark) ", -
	  "to get help on entering parameter values."
$	DISPLAY ""
$	P_MODE = MODE
$	GOSUB GET_PARAMS
$!
$! Save new parameter values, except for Parameter_File.  Also save some
$! internal variables for use in batch and restart modes.
$!
$	SPEC = PARAMETER_FILE
$	GOSUB CONTROL_VERSIONS
$	BDISPLAY "Saving parameter values in ",PARAMETER_FILE
$	CREATE/FDL=SYS$INPUT 'PARAMETER_FILE'
FILE
	MAXIMIZE_VERSION	no
	SUPERSEDE		yes
$!
$	OPEN/APPEND WM_PARAMS 'PARAMETER_FILE'
$	WRITE WM_PARAMS "!"
$	WRITE WM_PARAMS PARAM_PROC_KEYWORD,PROC_INFO
$	WRITE WM_PARAMS PARAM_USER_KEYWORD,THIS_NODE_USER," on ",NOW,"."
$	WRITE WM_PARAMS "!"
$	WRITE WM_PARAMS "! NOTE that editing this file", -
	  " will affect subsequent runs of the procedure."
$	WRITE WM_PARAMS "!"
$	TEMP_LIST = PARAM_LIST - "Parameter_File," -
	  + "!,ABS_START_TIME,ABS_STOP_TIME,ABS_TIME_LIST,ORIGINALLY_INVOKED,"
$SAVE_NEXT_PARAM:
$	PARAM = "''F$ELEMENT(0,",",TEMP_LIST)'"
$	TEMP_LIST = TEMP_LIST - PARAM - ","
$	IF F$EXTRACT(0,1,PARAM) .EQS. "!" THEN GOTO SAVE_INFO
$	STRING = 'PARAM'
$	IF STRING - """" .NES. STRING THEN GOSUB DOUBLE_QUOTES
$	PARAM = PARAM + " = """ + STRING + """"
$SAVE_INFO:
$	WRITE WM_PARAMS PARAM
$	IF TEMP_LIST .NES. "" THEN GOTO SAVE_NEXT_PARAM
$	CLOSE WM_PARAMS
$!
$! If a history file is desired, create a new (empty) one.
$!
$	IF HISTORY_FILE .EQS. "" THEN GOTO DO_NOT_CREATE_HISTORY
$	SPEC = HISTORY_FILE
$	GOSUB CONTROL_VERSIONS
$	BDISPLAY "Creating history file ",HISTORY_FILE
$	CREATE/FDL=SYS$INPUT 'HISTORY_FILE'
FILE
	MAXIMIZE_VERSION	no
	SUPERSEDE		yes
$!
$	OPEN/APPEND WM_HISTORY 'HISTORY_FILE'
$	WRITE WM_HISTORY "!"
$	WRITE WM_HISTORY "! History created by ",PROC_INFO
$	WRITE WM_HISTORY "! for ",THIS_NODE_USER," on ",NOW
$	WRITE WM_HISTORY "! using parameters in ",PARAMETER_FILE
$	WRITE WM_HISTORY "!"
$	WRITE WM_HISTORY "! The projected expiration date is ",ABS_STOP_TIME,"."
$	WRITE WM_HISTORY "!"
$	CLOSE WM_HISTORY
$DO_NOT_CREATE_HISTORY:
$!
$! Submit the procedure to run in batch and do the "real" work.  Have it start
$! executing as soon as possible, to catch problems with LOGIN.COM, etc.
$!
$INTERACTIVE_SUBMIT:
$	AFTER_TIME = NOW
$	GOSUB SUBMIT_PROCEDURE
$!
$	GOTO NORMAL_EXIT

$!=============================================================================
$!
$! Restart code.
$!
$!-----------------------------------------------------------------------------
$RESTART:
$!
$! Determine parameter file specification.
$!
$	SAVED_PARAM_LIST = PARAM_LIST
$	PARAM_LIST = "Parameter_File,"
$	DISPLAY ""
$	P_MODE = "INTERACTIVE"
$	GOSUB GET_PARAMS
$	PARAM_LIST = SAVED_PARAM_LIST
$!
$	GOTO BATCH

$!=============================================================================
$!
$! Batch (and some Restart) code.
$!
$!-----------------------------------------------------------------------------
$BATCH:
$!
$! Read the parameters, and then check them in case the user has edited the
$! file and to put them in a standard form.  Note that a missing parameter
$! file may be treated earlier as an invalid procedure argument if the spec
$! does not include a directory.
$!
$	IF F$SEARCH(PARAMETER_FILE) .NES. "" THEN GOTO PARAMETER_FILE_EXISTS
$	ERROR_MESSAGE = -
	  "E-NOPFILE, parameter file (" + PARAMETER_FILE + ") not found."
$	GOTO SEND_MESSAGE_AND_ABORT
$PARAMETER_FILE_EXISTS:
$	BDISPLAY "Reading and checking parameters from ",PARAMETER_FILE
$	SPEC = PARAMETER_FILE
$	GOSUB READ_PREVIOUS_PARAMS
$	P_MODE = MODE
$	GOSUB GET_PARAMS
$!
$! If the invocation time is "significantly after" the stop time, exit
$! without processing any mail and without resubmitting another batch job.
$! The "extra" time is to allow for small delays in startup of the job by
$! the queue manager, for processing (e.g., LOGIN.COM execution) before
$! the approximate invocation time is noted, and for some clock skew
$! between cluster members.
$!
$	IF "''F$CVTIME(INVOKED+"-0:05")'" .GTS. F$CVTIME(ABS_STOP_TIME) -
	  THEN GOTO NORMAL_EXIT
$!
$! If performing a restart, go submit the procedure and exit.
$!
$	IF MODE .EQS. "RESTART" THEN GOTO INTERACTIVE_SUBMIT
$!
$! If the current time precedes the first run time, resubmit for the first
$! run time and exit without doing any mail processing.
$!
$	IF F$CVTIME() .GES. F$CVTIME(FIRST_TIME) THEN GOTO OK_TO_RUN_NOW
$	AFTER_TIME = FIRST_TIME
$	GOSUB SUBMIT_PROCEDURE
$	GOTO NORMAL_EXIT
$OK_TO_RUN_NOW:
$!
$! Find next time to requeue for.  If that time does not exceed the stop time,
$! resubmit the procedure before doing any more significant work, so that
$! there is less of a "window" during which the job might get killed without
$! a "descendent" to carry on this important work.  Don't queue another batch
$! job too close to the current time.
$!
$	BASE_TIME = NOW
$	GOSUB FIND_NEXT_TIME
$	AFTER_TIME = NEXT_TIME
$	TOO_SOON = "''F$CVTIME("+0:45","ABSOLUTE")'"
$	IF F$CVTIME(AFTER_TIME) .LTS. F$CVTIME(TOO_SOON) -
	  THEN AFTER_TIME = TOO_SOON
$	IF F$CVTIME(AFTER_TIME) .LES. F$CVTIME(ABS_STOP_TIME) -
	  THEN GOSUB SUBMIT_PROCEDURE
$!
$! Find out if Nmail is available
$!
$	NMAIL_IMAGE = "SYS$SHARE:NM_MAILSHR.EXE"
$	NMAIL_PREFIX = ""
$	IF F$SEARCH(NMAIL_IMAGE) .NES. "" -
	  THEN IF F$FILE_ATTRIBUTES(NMAIL_IMAGE,"KNOWN") -
	    THEN NMAIL_PREFIX = "NM%"
$!
$! Determine specification of mail file.
$!
$	DEFINE/USER SYS$OUTPUT 'MAILINFO_FILE'
$	DEFINE/USER SYS$ERROR 'MAILINFO_FILE'
$	MAIL
SHOW FILE
EXIT
$	OPEN/READ/ERROR=UNKNOWN_MAIL_FILE WM_MAILINFO 'MAILINFO_FILE'
$	KEYWORD = "Your current mail file is "
$LOOK_FOR_MAIL_FILE:
$	READ/ERROR=UNKNOWN_MAIL_FILE WM_MAILINFO LINE
$	IF LINE - KEYWORD .EQS. LINE THEN GOTO LOOK_FOR_MAIL_FILE
$	CLOSE WM_MAILINFO
$	MAIL_FILE = LINE - KEYWORD + ".." - "..."
$	GOTO KNOWN_MAIL_FILE
$UNKNOWN_MAIL_FILE:
$	CLOSE WM_MAILINFO
$	ERROR_MESSAGE = -
	  "E-MAILFILE, unable to determine specification of mail file."
$	GOTO SEND_MESSAGE_AND_ABORT
$KNOWN_MAIL_FILE:
$!
$! Define or initialize various variables for processing the mail file.
$!
$	FORWARD = "FORWARD/NOEDIT" -	!command for forwarding mail
	  + "''F$ELEMENT(VMS_VERSION.GES."5.0","|","|/NOSELF/NOCC")'"
$	IN_ORDER = 1			!records are in chronological order
$	MAX_SUBJECT = 510		!max length of mail SUBJECT string
$	MOVE_COUNT = 0			!number of messages to move (and copy)
$	NEWMAIL_COUNT = 0		!number of new mail message
$	NEWMAIL_FOLDER = "NEWMAIL"	!name of folder containing new mail
$	NEWMAIL_RECORD = 0		!number of record in new mail folder
$	NEWMAIL_TIME = ""		!quadword time (in hex) of new mail
$	NM$CONFIRM = 0			!make sure Nmail Confirm is disabled
$!
$! The following text strings must **NOT** be changed, or else there is a
$! danger (if a history of replies is not maintained) that two incarnations of
$! this procedure could start replying to each other indefinitely!  It is not
$! sufficient that this procedure be internally consistent... the subject must
$! be identical to all versions of this procedure that are in use.
$!
$	WATCH_MAIL_SUBJECTS = " from Watch_Mail"
$	COPY_SUBJECT = "Auto Copy''WATCH_MAIL_SUBJECTS': "
$	REPLY_SUBJECT = "Auto Reply''WATCH_MAIL_SUBJECTS'" -
	  + " for ''ABS_START_TIME' to ''ABS_STOP_TIME'"
$!
$! Now begin processing the mail file (by reading it directly).
$!
$	OPEN/READ/SHARE=WRITE/ERROR=ERROR_OPENING_MAIL_FILE -
	  WM_MAILFILE 'MAIL_FILE'
$	READ/NOLOCK/INDEX=1/ERROR=NO_NEWMAIL/KEY="''NEWMAIL_FOLDER'" -
	  WM_MAILFILE LINE		!will get error if no new mail
$!
$! It appears that at least one message is present in the new mail folder.
$! Create the header for the temporary command file which will be used to
$! move new mail to the WATCH_MAIL folder, and also possibly used to copy
$! new mail to one or more other users.
$!
$! Note that some messages in the new mail folder may not be new (i.e.,
$! unread).  Also, MAIL processes messages in chronological order, while
$! this procedure processes mail records in folder order.  Further, the
$! records for old (i.e., previously read or filed) messages which are
$! refiled into the new mail folder may not be stored in chronological
$! order.  To help ensure that the message numbers derived while processing
$! the new mail records will correspond to those that MAIL will determine
$! when it is later used to process the messages, this procedure ignores old
$! mail records, checks that new mail records are in chronological order,
$! and uses SELECT/NEW (vs. selecting the new mail folder) when having MAIL
$! process the messages.
$!
$	OPEN/WRITE WM_COMMAND 'COMMAND_FILE'
$	WRITE WM_COMMAND "$	DEFINE/USER SYS$OUTPUT _NLA0:"
$	WRITE WM_COMMAND "$	DEFINE/USER SYS$ERROR _NLA0:"
$	WRITE WM_COMMAND "$	MAIL"
$	WRITE WM_COMMAND "SELECT/NEW"
$!
$! Define names for the fields within the mail file records, and a list of
$! the fields that must be present in every record.
$!
$	MF_0 = "SENDER"
$	MF_1 = "SENT_TO"
$	MF_2 = "SUBJECT"
$	MF_3 = "CC_TO"
$	MF_4 = "TYPE_4"
$	MF_5 = "REC_CNT"
$	MF_6 = "TYPE_6"
$	MF_7 = "TYPE_7"
$	MFP_REQUIRED = "0,2"
$!
$	GOTO GET_RECORD_INFO
$!
$ERROR_OPENING_MAIL_FILE:
$	STATUS = $STATUS		!save failure status
$	ERROR_MESSAGE = "E-MAILOPEN, error opening mail file (" + MAIL_FILE -
	  + ") -- """ + F$MESSAGE(STATUS) + """"
$	GOTO SEND_MESSAGE_AND_ABORT
$!
$! Read next record.  If the folder name is not that of the new mail folder,
$! assume we're done (since the records are in order by folder name).
$! Otherwise check to see that the record corresponds to a new message and
$! that it occurs in chronological order, and then obtain the sender's and
$! recipient's addresses, subject, etc.
$!
$! If records for new messages are NOT in chronological order, all messages
$! are left unread and without a reply, effectively disabling the functioning
$! of this procedure.  If an unexpected record field is encountered, the
$! field is ignored.  And if all required fields are not present, the record
$! is ignored and the message is left unread.  However, in each of these
$! cases, an entry is created in a problem report to indicate that a problem
$! exists and to log information that may assist in its diagnosis.
$!
$! NOTE that the sender's address may contain quotes ("), apostrophes ('),
$! and exclamation points (!), some of which may require special treatment.
$! The sender's address is also reduced to the first unquoted space element,
$! for the purpose of removing a "personal name".
$!
$NOT_NEW_MESSAGE:
$GET_NEXT_RECORD:
$	READ/NOLOCK/INDEX=1/END=END_OF_NEWMAIL WM_MAILFILE LINE
$GET_RECORD_INFO:
$	FOLDER = "''F$EXTRACT(9,F$CVUI(8*8,8,LINE),LINE)'"
$	IF FOLDER .NES. NEWMAIL_FOLDER THEN GOTO END_OF_NEWMAIL
$	NEWMAIL_RECORD = 'F$STRING( NEWMAIL_RECORD + 1 )'
$	IF 'F$STRING( 1 - F$CVUI(48*8,1,LINE) )' THEN GOTO NOT_NEW_MESSAGE
$	NEWMAIL_COUNT = 'F$STRING( NEWMAIL_COUNT + 1 )'
$!
$	NEWMAIL_PREVIOUS_TIME = NEWMAIL_TIME
$	NEWMAIL_TIME = "''F$FAO("!2(XL)",F$CVUI(32,32,LINE),F$CVUI(0,32,LINE))'"
$	IF 'F$STRING( NEWMAIL_TIME .GTS. NEWMAIL_PREVIOUS_TIME )' -
	  THEN GOTO RECORD_IN_ORDER
$	PROBLEM = "''NEWMAIL_FOLDER' record #''NEWMAIL_RECORD'" -
	  + " is not in chronological order"
$	GOSUB OPEN_PROBLEM_FILE
$	WRITE WM_PROBLEM "previous time: ''NEWMAIL_PREVIOUS_TIME'", -
	  ",  current time: ''NEWMAIL_TIME'"
$	WRITE /SYMBOL WM_PROBLEM LINE
$	CLOSE WM_PROBLEM
$	IN_ORDER = 0			!will not be able to move mail
$RECORD_IN_ORDER:
$!
$	LEN = -4
$	POS = 64
$	GOTO MRF_ON
$MRF_BAD:
$	STATUS = $STATUS
$	ERROR = F$MESSAGE(STATUS)
$	IF ERROR - "-NOCOMD," .EQS. ERROR THEN GOTO ABNORMAL_EXIT_2
$	PROBLEM = "error (''STATUS') extracting field at byte ''POS'" -
	  + " from ''NEWMAIL_FOLDER' record #''NEWMAIL_RECORD'"
$	GOSUB OPEN_PROBLEM_FILE
$	WRITE /SYMBOL WM_PROBLEM LINE
$	CLOSE WM_PROBLEM
$MRF_ON:
$	ON WARNING THEN GOTO MRF_BAD
$MRF:
$	POS = 'F$STRING(POS+4+LEN)'
$	IF 'F$STRING(POS+4)' .GT. 'F$LENGTH(LINE)' THEN GOTO MRF_END
$	LEN = 'F$CVUI(POS*8+16,16,LINE)'
$	&MF_'F$CVUI(POS*8,16,LINE)' = F$EXTRACT(POS+4,LEN,LINE)
$	MFP_'F$CVUI(POS*8,16,LINE)' = 'NEWMAIL_RECORD'
$	GOTO MRF
$MRF_END:
$	ON WARNING THEN GOTO ABNORMAL_EXIT
$!
$	VS = 'F$VERIFY(0)' + F$VERIFY(0'VERIFY_VAR' .GT. 0)	!usually off
$	POS = -1
$CHECK_REQUIRED_FIELD:
$	POS = 'POS' + 1
$	VALUE = "MFP_" + F$ELEMENT(POS,",",MFP_REQUIRED) - ","
$	IF F$TYPE('VALUE') .NES. "" THEN IF 'VALUE' .EQ. NEWMAIL_RECORD -
	  THEN GOTO CHECK_REQUIRED_FIELD
$	IF VALUE .EQS. "MFP_" THEN GOTO CHECKED_REQUIRED_FIELDS
$	V = 'F$VERIFY(0)' + F$VERIFY(VS)	!restore verify
$	PROBLEM = "required field (" + 'F$STRING(VALUE-"P")' + ") absent" -
	  + " from ''NEWMAIL_FOLDER' record #''NEWMAIL_RECORD'"
$	GOSUB OPEN_PROBLEM_FILE
$	WRITE /SYMBOL WM_PROBLEM LINE
$	CLOSE WM_PROBLEM
$	GOTO DO_NOT_MOVE_YET
$CHECKED_REQUIRED_FIELDS:
$	POS = 0
$FIND_UNQUOTED_SPACE:
$	V = 'F$VERIFY(0)' + F$VERIFY(VS)	!restore verify
$	QPOS = 'F$STRING( F$LOCATE("""",F$EXTRACT(POS,999,SENDER)) + POS )'
$	POS = 'F$STRING( F$LOCATE(" ",F$EXTRACT(POS,999,SENDER)) + POS )'
$	VS = 'F$VERIFY(0)' + F$VERIFY(0'VERIFY_VAR' .GT. 0)	!usually off
$	IF POS .LE. QPOS THEN GOTO FOUND_UNQUOTED_SPACE
$	POS = 'F$LOCATE("""",F$EXTRACT(QPOS+1,999,SENDER))' + QPOS + 2
$	GOTO FIND_UNQUOTED_SPACE
$FOUND_UNQUOTED_SPACE:
$	V = 'F$VERIFY(0)' + F$VERIFY(VS)	!restore verify
$	SENDER = F$EXTRACT(0,'POS',SENDER)
$	ADDRESS = SENDER
$	GOSUB REMOVE_ROUTING
$	U_SENDER = F$EDIT(SENDER,"UPCASE")	!upper case
$	R_SENDER = ADDRESS			!routing removed
$	RU_SENDER = F$EDIT(ADDRESS,"UPCASE")	!routing removed, upper case
$	S_NODE = F$EXTRACT(0,NLEN,RU_SENDER)
$	S_NAME = RU_SENDER - S_NODE
$!
$	VS = 'F$VERIFY(0)'		!verify off
$!
$! Display the sender and subject in batch mode, for possible debugging.
$! These are not shown via symbol substitution because they may contain
$! quotation marks and other "troublesome" characters.  They may also be
$! greater than 255 characters, so special care must be taken.
$!
$	LLINE = "From: <" + SENDER + ">"
$	DISPLAY /SYMBOL LLINE
$	LLINE = "  <" + SUBJECT + ">"
$	DISPLAY /SYMBOL LLINE
$	V = F$VERIFY(VS)		!restore verify
$!
$! Don't bother to reply to the current user, or to automatic mail from another
$! WATCH_MAIL job, or to mail from a mailer daemon or postmaster.
$!
$	IF RU_SENDER .EQS. THIS_CLUSTER_USER THEN GOTO DO_NOT_REPLY
$	IF RU_SENDER .EQS. THIS_NODE_USER THEN GOTO DO_NOT_REPLY
$	IF SUBJECT - WATCH_MAIL_SUBJECTS .NES. SUBJECT -
	  THEN IF F$EDIT(F$EXTRACT(0,4,SUBJECT),"UPCASE") .NES. "RE: " -
	    THEN GOTO DO_NOT_REPLY
$	IF RU_SENDER - "-DAEMON" - "POSTMASTER" .NES. RU_SENDER -
	  THEN GOTO DO_NOT_REPLY
$!
$! Don't send a reply if the sender is listed in the history file (because
$! they've already been sent a reply) or in the user-supplied exclude file.
$!
$	SPEC = HISTORY_FILE
$	GOSUB SEARCH_FILE
$	IF FOUND THEN GOTO DO_NOT_REPLY
$!
$	SPEC = EXCLUDE_FILE
$	GOSUB SEARCH_FILE
$	IF FOUND THEN GOTO DO_NOT_REPLY
$!
$! Now attempt to reply to the sender.
$!
$	STRING = NMAIL_PREFIX + SENDER
$	IF STRING - """" .NES. STRING THEN GOSUB DOUBLE_QUOTES
$	SET NOON
$	MAIL 'REPLY_FILE' -		!avoid long line
	  "''STRING'" -
	  /SUBJECT="''REPLY_SUBJECT'"
$	OK = $STATUS
$	SET ON
$	IF .NOT. OK THEN GOTO DO_NOT_MOVE_YET
$!
$! If a history file exists and the reply was successful, add the sender's
$! address (with routing information removed) to the history file.
$!
$	IF HISTORY_FILE .EQS. "" THEN GOTO DO_NOT_UPDATE_HISTORY
$	OPEN/APPEND/ERROR=ERROR_UPDATING_HISTORY WM_HISTORY 'HISTORY_FILE'
$	WRITE/ERROR=ERROR_UPDATING_HISTORY WM_HISTORY /SYMBOL R_SENDER
$	GOTO HISTORY_UPDATED
$ERROR_UPDATING_HISTORY:
$	STATUS = $STATUS		!save failure status
$	ERROR_MESSAGE = "W-HISTUPDATE, error updating history file -- """ -
	  + F$MESSAGE(STATUS) + """"
$	GOSUB SEND_MESSAGE
$	ERROR_MESSAGE = ""		!don't abend with this error message
$	HISTORY_FILE = ""		!prevent further use of history file
$HISTORY_UPDATED:
$	CLOSE WM_HISTORY
$DO_NOT_UPDATE_HISTORY:
$!
$! Generate commands which will subsequently be used to copy and move the
$! message.
$!
$DO_NOT_REPLY:
$	WRITE WM_COMMAND "READ ''NEWMAIL_COUNT'"
$	IF COPY_TO .EQS. "" THEN GOTO DO_NOT_COPY
$	WRITE WM_COMMAND FORWARD
$	WRITE WM_COMMAND COPY_TO
$	LLINE = SUBJECT
$	IF 'F$STRING( F$LOCATE(COPY_SUBJECT,SUBJECT+" ") .NE. 0 )' -
	  THEN LLINE = COPY_SUBJECT + LLINE
$	IF 'F$LENGTH(LLINE)' .GT. 'MAX_SUBJECT' -
	  THEN LLINE = F$EXTRACT(0,MAX_SUBJECT-3,LLINE) + "..."
$	WRITE WM_COMMAND /SYMBOL LLINE
$DO_NOT_COPY:
$	WRITE WM_COMMAND "MOVE/NOCONFIRM ''MOVETO_FOLDER'"
$	MOVE_COUNT = 'F$STRING( MOVE_COUNT + 1 )'
$!
$DO_NOT_MOVE_YET:
$	GOTO GET_NEXT_RECORD
$!
$! Done processing the new mail folder.  Copy and move the mail as necessary.
$!
$! NOTE that unfortunately it appears that MAIL does not abort or return
$! a failure status if something goes wrong (e.g., bad Copy_To address).
$!
$END_OF_NEWMAIL:
$	WRITE WM_COMMAND "EXIT"
$	WRITE WM_COMMAND "$!"
$	CLOSE WM_COMMAND
$NO_NEWMAIL:
$	CLOSE WM_MAILFILE
$	IF 'MOVE_COUNT' .GT. 0 THEN IF 'IN_ORDER' THEN @'COMMAND_FILE'
$	GOTO NORMAL_EXIT

$!=============================================================================
$!
$! Subroutines.
$!
$!-----------------------------------------------------------------------------
$!
$! Subroutine to determine important "constants" for use by this procedure,
$! and also disable the use of externally-defined symbols.  The constants
$! are defined here in a modular fashion because they may be needed earlier
$! than normally anticipated due to error handling.
$!
$DEFINE_CONSTANTS:
$!
$! Ignore all externally-defined symbols, thereby preventing user redefinition
$! of the VMS commands or their defaults, after first saving necessary values.
$!
$	ENABLE_SYMBOLS = "SET SYMBOL/SCOPE=(LOCAL,GLOBAL)"
$	ENABLE_SYMBOLS
$!
$	IF "''F$TYPE(WATCH_MAIL_EDIT)'" .NES. "" THEN EDIT = WATCH_MAIL_EDIT
$	IF "''F$TYPE(EDIT)'" .NES. "" THEN EDIT = EDIT
$!
$	DISABLE_SYMBOLS = "SET SYMBOL/SCOPE=(NOLOCAL,NOGLOBAL)"
$	DISABLE_SYMBOLS
$!
$! Determine version of VMS being used, exclusive of first letter.
$!
$	VMS_VERSION = F$EXTRACT( 1, 999, "''F$GETSYI("VERSION")'" )
$!
$! Define (or redefine) some commands for our own use.
$!
$	CLOSE = "CLOSE/NOLOG"
$	DISPLAY = "WRITE SYS$OUTPUT"
$	HELP_BEGIN = "COPY SYS$INPUT SYS$OUTPUT"
$	HELP_END = "RETURN"
$	MAIL = "MAIL/NOSELF"
$!
$	BDISPLAY = DISPLAY + " LF,"
$	LF[0,8] = 10
$!
$! Get process identification (as hex string) of current process.
$!
$	OUR_PID = "''F$GETJPI("","PID")'"
$!
$! Determine file specifications for some files.
$!
$! NOTE that the code that deletes "orphaned" temporary files relies
$! on the syntax of the temporary file specifications.
$!
$	PROC_FILE		= F$ENVIRONMENT("PROCEDURE")
$	PROC_NAME		= F$PARSE(PROC_FILE,,,"NAME")
$	SPEC_PREFIX		= "SYS$LOGIN:WM_"
$	DEFAULT_PARAMETER_FILE	= SPEC_PREFIX + "PARAMS.DAT;"
$	PROBLEM_FILE		= SPEC_PREFIX + "PROBLEMS.DAT;"
$	VERSION_FILE		= SPEC_PREFIX + "VERSION.DAT;"
$	TEMP_SUFFIX		= "_''OUR_PID'.TMP"
$	TEMP_FILES		= SPEC_PREFIX + "*" + TEMP_SUFFIX + ";*"
$	ORPHAN_FILES		= SPEC_PREFIX + "*" + "_*.TMP"    + ";*"
$	BATCHINFO_FILE		= SPEC_PREFIX + "BATCHINFO" + TEMP_SUFFIX
$	COMMAND_FILE		= SPEC_PREFIX + "COMMAND" + TEMP_SUFFIX
$	MAILINFO_FILE		= SPEC_PREFIX + "MAILINFO" + TEMP_SUFFIX
$	MESSAGE_FILE		= SPEC_PREFIX + "MESSAGE" + TEMP_SUFFIX
$!
$! Find out who the user is, and their node name (if DECnet is in use).
$!
$	THIS_USER		= "''F$EDIT(F$GETJPI("","USERNAME"),"TRIM")'"
$	THIS_NODE		= "''F$TRNLNM("SYS$NODE")'"
$	THIS_CLUSTER		= "''F$TRNLNM("SYS$CLUSTER_NODE")'"
$	IF THIS_CLUSTER .EQS. "" THEN THIS_CLUSTER = THIS_NODE
$	THIS_NODE_USER		= "''THIS_NODE'''THIS_USER'"
$	THIS_CLUSTER_USER	= "''THIS_CLUSTER'''THIS_USER'"
$!
$! Specify the name of the folder into which new mail will be moved.
$!
$	MOVETO_FOLDER = "WATCH_MAIL"
$!
$! Get the procedure arguments.  Then display the arguments and the value of
$! the system symbol "$RESTART" if in batch mode or debugging.
$!
$	ARGS = -
	  " " + F$EDIT( P1+" "+P2+" "+P3+" "+P4+" "+P5+" "+P6+" "+P7+" "+P8, -
	    "TRIM,COMPRESS" ) + " "
$	VS = 'F$VERIFY(0)'		!verify off
$	IF F$MODE() .NES. "BATCH" THEN IF 0'VERIFY_VAR' .LE. 0 -
	  THEN GOTO NO_SHOW_ARGS
$	DISPLAY "Args: <",F$EDIT(ARGS,"TRIM"),">"
$	DISPLAY "$RESTART: <",$RESTART,">"
$NO_SHOW_ARGS:
$	V = F$VERIFY(VS)		!restore verify
$!
$! Determine the "mode" of operation, as one of the following.  Note that
$! the interactive modes can also be "faked" while running in batch.
$!
$!	INTERACTIVE	normal interactive creation of batch job
$!	BATCH		normal batch job to scan for new mail
$!	CANCEL		interactive mode to cancel an existing batch job
$!	RESTART		interactive mode to restart batch job
$!	JOBS		interactive mode to list all batch jobs on the system
$!	CREATE		same as INTERACTIVE, for use when running in batch
$!
$	MODE = "''F$MODE()'"
$	TEMP_LIST = "JOBS,CANCEL,RESTART,CREATE,"
$CHECK_KEYWORD:
$	KEYWORD = "''F$ELEMENT(0,",",TEMP_LIST)'"
$	TEMP_LIST = TEMP_LIST - KEYWORD - ","
$	POS = 'F$LOCATE(" "+KEYWORD+" ",F$EDIT(ARGS,"UPCASE"))'
$	IF 'POS' .LT. 'F$LENGTH(ARGS)' THEN MODE = KEYWORD
$	ARGS = F$EXTRACT(0,POS,ARGS)+F$EXTRACT(POS+F$LENGTH(KEYWORD)+1,999,ARGS)
$	IF TEMP_LIST .NES. "" THEN GOTO CHECK_KEYWORD
$	IF 'F$STRING( MODE .EQS. "CREATE" )' THEN MODE = "INTERACTIVE"
$!
$	CONSTANTS_DEFINED = 1		!remember that they are now defined
$	RETURN
$!-----------------------------------------------------------------------------
$!
$! Subroutine to read previous parameters from a parameter file.
$!
$READ_PREVIOUS_PARAMS:
$	OPEN/READ WM_PARAMS 'SPEC'
$!		<< listing of parameters >>
$	VS = 'F$VERIFY(0)' + F$VERIFY(0'VERIFY_VAR' .GT. 0)	!usually off
$	PARAM_PROC_INFO = ""
$	PARAM_USER_INFO = ""
$READ_PREVIOUS_PARAM:
$	READ/END=END_PREVIOUS_PARAMS WM_PARAMS LINE
$	IF LINE - PARAM_PROC_KEYWORD .NES. LINE -
	  THEN PARAM_PROC_INFO = LINE - PARAM_PROC_KEYWORD
$	IF LINE - PARAM_USER_KEYWORD .NES. LINE -
	  THEN PARAM_USER_INFO = F$ELEMENT(0," ",LINE-PARAM_USER_KEYWORD)
$	V = 'F$VERIFY(0)' + F$VERIFY(VS)	!restore verify for next line
$ 'LINE'
$	V = 'F$VERIFY(0)' + F$VERIFY(V)		!usually off
$	GOTO READ_PREVIOUS_PARAM
$END_PREVIOUS_PARAMS:
$	STRING = F$ELEMENT(0," ",PARAM_PROC_INFO)
$	V = 'F$VERIFY(0)' + F$VERIFY(VS)	!restore verify
$!		<< end of parameter listing >>
$	CLOSE WM_PARAMS
$	PARAM_PROC_FILE = "''F$STRING( STRING - F$PARSE(STRING,,,"NODE") )'"
$	PARAM_VERSION = "''F$STRING( F$ELEMENT(2," ",PARAM_PROC_INFO) - " " )'"
$	RETURN
$!-----------------------------------------------------------------------------
$!
$! Subroutine to get parameters from the user (if in interactive parameter
$! mode), and check them (regardless of parameter mode).
$!
$GET_PARAMS:
$	PARAM_INDEX = 1
$GET_PREVIOUS_PARAM:
$	PARAM_INDEX = 'F$STRING( PARAM_INDEX - 2 )'
$GET_NEXT_PARAM:
$	PARAM_INDEX = 'F$STRING( PARAM_INDEX + 1 )'
$	PARAM = "''F$ELEMENT(PARAM_INDEX,",",PARAM_LIST)'"
$	IF PARAM .EQS. "" THEN RETURN
$	VALUE = F$EDIT('PARAM',"TRIM")
$	IF P_MODE .NES. "INTERACTIVE" THEN GOTO GOT_PARAM
$GET_PARAM_AGAIN:
$	STRING = PARAM + " [" + 'PARAM' + "]: "
$	IF STRING - """" .NES. STRING THEN GOSUB DOUBLE_QUOTES
$	READ/END=ABNORMAL_EXIT SYS$COMMAND VALUE -	!avoid long line
	  /PROMPT="''STRING'"
$	IF F$EXTRACT(0,1,VALUE) .EQS. "!" THEN GOTO GET_PARAM_AGAIN
$	IF VALUE .EQS. "" THEN VALUE = 'PARAM'
$	VALUE = F$EDIT(VALUE,"TRIM")
$	LEN = F$LENGTH(VALUE) + ( VALUE .EQS. "" )
$	IF VALUE .NES. F$EXTRACT(0,LEN,"??"+NUL) THEN GOTO NO_HELP_WANTED
$	GOSUB HELP__'F$ELEMENT(F$LENGTH(VALUE)-1,NUL,"GENERAL"+NUL+PARAM)'
$	GOTO GET_PARAM_AGAIN
$NO_HELP_WANTED:
$	IF VALUE .NES. "^" THEN GOTO GOT_PARAM
$	IF PARAM_INDEX .GT. 0 THEN GOTO GET_PREVIOUS_PARAM
$	ERROR = "No previous question."
$	GOTO BAD_PARAM
$GOT_PARAM:
$	STRING = F$EDIT( F$ELEMENT(0,"/",VALUE), "TRIM,UPCASE" )
$	IF STRING .NES. "^D" THEN GOTO PARAM_NOT_DEFAULTED
$	VALUE = F$EDIT( -
	  DEFAULT_'PARAM' + F$EXTRACT(F$LENGTH(STRING),999,VALUE), "TRIM" )
$	IF P_MODE .NES. "RESTART" THEN DISPLAY "  Using """,VALUE,"""."
$PARAM_NOT_DEFAULTED:
$	USER_VALUE = VALUE
$	ERROR = ""
$	GOSUB CHECK_'PARAM'
$	IF ERROR .NES. "" THEN GOTO BAD_PARAM
$	IF P_MODE .EQS. "INTERACTIVE" THEN IF VALUE .EQS. "" -
	  THEN IF VALUE .NES. 'PARAM' THEN DISPLAY "  Existing value deleted."
$	'PARAM' = VALUE
$	GOTO GET_NEXT_PARAM
$BAD_PARAM:
$	IF P_MODE .NES. "INTERACTIVE" .OR. F$MODE() .NES. P_MODE -
	  THEN GOTO FATAL_PARAM_ERROR	!dialog recovery not desired or possible
$	IF ERROR .NES. " " THEN DISPLAY "  ",F$FAO(ERROR,VALUE)
$	GOTO GET_PARAM_AGAIN
$FATAL_PARAM_ERROR:
$	STRING = F$FAO(ERROR,VALUE)
$	ERROR_MESSAGE = "E-BADPARAM, " -
	  + F$EDIT(F$EXTRACT(0,1,STRING),"LOWERCASE") + F$EXTRACT(1,999,STRING)
$	GOTO SEND_MESSAGE_AND_ABORT
$!
$CHECK_BATCH_QUEUE:
$	EQUIVALENCE = F$EDIT(VALUE,"UPCASE")
$	IF VMS_VERSION .LTS. "5.0" THEN GOTO USE_QUEUE_LIST
$	IF F$GETQUI("DISPLAY_QUEUE","QUEUE_BATCH",EQUIVALENCE) -
	  THEN GOTO BATCH_QUEUE_EXISTS
$	GOTO BATCH_QUEUE_NONEXISTENT
$USE_QUEUE_LIST:
$	LN = BQLN
$	TRANSLATION_COUNT = 0
$TRANSLATE_BATCH_QUEUE:
$	TRANSLATION_COUNT = TRANSLATION_COUNT + 1
$	QUEUE_NAME = EQUIVALENCE
$	EQUIVALENCE = F$TRNLNM(QUEUE_NAME)
$	IF EQUIVALENCE .NES. "" THEN IF TRANSLATION_COUNT .LT. 100 -
	  THEN GOTO TRANSLATE_BATCH_QUEUE
$SEARCH_NEXT_BQLE:
$	IF BQLE_'LN' - ( "," + QUEUE_NAME + "," ) .NES. BQLE_'LN' -
	  THEN GOTO BATCH_QUEUE_EXISTS
$	LN = 'F$STRING( LN - 1 )'
$	IF LN .GT. 0 THEN GOTO SEARCH_NEXT_BQLE
$BATCH_QUEUE_NONEXISTENT:
$	ERROR = "No such batch queue (!AS) on the system."
$BATCH_QUEUE_EXISTS:
$	IF VALUE .EQS. "" THEN ERROR = "You must specify a batch queue name."
$	RETURN
$!
$CHECK_COPY_TO:
$	U_VALUE = F$EDIT(VALUE,"UPCASE")
$	IF U_VALUE .EQS. THIS_USER -
	  .OR. U_VALUE .EQS. THIS_CLUSTER_USER -
	    .OR. U_VALUE .EQS. THIS_NODE_USER -
	      THEN ERROR = "You are not allowed to copy to yourself."
$	IF ERROR .EQS. "" -
	  THEN IF VALUE - 'PARAM' .NES. "" THEN IF P_MODE .EQS. "INTERACTIVE" -
	    THEN DISPLAY "  Be sure to verify the supplied address(es)."
$	RETURN
$!
$CHECK_TIME_LIST:
$	IF "''F$TYPE(ORIGINALLY_INVOKED)'" .EQS. "" -
	  .OR. MODE .EQS. "INTERACTIVE" THEN ORIGINALLY_INVOKED = INVOKED
$	VS = 'F$VERIFY(0)' + F$VERIFY(0'VERIFY_VAR' .GT. 0)	!usually off
$	IF VALUE .EQS. "" THEN ERROR = "You must specify one or more times."
$	IF ERROR .NES. "" THEN GOTO CHECKED_TIME_LIST
$	EVERY = F$ELEMENT(0," ",VALUE)
$	IF F$EXTRACT( 0, F$LENGTH(EVERY)+(EVERY.EQS.""), "EVERY" ) -
	  .NES. F$EDIT(EVERY,"UPCASE") THEN GOTO NOT_EVERY_SYNTAX
$	EVERY = F$EDIT(VALUE-EVERY,"TRIM")
$	IF "0" + EVERY .LE. 0 -
	  THEN ERROR = "Invalid EVERY N value (" + EVERY + ")."
$	IF ERROR .EQS. "" THEN IF 24 / EVERY * EVERY .NE. 24 THEN ERROR = -
	  "The EVERY N value (" + EVERY + ") does not evenly divide 24."
$	IF ERROR .NES. "" THEN GOTO CHECKED_TIME_LIST
$	RANDOM = 'F$CVTIME(ORIGINALLY_INVOKED,,"SECOND")' * 100 -
	  + 'F$CVTIME(ORIGINALLY_INVOKED,,"HUNDREDTH")'
$	HOUR = 'F$STRING( RANDOM - ( RANDOM / EVERY * EVERY ) )'
$	MINUTE = ":''F$CVTIME(ORIGINALLY_INVOKED,,"MINUTE")'"
$	STRING = ""
$GEN_EVERY_LIST:
$	STRING = STRING + ",''HOUR'''MINUTE'"
$	HOUR = 'F$STRING( HOUR + EVERY )'
$	IF 'HOUR' .LT. 24 THEN GOTO GEN_EVERY_LIST
$	IF P_MODE .NES. "RESTART" -
	  THEN DISPLAY "  Expanded to """,F$STRING(STRING-","),"""."
$	V = 'F$VERIFY(0)' + F$VERIFY(VS)	!restore verify
$	VALUE = "Every ''EVERY'"
$	ABS_TIME_LIST = "''F$EDIT(STRING-",","COLLAPSE")'"
$	GOTO CHECKED_TIME_LIST
$NOT_EVERY_SYNTAX:
$	LIST = VALUE
$	STRING = ""
$	PREVIOUS_TIME = ""
$CHECK_NEXT_TIME:
$	VALUE = F$EDIT(F$ELEMENT(0,",",LIST),"TRIM")
$	LIST = F$EDIT(LIST-VALUE-",","TRIM")
$	IF VALUE - " " .NES. VALUE -
	  THEN ERROR = "Times must be separated by commas."
$	DEFINE/USER SYS$OUTPUT _NLA0:
$	DEFINE/USER SYS$ERROR _NLA0:
$	SET NOON
$	TIME = "''F$EXTRACT(0,5,F$CVTIME("0+0-"+VALUE+":00",,"TIME"))'"
$	OK = '$STATUS'
$	SET ON
$	DEASSIGN/USER SYS$OUTPUT
$	DEASSIGN/USER SYS$ERROR
$	IF ERROR .EQS. "" THEN IF .NOT. OK .OR. VALUE .EQS. "" -
	  THEN ERROR = "Bad time value (!AS)."
$	IF ERROR .EQS. "" THEN VALUE = " ''TIME'" - " 0" - " " - ":00"
$	IF ERROR .EQS. "" THEN IF TIME .LES. PREVIOUS_TIME -
	  THEN ERROR = "Time (!AS) not listed in ascending order."
$	IF ERROR .EQS. "" THEN IF PREVIOUS_TIME .NES. "" -
	  THEN IF F$CVTIME(TIME) .LTS. F$CVTIME(PREVIOUS_TIME+"+1:00") THEN -
	    ERROR = "Time (!AS) less than an hour from previous time."
$	IF ERROR .NES. "" THEN GOTO CHECKED_TIME_LIST
$	STRING = STRING + ",''VALUE'"
$	PREVIOUS_TIME = "''TIME'"
$	IF LIST .NES. "" THEN GOTO CHECK_NEXT_TIME
$GOOD_TIME_LIST:
$	V = 'F$VERIFY(0)' + F$VERIFY(VS)	!restore verify
$	VALUE = "''F$STRING(STRING-",")'"
$	ABS_TIME_LIST = VALUE
$CHECKED_TIME_LIST:
$	V = 'F$VERIFY(0)' + F$VERIFY(VS)	!restore verify
$	RETURN
$!
$CHECK_PARAMETER_FILE:
$	IF VALUE .EQS. "" THEN ERROR = "You must specify a parameter file."
$	FILE_TYPE = "''F$ELEMENT( MODE.EQS."INTERACTIVE", "/", "IN/OUT" )'PUT"
$	IF ERROR .EQS. "" THEN GOSUB CHECK_FILE
$	IF ERROR .NES. "" THEN RETURN
$	IF MODE .EQS. "RESTART" THEN GOTO DO_NOT_READ_PARAMS	!will read later
$	IF F$SEARCH(VALUE) .EQS. "" THEN GOTO DO_NOT_READ_PARAMS
$	DISPLAY "  Reading defaults for other parameters from the file."
$	SPEC = VALUE
$	GOSUB READ_PREVIOUS_PARAMS
$DO_NOT_READ_PARAMS:
$	RETURN
$!
$CHECK_REPLY_FILE:
$	IF VALUE .EQS. "" THEN ERROR = "You must specify a reply file."
$	FILE_TYPE = "INPUT"
$	IF ERROR .EQS. "" THEN GOSUB CHECK_FILE
$	IF ERROR .EQS. "" THEN IF P_MODE .EQS. "INTERACTIVE" THEN DISPLAY -
	  "  Be sure that your message lacks sensitive information."
$	RETURN
$!
$CHECK_EXCLUDE_FILE:
$	IF VALUE .EQS. "" THEN RETURN
$	FILE_TYPE = "INPUT"
$	GOTO CHECK_FILE
$!
$CHECK_HISTORY_FILE:
$CHECK_LOG_FILE:
$	IF VALUE .EQS. "" THEN RETURN
$	FILE_TYPE = "OUTPUT"
$	GOTO CHECK_FILE
$!
$CHECK_START_TIME:
$	IF VALUE .EQS. "" THEN ERROR = "You must specify a start time."
$	IF ERROR .NES. "" THEN RETURN
$	BASE_DAY = NOW
$	GOSUB CHECK_TIME
$	IF MODE .EQS. "INTERACTIVE" THEN IF ERROR .EQS. "" -
	  THEN IF F$CVTIME(VALUE) .LTS. F$CVTIME(NOW) -
	    THEN ERROR = "Start time (!AS) is in the past."
$	IF ERROR .NES. "" THEN RETURN
$	GOSUB CHECKED_START_OR_STOP_TIME
$	BASE_TIME = "''F$CVTIME(ABS_START_TIME+"-0:00:00.01","ABSOLUTE")'"
$	GOSUB FIND_NEXT_TIME
$	FIRST_TIME = NEXT_TIME
$	RETURN
$!
$CHECK_STOP_TIME:
$	IF VALUE .EQS. "" THEN ERROR = "You must specify a stop time."
$	IF ERROR .NES. "" THEN RETURN
$	BASE_DAY = ABS_START_TIME
$	GOSUB CHECK_TIME
$	IF ERROR .NES. "" THEN RETURN
$	IF F$CVTIME(VALUE) .LTS. F$CVTIME(FIRST_TIME) THEN ERROR = -
	  "Stop time (!AS) precedes first run time (" + FIRST_TIME + ")."
$	IF ERROR .NES. "" THEN RETURN
$	GOTO CHECKED_START_OR_STOP_TIME
$!
$CHECK_VERSION_LIMIT:
$	IF VALUE .EQS. "" THEN ERROR = "You must specify a version limit."
$	IF ERROR .EQS. "" THEN IF "0" + VALUE .EQ. 0 -
	  THEN IF VALUE .NES. "0" THEN ERROR = "Invalid version limit (!AS)."
$	IF ERROR .NES. "" THEN RETURN
$	VALUE = "''F$INTEGER(VALUE)'"
$	IF VALUE .LT. 0 -
	  THEN ERROR = "Version limit (!AS) is less than zero (0)."
$	RETURN
$!
$CHECKED_START_OR_STOP_TIME:
$	IF MODE .EQS. "INTERACTIVE" THEN ABS_'PARAM' = VALUE
$	VALUE = USER_VALUE		!restore "Now", "Monday+21-5:00", etc.
$	IF P_MODE .NES. "RESTART" THEN DISPLAY "  Resultant time:  ", -
	  ABS_'PARAM',"  (",F$EXTRACT(0,3,F$CVTIME(ABS_'PARAM',,"WEEKDAY")),")"
$	RETURN
$!
$CHECK_TIME:
$	IF F$EDIT(VALUE,"UPCASE") .EQS. "NOW" THEN VALUE = NOW
$	DEFINE/USER SYS$OUTPUT _NLA0:
$	DEFINE/USER SYS$ERROR _NLA0:
$	SET NOON
$	VALUE = F$CVTIME(VALUE,"ABSOLUTE")	!value not changed if error
$	STATUS = $STATUS
$	SET ON
$	DEASSIGN/USER SYS$OUTPUT
$	DEASSIGN/USER SYS$ERROR
$	IF STATUS THEN GOTO GOOD_TIME
$	DAY = -1
$	STRING = F$EDIT(F$ELEMENT(0,"+",VALUE),"UPCASE,TRIM")
$	LEN = 'F$LENGTH(STRING)'
$	IF LEN .LT. 3 THEN LEN = 3	!check at least three characters
$CHECK_NEXT_WEEKDAY:
$	IF 'DAY' .GE. 6 THEN GOTO BAD_TIME
$	DAY = "+''F$STRING( DAY - "-0" + 1 )'-0"
$	WEEKDAY = "''F$CVTIME(BASE_DAY+DAY,,"WEEKDAY")'"
$	IF STRING .NES. "''F$EDIT(F$EXTRACT(0,LEN,WEEKDAY),"UPCASE")'" -
	  THEN GOTO CHECK_NEXT_WEEKDAY
$	VALUE = "''F$CVTIME(BASE_DAY+DAY,"ABSOLUTE","DATE")'" -
	  + F$EXTRACT(LEN,999,VALUE)
$	GOTO CHECK_TIME
$BAD_TIME:
$	ERROR = "Bad time specification (!AS)."
$	GOTO CHECKED_TIME
$GOOD_TIME:
$	VALUE = "''F$EXTRACT(0,F$LOCATE(":",VALUE)+3,VALUE)'"
$CHECKED_TIME:
$	RETURN
$!
$CHECK_FILE:
$	EDIT_FILE = 0
$CHECK_SPEC:
$	PARSED_SPEC = F$PARSE(VALUE,'PARAM',DEFAULT_'PARAM')
$	IF PARSED_SPEC .NES. "" THEN GOTO SPEC_OK
$	STRING = F$EXTRACT( F$LOCATE("/",VALUE), 999, VALUE )
$	IF F$EXTRACT(0,F$LENGTH(STRING),"/EDIT") .EQS. F$EDIT(STRING,"UPCASE") -
	  THEN IF F$LENGTH(STRING) .GE. 2 THEN IF FILE_TYPE .EQS. "INPUT" -
	    THEN IF MODE .EQS. "INTERACTIVE" THEN GOTO WANT_TO_EDIT_FILE
$	ERROR = "Bad file specification (!AS)."
$	RETURN
$WANT_TO_EDIT_FILE:
$	VALUE = F$EDIT(VALUE-STRING,"TRIM")
$	FILE_TYPE = "OUTPUT"
$	EDIT_FILE = 1
$	GOTO CHECK_SPEC
$SPEC_OK:
$	VALUE = PARSED_SPEC
$	IF VALUE - "*" - "%" .NES. VALUE -
	  THEN ERROR = "Wildcards not allowed (!AS)."
$	IF ERROR .NES. "" THEN RETURN
$	DESTROY_CONTEXT = F$SEARCH("")
$	IF FILE_TYPE .EQS. "INPUT" THEN IF F$SEARCH(VALUE) .EQS. "" -
	  THEN ERROR = "Input file (!AS) does not exist."
$	IF ERROR .NES. "" THEN RETURN
$	V_NEW = "''F$PARSE(VALUE,,,"VERSION")'" - ";0" - ";"
$	IF V_NEW .EQS. "" THEN GOTO GOOD_FILE
$	IF EDIT_FILE -
	  THEN ERROR = "Version number not allowed if editing the file."
$	IF ERROR .NES. "" THEN RETURN
$	ALL_VERSIONS = F$PARSE(";*",VALUE)
$	OLD = "''F$SEARCH(ALL_VERSIONS)'"
$	V_OLD = "''F$PARSE(OLD,,,"VERSION")'" - ";"
$	IF V_NEW .GE. V_OLD THEN GOTO GOOD_FILE
$	ERROR = "A higher version (" + OLD + ") exists."	
$	IF P_MODE .NES. "RESTART" THEN DISPLAY "  ",ERROR
$	IF FILE_TYPE .EQS. "OUTPUT" THEN GOTO HIGHER_OUTPUT_VERSION
$	YES = 1
$	PROMPT = "  Do you still want to use the lower version input file"
$	IF P_MODE .EQS. "INTERACTIVE" THEN GOSUB READ_YES_NO
$	IF YES THEN GOTO FILE_NOT_BAD
$	GOTO BAD_VERSION
$HIGHER_OUTPUT_VERSION:
$	IF P_MODE .NES. "INTERACTIVE" THEN RETURN
$	PROMPT = -
	  "  Is it okay to delete all existing versions of this output file"
$	GOSUB READ_YES_NO
$	IF YES THEN GOTO DELETE_VERSIONS
$BAD_VERSION:
$	DISPLAY "  Omit the version number or enter a higher one", -
	  " (e.g., "";"" or "";''V_OLD'"")."
$	GOTO GET_FILE_AGAIN
$DELETE_VERSIONS:
$	DELETE/LOG 'ALL_VERSIONS'
$FILE_NOT_BAD:
$	ERROR = ""			!reset error condition
$GOOD_FILE:
$	IF .NOT. EDIT_FILE THEN GOTO DO_NOT_EDIT_FILE
$	OLD = F$SEARCH(VALUE)
$	PROMPT = "  ''F$ELEMENT(OLD.NES."","/","Create/Edit")' " + VALUE
$	GOSUB READ_YES_NO
$	IF .NOT. YES THEN GOTO GET_FILE_AGAIN
$	DISPLAY "  Version limit for the file will be set to ''VERSION_LIMIT'."
$	SPEC = VALUE
$	IF OLD .NES. "" THEN GOSUB CONTROL_VERSIONS	!if file already exists
$	RDT = F$SEARCH(VALUE)
$	IF RDT .NES. "" THEN RDT = F$FILE_ATTRIBUTES(VALUE,"RDT")
$	DEFINE/USER SYS$INPUT SYS$COMMAND:
$	EDIT 'VALUE'
$	IF F$SEARCH(VALUE) .NES. "" -
	  THEN IF F$FILE_ATTRIBUTES(VALUE,"RDT") .NES. RDT THEN GOTO FILE_EDITED
$	ERROR = "A new version was not created."
$	RETURN
$FILE_EDITED:
$	IF OLD .EQS. "" THEN GOSUB CONTROL_VERSIONS	!if file did not exist
$	RETURN
$DO_NOT_EDIT_FILE:
$	IF VALUE .NES. USER_VALUE THEN IF MODE .NES. "RESTART" -
	  THEN DISPLAY "  Resultant spec:  ",VALUE
$	RETURN
$GET_FILE_AGAIN:
$	ERROR = " "			!an error, but not to be displayed
$	RETURN
$!-----------------------------------------------------------------------------
$!
$! Subroutine to read the answer to a yes/no question.  If not truly in
$! interactive mode, a "YES" response is assumed.
$!
$READ_YES_NO:
$	IF F$MODE() .NES. "INTERACTIVE" THEN GOTO ASSUME_YES
$	READ/END=ABNORMAL_EXIT SYS$COMMAND YES -	!avoid long line
	  /PROMPT="''PROMPT' [No]? "
$	IF F$EXTRACT(0,1,YES) .EQS. "!" THEN GOTO READ_YES_NO
$	IF F$LOCATE(","+F$EDIT(YES,"TRIM,UPCASE"),",YES,NO") .LT. 7 THEN RETURN
$	DISPLAY F$FAO( "!#* Enter ""Yes"" or ""No"" (or Control-Z).", -
	  F$LENGTH(PROMPT+NUL)-F$LENGTH(F$EDIT(PROMPT+NUL,"TRIM"))+2 )
$	GOTO READ_YES_NO
$ASSUME_YES:
$	YES = 1
$	RETURN
$!-----------------------------------------------------------------------------
$!
$! Subroutine to give instructions for entering parameter values.
$!
$HELP__GENERAL:
$	DI = DISPLAY + F$FAO(" ""!#* "",",2)
$	DI "When supplying parameter values, you may enter:"
$	DI "  an explicit (new) value,"
$	DI "  a null to use the existing value shown in brackets,"
$	DI "  one or more spaces to delete the existing value,"
$	DI "  ""^D"" (a circumflex and ""D"") ", -
	  "to specify the ""factory default"","
$	DI "  ""^"" (a circumflex) to return to the previous question,"
$	DI "  ""?"" (a question mark) to see these instructions again,"
$	DI "  or ""??"" (two question marks) to get parameter-specific help."
$	DI "If supplying the specification for an input file, you may "
$	DI "  append ""/EDIT"" to edit (or create) the corresponding file."
$	DI "You may abort the interaction by entering Control-Z whenever"
$	DI "  you are prompted for input."
$	DI "Additional information can be found by reading the comments at"
$	DI "  the beginning of this procedure."
$	RETURN
$!-----------------------------------------------------------------------------
$!
$! Subroutine to find the next time following a specified time at which the
$! procedure is to run.
$!
$FIND_NEXT_TIME:
$	BASE = "''F$CVTIME(BASE_TIME,,"TIME")'"
$	VS = 'F$VERIFY(0)' + F$VERIFY(0'VERIFY_VAR' .GT. 0)	!usually off
$	LIST = ABS_TIME_LIST
$COMPARE_NEXT_TIME:
$	TIME = "''F$ELEMENT(0,",",LIST)'"
$	LIST = LIST - TIME - ","
$	IF "''F$CVTIME(TIME,,"TIME")'" .GTS. "''BASE'" THEN GOTO FOUND_NEXT_TIME
$	IF LIST .NES. "" THEN GOTO COMPARE_NEXT_TIME
$	TIME = "1-''F$ELEMENT(0,",",ABS_TIME_LIST)'"
$FOUND_NEXT_TIME:
$	TIME = F$CVTIME( -
	  "''F$CVTIME(BASE_TIME,"ABSOLUTE","DATE")'+''TIME'","ABSOLUTE" )
$	V = 'F$VERIFY(0)' + F$VERIFY(VS)	!restore verify
$	NEXT_TIME = "''F$EXTRACT(0,F$LOCATE(":",TIME)+3,TIME)'"
$	RETURN
$!-----------------------------------------------------------------------------
$!
$! Subroutine to (re-)submit this procedure.
$!
$SUBMIT_PROCEDURE:
$	SPEC = LOG_FILE
$	GOSUB CONTROL_VERSIONS
$	SUBMIT_FILE = "''F$SEARCH( F$PARSE(";",PROC_FILE) )'"
$	IF SUBMIT_FILE .EQS. PROC_FILE THEN GOTO SUBMITTING_SAME_PROC
$	DATE = "''F$ELEMENT(0," ",AFTER_TIME)'"
$	TIME = "''F$EXTRACT(0,F$LOCATE(":",AFTER_TIME)+3,AFTER_TIME)'" -
	  - DATE - " 0" - " "
$	OPEN/WRITE WM_MESSAGE 'MESSAGE_FILE'
$	WO = "WRITE WM_MESSAGE"
$	W = "''WO' ""  "","
$	W "A different (and presumably newer) version of ''PROC_NAME' is about"
$	W "to be submitted.  Assuming that it is compatible with the version"
$	W "now executing and that it does not introduce any new bugs, the"
$	W "changeover should not cause any problems.  You may wish to examine"
$	W "the edit history within the procedure to see what has changed."
$	WO ""
$	W "Sometime after ''TIME' on ''DATE', when this different version of"
$	W "the procedure is scheduled to begin execution, you should receive"
$	W "an additional (and somewhat redundant) message informing you that"
$	W "it differs from the version of the procedure which is now in use."
$	WO ""
$	WO "Executing:   ''PROC_FILE' -- ''VERSION'"
$	WO "Submitting:  ''SUBMIT_FILE'"
$	CLOSE WM_MESSAGE
$	SPEC = MESSAGE_FILE
$	ERROR_MESSAGE = -
	  "I-DIFFPROC, submitting a different version of the procedure."
$	GOSUB SEND_MESSAGE_AND_FILE
$	ERROR_MESSAGE = ""		!reset error flag in case of a real one
$SUBMITTING_SAME_PROC:
$	DISPLAY ""
$	SUBMIT/NOPRINT/NOTIFY/RESTART -
	  /AFTER="''AFTER_TIME'" -
	  /QUEUE='BATCH_QUEUE' -
	  /'F$ELEMENT( LOG_FILE.EQS."", NUL, "LOG="+LOG_FILE+NUL+"NOLOG" )' -
	  /PARAMETERS=("''PARAMETER_FILE'","''VERSION_KEYWORD'''VERSION'") -
	    'SUBMIT_FILE'
$	SUBMITTED = 1			!remember that procedure is still queued
$	RETURN
$!-----------------------------------------------------------------------------
$!
$! Subroutines to mail (or display) error messages to the user.
$!
$SEND_MESSAGE_AND_ABORT:
$	GOSUB SEND_MESSAGE
$	GOTO ABNORMAL_EXIT
$SEND_MESSAGE:
$	SPEC = "_NLA0:"
$SEND_MESSAGE_AND_FILE:
$	IF F$MODE() .EQS. "BATCH" THEN GOTO BATCH_MESSAGE	!real mode
$	STRING = ERROR_MESSAGE - F$ELEMENT(0," ",ERROR_MESSAGE) - " "
$	BDISPLAY F$EDIT(F$EXTRACT(0,1,STRING),"UPCASE"), F$EXTRACT(1,999,STRING)
$	GOTO MESSAGE_DELIVERED
$BATCH_MESSAGE:
$	STRING = "%" + PROC_NAME + "-" + ERROR_MESSAGE
$	IF STRING - """" .NES. STRING THEN GOSUB DOUBLE_QUOTES
$	MAIL 'SPEC' 'THIS_USER' -	!avoid long line
	  /SUBJECT="''STRING'"
$MESSAGE_DELIVERED:
$	RETURN
$!-----------------------------------------------------------------------------
$!
$! Subroutine to double quotes.
$!
$DOUBLE_QUOTES:
$	QUOTE = -2
$FIND_NEXT_QUOTE:
$	QUOTE = 'F$LOCATE( """", F$EXTRACT(QUOTE+2,999,STRING) )' + 'QUOTE' + 2
$	STRING = F$EXTRACT(0,QUOTE+1,STRING) + F$EXTRACT(QUOTE,999,STRING)
$	IF QUOTE + 2 .LT. F$LENGTH(STRING) THEN GOTO FIND_NEXT_QUOTE
$	RETURN
$!-----------------------------------------------------------------------------
$!
$! Subroutine to control the number of versions of a created file.
$!
$! This subroutine must be called before creating a new version of a created
$! file, in order that the current value of the version limit will be used.
$!
$! Setting the version limit on the log file while in batch mode would get
$! an "access conflict" error because the file is open, so that the versions
$! of the log file are controlled instead either by the version limit having
$! been set during the interactive phase or by explicit purging of the file.
$!
$CONTROL_VERSIONS:
$	IF SPEC .EQS. "" THEN GOTO VERSIONS_OK
$	IF "''F$SEARCH(F$PARSE(";-"+VERSION_LIMIT,SPEC))'" .EQS. "" -
	  THEN GOTO FILE_PURGED
$	IF VERSION_LIMIT .LE. 0 THEN GOTO FILE_PURGED
$	DISPLAY ""
$	PURGE/LOG/KEEP='VERSION_LIMIT' 'F$STRING( F$PARSE(";",SPEC) - ";" )'
$FILE_PURGED:
$	IF MODE .EQS. "BATCH" THEN IF F$TYPE(LOG_FILE) .NES. "" -
	  THEN IF SPEC .EQS. LOG_FILE THEN GOTO VERSIONS_OK
$	ALL_VERSIONS = F$PARSE(";*",SPEC)
$	IF F$SEARCH(ALL_VERSIONS) .NES. "" THEN GOTO FILE_EXISTS
$	IF VERSION_LIMIT .LE. 0 THEN GOTO VERSIONS_OK
$	CREATE 'SPEC'
This version of the file was created so that a SET FILE/VERSION_LIMIT
command could be used to establish a limit on the number of versions.
It will not be needed after other versions exist, and will be deleted
when an attempt is made to exceed the version limit.
$!
$FILE_EXISTS:
$	SET FILE/VERSION_LIMIT='VERSION_LIMIT' 'ALL_VERSIONS'
$VERSIONS_OK:
$	RETURN
$!-----------------------------------------------------------------------------
$!
$! Subroutine to search a file for a sender's address, with routing node
$! information and alphabetic case being ignored (except within quotes).
$! Lines whose first non-blank character is an exclamation point (!) are
$! also ignored.  An asterisk (*) may be specified for the node and/or
$! username, allowing a match on any node or username respectively.
$!
$SEARCH_FILE:
$	FOUND = 0
$	IF SPEC .EQS. "" THEN GOTO NO_SEARCH_FILE
$	OPEN/READ WM_SEARCH 'SPEC'
$DISABLE_SEARCH_VERIFY:
$	VS2 = 'F$VERIFY(0)' + F$VERIFY(0'VERIFY_VAR' .GT. 0)	!usually off
$GET_SEARCH_LINE:
$	READ/END=FILE_SEARCHED WM_SEARCH LINE
$	ADDRESS = F$EDIT(LINE,"UPCASE,TRIM")
$	IF ADDRESS - S_NODE - S_NAME - "*" .EQS. ADDRESS -
	  .OR. F$EXTRACT(0,1,ADDRESS) .EQS. "!" THEN GOTO GET_SEARCH_LINE
$	GOSUB REMOVE_ROUTING
$	A_NODE = F$EXTRACT(0,NLEN,ADDRESS)
$	A_NAME = ADDRESS - A_NODE
$	IF A_NAME - S_NAME - "*" .NES. "" THEN GOTO GET_SEARCH_LINE
$	IF A_NODE - S_NODE - "*::" .NES. "" THEN GOTO GET_SEARCH_LINE
$	V = 'F$VERIFY(0)' + F$VERIFY(VS2)	!restore verify
$	FOUND = 1
$FILE_SEARCHED:
$	V = 'F$VERIFY(0)' + F$VERIFY(VS2)	!restore verify
$	CLOSE WM_SEARCH
$NO_SEARCH_FILE:
$	RETURN
$!-----------------------------------------------------------------------------
$!
$! Subroutine to remove routing node information from a DECnet mail address
$! and also return the length of the node name (including the "::" delimiter).
$! Note that a node name may not be present, and that portions of the address
$! specification may be quoted.
$!
$REMOVE_ROUTING:
$	NLEN = 'F$STRING( F$LOCATE("::",ADDRESS) + 2 )'
$	IF 'NLEN' .GT. 'F$LENGTH(ADDRESS)' THEN NLEN = 0
$	IF ADDRESS - "::" - "::" - """" .EQS. ADDRESS - "::" THEN RETURN
$	VS = 'F$VERIFY(0)' + F$VERIFY(0'VERIFY_VAR' .GT. 0)	!usually off
$	POS = 0
$	NPOS = POS
$FIND_NEXT_NODE:
$	PNPOS = 'NPOS'
$	NPOS = 'POS'
$QUOTED_ADDRESS:
$	V = 'F$VERIFY(0)' + F$VERIFY(VS)	!restore verify
$	QPOS = 'F$STRING( F$LOCATE("""",F$EXTRACT(POS,999,ADDRESS)) + POS + 1 )'
$	POS = 'F$STRING( F$LOCATE("::",F$EXTRACT(POS,999,ADDRESS)) + POS + 2 )'
$	VS = 'F$VERIFY(0)' + F$VERIFY(0'VERIFY_VAR' .GT. 0)	!usually off
$	IF POS .LE. QPOS THEN GOTO FIND_NEXT_NODE
$	IF POS .GT. F$LENGTH(ADDRESS) THEN GOTO FOUND_LAST_NODE
$	POS = 'F$LOCATE("""",F$EXTRACT(QPOS,999,ADDRESS))' + QPOS + 1
$	GOTO QUOTED_ADDRESS
$FOUND_LAST_NODE:
$	V = 'F$VERIFY(0)' + F$VERIFY(VS)	!restore verify
$	ADDRESS = F$EXTRACT('PNPOS',999,ADDRESS)
$	NLEN = 'F$STRING( NPOS - PNPOS )'
$	RETURN
$!-----------------------------------------------------------------------------
$!
$! Subroutine to open the problem file and add a problem description to it.
$! Additional problem-related information, possibly including binary data,
$! may be added later.  If the file has not previously been opened during
$! this [batch] session, a new file is created and header information is
$! inserted into it.  Version control is only applied when the first version
$! is created, and is set independently of the user-specified limit.
$!
$OPEN_PROBLEM_FILE:
$	IF F$SEARCH(PROBLEM_FILE) .NES. "" THEN GOTO PROBLEM_FILE_EXISTS
$	SAVE_VERSION_LIMIT = 'VERSION_LIMIT'
$	VERSION_LIMIT = 5
$	SPEC = PROBLEM_FILE
$	GOSUB CONTROL_VERSIONS
$	VERSION_LIMIT = 'SAVE_VERSION_LIMIT'
$PROBLEM_FILE_EXISTS:
$	IF 0'PROBLEM_COUNT' .GT. 0 THEN GOTO PROBLEM_FILE_ALREADY_OPENED
$	OPEN/WRITE WM_PROBLEM 'PROBLEM_FILE'
$	WRITE WM_PROBLEM "!"
$	WRITE WM_PROBLEM "! Problem report created by ",PROC_INFO
$	WRITE WM_PROBLEM "! for ",THIS_NODE_USER," on ",NOW,"."
$	IF LOG_FILE .NES. "" -
	  THEN WRITE WM_PROBLEM "! The current log file is ",F$SEARCH(LOG_FILE)
$	WRITE WM_PROBLEM "!"
$	WRITE WM_PROBLEM "! NOTE that this file may contain binary data!"
$	WRITE WM_PROBLEM "!"
$	CLOSE WM_PROBLEM
$PROBLEM_FILE_ALREADY_OPENED:
$	PROBLEM_COUNT = 0'PROBLEM_COUNT' + 1
$	OPEN/APPEND WM_PROBLEM 'PROBLEM_FILE'
$	WRITE WM_PROBLEM F$FAO( "!20*- !AS !20*-", "''F$TIME()'" )
$	WRITE WM_PROBLEM "Problem:  ",PROBLEM
$	RETURN

$!=============================================================================
$NORMAL_EXIT:
$	STATUS = $STATUS			!save success status
$	IF MODE .EQS. "RESTART" THEN IF 0'SUBMITTED' .LT. 1 -
	  THEN BDISPLAY "No more batch jobs need to be queued."
$	GOTO EXIT
$!
$ABNORMAL_EXIT:
$	STATUS = $STATUS			!save failure status
$ABNORMAL_EXIT_2:
$	IF STATUS THEN STATUS = "%X0800002C"	!force error if necessary
$	IF F$MODE() .EQS. "BATCH" THEN SET VERIFY	!in case it was off
$	IF .NOT. 0'CONSTANTS_DEFINED' THEN GOSUB DEFINE_CONSTANTS
$	IF F$MODE() .EQS. "BATCH" .OR. 0'VERIFY_VAR' .GT. 0 -
	  THEN SHOW SYMBOL/ALL
$	IF F$MODE() .NES. "BATCH" THEN GOTO INTERACTIVE_ABEND
$	IF F$TYPE(ERROR_MESSAGE) .NES. "" THEN IF ERROR_MESSAGE .NES. "" -
	  THEN GOTO BATCH_ERROR_SENT
$	ERROR_MESSAGE = -
	  "E-ABEND, abnormal exit -- """ + F$MESSAGE(STATUS) + """"
$	GOSUB SEND_MESSAGE
$BATCH_ERROR_SENT:
$	GOTO ALL_ABENDS
$INTERACTIVE_ABEND:
$	BDISPLAY PROC_NAME," procedure aborted."
$ALL_ABENDS:
$	GOTO EXIT
$!
$EXIT:
$	IF "''F$TYPE(SAVE_PRIVS)'" .NES. "" -
	  THEN SET PROCESS/PRIVILEGES=('SAVE_PRIVS')
$	IF 0'SUBMITTED' .GE. ( MODE .EQS. "BATCH" ) -
	  THEN GOTO BATCH_JOB_NOT_TERMINATED
$	IF F$SEARCH(PROBLEM_FILE) .EQS. "" THEN GOTO NO_NEW_PROBLEMS
$	IF "''ORIGINALLY_INVOKED'" .NES. "" -
	  THEN IF F$CVTIME(F$FILE_ATTRIBUTES(PROBLEM_FILE,"CDT")) -
	    .LTS. F$CVTIME(ORIGINALLY_INVOKED) THEN GOTO NO_NEW_PROBLEMS
$	OPEN/WRITE WM_MESSAGE 'MESSAGE_FILE'
$	WO = "WRITE WM_MESSAGE"
$	W = "''WO' ""  "","
$	W "There appear to have been some problems running ",PROC_NAME,"."
$	W "As a result, you may have some new mail that was not processed."
$	WO ""
$	W "The problem reports can be found in ",PROBLEM_FILE,"*."
$	W "It is recommended that you use DUMP/RECORD to examine these"
$	W "files, as they may contain binary data.  Alternatively, you can"
$	W "SEARCH them for ""Problem:"".  Additional information may also"
$	W "be found in the parameter file, optional log file(s), etc."
$	WO ""
$	W "Because a subsequent invocation of the procedure may result in"
$	W "many of these files being purged, it is recommended that you NOT"
$	W "run the procedure again until all possible information has been"
$	W "gleaned from them."
$	WO ""
$	W "After the problems have been resolved, the problem reports should"
$	W "be deleted (or renamed) so that you will not continue to be"
$	W "reminded of their existence."
$	WO ""
$	W "If you need assistance, try contacting the maintainers of the"
$	W "procedure (",PROC_FILE,")."
$	CLOSE WM_MESSAGE
$	SPEC = MESSAGE_FILE
$	ERROR_MESSAGE = "W-PROBLEMS, one or more problem reports exist."
$	GOSUB SEND_MESSAGE_AND_FILE
$NO_NEW_PROBLEMS:
$	ERROR_MESSAGE = -
	  "I-CHECKMAIL, remember to check your ''MOVETO_FOLDER' mail folder."
$	GOSUB SEND_MESSAGE
$BATCH_JOB_NOT_TERMINATED:
$	CLOSE WM_BATCHINFO
$	CLOSE WM_COMMAND
$	CLOSE WM_HISTORY
$	CLOSE WM_MAILFILE
$	CLOSE WM_MAILINFO
$	CLOSE WM_MESSAGE
$	CLOSE WM_PARAMS
$	CLOSE WM_PROBLEM
$	CLOSE WM_SEARCH
$	CLOSE WM_VERSION
$	IF "''F$SEARCH(TEMP_FILES)'" .NES. "" -
	  THEN DELETE'F$ELEMENT(MODE.EQS."BATCH",";",";/LOG")' 'TEMP_FILES'
$	EXIT ('STATUS'.OR.%X10000000) + F$VERIFY(SAVE_VERIFY,SAVE_VERIFY/2)*0
