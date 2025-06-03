WATCH_MAIL, UTILITIES, Automatically reply to (and/or copy) new mail


  --- DESCRIPTION ---

The purpose of this procedure is to automatically reply to new mail
with a specified message for a specified time interval, and possibly
also copy that mail to one or more other users.  For example, if you
are on vacation or away on business, the replies can be used to let
others know that you won't be able to respond to their mail for some
period of time, and also possibly inform them of other people they
might consider contacting in your absence.  The copies (if enabled)
might be checked by someone else to see if something urgent comes up
in your absence.

  --- INSTALLATION ---

Simply copy the procedure to a private or shared directory:

    $ COPY/LOG  FREEWARE$CD:[WATCH_MAIL]WATCH_MAIL.COM  <destination>

The advantage of a shared location is that it allows multiple people
to use the procedure without incurring the disk space costs of multiple
copies.  The procedure provides several features for facilitating the
use and management of such a shared copy.

  --- USAGE ---

Invoke the procedure interactively (e.g., @WATCH_MAIL), answering the
questions and providing input files as requested.  A batch job will
then be submitted to perform some additional checking and subsequent
mail processing.

Have a nice vacation (or whatever)!

Upon your return, new mail that has been successfully processed by
this procedure will be found in your WATCH_MAIL mail folder.  You may
also have new mail that has come in since the last time the batch job
ran, or to which the procedure has had difficulty replying.

  --- ADVANTAGES ---

The procedure is installed by simply copying the command file, and no
privileges are required to install or use it.  It allows group- and
system-specific tailoring of some default parameters, as may be desired
by a system manager.

There are several features to "delight" the user and system manager,
as detailed in the internal documentation (e.g., specify dates using
weekday notation ("Monday+8:00"), create or edit the reply file after
supplying its specification ("/EDIT"), etc.).

  --- LIMITATIONS ---

The chief limitation is that system crashes and batch queue-related
problems may result in the batch job disappearing, and hence the loss
of automatic replies to and copies of your new mail.

  --- MORE INFORMATION ---

See the extensive documentation contained in the comments within the
command file.  In particular, see the section on known restrictions
and bugs.

On-line help is also available by entering a question mark (?) in
response to one of the procedure's prompts.
