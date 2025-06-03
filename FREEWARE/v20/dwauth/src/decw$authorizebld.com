$ set noon
$ !
$ ! This command procedure will build a new version of the DECwindows
$ ! Authorize utility.
$ !
$ ! Start off by determining whether or not the software is being built on an
$ ! OpenVMS/AXP system.
$ !
$ alpha="false"						! Assume not building on Alpha
$ if f$getsyi ("cpu") .eq. 128 then alpha="true"	! If this is an Alpha system, note it
$ !
$ ! Compile all the modules needed by the DECwindows Authorize utility.
$ !
$ ! Build the message files.
$ !
$ message/list=uaf_messages		/object=uaf_messages		uaf_messages
$ !
$ ! Assemble the MACRO modules.
$ !
$ library/create/macro			uafpvtlib.mlb			uafpvtdef.mar
$ !
$ macro/list=uaf_tables			/object=uaf_tables		uaf_tables+sys$library:lib/lib+sys$disk:[]uafpvtlib/lib
$ !
$ ! Compile the VAX C modules.
$ !
$ cc_qual=""
$ if alpha then cc_qual="/standard=vaxc"
$ !
$ cc'cc_qual'/list=uaf_dw_access	/object=uaf_dw_access		uaf_dw_access
$ cc'cc_qual'/list=uaf_dw_entry		/object=uaf_dw_entry		uaf_dw_entry
$ cc'cc_qual'/list=uaf_dw_identifiers	/object=uaf_dw_identifiers	uaf_dw_identifiers
$ cc'cc_qual'/list=uaf_dw_init		/object=uaf_dw_init		uaf_dw_init
$ cc'cc_qual'/list=uaf_dw_main		/object=uaf_dw_main		uaf_dw_main
$ cc'cc_qual'/list=uaf_dw_subs		/object=uaf_dw_subs		uaf_dw_subs
$ cc'cc_qual'/list=uaf_dw_template	/object=uaf_dw_template		uaf_dw_template
$ cc'cc_qual'/list=uaf_file_subs	/object=uaf_file_subs		uaf_file_subs
$ cc'cc_qual'/list=uaf_global		/object=uaf_global		uaf_global
$ cc'cc_qual'/list=uaf_main		/object=uaf_main		uaf_main
$ cc'cc_qual'/list=uaf_misc		/object=uaf_misc		uaf_misc
$ cc'cc_qual'/list=uaf_xxxuai		/object=uaf_xxxuai		uaf_xxxuai
$ !
$ ! Compile the UIL modules.
$ !
$ uil/motif/list=decw$authorize		/output=decw$authorize		uaf
