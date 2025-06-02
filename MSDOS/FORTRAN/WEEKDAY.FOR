        PROGRAM TEST
        INTEGER DATE, MONTH, YEAR, DOW, DAY

        YEAR = 1989
        DO 10 MONTH = 1, 12, 1
        DO 10 DATE = 1, 31, 1
        DAY = DOW(DATE, MONTH, YEAR)
        WRITE(5,20)MONTH, DATE, YEAR, DAY
10      CONTINUE
20      FORMAT(1X, 4I)
        STOP
        END
C
C       DOW COMPUTES THE DAY OF THE WEEK BASED UPON THE GIVEN DATE,
C       MONTH AND YEAR.  IT USES THE ZELLER CONGRUENCE ALGORITHIM.
C       DATE IS THE DAY OF THE MONTH, 1 - 31
C       MONTH IS THE MONTH OF THE YEAR, 1 - 12
C       YEAR IS THE YEAR, E.G., 1989
C       IT RETURNS 1 FOR SUNDAY, 2 FOR MONDAY, ETC.
C
        INTEGER FUNCTION DOW(DATE, MONTH, YEAR)
        INTEGER DATE, MONTH, YEAR
        INTEGER DAY, YR, MN

        YR = YEAR
        MN = MONTH
C
C       IF JANUARY OR FEBRUARY, ADJUST MONTH AND YEAR
C
        IF (MN.GT.2)GO TO 10
        MN = MN + 12
        YR = YR - 1
10      N1 = (26 * (MN + 1)) / 10
        N2 = (125 * YR) / 100
        DAY = (DATE + N1 + N2 - (YR / 100) + (YR / 400) - 1)
        DOW = MOD(DAY, 7) + 1
        RETURN
        END
