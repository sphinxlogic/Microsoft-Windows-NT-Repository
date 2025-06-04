$set nover
$gosub INIT
$opc=""
$goto OPC_DONE
$CHK_ASS:if tok.eqs."ASSUME" then gosub SAV_ASS
$NXT_LINE:
$OPC_DONE:gosub GET_NEXT_LINE
$if alpha then goto M105
$opc_num=f$extract(opc_p,4,line)
$if opc_num.eqs."    " then goto NXT_LINE
$M105:term=1
$gosub GET_TOKEN
$if token.eqs."" then goto NXT_LINE
$label=""
$if f$extract(F$length(token)-1,1,TOKEN).nes.":" then goto M200
$label=token
$term=2
$gosub GET_TOKEN
$M200:i=0
$tok=f$edit(token,"UPCASE")
$tok_len=f$len(tok)
$M210:if i.eq.tok_len then goto M215
$if f$loc(f$extr(i,1,tok),opc_chr).eq.opc_len then goto NXT_LINE
$i=i + 1
$goto M210
$M215:if f$type(O_'token).eqs."" then goto CHK_ASS
$if f$loc(f$extr(0,2,opc),br_prefix).eq.br_prefix_len then last_opc=opc
$opc=token
$opc_loc=" "'opc'" "
$term=3
$arg=0
$first_ptr=ptr
$GET_NEXT_ARG:cur_dt=""
$sav_ptr=ptr
$gosub GET_TOKEN
$goto PROC_ARG
$GET_NEXT_LINE:read/end=NEXT mar_file line
$write typchk_file line
$if f$extract(0,1,line).eqs.form_feed then gosub CHK_HDR
$if (f$extr(src_p,7,line).eqs."EVAX = ").or.(f$extr(src_p,6,line).eqs."VAX = ").or.(f$extr(src_p,14,line).eqs."ADDRESSBITS = ") then gosub RESET_DFLT
$if .not. alpha then goto GNL10
$if (f$extr(lnum_p,lnum_s,line).eqs.line_num).and.(f$extr(metab_p,1,line).nes."	") then goto GET_NEXT_LINE
$if f$extract(flg_p,1,line).eqs."F" then goto GET_NEXT_LINE
$line_num=f$extr(lnum_p,lnum_s,line)
$if (f$extr(metab_p,1,line).eqs."	") then line_num="MACRO_EXP"
$GNL5:ptr=src_p
$if alpha.and.(line_num.eqs."MACRO_EXP") then ptr=metab_p
$first_ptr=err_p
$sav_ptr=err_p
$return
$GNL10:line_num=f$extr(lnum_p,lnum_s,line)
$if (line_num.eqs."     ") then line_num="MACRO_EXP"
$goto GNL5
$GET_TOKEN:special_constant=0
$TOKEN=""
$goto T01
$NXT_CHR:ptr=ptr + 1
$T01:a=f$extract(ptr,1,line)
$if a.eqs."" then goto chk_cont
$if special_constant.eq.0 then goto T05
$if a.eqs.terminator then special_constant=0
$goto T2
$T05:if (f$extract(f$length(token)-3,3,token).nes."#^A").and.(f$extract(f$length(token)-3,3,token).nes."#^M") then goto T0
$special_constant=1
$terminator=a
$if (f$extract(f$length(token)-3,3,token).eqs."#^M") then terminator=">"
$goto T2
$T0:if a.nes.";" then goto T1
$ptr=f$length(line)+1
$goto chk_cont
$T1:filler=(a.eqs." ").or.(a.eqs."	")
$if filler.and.(token.eqs."") then goto NXT_CHR
$if (term.eq.3).and.(a.eqs.",") then goto CHK_CONT
$if (term.ne.3).and.filler then goto CHK_CONT
$if (term.eq.1).and.(a.eqs.":") then goto LABEL_TERM
$if filler then GOTO NXT_CHR
T2:token=token + a
$goto nxt_chr
$CHK_CONT:if f$extract(f$length(token)-1,1,token).nes."-" then return
$token=f$extract(0,f$length(token)-1,token)
$gosub GET_NEXT_LINE
$goto T01
$LABEL_TERM:token=token + a
$ptr=ptr + 1
$a=f$extract(ptr,1,line)
$if a.nes.":" then goto CHK_CONT
$token=token + a
$ptr=ptr + 1
$goto CHK_CONT
$PROC_ARG:if token.eqs."" then goto OPC_DONE
$tok=f$edit(token,"UPCASE")
$tok_len=f$len(tok)
$ptr=ptr + 1
$chk_no_fld=0
$dtype=f$element(arg," ",O_'opc)
$typ=f$ele(0,"-",dtype)
$i=f$loc("LONG",typ).ne.f$len(typ)
$ii=f$loc("QUAD",typ).ne.f$len(typ)
$if (i.and..not. soft_64).or.(ii.and.soft_64) then typ=typ + ",INT_S"
$if (i.and..not. hard_64).or.(ii.and.hard_64) then typ=typ + ",INT_H"
$i=f$loc("PTR_L",typ).ne.f$len(typ)
$ii=f$loc("PTR_Q",typ).ne.f$len(typ)
$if (i.and..not. soft_64).or.(ii.and.soft_64) then typ=typ + ",PTR_S"
$if (i.and..not. hard_64).or.(ii.and.hard_64) then typ=typ + ",PTR_H"
$arg_dt=typ
$arg=arg + 1
$if typ.eqs." " then goto OPC_DONE
$lit=( (f$extr(0,1,tok).eqs."#").or.(f$extr(0,3,tok).eqs."S^#").or.(f$extr(0,3,tok).eqs."I^#") )
$idx=( (f$loc("[",tok).ne.tok_len).and.(f$extr(0,3,tok).nes."#^A") )
$if .not. lit then goto GO5
$if (f$extr(0,1,tok).eqs."#") then i=f$extr(1,f$len(tok)-1,tok)
$if (f$extr(0,3,tok).eqs."S^#").or.(f$extr(0,3,tok).eqs."I^#") then i=f$extr(3,f$len(tok)-3,tok)
$goto NEXT_PAG
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$NEXT_PAG:gosub CHK_INT
$if int then goto GET_DT
$if f$loc("^A",tok).ne.tok_len then goto GET_DT
$i=typ
$ii=f$len(i)
$if (f$loc("CONSTANT",i).eq.ii).and.(f$loc("SIZE",i).eq.ii).and.(f$loc("MASK",i).eq.ii).and.(f$loc("VIELD",i).eq.ii).and.(f$loc("REG_MASK",i).eq.ii) then typ="CONSTANT,MASK,SIZE"
$goto GO7
$GO5:i=tok
$gosub CHK_INT
$if int.and.FLAG_INTEGERS then goto INT_ERR
$GO7:i=0
$GO10:dt=f$elem(i,",",typ)
$if dt.eqs."," then goto OUT_ERR
$i=i + 1
$n=0
$GO20:str=f$ele(n," ",T_'dt)
$if str.eqs." " then goto GO10
$n=n + 1
$if f$loc(str,tok).ne.tok_len then goto TM_CHK
$if str.eqs."*NO_CHECK*" then goto GET_DT
$if str.nes."*LABEL*" then goto GO20
$if f$extr(tok_len-1,1,tok).nes."$" then goto GO20
$i=f$extr(0,tok_len-1,tok)
$gosub chk_INT
$if int then goto GET_DT
$goto GO20
$OUT_ERR:i=1
$OE10:if i .gt. num_exc then goto OE30
$if f$loc(E_'i,tok).ne.tok_len then goto GET_DT
$i=i + 1
$goto OE10
$OE30:gosub CHK_NO_FLD
$if ((i.eqs."").or.int).and..not. reg_only_args then goto OE40
$ii:=" "'tok'" "
$if (f$loc(ii,regs).ne.reg_len).and..not. reg_only_args then goto OE40
$if .not. flag_local.and.(f$loc("$",tok).eq.tok_len).and..not. int then goto GET_DT
$if (i.eqs."").or.int.or.(f$loc(ii,regs).ne.reg_len) then goto OE35
$if f$extr(1,1,i).eqs."^" then i=f$extr(2,f$len(i)-2,i)
$if f$extr(0,1,i).eqs."#" then i=f$extr(1,f$len(i)-1,i)
$i:=" "'i'" "
$if (f$loc(i,ass1).ne.f$len(ass1)).or.(f$loc(i,ass2).ne.f$len(ass2)).or.(f$loc(i,ass3).ne.f$len(ass3)).or.(f$loc(i,ass4).ne.f$len(ass4)).or.(f$loc(i,ass5).ne.f$len(ass5)) then goto GET_DT
$OE35:n:=" not "'typ'" field "
$gosub out_warn
$goto GET_DT
$OE40:if typ.eqs."REG_MASK" then goto OE35
$goto GET_DT
$CHK_INT:n=f$extr(0,2,i)
$if (n.eqs."B^").or.(n.eqs."W^").or.(n.eqs."L^") then i=f$extr(2,f$len(i)-2,i)
$ii=num_exp
$len=num_exp_len
$CI07:int=0
$n=0
$CI10:if n.eq.f$len(i) then goto CI15
$tmp=f$extr(n,1,i)
$if tmp.eqs."^" then goto CI30
$if f$loc(tmp,ii).eq.len then return
$n=n + 1
$goto CI10
$CI15: int=1
$return
$CI30:n=n + 1
$if n.eq.f$len(i) then return
$tmp=f$extr(n,1,i)
$n=n + 1
$if f$loc(tmp,radix_chr).eq.radix_chr_len then return
$if tmp.eqs."C" then goto CI10
$if tmp.eqs."X" then goto CI35
$ii=num_exp
$len=num_exp_len
$goto CI10
$CI35:ii=num_hex_exp
$len=num_hex_exp_len
$goto CI10
$CHK_NO_FLD:i=f$elem(0,"(",tok)
$if f$loc("[",i).ne.f$len(i) then i=f$elem(0,"[",i)
$if (f$extr(0,1,i).eqs."@").or.(f$extr(0,1,i).eqs."#") then i=f$extr(1,f$len(i)-1,i)
$ii:=" "'(f$extr(0,2,i)'" "
$if f$loc(ii,reg_chr).ne.reg_chr_len then i=f$extr(2,f$len(i)-2,i)
$gosub CHK_INT
$chk_no_fld=1
$return
$GET_DT:l=0
$GD10:dt=f$ele(l," ",TMs)
$if dt.eqs." " then goto OPT_CHK
$l=l + 1
$n=0
$GD20:str=f$ele(n," ",T_'dt)
$if str.eqs." " then goto GD10
$n=n + 1
$if str.eqs."*NO_CHECK*" then goto GD30
$if f$loc(str,tok).ne.tok_len then goto TM_CHK
$goto GD20
$GD30: if dt.nes.arg_dt then goto GD20
$TM_CHK:if f$type(TM_'dt).eqs."" then goto OPT_CHK
$cur_dt=dt
$tm=TM_'dt
$l=0
$tmp=""
$TM10:am=f$ele(l," ",tm)
$if (am.eqs." ").and.(tmp.nes."") then goto TM_ERR
$if (am.eqs." ") then goto OPT_CHK
$l=l + 1
$i=(f$extr(0,1,am).eqs."^")
$if i then am=f$extr(1,f$len(am)-1,am)
$if .not. i then tmp='tmp'","'am'
$TM20:if .not. i.and.lit then goto OPT_CHK
$if i.and.lit then goto ILL_MODE_ERR
$goto TM10
$OPT_CHK:if .not. int.and..not. chk_no_fld then gosub CHK_NO_FLD
$if flag_nodt_idx.and.int.and.idx then gosub NODT_IDX_WARN
$if flag_idx.and.(.not. int).and.idx then gosub DT_IDX_CHK
$if flag_reg_arg.and.(f$loc(opc_loc,regarg_opc).ne.regarg_opc_len).and.(arg.eq.1) then gosub REG_ARG_WARN
$i=f$extr(0,1,f$ele(1,"-",dtype))
$if flag_wrlit.and.lit.and.(i.nes."-") then if (i.eqs."W").or.(i.eqs."M") then gosub WRITE_LITERAL
$if .not. flag_unsign.or.(f$loc(opc_loc,br_opc).eq.br_opc_len) then goto GET_NEXT_ARG
$ii=" "'last_opc'" "
$if f$loc(ii,bw_opc).eq.bw_opc_len then goto GET_NEXT_ARG
$tok=""
$tok_len=0
$point_opc=1
$n:='opc'"U needed after "'last_opc'" operation ? "
$gosub out_warn
$point_opc=0
$goto GET_NEXT_ARG
$NODT_IDX_WARN:gosub get_idx_dt
$if dt.eqs."" then return
$n:=" using "'f$ele(0,",",dt)'" indexing on Unknown Data Type field "
$gosub out_warn
$return
$DT_IDX_CHK:gosub get_idx_dt
$if (dt.eqs."").or.(cur_dt.eqs.dt) then return
$l=0
$DIC05:typ=f$elem(l,",",dt)
$if typ.eqs."," then goto DIC20
$l=l + 1
$n=0
$DIC10:str=f$ele(n," ",T_'typ)
$if str.eqs." " then goto DIC05
$n=n + 1
$if f$loc(str,tok).ne.tok_len then return
$goto DIC10
$DIC20:n:=" using "'f$ele(0,",",dt)'" indexing on non-"'f$ele(0,",",dt)'" field "
$gosub out_warn
$return
$GET_IDX_DT:dt=""
$i=f$ele(1,"-",dtype)
$if (i.eqs."-").or.(f$extr(0,1,i).eqs."B") then return
$i=" " + f$extr(1,1,i) + "-"
$i=f$ele(1," ",f$extr(f$loc(i,idx_dt),idx_dt_len,idx_dt))
$dt=f$ele(1,"-",i)
$i=f$loc("LONG",dt).ne.f$len(dt)
$ii=f$loc("QUAD",dt).ne.f$len(dt)
$if i then dt=dt + ",ADDR,PTR_L"
$if ii then dt=dt + ",PTR_Q"
$if (i.and..not. soft_64).or.(ii.and.soft_64) then dt=dt + ",INT_S,PTR_S"
$if (i.and..not. hard_64).or.(ii.and.hard_64) then dt=dt + ",INT_H,PTR_H"
$if f$loc("BYTE",dt).ne.f$len(dt) then dt=dt + ",TEXT"
$return
$WRITE_LITERAL:n:=" is literal used as address for write "
$gosub out_warn
$return
$REG_ARG_WARN:sav_tok=tok
$tok=""
$tok_len=0
$point_opc=1
$n:='opc'" instruction, R0-R"'f$extr(f$loc(opc_loc,regarg_opc)+f$len(opc_loc),1,regarg_opc)'" destroyed "
$gosub out_warn
$tok=sav_tok
$tok_len=f$len(tok)
$point_opc=0
$return
$TM_ERR:n:=" not used in "'f$extr(1,f$len(tmp)-1,tmp)'" addressing mode "
$gosub out_warn
$goto OPT_CHK
$ILL_MODE_ERR:n:=" unexpected use of "'am'" addressing mode "
$gosub out_warn
$goto OPT_CHK
$out_warn:i=err_off + (sav_ptr - first_ptr) + (tok_len/2) - f$len(n) - tok_len
$if point_opc then i=i - 6
$if i .le. 0 then i=1
$if .not. alpha.and.(line_num.eqs."MACRO_EXP") then i=i + 8
$write typchk_file "WARN: ",tok,n,f$extr(0,i,dash),"^"
$return
$INT_ERR:n:=" is Integer not used as LITERAL "
$gosub out_warn
$goto GET_DT
$CHK_HDR:if alpha then goto M9
$read/end=NEXT mar_file line
$write typchk_file line
$if f$extract(0,17,line).nes."Table of contents" then goto M10
$M5:read/end=NEXT mar_file line
$write typchk_file line
$if f$extract(0,1,line).nes.form_feed then goto M5
$goto CHK_HDR
$M10:if (f$extract(0,12,line).nes."Symbol table").and.(f$extract(0,14,line).nes."Psect synopsis") then goto M7
$M14:gosub NEXT
$goto M8
$M7:read/end=NEXT mar_file line
$write typchk_file line
$M8:read/end=NEXT mar_file line
$write typchk_file line
$return
$M9:read/end=NEXT mar_file line
$write typchk_file line
$if f$extract(mcl_p,20,line).eqs."Machine Code Listing" then goto M14
$read/end=NEXT mar_file line
$write typchk_file line
$goto M7
$RESET_DFLT:if f$extr(src_p,8,line).eqs."EVAX = 1" then hard_64 = 1
$if f$extr(src_p,7,line).eqs."VAX = 1" then hard_64 = 0
$if f$extr(src_p,14,line).eqs."ADDRESSBITS = " then soft_64=+f$ele(1,"=",line).eq.64
$return
$SAV_ASS:ass5=ass4
$ass4=ass3
$ass3=ass2
$ass2=ass1
$ass1=f$edit(f$ele(0,";",f$extr(ptr,f$len(line)-ptr,line)),"COMPRESS,UPCASE")
$i=0
$ii=f$len(ass1)
$CA10:if i.eq.ii then return
$if f$loc(f$extr(i,1,ass1),oper_chr).ne.oper_chr_len then ass1=f$extr(0,i,ass1)+" "+f$extr(i+1,ii,ass1)
$i=i+1
$goto CA10
$GDF6:write sys$output ""
$write sys$output "Error - No Definition file (TYPCHK.DEFINITIONS) in local directory or in ",MASTER_LOC
$write sys$output "        Also logical TYPCHK$DEFINITIONS not defined."
$define_file=""
$goto done
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$INIT:gosub INIT_CONSTANTS
$GET_DEFN_FILE:define_file="TYPCHK.DEFINITIONS"
$open/error=GDF4 def_file 'define_file
$goto GDF10
$GDF4:define_file=f$trnlnm("TYPCHK$DEFINITIONS")
$if define_file.eqs."" then goto GDF5
$open/error=GDF5 def_file 'define_file
$goto GDF10
$GDF5:define_file=master_loc+"TYPCHK.DEFINITIONS"
$open/error=GDF6 def_file 'define_file
$goto GDF10
$GDF10:
$ND0:gosub GET_DEFN_TOKEN
$NEXT_DEFN:if token.eqs."" then goto DEFN_DONE
$if tok_type.nes."KEYWORD" then goto ND0
$if token.nes."DEFINE_OPCODE" then goto ND20
$gosub GET_DEFN_TOKEN
$if token.eqs."" then goto DEFN_DONE
$if tok_type.eqs."KEYWORD" then goto NEXT_DEFN
$opc=token
$args=""
$multiple_mode=0
$ND5:gosub GET_DEFN_TOKEN
$if token.eqs."" then goto ND17
$if tok_type.eqs."KEYWORD" then goto ND17
$i=f$ele(1,"-",token)
$if (i.nes."-") then if (f$len(i).ne.2).or.(f$loc(f$extr(0,1,i),"RWMB").eq.f$len("RWMB")).or.(f$loc(f$extr(1,1,i),"BWLQODFGH").eq.f$len("BWLQODFGH")) then goto SUFFIX_ERR
$if multiple_mode.eq.1 then goto ND8
$if f$extr(0,1,token).nes."(" then goto ND9
$if f$loc(")",token).ne.f$len(token) then goto ND89
$multiple_mode=1
$save_mult=""
$token=f$extr(1,f$len(token)-1,token)
$if token.eqs."" then goto ND5
$ND7:ii=f$ele(0,"-",token)
$if f$type(t_'ii).eqs."" then goto ND16
$save_mult:='save_mult'","'token'
$goto ND5
$ND8:if f$loc("(",token).ne.f$len(token) then goto ND15
$if f$loc(")",token).eq.f$len(token) then goto ND7
$if f$loc("-",token).ne.f$len(token) then token=f$ele(0,")",token) + f$ele(1,")",token) + ")"
$token=f$extr(0,f$len(token)-1,token)
$ii=f$ele(0,"-",token)
$if (token.nes."").and.(f$type(t_'ii).eqs."") then goto ND16
$if token.nes."" then save_mult:='save_mult'","'token'
$if f$extr(0,1,save_mult).eqs."," then save_mult=f$extr(1,f$len(save_mult)-1,save_mult)
$if save_mult.nes."" then args:='args'" "'save_mult'
$multiple_mode=0
$goto ND5
$ND89:token=f$extr(1,f$len(token)-1,token)
$token=f$ele(0,")",token) + f$ele(1,")",token)
$ND9:ii=f$ele(0,"-",token)
$if f$type(t_'ii).eqs."" then goto ND16
$args:='args'" "'token'
$goto ND5
$ND17:if (multiple_mode.eq.1) then goto ND15
$if f$extr(0,1,args).eqs." " then args=f$extr(1,f$len(args)-1,args)
$if args.nes."" then O_'opc:='args'
$if token.eqs."" then goto DEFN_DONE
$goto next_defn
$GET_DEFN_TOKEN:token=""
$TOK_TYPE=""
$GDT10: gosub GET_DEFN_CHAR
$if chr.eqs."EOF" then goto NO_MORE_DEFN_TOKENS
$if chr.eqs."" then goto GOT_DEFN_TOKEN
$token=token + chr
$goto GDT10
$GOT_DEFN_TOKEN:if token.eqs."" then goto GDT10
$t1:=" "'token'" "
$if f$loc(t1,keywords).ne.keywords_len then tok_type="KEYWORD"
$return
$NO_MORE_DEFN_TOKENS:return
$GET_DEFN_CHAR:if line1.nes."" then goto GDC10
$read/end=NO_MORE_DEFN_CHARS def_file line
$line1=f$edit(line,"UPCASE,TRIM,COMPRESS")
$line_pos=0
$GDC10:chr=f$extr(line_pos,1,line1)
$line_pos=line_pos + 1
$if (chr.eqs."!").or.(chr.eqs."") then line1=""
$if (chr.eqs." ") .or.(chr.eqs."!").or.(chr.eqs."	").or.(chr.eqs.",") then chr =""
$return
$NO_MORE_DEFN_CHARS: chr="EOF"
$return
$ND20: if token.nes."DEFINE_TYPE" then goto ND40
$gosub GET_DEFN_TOKEN
$if token.eqs."" then goto DEFN_DONE
$if tok_type.eqs."KEYWORD" then goto NEXT_DEFN
$type=token
$args=""
$ND25:gosub GET_DEFN_TOKEN
$if token.eqs."" then goto ND35
$if tok_type.eqs."KEYWORD" then goto ND30
$args:='args'" "'token'
$goto ND25
$ND30: if f$extr(0,1,args).eqs." " then args=f$extr(1,f$len(args)-1,args)
$if args.nes."" then T_'type:='args'
$goto next_defn
$ND35: if f$extr(0,1,args).eqs." " then args=f$extr(1,f$len(args)-1,args)
$if args.nes."" then T_'type:='args'
$goto DEFN_DONE
$ND40: if token.nes."DEFINE_EXCEPTION" then goto ND80
$ND45: gosub GET_DEFN_TOKEN
$if token.eqs."" then goto DEFN_DONE
$if tok_type.eqs."KEYWORD" then goto ND50
$E_'num_exc=token
$num_exc=num_exc + 1
$goto ND45
$ND50: goto next_defn
$ND80: if token.nes."DEFINE_TYPE_MODE" then goto ND100
$gosub GET_DEFN_TOKEN
$if token.eqs."" then goto DEFN_DONE
$if tok_type.eqs."KEYWORD" then goto NEXT_DEFN
$if f$type(T_'token).eqs."" then goto NO_SUCH_TYPE
$type=token
$args=""
$ND85:gosub GET_DEFN_TOKEN
$if token.eqs."" then goto ND95
$if tok_type.eqs."KEYWORD" then goto ND90
$i=token
$if f$extr(0,1,token).eqs."^" then i=f$extr(1,f$len(token)-1,token)
$ii=" "'i'" "
$if f$loc(ii,ams).eq.ams_len then goto NO_ADDR_MODE
$args:='args'" "'token'
$goto ND85
$ND90: if f$extr(0,1,args).eqs." " then args=f$extr(1,f$len(args)-1,args)
$if args.nes."" then TM_'type:='args'
$if args.nes."" then tms:='tms'" "'type'
$goto next_defn
$ND95: if f$extr(0,1,args).eqs." " then args=f$extr(1,f$len(args)-1,args)
$if args.nes."" then TM_'type:='args'
$if args.nes."" then tms:='tms'" "'type'
$goto DEFN_DONE
$NO_ADDR_MODE:write sys$output ""
$write sys$output "Error - No such Address Mode Defined: ",token
$goto done
$NO_SUCH_TYPE:write sys$output ""
$write sys$output "Error - No such Data Type: ",token
$goto done
$ND100: if token.nes."DEFINE_OPTION" then goto NEXT_DEFN
$ND105: gosub GET_DEFN_TOKEN
$if token.eqs."" then goto DEFN_DONE
$if tok_type.eqs."KEYWORD" then goto NEXT_DEFN
$t1:=" "'token'" "
$l=" FLAG_REGISTER_ONLY_ARGS FLAG_LOCAL FLAG_INTEGERS FLAG_NODT_INDEXING FLAG_INDEXING_MISMATCHES FLAG_UNSIGNED_BRANCHES FLAG_LITERAL_WRITE_ADDRS FLAG_IMPLICIT_REGISTER_USE FULL_TYPCHK_FILE ASSUME_SUPPRESS "
$if f$loc(t1,l).eq.f$len(l) then goto NO_SUCH_FLAG
$if token.eqs."FLAG_REGISTER_ONLY_ARGS" then reg_only_args=1
$if token.eqs."FLAG_LOCAL" then flag_local=1
$if token.eqs."FLAG_INTEGERS" then flag_integers=1
$if token.eqs."FLAG_NODT_INDEXING" then flag_nodt_idx=1
$if token.eqs."FLAG_INDEXING_MISMATCHES" then flag_idx=1
$if token.eqs."FLAG_UNSIGNED_BRANCHES" then flag_unsign=1
$if token.eqs."FLAG_LITERAL_WRITE_ADDRS" then flag_wrlit=1
$if token.eqs."FLAG_IMPLICIT_REGISTER_USE" then flag_reg_arg=1
$if token.eqs."FULL_TYPCHK_FILE" then flag_full=1
$if token.eqs."ASSUME_SUPPRESS" then flag_ass=1
$goto ND105
$NO_SUCH_FLAG:write sys$output ""
$write sys$output "Error - No such Flag: ",token
$goto done
$DEFN_DONE:if f$extr(0,1,tms).eqs." " then tms=f$extr(1,f$len(tms)-1,tms)
$num_exc=num_exc - 1
$close def_file
$define_file=""
$return
$DONE:close mar_file
$close typchk_file
$if f$type(define_file).nes."" then if define_file.nes."" then close def_file
$EXIT:exit
$NEXT:if .not. flag_full then goto DN5
$read/end=DN5 mar_file line
$write typchk_file line
$goto NEXT
$DN5:close mar_file
$close typchk_file
$DN10:if f$loc("*",file_name).eq.f$len(file_name) then goto EXIT
$name=f$sear(file_name)
$if name.eqs."" then goto EXIT
$gosub GET_NEXT_FILE
$if name.nes."" then return
$goto DN10
$ND15:write sys$output "Error - Unbalanced parentheses in DEFINE_OPCODE statement in Definition File encountered near line: ",line
$goto done
$ND16:write sys$output "Error - for opcode """,opc,""", Data Type """,token,"""  is not defined."
$goto done
$SUFFIX_ERR:write sys$output ""
$write sys$output "Error - Invalid Argument Suffix: ",token
$goto done
$INIT_CONSTANTS:delete/sym/all
$write sys$output ""
$inquire file_name "Enter VAX or AXP MACRO-32 Listing File to check"
$if file_name.eqs."" then goto exit
$i=f$elem(1,"]",file_name)
$if i.eqs."]" then i=file_name
$if f$loc(".",i).eq.f$len(i) then file_name=file_name + ".LIS"
$name=f$sear(file_name)
$if name.nes."" then goto IC10
$write sys$output ""
$write sys$output "No Such File - ",file_name
$goto init_constants
$IC10:gosub GET_NEXT_FILE
$if name.nes."" then goto IC15
$if f$loc("*",file_name).eq.f$len(file_name) then goto init_constants
$name=f$sear(file_name)
$if name.eqs."" then goto init_constants
$goto IC10
$IC15:form_feed=""
$dash="------------------------------------------------------------------------------------------------------------------------"
$num_hex_exp=" 0 1 2 3 4 5 6 7 8 9 A B C D E F + - * / @ & ! \ < > "
$num_hex_exp_len=f$len(num_hex_exp)
$num_exp    =" 0 1 2 3 4 5 6 7 8 9 + - * / @ & ! \ < > "
$num_exp_len=f$len(num_exp)
$oper_chr="+-*/@&!\<>"
$oper_chr_len=f$len(oper_chr)
$radix_chr=" D X B O C "
$radix_chr_len=f$len(radix_chr)
$opc_chr=" A B C D E F G H I J K L M N O P Q R S T U V W X Y Z 0 1 2 3 4 5 6 7 8 9 0 $ _ "
$opc_len=f$len(opc_chr)
$reg_chr=" G^ W^ B^ L^ "
$reg_chr_len=f$len(reg_chr)
$br_opc=" BGTR BLEQ BGEQ BLSS "
$br_opc_len=f$len(br_opc)
$bw_opc=" ACBB ACBW ADDB ADDB2 ADDB3 ADDW ADDW2 ADDW3 BICB BICB2 BICB3 BICW BICW2 BICW3 BISB BISB2 BISB3 BISW BISW2 BISW3"
$bw_opc:=""'bw_opc'" BITB BITW CLRB CLRW CMPB CMPW CVTBW CVTLB CVTLW CVTWB DECB DECW DIVB DIVB2 DIVB3 DIVW DIVW2 DIVW3"
$bw_opc:=""'bw_opc'" INCB INCW MCOMB MCOMW MNEGB MNEGW MOVB MOVW MOVZBW MULB MULB2 MULB3 MULW MULW2 MULW3 SUBB SUBB2 SUBB3"
$bw_opc:=""'bw_opc'" SUBW SUBW2 SUBW3 TSTB TSTW XORB XORB2 XORB3 XORW XORW2 XORW3 "
$bw_opc_len=f$len(bw_opc)
$br_prefix=" BE BG BL BN BV "
$br_prefix_len=f$len(br_prefix)
$idx_dt=" B-BYTE W-WORD L-LONG Q-QUAD O-OCTA D-DFLOAT F-FLOAT G-GFLOAT H-HFLOAT "
$idx_dt_len=f$len(idx_dt)
$regarg_opc:=" LOCC 1 MATCHC 3 MOVC 5 MOVC3 5 MOVC5 5 CMPC 3 CMPC3 3 CMPC5 3 SCANC 3 SKPC 1 SPANC 3 ADDP 3 ADDP4 3 ADDP6 5"
$regarg_opc:=""'REGARG_OPC'" ASHP 3 CMPP 3 CMPP3 3 CMPP4 3 CRC 3 CVTLP 3 CVTPL 3 CVTPS 3 CVTPT 3 CVTSP 3 CVTTP 3 EDITPC 5"
$regarg_opc:=""'REGARG_OPC'" MOVP 3 MOVTC 5 MOVTUC 5 MULP 5 POLYD 5 POLYF 3 POLYG 5 POLYH 5 SUBP 3 SUBP4 3 SUBP6 5 "
$regarg_opc_len=f$len(regarg_opc)
$keywords=" DEFINE_OPTION DEFINE_OPCODE DEFINE_EXCEPTION DEFINE_TYPE DEFINE_TYPE_MODE "
$keywords_len=f$len(keywords)
$ams=" LITERAL "
$ams_len=f$len(ams)
$num_exc=1
$flag_local=0
$flag_wrlit=0
$flag_integers=0
$reg_only_args=0
$flag_nodt_idx=0
$flag_idx=0
$flag_unsign=0
$flag_full=0
$flag_reg_arg=0
$flag_ass=0
$master_loc="DOCD$TOOLS:"
$return
$NOT_MACRO_FILE:write sys$output ""
$write sys$output "Error - Not a Macro Listing file: ",name
$write sys$output ""
$BAD_FILE:close mar_file
$close/nolog typchk_file
$name=""
$return
$GET_NEXT_FILE:i=f$elem(1,"]",name)
$if i.eqs."" then i=name
$i=f$element(0,".",i)
$open mar_file 'name
$on warning then goto done
$on error then goto done
$on control_y then goto done
$read/end=NOT_MACRO_FILE mar_file line
$alpha=""
$if f$loc(" VAX MACRO ",line).ne.f$len(line) then gosub SET_VAX
$if f$loc(" AMAC ",line).ne.f$len(line) then gosub SET_ALPHA
$if alpha.eqs."" then goto NOT_MACRO_FILE
$write sys$output ""
$write sys$output "Type Checking File: ",name
$write sys$output ""
$open/write typchk_file 'i.typchk
$write typchk_file line
$read/end=BAD_FILE mar_file line
$write typchk_file line
$read/end=BAD_FILE mar_file line
$write typchk_file line
$if .not. alpha then regs=" R0 R1 R2 R3 R4 R5 R6 R7 R8 R9 R10 R11 R12 R13 R14 R15 AP FP SP PC "
$if alpha then regs=" R0 R1 R2 R3 R4 R5 R6 R7 R8 R9 R10 R11 R12 R13 R14 R15 AP FP SP PC R16 R17 R18 R19 R20 R21 R22 R23 R24 R25 R26 R27 R28 R29 R30 R31 "
$if alpha then soft_64=0
$if alpha then hard_64=1
$if .not. alpha then soft_64=0
$if .not. alpha then hard_64=0
$reg_len=f$len(regs)
$line_num="          "
$line1=""
$line_pos=0
$last_opc=""
$point_opc=0
$ass1=""
$ass2=""
$ass3=""
$ass4=""
$ass5=""
$return
$SET_VAX:alpha=0
$src_p=48
$lnum_p=42
$lnum_s=5
$metab_p=40
$flg_p=30
$err_p=50
$err_off=56
$opc_p=31
$return
$SET_ALPHA:alpha=1
$ii=f$extr(f$loc(" AMAC ",line)+6,f$len(line),line)
$if f$extr(0,1,ii).nes."X" then goto NEW_FMT
$ii=f$ele(0," ",f$extr(1,f$len(ii),ii))
$if f$ele(0,".",ii) .gt. 0 then goto NEW_FMT
$ii=f$ele(1,".",ii)
$if f$ele(0,"-",ii) .lt. 9 then goto OLD_FMT
$if f$ele(0,"-",ii) .gt. 9 then goto NEW_FMT
$ii=f$ele(1,"-",ii)
$if ii .ge. 9 then goto NEW_FMT
$OLD_FMT:src_p=48
$lnum_p=41
$lnum_s=6
$metab_p=40
$flg_p=30
$err_p=50
$err_off=56
$mcl_p=32
$return
$NEW_FMT:src_p=39
$lnum_p=31
$lnum_s=8
$metab_p=31
$flg_p=20
$err_p=41
$err_off=48
$mcl_p=32
$return
$! Applied ECOs: 1
