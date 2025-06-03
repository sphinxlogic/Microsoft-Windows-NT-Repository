OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
SEARCH_DIR(/lib); SEARCH_DIR(/usr/lib); SEARCH_DIR(/usr/ccs/lib); SEARCH_DIR(/opt/lib); SEARCH_DIR(/usr/local/lib);
/* Do we need any of these for elf?
   __DYNAMIC = 0;    */
SECTIONS
{
  /* Read-only sections, merged into text segment: */
  .text 0x08000000  :
  {
    *(.text)
    CREATE_OBJECT_SYMBOLS
    _etext = .;
  }
  .init    .  : { *(.init)    } =0x9090
  .fini    .  : { *(.fini)    } =0x9090
  .ctors   .  : { *(.ctors)   }
  .dtors   .  : { *(.dtors)   }
  .rodata  .  : { *(.rodata)  }
  .rodata1 .  : { *(.rodata1) }
  /* also: .hash .dynsym .dynstr .plt(if r/o) .rel.got */
  /* Read-write section, merged into data segment: */
  .data  
	    
	     ADDR(.rodata1)+SIZEOF(.rodata1)+0x1000
	  
	  :
  {
    *(.data)
    CONSTRUCTORS
    _edata  =  .;
  }
  .data1 .  : { *(.data1) }
  /* also (before uninitialized portion): .dynamic .got .plt(if r/w)
     (or does .dynamic go into its own segment?) */
  /* We want the small data sections together, so single-instruction offsets
     can access them all, and initialized data all before uninitialized, so
     we can shorten the on-disk segment size.  */
  .sdata   .  : { *(.sdata) }
  .sbss    .  : { *(.sbss) *(.scommon) }
  .bss     .  :
  {
   __bss_start = .;
   *(.bss)
   *(COMMON)
   _end = . ;
   end = . ;
  }
  /* Debug sections.  These should never be loadable, but they must have
     zero addresses for the debuggers to work correctly.  */
  .line			0 : { *(.line)			}
  .debug		0 : { *(.debug)			}
  .debug_sfnames	0 : { *(.debug_sfnames)		}
  .debug_srcinfo	0 : { *(.debug_srcinfo)		}
  .debug_macinfo	0 : { *(.debug_macinfo)		}
  .debug_pubnames	0 : { *(.debug_pubnames)	}
  .debug_aranges	0 : { *(.debug_aranges)		}
}
