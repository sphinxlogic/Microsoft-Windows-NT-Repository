proc loopSel {func {args {}}} {
    set selection [getSelection]
    set start [lindex $selection 0]
    set end   [lindex $selection 1]
    set lines [lindex $selection 2]

    if {$start == $end} {return}

    setPos $start
    for {set line 0} {$line < $lines} {incr line} {
	action beginning-of-line
	eval $func $args
	action next-line
    }
    action beginning-of-line
}

proc prefix {{text {> }}} {
    loopSel action insert-string $text
}

proc stripn {{n 1}} {
    action multiply $n
    action delete-next-character
}

proc strip {{n 1}} {
    loopSel stripn $n
}

proc join {{n 1} {insert " "}} {
    for {set line 0} {$line < $n} {incr line} {
	action end-of-line
	action delete-next-character
	action insert-string $insert
    }
}

proc format {} {
    set caret [getPos]
    action beginning-of-file
    for {set lastpos -1; set curpos 0} \
            {$curpos != $lastpos} \
                {set lastpos $curpos; set curpos [getPos]} { 
	action forward-paragraph
	action form-paragraph
	action next-line
    }
    setPos $caret
    action redraw-display
}
