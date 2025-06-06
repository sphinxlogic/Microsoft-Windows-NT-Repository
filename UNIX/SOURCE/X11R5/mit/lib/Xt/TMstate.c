/* $XFree86: mit/lib/Xt/TMstate.c,v 1.4 1993/05/19 05:30:34 dawes Exp $ */
/* $XConsortium: TMstate.c,v 1.164 93/02/05 16:47:46 converse Exp $ */
/*LINTLIBRARY*/

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* TMstate.c -- maintains the state table of actions for the translation 
 *              manager.
 */

#include "IntrinsicI.h"
#include "StringDefs.h"

#ifndef TM_NO_MATCH
#define TM_NO_MATCH (-2)
#endif /* TM_NO_MATCH */

/* forward definitions */
static StatePtr NewState();


static String XtNtranslationError = "translationError";

#ifndef SVR3SHLIB
TMGlobalRec _XtGlobalTM; /* initialized to zero K&R */
#endif

#define MatchIncomingEvent(tmEvent, typeMatch, modMatch) \
  (typeMatch->eventType == tmEvent->event.eventType && \
   (typeMatch->matchEvent != NULL) && \
   (*typeMatch->matchEvent)(typeMatch, modMatch, tmEvent))


#define NumStateTrees(xlations) \
  ((translateData->isSimple) ? 1 : (TMComplexXlations(xlations))->numTrees)

static TMShortCard GetBranchHead(parseTree, typeIndex, modIndex, isDummy)
    TMParseStateTree	parseTree;
    TMShortCard		typeIndex;
    TMShortCard		modIndex;
    Boolean		isDummy;
{
#define TM_BRANCH_HEAD_TBL_ALLOC 	8
#define TM_BRANCH_HEAD_TBL_REALLOC 	8

    register TMBranchHead branchHead = parseTree->branchHeadTbl;
    TMShortCard	newSize, i;

    /*
     * dummy is used as a place holder for later matching in old-style
     * matching behavior. If there's already an entry we don't need
     * another dummy.
     */
    if (isDummy) {
	for (i = 0; i < parseTree->numBranchHeads; i++, branchHead++) {
	    if ((branchHead->typeIndex == typeIndex) &&
		(branchHead->modIndex == modIndex))
	      return i;
	}
    }
    if (parseTree->numBranchHeads == parseTree->branchHeadTblSize)
      {
	  if (parseTree->branchHeadTblSize == 0)
	    parseTree->branchHeadTblSize += TM_BRANCH_HEAD_TBL_ALLOC;
	  else
	    parseTree->branchHeadTblSize +=
	      TM_BRANCH_HEAD_TBL_REALLOC;
	  newSize = (parseTree->branchHeadTblSize * sizeof(TMBranchHeadRec));
	  if (parseTree->isStackBranchHeads) {
	      TMBranchHead	oldBranchHeadTbl = parseTree->branchHeadTbl;
	      parseTree->branchHeadTbl = (TMBranchHead) XtMalloc(newSize);
	      XtBCopy(oldBranchHeadTbl, parseTree->branchHeadTbl, newSize);
	      parseTree->isStackBranchHeads = False;
	  }
	  else {
	      parseTree->branchHeadTbl = (TMBranchHead) 
		XtRealloc((char *)parseTree->branchHeadTbl, 
			  (parseTree->branchHeadTblSize *
			   sizeof(TMBranchHeadRec)));
	  }
      }
#ifdef TRACE_TM
    _XtGlobalTM.numBranchHeads++;
#endif /* TRACE_TM */
    branchHead = 
      &parseTree->branchHeadTbl[parseTree->numBranchHeads++];
    branchHead->typeIndex = typeIndex;
    branchHead->modIndex = modIndex;
    branchHead->more = 0;
    branchHead->isSimple = True;
    branchHead->hasActions = False;
    branchHead->hasCycles = False;
    return parseTree->numBranchHeads-1;
}

TMShortCard _XtGetQuarkIndex(parseTree, quark)
    TMParseStateTree	parseTree;
    XrmQuark		quark;
{
#define TM_QUARK_TBL_ALLOC 	16
#define TM_QUARK_TBL_REALLOC 	16
    register TMShortCard  i = parseTree->numQuarks;

    for (i=0; i < parseTree->numQuarks; i++)
      if (parseTree->quarkTbl[i] == quark)
	    break;

    if (i == parseTree->numQuarks)
      {
	  if (parseTree->numQuarks == parseTree->quarkTblSize)
	    {
		TMShortCard	newSize;

		if (parseTree->quarkTblSize == 0)
		  parseTree->quarkTblSize += TM_QUARK_TBL_ALLOC;
		else
		  parseTree->quarkTblSize += TM_QUARK_TBL_REALLOC;
		newSize = (parseTree->quarkTblSize * sizeof(XrmQuark));

		if (parseTree->isStackQuarks) {
		    XrmQuark	*oldquarkTbl = parseTree->quarkTbl;
		    parseTree->quarkTbl = (XrmQuark *) XtMalloc(newSize);
		    XtBCopy(oldquarkTbl, parseTree->quarkTbl, newSize);
		    parseTree->isStackQuarks = False;
		}
		else {
		    parseTree->quarkTbl = (XrmQuark *) 
		      XtRealloc((char *)parseTree->quarkTbl, 
				(parseTree->quarkTblSize *
				 sizeof(XrmQuark)));
		}
	    }
	  parseTree->quarkTbl[parseTree->numQuarks++] = quark;
      }
    return i;
}

/*
 * Get an entry from the parseTrees complex branchHead tbl. If there's none
 * there then allocate one
 */
/*ARGSUSED*/
static TMShortCard GetComplexBranchIndex(parseTree, typeIndex, modIndex)
    TMParseStateTree	parseTree;
    TMShortCard		typeIndex;
    TMShortCard		modIndex;
{
#define TM_COMPLEXBRANCH_HEAD_TBL_ALLOC 8
#define TM_COMPLEXBRANCH_HEAD_TBL_REALLOC 4
    
    if (parseTree->numComplexBranchHeads == parseTree->complexBranchHeadTblSize) {
	TMShortCard	newSize;
	
	if (parseTree->complexBranchHeadTblSize == 0)
	  parseTree->complexBranchHeadTblSize += TM_COMPLEXBRANCH_HEAD_TBL_ALLOC;
	else
	  parseTree->complexBranchHeadTblSize += TM_COMPLEXBRANCH_HEAD_TBL_REALLOC;
	
	newSize = (parseTree->complexBranchHeadTblSize * sizeof(StatePtr));
	
	if (parseTree->isStackComplexBranchHeads) {
	    StatePtr *oldcomplexBranchHeadTbl 
	      = parseTree->complexBranchHeadTbl;
	    parseTree->complexBranchHeadTbl = (StatePtr *) XtMalloc(newSize);
	    XtBCopy(oldcomplexBranchHeadTbl, parseTree->complexBranchHeadTbl,
		    newSize);
	    parseTree->isStackComplexBranchHeads = False;
	}
	else {
	    parseTree->complexBranchHeadTbl = (StatePtr *) 
	      XtRealloc((char *)parseTree->complexBranchHeadTbl, 
			(parseTree->complexBranchHeadTblSize *
			 sizeof(StatePtr)));	
	}
    }
    parseTree->complexBranchHeadTbl[parseTree->numComplexBranchHeads++] = NULL;
    return parseTree->numComplexBranchHeads-1;
}

TMShortCard _XtGetTypeIndex(event)
    Event	*event;
{
    TMShortCard		i, j = TM_TYPE_SEGMENT_SIZE;
    TMShortCard		typeIndex = 0;
    TMTypeMatch 	typeMatch;
    TMTypeMatch		segment;

    for (i = 0; i < _XtGlobalTM.numTypeMatchSegments; i++) {
	segment = _XtGlobalTM.typeMatchSegmentTbl[i];
	for (j = 0; 
	     typeIndex < _XtGlobalTM.numTypeMatches && j <  TM_TYPE_SEGMENT_SIZE;
	     j++, typeIndex++) 
	  {
	      typeMatch = &(segment[j]);
	      if (event->eventType == typeMatch->eventType && 
		  event->eventCode == typeMatch->eventCode && 
		  event->eventCodeMask == typeMatch->eventCodeMask &&
		  event->matchEvent == typeMatch->matchEvent)
		    return typeIndex;
	  }
    }
    
    if (j == TM_TYPE_SEGMENT_SIZE) {
	if (_XtGlobalTM.numTypeMatchSegments == _XtGlobalTM.typeMatchSegmentTblSize) {
	    _XtGlobalTM.typeMatchSegmentTblSize += 4;
	    _XtGlobalTM.typeMatchSegmentTbl = (TMTypeMatch *)
	      XtRealloc((char *)_XtGlobalTM.typeMatchSegmentTbl,
			(_XtGlobalTM.typeMatchSegmentTblSize * sizeof(TMTypeMatch)));
	}
	_XtGlobalTM.typeMatchSegmentTbl[_XtGlobalTM.numTypeMatchSegments++] = 
	  segment = (TMTypeMatch)
	    XtMalloc(TM_TYPE_SEGMENT_SIZE * sizeof(TMTypeMatchRec));
	j = 0;
    }
    typeMatch = &segment[j];
    typeMatch->eventType = event->eventType;
    typeMatch->eventCode = event->eventCode;
    typeMatch->eventCodeMask = event->eventCodeMask;
    typeMatch->matchEvent = event->matchEvent;
    _XtGlobalTM.numTypeMatches++;
    return typeIndex;
}

static Boolean CompareLateModifiers(lateBind1P, lateBind2P)
    LateBindingsPtr lateBind1P, lateBind2P;
{
    LateBindingsPtr late1P = lateBind1P;
    LateBindingsPtr late2P = lateBind2P;

    if (late1P != NULL || late2P != NULL) {
	int i = 0;
	int j = 0;
	if (late1P != NULL)
	  for (; late1P->keysym != NoSymbol; i++) late1P++;
	if (late2P != NULL)
	  for (; late2P->keysym != NoSymbol; j++) late2P++;
	if (i != j) return FALSE;
	late1P--;
	while (late1P >= lateBind1P) {
	    Boolean last = True;
	    for (late2P = lateBind2P + i - 1;
		 late2P >= lateBind2P;
		 late2P--) {
		if (late1P->keysym == late2P->keysym
		    && late1P->knot == late2P->knot) {
		    j--;
		    if (last) i--;
		    break;
		}
		last = False;
	    }
	    late1P--;
	}
	if (j != 0) return FALSE;
    }
    return TRUE;
}

TMShortCard _XtGetModifierIndex(event)
    Event	*event;
{
    TMShortCard		i, j = TM_MOD_SEGMENT_SIZE;
    TMShortCard		modIndex = 0;
    TMModifierMatch 	modMatch;
    TMModifierMatch	segment;
    for (i = 0; i < _XtGlobalTM.numModMatchSegments; i++) {
	segment = _XtGlobalTM.modMatchSegmentTbl[i];
	for (j = 0; 
	     modIndex < _XtGlobalTM.numModMatches && j <  TM_MOD_SEGMENT_SIZE;
	     j++, modIndex++) {
	    modMatch = &(segment[j]);
	    if (event->modifiers == modMatch->modifiers && 
		event->modifierMask == modMatch->modifierMask && 
		event->standard == modMatch->standard && 
		((!event->lateModifiers && !modMatch->lateModifiers) || 
		 CompareLateModifiers(event->lateModifiers,
				      modMatch->lateModifiers))) {
		/*
		 * if we found a match then we can free the parser's
		 * late modifiers. If there isn't a match we use the
		 * parser's copy
		 */
		if (event->lateModifiers &&
		    --event->lateModifiers->ref_count == 0) {
		    XtFree((char *)event->lateModifiers);
		    event->lateModifiers = NULL;
		}
		return modIndex;
	    }
	}
    }
    
    if (j == TM_MOD_SEGMENT_SIZE) {
	if (_XtGlobalTM.numModMatchSegments == _XtGlobalTM.modMatchSegmentTblSize) {
	    _XtGlobalTM.modMatchSegmentTblSize += 4;
	    _XtGlobalTM.modMatchSegmentTbl = (TMModifierMatch *)
	      XtRealloc((char *)_XtGlobalTM.modMatchSegmentTbl,
			(_XtGlobalTM.modMatchSegmentTblSize * sizeof(TMModifierMatch)));
	}
	_XtGlobalTM.modMatchSegmentTbl[_XtGlobalTM.numModMatchSegments++] = 
	  segment = (TMModifierMatch)
	    XtMalloc(TM_MOD_SEGMENT_SIZE * sizeof(TMModifierMatchRec));
	j = 0;
    }
    modMatch = &segment[j];
    modMatch->modifiers = event->modifiers;;
    modMatch->modifierMask = event->modifierMask;
    modMatch->standard = event->standard;
    /*
     * We use the parser's copy of the late binding array
     */
#ifdef TRACE_TM
    if (event->lateModifiers)
      _XtGlobalTM.numLateBindings++;
#endif /* TRACE_TM */
    modMatch->lateModifiers = event->lateModifiers;
    _XtGlobalTM.numModMatches++;
    return modIndex;
}


/*
 * This is called from the SimpleStateHandler to match a stateTree
 * entry to the event coming in
 */
static int MatchBranchHead(stateTree, startIndex, event) 
    TMSimpleStateTree 	stateTree;
    int			startIndex;
    register TMEventPtr	event;
{
    register TMBranchHead
      branchHead = &stateTree->branchHeadTbl[startIndex];
    register int i;

    for (i = startIndex;
	 i < (int)stateTree->numBranchHeads; 
	 i++, branchHead++)
      {
	  TMTypeMatch 		typeMatch;
	  TMModifierMatch	modMatch;
	  
	  typeMatch  = TMGetTypeMatch(branchHead->typeIndex);
	  modMatch = TMGetModifierMatch(branchHead->modIndex);
	  
	  if (MatchIncomingEvent(event, typeMatch, modMatch))
	    return i;
      }    
    return (TM_NO_MATCH);
}

Boolean _XtRegularMatch(typeMatch, modMatch, eventSeq)
    TMTypeMatch 	typeMatch;
    TMModifierMatch	modMatch;
    TMEventPtr 		eventSeq;
{
    Modifiers computed =0;
    Modifiers computedMask =0;
    Boolean resolved = TRUE;
    if (typeMatch->eventCode != (eventSeq->event.eventCode &
				  typeMatch->eventCodeMask)) return FALSE;
    if (modMatch->lateModifiers != NULL)
	resolved = _XtComputeLateBindings(eventSeq->xev->xany.display,
					  modMatch->lateModifiers,
					  &computed, &computedMask);
    if (!resolved) return FALSE;
    computed |= modMatch->modifiers;
    computedMask |= modMatch->modifierMask;

    return ( (computed & computedMask) ==
	    (eventSeq->event.modifiers & computedMask));
}

/*ARGSUSED*/
Boolean _XtMatchAtom(typeMatch, modMatch, eventSeq)
    TMTypeMatch 	typeMatch;
    TMModifierMatch	modMatch;
    TMEventPtr 		eventSeq;
{
    Atom	atom;

    atom = XInternAtom(eventSeq->xev->xany.display, 
		       XrmQuarkToString(typeMatch->eventCode),
		       False);
    return (atom == eventSeq->event.eventCode);
}

#define IsOn(vec,idx) ((vec)[(idx)>>3] & (1 << ((idx) & 7)))

/*
 * there are certain cases where you want to ignore the event and stay
 * in the same state.
 */
static Boolean Ignore(event)
    register TMEventPtr event;
{
    register Display *dpy;
    register XtPerDisplay pd;

    if (event->event.eventType == MotionNotify)
	return TRUE;
    if (!(event->event.eventType == KeyPress ||
	  event->event.eventType == KeyRelease))
	return FALSE;
    dpy = event->xev->xany.display;
    pd = _XtGetPerDisplay(dpy);
    _InitializeKeysymTables(dpy, pd);
    return IsOn(pd->isModifier, event->event.eventCode) ? TRUE : FALSE;
}


static void XEventToTMEvent(event, tmEvent)
    register XEvent *event;
    register TMEventPtr tmEvent;
{
    tmEvent->xev = event;
    tmEvent->event.eventCodeMask = 0;
    tmEvent->event.modifierMask = 0;
    tmEvent->event.eventType = event->type;
    tmEvent->event.lateModifiers = NULL;
    tmEvent->event.matchEvent = NULL;
    tmEvent->event.standard = FALSE;

    switch (event->type) {

	case KeyPress:
	case KeyRelease:
            tmEvent->event.eventCode = event->xkey.keycode;
	    tmEvent->event.modifiers = event->xkey.state;
	    break;

	case ButtonPress:
	case ButtonRelease:
	    tmEvent->event.eventCode = event->xbutton.button;
	    tmEvent->event.modifiers = event->xbutton.state;
	    break;

	case MotionNotify:
	    tmEvent->event.eventCode = event->xmotion.is_hint;
	    tmEvent->event.modifiers = event->xmotion.state;
	    break;

	case EnterNotify:
	case LeaveNotify:
	    tmEvent->event.eventCode = event->xcrossing.mode;
	    tmEvent->event.modifiers = event->xcrossing.state;
	    break;

	case PropertyNotify:
	    tmEvent->event.eventCode = event->xproperty.atom;
	    tmEvent->event.modifiers = 0;
	    break;

	case SelectionClear:
	    tmEvent->event.eventCode = event->xselectionclear.selection;
	    tmEvent->event.modifiers = 0;
	    break;

	case SelectionRequest:
	    tmEvent->event.eventCode = event->xselectionrequest.selection;
	    tmEvent->event.modifiers = 0;
	    break;

	case SelectionNotify:
	    tmEvent->event.eventCode = event->xselection.selection;
	    tmEvent->event.modifiers = 0;
	    break;

	case ClientMessage:
	    tmEvent->event.eventCode = event->xclient.message_type;
	    tmEvent->event.modifiers = 0;
	    break;

	case MappingNotify:
	    tmEvent->event.eventCode = event->xmapping.request;
	    tmEvent->event.modifiers = 0;
	    break;

	case FocusIn:
	case FocusOut:
	    tmEvent->event.eventCode = event->xfocus.mode;
	    tmEvent->event.modifiers = 0;
	    break;

	default:
	    tmEvent->event.eventCode = 0;
	    tmEvent->event.modifiers = 0;
	    break;
    }
}


static unsigned long GetTime(tm, event)
    XtTM tm;
    register XEvent *event;
{
    switch (event->type) {

        case KeyPress:
	case KeyRelease:
	    return event->xkey.time;

        case ButtonPress:
	case ButtonRelease:
	    return event->xbutton.time;

	default:
	    return tm->lastEventTime;

    }

}

static void HandleActions(w, event, stateTree, accelWidget, procs, actions)
    Widget		w;
    XEvent		*event;
    TMSimpleStateTree	stateTree;
    Widget		accelWidget;
    XtActionProc	*procs;
    ActionRec		*actions;
{
    ActionHook	 	actionHookList;
    Widget		bindWidget;

    bindWidget = accelWidget ? accelWidget : w;
    if (!XtIsSensitive(bindWidget))
      return;

    actionHookList = XtWidgetToApplicationContext(w)->action_hook_list;

    while (actions != NULL) {
	/* perform any actions */
	if (procs[actions->idx] != NULL) {
	    if (actionHookList) {
		ActionHook hook;
		String procName =
		    XrmQuarkToString(stateTree->quarkTbl[actions->idx] );
	    
		for (hook = actionHookList; hook != NULL; hook = hook->next) {
		    (*hook->proc)(bindWidget,
				  hook->closure,
				  procName,
				  event,
				  actions->params,
				  &actions->num_params
				  );
		}
	    }
	    (*(procs[actions->idx]))
	      (bindWidget, event, 
	       actions->params, &actions->num_params );
	}
	actions = actions->next;
    }
}

typedef struct {
    unsigned int isCycleStart:1;
    unsigned int isCycleEnd:1;
    TMShortCard typeIndex;
    TMShortCard modIndex;
}MatchPairRec, *MatchPair;

typedef struct TMContextRec{
    TMShortCard	numMatches;
    TMShortCard	maxMatches;
    MatchPair	matches;
}TMContextRec, *TMContext;

static TMContextRec	contextCache[2];

#define GetContextPtr(tm) ((TMContext *)&(tm->current_state))

#define TM_CONTEXT_MATCHES_ALLOC 4
#define TM_CONTEXT_MATCHES_REALLOC 2

static void PushContext(contextPtr, newState)
    TMContext	*contextPtr;
    StatePtr	newState;
{
    TMContext 		context = *contextPtr;
    
    if (context == NULL)
      {
	  if (contextCache[0].numMatches == 0)
	    context = &contextCache[0];
	  else if (contextCache[1].numMatches == 0)
	    context = &contextCache[1];
	  if (!context)
	    {
		context = XtNew(TMContextRec);
		context->matches = NULL;
		context->numMatches =
		  context->maxMatches = 0;
	    }
      }
    if (context->numMatches && 
	context->matches[context->numMatches-1].isCycleEnd)
      {
	  TMShortCard	i;
	  for (i = 0; 
	       i < context->numMatches &&
	       !(context->matches[i].isCycleStart);
	       i++){};
	  if (i < context->numMatches)
	    context->numMatches = i+1;
#ifdef DEBUG
	  else
	    XtWarning("pushing cycle end with no cycle start");
#endif /* DEBUG */	  
      }
    else 
      {
	  if (context->numMatches == context->maxMatches)
	    {
		if (context->maxMatches == 0)
		  context->maxMatches += TM_CONTEXT_MATCHES_ALLOC;
		else
		  context->maxMatches += TM_CONTEXT_MATCHES_REALLOC;
		context->matches = (MatchPairRec *) 
		  XtRealloc((char *)context->matches,
			    context->maxMatches * sizeof(MatchPairRec));
	    }
	  context->matches[context->numMatches].isCycleStart = newState->isCycleStart;
	  context->matches[context->numMatches].isCycleEnd = newState->isCycleEnd;
	  context->matches[context->numMatches].typeIndex = newState->typeIndex;
	  context->matches[context->numMatches++].modIndex = newState->modIndex;
	  *contextPtr = context;
      }
}
static void FreeContext(contextPtr)
    TMContext	*contextPtr;
{
    TMContext 		context = NULL;
    
    if (&contextCache[0] == *contextPtr)
      context = &contextCache[0];
    else if (&contextCache[1] == *contextPtr)
      context = &contextCache[1];
    if (context)
      context->numMatches = 0;
    else
      XtFree((char *)*contextPtr);
    
    *contextPtr = NULL;
}

static int MatchExact(stateTree, startIndex, typeIndex, modIndex) 
    TMSimpleStateTree 	stateTree;
    int			startIndex;
    TMShortCard		typeIndex, modIndex;
{
    register TMBranchHead branchHead = &(stateTree->branchHeadTbl[startIndex]);
    register int i;

    for (i = startIndex;
	 i < (int)stateTree->numBranchHeads; 
	 i++, branchHead++)
      {
	  if ((branchHead->typeIndex == typeIndex) &&
	      (branchHead->modIndex == modIndex))
	    return i;
      }    
    return (TM_NO_MATCH);
}



static void HandleSimpleState(w, tmRecPtr, curEventPtr)
    Widget	w;
    XtTM	tmRecPtr;
    TMEventRec	*curEventPtr;
{  
    XtTranslations	xlations = tmRecPtr->translations;
    TMSimpleStateTree	stateTree;
    TMContext		*contextPtr = GetContextPtr(tmRecPtr);
    TMShortCard		i;
    ActionRec		*actions;
    Boolean		matchExact = False;
    Boolean	       	match = False; 
    StatePtr		complexMatchState = NULL;
    int			currIndex;
    TMShortCard		typeIndex, modIndex;
    int			matchTreeIndex = TM_NO_MATCH;
    
    
    stateTree = (TMSimpleStateTree)xlations->stateTreeTbl[0];
    
    for (i = 0; 
	 ((!match || !complexMatchState) && (i < xlations->numStateTrees));
	 i++){
	stateTree = (TMSimpleStateTree)xlations->stateTreeTbl[i];
	currIndex = -1;
	/*
	 * don't process this tree if we're only looking for a
	 * complexMatchState and there are no complex states
	 */
	while (!(match && stateTree->isSimple) &&
	       ((!match || !complexMatchState) && (currIndex != TM_NO_MATCH))) {
	    currIndex++;
	    if (matchExact)
	      currIndex = MatchExact(stateTree,currIndex,typeIndex,modIndex);
	    else
	      currIndex = MatchBranchHead(stateTree,currIndex,curEventPtr);
	    if (currIndex != TM_NO_MATCH) {
		TMBranchHead branchHead;
		StatePtr currState;
		
		branchHead = &stateTree->branchHeadTbl[currIndex];
		if (branchHead->isSimple)
		  currState = NULL;
		else
		  currState = ((TMComplexStateTree)stateTree)
		    ->complexBranchHeadTbl[TMBranchMore(branchHead)];
		
		/*
		 * first check for a complete match
		 */
		if (!match) {
		    if (branchHead->hasActions) {
			if (branchHead->isSimple) {
			    static ActionRec	dummyAction;
			    
			    dummyAction.idx = TMBranchMore(branchHead);
			    actions = &dummyAction;
			}
			else 
			  actions = currState->actions;
			tmRecPtr->lastEventTime = 
			  GetTime(tmRecPtr, curEventPtr->xev);
			FreeContext((TMContext
				     *)&tmRecPtr->current_state);
			match = True;
			matchTreeIndex = i;
		    }
		    /* 
		     * if it doesn't have actions and
		     * it's bc mode then it's a potential match node that is
		     * used to match later sequences.
		     */
		    if (!TMNewMatchSemantics() && !matchExact) {
			matchExact = True;
			typeIndex = branchHead->typeIndex;
			modIndex = branchHead->modIndex;
		    }
		}
		/*
		 * check for it being an event sequence which can be
		 * a future match 
		 */
		if (!branchHead->isSimple &&
		    !branchHead->hasActions &&
		    !complexMatchState) 
		  complexMatchState = currState;
	    }
	}
    }
    if (match)
      {
	  TMBindData	bindData = (TMBindData) tmRecPtr->proc_table;
	  XtActionProc	*procs;
	  Widget	accelWidget;

	  if (bindData->simple.isComplex) {
	      TMComplexBindProcs bindProcs =
		TMGetComplexBindEntry(bindData, matchTreeIndex);
	      procs = bindProcs->procs;
	      accelWidget = bindProcs->widget;
	  }
	  else {
	      TMSimpleBindProcs bindProcs = 
		TMGetSimpleBindEntry(bindData, matchTreeIndex);
	      procs = bindProcs->procs;
	      accelWidget = NULL;
	  }
	  HandleActions
	    (w, 
	     curEventPtr->xev, 
	     (TMSimpleStateTree)xlations->stateTreeTbl[matchTreeIndex],
	     accelWidget,
	     procs,
	     actions);
      }
    if (complexMatchState)
      PushContext(contextPtr, complexMatchState);
}

static int MatchComplexBranch(stateTree, startIndex, context, leafStateRtn)
    TMComplexStateTree	stateTree;
    int			startIndex;
    TMContext		context;
    StatePtr		*leafStateRtn;
{
    TMShortCard	i;

    for (i = startIndex; i < stateTree->numComplexBranchHeads; i++)
      {
	  StatePtr	candState;
	  TMShortCard	numMatches = context->numMatches;
	  MatchPair	statMatch = context->matches;

	  for (candState = stateTree->complexBranchHeadTbl[i];
	       numMatches && candState;
	       numMatches--, statMatch++, candState = candState->nextLevel)
	    {
		if ((statMatch->typeIndex != candState->typeIndex) ||
		    (statMatch->modIndex != candState->modIndex))
		  break;
	    }
	  if (numMatches == 0) {
	      *leafStateRtn = candState;
	      return i;
	  }
      }
    *leafStateRtn = NULL;
    return (TM_NO_MATCH);
}

static StatePtr TryCurrentTree(stateTreePtr, tmRecPtr, curEventPtr)
    TMComplexStateTree	*stateTreePtr;
    XtTM		tmRecPtr;
    TMEventRec		*curEventPtr;
{
    StatePtr		candState = NULL, matchState = NULL;
    TMContext		*contextPtr = GetContextPtr(tmRecPtr);
    TMTypeMatch 	typeMatch;
    TMModifierMatch 	modMatch;
    int			currIndex = -1;
    
    /*
     * we want the first sequence that both matches and has actions.
     * we keep on looking till we find both
     */
    while ((currIndex = 
	    MatchComplexBranch(*stateTreePtr,
			       ++currIndex,
			       (*contextPtr),
			       &candState))
	   != TM_NO_MATCH) {
	if (candState  != NULL) {
	    typeMatch  = TMGetTypeMatch(candState->typeIndex);
	    modMatch = TMGetModifierMatch(candState->modIndex);
	    
	    /* does this state's index match? --> done */
	    if (MatchIncomingEvent(curEventPtr, typeMatch, modMatch))
	      {
		  if (candState->actions) {
		      return candState;
		  }
		  else
		    matchState = candState;
	      }
	    /* is this an event timer? */
	    if (typeMatch->eventType == _XtEventTimerEventType) {
		StatePtr nextState = candState->nextLevel;
		
		/* does the succeeding state match? */
		if (nextState != NULL) {
		    TMTypeMatch 	nextTypeMatch;
		    TMModifierMatch	nextModMatch;
		    
		    nextTypeMatch  = TMGetTypeMatch(nextState->typeIndex);
		    nextModMatch = TMGetModifierMatch(nextState->modIndex);
		    
		    /* is it within the timeout? */
		    if (MatchIncomingEvent(curEventPtr, 
					   nextTypeMatch, 
					   nextModMatch)) {
			XEvent *xev = curEventPtr->xev;
			unsigned long time = GetTime(tmRecPtr, xev);
			XtPerDisplay pd = _XtGetPerDisplay(xev->xany.display);
			unsigned long delta = pd->multi_click_time;
			
			if ((tmRecPtr->lastEventTime + delta) >= time) {
			    if (nextState->actions) {
				return candState;
			    }
			    else
			      matchState = candState;
			}
		    }
		}
	    }
	}
    }
    return matchState;
}

static void HandleComplexState(w, tmRecPtr, curEventPtr)
    Widget	w;
    XtTM	tmRecPtr;
    TMEventRec	*curEventPtr;
{
    XtTranslations 	xlations = tmRecPtr->translations;
    TMContext		*contextPtr = GetContextPtr(tmRecPtr);
    TMShortCard		i, matchTreeIndex;
    StatePtr		matchState = NULL, candState;
    TMComplexStateTree 	*stateTreePtr = 
      (TMComplexStateTree *)&xlations->stateTreeTbl[0];
    
    for (i = 0;
	 i < xlations->numStateTrees;
	 i++, stateTreePtr++) {
	/* 
	 * some compilers sign extend Boolean bit fields so test for
	 * false |||
	 */
	if (((*stateTreePtr)->isSimple == False) &&
	    (candState = TryCurrentTree(stateTreePtr,
				       tmRecPtr,
				       curEventPtr))) {
	    if (!matchState || candState->actions) {
		matchTreeIndex = i;
		matchState = candState;
		if (candState->actions)
		    break;
	    }
	}
    }
    if (matchState == NULL){
	/* couldn't find it... */
	if (!Ignore(curEventPtr))
	  {
	      FreeContext(contextPtr);
	      HandleSimpleState(w, tmRecPtr, curEventPtr);
	  }
    }
    else {
	TMBindData	bindData = (TMBindData) tmRecPtr->proc_table;
	XtActionProc	*procs;
	Widget		accelWidget;
	TMTypeMatch 	typeMatch;
	
	typeMatch  = TMGetTypeMatch(matchState->typeIndex);

	PushContext(contextPtr, matchState);
	if (typeMatch->eventType == _XtEventTimerEventType) {
	    matchState = matchState->nextLevel;
	    PushContext(contextPtr, matchState);
	}
	tmRecPtr->lastEventTime = GetTime (tmRecPtr, curEventPtr->xev);

	if (bindData->simple.isComplex) {
	    TMComplexBindProcs bindProcs =
	      TMGetComplexBindEntry(bindData, matchTreeIndex);
	    procs = bindProcs->procs;
	    accelWidget = bindProcs->widget;
	}
	else {
	    TMSimpleBindProcs bindProcs = 
	      TMGetSimpleBindEntry(bindData, matchTreeIndex);
	    procs = bindProcs->procs;
	    accelWidget = NULL;
	}
	HandleActions(w, 
		      curEventPtr->xev, 
		      (TMSimpleStateTree)
		      xlations->stateTreeTbl[matchTreeIndex],
		      accelWidget,
		      procs,
		      matchState->actions);
    }
}


void _XtTranslateEvent (w, event)
    Widget w;
    register    XEvent * event;
{
    XtTM	tmRecPtr = &w->core.tm;
    TMEventRec 	curEvent;
    StatePtr 	current_state = tmRecPtr->current_state;

    XEventToTMEvent (event, &curEvent);
    
    if (! tmRecPtr->translations) {
        XtAppWarningMsg(XtWidgetToApplicationContext(w),
			XtNtranslationError,"nullTable",XtCXtToolkitError,
			"Can't translate event through NULL table",
			(String *)NULL, (Cardinal *)NULL);
	return ;
    }
    if (current_state == NULL)
	  HandleSimpleState(w, tmRecPtr, &curEvent);
    else
	  HandleComplexState(w, tmRecPtr, &curEvent);
}


/*ARGSUSED*/
static StatePtr NewState(stateTree, typeIndex, modIndex)
    TMParseStateTree stateTree;
    TMShortCard	typeIndex, modIndex;
{
    register StatePtr state = XtNew(StateRec);

#ifdef TRACE_TM
    _XtGlobalTM.numComplexStates++;
#endif /* TRACE_TM */
    state->typeIndex = typeIndex;
    state->modIndex = modIndex;
    state->nextLevel = NULL;
    state->actions = NULL;
    state->isCycleStart = state->isCycleEnd = False;
    return state;
}

/*
 * This routine is an iterator for state trees. If the func returns
 * true then iteration is over.
 */
void _XtTraverseStateTree(tree, func, data)
    TMStateTree	tree;
    _XtTraversalProc func;
    XtPointer 	data;
{
    register 	TMComplexStateTree stateTree = (TMComplexStateTree)tree;
    TMBranchHead	currBH;
    TMShortCard		i;
    StateRec		dummyStateRec, *dummyState = &dummyStateRec;
    ActionRec		dummyActionRec, *dummyAction = &dummyActionRec;
    Boolean		firstSimple = True;
    StatePtr 		currState;

    /* first traverse the complex states */
    if (stateTree->isSimple == False)
      for (i = 0; i < stateTree->numComplexBranchHeads; i++) {
	  currState = stateTree->complexBranchHeadTbl[i];
	  for (; currState; currState = currState->nextLevel) {
	      if (func(currState, data))
		return;
	      if (currState->isCycleEnd) 
		break;
	  }
      }

    /* now traverse the simple ones */
    for (i = 0, currBH = stateTree->branchHeadTbl;
	 i < stateTree->numBranchHeads;
	 i++, currBH++)
      {
	  if (currBH->isSimple && currBH->hasActions)
	    {
		if (firstSimple)
		  {
		      XtBZero((char *) dummyState, sizeof(StateRec));
		      XtBZero((char *) dummyAction, sizeof(ActionRec));
		      dummyState->actions = dummyAction;
		      firstSimple = False;
		  }
		dummyState->typeIndex = currBH->typeIndex;
		dummyState->modIndex = currBH->modIndex;
		dummyAction->idx = currBH->more;
		if (func(dummyState, data))
		  return;
	    }
      }
}

static EventMask EventToMask(typeMatch, modMatch)
    register TMTypeMatch     typeMatch;
    register TMModifierMatch modMatch;
{
    EventMask returnMask;
    unsigned long eventType = typeMatch->eventType;

    if (eventType == MotionNotify) {
        Modifiers modifierMask = modMatch->modifierMask;
        Modifiers tempMask;

	returnMask = 0;
        if (modifierMask == 0) {
	    if (modMatch->modifiers == AnyButtonMask)
		return ButtonMotionMask;
	    else
		return PointerMotionMask;
	}
        tempMask = modifierMask &
	    (Button1Mask | Button2Mask | Button3Mask
	     | Button4Mask | Button5Mask);
        if (tempMask == 0)
	    return PointerMotionMask;
        if (tempMask & Button1Mask)
            returnMask |= Button1MotionMask;
        if (tempMask & Button2Mask)
            returnMask |= Button2MotionMask;
        if (tempMask & Button3Mask)
            returnMask |= Button3MotionMask;
        if (tempMask & Button4Mask)
            returnMask |= Button4MotionMask;
        if (tempMask & Button5Mask)
            returnMask |= Button5MotionMask;
        return returnMask;
    }
    returnMask = _XtConvertTypeToMask(eventType);
    if (returnMask == (StructureNotifyMask|SubstructureNotifyMask))
	returnMask = StructureNotifyMask;
    return returnMask;
}

/*ARGSUSED*/
static void DispatchMappingNotify(widget, closure, call_data)
    Widget widget;		/* will be NULL from _RefreshMapping */
    XtPointer closure;		/* real Widget */
    XtPointer call_data;	/* XEvent* */
{
    _XtTranslateEvent( (Widget)closure, (XEvent*)call_data);
}

  
/*ARGSUSED*/
static void RemoveFromMappingCallbacks(widget, closure, call_data)
    Widget widget;
    XtPointer closure;		/* target widget */
    XtPointer call_data;
{
    _XtRemoveCallback( &_XtGetPerDisplay(XtDisplay(widget))->mapping_callbacks,
		       DispatchMappingNotify,
		       closure
		      );
}

static Boolean AggregateEventMask(state, data)
    StatePtr	state;
    XtPointer	data;
{
    *((EventMask *)data) |= EventToMask(TMGetTypeMatch(state->typeIndex), 
					TMGetModifierMatch(state->modIndex));
   return False;
}    

void _XtInstallTranslations(widget)
    Widget widget;
{
    XtTranslations xlations;
    register Cardinal	i;
    TMStateTree	stateTree;
    Boolean  mappingNotifyInterest = False;

    xlations = widget->core.tm.translations;
    if (xlations == NULL) return;
    
    /*
     * check for somebody stuffing the translations directly into the
     * instance structure. We will end up being called again out of
     * ComposeTranslations but we *should* have bindings by then
     */
    if (widget->core.tm.proc_table == NULL) {
	_XtMergeTranslations(widget, NULL, XtTableReplace);
	/* 
	 * if we're realized then we'll be called out of
	 * ComposeTranslations 
	 */
	if (XtIsRealized(widget))
	  return;
    }

    xlations->eventMask = 0;
    for (i = 0;
	 i < xlations->numStateTrees;
	 i++)
      {
	  stateTree = xlations->stateTreeTbl[i];
	  _XtTraverseStateTree(stateTree,
			    AggregateEventMask, 
			    (XtPointer)&xlations->eventMask);
	  mappingNotifyInterest |= stateTree->simple.mappingNotifyInterest;
      }
    /* double click needs to make sure that you have selected on both
	button down and up. */

    if (xlations->eventMask & ButtonPressMask)
      xlations->eventMask |= ButtonReleaseMask;
    if (xlations->eventMask & ButtonReleaseMask)
      xlations->eventMask |= ButtonPressMask;

    if (mappingNotifyInterest) {
	XtPerDisplay pd = _XtGetPerDisplay(XtDisplay(widget));
	if (pd->mapping_callbacks)
	    _XtAddCallbackOnce(&(pd->mapping_callbacks),
			       DispatchMappingNotify,
			       (XtPointer)widget);
	else
	    _XtAddCallback(&(pd->mapping_callbacks),
			   DispatchMappingNotify,
			   (XtPointer)widget);

	if (widget->core.destroy_callbacks != NULL)
	    _XtAddCallbackOnce( (InternalCallbackList *)
			        &widget->core.destroy_callbacks,
				RemoveFromMappingCallbacks,
				(XtPointer)widget
			       );
	else
	    _XtAddCallback((InternalCallbackList *)
			   &widget->core.destroy_callbacks,
			   RemoveFromMappingCallbacks,
			   (XtPointer)widget
			  );
    }
    _XtBindActions(widget, (XtTM)&widget->core.tm);
    _XtRegisterGrabs(widget);
}

void _XtRemoveTranslations(widget)
    Widget widget;
{
    register Cardinal	i;
    TMSimpleStateTree	stateTree;
    Boolean  		mappingNotifyInterest = False;
    XtTranslations		xlations = widget->core.tm.translations;
    
    if (xlations == NULL) 
      return;

    for (i = 0;
	 i < xlations->numStateTrees;
	 i++)
      {
	  stateTree = (TMSimpleStateTree)xlations->stateTreeTbl[i];
	  mappingNotifyInterest |= stateTree->mappingNotifyInterest;
      }
    if (mappingNotifyInterest)
      RemoveFromMappingCallbacks(widget, (XtPointer)widget, NULL);
}

static void _XtUninstallTranslations(widget)
    Widget widget;
{
    XtTranslations	xlations = widget->core.tm.translations;

    _XtUnbindActions(widget, 
		     xlations, 
		     (TMBindData)widget->core.tm.proc_table);
    _XtRemoveTranslations(widget);
    widget->core.tm.translations = NULL;
    FreeContext((TMContext *)&widget->core.tm.current_state);
}

void _XtDestroyTMData(widget)
    Widget	widget;
{
    TMComplexBindData	cBindData;

    _XtUninstallTranslations(widget);

    if (cBindData = (TMComplexBindData)widget->core.tm.proc_table) {
	if (cBindData->isComplex) {
	    ATranslations	aXlations, nXlations;
	    
	    nXlations = (ATranslations) cBindData->accel_context;
	    while (nXlations){
		aXlations = nXlations;
		nXlations = nXlations->next;
		XtFree((char *)aXlations);
	    }
	}
	XtFree((char *)cBindData);
    }
}

/*** Public procedures ***/


void XtUninstallTranslations(widget)
    Widget widget;
{
    EventMask	oldMask;

    if (! widget->core.tm.translations)
	return;
    oldMask = widget->core.tm.translations->eventMask;
    _XtUninstallTranslations(widget);
    if (XtIsRealized(widget) && oldMask)
	XSelectInput(XtDisplay(widget), XtWindow(widget), 
		     XtBuildEventMask(widget));
}

#if NeedFunctionPrototypes
XtTranslations _XtCreateXlations(
    TMStateTree		*stateTrees,
    TMShortCard		numStateTrees,
    XtTranslations	first,
    XtTranslations	second)
#else
XtTranslations _XtCreateXlations(stateTrees, numStateTrees, first, second)
    TMStateTree		*stateTrees;
    TMShortCard		numStateTrees;
    XtTranslations	first, second;
#endif
{
    register XtTranslations	xlations;
    TMShortCard i;

    xlations = (XtTranslations)
      XtMalloc(sizeof(TranslationData) +
	       (numStateTrees-1) * sizeof(TMStateTree));
#ifdef TRACE_TM
    if (_XtGlobalTM.numTms == _XtGlobalTM.tmTblSize) {
	_XtGlobalTM.tmTblSize += 16;
	_XtGlobalTM.tmTbl = (XtTranslations *)
	  XtRealloc((char *)_XtGlobalTM.tmTbl,
		   _XtGlobalTM.tmTblSize * sizeof(XtTranslations));
    }
    _XtGlobalTM.tmTbl[_XtGlobalTM.numTms++] = xlations;
#endif /* TRACE_TM */

    xlations->composers[0] = first;
    xlations->composers[1] = second;
    xlations->hasBindings = False;
    xlations->operation = XtTableReplace;

    for (i = 0;i < numStateTrees; i++)
      {
	  xlations->stateTreeTbl[i] = (TMStateTree) stateTrees[i];
	  stateTrees[i]->simple.refCount++;
      }
    xlations->numStateTrees = numStateTrees;
    xlations->eventMask = 0;
    return xlations;
}

TMStateTree _XtParseTreeToStateTree(parseTree)
    TMParseStateTree	parseTree;
{
    register TMSimpleStateTree  simpleTree;
    register unsigned int	tableSize;

    if (parseTree->numComplexBranchHeads) {
	register TMComplexStateTree complexTree;

	complexTree = XtNew(TMComplexStateTreeRec);
	complexTree->isSimple = False;
	tableSize = parseTree->numComplexBranchHeads * sizeof(StatePtr); 
	complexTree->complexBranchHeadTbl = (StatePtr *)
	  XtMalloc(tableSize);
	XtBCopy(parseTree->complexBranchHeadTbl,
		complexTree->complexBranchHeadTbl,
		tableSize);
	complexTree->numComplexBranchHeads = 
	  parseTree->numComplexBranchHeads;
	simpleTree = (TMSimpleStateTree)complexTree;
    }
    else {
	simpleTree = XtNew(TMSimpleStateTreeRec);
	simpleTree->isSimple = True;
    }
    simpleTree->isAccelerator = parseTree->isAccelerator;
    simpleTree->refCount = 0;
    simpleTree->mappingNotifyInterest = parseTree->mappingNotifyInterest;

    tableSize = parseTree->numBranchHeads * sizeof(TMBranchHeadRec);
    simpleTree->branchHeadTbl = (TMBranchHead)
      XtMalloc(tableSize);
    XtBCopy(parseTree->branchHeadTbl, simpleTree->branchHeadTbl, tableSize);
    simpleTree->numBranchHeads = parseTree->numBranchHeads;

    tableSize = parseTree->numQuarks * sizeof(XrmQuark);
    simpleTree->quarkTbl = (XrmQuark *) XtMalloc(tableSize);
    XtBCopy(parseTree->quarkTbl, simpleTree->quarkTbl, tableSize);
    simpleTree->numQuarks = parseTree->numQuarks;

    return (TMStateTree)simpleTree;
}

static void FreeActions(actions)
    ActionPtr	actions;
{
    register ActionPtr action;
    register TMShortCard i;
    for (action = actions; action;) {
	ActionPtr nextAction = action->next;
	for (i = action->num_params; i;) {
	    XtFree( action->params[--i] );
	}
	XtFree( (char*)action->params );
	XtFree((char*) action);
	action = nextAction;
    }
}

/*ARGSUSED*/
static void AmbigActions(initialEvent, state, stateTree)
    EventSeqPtr	initialEvent;
    StatePtr	*state;
    TMParseStateTree stateTree;
{
    String 	params[3];
    Cardinal 	numParams = 0;

    params[numParams++] = _XtPrintEventSeq(initialEvent, NULL);
    params[numParams++] = _XtPrintActions((*state)->actions,
					  stateTree->quarkTbl);
    XtWarningMsg (XtNtranslationError,"oldActions",XtCXtToolkitError,
		  "Previous entry was: %s %s", params, &numParams);
    XtFree((char *)params[0]);
    XtFree((char *)params[1]);
    numParams = 0;
    params[numParams++]  = _XtPrintActions(initialEvent->actions,
					   stateTree->quarkTbl);
    XtWarningMsg (XtNtranslationError,"newActions",XtCXtToolkitError,
		  "New actions are:%s", params, &numParams);
    XtFree((char *)params[0]);
    XtWarningMsg (XtNtranslationError,"ambiguousActions", 
		  XtCXtToolkitError,
		  "Overriding earlier translation manager actions.",
		  (String *)NULL, (Cardinal *)NULL);

    FreeActions((*state)->actions);
    (*state)->actions = NULL;
}


void _XtAddEventSeqToStateTree(eventSeq, stateTree)
    register EventSeqPtr 	eventSeq;
    TMParseStateTree		stateTree;
{
    register StatePtr		*state;
    EventSeqPtr			initialEvent = eventSeq;
    TMBranchHead		branchHead;
    TMShortCard			idx, modIndex, typeIndex;

    if (eventSeq == NULL) return;

    /* note that all states in the event seq passed in start out null */
    /* we fill them in with the matching state as we traverse the list */

    /*
     * We need to free the parser data structures !!!
     */

    typeIndex = _XtGetTypeIndex(&eventSeq->event);
    modIndex = _XtGetModifierIndex(&eventSeq->event);
    idx = GetBranchHead(stateTree, typeIndex, modIndex, False);
    branchHead = &stateTree->branchHeadTbl[idx];

    /*
     * Need to check for pre-existing actions with same lhs |||
     */
    
    /*
     * Check for optimized case. Don't assume that the eventSeq has actions.
     */
    if (!eventSeq->next && 
	 eventSeq->actions && 
	!eventSeq->actions->next && 
	!eventSeq->actions->num_params)
      {
	  if (eventSeq->event.eventType == MappingNotify)
	    stateTree->mappingNotifyInterest = True;
	  branchHead->hasActions = True;
	  branchHead->more = eventSeq->actions->idx;
	  FreeActions(eventSeq->actions);
	  eventSeq->actions = NULL;
	  return;
      }
    
    branchHead->isSimple = False;
    if (!eventSeq->next)
      branchHead->hasActions = True;
    branchHead->more = GetComplexBranchIndex(stateTree, typeIndex, modIndex);
    state = &stateTree->complexBranchHeadTbl[TMBranchMore(branchHead)];
    
    for (;;) {
	*state = NewState(stateTree, typeIndex, modIndex);
	
	if (eventSeq->event.eventType == MappingNotify)
	    stateTree->mappingNotifyInterest = True;
	
	/* *state now points at state record matching event */
	eventSeq->state = *state;
	
	if (eventSeq->actions != NULL) {
	    if ((*state)->actions != NULL)
	      AmbigActions(initialEvent, state, stateTree);
	    (*state)->actions = eventSeq->actions;
#ifdef TRACE_TM
	    _XtGlobalTM.numComplexActions++;
#endif /* TRACE_TM */
	}

	if (((eventSeq = eventSeq->next) == NULL) || (eventSeq->state))
	  break;

	state = &(*state)->nextLevel;
	typeIndex = _XtGetTypeIndex(&eventSeq->event);
	modIndex = _XtGetModifierIndex(&eventSeq->event);
	if (!TMNewMatchSemantics()) {
	    /* 
	     * force a potential empty entry into the branch head
	     * table in order to emulate old matching behavior
	     */
	    (void) GetBranchHead(stateTree, typeIndex, modIndex, True);
	}
    }

    if (eventSeq && eventSeq->state) {
	/* we've been here before... must be a cycle in the event seq. */
	branchHead->hasCycles = True;
	(*state)->nextLevel = eventSeq->state;
	eventSeq->state->isCycleStart = True;
	(*state)->isCycleEnd = TRUE;
    }
}


/*
 * Internal Converter for merging. Old and New must both be valid xlations
 */

/*ARGSUSED*/
Boolean _XtCvtMergeTranslations(dpy, args, num_args, from, to, closure_ret)
    Display	*dpy;
    XrmValuePtr args;
    Cardinal    *num_args;
    XrmValuePtr from,to;
    XtPointer	*closure_ret;
{
    XtTranslations 	first, second, xlations;
    TMStateTree		*stateTrees, stackStateTrees[16];
    TMShortCard		numStateTrees, i;

    if (*num_args != 0)
	XtWarningMsg("invalidParameters","mergeTranslations",XtCXtToolkitError,
             "MergeTM to TranslationTable needs no extra arguments",
               (String *)NULL, (Cardinal *)NULL);

    if (to->addr != NULL && to->size < sizeof(XtTranslations)) {
	to->size = sizeof(XtTranslations);
	return False;
    }
	
    first = ((TMConvertRec*)from->addr)->old;
    second = ((TMConvertRec*)from->addr)->new;

    numStateTrees = first->numStateTrees + second->numStateTrees;

    stateTrees = (TMStateTree *)
      XtStackAlloc(numStateTrees * sizeof(TMStateTree), stackStateTrees);

    for (i = 0; i < first->numStateTrees; i++)
      stateTrees[i] = first->stateTreeTbl[i];
    for (i = 0; i < second->numStateTrees; i++)
      stateTrees[i + first->numStateTrees] = second->stateTreeTbl[i];

    xlations = _XtCreateXlations(stateTrees, numStateTrees, first, second);

    if (to->addr != NULL) {
	*(XtTranslations*)to->addr = xlations;
    }
    else {
	static XtTranslations staticStateTable;
	staticStateTable = xlations;
	to->addr= (XPointer)&staticStateTable;
	to->size = sizeof(XtTranslations);
    }

    XtStackFree((XtPointer)stateTrees, (XtPointer)stackStateTrees);
    return True;
}


static XtTranslations MergeThem(dest, first, second)
    Widget		dest;	
    XtTranslations	first, second;
{
    XtCacheRef 		cache_ref;
    static XrmQuark 	from_type = NULLQUARK, to_type;
    XrmValue 		from, to;
    TMConvertRec 	convert_rec;
    XtTranslations	newTable;

    if (from_type == NULLQUARK) {
	from_type = XrmPermStringToQuark(_XtRStateTablePair);
	to_type = XrmPermStringToQuark(XtRTranslationTable);
    }
    from.addr = (XPointer)&convert_rec;
    from.size = sizeof(TMConvertRec);
    to.addr = (XPointer)&newTable;
    to.size = sizeof(XtTranslations);
    convert_rec.old = first;
    convert_rec.new = second;

    if (! _XtConvert(dest, from_type, &from, to_type, &to, &cache_ref))
	return NULL;
    if (cache_ref)
	XtAddCallback(dest, XtNdestroyCallback,
		      XtCallbackReleaseCacheRef, (XtPointer)cache_ref);
    return newTable;
}

/*
 * Unmerge will recursively traverse the xlation compose tree and
 * generate a new xlation that is the result of all instances of
 * xlations being removed. It currently doesn't differentiate between
 * the potential that an xlation will be both an accelerator and
 * normal. This is not supported by the spec anyway.
 */
static XtTranslations UnmergeTranslations(widget, xlations, unmergeXlations, 
					  currIndex,
					  oldBindings, numOldBindings,
					  newBindings, numNewBindingsRtn)
    Widget		widget;
    XtTranslations	xlations, unmergeXlations;
    TMComplexBindProcs	oldBindings, newBindings;
    TMShortCard		currIndex, numOldBindings, *numNewBindingsRtn;

{
    XtTranslations first, second, result;
    
    if (!xlations || (xlations == unmergeXlations))
      return NULL;
    
    if (xlations->composers[0]) {
	first = UnmergeTranslations(widget, xlations->composers[0],
				    unmergeXlations,  currIndex,
				    oldBindings, numOldBindings,
				    newBindings, numNewBindingsRtn);
    }
    else
      first = NULL;
    
    if (xlations->composers[1]) {
	second = UnmergeTranslations(widget, xlations->composers[1],
				     unmergeXlations,  
				     currIndex + 
				     xlations->composers[0]->numStateTrees,
				     oldBindings, numOldBindings,
				     newBindings, numNewBindingsRtn);
    }
    else
      second = NULL;

    if (first || second) {
	if (first && second) {
	    if ((first != xlations->composers[0]) ||
		(second != xlations->composers[1]))
	      result = MergeThem(widget, first, second);
	    else result = xlations;
	}
	else {
	    if (first)
	      result = first;
	    else
	      result = second;
	}
    } else { /* only update for leaf nodes */
  	if (numOldBindings) {
 	    Cardinal	i;
 	    for (i = 0; i < xlations->numStateTrees; i++) {
 		if (xlations->stateTreeTbl[i]->simple.isAccelerator)
		    newBindings[*numNewBindingsRtn] =
			oldBindings[currIndex + i];
 		(*numNewBindingsRtn)++;
	    }
	}
	result = xlations;
    }
    return result;
}

typedef struct {
    XtTranslations xlations;
    TMComplexBindProcs bindings;
}MergeBindRec, *MergeBind;

static XtTranslations MergeTranslations(widget, oldXlations, newXlations, 
					operation, source, oldBindings, 
					newBindings, numNewRtn)
    Widget		widget;
    XtTranslations	oldXlations, newXlations;
    _XtTranslateOp	operation;
    TMComplexBindProcs	oldBindings, newBindings;
    Widget		source;
    TMShortCard		*numNewRtn;
{
    XtTranslations      newTable, xlations;
    TMComplexBindProcs	bindings;
    TMShortCard		i, j;
    TMStateTree 	*treePtr;
    TMShortCard		numNew = *numNewRtn;
    MergeBindRec	bindPair[2];

    /* If the new translation has an accelerator context then pull it
     * off and pass it and the real xlations in to the caching merge
     * routine. 
     */
    if (newXlations->hasBindings) {
	xlations = ((ATranslations) newXlations)->xlations;
	bindings = (TMComplexBindProcs)
	    &((ATranslations) newXlations)->bindTbl[0];
    }
    else {
	xlations = newXlations;
	bindings = NULL;
    }
    switch(operation) {
      case XtTableReplace:
	newTable = bindPair[0].xlations = xlations;
	bindPair[0].bindings = bindings;
	bindPair[1].xlations = NULL;
	bindPair[1].bindings = NULL;
	break;
      case XtTableAugment:
	bindPair[0].xlations = oldXlations;
	bindPair[0].bindings = oldBindings;
	bindPair[1].xlations = xlations;
	bindPair[1].bindings = bindings;
	newTable = NULL;
	break;
      case XtTableOverride:
	bindPair[0].xlations = xlations;
	bindPair[0].bindings = bindings;
	bindPair[1].xlations = oldXlations;
	bindPair[1].bindings = oldBindings;
	newTable = NULL;
	break;
    }
    if (!newTable)
      newTable = MergeThem(widget, bindPair[0].xlations, bindPair[1].xlations);
    
    for (i = 0, numNew = 0; i < 2; i++) {
	if (bindPair[i].xlations)
	  for (j = 0; j < bindPair[i].xlations->numStateTrees; j++, numNew++) {
	      if (bindPair[i].xlations->stateTreeTbl[j]->simple.isAccelerator) {
		  if (bindPair[i].bindings)
		    newBindings[numNew] = bindPair[i].bindings[j];
		  else {
		      newBindings[numNew].widget = source;
		      newBindings[numNew].aXlations = 
			bindPair[i].xlations;
		  }
		}
	  }
    }
    *numNewRtn = numNew;
    treePtr = &newTable->stateTreeTbl[0];
    for (i = 0; i < newTable->numStateTrees; i++, treePtr++)
      (*treePtr)->simple.refCount++;
    return newTable;
}

static TMBindData MakeBindData(bindings, numBindings, oldBindData)
    TMComplexBindProcs	bindings;
    TMShortCard		numBindings;
    TMBindData		oldBindData;
{
    TMLongCard		bytes;
    TMShortCard		i;
    Boolean		isComplex;
    TMBindData		bindData;

    if (numBindings == 0) 
      return NULL;
    for (i = 0; i < numBindings; i++)
      if (bindings[i].widget)
	break;
    isComplex = (i < numBindings);
    if (isComplex)
      bytes = (sizeof(TMComplexBindDataRec) + 
	       ((numBindings - 1) * 
		sizeof(TMComplexBindProcsRec)));
    else
      bytes = (sizeof(TMSimpleBindDataRec) + 
	       ((numBindings - 1) * 
		sizeof(TMSimpleBindProcsRec)));

    bindData = (TMBindData) XtCalloc(sizeof(char), bytes);
    bindData->simple.isComplex = isComplex;
    if (isComplex) {
	TMComplexBindData cBindData = (TMComplexBindData)bindData;
	/* 
	 * If there were any accelerator contexts in the old bindData
	 * then propagate them to the new one.
	 */
	if (oldBindData && oldBindData->simple.isComplex)
	    cBindData->accel_context = 
		((TMComplexBindData) oldBindData)->accel_context;
	XtBCopy((char *)bindings,
		(char *)&cBindData->bindTbl[0],
		numBindings * sizeof(TMComplexBindProcsRec));
    }
    return bindData;
}

/*
 * This routine is the central clearinghouse for merging translations
 * into a widget. It takes care of preping the action bindings for
 * realize time and calling the converter or doing a straight merge if
 * the destination is empty. 
 */
static Boolean ComposeTranslations(dest, operation, source, newXlations)
    Widget dest, source;
    _XtTranslateOp operation;
    XtTranslations newXlations;
{
    XtTranslations 	newTable, oldXlations;
    XtTranslations	accNewXlations;
    EventMask		oldMask;
    TMBindData		bindData;
    TMComplexBindProcs	oldBindings = NULL;
    TMShortCard		numOldBindings, numNewBindings = 0, numBytes;
    TMComplexBindProcsRec stackBindings[16], *newBindings;

    /*
     * how should we be handling the refcount decrement for the
     * replaced translation table ???
     */
    if (!newXlations)
      {
	  XtAppWarningMsg(XtWidgetToApplicationContext(dest),
			  XtNtranslationError,"nullTable",XtCXtToolkitError,
			  "table to (un)merge must not be null",
			  (String *)NULL, (Cardinal *)NULL);
	  return False;
      }

    accNewXlations = newXlations;
    newXlations = ((newXlations->hasBindings)
		   ? ((ATranslations)newXlations)->xlations
		   : newXlations);

    if (!(oldXlations = dest->core.tm.translations))
      operation = XtTableReplace;

    /* 
     * try to avoid generation of duplicate state trees. If the source
     * isn't simple (1 state Tree) then it's too much hassle
     */
    if (((operation == XtTableAugment) || 
	 (operation == XtTableOverride)) &&
	(newXlations->numStateTrees == 1)) {
	Cardinal	i;
	for (i = 0; i < oldXlations->numStateTrees; i++)
	  if (oldXlations->stateTreeTbl[i] ==
	      newXlations->stateTreeTbl[0])
	    break;
	if (i < oldXlations->numStateTrees) {
	    if (operation == XtTableAugment) {
		/* 
		 * we don't need to do anything since it's already
		 * there 
		 */
		return True;
	    }
	    else {/* operation == XtTableOverride */
		/*
		 * We'll get rid of the duplicate trees throughout the
		 * and leave it with a pruned translation table. This
		 * will only work if the same table has been merged
		 * into this table (or one of it's composers
		 */
		_XtUnmergeTranslations(dest, newXlations);
		/*
		 * reset oldXlations so we're back in sync
		 */
		if (!(oldXlations = dest->core.tm.translations))
		  operation = XtTableReplace;
	    }
	}
    }

    bindData = (TMBindData) dest->core.tm.proc_table;
    if (bindData) {
	numOldBindings = (oldXlations ? oldXlations->numStateTrees : 0);
 	if (bindData->simple.isComplex)
	    oldBindings = &((TMComplexBindData)bindData)->bindTbl[0];
 	else
	    oldBindings = (TMComplexBindProcs)
		(&((TMSimpleBindData)bindData)->bindTbl[0]);
    }

    numBytes =(((oldXlations ? oldXlations->numStateTrees : 0)
		+ newXlations->numStateTrees) * sizeof(TMComplexBindProcsRec));
    newBindings = (TMComplexBindProcs) XtStackAlloc(numBytes,  stackBindings);
    XtBZero((char *)newBindings, numBytes);

    if (operation == XtTableUnmerge) {
	newTable = UnmergeTranslations(dest, 
				       oldXlations,
				       newXlations,
				       0,
				       oldBindings, numOldBindings,
				       newBindings, &numNewBindings);
#ifdef DEBUG
	/* check for no match for unmerge */
	if (newTable == oldXlations) {
	    XtWarning("attempt to unmerge invalid table");
	    XtStackFree((char *)newBindings, (char *)stackBindings);
	    return(newTable != NULL);
	}
#endif /* DEBUG */
    }
    else {
	newTable = MergeTranslations(dest, 
				     oldXlations,
				     accNewXlations,
				     operation,
				     source,
				     oldBindings,
				     newBindings,
				     &numNewBindings);
    }
    if (XtIsRealized(dest)) {
	oldMask = 0;
	if (oldXlations)
	    oldMask = oldXlations->eventMask;
	_XtUninstallTranslations(dest);
    }
    
    dest->core.tm.proc_table = 
      (XtActionProc *) MakeBindData(newBindings, numNewBindings, bindData);
    
    if (bindData) XtFree((char *)bindData);

    dest->core.tm.translations = newTable;

    if (XtIsRealized(dest)) {
	EventMask mask = 0;
	_XtInstallTranslations(dest);
	if (newTable)
	    mask = newTable->eventMask;
	if (mask != oldMask)
	    XSelectInput(XtDisplay(dest), XtWindow(dest), 
			 XtBuildEventMask(dest));
    }
    XtStackFree((char *)newBindings, (char *)stackBindings);
    return(newTable != NULL);
}

/*
 * If a GetValues is done on a translation resource that contains
 * accelerators we need to return the accelerator context in addition
 * to the pure translations.  Since this means returning memory that
 * the client controlls but we still own, we will track the "headers"
 * that we return (via a linked list pointed to from the bindData) and
 * free it at destroy time.
 */
XtTranslations _XtGetTranslationValue(w)
    Widget	w;
{
    XtTM		tmRecPtr = (XtTM) &w->core.tm;
    ATranslations	*aXlationsPtr;
    TMComplexBindData	cBindData = (TMComplexBindData) tmRecPtr->proc_table;
    XtTranslations	xlations = tmRecPtr->translations;

    if (!xlations || !cBindData || !cBindData->isComplex)
	return xlations;

    /* Walk the list looking to see if we already have generated a
     * header for the currently installed translations.  If we have,
     * just return that header.  Otherwise create a new header.
     */
    for (aXlationsPtr = (ATranslations *) &cBindData->accel_context;
	 *aXlationsPtr && (*aXlationsPtr)->xlations != xlations;
	 aXlationsPtr = &(*aXlationsPtr)->next)
	;
    if (*aXlationsPtr)
	return (XtTranslations) *aXlationsPtr;
    else { 
	/* create a new aXlations context */
	ATranslations	aXlations;
	Cardinal	numBindings = xlations->numStateTrees;

	(*aXlationsPtr) = aXlations = (ATranslations)
	    XtMalloc(sizeof(ATranslationData) + 
		     (numBindings - 1) * sizeof(TMComplexBindProcsRec));

	aXlations->hasBindings = True;
	aXlations->xlations = xlations;
	aXlations->next = NULL;
	XtBCopy((char *) &cBindData->bindTbl[0],
		(char *) &aXlations->bindTbl[0],
		numBindings * sizeof(TMComplexBindProcsRec));
	return (XtTranslations) aXlations;
    }
}


/*ARGSUSED*/
static void RemoveStateTree(tree)
    TMStateTree	tree;
{
#ifdef REFCNT_TRANSLATIONS    
    TMComplexStateTree stateTree = (TMComplexStateTree)tree;

    if (--stateTree->refCount == 0) {
	/*
	 * should we free/refcount the match recs ?
	 */
	if (!stateTree->isSimple)  {
	    StatePtr	currState, nextState;
	    TMShortCard i;
	    for (i = 0; i < stateTree->numComplexBranchHeads; i++) {
		currState = 
		  nextState = 
		    stateTree->complexBranchHeadTbl[i];
		for (; nextState;){
		    FreeActions(currState->actions);
		    currState->actions = NULL;
		    if (!currState->isCycleEnd)
		      nextState = currState->nextLevel;
		    else
		      nextState = NULL;
		    XtFree( (char*)currState );
		}
	    }
	    XtFree((char*)stateTree->complexBranchHeadTbl);
	}
	XtFree((char*)stateTree->branchHeadTbl);
	XtFree((char*)stateTree);
    }
#endif /* REFCNT_TRANSLATIONS */
}

void _XtRemoveStateTreeByIndex(xlations, i)
    XtTranslations	xlations;
    TMShortCard	i;
{
    TMStateTree		*stateTrees = xlations->stateTreeTbl;

    RemoveStateTree(stateTrees[i]);
    xlations->numStateTrees--;

    for (; i < xlations->numStateTrees; i++)
      {
	  stateTrees[i] = stateTrees[i+1];
      }
}

/* ARGSUSED */
void _XtFreeTranslations(app, toVal, closure, args, num_args)
    XtAppContext app;
    XrmValuePtr	toVal;
    XtPointer	closure;
    XrmValuePtr	args;
    Cardinal	*num_args;
{
    XtTranslations 	xlations;
    register int 	i;

    if (*num_args != 0)
	XtAppWarningMsg(app,
	  "invalidParameters","freeTranslations",XtCXtToolkitError,
          "Freeing XtTranslations requires no extra arguments",
	  (String *)NULL, (Cardinal *)NULL);

    xlations = *(XtTranslations*)toVal->addr;
    for (i = 0; i < (int)xlations->numStateTrees; i++)
      RemoveStateTree(xlations->stateTreeTbl[i]);
    XtFree((char *)xlations);
}

/*  The spec is not clear on when actions specified in accelerators are bound;
 *  Bind them at Realize the same as translations
 */
void XtInstallAccelerators(destination, source)
    Widget destination, source;
{
    XtTranslations	aXlations;
    _XtTranslateOp	op;
    String		buf;

    /*
     * test that it was parsed as an accelarator table. Even though
     * there doesn't need to be a distinction it makes life easier if
     * we honor the spec implication that aXlations is an accelerator
     */
    if ((!XtIsWidget(source)) || 
	((aXlations = source->core.accelerators) == NULL) ||
	(aXlations->stateTreeTbl[0]->simple.isAccelerator == False))
      return;
    
    aXlations = source->core.accelerators;
    op = aXlations->operation;

    if (ComposeTranslations(destination, op, source, aXlations) &&
	(XtClass(source)->core_class.display_accelerator != NULL)) {
	
	buf = _XtPrintXlations(destination, aXlations, source, False);
	(*(XtClass(source)->core_class.display_accelerator))(source,buf);
	XtFree(buf);
    }
}
  
void XtInstallAllAccelerators(destination,source)
    Widget destination,source;
{
    register int i;
    CompositeWidget cw;

    /* Recurse down normal children */
    if (XtIsComposite(source)) {
        cw = (CompositeWidget) source;
        for (i = 0; i < cw->composite.num_children; i++) {
            XtInstallAllAccelerators(destination,cw->composite.children[i]);
        }
    }

    /* Recurse down popup children */
    if (XtIsWidget(source)) {
        for (i = 0; i < source->core.num_popups; i++) {
            XtInstallAllAccelerators(destination,source->core.popup_list[i]);
        }
    }
    /* Finally, apply procedure to this widget */
    XtInstallAccelerators(destination,source);
}

#if 0 /* dead code */
static _XtTranslateOp _XtGetTMOperation(xlations)
    XtTranslations xlations;
{
    return ((xlations->hasBindings)
	    ? ((ATranslations)xlations)->xlations->operation
	    : xlations->operation);
}
#endif

void XtAugmentTranslations(widget, new)
    Widget widget;
    XtTranslations new;
{
    (void)ComposeTranslations(widget, XtTableAugment, (Widget)NULL, new);
}

void XtOverrideTranslations(widget, new)
    Widget widget;
    XtTranslations new;
{
    (void) ComposeTranslations(widget, XtTableOverride, (Widget)NULL, new);
}

void _XtMergeTranslations(widget, newXlations, op)
    Widget	widget;
    XtTranslations newXlations;
    _XtTranslateOp op;
{
    if (!newXlations){
	if (!widget->core.tm.translations)
	  return;
	else {
	  newXlations = widget->core.tm.translations;
	  widget->core.tm.translations = NULL;
      }
    }
    (void) ComposeTranslations(widget, 
			     op,
			     (Widget)NULL, 
			     newXlations);
}

void _XtUnmergeTranslations(widget, xlations) 
   Widget		widget;
    XtTranslations	xlations;
{
    ComposeTranslations(widget, XtTableUnmerge, (Widget)NULL, xlations);
}


void _XtPopupInitialize(app)
    XtAppContext app;
{
    /*
     * The _XtGlobalTM.newMatchSemantics flag determines whether
     * we support old or new matching
     * behavior. This is mainly an issue of whether subsequent lhs will
     * get pushed up in the match table if a lhs containing thier initial
     * sequence has already been encountered. Currently inited to False;
     */
#ifdef NEW_TM
    _XtGlobalTM.newMatchSemantics = True;
#else
    _XtGlobalTM.newMatchSemantics = False;
#endif
    _XtActionInitialize(app);
    _XtGrabInitialize(app);
}
