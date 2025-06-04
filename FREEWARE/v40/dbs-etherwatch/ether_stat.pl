#  Program Name            : ETHER_STAT.PL
#    Original Author       : J. LAURET
#  			     SUNY @ Stony Brook
#  			     Stony Brook NY 11790-3400
#
#    Date                  : 12-APR-1996
#    Program Description   : Uses ETHERWATCH with /COUNT and display network
#                          : statistics. Instaneous or Cumulative mode
#  			   currently avaialable as well as a sorting by number
#  			   of packages or by bytes.
#    Require : 	ETHERWATCH to be defined as a symbol or installed.
#   		lexical.pm  available at
#			http://nuchm2.chem.sunysb.edu/pub/vms/lexical.html
#  ----------------------- :
#    Date of Revision      : 17-APR-1996
#    Change Author         : NUCMGR
#    Purpose of Revision   :  Added the TYPES reading from a file pointed by
#                          : logical ETHER_TYPES
#    Format of the TYPES file:
#		Comment are allowed using "!"
#		Each line should contain the TYPE (as XX-XX or XXXX) and a
#  		string equivalent
#  		Ex:
#  			60-01 MOPDUMP
#  			6002  MODCONSOLE	! This is valid as well


use lexical								;

	# usage is printed if no argument is provided
	$version = "V01-100"						;
	print "ETHER_STAT $version\n"					;
	if( $#ARGV >= 0){
		if ($ENV{ETHERWATCHER} eq ""){
			print "You may define ETHERWATCHER to point on a NODELIST file.\n";
			print "See documentation for detail.\n"	        ;
		}

		# Some initialization

		# A list of possible ethernet types you will encounter
		# Use default list
		$pad = " -----------------------"			;
		$type{"60-01"} = "MOPDUMP"				;
		$type{"60-02"} = "MOPCONSOLE"				;
		$type{"60-03"} = "DECNET"				;
		$type{"60-04"} = "LAT"					;
		$type{"60-05"} = "DIAGNOSTICS"				;
		$type{"60-07"} = "LAVC"				        ;
		$type{"80-38"} = "RBMS"				        ;
		$type{"80-3B"} = "VAXELN"				;
		$type{"80-3E"} = "DNS"					;
		$type{"80-40"} = "NETBIOS"				;
		$type{"80-41"} = "LAST"				        ;

		$sttime	= f_time()					;
		$i	= 0						;
		$sum 	= 0						;
		$loop	= 0						;
		$mode	= 2						;


		# Alternative : You may define the logical ETHER_TYPES
		# to point on a file containing
		#	type	string
		# i.e.:  60-01 MOPDUMP
		#        6001  MOPDUMP	! is also a valid format
		$file = $ENV{ETHER_TYPES}				;
		if ($file ne ""){
		  # Open the TYPES file and suck-it in
		  if (open(FI,"$file") ){
			$pad = "  --------------------------------------------------";
			print "Reading $file\n"			        ;
			@TYPES = <FI>					;
			close(FI)					;
			foreach $el (@TYPES){
				$el = f_edit($el,"UNCOMMENT,COMPRESS,TRIM");
				if($el ne ""){
				 $tmp = f_element(0," ",$el)		;
				 $len = length($tmp)			;
				 # Fix it as XX-XX
				 if($len = 4){
					$tmp = 	substr($tmp,0,2)."-".
						substr($tmp,2,2)	;
				 }
				 if(length($tmp) > 5){
					print "Ignore $el - Invalid type\n";
				 } else {
				  	$what= substr($el,5,length($el)-5);
					$type{$tmp} = $what		;
				 }
				}
			}
		  } else {
			# TYPES will still be printed if defined in the
			# default list.
			print "Could not open TYPES file $file\n"	;
		  }
		}

		# Now get some data (first pass)
		$val = $ARGV[0]*1					;
		if($val == 0){
		  print "Reading data     ...\n"			;
		  open(FI,$ARGV[0]) || die "Could not open file $ARGV[0]\n";
		  @STAT = <FI>						;
		  close(FI)						;
		} else {
		  @STAT = &getdata($val)				;
		  if( $#ARGV >= 1){
			$loop = substr(uc($ARGV[1]),0,1)		;
			$mode = length($ARGV[1])			;
		  }
		  if( $#ARGV == 2){
			$elaps = $ARGV[2]				;
		  } else {
			$elaps = 10					;
		  }
		}


	} else {
		&usage							;
	}







 	STATIS:
	 print chr(27),"[H",chr(27),"[2J"				;
	 foreach $line (@STAT){
		die "$line\n" if( substr($line,0,1) eq "%" )		;
		if( uc(f_element(0," ",$line)) eq "FROM"){
			$ether = f_element(1," ",$line)		        ;
			$node  = f_element(2," ",$line)		        ;
		}
		if( uc(f_element(0," ",$line)) eq "PROTOCOL"){
			$line  = f_edit($line,"COMPRESS")		;
			$proto = f_element(1," ",$line)		        ;
			if( length($proto) == 5){
			 $bytes = f_element(4," ",$line)		;
			 $i++						;
			 if (!defined($A_NODE{$ether})){
				$A_NODE{$ether} = $node		        ;
			 }

			 # How many times we've seen it
			 $ETHER_CNT{$ether} += 1			;
			 # Sum of the bytes transferred
			 $ETHER_BYT{$ether} += $bytes			;
			 $sum += $bytes				        ;
			 # How many times this protocol is used
			 $ETHER_PRT{$proto} += 1			;
			}
		}
	 }

	 print "Statistics started at $sttime\n" if ($mode == 1)	;
	 print "$i counts -- "						;
	 print "Time is : ",f_time(),"\n"				;
	 print "Cumulative mode   " if ($mode == 1)			;
	 print "Instantaneous mode" if ($mode == 2)			;
	 if($loop eq "T"){
	 print " - Frequency sort\n\n"					;
	 } else {
	 print " - Bytes transferred sort\n\n"				;
	 }


	 printf " %60s\t%5s\n","Protocol Type","%tage"			;
	 printf "%70.70s\n",$pad					;
	 foreach $proto
		(sort{$ETHER_PRT{$b} <=> $ETHER_PRT{$a}}(keys %ETHER_PRT)){
		if( defined($type{$proto}) ){
			$str = $type{$proto}				;
		} else {
			$str = "[".$proto."]"				;
		}
		$prct1 = $ETHER_PRT{$proto}/$i*100			;
		printf " %60s\t%5.2f%%\n",$str,$prct1			;
	 }
	 printf "%70.70s\n\n",$pad					;


	 printf " %17.17s %15s \t%5s \t%5s \n","Hyphen","Node","% Bytes","% Frequency";
	 print  " ======================================================================\n";
	 if($loop eq "T"){
	   foreach $ether
		(sort{$ETHER_CNT{$b} <=> $ETHER_CNT{$a}} (keys %ETHER_CNT)){
		$prct2 = $ETHER_CNT{$ether}*100.0/$i			;
		if( $prct2 > 0.2){
		 $prct1 = $ETHER_BYT{$ether}*100.0/$sum		        ;
		 $node  = $A_NODE{$ether}				;
		 printf " %17s %15s \t%5.2f \t\t%5.2f \n"		,
			$ether,$node,$prct1,$prct2			;
		}
	   }
	 } else {
	   foreach $ether
		(sort{$ETHER_BYT{$b} <=> $ETHER_BYT{$a}} (keys %ETHER_BYT)){
		$prct1 = $ETHER_BYT{$ether}*100.0/$sum			;
		if( $prct1 > 0.2){
		 $prct2 = $ETHER_CNT{$ether}*100.0/$i			;
		 $node  = $A_NODE{$ether}				;
		 printf " %17s %15s \t%5.2f \t\t%5.2f \n"		,
			$ether,$node,$prct1,$prct2			;
		}
	   }
	 }



	# Case loop is asked
	if(($loop eq "T") || ($loop eq "B")){
		 if($mode == 2){
			$i	= 0					;
			$sum 	= 0					;
			undef(%ETHER_CNT)				;
			undef(%ETHER_BYT)				;
			undef(%ETHER_PRT)				;
		 }
		 undef(@STAT)						;
		 print "\n\n"						;
		 print "Waiting $elaps seconds \n"			;
		 sleep($elaps)						;
		 @STAT = &getdata($val)				        ;
		 goto STATIS						;
	} else {
		print "\n\n"						;
	}





#  -----------------------------------------------------------------------


sub usage
{
print " ----------------------------------------------------------------------\n";
print "  Usage is \n"							;
print "     ETHER_STAT P1 [P2 P3]\n"					;
print "        P1 can be \n"						;
print "           a FileName from which the data will be read or\n"	;
print "           a number of counts to process\n"			;
print "        P2 is used only if P1 is a number of counts. Value : \n" ;
print "           T  Sort by number of package \n"			;
print "           B  Sort by bytes transferred\n"			;
print "           T* Same as T but instantaneous instead of cumulative\n";
print "           B* instantaneous B mode \n"				;
print "        P3 A time to wait between loops\n"			;
print "  ----------------------------------------------------------------------\n";
exit									;
}


sub getdata
{
	my($cnt) = pop(@_)						;
	local(@A)							;
	print "Getting data     ...\n"					;
	`etherwatch/display=none/count=$cnt`				;
}

