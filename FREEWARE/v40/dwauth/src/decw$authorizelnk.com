$ !
$ ! Link a new copy of DECW$AUTHORIZE.EXE.
$ !
$ link/exe=decw$authorize/map=decw$authorize/full/cross/notrace -
	uaf_main,-
	uaf_global,-
	uaf_dw_init,-
	uaf_dw_main,-
	uaf_dw_identifiers,-
	uaf_dw_entry,-
	uaf_dw_access,-
	uaf_dw_subs,-
	uaf_dw_template,-
	uaf_file_subs,-
	uaf_misc,-
	uaf_xxxuai,-
	uaf_messages,-
	uaf_tables,-
	sys$library:vaxcrtl/library,-
	sys$input:/options
psect = $plit$, wrt
sys$library:decw$xtshr/share
sys$library:decw$dxmlibshr/share
sys$library:decw$xlibshr/share
sys$library:decw$xmlibshr/share
sys$library:decw$transport_common/share
sys$library:librtl/share
$ !
$ exit
