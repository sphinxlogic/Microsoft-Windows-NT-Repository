/*  CALLMON - A Call Monitor for OpenVMS Alpha
 *
 *  File:     CALLMON.H
 *  Author:   Thierry Lelegard
 *  Version:  1.0
 *  Date:     24-JUL-1996
 *
 *  Abstract: Public Interface of CALLMON
 */


#ifndef CALLMON_H_LOADED
#define CALLMON_H_LOADED 1

#include <ints.h>
#include "callmonmsg.h"

typedef enum {
    CALLMON$C_FALSE = 0,
    CALLMON$C_TRUE  = 1
} callmon$boolean_t;

#pragma nostandard
typedef struct {
    uint32 post_processing;
    uint32 call_it;
    uint64 result_r0;
    uint64 result_r1;
    uint64 result_f0;
    uint64 result_f1;
    uint64 arg_count;
    uint64 arg_list [];
} callmon$arguments_t;
#pragma standard

typedef void (*callmon$intercept_t) (
    callmon$arguments_t* arguments,
    uint32               caller_invo_handle,
    char*                routine_name,
    uint64               (*intercepted_routine)(),
    uint64               (*jacket_routine)());

typedef enum {
    CALLMON$C_ALL_ROUTINES         = 0,
    CALLMON$C_INTERCEPTED_ONLY     = 1,
    CALLMON$C_UNINTERCEPTABLE_ONLY = 2,
    CALLMON$C_UNRELOCATABLE_ONLY   = 3
} callmon$dump_routine_t;

typedef void (*callmon$disassemble_output_t) (
    void* pc,
    char* opcode,
    char* operands,
    void* user_data);

/*
 *  CALLMON Routines
 */

uint32 callmon$initialize (void);

uint32 callmon$intercept (
    char*               routine_name,
    callmon$intercept_t pre_routine,
    callmon$intercept_t post_routine);

uint32 callmon$intercept_all (
    char*               image_name,
    callmon$intercept_t pre_routine,
    callmon$intercept_t post_routine,
    callmon$boolean_t   override);

uint32 callmon$dump_image_tree (void);
uint32 callmon$dump_routine_tree (callmon$dump_routine_t rclass);

void callmon$disassemble (
    void*                        address,
    size_t                       size_in_bytes,
    void*                        displayed_pc,
    callmon$disassemble_output_t output_routine,
    void*                        user_data);

#endif /* CALLMON_H_LOADED */
