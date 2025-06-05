GNM, LANGUAGES, Common-sources tool for messages and documentation

This is GNM V2.0.

The GNM compiler allow you to have a common source file for both an 
OpenVMS message file (.MSG) and for DOCUMENT (.SDML) documentation.

GNM V2.0 is an overhaul of previous GNM versions, with improvements 
in the tool and its operations.  The command operations also differ 
from previous versions, and particularly with better diagnostics and
with a single executable image now used for both .MSG and .SDML output.

Here is the basic foreign-command sequence:

  $ gnm :== $ddcu:[dir]gnm.exe
  $ gnm gnm-in-file.gnm msg-out-file.msg sdml-out-file.sdml

GNM converts a .GNM file into a standard OpenVMS message file, which
can then be processed by the MESSAGE compiler and then (optionally)
with MESSAGE/SDL and the SDL tool to create language-specific include 
files.  GNM also converts the .GNM file into an .SDML file for use
with DOCUMENT.

The .GNM file can be thought of as a series of directives:

   .NAME	    - The message symbol without the VDE$_
   .MESSAGE	    - The message text, it may be wrapped
   .EXPLANATION    - The description that appears in the document
   .USER_ACTION    - The recovery section that appears in the document.
   .BREAK	    - May be used to start a new paragraph in the
		      .explanation or .user_action
   .DESTINATION    - Which file the following appears in, parameters are
		      both, message
   .SEVERITY       - informational, success, warning, error, or fatal

The first 4 directives: .name, .message, .explanation, .user_action,
occur as a group in that order for each message.  .explanation and
.user_action are optional.  The FAO arguments within the .message
are contained within <>.  Within the <> there are 2 arguments, the
first is a word which will be emphasized in that position in the
document, the second is the FAO argument which will appear in the
message file in that position.  

The .destination directive must appear between message groups and is
in effect until another .destination directive occurs.  Thus, when
one message is to goto the message file only a .destination message
will precede the message group and a .destination both will follow
the group.  

.FACILITY and .END frame the contents of the GNM file.  The arguments
on the .FACILITY directive indicate the facility name and the facility
number, and the directive also accepts qualifiers to pass through to 
the MESSAGE file .FACILITY directive.

Use spaces for the indentation of the message text and explanations.

The following is an example of a GNM file:


.facility	facnam,facnum /PREFIX=prefix$_
!
! Success messages go here.  DO NOT REORDER.
!
.severity success
.destination message

.name	    SUCCESS1
.message	successful
.explanation	This message indicates success
.user_action	None required
!
! Informational messages go here.  DO NOT REORDER.
!
.base 500
.severity informational
.destination both

.name	    INFO1
.message	information <text, !AC> is displayed
.explanation	This message informs you of something, and shows how
		to include some text in the message using a standard
                FAO directive.
.user_action	None required

!
! Warning messages go here.  DO NOT REORDER.
! 
.base 1000
.severity warning
.destination both

.name	    WARNING1
.message	ambiguous '<text,!AC>'
.explanation	Cannot complete the command because '<EMPHASIS>(text)' 
		is ambiguous in the context of this command.
.user_action	None required

!
! Error messages go here.  DO NOT REORDER.
!
.base 1500
.severity error
.destination both

.name	    ERROR1
.message	whoops, something unusual this way walked
.explanation	a part of the magical unknown mystery tour,
                with seats still available.
.user_action	Wave a rubber chicken over the application.
                If that fails, upgrade to Rubber Chicken V2.0.

!
! Fatal messages go here.  DO NOT REORDER.  Very few messages should
! be of fatal severity.
!
.base 2000
.severity fatal
.destination both

.name	    FATAL1
.message	a fatal crash has occured, film at 11
.explanation	unknown fatal mystery error
.user_action	Dance for the computer.  It might help.

.end
