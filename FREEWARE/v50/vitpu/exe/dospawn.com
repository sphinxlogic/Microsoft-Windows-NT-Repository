$ define/user sys$input sys$command
$ 'p1'
$ write sys$output " "
$ READ/PROMPT="[Hit ENTER to continue]" sys$command line
$ EXIT
