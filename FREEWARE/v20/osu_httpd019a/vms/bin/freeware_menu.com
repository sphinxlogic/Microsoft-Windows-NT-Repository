$!
$! script to convert freeware CD data base to HTML.
$! We assume freeware CD is mounted shared and logical freeware$loc
$! points to the directory containing menu_info.dat.
$!
$ write net_link "<DNETARG2>"
$ read net_link search_arg
$ if search_arg .nes. "" then goto show_product
$!
$! Get path info, issue redirect if missing (as opposed to null) argument.
$!
$ write net_link "<DNETPATH>"
$ read net_link path
$ url = p2 - path
$ if info .eqs. "" then
$ info_start = f$locate("/",url)
$ if info_start .ge. f$length(url)
$ then
$    write net_link "<DNETID>"
$    read net_link connect_info
$    hostname = f$element(1," ",connect_info)
$    port = f$element(2," ",connect_info)
$    if port .nes. "80" then hostname = hostname + ":" + port
$    write net_link "<DNETCGI>"
$    write net_link "Location: http://", hostname, p2, "/", f$fao("!/!/")
$    write net_link "</DNETCGI>
$    exit
$ endif
$ info = f$extract(info_start+1,255,url)
$!
$! Open menu database.
$!
$ open/read menu_db freeware$loc:menu_info.dat/ERROR=open_err
$ open error then goto cleanup
$ on control_y then goto cleanup
$!
$! scan the categories.
$!
$ write net_link "<DNETRECMODE>"
$ write net_link "<DNETCGI>"
$ write net_link "Content-type: text/html", f$fao("!/")
$ cleanup_string = "</DNETCGI>"
$ if info .nes. "" then goto sub_menu
$ cur_cat = "                    "
$ write net_link "<HTML><HEAD><TITLE>Freeware CD Menu</TITLE></HEAD><BODY>
$! write net_link "P2 = ", P2, "<BR>
$! write net_link "info = ", info, "<BR>"
$ write net_link "<H2>Freeware CD</H2>Categories<OL>"
$ next_cat:
$    read menu_db/key="''cur_cat'"/index=1/match="GT" line/error=cat_done
$    cur_cat = f$extract(15,20,line)
$    dsp_cat = f$edit(cur_cat,"TRIM")
$    a_cat = dsp_cat
$    esc_cat:
$	off = f$locate(" ",a_cat)
$	if off .eqs. f$length(a_cat) then goto esc_cat_done
$	a_cat = f$extract(0,off,a_cat) + "%20" + f$extract(off+1,20,a_cat)
$	goto esc_cat
$    esc_cat_done:
$    write net_link "<LI><A HREF=""",a_cat,""">",dsp_cat,"</A></LI>"
$    goto next_cat
$!
$ cat_done:
$ write net_link "</OL></BODY>"
$ goto cleanup
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$ sub_menu:
$ info_key = f$extract(0,20,info+"                    ")
$ write net_link "<HTML><HEAD><TITLE>Freeware CD - ",info,"</TITLE></HEAD><BODY>
$ key_qual = "/index=1/key=""" + info_key + """/match=eq"
$ write net_link "<H2>", info,"</H2><DL>"
$ count = 0
$ next_sub:
$    read menu_db 'key_qual' line/error=sub_done
$    key_qual = ""	! do sequential reads
$    cur_cat = f$extract(15,20,line)
$    if cur_cat .nes. info_key then goto sub_done
$!
$    prd = f$edit(f$extract(0,15,line),"TRIM")
$    count = count + 1
$    prd_loc = f$edit(f$extract(35,98,line),"TRIM")
$    prd_desc = f$edit(f$extract(137,60,line),"TRIM")
$    prd_demo_txt = f$extract(135,1,line)
$    prd_demo_com = f$extract(136,1,line)
$    write net_link "<DT><A HREF=""?", prd_loc, """>",prd,"</A></DT>"
$    write net_link "<DD>", prd_desc,"</DD>"
$    got next_sub
$ sub_done:
$ write net_link "</DL><HR>Item count: ", count, "<BR></BODY>"
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$ cleanup:
$ if f$type(cleanup_string) .nes. "" then write net_link cleanup_string
$ close menu_db
$ exit
$!
$ open_err:
$ write net_link "<DNETTEXT>"
$ write net_link "500 open error"
$ if f$trnlnm("freeware$loc") .eqs. "" then write net_link -
  "Freeware$loc not defined."
$ copy sys$Input net_link:
Could not open the menu database.
$ write net_link "</DNETTEXT>"
$ exit
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! display product.
$ show_product:
$ search_arg = f$extract(1,255,search_arg)	! trim leading "?"
$ decoded = ""
$ decode_escape:
$    percent = f$locate("%",search_arg)
$    decoded = decoded + f$extract(0,percent,search_arg)
$    if percent .ge. f$length(search_arg) then goto decode_done
$    decoded[f$length(decoded)*8,8] = %x0'f$extract(percent+1,2,search_arg)
$    search_arg = f$extract(percent+3,255,search_arg)
$    goto decode_escape
$ decode_done:
$ readme = f$search(f$parse(decoded,"freeware$loc:freeware_read*.txt"))
$ write net_link "<DNETTEXT>"
$ write net_link "200 OK"
$ if readme .nes. "" then copy 'readme' net_link
$ if ( readme .eqs. "" write net_link "search arg: ", decoded
$ write net_link "</DNETTEXT>"
$ readme = f$search("")
