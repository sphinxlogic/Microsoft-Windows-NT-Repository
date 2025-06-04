Password_Policy, System_Management, A Password Policy Module that more strickly controls passwords on the system.

This a VMS Password Policy Module to screen passwords before they are set.

This code will only work with VMS 5.4 or greater.

This module does not allow the password to be the following:

	The username and variations on such
	The account name from the UAF and variations on such.
	Variations of the owner field from the UAF
	Any word in a data file supplemental to the VMS provided dictionary.

To build and install this module, execute the command procedure
BUILD_PASSWORD_POLICY.COM.  Afterwards, make sure you add the following line:

	$ INSTALL ADD SYS$LIBRARY:VMS$PASSWORD_POLICY.EXE

to your SYS$MANAGER:SYSTARTUP_VMS.COM file.

An auxilary program is ADD_DICTIONARY which will allow you to add words
to the supplemental dictionary.  It will add words that are not already in
the VMS dictionary or the supplementary dictionary.

If you have a large number of words you would like to add to the supplemental 
dictionary, put them into a flat data file, one word to a line, ASSIGN the 
file to SYS$INPUT: and then run the ADD_DICTIONARY program.  It will read all 
the words from the file and add them, if needed to the supplemental 
dictionary.  The ADD_DICTIONARY program can also be run interactively.

(The supplement dictionary is used rather than adding words to the VMS 
provided dictionary to prevent those changes from being lost in future 
upgrades/updates of the operating system.)


Ted Nieland
nieland@hcst.com

HCST, Inc
513-427-9000
