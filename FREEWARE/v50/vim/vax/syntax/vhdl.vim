" Vim syntax file
" Language:	VHDL
" Maintainer:	Czo <Olivier.Sirol@lip6.fr>
" Credits:	Stephan Hegel <ea273@fen.baynet.de>
" $Id: vhdl.vim,v 1.8 1999/11/04 17:46:42 czo Exp $

" VHSIC Hardware Description Language
" Very High Scale Integrated Circuit

" Remove any old syntax stuff hanging around
syn clear

" This is not VHDL. I use the C-Preprocessor cpp to generate different binaries
" from one VHDL source file. Unfortunately there is no preprocessor for VHDL
" available. If you don't like this, please remove the following lines.
syn match cDefine "^#ifdef[ ]\+[A-Za-z_]\+"
syn match cDefine "^#endif"

" case is not significant
syn case ignore

" VHDL keywords
syn keyword vhdlStatement access after alias all assert
syn keyword vhdlStatement architecture array attribute
syn keyword vhdlStatement begin block body buffer bus
syn keyword vhdlStatement case component configuration constant
syn keyword vhdlStatement disconnect downto
syn keyword vhdlStatement else elsif end entity exit
syn keyword vhdlStatement file for function
syn keyword vhdlStatement generate generic group guarded
syn keyword vhdlStatement if impure in inertial inout is
syn keyword vhdlStatement label library linkage literal loop
syn keyword vhdlStatement map
syn keyword vhdlStatement new next null
syn keyword vhdlStatement of on open others out
syn keyword vhdlStatement package port postponed procedure process pure
syn keyword vhdlStatement range record register reject report return
syn keyword vhdlStatement select severity signal shared
syn keyword vhdlStatement subtype
syn keyword vhdlStatement then to transport type
syn keyword vhdlStatement unaffected units until use
syn keyword vhdlStatement variable wait when while with
syn keyword vhdlStatement note warning error failure

" Predifined VHDL types
syn keyword vhdlType bit bit_vector
syn keyword vhdlType character boolean integer real time
syn keyword vhdlType string severity_level
" Predifined standard ieee VHDL types
syn keyword vhdlType positive natural signed unsigned
syn keyword vhdlType line text
syn keyword vhdlType std_logic std_logic_vector
syn keyword vhdlType std_ulogic std_ulogic_vector
" Predefined non standard VHDL types for Mentor Graphics Sys1076/QuickHDL
syn keyword vhdlType qsim_state qsim_state_vector
syn keyword vhdlType qsim_12state qsim_12state_vector
syn keyword vhdlType qsim_strength
" Predefined non standard VHDL types for Alliance VLSI CAD
syn keyword vhdlType mux_bit mux_vector reg_bit reg_vector wor_bit wor_vector

" array attributes
syn match vhdlAttribute "\'high"
syn match vhdlAttribute "\'left"
syn match vhdlAttribute "\'length"
syn match vhdlAttribute "\'low"
syn match vhdlAttribute "\'range"
syn match vhdlAttribute "\'reverse_range"
syn match vhdlAttribute "\'right"
syn match vhdlAttribute "\'ascending"
" block attributes
syn match vhdlAttribute "\'behaviour"
syn match vhdlAttribute "\'structure"
syn match vhdlAttribute "\'simple_name"
syn match vhdlAttribute "\'instance_name"
syn match vhdlAttribute "\'path_name"
syn match vhdlAttribute "\'foreign"
" signal attribute
syn match vhdlAttribute "\'active"
syn match vhdlAttribute "\'delayed"
syn match vhdlAttribute "\'event"
syn match vhdlAttribute "\'last_active"
syn match vhdlAttribute "\'last_event"
syn match vhdlAttribute "\'last_value"
syn match vhdlAttribute "\'quiet"
syn match vhdlAttribute "\'stable"
syn match vhdlAttribute "\'transaction"
syn match vhdlAttribute "\'driving"
syn match vhdlAttribute "\'driving_value"
" type attributes
syn match vhdlAttribute "\'base"
syn match vhdlAttribute "\'high"
syn match vhdlAttribute "\'left"
syn match vhdlAttribute "\'leftof"
syn match vhdlAttribute "\'low"
syn match vhdlAttribute "\'pos"
syn match vhdlAttribute "\'pred"
syn match vhdlAttribute "\'rightof"
syn match vhdlAttribute "\'succ"
syn match vhdlAttribute "\'val"
syn match vhdlAttribute "\'image"
syn match vhdlAttribute "\'value"

syn keyword vhdlBoolean true false

" for this vector values case is significant
syn case match
" Values for standard VHDL types
syn match vhdlVector "\'[0L1HXWZU\-\?]\'"
" Values for non standard VHDL types qsim_12state for Mentor Graphics Sys1076/QuickHDL
syn keyword vhdlVector S0S S1S SXS S0R S1R SXR S0Z S1Z SXZ S0I S1I SXI
syn case ignore

syn match  vhdlVector "B\"[01_]\+\""
syn match  vhdlVector "O\"[0-7_]\+\""
syn match  vhdlVector "X\"[0-9a-f_]\+\""
syn match  vhdlCharacter "'.'"
syn region vhdlString start=+"+  end=+"+

" floating numbers
syn match vhdlNumber "-\=\<\d\+\.\d\+\(E[+\-]\=\d\+\)\>"
syn match vhdlNumber "-\=\<\d\+\.\d\+\>"
syn match vhdlNumber "0*2#[01_]\+\.[01_]\+#\(E[+\-]\=\d\+\)\="
syn match vhdlNumber "0*16#[0-9a-f_]\+\.[0-9a-f_]\+#\(E[+\-]\=\d\+\)\="
" integer numbers
syn match vhdlNumber "-\=\<\d\+\(E[+\-]\=\d\+\)\>"
syn match vhdlNumber "-\=\<\d\+\>"
syn match vhdlNumber "0*2#[01_]\+#\(E[+\-]\=\d\+\)\="
syn match vhdlNumber "0*16#[0-9a-f_]\+#\(E[+\-]\=\d\+\)\="
" operators
syn keyword vhdlOperator and nand or nor xor xnor
syn keyword vhdlOperator rol ror sla sll sra srl
syn keyword vhdlOperator mod rem abs not
syn match   vhdlOperator "[&><=:+\-*\/|]"
syn match   vhdlSpecial  "[().,;]"
" time
syn match vhdlTime "\<\d\+\s\+\(\([fpnum]s\)\|\(sec\)\|\(min\)\|\(hr\)\)\>"
syn match vhdlTime "\<\d\+\.\d\+\s\+\(\([fpnum]s\)\|\(sec\)\|\(min\)\|\(hr\)\)\>"

syn match vhdlComment "--.*$"
" syn match vhdlGlobal "[\'$#~!%@?\^\[\]{}\\]"

if !exists("did_vhdl_syntax_inits")
  let did_vhdl_syntax_inits = 1
  " The default methods for highlighting. Can be overridden later
  hi link cDefine       PreProc
  hi link vhdlSpecial   Special
  hi link vhdlStatement Statement
  hi link vhdlCharacter String
  hi link vhdlString    String
  hi link vhdlVector    String
  hi link vhdlBoolean   String
  hi link vhdlComment   Comment
  hi link vhdlNumber    String
  hi link vhdlTime      String
  hi link vhdlType      Type
  hi link vhdlOperator  Type
  hi link vhdlGlobal    Error
  hi link vhdlAttribute Type
endif

let b:current_syntax = "vhdl"

" vim: ts=8
