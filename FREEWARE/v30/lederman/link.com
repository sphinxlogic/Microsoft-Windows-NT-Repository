$! LINK /MAP /CROSS 'P1 monitor_to_csv, monitor_cld, SYS$INPUT /OPTIONS
$ LINK /MAP /CROSS 'P1 monitor_to_csv, SYS$INPUT /OPTIONS
	IDENTIFICATION = "Monitory To CSV"
	CLUSTER = librtl,,, sys$share:librtl.exe /SHAREABLE
	CLUSTER = starlt,,, SYS$COMMON:[SYSLIB]STARLET.OLB /LIBRARY
$ EXIT
