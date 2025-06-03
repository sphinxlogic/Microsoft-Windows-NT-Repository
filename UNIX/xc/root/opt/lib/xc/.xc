# .xc -- startup script
set bps 57600
set cis off
set proto 8N1
set cr on
set nl on
set xoff on
set hdplx off
set menu on
set auto on
set XCAPE 

#assign modem eq portname
# setting different default speeds for assorted modems
# note that the following means: if ANY of the three conditions are met
if modem eq "/dev/ttyA01"; modem eq "/dev/ttyA02"; modem eq "/dev/ttyA03" ; modem eq "/dev/ttyA08"
then
	set bps 38400
else
	set bps 2400
endif

#####
# Dynamic keyboard binding feature
#####

# Set terminal mode escape character ("XCAPE") to Control-Z:
# set escape ^Z

# Bind XCAPE-0 to set Telebit T2500 to not require MNP:
bind_string "AAAAT S95=0^M" 48

# Bind XCAPE-1 to set Telebit T2500 to attempt LAP/M and then MNP:
bind_string 49 "AAAAT S95=1 S96=1 S97=1 S98=3 S106=0^M"

# Bind XCAPE-^S to receive a file using sz (.sz included in distribution)
bind_script 19 "/usr/lib/xc/.sz"

# Bind XCAPE-^R to receive a file using rz (.rz included in distribution)
bind_script 18 "/usr/lib/xc/.rz"

# Bind XCAPE-^Z to exit out of XC (a la vi).  Note that this is dangerous
# because you no longer have a way of emitting a XCAPE through the modem,
# except by embedding the command into a "bind_string" or "bind_script".
# It's still a neat exit command...

bind_function 26 "quitchr"
