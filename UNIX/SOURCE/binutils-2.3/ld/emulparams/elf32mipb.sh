SCRIPT_NAME=elf
OUTPUT_FORMAT="elf32-bigmips"
TEXT_START_ADDR=0x0400000
MAXPAGESIZE=0x40000
NONPAGED_TEXT_START_ADDR=0x0400000
OTHER_READWRITE_SECTIONS='
  _gp = . + 0x8000;
  .lit8 . : { *(.lit8) }
  .lit4 . : { *(.lit4) }
'
ARCH=mips
