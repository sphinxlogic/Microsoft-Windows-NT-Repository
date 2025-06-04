/*  CALLMON - A Call Monitor for OpenVMS Alpha
 *
 *  File:     CALLMON$ALPHACODE.C
 *  Author:   Thierry Lelegard
 *  Version:  1.0
 *  Date:     24-JUL-1996
 *
 *  Abstract: This module contains utility routines to analyze
 *            Alpha machine code.
 */


#include "callmon$private.h"


/*
 *  The Alpha architecture introduced a number of new instructions
 *  (like byte access instructions) which not defined in older
 *  header files.
 */

#ifndef EVX$OPC_STB

#define EVX$OPC_LDBU       10
#define EVX$OPC_LDWU       12
#define EVX$OPC_STW        13
#define EVX$OPC_STB        14
#define EVX$OPC_EXTI       28

#define EVX$EXTI_SEXTB      0
#define EVX$EXTI_SEXTW      1

#define EVX$LOGI_AMASK     97
#define EVX$LOGI_IMPLVER  108

#endif


/*******************************************************************************
 *
 *  Description of Alpha instructions.
 */

typedef enum {
    IT_RESERVED, IT_PAL, IT_SYNC, IT_OPER, IT_MEMORY, IT_JUMP, IT_BRANCH,
    IT_FP, IT_FPAUX
} inst_type_t;

typedef struct {
    int   function;
    char* name;
} inst_name_t;

typedef struct {
    int    function;
    char*  name;
    uint32 read_regs;
    uint32 write_regs;
} inst_pal_t;

typedef struct {
    int   function;
    char* name;
    int   use_ra;
    int   use_rb;
} inst_sync_t;

typedef struct {
    int   function;
    char* name;
    int   use_fa;
} inst_fpaux_t;

typedef struct {
    int          opcode;
    char*        name;
    inst_type_t  type;
    int          float_inst;
    int          is_store;
    void*        name_list;
} inst_desc_t;

static char* fp_inst_name [] = {
    "ADD%s",   "SUB%s",   "MUL%s",   "DIV%s",
    "CMP%sUN", "CMP%sEQ", "CMP%sLT", "CMP%sLE",
    "fp",      "fp",      "fp",      "fp",
    "CVT%s%s", "CVT%s%s", "CVT%s%s", "CVT%s%s",
};

static char* fp_ieee_name [] = {"S", "", "T", "Q"};
static char* fp_dec_name [] = {"F", "D", "G", "Q"};
static char* fp_trap_name [] = {"", "U", "2", "3", "S", "SU", "6", "SUI"};
static char* fp_round_name [] = {"C", "M", "", "D"};

static inst_fpaux_t fpaux_opcodes [] = {
    {EVX$FPAUX_CPYSEE,   "CPYSEE",   1},
    {EVX$FPAUX_CPYS,     "CPYS",     1},
    {EVX$FPAUX_CPYSN,    "CPYSN",    1},
    {EVX$FPAUX_CPYSE,    "CPYSE",    1},
    {EVX$FPAUX_WR_FPCTL, "MT_FPCR",  1},
    {EVX$FPAUX_RD_FPCTL, "MF_FPCR",  1},
    {EVX$FPAUX_FCMOVEQ,  "FCMOVEQ",  1},
    {EVX$FPAUX_FCMOVNE,  "FCMOVNE",  1},
    {EVX$FPAUX_FCMOVLT,  "FCMOVLT",  1},
    {EVX$FPAUX_FCMOVGE,  "FCMOVGE",  1},
    {EVX$FPAUX_FCMOVLE,  "FCMOVLE",  1},
    {EVX$FPAUX_FCMOVGT,  "FCMOVGT",  1},
    {EVX$FPAUX_CVTLQ,    "CVTLQ",    0},
    {EVX$FPAUX_CVTQL,    "CVTQL",    0},
    {EVX$FPAUX_CVTQLV,   "CVTQL/V",  0},
    {EVX$FPAUX_CVTQLSV , "CVTQL/SV", 0},
    {0,                  NULL,       0}
};

static inst_sync_t sync_opcodes [] = {
    {EVX$SYNC_TRAPB,   "TRAPB",   0, 0},
    {EVX$SYNC_EXCB,    "EXCB",    0, 0},
    {EVX$SYNC_MB,      "MB",      0, 0},
    {EVX$SYNC_WMB,     "WMB",     0, 0},
    {EVX$SYNC_MB8,     "MB8",     0, 0},
    {EVX$SYNC_MBC,     "MBC",     0, 0},
    {EVX$SYNC_FETCH,   "FETCH",   0, 1},
    {EVX$SYNC_FETCH_M, "FETCH_M", 0, 1},
    {EVX$SYNC_RCC,     "RCC",     1, 0},
    {EVX$SYNC_RC,      "RC",      1, 0},
    {EVX$SYNC_RS,      "RS",      1, 0},
    {0,                NULL,      0, 0}
};

static inst_name_t addi_names [] = {
    {EVX$ADDI_ADDL,     "ADDL"},
    {EVX$ADDI_ADDLV,    "ADDL/V"},
    {EVX$ADDI_ADDQ,     "ADDQ"},
    {EVX$ADDI_ADDQV,    "ADDQ/V"},
    {EVX$ADDI_S4ADDL,   "S4ADDL"},
    {EVX$ADDI_S4ADDQ,   "S4ADDQ"},
    {EVX$ADDI_S8ADDL,   "S8ADDL"},
    {EVX$ADDI_S8ADDQ,   "S8ADDQ"},
    {EVX$ADDI_S4SUBL,   "S4SUBL"},
    {EVX$ADDI_S4SUBQ,   "S4SUBQ"},
    {EVX$ADDI_S8SUBL,   "S8SUBL"},
    {EVX$ADDI_S8SUBQ,   "S8SUBQ"},
    {EVX$ADDI_SUBL,     "SUBL"},
    {EVX$ADDI_SUBLV,    "SUBL/V"},
    {EVX$ADDI_SUBQ,     "SUBQ"},
    {EVX$ADDI_SUBQV,    "SUBQ/V"},
    {EVX$ADDI_CMPEQ,    "CMPEQ"},
    {EVX$ADDI_CMPLT,    "CMPLT"},
    {EVX$ADDI_CMPLE,    "CMPLE"},
    {EVX$ADDI_CMPULT,   "CMPULT"},
    {EVX$ADDI_CMPULE,   "CMPULE"},
    {EVX$ADDI_CMPBGE,   "CMPBGE"},
    {0,                 NULL}
};

static inst_name_t exti_names [] = {
    {EVX$EXTI_SEXTB, "SEXTB"},
    {EVX$EXTI_SEXTW, "SEXTW"},
    {0,              NULL}
};

static inst_name_t logi_names [] = {
    {EVX$LOGI_AND,     "AND"},
    {EVX$LOGI_BIC,     "BIC"},
    {EVX$LOGI_CMOVEQ,  "CMOVEQ"},
    {EVX$LOGI_CMOVNE,  "CMOVNE"},
    {EVX$LOGI_CMOVLBS, "CMOVLBS"},
    {EVX$LOGI_BIS,     "BIS"},
    {EVX$LOGI_ORNOT,   "ORNOT"},
    {EVX$LOGI_CMOVLT,  "CMOVLT"},
    {EVX$LOGI_CMOVGE,  "CMOVGE"},
    {EVX$LOGI_CMOVLBC, "CMOVLBC"},
    {EVX$LOGI_XOR,     "XOR"},
    {EVX$LOGI_EQV,     "EQV"},
    {EVX$LOGI_CMOVLE,  "CMOVLE"},
    {EVX$LOGI_CMOVGT,  "CMOVGT"},
    {EVX$LOGI_AMASK,   "AMASK"},
    {EVX$LOGI_IMPLVER, "IMPLVER"},
    {0,                NULL}
};

static inst_name_t shfi_names [] = {
    {EVX$SHFI_SLL,    "SLL"},
    {EVX$SHFI_SRA,    "SRA"},
    {EVX$SHFI_SRL,    "SRL"},
    {EVX$SHFI_EXTBL,  "EXTBL"},
    {EVX$SHFI_EXTWL,  "EXTWL"},
    {EVX$SHFI_EXTLL,  "EXTLL"},
    {EVX$SHFI_EXTQL,  "EXTQL"},
    {EVX$SHFI_EXTWH,  "EXTWH"},
    {EVX$SHFI_EXTLH,  "EXTLH"},
    {EVX$SHFI_EXTQH,  "EXTQH"},
    {EVX$SHFI_INSBL,  "INSBL"},
    {EVX$SHFI_INSWL,  "INSWL"},
    {EVX$SHFI_INSLL,  "INSLL"},
    {EVX$SHFI_INSQL,  "INSQL"},
    {EVX$SHFI_INSWH,  "INSWH"},
    {EVX$SHFI_INSLH,  "INSLH"},
    {EVX$SHFI_INSQH,  "INSQH"},
    {EVX$SHFI_MSKBL,  "MSKBL"},
    {EVX$SHFI_MSKWL,  "MSKWL"},
    {EVX$SHFI_MSKLL,  "MSKLL"},
    {EVX$SHFI_MSKQL,  "MSKQL"},
    {EVX$SHFI_MSKWH,  "MSKWH"},
    {EVX$SHFI_MSKLH,  "MSKLH"},
    {EVX$SHFI_MSKQH,  "MSKQH"},
    {EVX$SHFI_ZAP,    "ZAP"},
    {EVX$SHFI_ZAPNOT, "ZAPNOT"},
    {0,               NULL}
};

static inst_name_t muli_names [] = {
    {EVX$MULI_MULL,  "MULL"},
    {EVX$MULI_MULQV, "MULQ/V"},
    {EVX$MULI_MULLV, "MULL/V"},
    {EVX$MULI_UMULH, "UMULH"},
    {EVX$MULI_MULQ,  "MULQ"},
    {0,              NULL}
};

static inst_pal_t pal_names [] = {
    {EVX$PAL_HALT,           "HALT",         0x00000000, 0x00000000},
    {EVX$PAL_CFLUSH,         "CFLUSH",       0x00000000, 0x00000000},
    {EVX$PAL_DRAINA,         "DRAINA",       0x00000000, 0x00000000},
    {EVX$PAL_LDQP,           "LDQP",         0x00010000, 0x00000001},
    {EVX$PAL_STQP,           "STQP",         0x00030000, 0x00000000},
    {EVX$PAL_SWPCTX,         "SWPCTX",       0x00010000, 0x00000000},
    {EVX$PAL_MFPR_ASN,       "MFPR_ASN"  ,   0x00000000, 0x00030003},
    {EVX$PAL_MTPR_ASTEN,     "MTPR_ASTEN",   0x00030000, 0x00030003},
    {EVX$PAL_MTPR_ASTSR,     "MTPR_ASTSR",   0x00030000, 0x00030003},
    {EVX$PAL_CSERVE,         "CSERVE",       0x00000000, 0x00000000},
    {EVX$PAL_SWPPAL,         "SWPPAL",       0x003F0000, 0x003F0001},
    {EVX$PAL_MFPR_FEN,       "MFPR_FEN",     0x00000000, 0x00030003},
    {EVX$PAL_MTPR_FEN,       "MTPR_FEN",     0x00030000, 0x00030003},
    {EVX$PAL_MTPR_IPIR,      "MTPR_IPIR" ,   0x00030000, 0x00030003},
    {EVX$PAL_MFPR_IPL,       "MFPR_IPL",     0x00000000, 0x00030003},
    {EVX$PAL_MTPR_IPL,       "MTPR_IPL",     0x00030000, 0x00030003},
    {EVX$PAL_MFPR_MCES,      "MFPR_MCES",    0x00000000, 0x00030003},
    {EVX$PAL_MTPR_MCES,      "MTPR_MCES",    0x00030000, 0x00030003},
    {EVX$PAL_MFPR_PCBB,      "MFPR_PCBB",    0x00000000, 0x00030003},
    {EVX$PAL_MFPR_PRBR,      "MFPR_PRBR",    0x00000000, 0x00030003},
    {EVX$PAL_MTPR_PRBR,      "MTPR_PRBR",    0x00030000, 0x00030003},
    {EVX$PAL_MFPR_PTBR,      "MFPR_PTBR",    0x00000000, 0x00030003},
    {EVX$PAL_MFPR_SCBB,      "MFPR_SCBB",    0x00000000, 0x00030003},
    {EVX$PAL_MTPR_SCBB,      "MTPR_SCBB",    0x00030000, 0x00030003},
    {EVX$PAL_MTPR_SIRR,      "MTPR_SIRR",    0x00030000, 0x00030003},
    {EVX$PAL_MFPR_SISR,      "MFPR_SISR",    0x00000000, 0x00030003},
    {EVX$PAL_MFPR_TBCHK,     "MFPR_TBCHK",   0x00000000, 0x00030003},
    {EVX$PAL_MTPR_TBIA,      "MTPR_TBIA",    0x00030000, 0x00030003},
    {EVX$PAL_MTPR_TBIAP,     "MTPR_TBIAP",   0x00030000, 0x00030003},
    {EVX$PAL_MTPR_TBIS,      "MTPR_TBIS",    0x00030000, 0x00030003},
    {EVX$PAL_MFPR_ESP,       "MFPR_ESP",     0x00000000, 0x00030003},
    {EVX$PAL_MTPR_ESP,       "MTPR_ESP",     0x00030000, 0x00030003},
    {EVX$PAL_MFPR_SSP,       "MFPR_SSP",     0x00000000, 0x00030003},
    {EVX$PAL_MTPR_SSP,       "MTPR_SSP",     0x00030000, 0x00030003},
    {EVX$PAL_MFPR_USP,       "MFPR_USP",     0x00000000, 0x00030003},
    {EVX$PAL_MTPR_USP,       "MTPR_USP",     0x00030000, 0x00030003},
    {EVX$PAL_MTPR_TBISD,     "MTPR_TBISD",   0x00030000, 0x00030003},
    {EVX$PAL_MTPR_TBISI,     "MTPR_TBISI",   0x00030000, 0x00030003},
    {EVX$PAL_MFPR_ASTEN,     "MFPR_ASTEN",   0x00000000, 0x00030003},
    {EVX$PAL_MFPR_ASTSR,     "MFPR_ASTSR",   0x00000000, 0x00030003},
    {EVX$PAL_VMS2OSF,        "VMS2OSF",      0x00000000, 0x00000000},
    {EVX$PAL_MFPR_VPTBASE,   "MFPR_VPTBASE", 0x00000000, 0x00030003},
    {EVX$PAL_MTPR_VPTBASE,   "MTPR_VPTBASE", 0x00030000, 0x00030003},
    {EVX$PAL_MTPR_PERFMON,   "MTPR_PERFMON", 0x00030000, 0x00030003},
    {EVX$PAL_WRVPTPTR,       "WRVPTPTR",     0x00000000, 0x00000000},
    {EVX$PAL_MTPR_DATAFX,    "MTPR_DATAFX",  0x00030000, 0x00030003},
    {EVX$PAL_OSFSWPCTX,      "OSFSWPCTX",    0x00000000, 0x00000000},
    {EVX$PAL_WRVAL,          "WRVAL",        0x00000000, 0x00000000},
    {EVX$PAL_RDVAL,          "RDVAL",        0x00000000, 0x00000000},
    {EVX$PAL_TBI,            "TBI",          0x00000000, 0x00000000},
    {EVX$PAL_WRENT,          "WRENT",        0x00000000, 0x00000000},
    {EVX$PAL_SWPIPL,         "SWPIPL",       0x00000000, 0x00000000},
    {EVX$PAL_RDPS,           "RDPS",         0x00000000, 0x00000000},
    {EVX$PAL_WRKGP,          "WRKGP",        0x00000000, 0x00000000},
    {EVX$PAL_WRUSP,          "WRUSP",        0x00000000, 0x00000000},
    {EVX$PAL_WRPERFMON,      "WRPERFMON",    0x00000000, 0x00000000},
    {EVX$PAL_RDUSP,          "RDUSP",        0x00000000, 0x00000000},
    {EVX$PAL_WHAMI,          "WHAMI",        0x00000000, 0x00000000},
    {EVX$PAL_RETSYS,         "RETSYS",       0x00000000, 0x00000000},
    {EVX$PAL_MFPR_WHAMI,     "MFPR_WHAMI",   0x00000000, 0x00030003},
    {EVX$PAL_RESERVED_GETSS, "GETSS",        0x00000000, 0x00000000},
    {EVX$PAL_RESERVED_PUTSS, "PUTSS",        0x00000000, 0x00000000},
    {EVX$PAL_RESERVED_RLS1,  "RLS1",         0x00000000, 0x00000000},
    {EVX$PAL_RESERVED_RLS2,  "RLS2",         0x00000000, 0x00000000},
    {EVX$PAL_RESERVED_DVT1,  "DVT1",         0x00000000, 0x00000000},
    {EVX$PAL_BPT,            "BPT",          0x00000000, 0x00000000},
    {EVX$PAL_BUGCHK,         "BUGCHK"  ,     0x00000000, 0x00000000},
    {EVX$PAL_CHME,           "CHME",         0x000000FC, 0x09C00000},
    {EVX$PAL_CHMK,           "CHMK",         0x000000FC, 0x09C00000},
    {EVX$PAL_CHMS,           "CHMS",         0x000000FC, 0x00000000},
    {EVX$PAL_CHMU,           "CHMU",         0x000000FC, 0x00000000},
    {EVX$PAL_IMB,            "IMB",          0x00000000, 0x00000000},
    {EVX$PAL_INSQHIL,        "INSQHIL",      0x00030000, 0x00000001},
    {EVX$PAL_INSQTIL,        "INSQTIL",      0x00030000, 0x00000001},
    {EVX$PAL_INSQHIQ,        "INSQHIQ",      0x00030000, 0x00000001},
    {EVX$PAL_INSQTIQ,        "INSQTIQ",      0x00030000, 0x00000001},
    {EVX$PAL_INSQUEL,        "INSQUEL",      0x00030000, 0x00000001},
    {EVX$PAL_INSQUEQ,        "INSQUEQ",      0x00030000, 0x00000001},
    {EVX$PAL_INSQUELD,       "INSQUELD",     0x00030000, 0x00000001},
    {EVX$PAL_INSQUEQD,       "INSQUEQD",     0x00030000, 0x00000001},
    {EVX$PAL_PROBER,         "PROBER",       0x00070000, 0x00000001},
    {EVX$PAL_PROBEW,         "PROBEW",       0x00070000, 0x00000001},
    {EVX$PAL_RD_PS,          "RD_PS",        0x00000000, 0x00000001},
    {EVX$PAL_REI,            "REI",          0x00000000, 0x000000FC},
    {EVX$PAL_REMQHIL,        "REMQHIL",      0x00010000, 0x00000003},
    {EVX$PAL_REMQTIL,        "REMQTIL",      0x00010000, 0x00000003},
    {EVX$PAL_REMQHIQ,        "REMQHIQ",      0x00010000, 0x00000003},
    {EVX$PAL_REMQTIQ,        "REMQTIQ",      0x00010000, 0x00000003},
    {EVX$PAL_REMQUEL,        "REMQUEL",      0x00010000, 0x00000003},
    {EVX$PAL_REMQUEQ,        "REMQUEQ",      0x00010000, 0x00000003},
    {EVX$PAL_REMQUELD,       "REMQUELD",     0x00010000, 0x00000003},
    {EVX$PAL_REMQUEQD,       "REMQUEQD",     0x00010000, 0x00000003},
    {EVX$PAL_SWASTEN,        "SWASTEN",      0x00010000, 0x00000001},
    {EVX$PAL_WR_PS_SW,       "WR_PS_SW",     0x00010000, 0x00000000},
    {EVX$PAL_RSCC,           "RSCC",         0x00000000, 0x00000001},
    {EVX$PAL_RDUNIQUE,       "RDUNIQUE",     0x00000000, 0x00000001},
    {EVX$PAL_WRUNIQUE,       "WRUNIQUE",     0x00010000, 0x00000000},
    {EVX$PAL_AMOVRR,         "AMOVRR",       0x003F0000, 0x00000000},
    {EVX$PAL_AMOVRM,         "AMOVRM",       0x003F0000, 0x00000000},
    {EVX$PAL_INSQHILR,       "INSQHILR",     0x00030000, 0x00000001},
    {EVX$PAL_INSQTILR,       "INSQTILR",     0x00030000, 0x00000001},
    {EVX$PAL_INSQHIQR,       "INSQHIQR",     0x00030000, 0x00000001},
    {EVX$PAL_INSQTIQR,       "INSQTIQR",     0x00030000, 0x00000001},
    {EVX$PAL_REMQHILR,       "REMQHILR",     0x00010000, 0x00000003},
    {EVX$PAL_REMQTILR,       "REMQTILR",     0x00010000, 0x00000003},
    {EVX$PAL_REMQHIQR,       "REMQHIQR",     0x00010000, 0x00000003},
    {EVX$PAL_REMQTIQR,       "REMQTIQR",     0x00010000, 0x00000003},
    {EVX$PAL_GENTRAP,        "GENTRAP",      0x000000FC, 0x00000000},
    {EVX$PAL_NPHALT,         "NPHALT",       0x00000000, 0x00000000},
    {EVX$PAL_RESERVED_DVT,   "DVT",          0x00000000, 0x00000000},
    {EVX$PAL_RESERVED_SIM,   "SIM",          0x00000000, 0x00000000},
    {0,                      NULL,           0x00000000, 0x00000000} 
};

/*
 *  The array alpha_opcodes is sorted by opcodes value. Use the opcode value
 *  (6 bits: 0 to 63) as index. The "opcode" field is here for sanity check.
 */

static inst_desc_t alpha_opcodes [] = {
    {EVX$OPC_PAL,    "CALL_PAL", IT_PAL,      0, 0, pal_names},
    {EVX$OPC_OPC01,  "OPC01",    IT_RESERVED, 0, 0, NULL},
    {EVX$OPC_OPC02,  "OPC02",    IT_RESERVED, 0, 0, NULL},
    {EVX$OPC_OPC03,  "OPC03",    IT_RESERVED, 0, 0, NULL},
    {EVX$OPC_OPC04,  "OPC04",    IT_RESERVED, 0, 0, NULL},
    {EVX$OPC_OPC05,  "OPC05",    IT_RESERVED, 0, 0, NULL},
    {EVX$OPC_OPC06,  "OPC06",    IT_RESERVED, 0, 0, NULL},
    {EVX$OPC_OPC07,  "OPC07",    IT_RESERVED, 0, 0, NULL},
    {EVX$OPC_LDA,    "LDA",      IT_MEMORY,   0, 0, NULL},
    {EVX$OPC_LDAH,   "LDAH",     IT_MEMORY,   0, 0, NULL},
    {EVX$OPC_LDBU,   "LDB_U",    IT_MEMORY,   0, 0, NULL},
    {EVX$OPC_LDQU,   "LDQ_U",    IT_MEMORY,   0, 0, NULL},
    {EVX$OPC_LDWU,   "LDW_U",    IT_MEMORY,   0, 0, NULL},
    {EVX$OPC_STW,    "STW",      IT_MEMORY,   0, 1, NULL},
    {EVX$OPC_STB,    "STB",      IT_MEMORY,   0, 1, NULL},
    {EVX$OPC_STQU,   "STQ_U",    IT_MEMORY,   0, 1, NULL},
    {EVX$OPC_ADDI,   "ADDI",     IT_OPER,     0, 0, addi_names},
    {EVX$OPC_LOGI,   "LOGI",     IT_OPER,     0, 0, logi_names},
    {EVX$OPC_SHFI,   "SHFI",     IT_OPER,     0, 0, shfi_names},
    {EVX$OPC_MULI,   "MULI",     IT_OPER,     0, 0, muli_names},
    {EVX$OPC_OPC14,  "OPC14",    IT_RESERVED, 0, 0, NULL},
    {EVX$OPC_FPDEC,  "FPDEC",    IT_FP,       1, 0, NULL},
    {EVX$OPC_FPIEEE, "FPIEEE",   IT_FP,       1, 0, NULL},
    {EVX$OPC_FPAUX,  "FPAUX",    IT_FPAUX,    1, 0, NULL},
    {EVX$OPC_SYNC,   "SYNC",     IT_SYNC,     0, 0, NULL},
    {EVX$OPC_PAL19,  "PAL19",    IT_PAL,      0, 0, NULL},
    {EVX$OPC_JSR,    "JSR",      IT_JUMP,     0, 0, NULL},
    {EVX$OPC_PAL1B,  "PAL1B",    IT_PAL,      0, 0, NULL},
    {EVX$OPC_EXTI,   "EXTI",     IT_OPER,     0, 0, exti_names},
    {EVX$OPC_PAL1D,  "PAL1D",    IT_PAL,      0, 0, NULL},
    {EVX$OPC_PAL1E,  "PAL1E",    IT_PAL,      0, 0, NULL},
    {EVX$OPC_PAL1F,  "PAL1F",    IT_PAL,      0, 0, NULL},
    {EVX$OPC_LDF,    "LDF",      IT_MEMORY,   1, 0, NULL},
    {EVX$OPC_LDD,    "LDD",      IT_MEMORY,   1, 0, NULL},
    {EVX$OPC_LDS,    "LDS",      IT_MEMORY,   1, 0, NULL},
    {EVX$OPC_LDT,    "LDT",      IT_MEMORY,   1, 0, NULL},
    {EVX$OPC_STF,    "STF",      IT_MEMORY,   1, 1, NULL},
    {EVX$OPC_STD,    "STD",      IT_MEMORY,   1, 1, NULL},
    {EVX$OPC_STS,    "STS",      IT_MEMORY,   1, 1, NULL},
    {EVX$OPC_STT,    "STT",      IT_MEMORY,   1, 1, NULL},
    {EVX$OPC_LDL,    "LDL",      IT_MEMORY,   0, 0, NULL},
    {EVX$OPC_LDQ,    "LDQ",      IT_MEMORY,   0, 0, NULL},
    {EVX$OPC_LDLL,   "LDL_L",    IT_MEMORY,   0, 0, NULL},
    {EVX$OPC_LDQL,   "LDQ_L",    IT_MEMORY,   0, 0, NULL},
    {EVX$OPC_STL,    "STL",      IT_MEMORY,   0, 1, NULL},
    {EVX$OPC_STQ,    "STQ",      IT_MEMORY,   0, 1, NULL},
    {EVX$OPC_STLC,   "STL_C",    IT_MEMORY,   0, 1, NULL},
    {EVX$OPC_STQC,   "STQ_C",    IT_MEMORY,   0, 1, NULL},
    {EVX$OPC_BR,     "BR",       IT_BRANCH,   0, 0, NULL},
    {EVX$OPC_FBEQ,   "FBEQ",     IT_BRANCH,   1, 0, NULL},
    {EVX$OPC_FBLT,   "FBLT",     IT_BRANCH,   1, 0, NULL},
    {EVX$OPC_FBLE,   "FBLE",     IT_BRANCH,   1, 0, NULL},
    {EVX$OPC_BSR,    "BSR",      IT_BRANCH,   0, 0, NULL},
    {EVX$OPC_FBNE,   "FBNE",     IT_BRANCH,   1, 0, NULL},
    {EVX$OPC_FBGE,   "FBGE",     IT_BRANCH,   1, 0, NULL},
    {EVX$OPC_FBGT,   "FBGT",     IT_BRANCH,   1, 0, NULL},
    {EVX$OPC_BLBC,   "BLBC",     IT_BRANCH,   0, 0, NULL},
    {EVX$OPC_BEQ,    "BEQ",      IT_BRANCH,   0, 0, NULL},
    {EVX$OPC_BLT,    "BLT",      IT_BRANCH,   0, 0, NULL},
    {EVX$OPC_BLE,    "BLE",      IT_BRANCH,   0, 0, NULL},
    {EVX$OPC_BLBS,   "BLBS",     IT_BRANCH,   0, 0, NULL},
    {EVX$OPC_BNE,    "BNE",      IT_BRANCH,   0, 0, NULL},
    {EVX$OPC_BGE,    "BGE",      IT_BRANCH,   0, 0, NULL},
    {EVX$OPC_BGT,    "BGT",      IT_BRANCH,   0, 0, NULL},
};


/*******************************************************************************
 *
 *  Default output routine for callmon$disassemble
 */

static void default_output (void* pc, char* opcode, char* operands, void* data)
{
    printf ("  %08X:  %-7s %s\n", pc, opcode, operands);
}


/*******************************************************************************
 *
 *  This routine formats and displays Alpha machine code instructions.
 */

void callmon$$disassemble (
    inst_t*                          address,
    size_t                           size_in_bytes,
    void*                            displayed_pc,
    callmon$disassemble_output_t     output_routine,
    callmon$disassemble_output_dsc_t output_routine_dsc,
    void*                            user_data)
{
    inst_t* end = (inst_t*) ((char*) address + size_in_bytes);
    inst_t* pc;
    inst_t* disp_pc;
    inst_desc_t* inst;
    char opcode [40], operands [80];
    desc_t opcode_d, operands_d;

    /* Ensure that there is at least one output routine */

    if (output_routine == NULL && output_routine_dsc == NULL)
        output_routine = default_output;

    for (pc = address, disp_pc = displayed_pc; pc < end; pc++, disp_pc++) {

        inst = alpha_opcodes + pc->u.op.opcode;

        /* Opcode will be filled, operands are optional */

        operands [0] = '\0';

        /* Look for "stylized code forms". */

        /* NOP = BIS R31, R31, R31 */

        if (pc->u.instr ==
            (EVX$OPC32_BIS | INST_RA_MASK | INST_RB_MASK | INST_RC_MASK)) {

            strcpy (opcode, "NOP");
        }

        /* FNOP = CPYS F31, F31, F31 */

        else if (pc->u.instr 
            == (EVX$OPC32_CPYS | INST_RA_MASK | INST_RB_MASK | INST_RC_MASK)) {

            strcpy (opcode, "FNOP");
        }

        /* CLR Rx = BIS R31, R31, Rx */

        else if ((pc->u.instr & ~INST_RC_MASK) 
            == (EVX$OPC32_BIS | INST_RA_MASK | INST_RB_MASK)) {

            strcpy (opcode, "CLR");
            sprintf (operands, "R%d", pc->u.op.rc);
        }

        /* FCLR Fx = CPYS F31, F31, Fx */

        else if ((pc->u.instr & ~INST_RC_MASK)
            == (EVX$OPC32_CPYS | INST_RA_MASK | INST_RB_MASK)) {

            strcpy (opcode, "FCLR");
            sprintf (operands, "F%d", pc->u.op.rc);
        }

        /* MOV Rx, Ry = BIS R31, Rx, Ry = BIS Rx, Rx, Ry */

        else if ((pc->u.instr & ~INST_RB_MASK & ~INST_RC_MASK) 
            == (EVX$OPC32_BIS | INST_RA_MASK) ||
            ((pc->u.instr & ~INST_RA_MASK & ~INST_RB_MASK & ~INST_RC_MASK)
            == EVX$OPC32_BIS && pc->u.op.ra == pc->u.op.rb)) {

            strcpy (opcode, "MOV");
            sprintf (operands, "R%d, R%d", pc->u.op.rb, pc->u.op.rc);
        }

        /* MOV #lit, Ry = BIS R31, #lit, Ry */

        else if ((pc->u.instr & ~INST_LIT_MASK & ~INST_RC_MASK)
            == (EVX$OPC32_BIS | INST_RA_MASK | INST_LITFLG_MASK)) {

            strcpy (opcode, "MOV");
            sprintf (operands, "#%d, R%d", pc->u.oplit.lit, pc->u.oplit.rc);
        }

        /* FMOV Fx, Fy = CPYS Fx, Fx, Fy */

        else if ((pc->u.instr & ~INST_RA_MASK & ~INST_RB_MASK & ~INST_RC_MASK)
            == EVX$OPC32_CPYS && pc->u.op.ra == pc->u.op.rb) {

            strcpy (opcode, "FMOV");
            sprintf (operands, "F%d, F%d", pc->u.op.rb, pc->u.op.rc);
        }

        /* NOT Rx, Ry = ORNOT R31, Rx, Ry */

        else if ((pc->u.instr & ~INST_RB_MASK & ~INST_RC_MASK)
            == (EVX$OPC32_ORNOT | INST_RA_MASK)) {

            strcpy (opcode, "NOT");
            sprintf (operands, "R%d, R%d", pc->u.op.rb, pc->u.op.rc);
        }

        /* NEGL Rx, Ry = SUBL R31, Rx, Ry */

        else if ((pc->u.instr & ~INST_RB_MASK & ~INST_RC_MASK)
            == (EVX$OPC32_SUBL | INST_RA_MASK)) {

            strcpy (opcode, "NEGL");
            sprintf (operands, "R%d, R%d", pc->u.op.rb, pc->u.op.rc);
        }

        /* NEGQ Rx, Ry = SUBQ R31, Rx, Ry */

        else if ((pc->u.instr & ~INST_RB_MASK & ~INST_RC_MASK)
            == (EVX$OPC32_SUBQ | INST_RA_MASK)) {

            strcpy (opcode, "NEGQ");
            sprintf (operands, "R%d, R%d", pc->u.op.rb, pc->u.op.rc);
        }

        /* Standard code form processing */

        else {
            switch (inst->type) {

                case IT_RESERVED: {
                    strcpy (opcode, inst->name);
                    break;
                }

                case IT_PAL: {
                    inst_pal_t* pal;
                    strcpy (opcode, inst->name);
                    if ((pal = inst->name_list) != NULL) {
                        for (; pal->name != NULL; pal++) {
                            if (pc->u.pal.number == pal->function) {
                                strcpy (operands, pal->name);
                                break;
                            }
                        }
                    }
                    if (pal == NULL || pal->name == NULL)
                        sprintf (operands, "%d", pc->u.pal.number);
                    break;
                }

                case IT_SYNC: {
                    inst_sync_t* sync;
                    char* op;
                    for (sync = sync_opcodes; sync->name != NULL; sync++) {
                        if (pc->u.umem.func == sync->function) {
                            strcpy (opcode, sync->name);
                            op = operands;
                            if (sync->use_ra)
                                op += sprintf (op, "R%s", pc->u.umem.ra);
                            if (sync->use_ra && sync->use_rb)
                                op += strlen (strcpy (op, ", "));
                            if (sync->use_rb)
                                sprintf (op, "(R%s)", pc->u.umem.rb);
                            break;
                        }
                    }
                    if (sync->name == NULL)
                        sprintf (opcode, "%s_%d", inst->name, pc->u.umem.func);
                    break;
                }

                case IT_OPER: {
                    inst_name_t* name;
                    for (name = inst->name_list; name->name != NULL; name++) {
                        if (pc->u.op.func == name->function) {
                            strcpy (opcode, name->name);
                            break;
                        }
                    }
                    if (name->name == NULL)
                        sprintf (opcode, "%s_%d", inst->name, pc->u.op.func);
                    sprintf (operands, "R%d, %c%d, R%d", pc->u.op.ra,
                        pc->u.op.litflg ? '#' : 'R',
                        pc->u.op.litflg ? pc->u.oplit.lit : pc->u.op.rb,
                        pc->u.op.rc);
                    break;
                }

                case IT_MEMORY: {
                    strcpy (opcode, inst->name);
                    sprintf (operands, "%c%d, %d (R%d)",
                        inst->float_inst ? 'F' : 'R',
                        pc->u.mem.ra, pc->u.mem.disp, pc->u.mem.rb);
                    break;
                }

                case IT_BRANCH: {
                    routine_t* routine;
                    inst_t* target = disp_pc + 1 + pc->u.branch.disp;
                    char* op = operands;
                    strcpy (opcode, inst->name);
                    if (pc->u.branch.opcode != EVX$OPC_BR ||
                        pc->u.branch.ra != 31) {
                        op += sprintf (op, "%c%d, ",
                            inst->float_inst ? 'F' : 'R',
                            pc->u.branch.ra);
                    }
                    if (pc->u.branch.opcode == EVX$OPC_BSR && (routine =
                        callmon$$get_routine_by_entry ((int64) target)) != NULL)
                        strcpy (op, routine->name);
                    else
                        sprintf (op, "%08X", target);
                    break;
                }

                case IT_JUMP: {
                    static char* jsr_names [] = {
                        "JMP", "JSR", "RET", "JSR_COROUTINE"};
                    char* op = operands;
                    strcpy (opcode, jsr_names [pc->u.jump.hint]);
                    if (pc->u.jump.ra != 31)
                        op += sprintf (op, "R%d, ", pc->u.jump.ra);
                    sprintf (op, "(R%d)", pc->u.jump.rb);
                    break;
                }

                case IT_FPAUX: {
                    inst_fpaux_t* fp;
                    char* op = operands;
                    for (fp = fpaux_opcodes; fp->name != NULL; fp++) {
                        if (pc->u.fpaux.func == fp->function) {
                            strcpy (opcode, fp->name);
                            if (fp->use_fa)
                                op += sprintf (op, "F%d, ", pc->u.fpaux.fa);
                            sprintf (op, "F%d, F%d", pc->u.fpaux.fb,
                                pc->u.fpaux.fc);
                            break;
                        }
                    }
                    if (fp->name == NULL)
                        sprintf (opcode, "%s_%d", inst->name, pc->u.fpaux.func);
                    break;
                }

                case IT_FP: {
                    char* op;
                    char** names = pc->u.fp.opcode == EVX$OPC_FPIEEE ?
                        fp_ieee_name : fp_dec_name;
                    op = opcode + sprintf (opcode, fp_inst_name [pc->u.fp.func],
                        names [pc->u.fp.src], names [pc->u.fp.func & 0x03]);
                    op += sprintf (op, "/%s%s", fp_trap_name [pc->u.fp.trp],
                        fp_round_name [pc->u.fp.rnd]);
                    if (op [-1] == '/')
                        op [-1] = '\0';
                    op = operands;
                    if (pc->u.fp.func < 8)
                        op += sprintf (op, "F%d, ", pc->u.fp.fa);
                    sprintf (op, "F%d, F%d", pc->u.fp.fb, pc->u.fp.fc);
                    break;
                }

            } /* switch */
        } /* else */

        /* Call the display routine */

        if (output_routine != NULL)
            output_routine (disp_pc, opcode, operands, user_data);

        if (output_routine_dsc != NULL)
            output_routine_dsc (disp_pc, set_str_desc (&opcode_d, opcode),
                set_str_desc (&operands_d, operands), user_data);

    } /* for */
}


/*******************************************************************************
 *
 *  These public routines format and display Alpha machine code instructions.
 */

void callmon$disassemble (
    void*                        address,
    size_t                       size_in_bytes,
    void*                        displayed_pc,
    callmon$disassemble_output_t output_routine,
    void*                        user_data)
{
    callmon$initialize ();

    callmon$$disassemble (address, size_in_bytes, displayed_pc,
        output_routine, NULL, user_data);
}

void callmon$disassemble_dsc (
    void*                            address,
    size_t                           size_in_bytes,
    void*                            displayed_pc,
    callmon$disassemble_output_dsc_t output_routine,
    void*                            user_data)
{
    callmon$initialize ();

    callmon$$disassemble (address, size_in_bytes, displayed_pc,
        NULL, output_routine, user_data);
}


/*******************************************************************************
 *
 *  This routine returns the list of integer registers which are read
 *  by the specified instruction. Return a mask.
 */

uint32 callmon$$read_register_mask (inst_t* pc)
{
    uint32 result = 0;
    inst_desc_t* inst = alpha_opcodes + pc->u.op.opcode;

    switch (inst->type) {

        case IT_PAL: {
            inst_pal_t* pal;
            if ((pal = inst->name_list) != NULL) {
                for (; pal->name != NULL; pal++) {
                    if (pc->u.pal.number == pal->function) {
                        result = pal->read_regs;
                        break;
                    }
                }
            }
            break;
        }

        case IT_JUMP:
        case IT_MEMORY: {
            result = 1 << pc->u.mem.rb;
            if (!inst->float_inst && inst->is_store)
                result |= 1 << pc->u.mem.ra;
            break;
        }

        case IT_BRANCH: {
            if (!inst->float_inst &&
                pc->u.branch.opcode != EVX$OPC_BR &&
                pc->u.branch.opcode != EVX$OPC_BSR)
                result = 1 << pc->u.branch.ra;
            break;
        }

        case IT_SYNC: {
            inst_sync_t* sync;
            for (sync = sync_opcodes; sync->name != NULL; sync++) {
                if (pc->u.umem.func == sync->function) {
                    result = sync->use_rb ? (1 << pc->u.umem.rb) : 0;
                    break;
                }
            }
            break;
        }

        case IT_OPER: {
            result = 1 << pc->u.op.ra;
            if (!pc->u.op.litflg)
                result |= 1 << pc->u.op.rb;
            break;
        }
    }

    /* Register R31 is not significant */

    return result & 0x7FFFFFFF;
}


/*******************************************************************************
 *
 *  This routine returns the list of integer registers which are written
 *  by the specified instruction. Return a mask.
 */

uint32 callmon$$write_register_mask (inst_t* pc)
{
    uint32 result = 0;
    inst_desc_t* inst = alpha_opcodes + pc->u.op.opcode;

    switch (inst->type) {

        case IT_PAL: {
            inst_pal_t* pal;
            if ((pal = inst->name_list) != NULL) {
                for (; pal->name != NULL; pal++) {
                    if (pc->u.pal.number == pal->function) {
                        result = pal->write_regs;
                        break;
                    }
                }
            }
            break;
        }

        case IT_JUMP:
        case IT_MEMORY: {
            if (!inst->float_inst && !inst->is_store)
                result = 1 << pc->u.mem.ra;
            break;
        }

        case IT_BRANCH: {
            if (pc->u.branch.opcode == EVX$OPC_BR ||
                pc->u.branch.opcode == EVX$OPC_BSR)
                result = 1 << pc->u.branch.ra;
            break;
        }

        case IT_SYNC: {
            inst_sync_t* sync;
            for (sync = sync_opcodes; sync->name != NULL; sync++) {
                if (pc->u.umem.func == sync->function) {
                    result = sync->use_ra ? (1 << pc->u.umem.ra) : 0;
                    break;
                }
            }
            break;
        }

        case IT_OPER: {
            result = 1 << pc->u.op.rc;
            break;
        }
    }

    /* Register R31 is not significant */

    return result & 0x7FFFFFFF;
}


/*******************************************************************************
 *
 *  This routine checks if the specified instruction takes a branch.
 *  Possible return values are:
 *
 *    0 : no branch
 *    1 : call to PAL (returns in sequence)
 *    2 : call to subroutine (should return in sequence)
 *    3 : conditional branch
 *    4 : unconditional branch
 */

int callmon$$branch_instruction (inst_t* pc)
{
    inst_desc_t* inst = alpha_opcodes + pc->u.op.opcode;

    switch (inst->type) {

        case IT_PAL: {
            return 1;
        }

        case IT_BRANCH: {
            if (pc->u.branch.opcode == EVX$OPC_BSR)
                return 2;
            else if (pc->u.branch.opcode == EVX$OPC_BR)
                return 4;
            else
                return 3;
        }

        case IT_JUMP: {
            static int jsr_values [] = {
                /*JMP*/ 4, /*JSR*/ 2, /*RET*/ 4, /*JSR_COROUTINE*/ 2};
            return jsr_values [pc->u.jump.hint];
        }
    }

    return 0;
}


/*******************************************************************************
 *
 *  This routine checks if the specified instruction is a PC-relative
 *  branch instruction. If yes, return the target of the branch. If no,
 *  return NULL.
 */

inst_t* callmon$$branch_target (inst_t* pc)
{
    inst_desc_t* inst = alpha_opcodes + pc->u.op.opcode;

    /* Compute the target of the branch. Note that displacement must
     * be applied on the adjusted PC and uses a factor of 4, which
     * is the factor used by C since this is the size of inst_t. */

    if (inst->type == IT_BRANCH)
        return pc + 1 + pc->u.branch.disp;
    else
        return NULL;
}
