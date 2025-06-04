This directory tree contains the sources and documentation for the Majordomo
portion of the VMS Majordomo kit. Note that pains are taken to say "VMS
Majordomo" when we are talking about the port of Majordomo from Unix. We say
"Unix Majordomo" when we are talking about the Unix based code from which VMS
Majordomo was ported. When we are talking about something that is the same for
both Unix and VMS the unqualified term "Majordomo" is used.

Since this is a field test version this documentation is incomplete. Sorry.
However if you are not familiar with Majordomo the Unix Majordomo documentation
will help you get started and it is strongly recomended that you look through
it. Although some of the implementation details differ the commands served by
VMS Majordomo are the same as those served by its Unix parent. Further, each
Perl script for VMS Majordomo does the same thing as its corresponding Unix
Majordomo Perl script. In general there are many more similarities than
differences since every effort was made to make the port a "close" one which
will be evident to those familiar with Unix Majordomo. 

The most useful documentation for Unix Majordomo from V1.93 has been extracted
and placed in [.UNIXDOC]. You are urged to look at it if you are unfamiliar with
Majordomo.

See the file LICENSE.TXT in the main directory for VMS Majordomo for information
on where to get the full sources and doc's for Unix Majordomo.


Some Unix Majordomo features have not yet been ported to VMS Majordomo.

Unported features are:

   -  FTPmail relay within archive function
   
Unported perl scripts are:

   -  BOUNCE
   -  BOUNCE-REMIND
   -  MEDIT
   -  NEW-LIST


|New features for this release:
|
|   -  support for new MAILPIPE Security Registry file
|      All this entails is the new INSTALL_PIPE_REGISTRY.COM file
|   -  bug fixes. See changelog.txt

Implementation differences between VMS Majordomo and Unix Majordomo:

The differences discussed in this section are differences in implementation
only. The biggest differences between Unix Majordomo and the VMS port lie in the
way the scripts interact with the OS's mail environment. There are also
differences in how Majordomo is configured.  No effort is made to document every
last change - only the changes that have some significance.

The differences described here should not affect the workings of Majordomo from
the perspective of the list owner or list member. They will affect the view of
Majordomo that the Majordomo owner has although the reader familiar with Unix
Majordomo should feel quite at home with VMS Majordomo providing they are
familiar with VMS.

   -  How "aliases" are set up.
      Unix Majordomo makes heavy use of the Unix alias file to configure lists
      and Majordomo itself. Fortunately VMS has something analogous to the Unix
      alias file, the SET FORWARD/USER command. So, where the Unix version of
      Majordomo uses an entry in the alias file the VMS version uses a SET
      FORWARD/USER command.

      For many of the aliases there is a direct mapping between an alias entry
      and a SET FORWARD/USER command. For example, the Unix alias entry:

         majordomo-owner: jerry

      maps directly to VMS mail command:
      
         SET FORWARD/USER=majordomo-owner jerry

      However Unix Majordomo makes use of two features of the alias file that,
      to our knowledge, were not previously available on VMS. To support these
      two features new code was written for VMS Majordomo. The two features are
      the ability to pipe a mail message being delivered to an alias to a
      program and the ability to make an alias "include" a list of addresses
      from a file. Following are examples of such Unix alias file entries. The
      first sets up an alias file entry for the majordomo alias itself. It
      causes any mail sent to "majordomo" to be piped to the Majordomo "wrapper"
      program which in turn pipes it to the "majordomo" Perl Script. The second
      causes mail to "sample-outgoing" to be sent to all of the addresses
      specifed in the file specified.

         majordomo: "|/usr/local/majordomo/bin/wrapper majordomo"
         ...
         sample-outgoing:
            :include:/usr/local/majordomo/lists/sample

      The code in this kit has support for these two mechanisms. The PIPE% VMS
      mail foreign transport supports "piping" mail to a program as a stream
      of bytes and is described in the [-.PIPE] directory. Support for the
      include file option was added in to SMTP code itself and is discussed in
      the [-.SMTP] directory.

      So, for all of the Unix style aliases there is a corresponding SET
      FORWARD/USER done. VMS Majordomo provides some scripts to set the VMS
      style aliases up for you so you don't have to do it manually. They are
      described elsewhere in this file.

      -  Difference in how mail passed to DIGEST script.
 
         -  With Unix Majordomo you set up the aliases as follows to cause each
         message sent to the list to also go to the digest:
 
            listname-outgoing: :include:/usr/local/mail/lists/listname, listname-digestify
            listname-digestify: "|/usr/local/mail/majordomo/wrapper digest -r -C"
 
 
         Unfortunately there is no way under VMS mail to achieve what the alias
         listname-outgoing is doing here. It is sending the mail to the users in
         the file /usr/mumble/listname *and* to the alias listname-digestify
         which is in turn going to the digest script. With VMS Majordomo this is
 	 accomplished by adding the listname-DIGEST alias to the list's list
 	 file. For example if list "test" is to have a digest then the alias for
 	 that digest - "test-digest" is added to the TEST. file. This way, each
 	 time a mail is delivered to the list it is also delivered to the
 	 test-digest alias (which invokes the digest script.)
 	 
   -  How a Majordomo Perl script passes a mail message to the mailer for
   delivery.
      Unix Majordomo uses the Unix sendmail interface to pass a mail message
      that it has created (or a message that it has processed and wants to
      forward on) to the Unix mailer to "explode". This interface allows
      Majordomo complete control over the headers in a message. Unfortunately
      VMS callable mail is insufficient to achieve this. With VMS Majordomo the
      Perl scripts pass mail using the SFF facility described in [-.SFF].

   -  No wrapper program
      -  The Unix Majordomo program includes a special intermediary program to
      bridge between an alias file alias and a Perl script. This wasn't need on
      VMS.

   -  Most of stuff defined for Unix in majordomo.cf is either not yet supported
   or defined in UCX$MAJORDOMO_SYSTARTUP.COM. See section on "Configuration of
   VMS Majordomo"
   

Installing VMS Majordomo

Assuming you've already installed the other required pieces you can now install
VMS Majordomo. It is suggested you read through this entire file first. Then
install Majordomo by following these steps. Finally configure Majordomo
according to the directions listed in Configuring Majordomo and Majordomo lists.


   1) Manually create the account for VMS Majordomo to run in:

      -  The account must be named "UCX$MAJORDOM". (Notice the absence of the
      last "O". Ie. it's "UCX$MAJORDOM" not "UCX$MAJORDOMO".)
      -  The account *must* be in the same group as UCX_SMTP.
         -  So files created by VMS Majordomo can be read by UCX_SMTP.
      -  The account must have SYSPRV defined as a defpriv.
         -  This is because the UCX SMTP SFF capability requires it.
      -  The account must have full batch access but doesn't need network,
      local, dial-up or remote access.
      -  If you want VMS Majordomo to run cluster wide then make the device name
      for the account a cluster wide device. Eg. Use SYS$COMMON or USER$DISK
      rather than SYS$SPECIFIC or SYS$SYSDEVICE. Note that if you want to run
      VMS Majordomo cluster wide you need to install Perl, SFF and PIPE on all
      your cluster nodes. Nonetheless it is recommended that you put the
      UCX$MAJORDOM account on a cluster wide device. Also note that you do not
      have to have the UCX$MAJORDOM account on the system disk.

      If you put UCX$MAJORDOM on a system specific device you'll need to run
      INSTALL for VMS Majordomo on each node. (But you don't *want* to put it on
      a system specific disk *do* you? ;^)
      
      -  The following quotas are in use here on our test system and seem to be
      working

         Maxjobs:         0  Fillm:        50  Bytlm:       108000
         Maxacctjobs:     0  Shrfillm:      0  Pbytlm:           0
         Maxdetach:       0  BIOlm:       105  JTquota:       8192
         Prclm:          10  DIOlm:       100  WSdef:          350
         Prio:            8  ASTlm:       100  WSquo:          512
         Queprio:         0  TQElm:       100  WSextent:       512
         CPU:        (none)  Enqlm:       100  Pgflquo:      10240

      -  When running a batch job with the username pointing to UCX$MAJORDOM I
      had to do a

         UAF> MODIF UCX$MAJORDOM/FLAG=NORESTRICT

         to get the batch job to run right even though the account has batch
         access. Without this change the job would terminate with:

         %DCL-E-NOCMDPROC, error opening captive command procedure - access denied

         I don't understand VMS security well enough to know why this is
         happening. I put it off to figure out what was going on here until
         later. If you think you know then please send mail to
         majordomo-owner@ucx.lkg.dec.com.

      -  Where the VMS Majordomo Perl scripts reside:
        The VMS Majordomo Perl scripts all sit in the UCX$MAJORDOM account's
        home directory. Hence the requirement that if you set up the account in
        a system specific area you need to run the VMS Majordomo's INSTALL.COM
        on each node.

   2)  Set up SYS$MANAGER:UCX$MAJORDOMO_*.COM
       - Copy UCX$MAJORDOMO_STARTUP.COM to SYS$COMMON:[SYSMGR]
          $ COPY UCX$MAJORDOMO_STARTUP.COM SYS$COMMON:[SYSMGR]
       -  Copy UCX$MAJORDOMO_SYSTARTUP.TEMPLATE to SYS$COMMON:[SYSMGR] and
       customize.
          $ COPY UCX$MAJORDOMO_SYSTARTUP.TEMPLATE SYS$COMMON:[SYSMGR]*.COM
          -  After doing this edit SYS$MANAGER:UCX$MAJORDOMO_SYSTARTUP.COM. It
          defines all the VMS Majordomo configuration logicals. You can pretty
	  much put stuff where you want as long as the directories are all owned
	  by UCX$MAJORDOM. The standard (and easiest) thing to do would be to
	  take the default directories specified by the template file but simply
	  change the placeholder device name of MAJDISK$  to the device name
	  where you set up the UCX$MAJORDOM account.
	  
   3)  Manually create the UCX$MAJORDOM account's default directory and the
   lists and digests subdirectory corresponding to the directories you pointed
   the majordomo logicals to in SYS$MANAGER:UCX$MAJORDOMO_SYSTARTUP.COM.. Eg:
 
      $ CREATE/DIREC USER$DISK:[UCX$MAJORDOM]/OWNER=UCX$MAJORDOM
      $ CREATE/DIREC USER$DISK:[UCX$MAJORDOM.LISTS]/OWNER=UCX$MAJORDOM
      $ CREATE/DIREC USER$DISK:[UCX$MAJORDOM.DIGESTS]/OWNER=UCX$MAJORDOM

   4) If allowing at least group read is not your defualt then set the
   directories to have it:

      $ SET FILE/PROT=G:R USER$DISK:[000000]UCX$MAJORDOM.DIR
      $ SET FILE/PROT=G:R USER$DISK:[UCX$MAJORDOM]LIST.DIR

   5) @SYS$MANAGER:UCX$MAJORDOMO_STARTUP.COM
      This should define all the logicals.

   6) Run the "INSTALL.COM" procedure. All this does is copy the sources from
   the kit directory to UCX$MAJORDOMO_HOME.

	@INSTALL.COM

Configuring Majordomo and Majordomo lists.

There are two areas of configuration with respect to Majordomo. Configuration of
Majordomo itself and configuration of the individual lists. This section
discusses both. Configuration of Majordomo itself is the job of the Majordomo
administrator as is the job of creation and deletion of lists. Configuration of
each list is the job of the list owner.

Configuration of VMS Majordomo

VMS Majordomo's version of majordomo.cf:
   
   With Unix Majordomo all configuration of Majordomo itself is done by editing
   the majordomo.cf file. This differs from VMS Majordomo. VMS Majordomo also
   has a majordomo.cf file but for the most part you do not want to change it.
   Most of the stuff that you could change with Unix's majordomo.cf (namely the
   directories) are set up under VMS in SYS$MANAGER:UCX$MAJORDOMO_SYSTARTUP.COM.

   At this time there is one item in the VMS Majordomo majordomo.cf file that
   you must set yourself. It is the $whereami item. (BTW. For those not familiar
   with Perl the majordomo.cf is actually Perl source code and the things it is
   defining are Perl variables.) The $whereami variable should be defined to be
   either the name of the system or, if desired, the UCX SMTP substitute domain.
   The value *must* be fully qualified. For example:

	$whereami = "acme.com";

   Note that in the future we may enhance the code to pick up the UCX SMTP
   substitute domain automatically and put that value into $whereami.

   When you ran INSTALL.COM for VMS Majordomo it copied a file called
   MAJORDOMO.CF_TEMPLATE to your UCX$MAJORDOMO_HOME directory. Before you try to
   run VMS Majordomo you need to manually rename the file to MAJORDOMO.CF and
   edit it. As stated above you need to enter the value for $whereami.

   You may change the following items in VMS's majordomo.cf although you may
   also leave the the system defaults that come with the default majordomo.cf.

      $return_subject
      $majordomo_request
      $analyze_slash_in_address
      $no_x400at
      $no_true_x400

   
++++++++++
VMS Majordomo list files:

Each Majordomo list has four files associated with it. They are:

   -  list file
      -  Contains the address of the list members. When one subscribes their
      address is added to this file.
   -  config file
      -  This is the config file the list owner sees when they do a config
      command. When they do newconfig a new version of the file is created.
   -  password file
      -  This file contains the list password.
   -  info file (optional)
      -  This is a free text file written by the list owner. This text is sent
      to anyone in response to an info command. The file is written by the list
      owner with the newinfo command.

++++++++++
New Perl scripts to ease configruation:

VMS Majordomo includes a bunch of Perl scripts to automate configuring lists.
This is an advantage over Unix Majordomo which forces you to manually edit the
alias file. All of the scripts require SYSPRV and should be run by the
Majordomo owner. They are run directly from the command line. EG:

   $ perl UCX$MAJORDOMO_HOME:CREATE-MAJORDOMO-FWD.

Scripts for configuring *majordom* forwarding:

   CREATE-MAJORDOMO-FWD
      This script sets up the forwarding for all of the Majordomo "aliases". You
      supply this script with the name of the person on the system who "owns"
      (ie. is responsible for) VMS Majordomo.  You *must* run this script in
      order for VMS Majordomo to work.

      For those familiar with Unix Majordomo this script automatically creates
      the SET FORWARD/USER entries that correspond to all the *majordom* aliases
      you need to set up manually in your alias file.
      
   DELETE-MAJORDOMO-FWD
      This script deletes all of the forwarding set up by CREATE-MAJORDOMO-FWD.

Scripts for setting up lists:

   CREATE-LIST
      Run this script each time you want to create a new VMS Majordomo list
      served by your system. It sets up forwarding addresses for the list, the
      list's owners, etc. The script asks you for the following information
      (optional info in square brackets) and then uses it to create the list:

         List name
            -  CREATE-LIST ensures that the list does not already exist as a VMS
            Majordomo list. Note that if you want a list with a digest you do
 	    not type listname-DIGEST here to create the digest. You simply
 	    answer yes to the question about whether you want the list to have a
 	    digest and the digest is created for you automatically.
	 List owner
	    -  Username or email address of owner of the list. The list owner does not
	    have to be the same person as the Majordomo owner. In fact the list
	    owner does not even need an account on the system. Further, the
            "list owner" can be multiple people. The CREATE-LIST examples (which
	    you see when CREATE-LIST queues you for the list owner) show you how
	    to set this up.
	 List password
	    -  Password associated with the list.
 	 [Create digest too?]
 	    -  You are asked if you want the list to have a digest. If you
 	    answer yes you are also asked to enter the digest passwd.
	 [Reply-To header]
	    -  If you want the Reply-To header to be configured in the SET
	    FORWARD/USER entry for the list (rather than the list's config file)
	    you can set that up here.

      List of steps CREATE-LIST takes when creating a new list:

         -  Makes the list's alias entries with the VMS mail SET FORWARD/USER
         command. For those familiar with Unix Majordomo these entries are
         analogous to the aliases you enter manually for each list in the Unix
         alias file.
	 -  Creates the initial list passwd file.
	 -  Creates the initial list file.
            The list file contains the addresses of the list members.

            When you enter the name of the list CREATE-LIST checks to see if the
            name matches the name of an existing UCX SMTP mailing list already
            on your system. It tries to find a list in either UCX$SMTP_COMMON,
            UCX$SMTP_DIS_DIRECTORY (an undocumented pre-cursor to
            UCX$SMTP_COMMON, now obsolete) and SYS$SPECIFIC:[UCX_SMTP]. If it
            does not find an existing UCX SMTP list with the same list name it
            creates an empty initial list file for the new list.

            If CREATE-LIST does find an existing UCX SMTP list matching the new
            list name this is a problem that needs to be resolved.  You cannot
            have the same list name on the same system for two different lists. 
            CREATE-LIST gives you two options. You can either bail out or you
            can convert the existing list to VMS Majordomo. If you bail then
            list creation is aborted. If you choose to convert, the old list
            file is copied from whereever it was found to the new VMS Majordomo
            list file for the list. After it copies the file CREATE-LIST will
            either delete the old file *or* rename it to "listname.DISOLD". (It
            gives you a choice whether to delete or rename.)

            If you are maintaining copies of the same UCX SMTP lists in 
            different system's  SYS$SPECIFIC:[UCX_SMTP] directories *and* you
            have configured VMS Majordomo's UCX$MAJORDOMO_LIST directory to be a
            cluster-wide directory (rather than system specific) then you do not
            need to run CREATE-LIST on more than one cluster node. However you
            *do* need to manually delete (or rename) the old UCX SMTP list files
            in SYS$SPECIFIC:[UCX_SMTP] that have been converted to VMS Majordomo
            on the other systems. When you run CREATE-LIST it will only
            delete/rename the SYS$SPECIFIC:[UCX_SMTP] UCX SMTP list file on the
            system on which you run it.

            A UCX SMTP list file and a VMS Majordomo list file follow the same
            format. Each line is a single SMTP (RFC 822) style address. For
            compatability with UCX SMTP list files VMS Majordomo will honor the
            "!" characters as a comment character in the same way it is honored
            in UCX SMTP lists. That is, the "!" signifies a comment only if it
            is the first non blank character on a line;  everything after the
            "!" becomes part of the comment.

	 -  Does *not* create the initial list config file.
            The first time any part of Majordomo tries to read the list's config
            file it detects that there is no config file and writes a default
            config file for starters.

 	 -  Creates digest if digest was selected:
            -  Creates forwarding entries for digest.
               -  The name of the digest is listname-DIGEST.
 	       -  The owner of the digest is listname-OWNER.
            -  Creates initial empty digest list and password file.
            -  Add digest alias entry to list file of list.
 	    -  For more information on digests see the section "VMS Majordomo
 	    Digests"
 
         -  Mails list owner information to the list owner.
 	    -  A file containing information about the newly created list (the
 	    list name, password, etc.) and general information on the Majordomo
            commands that a list owner can perform is mailed to the list owner.
 
      Once you run CREATE-LIST to make your new list it is recommended that you
      create a .INFO file for the list and then use the "config" command to 
      create the initial list config file and then, optionally, the newconfig
      command to change any configuration options.

   DELETE-LIST
      Run this script when you want to delete an existing VMS Majordomo list.
      This script deletes what CREATE-LIST creates. You must enter the list
      password to run this script.

      List of steps DELETE-LIST takes when deleting a list:
         -  Deletes the list's alias entries with the VMS mail SET
         NOFORWARD/USER command. 
	 -  Deletes all of the list files:
	    -  passwd
	    -  config
	    -  list file itself
	    -  info

 	    Note that at this time when you run the DELETE-LIST script on a
 	    digest it does not delete any pending message files that have been
 	    saved. Nor does it delete the archived digests.
	    
   CHANGE-LIST
      This script allows you to change the values associated with an existing
      list's alias entries. Use it to change the owner of a list or the Reply-To
      RFC header (if defined in the alias for the list rather than in the config
      file). This script leaves the list file, config file and info file alone.
      You must enter the list password to run this script.
 
      -  Creating/deleting a digest for an existing list
         If you have an existing list and want to make a digest for it you can
 	 use CHANGE-LIST.
 
         When CHANGE-LIST creates a digest it does all the same steps as
         CREATE-LIST.
 
         The delete that CHANGE-LIST does on a digest is not complete. All it
         does is remove the "listname-DIGEST" alias from the list's list file
         which is like unsubscribing the digest from the list. This effectively
         disassociates the digest from the list because submissions to the list
         will no longer go to the digest. However the digest's files (list file,
         password file, etc.) are not deleted and the digest's forwarding
         entries are still present in the VMS mail database. In order to fully
         delete the digest you need to run the DELETE-LIST script on the digest.

Miscellaneous other scripts:

   APPROVE
      For moderated lists each message sent to the list must be approved by the
      list owner. For closed lists subscribe and unsubscribe requests must be
      approved. The list owner info file sent to a list's owner(s) includes a
      description of how to do this manually. The APPROVE script is provided to
      help automate the process a bit. The APPROVE script works like the Unix
      original. This is documented in VMS-LIST-OWNER-INFO.TXT.

      Since it is possible that a list owner resides on Unix. The original
      Unix APPROVE script has also been included in the VMS Majordomo kit. It
      has been renamed approve-unix. List owners may request a copy of the
      approve script. If they are on VMS send them "APPROVE.". If they are on
      Unix send them "APPROVE-UNIX".

VMS Majorodomo archives.

Majordomo supports archiving files and allowing remote users to fetch them. This
is done with the index and get commands which are documented in
VMS-USER-INFO.TXT.

This section discusses how to set up an archive. Note that Unix Majordomo comes
with a bunch of other archive scripts which have not been ported yet. The
archive support in VMS Majordomo is only what can be done by the index and get
commands.

   A note from the author:

   With Unix Majordomo you can change the alias for a list to append each
   message sent to the list to an archive file. You can't do this for VMS
   Majordomo. I didn't quite understand how this was a significant value added
   over plain old digests so I didn't take the time to port it.

To VMS Majordomo an archive is a directory (possibly with sub-directories) that
contain files which can be retrieved via email.  Each archive is associated with
a list. To find a list's top level archive directory the same algorithm for
naming the directory is used as with the mkdigest command. (The section "VMS
Majordomo Digests" subsection on Digest directories discusses how the name for a
digest directory is figured out when a digest is generated.) If you take the
default majordomo.cf values for $filedir and $filedir_suffix the archive file a
list's archive directory will be a sub-directory of the UCX$MAJORDOMO_LIST
directory named listname-ARCHIVE.DIR. Eg:
UCX$MAJORDOMO_LISTROOT:[SAMPLE-ARCHIVE].

Since a digest has such a directory it automatically has archive capability. For
regular lists you can manually create such a directory and populate it with
files. (Sorry it won't be populated automatically. Like I said I couldn't see
that much difference between this and a digest and the time I have to spend on
this project is coming to an end.)

Here is an example of the directory for the vms-majordomo-users list:

   $ set def ucx$majordomo_list
   $ set def [.vms-majordomo-users-archive]
   $ dir [...]

   Directory XXX:[UCX$MAJORDOM.LISTS.VMS-MAJORDOMO-USERS-ARCHIVE]

   VMS_MAJORDOMO_FT3_ZIP.UUENCODED;1
                           2339   9-DEC-1996 09:59:34.54

   Total of 1 file, 2339 blocks.

   $ dir ucx$majordomo_list:vms-majordomo-users-archive.dir/secu

   Directory XXX:[UCX$MAJORDOM.LISTS]

   VMS-MAJORDOMO-USERS-ARCHIVE.DIR;1
                              1  13-DEC-1996 14:38:41.96  [UCX$AUX,UCX$MAJORDO
   (RWE,RWE,RE,E)

   Total of 1 file, 1 block.

There are two list configuration parameters that must be set too. They are
"private_index" (default "no") and "private_get" (default "yes"). These
parameters are booleans to say when the index commands should be allowed for the
list. If the option is turned on (ie. value "yes") then only members of the list
may do the command. So, by default anyone may do an index command on your list
but only a list member may do a get.


VMS Majordomo Digests

A VMS Majordomo digest is a special type of list. It has all the same files as a
list - a list file, passwd file, config file and info file. A digest also has
VMS mail forwarding entries as does a "regular" list. However when mail is
delivered to a digest it doesn't go to the list members immediately. Instead it
is stored away in a file as a "pending" message. As each new mail message is
delivered to the digest it too is stored away. When the digest owner issues a
mkdigest command for the digest the saved messages are concatenated into one
large mail message (a digest) and mailed out to the digest members.

Each digest is associated with a list. The name of a digest (used for forwarding
entries and file names, etc.) is the listname followed by "-DIGEST". So, if you
wanted a list named WIDGETS to have a digest associated with it the digest would
be named "WIDGETS-DIGEST". To make a list with a digest use the CREATE-LIST
script. To make a digest for an already existing list use the CHANGE-LIST
script. (For more information on these scripts see the section "Configuring
Majordomo and Majordomo lists".)

++++++++++
Digest subscriptions

Since a digest is a list one needs to subscribe/unsubscribe to it like any other
list. Just because one is subscribed to the list doesn't mean they are
subscribed to the digest since the two lists have different list files. Indeed
it may be possible that some users would want to be subscribed to one without
being subscribed to the other.

If there is demand VMS Majordomo could be enhanced to keep a digest list file
synchronized automatically with the list file of the list that points to it.

++++++++++
Digest directories

VMS Majordomo stores the pending mail messages for each digest in a directory
reserved for that digest. The directory is a subdirectory of the
UCX$MAJORDOMO_DIGEST directory and has the name of the digest. For example the
pending files for WIDGETS-DIGEST would be in the subdirectory named
WIDGETS-DIGEST.DIR under UCX$MAJORDOMO_DIGEST or, alternatively,
UCX$MAJORDOMO_DIGESTROOT:[WIDGETS-DIGEST].

When a digest is generated the pending messages are rolled up into the digest
mail message which is sent to the digest list members. At this time the digest
message is saved in a directory reserved for that digest. The location and name
of the directory is formed from two variables set in the majordomo.cf file
($filedir and $filedir_suffix) and the digest name. The parent directory for
the digest file directory comes from the $filedir variable. Within that
directory  a subdirectory is created for holding messages for each digest. The
name of the subdirectory is formed by concatenating the name of the digest to
the value of the $filedir_suffix variable.

For example, say we have a digest named WIDGETS-DIGEST, with a value of $filedir
equal to "$listdir" and a value of $filedir_suffix equal to "-ARCHIVE". The
digest files are saved in a subdirectory of UCX$MAJORDOMO_LIST which is because
the value of the $listdir variable points to the UCX$MAJORDOMO_LIST directory.
The name of the subdirectory would be WIDGETS-DIGEST-ARCHIVE.DIR. Alternatively
we have a directory name of UCX$MAJORDOMO_LISTROOT:[WIDGETS-DIGEST-ARCHIVE].

The above values for $filedir and $filedir_suffix ("$listdir" and "-ARCHIVE"
respectively) are the default values provided in the template majordomo.cf file.
You probably won't ever need to change them.

++++++++++
Delivery to a digest

When mail is delivered to a list that has a digest it is delivered to the digest
alias just the same as any of the other members of the list. The resend script
does not treat this particular address in the list file in any special way. When
the resend script tells VMS callable mail to deliver the mail message to the
listname-DIGEST address the VMS mail forwarding entry for listname-DIGEST kicks
in and feeds the message to the DIGEST script to be saved away as a pending
message.

A message that is sent to a list that has a digest but is bounced by the resend
script is not delivered to the listname-DIGEST address any more than it is
passed to any of the other lists members. When a message is sent to a moderated
list it must first be approved by the list owner and only then will it be sent
to the list itself and so to the digest script.

Since a digest is a list just like any other list it is also possible (although
probably not desirable in most cases) that users could send a mail directly to
the digest itself, bypassing the list. 

++++++++++
Generating a digest

Majordomo generates a digest in one of two ways - on demand or automatically. To
generate a digest on demand the list owner submits a mkdigest command to
majordomo. A digest is also generated automatically if the addition of a file to
the pending directory causes the total size of the files in the directory to go
over the limit set by the "maxlength" value in the digest's
.CONFIG file. (The "maxlength" value measures bytes, not RMS blocks.)

When a digest is generated the resulting file is saved away in the digest
directory and also passed to UCX SMTP to be delivered to the members of the
digest. The file name used for the saved digest file is of the form:

   V{digest_volume}.N{digest_issue}

Where

   "digest_volume" is the digest volume number from the "digest_volume" value of
   the digest's .CONFIG file.

   "digest_issue" is the digest issue number from the "digest_issue" value of
   the digest's .CONFIG file.

An example filename would be "V01.N001" which is the first issue of the first
volume of the digest.

Each time a digest is made (whether it's made automatically or on demand) the
digest_issue value in the digest's .CONFIG file is incremented by the digest
script so that he next issue has the next higher issue number. The
digest_volume value is not incremented by VMS Majordomo. It must be incremented
by the digest owner with a newconfig command.

The pending files are deleted from the pending file directory when a digest is
generated.


VMS Majordomo Use of Forwarding Entries

This section describes each of the forwarding entries (hereafter referred to as
aliases) used by VMS Majordomo. The aliases are created using the VMS Mail SET
FORWARD/USER command. There are aliases for each list on your system and also
for VMS Majordomo itself. Most of the aliases are hardwired, that is, the values
are set by the CREATE-LIST or CREATE-MAJORDOMO-FWD Perl scripts and are not
system specific values that you can change.

++++++++++
A note about triple double quotes and VMS mail

Newer versions of VMS mail parse double quotes using a new method for allowing
nested double quotes. This means that in order to get the correct aliases into
VMS mail different syntax is required for the SET FORWARD/USER command on the
different versions of VMS mail. For example to set an alias of "xxx" pointing to
SMTP%"jones@acme.com" on systems running the old-style VMS mail parsing code
this syntax is used:

   MAIL> set forward/user=xxx SMTP%"""jones@acme.com"""

   MAIL> sho forward /user=xxx
   XXX has mail forwarded to SMTP%"jones@acme.com"
				  ^		 ^
				  |		 |
   Note the single ----->---->----+--------------+
   quotes in the result
   
On systems with the new VMS mail the triple double quotes are not required:

    MAIL> set forward/user=xxx SMTP%"jones@acme.com"
 
    MAIL> sho forward /user=xxx
    XXX has mail forwarded to SMTP%"jones@acme.com"
 				   ^		  ^
 				   |		  |
    Note the single ----->---->----+--------------+
    quotes in the result
    
Note that with both of the above examples the result of the SHOW FORWARD command
is the same. Although the input syntax to get the alias *into* the VMS mail
database is different the format of the data in the file is the same so the two
can co-exist. Ie. there are no issues around upgrading to the new VMS mail and
having to change all your forwarding entries. Further you can have the different
versions VMS mail running on different cluster members if you want. 

The CREATE-LIST and CREATE-MAJORDOMO-FWD Perl scripts call a routine to detect
which SET FORWARD parsing scheme is in use on the system you're working on. The
scripts will then use the correct syntax for your system.

There are two places where you, the Majordomo administrator, enter a string that
is fed to the SET FORWARD/USER command. In the CREATE-MAJORDOMO-FWD script you
enter the value to be associated with MAJORDOMO-OWNER and in the CREATE-LIST and
CHANGE-LIST scripts you enter the value to be associated with listname-OWNER.
When entering these values you must use the correct syntax for the version of
VMS mail on the system you are working on. That's the bad news. The good news is
that the scripts check what you've entered against what the system you're
working on will expect and if it's not right you are warned and must re-enter.

At this time I can't say authoritatively which versions of VMS will have the new
parsing scheme for the SET FORWARD command. V7.1 SSB uses the old scheme, for
example, but a later patched version of VMS V7.1 uses the new scheme.  As a
matter of fact the way that the scripts detect which parsing syntax is in use is
not by checking an ident of a mail image or something like that. The scripts
simply build a test SET FORWARD/USER command with the old style syntax (ie.
triple double quotes) and look at the result with a SHO FORWARD/USER command. If
the result is correct then you are running on a system that is using the old
syntax. If the result is not correct then you are running on a system using the
new syntax. Hey, it works. ;^)

++++++++++
VMS Majordomo aliases:

VMS Majordomo depends on the definition of these aliases to function. They are
created by the CREATE-MAJORDOMO-FWD script.

Alias:		MAJORDOMO
Hard-wired:	Yes
Forwards to:	PIPE%"UCX$MAJORDOMO_HOME:MAJORDOMO.COM UCX$MAJORDOM"
Comments:	Pipes mail sent to "majordomo" to the MAJORDOMO Perl script.

Alias:		MAJORDOMO-OWNER
Hard-wired:	No
Forwards to:    <value provided by Majordomo administrator>
Comments:	Points to Majordomo administrator(s). When the Majordomo
                administrator runs the CREATE-MAJORDOMO-FWD Perl script the
                value they enter for the name of the Majordomo owner is entered
                here.

Alias:		MAJORDOMO-SMTP
Hard-wired:	Yes
Forwards to:	SMTP%"majordomo@ucx.lkg.dec.com"
Comments:	Provides access to majordomo from non-SMTP mail. See section
                "VMS Majordomo support for non-SMTP mail addresses" for more
                info.

Alias:		OWNER-MAJORDOMO
Hard-wired:	Yes
Forwards to:	MAJORDOMO-OWNER
Comments:	Provided because it's hard to remember whether the correct
                address for the Majordomo owner is OWNER-MAJORDOMO or
                MAJORDOMO-OWNER so we accept both.

Alias:		UCX$MAJORDOM
Hard-wired:	Yes
Forwards to:	OWNER-MAJORDOMO
Comments:	Forwards any mail sent to the VMS Majordomo account to the
                Majordomo administrator

Alias:		MAJORDOM
Hard-wired:	Yes
Forwards to:    OWNER-MAJORDOMO
Comments:	Should probably never be invoked. It is there for Unix to
                forward any mail sent to the Unix majordomo account (which is
                "majordom") to the majordomo administrator. It was left alone in
                the port. If it ain't broke. Don't fix it.
 
 
To see the aliases for majordomo on your system do

    $ SET PROCESS/PRIV=SYSPRV
    $ MAIL
    MAIL> SHO FORWARD/USER=*MAJORDOM*

++++++++++
Per-list aliases:

Aliases are also defined for each VMS Majordomo list. The aliases follow a
pattern where each entry has as part of it's name the listname itself. For the
list below the string "listname" is the name of the list entered when the list
was created with the CREATE-LIST Perl script

Alias:		listname-OWNER
Hard-wired:	No
Forwards to:    <value provided by Majordomo administrator>
Comments:	Points to the list owner(s). When the Majordomo administrator
                runs the CREATE-LIST Perl script the value they enter for the
                list owner is entered here. It is possible to have a list owned
                by more than one person. To do this you need to take advantage
                of a feature of UCX SMTP that allows you to "trick" VMS mail
                into letting you do a SET FORWARD/USER command where the address
                being forwarded to is a list. For more info on how to enter the
                list owner to do this enter a "?" when the CREATE-LIST Perl
                script asks for the list owner.

Aliases:	OWNER-listname, OWNER-listname-OUTGOING, OWNER-listname-REQUEST,
		listname-APPROVAL
Hard-wired:	Yes
Forwards to:	listname-OWNER
Comments:	Each list has all of the above aliases set up for it. They all
                point to the list owner so that mail goes to a real person. In
                some cases users may send mail to one of these addresses.  In
                many cases Majordomo uses these aliases in the Return-Path for
                mail that it sends out. Majordomo also sends mail directly to
                the "listname-APPROVAL" in some cases.

Alias:		listname
Hard-wired:	Yes and no ;^)
Forwards to:	PIPE%"UCX$MAJORDOMO_HOME:RESEND.COM UCX$MAJORDOM [-r reply-to]
                   -l listname listname-OUTGOING"
Comments:	This alias pipes mail sent to the list to the RESEND Perl
		script. The syntax of the parameter list for this alias bears
		more discussion. It is as follows:

		   [-r reply-to] -l listname listname-OUTGOING

		[-r reply-to]
		   If you configure a Reply-To using CREATE-LIST then it is
		   placed here. This is optional. If no Reply-To was configured
		   then there is no -r option. Also note that the Reply-To can
		   be configured in the list's config file.

		-l listname
                   The -l option tells the RESEND script the name of the list
                   that the message was sent to. It is required and is always
		   set to the name of the list itself.

                listname-OUTGOING
		   The last value in the parameter list is the name of the alias
                   that the RESEND script uses to pass a mail message to UCX
                   SMTP to be delivered (ie. "exploded"). In other words when
                   users send mail to the "listname" alias it is processed by
                   the RESEND script and then sent by the RESEND script to
                   "listname-OUTGOING" which causes the UCX SMTP symbiont to
                   send the mail to the members of the list. (In the case of a
                   moderated list an unapproved message is first sent to the
                   list owner to be approved. When the list owner sends it back
                   to the list as approved only *then* does the RESEND script
                   send it to "listname-OUTGOING".)

Alias:		listname-LIST
Hard-wired:	Yes
Forwards to:	SMTP%"listname@ucx.lkg.dec.com"
Comments:	Provides access to the list from non-SMTP mail. See section "VMS
                Majordomo support for non-SMTP mail addresses" for more info.

Alias:		listname-OUTGOING
Hard-wired:	Yes
Forwards to:	SMTP%"¢UCX$MAJORDOMO_LIST:listname.¢@ucx.lkg.dec.com"
Comments:	When the RESEND script decides it's time for a mail to be sent
		to the members of the list it sends the mail to this alias. This
		takes advantage of a UCX SMTP feature that allows you to specify
		a full filespec as the local part of an SMTP address. If this is
		done then UCX SMTP looks for the file and treats it like a text
		file containing a list of SMTP addresses. Ie. a mailing list
		file.

Alias:		listname-REQUEST
Hard-wired:     Yes
Forwards to:    PIPE%"UCX$MAJORDOMO_HOME:REQUEST-ANSWER.COM UCX$MAJORDOM listname"
Comments:	Pipes any mail sent to listname-REQUEST to the REQUEST-ANSWER
		Perl script. Other mailing list software (Listproc and Listserv)
		use the convention of "listname-REQUEST" to send commands to the
		list. This Majordomo script is there to answer users who send
		such messages to the Majordomo list. It sends help back to the
		sender telling them that the list is a Majordomo list and what
		commands they can use on it. It makes no effort to serve up the
		list as a Listproc list.


++++++++++
Special case for digest list name

As discussed in the section on digests a digest is a list itself and so has all
the aliases that a list does. The only alias that differs is the alias for the
digest itself. A digest will always be named "listname-DIGEST". Its alias is as
follows:

Alias:		listname-DIGEST
Hard-wired:	Yes
Forwards to:	PIPE%"UCX$MAJORDOMO_HOME:DIGEST.COM UCX$MAJORDOM -r ¢-C¢
		-l digest-name digest-name-OUTGOING"
Comments:	This alias pipes mail sent to the digest to the DIGEST Perl
		script. The syntax of the parameter list for this alias bears
		more discussion. It is as follows:

		   -r -C -l digest-name digest-name-OUTGOING

		-r
                   The DIGEST script can be invoked to either receive an input
                   mail message and save it as a pending message or to make a
                   digest. The -r option invokes the former function and the -m
                   option invokes the latter. Since the alias for the digest
                   will always be used for the former purpose (ie. for passing a
                   mail message to the DIGEST script to be saved away) the -r
                   option is always present here. When the MAJORDOMO script is
                   serving a mkdigest command it invokes the RESEND script with
                   the -m option.

		-C
		   Tells the DIGEST script to get the configuration information
		   for the digest from the listname-DIGEST.CONFIG file. At this
		   time this is the only tested way of doing this.

		-l digest-name
                   The -l option tells the DIGEST script the name of the digest
                   that the message was sent to. It is required and is always
                   set to the name of the digest itself.

                digest-name-OUTGOING
                   The last value in the parameter list is the name of the alias
                   that the DIGEST script uses to pass a mail message to UCX
                   SMTP to be delivered (ie. "exploded"). It will only use this
                   if it decides it needs to generate a digest. If the DIGEST
                   script generates a digest it has to send it to the digest
                   list. This only happens if the size of all of the pending
                   messages is greater than the digest's config file permits.


To see the aliases for a list named SAMPLE on your system do

   $ SET PROCESS/PRIV=SYSPRV
   $ MAIL
   MAIL> SHO FORWARD/USER=*SAMPLE*

If the list SAMPLE has a digest defined the digest's aliases will also appear.


VMS Majordomo support for non-SMTP mail addresses

One of the goals of the port of Majordomo to VMS is to provide compatability
with non-SMTP mail environments which, in a VMS world, almost always means plain
old intra-cluster VMS mail and DECnet mail and often means All-In-1. The work in
this area is most likely not complete however some work has been done.

Additional aliases provided to allow use of VMS Majordomo from DECnet:

   For each new list you make the CREATE-LIST script creates an alias (Ie. does
   a SET FORWARD/USER command) in the form of "listname-LIST" where "listname"
   is the name of the list. The forward is set to point to the list itself but
   does so via the SMTP protocol. For example, for a list named "sample" you
   have:

      MAIL> sho forward /user=sample-list
      SAMPLE-LIST has mail forwarded to SMTP%"sample@acme.com"

   The CREATE-MAJORDOMO-FWD script creates a similar entry for majordomo itself,
   MAJORDOMO-SMTP:

      MAIL> show forw/user=majordomo-smtp
      MAJORDOMO-SMTP has mail forwarded to SMTP%"majordomo@acme.com"
   

   Providing such aliases allows those who have only DECnet connectivity to the
   VMS Majordomo server to send mail to lists, to issue all the VMS Majordomo
   list commands and even to act as a list owner. To send to a list the user
   sends to NODNAM::listname-LIST, eg.

      MAIL> send
      To: ACME::SAMPLE-LIST

   Likewise to send to majordomo itself (as with a subscribe command) one uses
   MAJORDOMO-SMTP. EG:

      MAIL> send
      To: ACME::MAJORDOMO-SMTP
      Subj: bla blah blah
      subscribe sample
      <CTRL-Z>

   In cases of DECnet subscriptions the "SMTPfied" version of the user's From
   address is stored in the file.

   The most important effect of the "listname-LIST" and MAJORDOMO-SMTP aliases
   is that funneling the mail through SMTP causes creation of RFC 822 headers
   which Majordomo wants to see.

Field test Caveats

   At this point we have tested subscribing a DECnet address, sending and
   receiving mail and unsubscribing. Setting a DECnet user as a list owner has
   also been tested to a small extent. None of this has been tested with
   All-In-1 although we believe the mechanisms we have put in place should allow
   for it. We would be very interested in hearing how anyone who tries this with
   MTS/All-In-1 makes out. (Send mail to majordomo-owner@ucx.lkg.dec.com.)


Miscellaneous:

   -  To send to a list or to majordomo when you're logged onto the system
   running VMS Majordomo you might think you could do this:

      MAIL> send	-- OR --    MAIL> send
      To: MAJORDOMO	--THIS--    To: SAMPLE

   but actually you shouldn't. This is because mail to VMS Majordomo needs to
   have RFC 822 headers and so must come from the SMTP world. If you send
   directly to majordomo or directly to the list your message will be piped into
   a Majordomo script without RFC 822 headers. So, the correct way to send to
   majordomo and a list when logged onto the system running VMS Majordomo is
   this:

      MAIL> send	  --AND --    MAIL> send
      To: MAJORDOMO-SMTP  --THIS--    To: SAMPLE-LIST

   -  Interaction between UCX SMTP symbiont, PIPE% code and Majordomo scripts - 
   The PIPE% code waits until the batch job that it creates exits before
   returning. This means that the process sending mail to the PIPE% mechanism
   will block until the batch job exits. With VMS Majordomo all mail to the
   PIPE% mechanism is delivered in the UCX SMTP symbiont process. This means
   that the UCX SMTP symbiont process is blocked when delivering mail to a
   Majordomo PIPEed alias until the corresponding script completes.
   Every VMS Majordomo script completes once it has read through all its input and
   submitted any mail message or messsages to the UCX SMTP queue. Note that the
   scripts don't wait for the mail they submit to the queue to be sent. They
   simply wait for the mail job to be queued.

 
VMS Majordomo diagnostics:

In addition to the diagnostics that you can turn on by setting the appropriate
Perl script's debug variable you can also define logical to have the majordomo
scripts dump diagnostics to the log file.  You guessed it.

   DEFINE/SYS UCX$MAJORDOMO_DEBUG 1

Once caveat is that you also need to turn on PIPE% diagnostics. The reason is
that when a VMS Majordomo Perl script is run (not counting the configuration
Perl scripts written expressly for VMS Majrodomo) it is run under the PIPE%
mechanism which is currently a batch job. At present the only way to get the
PIPE% mechanism to /KEEP the log file is to turn on PIPE% diagnostics.
[-.PIPE]README.TXT says how to do that.
