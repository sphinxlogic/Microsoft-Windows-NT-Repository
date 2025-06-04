C++
C
C 	SHSDEF.FOR  - Define data structures & parameters for SHS
C 
C
C AUTHOR:  James Shen (jshen1@ford.com)
C          Powertrain Operation
C          Ford Motor Company
C          1981 Front Wheel Drive
C          Batavia, OH 45103
C
C CREATION DATE: Sept, 1995
C
C MODIFIED BY:
C
C        : VERSION
C 01    -
C
C--

C
C Itm - PROCESS_SCAN item list entry
C
	STRUCTURE /LNMITMLIST/
           UNION
              MAP
           	 INTEGER*2    BUFLEN, CODE
	         INTEGER*4    BUFADR, RETLENADR
              END MAP
              MAP
           	 INTEGER*4    END_LIST
              END MAP
           END UNION
        END STRUCTURE

C
C Itm - PROCESS_SCAN item list entry
C
	STRUCTURE /PSCANITMLIST/
           UNION
              MAP
                 INTEGER*2    BUFLEN, CODE
                 INTEGER*4    BUFADR, ITMFLAGS
              END MAP
              MAP
                 INTEGER*4    END_LIST
              END MAP
           END UNION
        END STRUCTURE

C
C Itm - GETJPI item list entry
C
        STRUCTURE /JPIITMLIST/
           UNION
              MAP
           	 INTEGER*2    BUFLEN, CODE
                 INTEGER*4    BUFADR, RETLENADR
              END MAP
              MAP
                 INTEGER*4    END_LIST
              END MAP
           END UNION
        END STRUCTURE

C
C Itm - GETSYI item list entry
C
        STRUCTURE /SYIITMLIST/
           UNION
              MAP
           	 INTEGER*2    BUFLEN, CODE
           	 INTEGER*4    BUFADR, RETLENADR
              END MAP
              MAP
           	 INTEGER*4    END_LIST
              END MAP
           END UNION
        END STRUCTURE

C
C Itm - GETDVI item list entry
C
        STRUCTURE /DVIITMLIST/
           UNION
              MAP
                 INTEGER*2    BUFLEN, CODE
           	 INTEGER*4    BUFADR, RETLENADR
              END MAP
              MAP
           	 INTEGER*4    END_LIST
              END MAP
           END UNION
        END STRUCTURE

C
C Sense/Set Mode I/O status block
C
	STRUCTURE /IOSB_STAT/
	   INTEGER*2	STATUS
	   BYTE		TRANSMIT,
	2		RECEIVE,
	2		CRFIL,
	2		LFFIL,
	2		PARITY,
	2		ZERO
	END STRUCTURE

C
C Termianl Basic Characteristics Buffer 
C
	STRUCTURE /CHARACTERISTICS/
	   BYTE		TYPE,
	2		CLASS
	   INTEGER*2	WIDTH
	   UNION
	      MAP
	   	 INTEGER*4	BASIC
	      END MAP
	      MAP
	         BYTE		LENGTH(4)
	      END MAP
	   END UNION
	END STRUCTURE

C
C Itm - Generic I/O status block
C
	STRUCTURE /IOSB/
           UNION
              MAP
           	 INTEGER*2    STATUS, COUNT
              END MAP
              MAP
           	 INTEGER*4    %FILL
              END MAP
           END UNION
        END STRUCTURE

C
C Symbols for SORY_BY keywords
C
        INTEGER*4       SHS_NOKEY
        PARAMETER       ( SHS_NOKEY   	= 0)
        INTEGER*4       SHS_PROCNAME
        PARAMETER       ( SHS_PROCNAME  = 1)
        INTEGER*4       SHS_CPU
        PARAMETER       ( SHS_CPU       = 2)
        INTEGER*4       SHS_IO
        PARAMETER       ( SHS_IO        = 3)
        INTEGER*4       SHS_CURPRIO
        PARAMETER       ( SHS_CURPRIO   = 4)
        INTEGER*4       SHS_PAGEFAULT
        PARAMETER       ( SHS_PAGEFAULT = 5)
        INTEGER*4       SHS_PHYSICAL
        PARAMETER       ( SHS_PHYSICAL  = 6)

C
C Symbols for Sort Order
C
        INTEGER*4       SHS_ASCENDING
        PARAMETER       ( SHS_ASCENDING = 0)
        INTEGER*4       SHS_DESCENDING
        PARAMETER       ( SHS_DESCENDING= 1)
