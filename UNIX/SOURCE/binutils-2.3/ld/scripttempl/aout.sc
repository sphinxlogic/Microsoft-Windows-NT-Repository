cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})

${RELOCATING+${LIB_SEARCH_DIRS}}
${RELOCATING+__DYNAMIC  =  0;}
${STACKZERO+${RELOCATING+${STACKZERO}}}
${SHLIB_PATH+${RELOCATING+${SHLIB_PATH}}}
SECTIONS
{
  .text ${RELOCATING+${TEXT_START_ADDR}}:
  {
    CREATE_OBJECT_SYMBOLS
    *(.text)
    ${RELOCATING+_etext = ${DATA_ALIGNMENT};}
  }
  .data  ${RELOCATING+${DATA_ALIGNMENT}} :
  {
    *(.data)
    ${CONSTRUCTING+CONSTRUCTORS}
    ${RELOCATING+_edata  =  .;}
  }
  .bss ${RELOCATING+SIZEOF(.data) + ADDR(.data)} :
  {
    ${RELOCATING+ __bss_start = .};
   *(.bss)
   *(COMMON)
   ${RELOCATING+_end = . };
   ${RELOCATING+__end = . };
  }
}
EOF
