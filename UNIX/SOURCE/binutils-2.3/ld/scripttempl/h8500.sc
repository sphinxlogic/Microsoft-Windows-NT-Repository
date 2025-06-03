cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})

MEMORY {
	rom   : o = 0x0000, l = 0x7fe0 
	duart : o = 0x7fe0, l = 16
	ram   : o = 0x8000, l = 28k
	topram : o = 0x8000+28k, l = 1k
	hmsram : o = 0xfb80, l = 512
	}

SECTIONS 				
{ 					
.text 0x10010 :
	{ 					
	  *(.text) 				
	  *(.strings)
   	 ${RELOCATING+ _etext = . ; }
	} ${RELOCATING+ > ram}
.data  0x20010 :
	{
	*(.data)
	${RELOCATING+ _edata = . ; }
	} ${RELOCATING+ > ram}
.bss  0x30010 :
	{
	${RELOCATING+ __start_bss = . ; }
	*(.bss)
	*(COMMON)
	${RELOCATING+ _end = . ;  }
	} ${RELOCATING+ >ram}
.stack 0x40010  : 
	{
	${RELOCATING+ _stack = . ; }
	*(.stack)
	} ${RELOCATING+ > topram}
  .stab  . (NOLOAD) : 
  {
    [ .stab ]
  }
  .stabstr  . (NOLOAD) :
  {
    [ .stabstr ]
  }
}
EOF




