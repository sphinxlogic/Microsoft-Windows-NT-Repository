$! FORMAT.COM
$!
$!This software is COPYRIGHT © 1989-1996, Stephane Germain. ALL RIGHTS RESERVED.
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
$! recording made with PROBE version 2.2*.
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
$ if (f$cvsi(0,byte,line).ne.%x01) then goto NO_PRB
$ if (f$cvsi(6*byte,word,line).ne.%x0202) then goto BAD_VERSION
$ if (f$cvsi(3*byte+4,1,line).eq.0)	! 2.2B: 1=alternate map
$ then
$    if (f$cvsi(3*byte+6,1,line).eq.0) then goto BAD_DATATYPE ! no VMS F floating or Quad time
$    if (f$cvsi(3*byte+7,1,line).eq.0)	! 2.2B: 0=VAX 1=AXP
$    then platform = "VAX"
$    else platform = "Alpha"
$    endif
$    if (f$cvsi(3*byte+5,1,line).eq.0)	! 2.2B: 1=extracted
$    then platform = platform + ": original recording"
$    else platform = platform + ": extracted sample"
$    endif
$ else
$    platform = "platform unknown"
$ endif
$ payload  = f$cvsi(2*byte,12,line)
$ nodesize = f$cvsi(31*byte,byte,line)
$ devsize  = f$cvsi(30*byte,byte,line)
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
$ put "PROBE 2.2B RECORDING-FILE FORMATTER"
$ put ""
$ put f$fao("Input file: !AS",infile)
$ put f$fao("Made on:    !AS::!AS (!AS)",node,dev,platform)
$ put f$fao("Cycle:      !UL (Standby: !UL second!%S, Activity: !UL second!%S)",-
            f$cvsi(28*byte,word,line),f$cvsi(26*byte,word,line),f$cvsi(24*byte,word,line))
$ put f$fao("Payload:    !UL byte!%S recorded per frame !AS",payload,notice)
$ put ""
$!
$ frames = 0
$LOOP:
$ read/end=DONE/error=BAD_INPUT recording line
$ entry = f$cvsi(0,byte,line)
$ if (entry.gt.7) then goto BAD_TYPE
$ goto type_'entry'
$!
$TYPE_0:
$ frames = frames + 1
$ cycle_frames = cycle_frames + 1
$ frametype = f$cvsi(1*byte,1,line)
$ if (frametype.eq.0)
$ then tag = "(Ethernet)"
$ else tag = "(IEEE)"
$ endif
$ framedata = f$cvsi(1*byte+1,1,line)
$ moredata  = f$cvsi(1*byte+2,1,line)
$! iowarning = f$cvsi(1*byte+3,1,line)
$ overhead  = f$cvsi(1*byte+4,4,line)
$ if (overhead.lt.0) then overhead = 8	! because previous extract is sign-extented to longword
$ framesize = f$cvsi(2*byte,word,line)
$ totalsize = framesize + overhead + 18
$ if (totalsize.lt.64) then totalsize = 64
$ srcvendor  = f$fao("!2XB-!2XB-!2XB",f$cvsi(8*byte,byte,line),f$cvsi(9*byte,byte,line),f$cvsi(10*byte,byte,line))
$ srcmachine = f$fao("!2XB-!2XB-!2XB",f$cvsi(11*byte,byte,line),f$cvsi(12*byte,byte,line),f$cvsi(13*byte,byte,line))
$ dstvendor  = f$fao("!2XB-!2XB-!2XB",f$cvsi(16*byte,byte,line),f$cvsi(17*byte,byte,line),f$cvsi(18*byte,byte,line))
$ dstmachine = f$fao("!2XB-!2XB-!2XB",f$cvsi(19*byte,byte,line),f$cvsi(20*byte,byte,line),f$cvsi(21*byte,byte,line))
$ protocol   = f$fao("!2XB-!2XB",f$cvsi(24*byte,byte,line),f$cvsi(25*byte,byte,line))
$ origin  = srcvendor + "-" + srcmachine
$ address = origin - "-" - "-" - "-" - "-" - "-"
$ if (f$type(n'address').nes."") then origin = n'address'
$ target  = dstvendor + "-" + dstmachine
$ address = target - "-" - "-" - "-" - "-" - "-"
$ if (f$type(n'address').nes."") then target = n'address'
$ sqzptcl = protocol - "-"
$ if (f$type(p'sqzptcl').nes."") then protocol = p'sqzptcl'
$ put f$fao("Frame !10ZL !10AS @ XXXXX.YY  From: !17AS  To: !17AS  Size: !4UW/!4<!UW!>  Protocol: !AS",-
            frames,tag,origin,target,framesize,totalsize,protocol)
$ if (framedata.and.format_data)
$ then
$    count = 0
$    max_count = payload
$    if (max_count.gt.framesize) then max_count = framesize
$    sequence = 0
$    offset = 32
$    goto EXTRACT
$ endif
$ goto LOOP
$TYPE_2: ! last frame data
$ moredata = 0
$TYPE_6: ! continuation frame data
$ if .not.(format_data) then goto LOOP
$ frameseq = f$cvsi(1*byte,byte,line)
$ if (frameseq.ne.sequence)
$ then
$    put " << improper frame data sequence ... byte ordering lost >>"
$    goto LOOP
$ endif
$ offset = 2
$EXTRACT:
$    data'count' = f$cvsi(offset*8,byte,line)
$    offset = offset + 1
$    count = count + 1
$    if ((offset.lt.48).and.(count.lt.max_count)) then goto EXTRACT
$ sequence = sequence + 1
$ if .not.(moredata)
$ then
$    count = 0
$NEWLINE:
$    hex   = f$fao(" Data !4ZL> ",count)
$    ascii = "ASCII> "
$    index = 0
$BUILD:
$    hex = hex + f$fao("!XB,",data'count')
$    char[0,8] = data'count'
$    if ((f$cvsi(5,2,char).eq.0).or.(f$cvsi(0,7,char).eq.%x7f)) then char = "."
$    ascii = ascii + char
$    index = index + 1
$    count = count + 1
$    if ((index.lt.25).and.(count.lt.max_count)) then goto BUILD
$    if (count.ge.max_count) then hex = f$extract(0,f$length(hex)-1,hex)
$    put f$fao("!87AS !AS",hex,ascii)
$    if (count.lt.max_count) then goto NEWLINE
$ endif
$ goto LOOP
$!
$TYPE_3: ! Protocol and Node definitions
$ subentry = f$cvsi(1*byte,byte,line)
$ if (subentry.eq.0) then goto LOOP
$ if (subentry.eq.1)
$ then
$    id = f$cvsi(2*byte,word,line)
$    ptclsize = f$cvsi(6*byte,word,line)
$    ptcl = f$extract(8,ptclsize,line)
$    value = f$fao("!2XB-!2XB",f$cvsi(20*byte,byte,line),f$cvsi(21*byte,byte,line))
$    put f$fao("Protocol (!1XW)  !12AS = !AS",id,ptcl,value)
$    sqz = value - "-"
$    p'sqz' = ptcl
$ else
$  if (subentry.eq.2)
$  then
$    id = f$cvsi(2*byte,word,line)
$    namesize = f$cvsi(6*byte,word,line)
$    name = f$extract(8,namesize,line)
$    vendor = f$fao("!2XB-!2XB-!2XB",f$cvsi(20*byte,byte,line),f$cvsi(21*byte,byte,line),f$cvsi(22*byte,byte,line))
$    machine = f$fao("!2XB-!2XB-!2XB",f$cvsi(23*byte,byte,line),f$cvsi(24*byte,byte,line),f$cvsi(25*byte,byte,line))
$    put f$fao("Node          !12AS = !AS-!AS",name,vendor,machine)
$    sqz = vendor + machine - "-" - "-" - "-" - "-"
$    n'sqz' = name
$  else
$     put "<<Unknown table entry>>"
$  endif
$ endif
$ goto LOOP
$!
$TYPE_4: ! Collision report
$ put f$fao("CD report (!UL) Frames transmitted: !UL, deferred: !UL, C1: !UL, C2+: !UL",-
            f$cvsi(4*byte,long,line),f$cvsi(16*byte,long,line),-
            f$cvsi(20*byte,long,line),f$cvsi(24*byte,long,line),f$cvsi(28*byte,long,line))
$ goto LOOP
$!
$TYPE_5: ! Cycle start
$ cycle_frames = 0
$ put ""
$ put f$fao("Cycle !UL started",f$cvsi(4*byte,long,line))
$ goto LOOP
$!
$TYPE_7: ! Cycle end
$ flag = f$cvsi(1*byte,byte,line)
$ if (flag.eq.1) then put f$fao("Cycle !UL stopped (!UL frame!%S)",f$cvsi(4*byte,long,line),cycle_frames)
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
