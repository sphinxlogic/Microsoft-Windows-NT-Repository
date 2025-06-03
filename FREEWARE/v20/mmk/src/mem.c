/*
**++
**  FACILITY:	MMK
**
**  ABSTRACT:	Memory management routines
**
**  MODULE DESCRIPTION:
**
**  	Memory allocation and deallocation routines for MMS structures.
**  Note that character strings are just allocated using malloc.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT � 1992, 1993  MADGOAT SOFTWARE.
**  	    	    ALL RIGHTS RESERVED.
**
**  CREATION DATE:  20-AUG-1992
**
**  MODIFICATION HISTORY:
**
**  	20-AUG-1992 V1.0    Madison 	Initial coding.
**  	27-AUG-1992 V1.1    Madison 	Use VM zones.
**  	09-APR-1993 V1.1-1  Madison 	Comments.
**--
*/
#ifdef __DECC
#pragma module MEM "V1.1-1"
#else
#ifndef __GNUC__
#module MEM "V1.1-1"
#endif
#endif
#include "mmk.h"
#ifdef __GNUC__
#include <vms/libvmdef.h>
#else
#include <libvmdef.h>
#endif

#pragma nostandard
#include "mmk_msg.h"
#pragma standard

/*
** Forward declarations
*/
    struct CMD *mem_get_cmd(void);
    void mem_free_cmd(struct CMD *);
    static unsigned int CMD_S_CMDDEF = sizeof(struct CMD);

    struct DEPEND *mem_get_depend(void);
    void mem_free_depend(struct DEPEND *);
    static unsigned int DEPEND_S_DEPENDDEF = sizeof(struct DEPEND);

    struct SYMBOL *mem_get_symbol(void);
    void mem_free_symbol(struct SYMBOL *);
    static unsigned int SYMBOL_S_SYMBOLDEF = sizeof(struct SYMBOL);

    struct RULE *mem_get_rule(void);
    void mem_free_rule(struct RULE *);
    static unsigned int RULE_S_RULEDEF = sizeof(struct RULE);

    struct OBJECT *mem_get_object(void);
    void mem_free_object(struct OBJECT *);
    static unsigned int OBJECT_S_OBJECTDEF = sizeof(struct OBJECT);

    struct OBJREF *mem_get_objref(void);
    void mem_free_objref(struct OBJREF *);
    static unsigned int OBJREF_S_OBJREFDEF = sizeof(struct OBJREF);

    struct SFX *mem_get_sfx(void);
    void mem_free_sfx(struct SFX *);
    static unsigned int SFX_S_SFXDEF = sizeof(struct SFX);

/*
** The VM zones we're using
*/

    static unsigned int cmdzone=0, dependzone=0, symbolzone=0,
    	    	    	rulezone=0, objectzone=0, objrefzone=0,
    	    	    	sfxzone=0;


/*
**++
**  ROUTINE:	mem_get_cmd
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Allocates a CMD block.
**
**  RETURNS:	struct CMD *
**
**  PROTOTYPE:
**
**  	mem_get_cmd()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.	(errors signalled)
**
**  SIDE EFFECTS:   	cmdzone created.
**
**--
*/
struct CMD *mem_get_cmd() {

    struct CMD *c;
    unsigned int status;

    if (!cmdzone) {
    	unsigned int algorithm=LIB$K_VM_FIXED;
    	unsigned int flags=(LIB$M_VM_GET_FILL0|LIB$M_VM_EXTEND_AREA);
    	status = lib$create_vm_zone(&cmdzone, &algorithm, &CMD_S_CMDDEF, &flags);
    	if (!OK(status)) lib$signal(MMK__NOALLOC, 1, "CMD", status);
    }
    status = lib$get_vm(&CMD_S_CMDDEF, &c, &cmdzone);
    if (!OK(status)) {
    	lib$signal(MMK__NOALLOC, 1, "CMD", status);
    }

    return c;
}

/*
**++
**  ROUTINE:	mem_free_cmd
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Frees a CMD block
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	mem_free_cmd(struct CMD *c)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void mem_free_cmd(struct CMD *c) {

    if (c->cmd) free(c->cmd);
    lib$free_vm(&CMD_S_CMDDEF, &c, &cmdzone);
}

/*
**++
**  ROUTINE:	mem_get_depend
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Allocates a DEPEND block.
**
**  RETURNS:	struct DEPEND *
**
**  PROTOTYPE:
**
**  	mem_get_depend()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.	(errors signalled)
**
**  SIDE EFFECTS:   	dependzone created.
**
**--
*/
struct DEPEND *mem_get_depend() {

    struct DEPEND *c;
    unsigned int status;

    if (!dependzone) {
    	unsigned int algorithm=LIB$K_VM_FIXED;
    	unsigned int flags=(LIB$M_VM_GET_FILL0|LIB$M_VM_EXTEND_AREA);
    	status = lib$create_vm_zone(&dependzone, &algorithm,
    	    	    	&DEPEND_S_DEPENDDEF, &flags);
    	if (!OK(status)) lib$signal(MMK__NOALLOC, 1, "DEPEND", status);
    }
    status = lib$get_vm(&DEPEND_S_DEPENDDEF, &c, &dependzone);
    if (!OK(status)) {
    	lib$signal(MMK__NOALLOC, 1, "DEPEND", status);
    } else {
    	c->sources.flink = c->sources.blink = &c->sources;
    }
    return c;
}

/*
**++
**  ROUTINE:	mem_free_depend
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Frees a DEPEND block.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	mem_free_depend(struct DEPEND *c)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void mem_free_depend(struct DEPEND *dep) {

    struct OBJREF *obj;

    while (queue_remove(dep->sources.flink, &obj)) mem_free_objref(obj);
    lib$free_vm(&DEPEND_S_DEPENDDEF, &dep, &dependzone);
}

/*
**++
**  ROUTINE:	mem_get_symbol
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Allocates a SYMBOL block.
**
**  RETURNS:	struct SYMBOL *
**
**  PROTOTYPE:
**
**  	mem_get_symbol()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.	(errors signalled)
**
**  SIDE EFFECTS:   	symbolzone created.
**
**--
*/
struct SYMBOL *mem_get_symbol() {

    struct SYMBOL *c;
    unsigned int status;

    if (!symbolzone) {
    	unsigned int algorithm=LIB$K_VM_FIXED;
    	unsigned int flags=(LIB$M_VM_GET_FILL0|LIB$M_VM_EXTEND_AREA);
    	status = lib$create_vm_zone(&symbolzone, &algorithm,
    	    	    	&SYMBOL_S_SYMBOLDEF, &flags);
    	if (!OK(status)) lib$signal(MMK__NOALLOC, 1, "SYMBOL", status);
    }
    status = lib$get_vm(&SYMBOL_S_SYMBOLDEF, &c, &symbolzone);
    if (!OK(status)) {
    	lib$signal(MMK__NOALLOC, 1, "SYMBOL", status);
    }

    return c;
}

/*
**++
**  ROUTINE:	mem_free_symbol
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Frees a SYMBOL block.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	mem_free_symbol(struct SYMBOL *c)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void mem_free_symbol(struct SYMBOL *sym) {

    if (sym->value) free(sym->value);
    lib$free_vm(&SYMBOL_S_SYMBOLDEF, &sym, &symbolzone);

}

/*
**++
**  ROUTINE:	mem_get_rule
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Allocates a RULE block.
**
**  RETURNS:	struct RULE *
**
**  PROTOTYPE:
**
**  	mem_get_rule()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.	(errors signalled)
**
**  SIDE EFFECTS:   	rulezone created.
**
**--
*/
struct RULE *mem_get_rule() {

    struct RULE *c;
    unsigned int status;

    if (!rulezone) {
    	unsigned int algorithm=LIB$K_VM_FIXED;
    	unsigned int flags=(LIB$M_VM_GET_FILL0|LIB$M_VM_EXTEND_AREA);
    	status = lib$create_vm_zone(&rulezone, &algorithm,
    	    	    	&RULE_S_RULEDEF, &flags);
    	if (!OK(status)) lib$signal(MMK__NOALLOC, 1, "RULE", status);
    }
    status = lib$get_vm(&RULE_S_RULEDEF, &c, &rulezone);
    if (!OK(status)) {
    	lib$signal(MMK__NOALLOC, 1, "RULE", status);
    } else {
    	c->cmdque.flink = c->cmdque.blink = &c->cmdque;
    }

    return c;
}

/*
**++
**  ROUTINE:	mem_free_rule
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Frees a RULE block.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	mem_free_rule(struct RULE *c)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void mem_free_rule(struct RULE *r) {

    struct CMD *cmd;

    while (queue_remove(r->cmdque.flink, &cmd)) mem_free_cmd(cmd);
    lib$free_vm(&RULE_S_RULEDEF, &r, &rulezone);

}

/*
**++
**  ROUTINE:	mem_get_object
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Allocates an OBJECT block.
**
**  RETURNS:	struct OBJECT *
**
**  PROTOTYPE:
**
**  	mem_get_object()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.	(errors signalled)
**
**  SIDE EFFECTS:   	objectzone created.
**
**--
*/
struct OBJECT *mem_get_object() {

    struct OBJECT *c;
    unsigned int status;

    if (!objectzone) {
    	unsigned int algorithm=LIB$K_VM_FIXED;
    	unsigned int flags=(LIB$M_VM_GET_FILL0|LIB$M_VM_EXTEND_AREA);
    	status = lib$create_vm_zone(&objectzone, &algorithm,
    	    	    	&OBJECT_S_OBJECTDEF, &flags);
    	if (!OK(status)) lib$signal(MMK__NOALLOC, 1, "OBJECT", status);
    }
    status = lib$get_vm(&OBJECT_S_OBJECTDEF, &c, &objectzone);
    if (!OK(status)) {
    	lib$signal(MMK__NOALLOC, 1, "OBJECT", status);
    }
    c->libmodque.head = c->libmodque.tail = &c->libmodque;
    return c;
}

/*
**++
**  ROUTINE:	mem_free_object
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Frees a OBJECT block.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	mem_free_object(struct OBJECT *c)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void mem_free_object(struct OBJECT *obj) {

    lib$free_vm(&OBJECT_S_OBJECTDEF, &obj, &objectzone);

}

/*
**++
**  ROUTINE:	mem_get_objref
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Allocates an OBJREF block.
**
**  RETURNS:	struct OBJREF *
**
**  PROTOTYPE:
**
**  	mem_get_objref()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.	(errors signalled)
**
**  SIDE EFFECTS:   	objrefzone created.
**
**--
*/
struct OBJREF *mem_get_objref() {

    struct OBJREF *c;
    unsigned int status;

    if (!objrefzone) {
    	unsigned int algorithm=LIB$K_VM_FIXED;
    	unsigned int flags=(LIB$M_VM_GET_FILL0|LIB$M_VM_EXTEND_AREA);
    	status = lib$create_vm_zone(&objrefzone, &algorithm,
    	    	    	&OBJREF_S_OBJREFDEF, &flags);
    	if (!OK(status)) lib$signal(MMK__NOALLOC, 1, "OBJREF", status);
    }
    status = lib$get_vm(&OBJREF_S_OBJREFDEF, &c, &objrefzone);
    if (!OK(status)) {
    	lib$signal(MMK__NOALLOC, 1, "OBJREF", status);
    }
    return c;
}

/*
**++
**  ROUTINE:	mem_free_objref
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Frees a OBJREF block.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	mem_free_objref(struct OBJREF *c)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void mem_free_objref(struct OBJREF *obj) {

    lib$free_vm(&OBJREF_S_OBJREFDEF, &obj, &objrefzone);

}

/*
**++
**  ROUTINE:	mem_get_sfx
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Allocates a SFX block.
**
**  RETURNS:	struct SFX *
**
**  PROTOTYPE:
**
**  	mem_get_sfx()
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.	(errors signalled)
**
**  SIDE EFFECTS:   	sfxzone created.
**
**--
*/
struct SFX *mem_get_sfx() {

    struct SFX *c;
    unsigned int status;

    if (!sfxzone) {
    	unsigned int algorithm=LIB$K_VM_FIXED;
    	unsigned int flags=(LIB$M_VM_GET_FILL0|LIB$M_VM_EXTEND_AREA);
    	status = lib$create_vm_zone(&sfxzone, &algorithm,
    	    	    	&SFX_S_SFXDEF, &flags);
    	if (!OK(status)) lib$signal(MMK__NOALLOC, 1, "SFX", status);
    }
    status = lib$get_vm(&SFX_S_SFXDEF, &c, &sfxzone);
    if (!OK(status)) {
    	lib$signal(MMK__NOALLOC, 1, "SFX", status);
    }

    return c;
}

/*
**++
**  ROUTINE:	mem_free_sfx
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Frees a SFX block.
**
**  RETURNS:	void
**
**  PROTOTYPE:
**
**  	mem_free_sfx(struct SFX *c)
**
**  IMPLICIT INPUTS:	None.
**
**  IMPLICIT OUTPUTS:	None.
**
**  COMPLETION CODES:	None.
**
**  SIDE EFFECTS:   	None.
**
**--
*/
void mem_free_sfx(struct SFX *s) {

    lib$free_vm(&SFX_S_SFXDEF, &s, &sfxzone);

}
