
The only added complexity is that systems with no usleep() call
should compile Dclock.c with -DNO_USLEEP.

 -- Steve

Copyright (c) Dan Heller <argv@sun.com> 1988

To compile, just run make and watch it go.

When the program is running and the mouse is in the window, you can
type the following keys:
    
    r -- toggle reverse video
    s -- toggle seconds display
    b -- toggle the bell to ring on half hours
    j -- toggle jump or smooth scrolling
    d -- toggle date string display
    m -- toggle military (24hr) time format
    a -- toggle the alarm clock
    f -- toggle fade
    q -- quit

To set the alarm time, use the third mouse button and click on the
digits till the time is set correctly.  Alarm time is set in 24-hour
format to distringuish between am and pm.  If there is an asterisk
on the same line as the date, then the alarm is set.  Use the 'a' key
to set the alarm.

typical uses:

    # set alarm for noon
    dclock -fg green -gb black -alarmTime 12:00 -alarm

    # set date string and have bell go off on hour/half-hour intervals
    dclock -date "Today is %W, %M %d" -fg yellow -bg brown -bell

    # set date more simply, use 24-hour military time format and display secs
    dclock -date "%w, %m %d" -miltime -seconds
