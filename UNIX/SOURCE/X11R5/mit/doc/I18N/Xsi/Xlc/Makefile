ROFF	= troff
ROFFPR	= lpr -n
LPR	= lpr

SRC	= XCnvCTToMB.3 XCnvCTToWC.3 XCnvMBToCT.3 XCnvMBToWC.3 XCnvWCToCT.3 \
	  XCnvWCToMB.3 XOSLocale.3 XParseCT.3 setlocale.3
DOC	= CTWC.doc XlcFunct.doc Xlocale.doc

all:

print:
	$(ROFF) -man $(SRC) | $(ROFFPR)
	$(LPR) $(DOC)
