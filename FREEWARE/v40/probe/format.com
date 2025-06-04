$! FORMAT.COM
$!
$!This software is COPYRIGHT © 1989-1997, Stephane Germain. ALL RIGHTS RESERVED.
$!Permission is granted for not-for-profit redistribution, provided all source
$!and object code remain unchanged from the original distribution, and that all
$!copyright notices remain intact.
$!
$!This software is provided "AS IS". The author makes no representations or
$!warranties with respect to the software and specifically disclaim any implied
$!warranties of merchantability or fitness for any particular purpose.
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$! This command procedure formats a PROBE recording. It is meant to paliate the
$! current absence of a native Alpha-based playback module. It only understands
$! recording made with PROBE version 2.2* and 2.3.
$!
$! Invocation:
$!
$!	@sys_probe:format  probe-recording-file-spec  [output-file-spec]
$!
$! By default, the input recording file type is .PRB and output is delivered
$! to SYS$OUTPUT. The output resembles but is not identical to what is produced
$! by PROBE/PLAYBACK/ANALYZE. If frame data (payload) display is wanted, define
$! the logical name PROBE$FORMAT_DATA to any value.
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$ set noverify
$ on error then goto HANDLER
$ on control_y then goto DONE
$!
$ byte = 8
$ word = 16
$ long = 32
$!
$ if (P1.eqs."") then goto BAD_PARAM
$ file = f$parse(P1,,,"DEVICE")+f$parse(P1,,,"DIRECTORY")+f$parse(P1,,,"NAME")+f$parse(P1,".PRB",,"TYPE")+f$parse(P1,,,"VERSION")
$ infile = f$search(file)
$ if (infile.eqs."") then goto NO_INPUT
$ open/read/error=BAD_INPUT recording 'infile'
$ if (P2.eqs."")
$ then
$    put := write sys$output
$ else
$    result = f$parse(P2,,,"DEVICE")+f$parse(P2,,,"DIRECTORY")+f$parse(P2,,,"NAME")+f$parse(P2,".PRB_FORMAT",,"TYPE")
$    open/write/error=BAD_OUTPUT analysis 'result'
$    put := write/error=BAD_OUTPUT analysis
$ endif
$!
$ read/end=DONE/error=BAD_INPUT recording line
$ if (f$cvui(0,byte,line).ne.%x01) then goto NO_PRB
$ if (f$cvui(6*byte,word,line).ne.%x0202).and.(f$cvui(6*byte,word,line).ne.%x0203) then goto BAD_VERSION
$ if (f$cvui(3*byte+4,1,line).eq.0)	! 2.2B: 1=alternate map
$ then
$    if (f$cvui(3*byte+6,1,line).eq.0) then goto BAD_DATATYPE ! no VMS F floating or Quad time
$    if (f$cvui(3*byte+7,1,line).eq.0)	! 2.2B: 0=VAX 1=AXP
$    then platform = "VAX"
$    else platform = "Alpha"
$    endif
$    if (f$cvui(3*byte+5,1,line).eq.0)	! 2.2B: 1=extracted
$    then platform = platform + ": original recording"
$    else platform = platform + ": extracted sample"
$    endif
$ else
$    platform = "platform unknown"
$ endif
$ payload  = f$cvui(2*byte,12,line)
$ nodesize = f$cvui(31*byte,byte,line)
$ devsize  = f$cvui(30*byte,byte,line)
$ node = f$extract(40,nodesize,line)
$ dev  = f$extract(32,devsize,line)
$!
$ if ((f$trnlnm("PROBE$FORMAT_DATA").nes."").and.(payload.ne.0))
$ then
$    format_data = 1
$    notice = ""
$ else
$    format_data = 0
$    notice = "(will not be displayed)"
$ endif
$!
$ put "PROBE 2.3 RECORDING-FILE FORMATTER"
$ put ""
$ put f$fao("Input file: !AS",infile)
$ put f$fao("Made on:    !AS::!AS (!AS)",node,dev,platform)
$ put f$fao("Cycle:      !UL (Standby: !UL second!%S, Activity: !UL second!%S)",-
            f$cvui(28*byte,word,line),f$cvui(26*byte,word,line),f$cvui(24*byte,word,line))
$ put f$fao("Payload:    up to !UL byte!%S recorded per frame !AS",payload,notice)
$ put ""
$!
$ frames = 0
$LOOP:
$ read/end=DONE/error=BAD_INPUT recording line
$ entry = f$cvui(0,byte,line)
$ if (entry.gt.7) then goto BAD_TYPE
$ goto type_'entry'
$!
$TYPE_0:
$ frames = frames + 1
$ cycle_frames = cycle_frames + 1
$ frametype = f$cvui(1*byte,1,line)	! 0=ethernet 1=IEEE802.3
$ framedata = f$cvui(1*byte+1,1,line)
$ moredata  = f$cvui(1*byte+2,1,line)
$ iowarning = f$cvui(1*byte+3,1,line)
$ overhead  = f$cvui(1*byte+4,4,line)
$ datasize  = f$cvui(2*byte,word,line)
$ framesize = datasize + overhead
$ totalsize = framesize + 18
$ if (totalsize.lt.64) then totalsize = 64
$ frametime[0,32] = f$cvui(4*byte,long,line)
$ mantissa       = "" ! force symbol to be a (null) string before overlays
$ mantissa[0,16] = f$cvui(16,16,frametime)
$ mantissa[16,7] = f$cvui(0,7,frametime)
$ mantissa[23,9] = %x001
$ exponent = f$cvui(7,8,frametime) - 128
$ if (exponent.le.0)
$ then
$    timeint = 0
$    if (exponent.le.-7)
$    then timefrac = 0
$    else timefrac = (f$cvui(24-exponent-8,8,mantissa)*100)/256
$    endif
$ else
$    timeint = f$cvui(24-exponent,exponent,mantissa)
$    if (exponent.gt.16)
$    then timefrac = 0
$    else timefrac = (f$cvui(24-exponent-8,8,mantissa)*100)/256
$    endif
$ endif
$ srcvendor  = f$fao("!2XB-!2XB-!2XB",f$cvui(8*byte,byte,line),f$cvui(9*byte,byte,line),f$cvui(10*byte,byte,line))
$ srcmachine = f$fao("!2XB-!2XB-!2XB",f$cvui(11*byte,byte,line),f$cvui(12*byte,byte,line),f$cvui(13*byte,byte,line))
$ dstvendor  = f$fao("!2XB-!2XB-!2XB",f$cvui(16*byte,byte,line),f$cvui(17*byte,byte,line),f$cvui(18*byte,byte,line))
$ dstmachine = f$fao("!2XB-!2XB-!2XB",f$cvui(19*byte,byte,line),f$cvui(20*byte,byte,line),f$cvui(21*byte,byte,line))
$ protocol   = f$fao("!2XB-!2XB",f$cvui(24*byte,byte,line),f$cvui(25*byte,byte,line))	! assume ethernet frame
$ origin  = srcvendor + "-" + srcmachine
$ address = origin - "-" - "-" - "-" - "-" - "-"
$ if (f$type(n'address').nes."")
$ then origin = n'address'
$ else
$    sqzsrc = srcvendor - "-" - "-"
$    if (f$type(v'sqzsrc').nes."") then origin = f$fao("!AS:!AS",v'sqzsrc',srcmachine)
$ endif
$ target  = dstvendor + "-" + dstmachine
$ address = target - "-" - "-" - "-" - "-" - "-"
$ if (f$type(n'address').nes."")
$ then target = n'address'
$ else
$    sqzdst = dstvendor - "-" - "-"
$    if (f$type(v'sqzdst').nes."") then target = f$fao("!AS:!AS",v'sqzdst',dstmachine)
$ endif
$ if (frametype.ne.0)
$ then	! IEEE frame
$    tag   = "D-S SAPs"
$    style = ""
$    if (protocol.eqs."AA-AA")
$    then
$	tag      = "Protocol"
$       protocol = f$fao("!2XB-!2XB",f$cvui(30*byte,byte,line),f$cvui(31*byte,byte,line))
$	style    = "(SNAP)"
$    endif
$    if (protocol.eqs."E0-E0")
$    then
$	style    = "(IPX802.2)"
$    endif
$    if (protocol.eqs."FF-FF")
$    then
$       protocol = "none "
$	style    = "(IPX802.3)"
$	datasize = framesize	! overhead is considered data
$    endif
$ endif
$ sqzptcl = protocol - "-"
$ if (f$type(p'sqzptcl').nes."") then protocol = p'sqzptcl'
$ if (iowarning)
$ then sticker = "?"
$ else sticker = " "
$ endif
$ if (frametype.eq.0)
$ then put f$fao("!ASFrame !10ZL (Ethernet) @ !5ZL.!2ZL  From: !17AS  To: !17AS  Size: !4UW/!4<!UW!>  Protocol: !AS",-
		  sticker,frames,timeint,timefrac,origin,target,datasize,totalsize,protocol)
$ else put f$fao("!ASFrame !10ZL (IEEE)     @ !5ZL.!2ZL  From: !17AS  To: !17AS  Size: !4UW/!4<!UW!>  !8AS: !AS !AS",-
		  sticker,frames,timeint,timefrac,origin,target,datasize,totalsize,tag,protocol,style)
$ endif
$ if (framedata.and.format_data)
$ then
$    count = 0
$    offset = 32
$    sequence = 0
$    max_count = payload
$    if (max_count.gt.datasize) then max_count = datasize
$    if ((frametype.ne.0).and.(f$edit(protocol,"COLLAPSE").eqs."none"))
$    then ! some control information is in fact data
$	offset = offset - overhead ! backtrack start-of-data index
$	line[offset*byte,overhead*byte] = f$cvui(24*byte,overhead*byte,line)
$    endif
$    goto EXTRACT
$ endif
$ goto LOOP
$TYPE_2: ! last frame data
$ moredata = 0
$TYPE_6: ! continuation frame data
$ if .not.(format_data) then goto LOOP
$ frameseq = f$cvui(1*byte,byte,line)
$ if (frameseq.ne.sequence)
$ then
$    put " << improper frame data sequence ... byte ordering lost >>"
$    goto LOOP
$ endif
$ offset = 2
$EXTRACT:
$    data'count' = f$cvui(offset*byte,byte,line)
$    offset = offset + 1
$    count = count + 1
$    if ((offset.lt.48).and.(count.lt.max_count)) then goto EXTRACT
$ sequence = sequence + 1
$ if .not.(moredata)
$ then
$    count = 0
$NEWLINE:
$    hex   = f$fao("  Data !4ZL> ",count)
$    ascii = "ASCII> "
$    if (max_count.eq.0) then goto SHOWLINE
$    index = 0
$BUILD:
$    hex = hex + f$fao("!XB,",data'count')
$    char[0,8] = data'count'
$    if ((f$cvui(5,2,char).eq.0).or.(f$cvui(0,7,char).eq.%x7f)) then char = "."
$    ascii = ascii + char
$    index = index + 1
$    count = count + 1
$    if ((index.lt.25).and.(count.lt.max_count)) then goto BUILD
$    if (count.ge.max_count) then hex = f$extract(0,f$length(hex)-1,hex)
$SHOWLINE:
$    put f$fao("!88AS !AS",hex,ascii)
$    if (count.lt.max_count) then goto NEWLINE
$ endif
$ goto LOOP
$!
$TYPE_3: ! Protocol and Node definitions
$ subentry = f$cvui(1*byte,byte,line)
$ if (subentry.eq.0) then goto LOOP
$ if (subentry.eq.1)
$ then
$    id = f$cvui(2*byte,word,line)
$    ptclsize = f$cvui(6*byte,word,line)
$    ptcl = f$extract(8,ptclsize,line)
$    value = f$fao("!2XB-!2XB",f$cvui(20*byte,byte,line),f$cvui(21*byte,byte,line))
$    put f$fao("Protocol (!1XW)  !12AS = !AS",id,ptcl,value)
$    sqz = value - "-"
$    p'sqz' = ptcl
$ else
$  if (subentry.eq.2)
$  then
$    id = f$cvui(2*byte,word,line)
$    namesize = f$cvui(6*byte,word,line)
$    name = f$extract(8,namesize,line)
$    vendor = f$fao("!2XB-!2XB-!2XB",f$cvui(20*byte,byte,line),f$cvui(21*byte,byte,line),f$cvui(22*byte,byte,line))
$    machine = f$fao("!2XB-!2XB-!2XB",f$cvui(23*byte,byte,line),f$cvui(24*byte,byte,line),f$cvui(25*byte,byte,line))
$    put f$fao("Node          !12AS = !AS-!AS",name,vendor,machine)
$    sqz = vendor + machine - "-" - "-" - "-" - "-"
$    n'sqz' = name
$  else
$   if (subentry.eq.3)
$   then
$    id = f$cvui(2*byte,word,line)
$    namesize = f$cvui(6*byte,word,line)
$    name = f$extract(8,namesize,line)
$    vendor = f$fao("!2XB-!2XB-!2XB",f$cvui(20*byte,byte,line),f$cvui(21*byte,byte,line),f$cvui(22*byte,byte,line))
$    put f$fao("Vendor        !12AS = !AS",name,vendor)
$    sqz = vendor - "-" - "-"
$    v'sqz' = f$edit(f$fao("!8AS",name),"TRIM")
$   else
$    put "<<Unknown table entry>>"
$   endif
$  endif
$ endif
$ goto LOOP
$!
$TYPE_4: ! Collision report
$ put f$fao(" CD report (!UL) Frames transmitted: !UL, deferred: !UL, C1: !UL, C2+: !UL",-
            f$cvui(4*byte,long,line),f$cvui(16*byte,long,line),-
            f$cvui(20*byte,long,line),f$cvui(24*byte,long,line),f$cvui(28*byte,long,line))
$ goto LOOP
$!
$TYPE_5: ! Cycle start
$ cycle_frames = 0
$ put ""
$ put f$fao("Cycle !UL started",f$cvui(4*byte,long,line))
$ goto LOOP
$!
$TYPE_7: ! Cycle end
$ flag = f$cvui(1*byte,byte,line)
$ if (flag.eq.1) then put f$fao("Cycle !UL stopped (!UL frame!%S)",f$cvui(4*byte,long,line),cycle_frames)
$ goto LOOP
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$BAD_PARAM:
$ write sys$output "%FORMAT-E-NOSPEC, input file specification missing"
$ goto DONE
$NO_INPUT:
$ write sys$output "%FORMAT-E-NOFILE, cannot find input file ''file'"
$ goto DONE
$BAD_INPUT:
$ write sys$output "%FORMAT-E-NOREAD, error reading input file ''infile'"
$ goto DONE
$BAD_OUTPUT:
$ write sys$output "%FORMAT-E-NOOUTP, error writing to analysis file"
$ goto DONE
$NO_PRB:
$ write sys$output "%FORMAT-E-NOTPRB, no PROBE signature found in input file"
$ goto DONE
$BAD_VERSION:
$ write sys$output "%FORMAT-E-UNKVER, unknown PROBE recording version"
$ goto DONE
$BAD_DATATYPE:
$ write sys$output "%FORMAT-E-NOVMSF, unknown PROBE recording data type"
$ goto DONE
$BAD_TYPE:
$ write sys$output "%FORMAT-E-UNKTYP, invalid record type (''entry') found"
$ goto DONE
$HANDLER:
$ write sys$output "%FORMAT-E-??????, unexpected condition signaled"
$DONE:
$ if (f$trnlnm("recording").nes."") then close recording
$ if (f$trnlnm("analysis").nes."") then close analysis
$ exit
