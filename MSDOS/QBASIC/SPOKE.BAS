
'     * ***************************************************** *
'     * *                                                   * *
'     * *      Spoke.bas 1.0, by Roger Marquis, 3/89        * *
'     * *    Thanks to Rik & Jon Hjertberg @ Wheelsmith     * *
'     * *       Runs under Microsoft QuickBasic (C)         * *
'     * *                                                   * *
'     * *           Public  Domain,   Freeware.             * *
'     * *                                                   * *
'     * ***************************************************** *

BEGIN:
 rimdiam = 0         ' rim diameter
 HUBWID = 0          ' hub width
 HUBDIAM = 0         ' hub diameter
 NCROSS$ = ""        ' number of spoke crosses, input
 NCROSS = 0          ' number of spoke crosses, output
 NSPKS = 0           ' number of spokes
 'Q and Q$           ' misc input variable

'*************************************************************************

 CLS : LOCATE 3, 1
 PRINT "           Input rim diameter, spoke tip to opposite tip in mm"
 PRINT "               or RETURN for a list of common rim diameters"
 LOCATE 10, 5
 INPUT ; rimdiam
 WHILE rimdiam < 99
     IF rimdiam < 99 THEN GOSUB RIMMENU
     PRINT : PRINT "? ", : Q$ = INPUT$(1): Q = VAL(Q$)
     ON Q GOSUB RIMCHART1, RIMCHART2, RIMCHART3, RIMCHART4, RIMCHART5
     PRINT : PRINT : PRINT "DIAMETER (or ENTER for menu)  ";
     INPUT ; rimdiam
 WEND

 '************************************************************************

 CLS : LOCATE 2, 1
 PRINT "          Input effective hub diameter in mm"
 PRINT "          measured from center of spoke hole, across"
 PRINT "          axle, to opposite spoke hole"
 PRINT
 PRINT "          And the width of the hub in mm"
 PRINT "          measured from flange to opposite flange"
 PRINT
 PRINT "          or RETURN for a list of common hub dimensions"
 LOCATE 12, 1
 WHILE HUBDIAM = 0 OR HUBDIAM > 99
     INPUT "DIAMETER"; HUBDIAM: IF HUBDIAM = 0 THEN GOSUB HUBCHART
 WEND
 PRINT
 WHILE HUBWID = 0 OR HUBWID > 99
     INPUT "WIDTH"; HUBWID:    IF HUBWID = 0 THEN GOSUB HUBCHART
 WEND

 '************************************************************************

 WHILE NCROSS$ < "0" OR NCROSS > 9
      CLS : LOCATE 2, 22: PRINT "Input cross pattern:"
      LOCATE 9, 6: INPUT ; NCROSS$: NCROSS = VAL(NCROSS$)
 WEND
                      
 '************************************************************************

 WHILE NSPKS < 10 OR NSPKS > 99
     CLS : LOCATE 3, 20: PRINT "Input number of spokes:"
     LOCATE 8, 6: INPUT NSPKS
 WEND

 '************************************************************************

                      Y = 720 * (NCROSS / NSPKS)
              A = (HUBDIAM / 2) * SIN(Y * (3.1415 / 180))
      B = (rimdiam / 2) - (HUBDIAM / 2) * COS(Y * (3.1415 / 180))
                            C = HUBWID / 2
                LENGTH% = SQR(A ^ 2 + B ^ 2 + C ^ 2) - .5
 
 '************************************************************************
 
 CLS : LOCATE 2, 23
 PRINT "RIM SIZE =    "; rimdiam: PRINT
 PRINT "       CROSS    =   "; NCROSS, "           HUB DIAMETER = "; HUBDIAM
 PRINT "       SPOKES   =   "; NSPKS, "           HUB WIDTH =    "; HUBWID
 PRINT : PRINT
 PRINT "                         Spoke length is :"
 PRINT : PRINT
 PRINT "                       ", LENGTH%
 PRINT : PRINT
 PRINT "    Add and subtract 1mm for each side of a dished 6/7 spd. rear wheel."
 PRINT
 PRINT "              NOTE: For non-Wheelsmith (DT) spokes or spokes"
 PRINT "                heavier than 1.8mm (15):   add 1 to 1.5 mm"
 PRINT "                (depending on resaler measurement method)."
 PRINT : PRINT : PRINT
 PRINT "                    RUN AGAIN  ?  (enter N to quit)"

 Q$ = INPUT$(1)
 IF Q$ = "N" OR Q$ = "n" THEN END ELSE GOTO BEGIN

 '*************************************************************************
 '           Menu  screens:
 '*************************************************************************
RIMMENU:
 CLS : LOCATE 5, 1
 PRINT "          ENTER :": PRINT
 PRINT "                   1   for   700c   clincher   rims": PRINT
 PRINT "                   2   for   27in   clincher   rims": PRINT
 PRINT "                   3   for   26in   clincher   rims": PRINT
 PRINT "                   4   for   24/20in   rims": PRINT
 PRINT "             or    5   for   27/26in   sew-up   rims":
 RETURN

'***************************************************************************

RIMCHART1: CLS
 PRINT "                              700c   rims :"
 PRINT
 PRINT "  Ambrosio        Aero            605 |  Nisi            Mixer           612"
 PRINT "                  Elite           614 |  Rigida          1320            610"
 PRINT "  Araya           1W              605 |                  Score           612"
 PRINT "                  CT19            611 |                  HLC2000         606"
 PRINT "                  CTL730          609 |  Specialized     HC19            613"
 PRINT "  Campagnolo      V-profil        597 |                  HC20            614"
 PRINT "                  Std.            613 |  Sun/Mistral     M132            610"
 PRINT "  Galli           Aero            609 |                  M17             612"
 PRINT "  Matrix          Iso-C           597 |  Weinman         Concave         617"
 PRINT "  Mavic           E2 / G40        613 |  Wolber          58 / Gtlmn      613"
 PRINT "                  MA2 / MA40      611 |                  Alpine          612"
 PRINT "                  M3CD            613 |                  TX              602"
 PRINT "                  OPEN4CD         605 |"
 PRINT
 RETURN

'*****************************************************************************

RIMCHART2: CLS : LOCATE 3, 1
 PRINT "                           27in   Clinchers :"
 PRINT
 PRINT "                    Araya           1W              617"
 PRINT "                    Matrix          Iso-C           609"
 PRINT "                                    Titan           621"
 PRINT "                    Mavic           MA2 / MA2       618"
 PRINT "                                    E2 / G40        620"
 PRINT "                    Rigida          1320            617"
 PRINT "                                    steel 1-1/4     625"
 PRINT "                    Sun/Mistral     M17             619"
 PRINT "                    Ukai            16A(2)          624"
 PRINT "                    Weinmann        concave         624"
 PRINT "                    Wolber          58 / Gtlmn      620"
 PRINT
 RETURN

'***************************************************************************

RIMCHART3: CLS : LOCATE 3, 1
 PRINT "                           26in   Clinchers :"
 PRINT
 PRINT "                    Araya           RM20 / RM25     547"
 PRINT "                    Mavic           Oxygen          541"
 PRINT "                                    Bontrager MA40  546"
 PRINT "                    Rigida          steel 1-3/8     581"
 PRINT "                    Ritchey         Vantage         544"
 PRINT "                    Specialized     X22             554"
 PRINT "                                    X26             547"
 PRINT "                                    X28             543"
 PRINT "                                    X32             549"
 PRINT
 RETURN

 '***************************************************************************

RIMCHART4: CLS
 PRINT "                             24in   Rims :": PRINT
 PRINT "                    SEW-UP:"
 PRINT "                    Ambrosio        aero            505"
 PRINT "                    Araya           ADX4            506"
 PRINT "                    Assos           16mm            505"
 PRINT "                    Fiamme          red             515"
 PRINT "                    Mavic           CX18            517"
 PRINT "                                    Mach2           503"
 PRINT "                    Nisi            Laser           506"
 PRINT
 PRINT "                             20in   Rims":
 PRINT
 PRINT "                    SEW-UP:"
 PRINT "                    Araya           aero            400"
 PRINT
 RETURN

'****************************************************************************

RIMCHART5: CLS
 PRINT "                           26/27in   Sew-up   Rims :"
 PRINT
 PRINT "      Ambrosio   Aero        604  |   Matrix     Iso         610"
 PRINT "                 20          617  |   Mavic      GP/GL/GEL   615"
 PRINT "                 Synthesis   617  |              Mach 2      602"
 PRINT "                 Crono       617  |   Mistral    M19A        602"
 PRINT "                 std.        616  |   Nisi       Laser       615"
 PRINT "      Araya      ADX-4       611  |   Saavedra   aero        608"
 PRINT "                 (26in)ADX4  558  |              (26in)aero  552"
 PRINT "      Assos      16mm        616  |   Sun        M17         612"
 PRINT "                 18mm        625  |              M19         603"
 PRINT "                 (26in)16mm  558  |   Wolber     Aspin       614"
 PRINT "      Campagnolo all         616  |              Profil-18   611"
 PRINT "      Fiamme     Red         614  |              Profil-20   600"
 PRINT : PRINT : PRINT
 RETURN
                                                                           
'*******************************************************************************

HUBCHART: CLS
 PRINT "                              Flange   DIAMETERS /  WIDTHS :"
 PRINT
 PRINT "     Campagnolo      LF      Rear            44       57"
 PRINT "                             Front           39       70"
 PRINT "                     HF      Rear            67       57"
 PRINT "                             Front           67       68"
 PRINT "            track    HF      Rear            67       75"
 PRINT "                             Front           67       64"
 PRINT "     Shimano         LF      Rear cassette   44       55"
 PRINT "                             Rear standard   44       56"
 PRINT "                             Front           38       71"
 PRINT "     Specialized     MF      Rear            46       51"
 PRINT "                             Front           43       66"
 PRINT "     Mavic           LF      Rear            44       56"
 PRINT "                             Front           40       56"
 PRINT "     Suntour         LF      Rear            44       55"
 PRINT "                             Front           38       66"
 PRINT "             track   HF      Rear            67       66"
 PRINT "                             Front           67       64"
 PRINT
 RETURN

'********************************************< end of program >***********
