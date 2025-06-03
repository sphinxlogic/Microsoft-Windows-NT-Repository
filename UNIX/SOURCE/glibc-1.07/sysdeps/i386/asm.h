#ifdef GAS_SYNTAX
#define R(r) %##r
#define MEM(base)(base)
#define MEM_DISP(base,displacement)displacement##(R(base))
#define MEM_INDEX(base,index,size)(R(base),R(index),size)
#define INSN1(mnemonic,size_suffix,dst)mnemonic##size_suffix dst
#define INSN2(mnemonic,size_suffix,dst,src)mnemonic##size_suffix src##,##dst
#endif

#ifdef INTEL_SYNTAX
#define R(r) r
#define MEM(base)[base]
#define MEM_DISP(base,displacement)[base+(displacement)]
#define MEM_INDEX(base,index,size)[base+index*size]
#define INSN1(mnemonic,size_suffix,dst)mnemonic dst
#define INSN2(mnemonic,size_suffix,dst,src)mnemonic dst##,##src
#endif

#include <sysdep.h>
