/*
 *	file: fsm.c
 *	Last updated for version: 1.0.00
 *	Date of the last update:  Feb 23, 2012
 */

/**
 * 	\file fsm.c
 *	\brief
 */


#include "mytypes.h"
#include "fsm.h"
#include "flw.h"


#define FSM_RUNTIME_INIT( fsm )				\
			(fsm)->state = (fsm)->istate;	\
			(fsm)->lsig = SIG_11;			\
			(fsm)->ppc = 0;					\
			(fsm)->pps = 0;					\
			(fsm)->hf1 = 0;					\
			(fsm)->hf2 = 0;					\
			(fsm)->flowdir = FORWARD;		\
			(fsm)->ta = 0;					\
			(fsm)->tb = 0


FSM_T *
verify_fsm( MUInt fsm )
{
	return &fsms[ fsm ];
}


void
fsm_init( MUInt fsm )
{
    FSM_T *p;

    p = verify_fsm( fsm );
	FSM_RUNTIME_INIT( p );

    if( p->init_action != CI(0) )
        (*p->init_action)( p );
}


void
fsm_dispatch( MUInt fsm, EVT_T e )
{
	FSM_T *pf;
	const TRN_T *tr, *s;

	pf = verify_fsm( fsm );
	for( tr = pf->state; tr->event != e && tr->event != ANY; ++tr )
		;

	s = CT(0);
	if( tr->action != CA(0) )
		s = (*tr->action)( pf, e );

	if( tr->next != CT(0) )
		pf->state = tr->next;
	else if( s != CT(0) )
		pf->state = s;
}
