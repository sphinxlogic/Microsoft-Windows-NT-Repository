$ !
$ ! CBPG Key Generation System V1.1 
$ !
$ !			    FOR INTERNAL USE ONLY
$ !
$ ! Badge: BLISS PAK
$ !  Name: BLISS PAK
$ !    CC: BLISS PAK
$ !
$ !		     Issuer: DEC
$ !    Authorization Number: CBPTMP015635
$ !	       Product Name: BLISS32
$ !		      Units: 0
$ !    Product Release Date: 
$ !	   Termination Date: 
$ !		    Version: 
$ ! Availability Table Code: 
$ !	Activity Table Code: CONSTANT=100
$ !	    License Options: 
$ !	      Product Token: 
$ !             Hardware Id: 
$ !		   Checksum: 2-CGEC-BEKM-PFJD-PKLJ
$ !
$ ! REGISTER the license
$ !
$ LICENSE REGISTER "BLISS32" -
/ISSUER="DEC" -
/AUTHORIZATION=CBPTMP015635 -
/PRODUCER=DEC -
/UNITS=0 -
/ACTIVITY=CONSTANT=100 -
/CHECKSUM=2-CGEC-BEKM-PFJD-PKLJ 
$ LICENSE LOAD BLISS32
