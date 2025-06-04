/* print TeX DVI file on the LN03 laser printer. The que is defined by the 
   symbol LN03QUE.
*/
babble := write sys$output

if( p1 .eqs. "?" ){
type sys$input
#   Usage:
#       laser file -nc -np -f s=n n=pcnt h=hoffset v=voffset p=totalpages b=batchsize
#
#   where
#	file         is the file created after TeXing or from a previous 
#		     laser command. Specify filename but no extension if
#		     it is .DVI files, specify the extension if you want
#		     to simply print a file.
#	
#	-nc 	     Donot run the file thru DVI to LN03 conversion step
#	-np	     Donot submit the file to the laser printer queue
#	-f	     cause print/feed command instead of print/nofeed which
#		     is the default
#	s=n	     start processing .DVI file at this page
#	n=pcnt       number of pages to process starting with the page 
#		     number specified with the "s=n" option
#	h=hoffset    Move all output right by offset (specify in pts
#		     300 pts = 1inch )
#	v=voffset    Insert vertical offset.
#		     Note: hoffset and voffset are *ADDED* to TeX offsets
#
#	p=totalpages Total number of pages in the document
#	b=batchsize  Print document in batches of size "batchsize" pages
#		     Ouput is placed in .LN3_1, .LN3_2 etc for each batch
exit
}

feed := "/NOFEED"
send := "YES"
conv := "YES"
ln03que := "ttf5:"   /* change this for your system */
sp   := ""
pcnt := ""
bugp = 2            /* even integer is false */
hor := "/h=0"
ver := "/v=0"
total_pages = 0     /* print entire document as one batch *default* */

for( count = 2; p'count' .nes. ""; count = (count + 1) ){
	if( p'count' .eqs. "-D" ){
	    bugp = 1	/* odd integer is true */
	    next        /* go to next parameter */
	}

	if( bugp ){
	    val = f$string( p'count')
	    babble "DEBUG: parameter ''count': ''val'"
	}

	if( p'count' .eqs. "-NP" )   
	    send := "NO"	/* don't send to printer */
	else if( p'count' .eqs. "-NC" )
	    conv := "NO"	/* no dvi to ln3 */
	else if( p'count' .eqs. "-F" )
	    feed := ""		/* do a feed */
        else {               
	    val = f$string( p'count')
            fc  = f$extract( 0,1,val)
	    pval = f$extract( 2, f$length(val)-2, val )

	    if( fc .eqs. "S" ){
		sp := "/"'val'
	        if( bugp )
	            babble "DEBUG: startpage ''sp'"
            }
	    else if( fc .eqs. "N" ){
		pcnt := "/"'val'
	        if( bugp )
	            babble "DEBUG: pagecount ''pcnt'"
            }
	    else if( fc .eqs. "H" ){
		hor := "/"'val'
	        if( bugp )
	            babble "DEBUG: hoffset ''hor'"
	    }
	    else if( fc .eqs. "V" ){
		ver := "/"'val'
	        if( bugp )
	            babble "DEBUG: voffset ''ver'"
	    }
	    else if( fc .eqs. "P" ){
		total_pages = f$integer( pval )
	        if( bugp )
	            babble "DEBUG: total_pages ''f$string(total_pages)'"
	    }
	    else if( fc .eqs. "B" ){
		batch_size = f$integer( pval )
	        if( bugp )
	            babble "DEBUG: batch_size ''f$string(batch_size)'"
	    }
	    else
		babble "Strange option ''val' ignored"
	}
}

if( p1 .nes. "" ){
    if( conv .eqs. "YES" ){
	if( total_pages .eq. 0 ){
    	    dvi2lng 'p1' 'sp' 'pcnt'
    	    ln03topp 'p1' 'hor' 'ver'
    	    dele *.typ;*,*.lng;*,*.lnh;*
    	    ln3 := 'p1'".ln3"
	}
	else {
	    if( (batch_size .gt. total_pages) .or. (batch_size .le. 0) )
		batch_size = total_pages

	    batch_no = 0
	    end_page = batch_size

	    for( start_page = 1; end_page .le. total_pages; ){
		str_val = f$string(start_page )
		sp := "/s="'str_val'
		count = end_page - start_page + 1
		str_val = f$string(count)    /* number of pages */
		pcnt := "/n="'str_val'

		if( bugp .eq. 2 ){
		    dvi2lng 'p1' 'sp' 'pcnt'
		    ln03topp 'p1' 'hor' 'ver'
		}

		batch_no = batch_no + 1
		str_val = f$string(batch_no)
		ln3 := 'p1'".ln3_"'str_val'

		if( .not. bugp )
		    rename 'p1'.ln3 'ln3'

	        if( bugp ){
	            babble "DEBUG: sp       ''sp'"
	            babble "       pcnt     ''pcnt'"
	            babble "       ln3_file ''ln3'"
		}
		
		if( send .eqs. "YES"){
		    babble "Submitting ''ln3'"
		    print/nofeed/que='ln03que' 'ln3'/passall
		}

		start_page = start_page + batch_size
		if( start_page .gt. total_pages )
		    end_page = total_pages+1  /* this will stop the loop */
		else {
		    end_page = start_page + batch_size - 1
		    if( end_page .gt. total_pages )
			end_page = total_pages
		}

	    }  /* end for */
	    if( .not. bugp ){
	        dele *.typ;*,*.lng;*,*.lnh;*
	}
    }
    if( send .eqs. "YES" .and. total_pages .eq. 0){
	if( conv .eqs. "NO" ) 
	    ln3 := 'p1'

    	babble "submitting ''ln3'"
	if( .not. bugp ){
	    if( feed .eqs. "/NOFEED" )
    	        print/nofeed/que='ln03que' 'ln3'/passall
	    else
	        print/que='ln03que'  'ln3'
	}
    }
}
else {
    babble "Type ""laser ?"" for usage details"
}
exit
