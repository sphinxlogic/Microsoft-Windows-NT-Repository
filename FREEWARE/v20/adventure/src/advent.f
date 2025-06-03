C  Adventure - main program
C
	PROGRAM ADVENT
	IMPLICIT INTEGER (A-Z)
	INCLUDE 'aparam.for'
	DATA VMAJ/4/,VMIN/0/,VEDIT/'A'/
C
	CALL INIT				! initialize data structures
	CALL MAIN				! play game
	CALL EXIT				! done
	END
