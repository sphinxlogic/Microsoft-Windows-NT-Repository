:
:	An example of setting up a script for doublesided printing
:
:	usage:		psdouble file
:		where	  "file" is the file to be printed
:			  	 double sided, two-up.
:
:	The -d option for psf causes file "psfbook.psd" to be created
:	which is then processed by psfdoub to actually print double sided.
:
:
@psf -2d %1
@echo Side 1 is ready for printing.
@Pause
@psfdoub -1 psfbook.psd > prn
@echo Refeed the paper for the 2nd side.
@Pause
@psfdoub -2 psfbook.psd > prn
@del psfbook.psd
