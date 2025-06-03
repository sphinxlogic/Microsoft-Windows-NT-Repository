/*
**++
**  FACILITY:	MMK
**
**  ABSTRACT:	Map the default rules.
**
**  MODULE DESCRIPTION:
**
**  	Builds the symbols, rules, and suffixes lists from the
**  default rules compiled using MMK_COMPILE_RULES.
**
**  AUTHOR: 	    M. Madison
**  	    	    COPYRIGHT © 1993, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  CREATION DATE:  30-APR-1993
**
**  MODIFICATION HISTORY:
**
**  	30-APR-1993 V1.0    Madison 	Initial coding.
**  	04-JUN-1993 V1.1    Madison 	Default rule support.
**--
*/

#ifdef __DECC
#pragma module DEFAULT_RULES "V1.1"
#else
#ifndef __GNUC__
#module DEFAULT_RULES "V1.1"
#endif
#endif
#include "mmk.h"
#include "globals.h"
#ifdef __ALPHA
#include "mmk_default_rules_axp.h"
#else
#include "mmk_default_rules.h"
#endif


/*
**++
**  ROUTINE:	Map_Default_Rules
**
**  FUNCTIONAL DESCRIPTION:
**
**  	Builds the symbols, rules, and suffixes lists from the
**  default rules compiled using MMK_COMPILE_RULES.
**
**  RETURNS:	cond_value, longword (unsigned), write only, by value
**
**  PROTOTYPE:
**
**  	Map_Default_Rules();
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
void Map_Default_Rules() {

    int i, j, next_rule, last_rule;
    char *cp;

    if (SFX_INIT_COUNT > 0) {
    	suffixes.flink = &sfx_init[0];
    	suffixes.blink = &sfx_init[SFX_INIT_COUNT-1];
    	sfx_init[0].flink = (SFX_INIT_COUNT > 1 ? &sfx_init[1] : &suffixes);
    	sfx_init[0].blink = (struct SFX *) &suffixes;
    	sfx_init[SFX_INIT_COUNT-1].flink = &suffixes;
    	sfx_init[SFX_INIT_COUNT-1].blink = 
    	    (SFX_INIT_COUNT > 1 ? &sfx_init[SFX_INIT_COUNT-2] :
    	    	(struct SFX *) &suffixes);
    	for (i = 1; i < SFX_INIT_COUNT-1; i++) {
    	    sfx_init[i].flink = &sfx_init[i+1];
    	    sfx_init[i].blink = &sfx_init[i-1];
    	}
    }

    if (GSYM_INIT_COUNT > 0) {
    	global_symbols.flink = &gsym_init[0];
    	global_symbols.blink = &gsym_init[GSYM_INIT_COUNT-1];
    	gsym_init[0].flink = (GSYM_INIT_COUNT > 1 ?
    	    	    &gsym_init[1] : &global_symbols);
    	gsym_init[0].blink = (struct SYMBOL *) &global_symbols;
    	gsym_init[GSYM_INIT_COUNT-1].flink = &global_symbols;
    	gsym_init[GSYM_INIT_COUNT-1].blink = 
    	    (GSYM_INIT_COUNT > 1 ? &gsym_init[GSYM_INIT_COUNT-2] :
    	    	(struct SYMBOL *) &global_symbols);
    	for (i = 0; i < GSYM_INIT_COUNT; i++) {
    	    cp = malloc(strlen(gsym_init[i].value)+1);
    	    strcpy(cp, gsym_init[i].value);
    	    gsym_init[i].value = cp;
    	    if (i == 0 || i == GSYM_INIT_COUNT-1) continue;
    	    gsym_init[i].flink = &gsym_init[i+1];
    	    gsym_init[i].blink = &gsym_init[i-1];
    	}
    }

    if (RULE_INIT_COUNT > 0) {
    	rules.flink = &rule_init[0];
    	rules.blink = &rule_init[RULE_REAL_LAST_IDX];
    	last_rule = i = 0;
    	while (i < RULE_INIT_COUNT) {
    	    if (rule_init[i].next != 0) {
    	    	for (j = i+1; j < RULE_INIT_COUNT; j++) {
    	    	    rule_init[j-1].next = &rule_init[j];
    	    	    if (rule_init[j].next == 0) break;
    	    	}
    	    	next_rule = j + 1;
    	    } else {
    	    	next_rule = i + 1;
    	    }
    	    if (i == 0) {
    	    	rule_init[i].flink = (RULE_INIT_COUNT_REAL > 1 ? &rule_init[next_rule] :
    	    	    	(struct RULE *) &rules);
    	    	rule_init[i].blink = (struct RULE *) &rules;
    	    } else if (i == RULE_REAL_LAST_IDX) {
    	    	rule_init[i].flink = &rules;
    	    	rule_init[i].blink = 
    	    	    (RULE_INIT_COUNT_REAL > 1 ? &rule_init[last_rule] :
    	    	    	(struct RULE *) &rules);
    	    } else {
    	    	rule_init[i].flink = &rule_init[next_rule];
    	    	rule_init[i].blink = &rule_init[last_rule];
    	    }

    	    if (rule_init[i].cmdque.flags > 0) {
    	    	struct CMD *cmd_init = rule_init[i].cmdque.flink;
    	    	for (j = 0; j < rule_init[i].cmdque.flags; j++) {
    	    	    if (j == 0) {
    	    	    	cmd_init[j].flink =
    	    	    	    (rule_init[i].cmdque.flags > 1 ? &cmd_init[1] :
    	    	    	    	&rule_init[i].cmdque);
    	    	    	cmd_init[j].blink = &rule_init[i].cmdque;
    	    	    } else if (j == rule_init[i].cmdque.flags-1) {
    	    	    	cmd_init[j].flink = &rule_init[i].cmdque;
    	    	    	cmd_init[j].blink = 
    	    	    	    (rule_init[i].cmdque.flags > 1 ? &cmd_init[j-1] :
    	    	    	    &rule_init[i].cmdque);
    	    	    	rule_init[i].cmdque.blink = &cmd_init[j];
    	    	    } else {
    	    	    	cmd_init[j].flink = &cmd_init[j+1];
    	    	    	cmd_init[j].blink = &cmd_init[j-1];
    	    	    }
    	    	}
    	    }
    	    last_rule = i;
    	    i = next_rule;
    	}
    }

#ifdef DEFAULT_RULE_EXISTS
    default_rule = &default_rule_init;
    if (default_rule_init.cmdque.flags > 0) {
    	struct CMD *cmd_init = default_rule_init.cmdque.flink;
    	for (j = 0; j < default_rule_init.cmdque.flags; j++) {
    	    if (j == 0) {
    	    	cmd_init[j].flink =
    	    	    (default_rule_init.cmdque.flags > 1 ? &cmd_init[1] :
    	    	    	    	&default_rule_init.cmdque);
    	    	cmd_init[j].blink = &default_rule_init.cmdque;
    	    } else if (j == default_rule_init.cmdque.flags-1) {
    	    	cmd_init[j].flink = &default_rule_init.cmdque;
    	    	cmd_init[j].blink = 
    	    	    	(default_rule_init.cmdque.flags > 1 ? &cmd_init[j-1] :
    	    	    	&default_rule_init.cmdque);
    	    	default_rule_init.cmdque.blink = &cmd_init[j];
    	    } else {
    	    	cmd_init[j].flink = &cmd_init[j+1];
    	    	cmd_init[j].blink = &cmd_init[j-1];
    	    }
    	}
    }
#endif /* DEFAULT_RULE_EXISTS */

    return;

} /* Map_Default_Rules */
