$! ns_servers.com
$!
$! V0.2.1   2-May-1994
$!
$! Expected inputs:
$!
$! File: ns_servers.raw  If you are running PH, this file can be obtained 
$!                       from ns.nwu.edu by:
$!                           $ define/user sys$output ns_servers.raw
$!                           $ ph -m -s ns.nwu.edu ns-servers return all
$!                       The -m is supposed to turn off the "pager" in ph, 
$!                       but this option seems to have no effect in my Ph client.
$!                       In reality what I do is cheat. I'm using MultiNet and I rsh
$!                       to a Unix machine and use that machine's Ph:
$!
$!    $ rsh/user=me/out=ns_servers.raw unixhost ph -m -s ns.nwu.edu ns-servers return all
$!
$!                       Or you can get the file via anonymous FTP to geology.ucdavis.edu
$!
$! File: CSO_CONFIG      This is probably a logical name that points to your
$!                       CSO x.cnf file. I want to be able to scan this file to
$!                       determine which field numbers you are using for the
$!                       field names: alias, name, type, and text, since there
$!                       is no guarantee that you decided to use the same
$!                       numbers I did.
$!
$! File: ns_servers_local.flat  This file is a locally generated file that
$!                       contains the names of servers and sites that you
$!                       want to use or advertise that may not be in the
$!                       ns.nwu.edu listing. The contents of this file will
$!                       merged with the information in ns_servers.raw. The
$!                       file can be null.
$!
$!                       The format should be:
$!                       <server>|<site>|[<domain>]
$!
$!                       An example is:
$!
$!                          University of California at Davis|geology.ucdavis.edu
$!
$! File: ns_servers_here.flat  This file is a locally generated file that
$!                       contains the name of your server and site. It will be
$!                       prepended and appended to the sorted ns_servers.flat
$!                       file just prior to generating the QI_ useable files.
$!                       This is useful if you want to force your site to also
$!                       appear at the top and bottom of the site list. The
$!                       Macintosh Ph client uses this site list to build a menu
$!                       of known sites. I find it useful to be able to just
$!                       scroll to the top or bottom of the list to make my home
$!                       site the default. The file can be null.
$!
$!                       The format should be:
$!                       <server>|<site>|[<domain>]
$!
$!                       An example is:
$!
$!                          U.C. Davis|geology.ucdavis.edu
$!
$!                       An example with multiple entries is:
$!
$!                          University of California Systemwide Administration|directory.ucop.edu|
$!                          University of California at Berkeley|infocal.berkeley.edu|
$!                          University of California at Davis|geology.ucdavis.edu
$!                          University of California at Irvine|qi.service.uci.edu|
$!                          University of California at Los Angeles|directory.ucla.edu|
$!                          University of California at San Diego|ucsd.edu|
$!                          University of California at Santa Barbara|ucsbuxa.ucsb.edu|
$!
$ say :== write sys$output
$!
$ show_debug = "show "
$ show_debug = "! show"
$!
$ say " "
$ say "This procedure expects to find a number of files."
$ say " "
$!
$ targets = "ns_servers.raw/cso_config/ns_servers_local.flat/ns_servers_here.flat"
$ fault = 0
$ n = 0
$!
$ loop0:
$    fault_'n' = 0
$    target = f$element(n, "/", targets)
$    if target .eqs. "/" then goto targets_done
$    found = f$search(target)
$    if found .nes. ""
$    then
$       say "     Found: ''target'"
$    else
$       say "  *  Unable to find: ''target'"
$       fault = 1
$       fault_'n' = 1
$    endif
$    n = n + 1
$ goto loop0
$!
$ targets_done:
$!
$ say " "
$!
$ if fault .eq. 1 
$ then 
$    if (fault_0 .eq. 1) .or. (fault_1 .eq. 1)
$    then
$       say "  Please try to correct the problem displayed above"
$       say "  and then try this procedure again."
$       say " "
$       say "You may find the comments in the command file useful."
$       exit
$    else
$       say "  We can live with that."
$       say " "
$    endif
$ endif
$!
$ index = 1
$!
$ say "   Defining field codes from CSO_CONFIG..."
$!
$! get the local configuration codes for alias, name, type, and text
$!
$ cnf_field_names = "alias/name/type/text"
$ n = 0
$ loop1:
$    target = f$element(n,"/",cnf_field_names)
$    if target .eqs. "/" then goto section_2
$    gosub get_field_codes
$    n = n + 1
$ goto loop1
$!
$ section_2:
$!
$ say "   Flattening the nameserver download..."
$!
$! The server list that I download from Northwestern has Northwestern's site at
$! the top. I'd like to have our site at the top and Northwestern's at the
$! appropriate place alphabetically in the list. And I might accumulate a list
$! of additional servers that I'd like to add. So here's my strategy: flatten
$! out the site list from nwu, one record per site, append any local
$! additions, SORT this file, and then transform it into a QI_ format.
$!
$ open/read  ns_infile   ns_servers.raw
$ copy nl: ns_servers.flat
$ open/append ns_outfile  ns_servers.flat
$!
$ read_site:
$ read/end=section_3 ns_infile record
$ site = ""
$!
$ test_site:
$ tag = f$element(1, ":", record)
$ tag = f$edit(tag, "COLLAPSE")
$ 'show_debug' symbol tag
$ if tag .nes. "site" then goto read_site
$ site = f$element(2, ":", record)
$ site = f$edit(site, "COMPRESS,TRIM")
$ 'show_debug' symbol site
$!
$ read_server:
$ read/end=section_3 ns_infile record
$ server = ""
$!
$ test_server:
$ tag = f$element(1, ":", record)
$ tag = f$edit(tag, "COLLAPSE")
$ 'show_debug' symbol tag
$ if tag .nes. "server" then goto read_server
$ server = f$element(2, ":", record)
$ server = f$edit(server, "COMPRESS,TRIM")
$ 'show_debug' symbol server
$!
$ read_domain:
$ read/end=section_3 ns_infile record
$ domain = ""
$!
$ test_domain:
$ tag = f$element(1, ":", record)
$ tag = f$edit(tag, "COLLAPSE")
$ 'show_debug' symbol tag
$!$ if tag .nes. "domain" then goto read_domain
$ if tag .eqs. "domain" 
$ then
$    domain = f$element(2, ":", record)
$    domain = f$edit(domain, "COMPRESS,TRIM")
$ endif
$ 'show_debug' symbol domain
$!
$ write ns_outfile site + "|" + server + "|" + domain
$!
$! If tag .ne. "domain" then the record probably is the next site record,
$! so try that first.
$!
$ if tag .nes. "domain" then goto test_site
$!
$ goto read_site
$!
$!
$! ====================================================================
$!
$ section_3:
$!
$ close ns_infile
$ close ns_outfile
$!
$ if f$search("ns_servers_local.flat") .eqs. "" then copy nl: ns_servers_local.flat
$ if f$search("ns_servers_here.flat")  .eqs. "" then copy nl: ns_servers_here.flat
$!
$ append ns_servers_local.flat ns_servers.flat
$ sort ns_servers.flat ns_servers.sort
$!
$ copy/log ns_servers_here.flat,ns_servers.sort,ns_servers_here.flat ns_servers.flat
$!
$ delete/log ns_servers.sort;*
$ purge/log  ns_servers.flat
$!
$ part2:
$!
$ say "   Generating a QI_ usable file..."
$!
$ open/read  ns_infile  ns_servers.flat
$ open/write ns_outfile ns_servers.dat
$!
$ continuation_max = 99
$ qir_offset = 9999
$ qir_index   = 1
$!
$ Preamble:
$!
$!-----------------------------------
$!
$ qir_count   = 0
$ qir_code    = alias_code
$ qir_string  = "ns-servers-''qir_index'"
$!
$ gosub make_qi_record 
$ write ns_outfile qi_record
$!-----------------------------------
$!
$ qir_code    = name_code
$ qir_string  = "ns-servers"
$!
$ gosub make_qi_record 
$ write ns_outfile qi_record
$!-----------------------------------
$!
$ qir_code    = type_code
$ qir_string  = "serverlist"
$!
$ gosub make_qi_record
$ write ns_outfile qi_record
$!-----------------------------------
$!
$ qir_code    = text_code
$!
$ read_flat_loop:
$!
$ read/end=EOF_flat ns_infile record
$ site_part   = f$element(0, "|", record)
$ server_part = f$element(1, "|", record)
$ domain_part = f$element(2, "|", record)
$!
$ 'show_debug' symbol site_part
$ 'show_debug' symbol server_part
$ 'show_debug' symbol domain_part
$!
$!
$ qir_string = "site:" + site_part
$ gosub make_qi_record 
$ write ns_outfile qi_record
$ qir_count = qir_count + 1
$!
$!-----------------------------------
$ qir_string = "server:" + server_part
$ gosub make_qi_record 
$ write ns_outfile qi_record
$ qir_count = qir_count + 1
$! say "   ''server_part'"
$!-----------------------------------
$!
$! take care of both cases of no-domain: 
$!   first,  no trailing separator ("|") and no domain
$ if domain_part .eqs. "" then goto domain_done
$!   second, trailing separator ("|") and no domain 
$ if domain_part .eqs. "|" then goto domain_done
$!   otherwise, assume domain info is here.
$!
$ qir_string = "domain:" + domain_part
$ gosub make_qi_record 
$ write ns_outfile qi_record
$ qir_count = qir_count + 1
$!
$ domain_done:
$!
$!-----------------------------------
$!
$ if qir_count .lt. (continuation_max - 3) then goto read_flat_loop
$! otherwise
$!
$ qir_index = qir_index + 1
$ goto preamble
$!
$ EOF_flat:
$!
$ close ns_infile
$ close ns_outfile
$!
$ delete  ns_servers.flat;
$!
$ say "   Done."
$ say " "
$ say "   NS_SERVERS.DAT is now ready to be merged with your QI_ database.
$ say " "
$!
$ EXIT
$!
$! =================================================================
$!
$ get_field_codes:
$!
$    search cso_config :'target':/output=sys$scratch:search_temp.tmp
$    open/read tmp sys$scratch:search_temp.tmp
$    read tmp record
$    close tmp
$    delete sys$scratch:search_temp.tmp;
$    code = f$element(0, ":", record)
$    'target'_code = f$integer(code)
$    'show_debug' symbol 'target'_code
$ return
$!
$! =================================================================
$!
$ Make_qi_record:
$!
$ 'show_debug' symbol qir_offset
$ 'show_debug' symbol qir_index
$ 'show_debug' symbol qir_code
$ 'show_debug' symbol qir_count
$ 'show_debug' symbol qir_string
$!
$ qi_record = f$fao("!4SL!5ZL!2ZL!2ZL0!AS", qir_offset, qir_index, qir_code, qir_count, qir_string) 
$!
$ 'show_debug' symbol qi_record
$ return
$! ====================================================================
$!
$! R e l e a s e   N o t e s
$!
$! V0.2.1   2-May-1994
$!
$!   o  Corrected the spelling of sys$output in the text that explains the
$!      ns_servers.raw file.  Expanded the explanation of same. Minor spelling
$!      corrections.
$!
$! V0.2  25-Apr-1994
$!
$!   o  In V0.2 the name of the main input file has been changed to
$!      ns_servers.raw from ns_servers.log
$!
$! V0.2 fixes the following problems:
$!
$!   o  V0.1 generated new, null NS_SERVERS_HERE.FLAT and NS_SERVERS_LOCAL.FLAT
$!      files even if those files were already present.
$!
$!   o  Files generated by V0.1 had the server and site field values reversed.
$!
$!   o  V0.2 does a better job of cleaning up after itself.
$!
$!   o  V0.1 could be fooled by a missing "|" domain delimiter, and thereby
$!      generated incorrect output files.
$!
