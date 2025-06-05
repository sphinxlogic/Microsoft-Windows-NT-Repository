DVDRTOOLS, UTILITIES, DVD-R/RW Recording Tools for OpenVMS

This is an early port of DVDRTOOLS-0_1_5.

The core CDRECORD engine is known to function with local CD-R drives,
but a (local) lack of DVD-R and DVD-RW drives and a lack of support 
within this DVDRTOOLS-0_1_5 for the available local DVD+R and DVD+RW
drives has meant that none of the DVD code has been tested.  

Again, CD-R operations have been tried and do function with Plextor
PlexWriter SCSI drives and do with CD-R media loaded in HP DVD200 
series DVD drives.

This version of MKISOFS is known not to function.  (Problems within
the assumptions made around the readdir and related calls being one
reported area of incompatibility.)

Other components of DVDRTOOLS here have not been tested.

