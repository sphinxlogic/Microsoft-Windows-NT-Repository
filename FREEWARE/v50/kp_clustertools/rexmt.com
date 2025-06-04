$! REXMT.COM
$!
$!  Examine PEDRIVER's retransmitted-packet counters under SDA, to measure
$!  the severity of LAN problems in an LAVc environment.
$!
$!  ReXmt counter shows re-transmitted pkts (pkt lost or its Ack lost)
$!  ReRcv counter shows duplicate pkts received (Ack lost on its way to sender)
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$!   http://encompasserve.org/~parris/ or http://www.geocities.com/keithparris/ 
$	old_verify = f$verify(0)
$	close/nolog com
$	open/write com rexmt_temp.com_0
$	write com "$	define/user sys$output nl:"
$	write com "$	define/user sys$error nl:"
$!!!	write com "$	define/user sys$command sys$input"
$	write com "$	analyze/system"
$	write com "show port"			!Set up SDA symbols (ignore output from this)
$	write com "set output rexmt_temp.list"	!Put output into a file
$	write com "show port/addr=pe_pdt"		!List virtual circuits going through PEDRIVER
$!!!	write com "exit"
$	write com "$ exit"
$	close com
$	@rexmt_temp.com_0
$	delete rexmt_temp.com_0;*
$	open/write com rexmt_temp.com
$! Grab nodenames and page headings
$	search rexmt_temp.list/out=rexmt_temp.list2 "/","-",":"/match=and
$! Drop page headings
$	search rexmt_temp.list2/out=rexmt_temp.nodes "System Dump Analysis"/match=nand	!Drop headings
$	close/nolog temp	!In case we got aborted before...
$!!!	close/nolog com
$	open/read temp rexmt_temp.nodes
$	open/write com rexmt_temp.com
$	write com "$ analyze/system"
$	write com "show port	!Set up SDA symbols"
$!	write com "show port/addr=pe_pdt"
$	write com "set output rexmt_temp.vc"
$ LOOP:
$	read/end=EOF temp rec
$	nodename = f$element(1," ",f$edit(rec,"COMPRESS"))
$	write com "show port/vc=vc_",nodename
$	goto LOOP
$ EOF:
$	close com
$	@rexmt_temp.com/out=nl:
$	search rexmt_temp.vc "Remote System Name",ReXmt,ReRcv
$	delete rexmt_temp.*;*
$	exit 1+0*f$verify(old_verify)
