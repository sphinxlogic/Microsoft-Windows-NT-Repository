ROSEGARDEN, MISCELLANEOUS, Musical Notation Editor and Midi Sequencer

Rosegarden 2.1
==============

This is the Rosegarden Editor and Sequencer suite, release 2.1.

See COPYRIGHT for information about copyrights, and COPYING for
information about redistribution and (the lack of) warranty.

See INSTALL for details of how to install this distribution.

Chris Cannam, cannam@zands.com        October 1997

Rosegarden V 2.1 port under OpenVMS                       January 1998
===================================

Rosegarden provides a Musical Notation Editor and a Midi Sequencer on various
Unixes and now on VMS systems !!

Usage:
------
Rosegarden is composed of 3 executables, Rosegarden.exe (also called the
Topbox), Editor.exe and Sequencer.exe . Rosegarden.exe lets you execute the
Musical Notation Editor, the Sequencer and access to the Help system.

The kit contains binaries for VAX and Alpha, built under VAX VMS 6.1 and Alpha
VMS 6.2

You need to copy ROSEGARDEN.DAT ressource file under your SYS$LOGIN directory
or under SYS$COMMON:[DECW$DEFAULTS.USER] directory.

On VAX, you need to rename *.EXE_VAX files into *.EXE

Now, run SETUP_VMS.COM procedure to define logicals and the rosegarden symbol.

You invoke Rosegarden by:

$ rosegarden

You can also run the Editor and the Sequencer as stand-alone programs using
roseditor and rosesequencer command. However, the help system is only
available through rosegarden "topbox" application.

The Sequencer can be used as an X11 interface for Timidity, the nice Midi
player you must use on VMS. Timidity for VMS can be found at my site (and its
mirrors) at urls:

 http://www2.cenaath.cena.dgac.fr/ftp/decwindows/timidity-02i.zip

You also need instrument samples (also called patches):

 http://www2.cenaath.cena.dgac.fr/ftp/decwindows/timidity_base_samples.zip

 http://www2.cenaath.cena.dgac.fr/ftp/decwindows/timidity_instruments_samples.zip
 http://www2.cenaath.cena.dgac.fr/ftp/decwindows/timidity_drums_samples.zip
 http://www2.cenaath.cena.dgac.fr/ftp/decwindows/timidity_wow_samples.zip
 http://www2.cenaath.cena.dgac.fr/ftp/decwindows/timidity_new_samples.zip

Unzip the patches archives in the same order. The best configurations files
(using selected and tested patches from the sets) are in the last one. The
total size of patches is about 15 Mo (the price to pay for a good quality midi
software only player) but the result is really nice.

Rebuild:
-------
You need an X11R5 XAW and XMU library. You can find it at my site and its
mirrors at urls:

  http://www2.cenaath.cena.dgac.fr/ftp/decwindows/x11r5_xaw_xmu.zip
  ftp://ftp2.cnam.fr/decwindows/x11r5_xaw_xmu.zip
  ftp://ftp.wku.edu/vms/decwindows/x11r5_xaw_xmu.zip

The definition of X11R5 logical names is done into MAKE_*.COM files. You must
customize accordingly to your own installation. By default, I invoke
[X11R5]LOGICALS.COM procedure

To rebuild on VAX, you type:

$ @setup_vms
$ @make_vax

To rebuild on Alpha, you type:

$ @setup_vms
$ @make_alpha

At the end of the build, the 3 executables EDITOR.EXE, SEQUENCER.EXE and
TOPBOX.EXE are moved to [.BIN] directory

If you just want to relink, you'll find LINK_AXP.COM (for Alpha) and
LINK_DECC.COM (for VAX) procedures into [.EDITOR.SRC], [.SEQUENCER.SRC] and
[.TOPBOX.SRC] directories.

Modifications and adaptations:
-----------------------------
The BUFFER_INITIAL_SIZE for Midifiles is now 400 Kbytes to handle big files.
(in [.MIDI.INCLUDE]MIDIFILE.H)

Sub processes are spawned via LIB$SPAWN in NOWAIT state. An AST routine is
used to handle sub process termination. (I don't use C/Unix signals).

Subprocesses are used by TOPBOX to execute the Editor and the Sequencer. The
Sequencer also uses a subprocess to play midi files via an external player
(timidity on VMS).

The ressource file for VMS is ROSEGARDEN.DAT . Note that you need to provide
either a command name or an "mc file_specification" for Editor and Sequenceer
executables (because I use lib$spawn):

!!! Paths, edit these

Rosegarden*editorName:          mc ROSEGARDEN_DIR:[bin]editor
Rosegarden*sequencerName:       mc ROSEGARDEN_DIR:[bin]sequencer
Rosegarden*helpFile:            ROSEGARDEN_DIR:[common.help]rosehelp.info
Rosegarden*midiFmPatchFile:     ROSEGARDEN_DIR:[common.synth-patches]std.sb
Rosegarden*midiFmDrumPFile:     ROSEGARDEN_DIR:[common.synth-patches]drums.sb
Rosegarden*sequencerName:       mc ROSEGARDEN_DIR:[bin]sequencer
Rosegarden*helpFile:            ROSEGARDEN_DIR:[common.help]rosehelp.info
Rosegarden*midiFmPatchFile:     ROSEGARDEN_DIR:[common.synth-patches]std.sb
Rosegarden*midiFmDrumPFile:     ROSEGARDEN_DIR:[common.synth-patches]drums.sb

You need to put your preferred Timidity options in the ressource file:

!!! this is for the sequencer's "Play through Slave":

Rosegarden*externalPlayer:      timidity "-a" "-s32000"


Bugs/Limitations:
----------------
This port has some limitations, mainly in the Interlock system and the file
browser:

The file browser is ok with files but not yet with directories.

The Interlock library is not ported on VMS. The Editor and the Sequencer cannot
communicate without explicit action on the both sides (for example you export
to a midi file in the editor and you read that file in the sequencer. Not a big
deal). If someone has some spare time to implement Interlock library ...

The Help system is only available in Rosagarden aplication (the topbox) and not
into the Editor or the Sequencer.

Filters in TCL are not tested under VMS.

Enjoy !!

Patrick Moreau
pmoreau@cena.dgac.fr
moreau_p@decus.fr
