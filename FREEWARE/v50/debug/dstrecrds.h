/**/
/******************************************************************************/
/**                                                                          **/
/**  Copyright (c) 1998                                                      **/
/**  by DIGITAL Equipment Corporation, Maynard, Mass.                        **/
/**                                                                          **/
/**  This software is furnished under a license and may be used and  copied  **/
/**  only  in  accordance  with  the  terms  of  such  license and with the  **/
/**  inclusion of the above copyright notice.  This software or  any  other  **/
/**  copies  thereof may not be provided or otherwise made available to any  **/
/**  other person.  No title to and ownership of  the  software  is  hereby  **/
/**  transferred.                                                            **/
/**                                                                          **/
/**  The information in this software is subject to change  without  notice  **/
/**  and  should  not  be  construed  as  a commitment by DIGITAL Equipment  **/
/**  Corporation.                                                            **/
/**                                                                          **/
/**  DIGITAL assumes no responsibility for the use or  reliability  of  its  **/
/**  software on equipment which is not supplied by DIGITAL.                 **/
/**                                                                          **/
/******************************************************************************/
/********************************************************************************************************************************/
/* Created  8-JUN-1998 10:33:19 by VAX SDL V3.2-12     Source:  8-JUN-1998 10:32:56 MAWK$DKA300:[ARSENAULT.EVMS.CODE]DSTRECRDS.SD */
/********************************************************************************************************************************/
 
/*** MODULE DSTRECRDS ***/
#ifndef DSTRECRDS_H
#define DSTRECRDS_H 1
typedef struct _DBG$DMT_PSECT {
    int *dbg$l_dmt_psect_start;
    unsigned long int dbg$l_dmt_psect_length;
    } DBG$DMT_PSECT;
#define DBG$K_DMT_PSECT_SIZE 8
typedef struct _DBG$DMT_HEADER {
    int *dbg$l_dmt_modbeg;
    unsigned long int dbg$l_dmt_dst_size;
    unsigned short int dbg$w_dmt_psect_count;
    unsigned short int dbg$w_dmt_mbz;
    } DBG$DMT_HEADER;
#define DBG$K_DMT_HEADER_SIZE 12
typedef unsigned char DST$DTYPE;
#define DSC$K_DTYPE_LOWEST 1
#define DSC$K_DTYPE_HIGHEST 39
#define DSC$K_DTYPE_TF 40
#define DSC$K_DTYPE_SV 41
#define DSC$K_DTYPE_SVU 42
#define DSC$K_DTYPE_FIXED 43
#define DSC$K_DTYPE_TASK 44
#define DSC$K_DTYPE_AC 45
#define DSC$K_DTYPE_AZ 46
#define DSC$K_DTYPE_M68_S 47
#define DSC$K_DTYPE_M68_D 48
#define DSC$K_DTYPE_M68_X 49
#define DSC$K_DTYPE_1750_S 50
#define DSC$K_DTYPE_1750_X 51
#define DBG$K_MINIMUM_DTYPE 0
#define DBG$K_MAXIMUM_DTYPE 58
#define DSC$K_DTYPE_LITERAL 191
#define DBG$K_DTYPE_AD 192
#define DST$K_BLI 0
#define DST$K_LOWEST 116
#define DST$K_SYMBOL_FIXUP_64 116
#define DST$K_FIXUP_64 117
#define DST$K_DIS_RANGE 118
#define DST$K_PROLOG_LIST 119
#define DST$K_RTN_UNALLOC 120
#define DST$K_SYMBOL_FIXUP 121
#define DST$K_BASE_CLASS 122
#define DST$K_TEMP_DECL 123
#define DST$K_VIRT_FUNC 124
#define DST$K_EXCEPTION 125
#define DST$K_RETURN 126
#define DST$K_EPILOG 127
#define DST$K_REG_SAVE_END 128
#define DST$K_REG_SAVE 129
#define DST$K_REG_SAVE_BEGIN 130
#define DST$K_BLIFLDBEG 131
#define DST$K_BLIFLDEND 132
#define DST$K_FULFILLS_TYPE 133
#define DST$K_FIXUP 134
#define DST$K_IMAGE 135
#define DST$K_INLINE 136
#define DST$K_EPILOGS 137
#define DST$K_TYPE_SIG 138
#define DST$K_EDIT_SOURCE 139
#define DST$K_ALIAS 140
#define DST$K_CXX_ATTRIBUTES 141
#define DST$K_GOTO 142
#define DST$K_TARGET 143
#define DST$K_REAL_NAME 144
#define DST$K_BODY_SPEC 145
#define DST$K_PACK_SPEC_BEG 146
#define DST$K_PACK_SPEC_END 147
#define DST$K_PACK_BODY_BEG 148
#define DST$K_PACK_BODY_END 149
#define DST$K_SUBUNIT 150
#define DST$K_SET_MODULE 151
#define DST$K_USE_CLAUSE 152
#define DST$K_VERSION 153
#define DST$K_COBOLGBL 154
#define DST$K_SOURCE 155
#define DST$K_STATLINK 156
#define DST$K_VARVAL 157
#define DST$K_BOOL 158
#define DST$K_EXTRNXT 159
#define DST$K_GLOBNXT 160
#define DSC$K_DTYPE_UBS 161
#define DST$K_PROLOG 162
#define DST$K_SEPTYP 163
#define DST$K_ENUMELT 164
#define DST$K_ENUMBEG 165
#define DST$K_ENUMEND 166
#define DST$K_VARBEG 167
#define DST$K_VAREND 168
#define DST$K_OVERLOAD 169
#define DST$K_DEF_LNUM 170
#define DST$K_RECBEG 171
#define DST$K_RECEND 172
#define DST$K_CONTIN 173
#define DST$K_VALSPEC 174
#define DST$K_TYPSPEC 175
#define DST$K_BLKBEG 176
#define DST$K_BLKEND 177
#define DST$K_COB_HACK 178
#define DST$K_DTYPE_RESERVED_1 179
#define DST$K_USING 180
#define DST$K_ENTRY 181
#define DST$K_LINE_NUM_REL_R11 182
#define DST$K_BLIFLD 183
#define DST$K_PSECT 184
#define DST$K_LINE_NUM 185
#define DST$K_LBLORLIT 186
#define DST$K_LABEL 187
#define DST$K_MODBEG 188
#define DST$K_MODEND 189
#define DST$K_RTNBEG 190
#define DST$K_RTNEND 191
#define DST$K_PCLOC 192
#define DST$K_HIGHEST 192
typedef struct _DST$HEADER {
    union  {
        unsigned char dst$b_length;
        unsigned char dst$x_length;
        } dst$$header_length;
    union  {
        DST$DTYPE dst$b_type;
        DST$DTYPE dst$x_type;
        } dst$$header_type;
    } DST$HEADER;
#define DST$K_DST_BASE_FOR_NEXT 1
#define DST$K_DST_HEADER_SIZE 2
#define DST$K_LENGTH_LENGTH 1
#define DST$K_LENGTH_TYPE 1
typedef unsigned long int DST$LANGUAGE;
typedef unsigned char DBG$DST_LANGUAGE;
#define DST$K_MIN_LANGUAGE 0
#define DST$K_MACRO 0
#define DST$K_FORTRAN 1
#define DST$K_BLISS 2
#define DST$K_COBOL 3
#define DST$K_BASIC 4
#define DST$K_PLI 5
#define DST$K_PASCAL 6
#define DST$K_C 7
#define DST$K_RPG 8
#define DST$K_ADA 9
#define DST$K_UNKNOWN 10
#define DST$K_SCAN 11
#define DST$K_DIBOL 12
#define DST$K_MODULA 13
#define DST$K_PILLAR 14
#define DST$K_CXX 15
#define DST$K_AMACRO 16
#define DST$K_MACRO64 17
#define DST$K_MAX_LANGUAGE 17
#define DBG$K_MIN_LANGUAGE 0
#define DBG$K_MACRO 0
#define DBG$K_FORTRAN 1
#define DBG$K_BLISS 2
#define DBG$K_COBOL 3
#define DBG$K_BASIC 4
#define DBG$K_PLI 5
#define DBG$K_PASCAL 6
#define DBG$K_C 7
#define DBG$K_RPG 8
#define DBG$K_ADA 9
#define DBG$K_UNKNOWN 10
#define DBG$K_SCAN 11
#define DBG$K_DIBOL 12
#define DBG$K_MODULA 13
#define DBG$K_PILLAR 14
#define DBG$K_CXX 15
#define DBG$K_AMACRO 16
#define DBG$K_MACRO64 17
#define DBG$K_MAX_LANGUAGE 17
typedef struct _DST$MODULE_BEGIN {
    DST$HEADER dst$a_modbeg_header;
    struct  {
        unsigned dst$v_modbeg_hide : 1;
        unsigned dst$v_modbeg_version : 1;
        unsigned dst$v_modbeg_unused : 6;
        } dst$b_modbeg_flags;
    DST$LANGUAGE dst$l_modbeg_language;
    unsigned char dst$b_modbeg_name;
    } DST$MODULE_BEGIN;
#define DST$K_MODBEG_SIZE 8
typedef struct _DST$MODULE_END {
    DST$HEADER dst$a_modend_header;
    } DST$MODULE_END;
#define DST$K_MODEND_SIZE 2
typedef struct _DST$ROUTINE_BEGIN {
    DST$HEADER dst$a_rtnbeg_header;
    struct  {
        unsigned dst$v_rtnbeg_unused : 4;
        unsigned dst$v_rtnbeg_unalloc : 1;
        unsigned dst$v_rtnbeg_prototype : 1;
        unsigned dst$v_rtnbeg_inlined : 1;
        unsigned dst$v_rtnbeg_no_call : 1;
        } dst$b_rtnbeg_flags;
    int *dst$l_rtnbeg_address;
    unsigned char dst$b_rtnbeg_name;
    } DST$ROUTINE_BEGIN;
#define DST$K_RTNBEG_SIZE 8
typedef struct _DST$ROUTINE_END {
    DST$HEADER dst$a_rtnend_header;
    char dst$b_rtnend_unused;
    unsigned long int dst$l_rtnend_size;
    } DST$ROUTINE_END;
#define DST$K_RTNEND_SIZE 7
typedef struct _DST$ROUTINE_UNALLOC {
    DST$HEADER dst$a_rtnunalloc_header;
    unsigned char dst$b_rtnunalloc_name;
    } DST$ROUTINE_UNALLOC;
#define DST$K_RTNUNALLOC_SIZE 3
typedef struct _DST$INLINE {
    DST$HEADER dst$a_inline_header;
    unsigned long int dst$l_inline_rtn_dst;
    unsigned long int dst$l_inline_caller_line;
    } DST$INLINE;
#define DST$K_INLINE_SIZE 10
typedef struct _DST$BLOCK_BEGIN {
    DST$HEADER dst$a_blkbeg_header;
    unsigned char dst$b_blkbeg_unused;
    int *dst$l_blkbeg_address;
    unsigned char dst$b_blkbeg_name;
    } DST$BLOCK_BEGIN;
#define DST$K_BLKBEG_SIZE 8
typedef struct _DST$BLOCK_END {
    DST$HEADER dst$a_blkend_header;
    unsigned char dst$b_blkend_unused;
    unsigned long int dst$l_blkend_size;
    } DST$BLOCK_END;
#define DST$K_BLKEND_SIZE 7
#define DST$K_NOELAB 0
#define DST$K_ELAB_NOCALL 1
#define DST$K_ELAB_CALL 2
typedef struct _DST$PACKAGE_SPEC_BEGIN {
    DST$HEADER dst$a_packspec_beg_header;
    struct  {
        unsigned dst$v_packspec_elab : 2;
        unsigned dst$v_packspec_dupl : 1;
        unsigned dst$v_packspec_beg_unused : 5;
        } dst$b_packspec_flags;
    int *dst$l_packspec_address;
    unsigned char dst$b_packspec_name;
    } DST$PACKAGE_SPEC_BEGIN;
#define DST$K_PACKSPEC_BEG_SIZE 8
typedef struct _DST$PACKAGE_SPEC_END {
    DST$HEADER dst$a_packspec_end_header;
    unsigned char dst$b_packspec_end_unused;
    unsigned long int dst$l_packspec_end_size;
    } DST$PACKAGE_SPEC_END;
#define DST$K_PACKSPEC_END_SIZE 7
typedef struct _DST$PACKAGE_BODY_BEGIN {
    DST$HEADER dst$a_packbody_beg_header;
    struct  {
        unsigned dst$v_packbody_elab : 2;
        unsigned dst$v_packbody_mbz : 6;
        } dst$b_packbody_flags;
    int *dst$l_packbody_address;
    unsigned char dst$b_packbody_name;
    } DST$PACKAGE_BODY_BEGIN;
#define DST$K_PACKBODY_BEG_SIZE 8
typedef struct _DST$PACKAGE_BODY_END {
    DST$HEADER dst$a_packbody_end_header;
    unsigned char dst$b_packbody_end_unused;
    unsigned long int dst$l_packbody_end_size;
    } DST$PACKAGE_BODY_END;
#define DST$K_PACKBODY_END_SIZE 7
typedef struct _DST$DIS_RANGES {
    DST$HEADER dst$a_disrng_header;
    unsigned long int dst$lu_disrng_count;
    } DST$DIS_RANGES;
#define DST$K_DISRNGS_SIZE 6
typedef struct _DST$DIS_RANGE {
    unsigned long int dst$lu_disrng_address;
    unsigned long int dst$lu_disrng_size;
    } DST$DIS_RANGE;
#define DST$K_DISRNG_SIZE 8
typedef struct _DST$RECBEG_TRLR {
    unsigned long int dst$l_recbeg_size;
    } DST$RECBEG_TRLR;
#define DST$K_RECBEG_TRAILER_SIZE 4
typedef struct _DST$RECORD_END {
    DST$HEADER dst$a_recend_header;
    } DST$RECORD_END;
#define DST$K_RECEND_SIZE 2
typedef struct _DST$VARBEG_TRAILER {
    unsigned long int dst$l_varbeg_size;
    int *dst$l_varbeg_tag_ptr;
    } DST$VARBEG_TRAILER;
#define DST$K_VARBEG_TRAILER_SIZE 8
typedef struct _DST$VARIANT_VALUE {
    DST$HEADER dst$a_varval_header;
    unsigned long int dst$l_varval_size;
    unsigned short int dst$w_varval_count;
    } DST$VARIANT_VALUE;
#define DST$K_VARIANT_VALUE_SIZE 8
typedef struct _DST$VARVAL_RANGE {
    unsigned char dst$b_varval_rngkind;
    } DST$VARVAL_RANGE;
#define DST$K_VARVAL_RANGE_SIZE 1
#define DST$K_VARVAL_SINGLE 1
#define DST$K_VARVAL_RANGE 2
typedef struct _DST$VARIANT_SET_END {
    DST$HEADER dst$a_varset_end_header;
    } DST$VARIANT_SET_END;
#define DST$K_VARSET_END_SIZE 2
typedef struct _DST$DATA_HEADER {
    DST$HEADER dst$a_data_header;
    struct  {
        unsigned dst$v_valkind : 2;
        unsigned dst$v_indirect : 1;
        unsigned dst$v_disp : 1;
        unsigned dst$v_regnum : 4;
        } dst$b_vflags;
    } DST$DATA_HEADER;
#define DST$K_DATA_HEADER_SIZE 3
typedef struct _DST$DATA_DST {
    DST$DATA_HEADER dst$a_data_dst_header;
    unsigned long int dst$l_value;
    unsigned char dst$b_name;
    } DST$DATA_DST;
#define DST$K_DATA_SIZE 8
#define DST$K_VFLAGS_NOVAL 128
#define DST$K_VFLAGS_NOTACTIVE 248
#define DST$K_VFLAGS_UNALLOC 249
#define DST$K_VFLAGS_DSC 250
#define DST$K_VFLAGS_TVS 251
#define DST$K_VS_FOLLOWS 253
#define DST$K_VFLAGS_BITOFFS 255
#define DST$K_VALKIND_LITERAL 0
#define DST$K_VALKIND_ADDR 1
#define DST$K_VALKIND_DESC 2
#define DST$K_VALKIND_REG 3
#define DST$K_VALKIND_MIN 0
#define DST$K_VALKIND_MAX 3
#define DST$K_NO_SUCH_REG -1
#define DST$K_REG_VAX_R0 0
#define DST$K_REG_VAX_R1 1
#define DST$K_REG_VAX_R2 2
#define DST$K_REG_VAX_R3 3
#define DST$K_REG_VAX_R4 4
#define DST$K_REG_VAX_R5 5
#define DST$K_REG_VAX_R6 6
#define DST$K_REG_VAX_R7 7
#define DST$K_REG_VAX_R8 8
#define DST$K_REG_VAX_R9 9
#define DST$K_REG_VAX_R10 10
#define DST$K_REG_VAX_R11 11
#define DST$K_REG_VAX_R12 12
#define DST$K_REG_VAX_R13 13
#define DST$K_REG_VAX_R14 14
#define DST$K_REG_VAX_R15 15
#define DST$K_REG_VAX_PSL 16
#define DST$K_REG_VAX_V0 17
#define DST$K_REG_VAX_V1 18
#define DST$K_REG_VAX_V2 19
#define DST$K_REG_VAX_V3 20
#define DST$K_REG_VAX_V4 21
#define DST$K_REG_VAX_V5 22
#define DST$K_REG_VAX_V6 23
#define DST$K_REG_VAX_V7 24
#define DST$K_REG_VAX_V8 25
#define DST$K_REG_VAX_V9 26
#define DST$K_REG_VAX_V10 27
#define DST$K_REG_VAX_V11 28
#define DST$K_REG_VAX_V12 29
#define DST$K_REG_VAX_V13 30
#define DST$K_REG_VAX_V14 31
#define DST$K_REG_VAX_V15 32
#define DST$K_REG_VAX_VCR 33
#define DST$K_REG_VAX_VLR 34
#define DST$K_REG_VAX_VMR 35
#define DST$K_REG_VAX_MIN 0
#define DST$K_REG_VAX_MAX 35
#define DST$K_REG_VAX_AP 12
#define DST$K_REG_VAX_FP 13
#define DST$K_REG_VAX_SP 14
#define DST$K_REG_VAX_PC 15
#define DST$K_REG_VAX_PS 16
#define DST$K_REG_VAX_MIN_SCALAR 0
#define DST$K_REG_VAX_MAX_SCALAR 15
#define DST$K_REG_VAX_MIN_FLOAT -1
#define DST$K_REG_VAX_MAX_FLOAT -1
#define DST$K_REG_VAX_MIN_REAL_VECTOR 17
#define DST$K_REG_VAX_MAX_REAL_VECTOR 32
#define DST$K_REG_VAX_MIN_VECTOR 17
#define DST$K_REG_VAX_MAX_VECTOR 35
#define DST$K_REG_ALPHA_R0 36
#define DST$K_REG_ALPHA_R1 37
#define DST$K_REG_ALPHA_R2 38
#define DST$K_REG_ALPHA_R3 39
#define DST$K_REG_ALPHA_R4 40
#define DST$K_REG_ALPHA_R5 41
#define DST$K_REG_ALPHA_R6 42
#define DST$K_REG_ALPHA_R7 43
#define DST$K_REG_ALPHA_R8 44
#define DST$K_REG_ALPHA_R9 45
#define DST$K_REG_ALPHA_R10 46
#define DST$K_REG_ALPHA_R11 47
#define DST$K_REG_ALPHA_R12 48
#define DST$K_REG_ALPHA_R13 49
#define DST$K_REG_ALPHA_R14 50
#define DST$K_REG_ALPHA_R15 51
#define DST$K_REG_ALPHA_R16 0
#define DST$K_REG_ALPHA_R17 1
#define DST$K_REG_ALPHA_R18 2
#define DST$K_REG_ALPHA_R19 3
#define DST$K_REG_ALPHA_R20 4
#define DST$K_REG_ALPHA_R21 5
#define DST$K_REG_ALPHA_R22 6
#define DST$K_REG_ALPHA_R23 7
#define DST$K_REG_ALPHA_R24 8
#define DST$K_REG_ALPHA_R25 9
#define DST$K_REG_ALPHA_R26 10
#define DST$K_REG_ALPHA_R27 11
#define DST$K_REG_ALPHA_R28 12
#define DST$K_REG_ALPHA_R29 13
#define DST$K_REG_ALPHA_R30 14
#define DST$K_REG_ALPHA_R31 15
#define DST$K_REG_ALPHA_F0 52
#define DST$K_REG_ALPHA_F1 53
#define DST$K_REG_ALPHA_F2 54
#define DST$K_REG_ALPHA_F3 55
#define DST$K_REG_ALPHA_F4 56
#define DST$K_REG_ALPHA_F5 57
#define DST$K_REG_ALPHA_F6 58
#define DST$K_REG_ALPHA_F7 59
#define DST$K_REG_ALPHA_F8 60
#define DST$K_REG_ALPHA_F9 61
#define DST$K_REG_ALPHA_F10 62
#define DST$K_REG_ALPHA_F11 63
#define DST$K_REG_ALPHA_F12 64
#define DST$K_REG_ALPHA_F13 65
#define DST$K_REG_ALPHA_F14 66
#define DST$K_REG_ALPHA_F15 67
#define DST$K_REG_ALPHA_F16 68
#define DST$K_REG_ALPHA_F17 69
#define DST$K_REG_ALPHA_F18 70
#define DST$K_REG_ALPHA_F19 71
#define DST$K_REG_ALPHA_F20 72
#define DST$K_REG_ALPHA_F21 73
#define DST$K_REG_ALPHA_F22 74
#define DST$K_REG_ALPHA_F23 75
#define DST$K_REG_ALPHA_F24 76
#define DST$K_REG_ALPHA_F25 77
#define DST$K_REG_ALPHA_F26 78
#define DST$K_REG_ALPHA_F27 79
#define DST$K_REG_ALPHA_F28 80
#define DST$K_REG_ALPHA_F29 81
#define DST$K_REG_ALPHA_F30 82
#define DST$K_REG_ALPHA_F31 83
#define DST$K_REG_ALPHA_PC 84
#define DST$K_REG_ALPHA_PS 85
#define DST$K_REG_ALPHA_MIN 0
#define DST$K_REG_ALPHA_MAX 85
#define DST$K_REG_ALPHA_AI 9
#define DST$K_REG_ALPHA_RA 10
#define DST$K_REG_ALPHA_PV 11
#define DST$K_REG_ALPHA_FP 13
#define DST$K_REG_ALPHA_SP 14
#define DST$K_REG_ALPHA_REGNUM_FP 13
#define DST$K_REG_ALPHA_REGNUM_SP 14
#define DST$K_REG_ALPHA_MIN_SCALAR 36
#define DST$K_REG_ALPHA_MAX_SCALAR 15
#define DST$K_REG_ALPHA_MIN_FLOAT 52
#define DST$K_REG_ALPHA_MAX_FLOAT 83
#define DST$K_REG_ALPHA_MIN_VECTOR -1
#define DST$K_REG_ALPHA_MAX_VECTOR -1
#define DST$K_REG_M68_A0 0
#define DST$K_REG_M68_A1 1
#define DST$K_REG_M68_A2 2
#define DST$K_REG_M68_A3 3
#define DST$K_REG_M68_A4 4
#define DST$K_REG_M68_A5 5
#define DST$K_REG_M68_A6 6
#define DST$K_REG_M68_A7 7
#define DST$K_REG_M68_D0 8
#define DST$K_REG_M68_D1 9
#define DST$K_REG_M68_D2 10
#define DST$K_REG_M68_D3 11
#define DST$K_REG_M68_D4 12
#define DST$K_REG_M68_D5 13
#define DST$K_REG_M68_D6 14
#define DST$K_REG_M68_D7 15
#define DST$K_REG_M68_FP0 16
#define DST$K_REG_M68_FP1 17
#define DST$K_REG_M68_FP2 18
#define DST$K_REG_M68_FP3 19
#define DST$K_REG_M68_FP4 20
#define DST$K_REG_M68_FP5 21
#define DST$K_REG_M68_FP6 22
#define DST$K_REG_M68_FP7 23
#define DST$K_REG_M68_PC 24
#define DST$K_REG_M68_SR 25
#define DST$K_REG_M68_FPCR 26
#define DST$K_REG_M68_FPSR 27
#define DST$K_REG_M68_FPIAR 28
#define DST$K_REG_M68_USP 29
#define DST$K_REG_M68_MSP 30
#define DST$K_REG_M68_CAAR 31
#define DST$K_REG_M68_CACR 32
#define DST$K_REG_M68_VBR 33
#define DST$K_REG_M68_SFC 34
#define DST$K_REG_M68_DFC 35
#define DST$K_REG_M68_MIN 0
#define DST$K_REG_M68_MAX 35
#define DST$K_REG_M68_MIN_SCALAR 0
#define DST$K_REG_M68_MAX_SCALAR 15
#define DST$K_REG_M68_MIN_FLOAT 16
#define DST$K_REG_M68_MAX_FLOAT 23
#define DST$K_REG_M68_MIN_VECTOR -1
#define DST$K_REG_M68_MAX_VECTOR -1
#define DST$K_REG_1750_R15 0
#define DST$K_REG_1750_R14 1
#define DST$K_REG_1750_R13 2
#define DST$K_REG_1750_R12 3
#define DST$K_REG_1750_R11 4
#define DST$K_REG_1750_R10 5
#define DST$K_REG_1750_R9 6
#define DST$K_REG_1750_R8 7
#define DST$K_REG_1750_R7 8
#define DST$K_REG_1750_R6 9
#define DST$K_REG_1750_R5 10
#define DST$K_REG_1750_R4 11
#define DST$K_REG_1750_R3 12
#define DST$K_REG_1750_R2 13
#define DST$K_REG_1750_R1 14
#define DST$K_REG_1750_R0 15
#define DST$K_REG_1750_PC 16
#define DST$K_REG_1750_SW 17
#define DST$K_REG_1750_FT 18
#define DST$K_REG_1750_MK 19
#define DST$K_REG_1750_PI 20
#define DST$K_REG_1750_IOIC 21
#define DST$K_REG_1750_MFSR 22
#define DST$K_REG_1750_PAGE 23
#define DST$K_REG_1750_MIN 0
#define DST$K_REG_1750_MAX 23
#define DST$K_REG_1750_SP 4
#define DST$K_REG_1750_FP 0
#define DST$K_REG_1750_IC 16
#define DST$K_REG_1750_MIN_SCALAR 0
#define DST$K_REG_1750_MAX_SCALAR 15
#define DST$K_REG_1750_MIN_INDEX 0
#define DST$K_REG_1750_MAX_INDEX 14
#define DST$K_REG_1750_MIN_BASE 0
#define DST$K_REG_1750_MAX_BASE 3
#define DST$K_REG_1750_MIN_FLOAT -1
#define DST$K_REG_1750_MAX_FLOAT -1
#define DST$K_REG_1750_MIN_VECTOR -1
#define DST$K_REG_1750_MAX_VECTOR -1
typedef struct _DST$DESCRIPTOR_FORMAT {
    DST$DATA_HEADER dst$a_dsc_format_header;
    unsigned long int dst$l_dsc_offs;
    } DST$DESCRIPTOR_FORMAT;
#define DST$K_DESCRIPTOR_FORMAT_SIZE 7
typedef struct _DST$TRAILING_VALSPEC {
    DST$DATA_HEADER dst$a_tvs_header;
    unsigned long int dst$l_tvs_offset;
    } DST$TRAILING_VALSPEC;
#define DST$K_TRAILING_VALSPEC_SIZE 7
#define DST$K_SEPTYP_SIZE 8
typedef unsigned char DST$VS_ALLOC_KIND;
#define DST$K_VS_ALLOC_STAT 1
#define DST$K_VS_ALLOC_DYN 2
#define DST$K_VS_ALLOC_SPLIT 3
#define DST$K_VS_ALLOC_BIASED 4
#define DST$K_VS_ALLOC_XVS 5
#define DST$K_VS_ALLOC_MIN 1
#define DST$K_VS_ALLOC_MAX 5
typedef struct _DST$VAL_SPEC {
    struct  {
        unsigned dst$v_vs_valkind : 2;
        unsigned dst$v_vs_indirect : 1;
        unsigned dst$v_vs_disp : 1;
        unsigned dst$v_vs_regnum : 4;
        } dst$b_vs_vflags;
    union  {
        unsigned long int dst$l_vs_value;
        unsigned long int dst$l_vs_dsc_offs;
        unsigned long int dst$l_vs_tvs_offset;
        struct  {
            unsigned short int dst$w_vs_length;
            DST$VS_ALLOC_KIND dst$b_vs_alloc;
            union  {
                unsigned char dst$b_vs_num_bindings;
                unsigned char dst$b_vs_reserved;
                } dst$a_mat_spec_info;
            } dst$a_vs_materialization_spec;
        } dst$value_spec_variants;
    } DST$VAL_SPEC;
#define DST$K_VALUE_SPEC_SIZE 5
#define DST$K_VS_DSC_BASE 5
#define DST$K_VS_TVS_BASE 5
#define DST$K_VS_MATSPEC_BASE 4
#define DST$K_VS_BINDSPEC_BASE 5
#define DST$K_VS_BIASING_VS_BASE 5
#define DST$K_VS_BIASED_VS_BASE 10
#define DST$K_VS_XVS_BASE 4
typedef struct _DST$BIND_SPEC {
    unsigned long int dst$l_bs_lo_pc;
    unsigned long int dst$l_bs_hi_pc;
    } DST$BIND_SPEC;
#define DST$K_BIND_SPEC_SIZE 8
typedef unsigned char DST$MS_KIND;
#define DST$K_MS_BYTADDR 1
#define DST$K_MS_BITADDR 2
#define DST$K_MS_BITOFFS 3
#define DST$K_MS_RVAL 4
#define DST$K_MS_REG 5
#define DST$K_MS_DSC 6
#define DST$K_MS_ADDR_DSC 7
#define DST$K_MS_LOWEST 1
#define DST$K_MS_HIGHEST 7
typedef unsigned char DST$MS_MECH;
#define DST$K_MS_MECH_RTNCALL 1
#define DST$K_MS_MECH_STK 2
#define DST$K_MS_MECH_RTN_NOFP 3
#define DST$K_MS_MECH_MIN 1
#define DST$K_MS_MECH_MAX 3
typedef struct _DST$MATER_SPEC {
    DST$MS_KIND dst$b_ms_kind;
    DST$MS_MECH dst$b_ms_mech;
    struct  {
        unsigned dst$v_ms_noeval : 1;
        unsigned dst$v_ms_dumarg : 1;
        unsigned dst$v_ms_wrongbounds : 1;
        unsigned dst$v_ms_mbz : 5;
        } dst$b_ms_flagbits;
    int *dst$l_ms_mech_rtnaddr;
    } DST$MATER_SPEC;
#define DST$K_MATER_SPEC_HEADER_SIZE 3
#define DST$K_MATER_SPEC_SIZE 7
#define DST$K_STK_LOW 0
#define DST$K_STK_PUSHR0 0
#define DST$K_STK_PUSHR1 1
#define DST$K_STK_PUSHR2 2
#define DST$K_STK_PUSHR3 3
#define DST$K_STK_PUSHR4 4
#define DST$K_STK_PUSHR5 5
#define DST$K_STK_PUSHR6 6
#define DST$K_STK_PUSHR7 7
#define DST$K_STK_PUSHR8 8
#define DST$K_STK_PUSHR9 9
#define DST$K_STK_PUSHR10 10
#define DST$K_STK_PUSHR11 11
#define DST$K_STK_PUSHRAP 12
#define DST$K_STK_PUSHRFP 13
#define DST$K_STK_PUSHRSP 14
#define DST$K_STK_PUSHRPC 15
#define DST$K_STK_PUSH_ALPHA_R0 55
#define DST$K_STK_PUSH_ALPHA_R1 56
#define DST$K_STK_PUSH_ALPHA_R2 57
#define DST$K_STK_PUSH_ALPHA_R3 58
#define DST$K_STK_PUSH_ALPHA_R4 59
#define DST$K_STK_PUSH_ALPHA_R5 60
#define DST$K_STK_PUSH_ALPHA_R6 61
#define DST$K_STK_PUSH_ALPHA_R7 62
#define DST$K_STK_PUSH_ALPHA_R8 63
#define DST$K_STK_PUSH_ALPHA_R9 64
#define DST$K_STK_PUSH_ALPHA_R10 65
#define DST$K_STK_PUSH_ALPHA_R11 66
#define DST$K_STK_PUSH_ALPHA_R12 67
#define DST$K_STK_PUSH_ALPHA_R13 68
#define DST$K_STK_PUSH_ALPHA_R14 69
#define DST$K_STK_PUSH_ALPHA_R15 70
#define DST$K_STK_PUSH_ALPHA_R16 71
#define DST$K_STK_PUSH_ALPHA_R17 72
#define DST$K_STK_PUSH_ALPHA_R18 73
#define DST$K_STK_PUSH_ALPHA_R19 74
#define DST$K_STK_PUSH_ALPHA_R20 75
#define DST$K_STK_PUSH_ALPHA_R21 76
#define DST$K_STK_PUSH_ALPHA_R22 77
#define DST$K_STK_PUSH_ALPHA_R23 78
#define DST$K_STK_PUSH_ALPHA_R24 79
#define DST$K_STK_PUSH_ALPHA_R25 80
#define DST$K_STK_PUSH_ALPHA_R26 81
#define DST$K_STK_PUSH_ALPHA_R27 82
#define DST$K_STK_PUSH_ALPHA_R28 83
#define DST$K_STK_PUSH_ALPHA_R29 84
#define DST$K_STK_PUSH_ALPHA_R30 85
#define DST$K_STK_PUSH_ALPHA_R31 86
#define DST$K_STK_PUSH_ALPHA_AI 80
#define DST$K_STK_PUSH_ALPHA_RA 81
#define DST$K_STK_PUSH_ALPHA_PV 82
#define DST$K_STK_PUSH_ALPHA_FP 84
#define DST$K_STK_PUSH_ALPHA_SP 85
#define DST$K_STK_PUSH_M68_A0 0
#define DST$K_STK_PUSH_M68_A1 1
#define DST$K_STK_PUSH_M68_A2 2
#define DST$K_STK_PUSH_M68_A3 3
#define DST$K_STK_PUSH_M68_A4 4
#define DST$K_STK_PUSH_M68_A5 5
#define DST$K_STK_PUSH_M68_A6 6
#define DST$K_STK_PUSH_M68_A7 7
#define DST$K_STK_PUSH_M68_D0 8
#define DST$K_STK_PUSH_M68_D1 9
#define DST$K_STK_PUSH_M68_D2 10
#define DST$K_STK_PUSH_M68_D3 11
#define DST$K_STK_PUSH_M68_D4 12
#define DST$K_STK_PUSH_M68_D5 13
#define DST$K_STK_PUSH_M68_D6 14
#define DST$K_STK_PUSH_M68_D7 15
#define DST$K_STK_PUSH_1750_R0 15
#define DST$K_STK_PUSH_1750_R1 14
#define DST$K_STK_PUSH_1750_R2 13
#define DST$K_STK_PUSH_1750_R3 12
#define DST$K_STK_PUSH_1750_R4 11
#define DST$K_STK_PUSH_1750_R5 10
#define DST$K_STK_PUSH_1750_R6 9
#define DST$K_STK_PUSH_1750_R7 8
#define DST$K_STK_PUSH_1750_R8 7
#define DST$K_STK_PUSH_1750_R9 6
#define DST$K_STK_PUSH_1750_R10 5
#define DST$K_STK_PUSH_1750_R11 4
#define DST$K_STK_PUSH_1750_R12 3
#define DST$K_STK_PUSH_1750_R13 2
#define DST$K_STK_PUSH_1750_R14 1
#define DST$K_STK_PUSH_1750_R15 0
#define DST$K_STK_PUSHIMB 16
#define DST$K_STK_PUSHIMW 17
#define DST$K_STK_PUSHIML 18
#define DST$K_STK_PUSHIM_VAR 24
#define DST$K_STK_PUSHIMBU 25
#define DST$K_STK_PUSHIMWU 26
#define DST$K_STK_PUSHINB 20
#define DST$K_STK_PUSHINW 21
#define DST$K_STK_PUSHINL 22
#define DST$K_STK_PUSHINBU 27
#define DST$K_STK_PUSHINWU 28
#define DST$K_STK_ADD 19
#define DST$K_STK_SUB 29
#define DST$K_STK_MULT 30
#define DST$K_STK_DIV 31
#define DST$K_STK_LSH 32
#define DST$K_STK_ROT 33
#define DST$K_STK_EXTV_IMED 45
#define DST$K_STK_EXTZV_IMED 46
#define DST$K_STK_EXTV_IND 47
#define DST$K_STK_EXTZV_IND 48
#define DST$K_STK_COP 34
#define DST$K_STK_EXCH 35
#define DST$K_STK_STO_B 36
#define DST$K_STK_STO_W 37
#define DST$K_STK_STO_L 38
#define DST$K_STK_FET_B 49
#define DST$K_STK_FET_W 50
#define DST$K_STK_FET_L 51
#define DST$K_STK_POP 39
#define DST$K_STK_STOP 23
#define DST$K_STK_RTNCALL 40
#define DST$K_STK_RTNCALL_ALT 41
#define DST$K_STK_RTN_NOFP 44
#define DST$K_STK_PUSH_OUTER_REC 42
#define DST$K_STK_PUSH_INNER_REC 43
#define DST$K_STK_POS 52
#define DST$K_STK_PUSH_VALSPEC 53
#define DST$K_STK_PUSH_INNER_ARRAY 54
#define DST$K_STK_HIGH 86
typedef struct _DST$XVS_SPEC {
    struct  {
        unsigned dst$v_xvs_indirect : 1;
        unsigned dst$v_xvs_disp : 1;
        unsigned dst$v_xvs_valkind : 2;
        unsigned dst$v_xvs_fill0 : 12;
        } dst$w_xvs_flags;
    unsigned short int dst$w_xvs_regnum;
    struct  {
        long int dst$l_xvs_low_value;
        long int dst$l_xvs_hi_value;
        } dst$q_xvs_value;
    } DST$XVS_SPEC;
#define DST$K_XVS_SPEC_SIZE 12
typedef struct _DST$TYPSPEC {
    DST$HEADER dst$a_typspec_header;
    unsigned char dst$b_typspec_name;
    } DST$TYPSPEC;
#define DST$K_TYPSPEC_SIZE 3
typedef unsigned char DST$TS_DTYPE;
#define DST$K_TS_ATOM 1
#define DST$K_TS_DSC 2
#define DST$K_TS_IND 3
#define DST$K_TS_TPTR 4
#define DST$K_TS_PTR 5
#define DST$K_TS_PIC 6
#define DST$K_TS_ARRAY 7
#define DST$K_TS_SET 8
#define DST$K_TS_SUBRANGE 9
#define DST$K_TS_ADA_DSC 10
#define DST$K_TS_FILE 11
#define DST$K_TS_AREA 12
#define DST$K_TS_OFFSET 13
#define DST$K_TS_NOV_LENG 14
#define DST$K_TS_IND_TSPEC 15
#define DST$K_TS_SELF_REL_LABEL 16
#define DST$K_TS_RFA 17
#define DST$K_TS_TASK 18
#define DST$K_TS_ADA_ARRAY 19
#define DST$K_TS_XMOD_IND 20
#define DST$K_TS_CONSTRAINED 21
#define DST$K_TS_MAYBE_CONSTR 22
#define DST$K_TS_DYN_NOV_LENG 23
#define DST$K_TS_TPTR_D 24
#define DST$K_TS_SCAN_TREE 25
#define DST$K_TS_SCAN_TREEPTR 26
#define DST$K_TS_INCOMPLETE 27
#define DST$K_TS_BLISS_BLOCK 28
#define DST$K_TS_TPTR_64 29
#define DST$K_TS_PTR_64 30
#define DST$K_TS_REF 31
#define DST$K_TS_REF_64 32
#define DST$K_TS_DTYPE_LOWEST 1
#define DST$K_TS_DTYPE_HIGHEST 32
typedef struct _DST$TYPE_SPEC {
    unsigned short int dst$w_ts_length;
    DST$TS_DTYPE dst$b_ts_kind;
    union  {
        struct  {
            unsigned char dst$b_ts_atom_typ;
            } dst$atomic_type;
        struct  {
            unsigned char dst$b_ts_ada_dsc_class;
            unsigned char dst$b_ts_ada_dsc_dtype;
            } dst$ada_descriptor_type;
        struct  {
            unsigned long int dst$l_ts_ind_ptr;
            } dst$indirect_type;
        struct  {
            unsigned long int dst$l_ts_xmod_offset;
            unsigned char dst$b_ts_xmod_modname;
            } dst$xmod_indirect_type;
        struct  {
            unsigned char dst$b_ts_pic_dleng;
            DBG$DST_LANGUAGE dst$b_ts_pic_lang;
            unsigned char dst$b_ts_pic_pleng;
            } dst$picture_type;
        struct  {
            unsigned char dst$b_ts_array_dim;
            } dst$array_type;
        struct  {
            unsigned char dst$b_ts_ada_array_dim;
            unsigned char dst$b_ts_ada_array_class;
            unsigned char dst$b_ts_ada_array_dtype;
            } dst$ada_array_type;
        struct  {
            unsigned long int dst$l_ts_set_leng;
            } dst$set_type;
        struct  {
            unsigned long int dst$l_ts_subr_leng;
            } dst$subrange_type;
        struct  {
            DBG$DST_LANGUAGE dst$b_ts_file_lang;
            } dst$file_type;
        struct  {
            unsigned long int dst$l_ts_nov_leng;
            unsigned long int dst$l_ts_nov_leng_par_tspec;
            } dst$novel_length_type;
        struct  {
            DST$VAL_SPEC dst$a_dyn_nov_val_spec;
            } dst$dynamic_novel_length_type;
        struct  {
            unsigned long int dst$l_ts_self_leng;
            } dst$self_relative_type;
        struct  {
            unsigned short int dst$wu_ts_task_entry_count;
            } dst$task_type;
        struct  {
            unsigned long int dst$l_ts_constr_record;
            unsigned long int dst$l_ts_constr_count;
            } dst$constrained_type;
        struct  {
            unsigned long int dst$l_ts_mightbe_record;
            } dst$might_be_constrained_type;
        struct  {
            unsigned char dst$b_ts_scan_tree_depth;
            } dst$scan_tree_type;
        struct  {
            unsigned long int dst$l_ts_incomplete_ptr;
            } dst$incomplete_type;
        struct  {
            unsigned long int dst$l_ts_number_units;
            unsigned char dst$b_ts_unit_size;
            unsigned char dst$b_ts_field_set_count;
            } dst$bliss_block_type;
        } dst$type_spec_variants;
    } DST$TYPE_SPEC;
#define DST$K_TYPE_SPEC_SIZE 11
#define DST$K_TS_ATOM_LENG 4
#define DST$K_TS_IND_LENG 7
#define DST$K_TS_PTR_LENG 3
#define DST$K_TS_PTR_64_LENG 3
#define DST$K_TS_FILE_LENG 4
#define DST$K_TS_AREA_LENG 3
#define DST$K_TS_OFFSET_LENG 3
#define DST$K_TS_NOV_LENG_LENG 11
#define DST$K_TS_TASK_LENG 3
#define DST$K_TS_INCOMPLETE_LENG 7
#define DST$K_TS_BASE 2
#define DST$K_TS_DSC_VSPEC 3
#define DST$K_TS_ADA_DSC_VSPEC 5
#define DST$K_TS_TPTR_TSPEC 3
#define DST$K_TS_TPTR_64_TSPEC 3
#define DST$K_TS_REF_TSPEC 3
#define DST$K_TS_REF_64_TSPEC 3
#define DST$K_TS_PIC_ADDR 6
#define DST$K_TS_ARRAY_FLAGS 4
#define DST$K_TS_ADA_ARRAY_FLAGS 6
#define DST$K_TS_SET_PAR_TSPEC 7
#define DST$K_TS_SUBR_PAR_TSPEC 7
#define DST$K_TS_FILE_RCRD_TYP 4
#define DST$K_TS_AREA_BYTE_LEN 3
#define DST$K_TS_OFFSET_VALSPEC 3
#define DST$K_TS_NOV_LENG_VSPEC 3
#define DST$K_TS_NOV_LENG_TSPEC 8
#define DST$K_TS_TASK_ENTRY 5
#define DST$K_TS_CONSTR_LIST 11
#define DST$K_TS_MIGHTBE_VALSPEC 7
#define DST$K_TS_SCAN_TREE_FLAGS 4
#define DST$K_TS_SCAN_TREEPTR_TREE 3
#define DST$K_TS_FIELD_SET_LIST 9
#define DBG$K_SET_SIZE_MAX 8192
#define DBG$K_PREDEF_SET_SIZE_MAX 32
typedef struct _DST$TASK_TS_ENTRY {
    struct  {
        unsigned dst$v_ts_task_entry_family : 1;
        unsigned dst$v_ts_task_entry_mbz : 7;
        } dst$bu_ts_task_entry_flags;
    union  {
        unsigned char dst$bu_ts_task_entry_name;
        unsigned char dst$bu_ts_task_entry_trlr_offs;
        } dst$bu_ts_task_fields;
    } DST$TASK_TS_ENTRY;
#define DST$K_TASK_TS_ENTRY_SIZE 2
typedef struct _DST$TASK_TS_FAMILY {
    DST$VAL_SPEC dst$a_ts_entry_family_lb;
    DST$VAL_SPEC dst$a_ts_entry_family_ub;
    } DST$TASK_TS_FAMILY;
#define DST$K_TASK_TS_ENTRY_FAMILY_SIZE 10
typedef struct _DST$ENUM_BEGIN {
    DST$HEADER dst$a_enumbeg_header;
    unsigned char dst$b_enumbeg_leng;
    unsigned char dst$b_enumbeg_name;
    } DST$ENUM_BEGIN;
#define DST$K_ENUM_BEGIN_SIZE 4
typedef struct _DST$ENUM_END {
    DST$HEADER dst$a_enum_end_header;
    } DST$ENUM_END;
#define DST$K_ENUM_END_SIZE 2
typedef struct _DST$BLISS_FIELD_BEGIN {
    DST$HEADER dst$a_blifldbeg_header;
    unsigned char dst$b_blifldbeg_name;
    } DST$BLISS_FIELD_BEGIN;
#define DST$K_BLISS_FIELD_BEGIN_SIZE 3
typedef struct _DST$BLISS_FIELD_END {
    DST$HEADER dst$a_blifld_end_header;
    } DST$BLISS_FIELD_END;
#define DST$K_BLISS_FIELD_END_SIZE 2
typedef struct _DST$BLISS_FIELD {
    DST$HEADER dst$a_blifld_header;
    unsigned char dst$b_blifld_unused;
    unsigned long int dst$l_blifld_comps;
    unsigned char dst$b_blifld_name;
    } DST$BLISS_FIELD;
#define DST$K_BLISS_FIELD_SIZE 8
#define DST$K_BLI_NOSTRUC 0
#define DST$K_BLI_VEC 1
#define DST$K_BLI_BITVEC 2
#define DST$K_BLI_BLOCK 3
#define DST$K_BLI_BLKVEC 4
#define DST$K_BLI_STRUC_MIN 0
#define DST$K_BLI_STRUC_MAX 4
typedef struct _DST$BLI_FIELDS {
    DST$HEADER dst$a_bli_fields_header;
    unsigned char dst$b_bli_lng;
    unsigned char dst$b_bli_formal;
    unsigned char dst$b_bli_vflags;
    struct  {
        unsigned dst$v_bli_struc : 3;
        unsigned dst$v_bli_mbz : 4;
        unsigned dst$v_bli_ref : 1;
        } dst$b_bli_sym_type;
    union  {
        struct  {
            unsigned long int dst$l_bli_vec_units;
            struct  {
                unsigned dst$v_bli_vec_unit_size : 4;
                unsigned dst$v_bli_vec_sign_ext : 4;
                } dst$v_bli_vec_flags;
            } dst$bli_struc_vector;
        struct  {
            unsigned long int dst$l_bli_bitvec_size;
            } dst$bli_struc_bitvector;
        struct  {
            unsigned long int dst$l_bli_block_units;
            struct  {
                unsigned dst$v_bli_block_unit_size : 4;
                unsigned dst$v_bli_block_mbz : 4;
                } dst$v_bli_block_flags;
            } dst$bli_struc_block;
        struct  {
            unsigned long int dst$l_bli_blkvec_blocks;
            unsigned long int dst$l_bli_blkvec_units;
            unsigned char dst$b_bli_blkvec_unit_size;
            } dst$bli_struc_blockvector;
        } dst$bli_struc_variants;
    } DST$BLI_FIELDS;
#define DST$K_BLI_FIELDS_SIZE 15
#define DST$K_BLI_TRLR1 3
#define DST$K_BLI_SYM_ATTR 6
typedef struct _DST$BLI_TRAILER1 {
    unsigned long int dst$l_bli_value;
    unsigned char dst$b_bli_name;
    } DST$BLI_TRAILER1;
#define DST$K_BLI_TRAILER1_SIZE 5
typedef struct _DST$BLI_TRAILER2 {
    unsigned long int dst$l_bli_size;
    } DST$BLI_TRAILER2;
#define DST$K_BLI_TRAILER2_SIZE 4
typedef struct _DST$IMAGE {
    DST$DATA_DST dst$a_image_header;
    } DST$IMAGE;
#define DST$K_IMAGE_SIZE 8
typedef struct _DST$PSECT {
    DST$HEADER dst$a_psect_header;
    unsigned char dst$b_psect_unused;
    unsigned long int dst$l_psect_value;
    union  {
        unsigned char dst$b_psect_name;
        unsigned char dst$b_psect_trlr_offs;
        } dst$a_psect_info;
    } DST$PSECT;
#define DST$K_PSECT_HEADER_SIZE 8
typedef struct _DST$PSECT_TRAILER {
    unsigned long int dst$l_psect_size;
    } DST$PSECT_TRAILER;
#define DST$K_PSECT_TRAILER_SIZE 4
#define DST$K_LABEL_SIZE 8
#define DST$K_LBL_OR_LIT_SIZE 8
typedef struct _DST$ENTRY {
    DST$HEADER dst$a_entry_dst_header;
    struct  {
        unsigned dst$v_entry_mbz : 8;
        } dst$b_entry_flags;
    int *dst$l_entry_address;
    unsigned char dst$b_entry_name;
    } DST$ENTRY;
#define DST$K_ENTRY_SIZE 8
typedef struct _DST$LINE_NUM_HEADER {
    DST$HEADER dst$a_line_num_header;
    } DST$LINE_NUM_HEADER;
#define DST$K_LINE_NUM_HEADER_SIZE 2
typedef struct _DST$PCLINE_COMMANDS {
    char dst$b_pcline_command;
    union  {
        unsigned long int dst$l_pcline_unslong;
        unsigned short int dst$w_pcline_unsword;
        unsigned char dst$b_pcline_unsbyte;
        } dst$a_pcline_access_fields;
    } DST$PCLINE_COMMANDS;
#define DST$K_PCLINE_COMMANDS_SIZE 5
#define DST$K_PCLINE_COMMANDS_SIZE_MIN 2
#define DST$K_PCLINE_COMMANDS_SIZE_MAX 5
#define DST$K_DELTA_PC_LOW -128
#define DST$K_DELTA_PC_HIGH 0
#define DST$K_DELTA_PC_W 1
#define DST$K_INCR_LINUM 2
#define DST$K_INCR_LINUM_W 3
#define DST$K_SET_LINUM_INCR 4
#define DST$K_SET_LINUM_INCR_W 5
#define DST$K_RESET_LINUM_INCR 6
#define DST$K_BEG_STMT_MODE 7
#define DST$K_END_STMT_MODE 8
#define DST$K_SET_LINUM 9
#define DST$K_SET_PC 10
#define DST$K_SET_PC_W 11
#define DST$K_SET_PC_L 12
#define DST$K_SET_STMTNUM 13
#define DST$K_TERM 14
#define DST$K_TERM_W 15
#define DST$K_SET_ABS_PC 16
#define DST$K_DELTA_PC_L 17
#define DST$K_INCR_LINUM_L 18
#define DST$K_SET_LINUM_B 19
#define DST$K_SET_LINUM_L 20
#define DST$K_TERM_L 21
#define DST$K_PCCOR_LOW -128
#define DST$K_PCCOR_HIGH 21
typedef struct _DST$SOURCE_CORR {
    DST$HEADER dst$a_source_corr_header;
    } DST$SOURCE_CORR;
#define DST$K_SOURCE_CORR_HEADER_SIZE 2
#define DST$K_SRC_DECLFILE 1
#define DST$K_SRC_SETFILE 2
#define DST$K_SRC_SETREC_L 3
#define DST$K_SRC_SETREC_W 4
#define DST$K_SRC_SETLNUM_L 5
#define DST$K_SRC_SETLNUM_W 6
#define DST$K_SRC_INCRLNUM_B 7
#define DST$K_SRC_UNUSED1 8
#define DST$K_SRC_UNUSED2 9
#define DST$K_SRC_DEFLINES_W 10
#define DST$K_SRC_DEFLINES_B 11
#define DST$K_SRC_UNUSED3 12
#define DST$K_SRC_UNUSED4 13
#define DST$K_SRC_UNUSED5 14
#define DST$K_SRC_UNUSED6 15
#define DST$K_SRC_FORMFEED 16
#define DST$K_SRC_MIN_CMD 1
#define DST$K_SRC_MAX_CMD 16
typedef struct _DST$SRC_COMMAND {
    unsigned char dst$b_src_command;
    union  {
        struct  {
            unsigned char dst$b_src_df_length;
            unsigned char dst$b_src_df_flags;
            unsigned short int dst$w_src_df_fileid;
            unsigned int dst$q_src_df_rms_cdt [2];
            unsigned long int dst$l_src_df_rms_ebk;
            unsigned short int dst$w_src_df_rms_ffb;
            unsigned char dst$b_src_df_rms_rfo;
            unsigned char dst$b_src_df_filename;
            } dst$a_src_decl_src;
        unsigned long int dst$l_src_unslong;
        unsigned short int dst$w_src_unsword;
        unsigned char dst$b_src_unsbyte;
        } dst$a_src_cmd_fields;
    } DST$SRC_COMMAND;
#define DST$K_SRC_COMMAND_SIZE 21
#define DST$K_SRC_DF_FILENAME_BASE 21
typedef struct _DST$SRC_CMDTRLR {
    unsigned char dst$b_src_df_libmodname;
    } DST$SRC_CMDTRLR;
#define DST$K_SRC_CMDTRLR_SIZE 1
typedef struct _DST$GEM_LOC_HEADER {
    DST$HEADER dst$a_gem_loc_header;
    } DST$GEM_LOC_HEADER;
#define DST$K_GEM_LOC_HEADER_SIZE 2
typedef struct _DST$PCLOC_COMMANDS {
    char dst$b_pcloc_command;
    union  {
        struct  {
            unsigned short int dst$w_pcloc_pnts_line;
            unsigned char dst$b_pcloc_pnts_column;
            } dst$a_pcloc_pnts;
        struct  {
            unsigned long int dst$l_pcloc_pntl_line;
            unsigned short int dst$w_pcloc_pntl_column;
            } dst$a_pcloc_pntl;
        struct  {
            unsigned short int dst$w_pcloc_rngs_low_line;
            unsigned char dst$b_pcloc_rngs_low_column;
            unsigned short int dst$w_pcloc_rngs_high_line;
            unsigned char dst$b_pcloc_rngs_high_column;
            } dst$a_pcloc_rngs;
        struct  {
            unsigned long int dst$l_pcloc_rngl_low_line;
            unsigned short int dst$w_pcloc_rngl_low_column;
            unsigned long int dst$l_pcloc_rngl_high_line;
            unsigned short int dst$w_pcloc_rngl_high_column;
            } dst$a_pcloc_rngl;
        struct  {
            unsigned short int dst$w_pcloc_lins_line;
            unsigned char dst$b_pcloc_lins_low_column;
            unsigned char dst$b_pcloc_lins_high_column;
            } dst$a_pcloc_lins;
        struct  {
            unsigned int dst$q_pcloc_setpc64_value [2];
            } dst$a_pcloc_setpc64;
        struct  {
            unsigned long int dst$l_pcloc_setpc32_value;
            } dst$a_pcloc_setpc32;
        struct  {
            unsigned long int dst$l_pcloc_event_read_sym1;
            unsigned long int dst$l_pcloc_event_read_sym2;
            } dst$a_pcloc_event_read;
        struct  {
            unsigned long int dst$l_pcloc_event_write_sym;
            } dst$a_pcloc_event_write;
        } dst$pcloc_subcommand_args;
    } DST$PCLOC_COMMANDS;
#define DST$K_PCLOC_CMD_SIZE_END 1
#define DST$K_PCLOC_CMD_SIZE_PNTS 4
#define DST$K_PCLOC_CMD_SIZE_PNTL 7
#define DST$K_PCLOC_CMD_SIZE_RNGS 7
#define DST$K_PCLOC_CMD_SIZE_RNGL 13
#define DST$K_PCLOC_CMD_SIZE_LINS 5
#define DST$K_PCLOC_CMD_SIZE_INCR 1
#define DST$K_PCLOC_CMD_SIZE_SETPC64 9
#define DST$K_PCLOC_CMD_SIZE_SETPC32 5
#define DST$K_PCLOC_CMD_SIZE_EVENT 1
#define DST$K_PCLOC_CMD_SIZE_EVENT_RD 9
#define DST$K_PCLOC_CMD_SIZE_EVENT_WR 5
#define DST$K_PCLOC_END 0
#define DST$K_PCLOC_PNTS_INCR 1
#define DST$K_PCLOC_PNTL_INCR 2
#define DST$K_PCLOC_RNGS_INCR 3
#define DST$K_PCLOC_RNGL_INCR 4
#define DST$K_PCLOC_PNTS 5
#define DST$K_PCLOC_PNTL 6
#define DST$K_PCLOC_RNGS 7
#define DST$K_PCLOC_RNGL 8
#define DST$K_PCLOC_INCR 9
#define DST$K_PCLOC_SETPC64 10
#define DST$K_PCLOC_SETPC32 11
#define DST$K_PCLOC_EVENT_INST 12
#define DST$K_PCLOC_EVENT_READ 13
#define DST$K_PCLOC_EVENT_WRITE 14
#define DST$K_PCLOC_EVENT_CTRL 15
#define DST$K_PCLOC_EVENT_CALL 16
#define DST$K_PCLOC_LINS 17
#define DST$K_PCLOC_LINS_INCR 18
#define DST$K_PCLOC_EVENT_PWRIT 19
#define DST$K_PCLOC_EVENT_LABEL 20
#define DST$K_PCLOC_LOW 0
#define DST$K_PCLOC_HIGH 20
typedef struct _DST$CONTINUATION {
    DST$HEADER dst$a_continuation_header;
    } DST$CONTINUATION;
#define DST$K_CONTINUATION_HEADER_SIZE 2
typedef struct _DST$GOTO {
    DST$HEADER dst$a_goto_header;
    unsigned long int dst$l_goto_ptr;
    } DST$GOTO;
#define DST$K_GOTO_SIZE 6
typedef struct _DST$OVERLOAD_HEADER {
    DST$HEADER dst$a_ol_header;
    unsigned char dst$b_ol_name;
    } DST$OVERLOAD_HEADER;
#define DST$K_OVERLOAD_HEADER_SIZE 3
typedef struct _DST$OVERLOAD_TRLR {
    unsigned short int dst$w_ol_count;
    } DST$OVERLOAD_TRLR;
#define DST$K_OVERLOAD_VECTOR_BASE 2
typedef struct _DST$INLINED_HEADER {
    DST$HEADER dst$a_il_header;
    unsigned char dst$b_il_name;
    } DST$INLINED_HEADER;
#define DST$K_INLINED_HEADER_SIZE 3
typedef struct _DST$INLINE_TRLR {
    unsigned short int dst$w_il_count;
    } DST$INLINE_TRLR;
#define DST$K_INLINE_VECTOR_BASE 2
typedef struct _DST$SUBUNIT {
    DST$HEADER dst$a_subunit_header;
    unsigned char dst$b_subunit_pathname_count;
    } DST$SUBUNIT;
#define DST$K_SUBUNIT_SIZE 3
typedef struct _DST$SET_MODULE {
    DST$HEADER dst$a_set_module_header;
    unsigned char dst$b_set_module_name;
    } DST$SET_MODULE;
#define DST$K_SET_MODULE_SIZE 3
typedef struct _DST$USE_CLAUSE {
    DST$HEADER dst$a_use_header;
    unsigned char dst$b_use_pathname_count;
    } DST$USE_CLAUSE;
#define DST$K_USE_CLAUSE_SIZE 3
typedef struct _DST$REAL_NAME {
    DST$HEADER dst$a_real_name_header;
    unsigned char dst$b_real_name;
    } DST$REAL_NAME;
#define DST$K_REAL_NAME_SIZE 3
typedef struct _DST$BODY_SPEC {
    DST$HEADER dst$a_body_spec_header;
    unsigned char dst$b_body_spec_pathname_count;
    } DST$BODY_SPEC;
#define DST$K_BODY_SPEC_SIZE 3
typedef struct _DST$ALIAS {
    DST$HEADER dst$a_alias_header;
    unsigned long int dst$l_alias_mod_offset;
    unsigned char dst$b_alias_name;
    } DST$ALIAS;
#define DST$K_ALIAS_SIZE 7
typedef struct _DST$FULFILLS_TYPE {
    DST$HEADER dst$a_fulfills_header;
    } DST$FULFILLS_TYPE;
#define DST$K_FULFILLS_TYPE_SIZE 2
typedef struct _DST$DEF_LNUM {
    DST$HEADER dst$a_def_lnum_header;
    unsigned char dst$b_def_lnum_mbz;
    unsigned long int dst$l_def_lnum_line;
    } DST$DEF_LNUM;
#define DST$K_DEF_LNUM_SIZE 7
typedef struct _DST$STATLINK {
    DST$HEADER dst$a_sl_header;
    } DST$STATLINK;
#define DST$K_STATLINK_SIZE 2
typedef struct _DST$PROLOG {
    DST$HEADER dst$a_prolog_header;
    unsigned long int dst$l_prolog_bkpt_addr;
    } DST$PROLOG;
#define DST$K_PROLOG_SIZE 6
typedef struct _DST$PROLIST {
    DST$HEADER dst$w_prolist_header;
    unsigned long int dst$lu_prolist_count;
    } DST$PROLIST;
#define DST$K_PROLIST_SIZE 6
typedef struct _DST$PROLIST_ENTRY {
    unsigned long int dst$lu_prolist_bkpt_addr;
    } DST$PROLIST_ENTRY;
#define DST$K_PROLIST_ENTRY_SIZE 4
typedef struct _DST$EPILOG {
    DST$HEADER dst$w_epilog_header;
    struct  {
        unsigned dst$v_epilog_addr_pairs_flag : 1;
        unsigned dst$v_epilog_mbz : 7;
        } dst$b_epilog_flags;
    unsigned long int dst$lu_epilog_count;
    } DST$EPILOG;
#define DST$K_EPILOG_SIZE 7
typedef struct _DST$EPI_SINGLETON {
    unsigned long int dst$lu_epilog_single_addr;
    } DST$EPI_SINGLETON;
#define DST$K_EPILOG_SINGLETON_SIZE 4
typedef struct _DST$EPI_PAIR {
    unsigned long int dst$lu_epilog_pair_low_addr;
    unsigned long int dst$lu_epilog_pair_high_addr;
    } DST$EPI_PAIR;
#define DST$K_EPILOG_PAIR_SIZE 8
typedef struct _DST$RETURN {
    DST$HEADER dst$w_return_header;
    unsigned char dst$b_return_mbz;
    unsigned long int dst$lu_return_count;
    } DST$RETURN;
#define DST$K_RETURN_SIZE 7
typedef struct _DST$VERSION {
    DST$HEADER dst$a_version_header;
    unsigned char dst$b_version_major;
    unsigned char dst$b_version_minor;
    } DST$VERSION;
#define DST$K_VERSION_SIZE 4
typedef struct _DST$REG_SAVE_BEGIN {
    DST$HEADER dst$w_regbeg_header;
    struct  {
        unsigned dst$v_regbeg_save_mask_flag : 1;
        unsigned dst$v_regbeg_mbz : 7;
        } dst$bu_regbeg_flags;
    unsigned char dst$bu_regbeg_save_mask_length;
    } DST$REG_SAVE_BEGIN;
#define DST$K_REGISTER_SAVE_BEGIN_SIZE 4
typedef struct _DST$REGISTER_SAVE {
    DST$HEADER dst$w_reg_save_header;
    DST$VAL_SPEC dst$a_reg_save_valspec;
    unsigned char dst$bu_reg_save_regnum;
    } DST$REGISTER_SAVE;
#define DST$K_REGISTER_SAVE_SIZE 8
typedef struct _DST$REGISTER_SAVE_END {
    DST$HEADER dst$w_regend_header;
    } DST$REGISTER_SAVE_END;
#define DST$K_REGISTER_SAVE_END_SIZE 2
typedef struct _DST$EXCEPTION {
    DST$HEADER dst$w_exception_header;
    struct  {
        unsigned dst$v_exception_mod_name_flag : 1;
        unsigned dst$v_exception_mbz : 7;
        } dst$bu_exception_flags;
    unsigned long int dst$lu_exception_value;
    union  {
        unsigned char dst$b_exception_name;
        unsigned char dst$b_exception_trlr_offs;
        } dst$a_exception_variant;
    } DST$EXCEPTION;
#define DST$K_EXCEPTION_SIZE 8
typedef struct _DST$EXCEPTION_TRAILER {
    unsigned char dst$b_exception_mod_name;
    } DST$EXCEPTION_TRAILER;
#define DST$K_EXCEPTION_TRAILER_SIZE 1
typedef struct _DST$BASE_CLASS {
    DST$HEADER dst$w_base_class_header;
    struct  {
        unsigned dst$v_base_class_virtual : 1;
        unsigned dst$v_base_class_unused : 7;
        } dst$b_base_class_flags;
    unsigned long int dst$l_base_class_dst;
    unsigned long int dst$l_base_class_value;
    } DST$BASE_CLASS;
#define DST$K_BASE_CLASS_SIZE 11
typedef struct _DST$VIRT_FUNC {
    DST$HEADER dst$w_virt_func_header;
    struct  {
        unsigned dst$v_virt_func_unused : 8;
        } dst$b_virt_func_flags;
    unsigned long int dst$l_virt_func_index;
    } DST$VIRT_FUNC;
#define DST$K_VIRT_FUNC_SIZE 7
typedef struct _DST$TYPE_SIG {
    DST$HEADER dst$a_type_sig_header;
    struct  {
        unsigned dst$v_type_sig_unused : 8;
        } dst$b_type_sig_flags;
    unsigned char dst$b_type_sig_string;
    } DST$TYPE_SIG;
#define DST$K_TYPE_SIG_SIZE 4
typedef struct _DST$CXX_ATTRIBUTES {
    DST$HEADER dst$w_cxxa_header;
    struct  {
        unsigned dst$v_cxxa_unused1 : 1;
        unsigned dst$v_cxxa_unused2 : 1;
        unsigned dst$v_cxxa_unused3 : 1;
        unsigned dst$v_cxxa_namespace : 1;
        unsigned dst$v_cxxa_struct : 1;
        unsigned dst$v_cxxa_union : 1;
        unsigned dst$v_cxxa_static : 1;
        unsigned dst$v_cxxa_unused : 9;
        } dst$w_cxxa_flags;
    } DST$CXX_ATTRIBUTES;
#define DST$K_CXX_ATTRIBUTES_SIZE 4
typedef struct _DST$TEMP_DECL {
    DST$HEADER dst$w_temp_decl_header;
    unsigned char dst$b_temp_decl_name;
    } DST$TEMP_DECL;
#define DST$K_TEMP_DECL_SIZE 3
typedef struct _DST$USING {
    DST$HEADER dst$w_using_header;
    unsigned long int dst$l_using_dst;
    } DST$USING;
#define DST$K_USING_SIZE 6
typedef struct _DST$COB_HACK {
    DST$DATA_DST dst$a_cobhack_fields;
    } DST$COB_HACK;
#define DST$K_COB_HACK_SIZE 8
typedef struct _DST$CH_TRLR {
    unsigned char dst$b_ch_type;
    } DST$CH_TRLR;
#define DST$K_CH_TRLR_SIZE 1
typedef struct _DST$VALSPEC {
    DST$HEADER dst$a_valspec_header;
    } DST$VALSPEC;
#define DST$K_VALSPEC_SIZE 2
#endif /* DSTRECRDS_H */
