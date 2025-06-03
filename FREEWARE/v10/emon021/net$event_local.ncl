!+	les lignes qui suivent sont indispensables pour EMON
!	the following lines are necessary for EMON
block event dispatcher outbound stream * global filter -
	((node, csma-cd, station), unavailable user buffer)
block event dispatcher outbound stream * global filter -
	((node, csma-cd, station), unavailable station buffer)
block event dispatcher outbound stream * global filter -
	((node, csma-cd, station), unrecognized individual destination PDU)
!-
