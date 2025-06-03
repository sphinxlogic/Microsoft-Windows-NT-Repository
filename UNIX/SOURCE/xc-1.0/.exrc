" the leading '"' makes this line a comment
" uncomment lines by removing the leading '"'
"
" tab settings for all XC source files 
set ts=4 sw=4
"
" macros for handling CompuServe downloads
" make sure the ^M below are true Ctrl-M
" make sure the ^[ below are true ESCAPE
"
" set wm=1 
"
" F5 squeezes out empty lines, marks new setion with 'm'
" map	#5	/^welcome to /
"
" F2 deletes from current line to mark 'm', and leaves a new mark 'm'
" map	#2	d`mSmmz
"
" with cursor within a message, F1 sets up for a reply
" map	#1	?^#: \([1-9][0-9]*\) .*S[0-9]*/.*?
" map		P:s;;re\1;
"
" ^X takes a reply prepared after an F1, and appends it to an upload file
" map		'aO:'a,'b w>>%R
"
" F8 places next message at top of screen
" map	#8	/#:/z
