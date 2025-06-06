" VIM syntax file
" Language:	Nroff/Troff
" Maintainer:	Matthias Burian <burian@grabner-instruments.com>
" Last Change:	Aug. 09, 1999

syn clear

syn match nroffCommand "^\.[a-zA-Z]" nextgroup=nroffCmdArg
syn match nroffCommand "^\.[a-zA-Z][a-zA-Z0-9\\]\+" nextgroup=nroffCmdArg

syn match nroffCmdArg contained ".*" contains=nroffString,nroffComArg
syn region nroffString contained start=/"/ end=/"/ contains=nroffFont
syn region nroffString contained start=/'/ end=/'/ contains=nroffFont
syn match nroffComArg +\\["#].*+
syn match nroffComment +^\.\\".*+

syn region nroffFont start="\\f[A-Z]"hs=s+3 end="\\f[A-Z]"he=e-3 end="$"
syn region nroffFont start="\\\*<"hs=s+3 end="\\\*>"he=e-3
syn region nroffDefine start="\.ds\ [A-Za-z_]\+" end="$" contains=ALL
syn region nroffSize start="\\s[0-9]*" end="\\s[0-9]*"
syn region nroffSpecial start="^\.[TP]S$" end="^\.[TP]E$"
syn region nroffSpecial start="^\.EQ$" end="^\.EN$"

if !exists("did_nroff_syntax_inits")
  let did_nroff_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link nroffCommand			Statement
  hi link nroffComment			Comment
  hi link nroffComArg			Comment
  hi link nroffFont			PreProc
  hi link nroffSize			PreProc
  hi link nroffDefine			String
  hi link nroffString			String
  hi link nroffSpecial			Question
endif

let b:current_syntax = "nroff"

" vim: ts=8
