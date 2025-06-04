	character	dev_name *20

	integer	*4	name_len
	integer	*4	ret_stat

	external	unlink_vt
	integer	*4	unlink_vt

	do while (.true.)

		write (6, fmt='(''$enter device name: '')')
		read (5, fmt='(q,a)',end=100,err=200) name_len, dev_name(:name_len)
		ret_stat = unlink_vt (dev_name(1:name_len))
		write (6, fmt='(''/ status is:'', z8, /)') ret_stat

	enddo

100	stop	'That''s all folks'
200	stop	'Read error'
    
	end
    
