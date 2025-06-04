/*  CALLMON - A Call Monitor for OpenVMS Alpha
 *
 *  File:     CALLMON$BRANCH.C
 *  Author:   Thierry Lelegard
 *  Version:  1.0
 *  Date:     24-JUL-1996
 *
 *  Abstract: This module processes the BSR instructions (branch to
 *            subroutine) inside an image.
 */


#include "callmon$private.h"


/*******************************************************************************
 *
 *  This module analyzes code sections. During the analysis of the code
 *  stream of a section, the following structure describes the state of
 *  the section.
 */

typedef struct {
    inst_t* start;          /* Starting address of the section */
    inst_t* end;            /* End address (+1) of the section */
    uint32* targets;        /* Bit array: if set, inst is target of a branch */
    size_t targets_size;    /* Allocated size of targets (in bytes) */
    int targets_count;      /* Number of bits set in targets */
    inst_t* last_nop;       /* Last NOP instruction */
    inst_t* last_load_r27;  /* Last inst which loads something into R27 */
    inst_t* last_use_r27;   /* Last inst which uses the content of R27 */
} code_state_t;


/*******************************************************************************
 *
 *  Output routine for callmon$disassemble.
 *  Used when instruction tracing is enabled.
 */

typedef struct {
    FILE* fp;
    enum {DISPLAY_NONE, DISPLAY_CHANGE, DISPLAY_ALL} display;
    code_state_t previous_state;
    code_state_t state;
} trace_output_data_t;

static void trace_output (
    void* pc,
    char* opcode,
    char* operands,
    void* user_data)
{
    trace_output_data_t* trdata = user_data;
    routine_t* routine;
    int offset;
    int is_target;

    /* Check if the PC is at the entry of a known procedure */

    routine = callmon$$get_routine_by_entry ((int64) (pc));

    if (routine != NULL)
        fprintf (trdata->fp, "%s:\n", routine->name);

    /* Check if the instruction is the target of a branch */

    offset = (inst_t*) pc - trdata->state.start;
    is_target = trdata->state.targets [offset / 32] & (1 << (offset % 32));

    /* Display the instruction */

    fprintf (trdata->fp, "%08X:%08X%s%-7s %s", pc, *(uint32*)pc,
        is_target ? "->" : "  ", opcode, operands);

    /* Display the change of state */

    if (trdata->display != DISPLAY_NONE) {

        if (trdata->display == DISPLAY_ALL ||
            trdata->state.last_nop != trdata->previous_state.last_nop)
            fprintf (trdata->fp, " ** NOP:%08X", trdata->state.last_nop);

        if (trdata->display == DISPLAY_ALL ||
            trdata->state.last_load_r27 != trdata->previous_state.last_load_r27)
            fprintf (trdata->fp, " ** LDxR27:%08X",trdata->state.last_load_r27);

        if (trdata->display == DISPLAY_ALL ||
            trdata->state.last_use_r27 != trdata->previous_state.last_use_r27)
            fprintf (trdata->fp, " ** STxR27:%08X", trdata->state.last_use_r27);
    }

    fprintf (trdata->fp, "\n");
}


/*******************************************************************************
 *
 *  This routine attempts to transform all BSR instructions in the
 *  specified image into JSR instructions.
 */

void callmon$$remove_bsr (image_t* image)
{
    uint32 status;
    EISD* eisd;
    char* end_of_header;
    int section_count;
    int section_max_size;
    routine_t* routine;
    int is_bsr_r26;
    int ok_to_replace;
    int offset;
    inst_t ldq_r26, jsr, nop;
    uint32 input_regs, output_regs;
    inst_t *pc, *branch_target, *first;
    code_state_t state;
    trace_output_data_t trace;
    char trace_file_name [FILE_SPEC_SIZE];

    /* Do this only once per image */

    if (!image->replace_bsr)
        return;

    image->replace_bsr = 0;

    /* Build an instruction to load the entry code of a procedure:
     * LDQ R26, PDSC$Q_ENTRY (R27) */

    ldq_r26.u.mem.opcode = EVX$OPC_LDQ;
    ldq_r26.u.mem.ra = 26;
    ldq_r26.u.mem.rb = 27;
    ldq_r26.u.mem.disp = OFFSET (pdsc_t, pdsc$l_entry);

    /* Build a standard JSR instruction: JSR R26, (R26) */

    jsr.u.instr = EVX$OPC32_JSR;
    jsr.u.mem.ra = jsr.u.mem.rb = 26;

    /* Build a NOP instruction: BIS R31, R31, R31 */

    nop.u.instr = EVX$OPC32_BIS;
    nop.u.op.ra = nop.u.op.rb = nop.u.op.rc = 31;

    /* The table of ISD (image section descriptors) is at the end of the image
     * header. The last ISD is found when the end of header is reached. */

    end_of_header = (char*) image->eihd + image->eihd->eihd$l_size;

    /* Find the biggest code section in the image. We loop on all sections
     * with the EXE attributes and find the biggest one. */

    section_max_size = 0;

    for (eisd = image->eisd;
         (char*) eisd < end_of_header &&
         (char*) eisd + eisd->eisd$l_eisdsize <= end_of_header;
         eisd = (EISD*) ((char*)eisd + eisd->eisd$l_eisdsize)) {

        if (eisd->eisd$v_exe && eisd->eisd$l_secsize > section_max_size)
            section_max_size = eisd->eisd$l_secsize;
    }

    /* Allocate on stack an array of bits. There is one bit for each
     * instruction in the current code section. If the bit is set,
     * the instruction is the target of a branch. Notes:
     *   - instructions are 32-bits => divide section size by 4
     *   - bits are grouped in uint32 => add 31 to the number of instructions
     *   - divide by 8 gives the number of bytes to allocate */

    state.targets_size = ((section_max_size / 4) + 31) / 8;
    state.targets = __ALLOCA (state.targets_size);

    /* Now, examine all code sections, looking for BSR instructions pointing
     * to known routines. */

    section_count = 0;

    for (eisd = image->eisd;
         (char*) eisd < end_of_header &&
         (char*) eisd + eisd->eisd$l_eisdsize <= end_of_header;
         eisd = (EISD*) ((char*)eisd + eisd->eisd$l_eisdsize)) {

        section_count++;

        /* If the section does not contain code, no interest */

        if (!eisd->eisd$v_exe)
            continue;

        /* Compute the starting and ending addresses of the section.
         * For shareable images, the section address is relative to
         * the image base address. */

        if (image->imcb->imcb$b_act_code == IMCB$K_MAIN_PROGRAM)
            state.start = (inst_t*) eisd->eisd$l_virt_addr;
        else
            state.start = (inst_t*) ((char*) image->imcb->imcb$l_base_address +
                (int32) eisd->eisd$l_virt_addr);

        state.end = (inst_t*) ((char*) state.start + eisd->eisd$l_secsize);

        /* If instruction tracing is enabled, creates a text file
         * for each code section */

        trace.fp = NULL;

        if (callmon$$own.trace_flags & TRACE_INSTRUCTIONS) {

            sprintf (trace_file_name, "%s_%03d.LIS",
                image->logname, section_count);

            callmon$$putmsg (CALLMON$_DOISD, 4, section_count, image->logname,
                state.start, state.end, CALLMON$_CREFLW, 1, trace_file_name);

            trace.fp = fopen (trace_file_name, "w",
                "dna=CALLMON$TRACE_DIRECTORY:");

            if (trace.fp != NULL) {
                fprintf (trace.fp, "Image file %s\n"
                    "Image image section number %d "
                    "(%d bytes, %d instructions)\n"
                    "Image base address: %08X\n"
                    "Section base address: %08X, end: %08X\n",
                    image->file_spec, section_count,
                    eisd->eisd$l_secsize, state.end - state.start,
                    image->imcb->imcb$l_base_address,
                    state.start, state.end);
            }
            else if (errno == EVMSERR) {
                callmon$$putmsg (CALLMON$_OPEN, 1, trace_file_name, vaxc$errno);
            }
            else {
                callmon$$putmsg (CALLMON$_OPEN, 1, trace_file_name);
                perror ("fopen");
            }
        }

        /* First pass: find all instructions which are targets of branches */

        memset (state.targets, 0, state.targets_size);
        state.targets_count = 0;

        for (pc = state.start; pc < state.end; pc++) {
            if ((branch_target = callmon$$branch_target (pc)) != NULL) {
                offset = branch_target - state.start;
                if ((state.targets [offset / 32] & (1 << (offset % 32))) == 0) {
                    state.targets [offset / 32] |= 1 << (offset % 32);
                    state.targets_count++;
                }
            }
        }

        if (trace.fp != NULL) {
            callmon$$putmsg (CALLMON$_PASS1, 2, state.end - state.start,
                state.targets_count);
            fprintf (trace.fp, "Number of branch targets: %d\n\n",
                state.targets_count);
        }

        /* Second pass: Process BSR instructions */

        state.last_nop = state.last_load_r27 = state.last_use_r27 = NULL;

        for (pc = state.start; pc < state.end; pc++) {

            /* Save current state to trace state change */

            if (trace.fp != NULL) {
                trace.previous_state = state;
                if (pc > state.start && (pc - state.start) % 10000 == 0)
                    callmon$$putmsg (CALLMON$_INSDUMP, 1, pc - state.start);
            }

            /* If the current instruction is the target of a branch,
             * we cannot hack instructions backwards, Reset */

            offset = pc - state.start;

            if (state.targets [offset / 32] & (1 << (offset % 32))) {
                state.last_nop = NULL;
                state.last_load_r27 = NULL;
                state.last_use_r27 = NULL;
            }

            /* Keep track of last NOP instruction. A NOP can be later
             * replaced by a new instruction. */

            if (pc->u.instr == nop.u.instr)
                state.last_nop = pc;

            /* Check if the instruction is a standard call using BSR.
             * In a standard call, the R26 register must point to the
             * return address. */

            is_bsr_r26 = pc->u.branch.opcode == EVX$OPC_BSR &&
                pc->u.branch.ra == 26;

            /* Get the registers which are used by the instruction */

            input_regs = callmon$$read_register_mask (pc);
            output_regs = callmon$$write_register_mask (pc);

            /* Keep track of last instruction loading something into R27.
             * R27 is loaded with the procedure value (address of PDSC)
             * before a standard call. */

            if (output_regs & (1 << 27))
                state.last_load_r27 = pc;

            /* Keep track of last instruction using the content of R27.
             * If we later relocate the instruction loading into R27,
             * we cannot move it before this one. */

            if (input_regs & (1 << 27))
                state.last_use_r27 = pc;

            /* If an instruction uses R26, we cannot insert a LDQ R26
             * before it without breaking the code. Consequently, we
             * cannot patch the last NOP location. Note however that
             * the BSR R26 is a special case (see below). */

            if (!is_bsr_r26 &&
                ((input_regs & (1 << 26)) || (output_regs & (1 << 26))))
                state.last_nop = NULL;

            /* Branch instructions reset the last NOP and load R27
             * because we lose track of register usage. Note however
             * that the BSR R26 is a special case (see below). */

            if (!is_bsr_r26 && callmon$$branch_instruction (pc)) {
                state.last_nop = NULL;
                state.last_load_r27 = NULL;
                state.last_use_r27 = NULL;
            }

            /* Dump instruction in file if trace is enabled */

            if (trace.fp != NULL) {
                trace.state = state;
                trace.display = is_bsr_r26 ? DISPLAY_ALL : DISPLAY_CHANGE;
                callmon$disassemble (pc, 4, pc, trace_output, &trace);
            }

            /* The rest of the loop is the processing of a BSR instruction
             * for standard call, ie a BSR R26. If the instruction is not
             * a BSR R26, loop on next instruction. */

            if (!is_bsr_r26)
                continue;

            /* Check if the target of the BSR is a known routine. If the
             * target is a routine without universal symbol, we don't care.
             * If the routine is not relocatable, don't care as well. */

            routine = callmon$$get_routine_by_entry (
                (int64) callmon$$branch_target (pc));

            if (routine == NULL || routine->unrelocatable) {
                state.last_nop = NULL;
                state.last_load_r27 = NULL;
                state.last_use_r27 = NULL;
                continue;
            }

            /* Check if we are allowed to perform BSR replacement */

            switch (callmon$$own.bsr_replacement) {
                case BSR_OFF:
                    ok_to_replace = 0;
                    break;
                case BSR_IF_R27:
                    ok_to_replace = state.last_load_r27 != NULL;
                    break;
                case BSR_ALL:
                    ok_to_replace = 1;
                    break;
                default:
                    ok_to_replace = 0; /* should not be there */
            }

            /* Check we can modify the BSR into a JSR.
             * The input sequence is something like:
             *
             *     NOP                      LDx R27, offset(Rx)
             *     ....                     ....
             *     LDx R27, offset(Rx)      NOP
             *     ....                     ....
             *     BSR R26, displacement    BSR R26, displacement
             *
             * We modify the code sequence into a traditional call:
             *
             *     LDx R27, offset(Rx)
             *     ....
             *     LDQ R26, PSDC$Q_ENTRY(R27)
             *     ....
             *     JSR R26, (R26)
             */

            first = state.last_load_r27 == NULL ||
                state.last_load_r27 > state.last_nop ?
                state.last_nop : state.last_load_r27;

            if (ok_to_replace &&
                first >= pc - callmon$$own.bsr_depth &&
                (state.last_nop != first ||
                    state.last_use_r27 < state.last_nop) &&
                callmon$$writeable (first,
                    (pc + 1 - first) * sizeof (inst_t))) {

                /* Modify the code sequence with a JSR */

                if (state.last_nop == first && state.last_load_r27 != NULL) {
                    /* Move the LDQ R27 first */
                    *state.last_nop = *state.last_load_r27;
                    state.last_nop = state.last_load_r27;
                }
                *state.last_nop = ldq_r26;
                *pc = jsr;

                /* Invalidate instruction stream */

                __MB ();
                __PAL_IMB ();

                /* Trace this modification */

                if (trace.fp != NULL) {
                    fprintf (trace.fp, "========  "
                        "Instruction stream modified to:\n");
                    trace.display = DISPLAY_NONE;
                    callmon$disassemble (first, (pc + 1 - first) * 4, first,
                        trace_output, &trace);
                    fprintf (trace.fp, "===================================\n");
                }
            }

            else {

                /* We cannot convert the BSR into a JSR. Consequently, we
                 * cannot safely hook the relocation of the target routine. */

                callmon$$set_unrelocatable (routine);

                if ((callmon$$own.trace_flags & TRACE_UNRELOCATABLE))
                    callmon$$putmsg (CALLMON$_NOTREL, 1, routine->name);

                if (trace.fp != NULL)
                    fprintf (trace.fp, "========  "
                        "Routine %s becomes unrelocatable\n", routine->name);
            }

            /* Reset NOP and LDx R27 after the BSR */

            state.last_nop = state.last_load_r27 = NULL;
        }

        /* If instruction tracing is enabled, close the trace file */

        if (trace.fp != NULL) {
            fclose (trace.fp);
            callmon$$putmsg (CALLMON$_FILDONE, 1, trace_file_name);
        }
    }
}
