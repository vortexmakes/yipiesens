/*
 *	file: flwact.h
 *	Last updated for version: 1.0.00
 *	Date of the last update:  Feb 23, 2012
 */

/**
 * 	\file flwact.h
 *	\brief
 */


#ifndef __FLWACT_H__
#define __FLWACT_H__


#include "fsm.h"


void sig_init( FSM_T *fsm );
const struct trn_t *start_ta( FSM_T *fsm, EVT_T e );
const struct trn_t *reset_ta( FSM_T *fsm, EVT_T e );
const struct trn_t *stop_hf1( FSM_T *fsm, EVT_T e );
const struct trn_t *stop_hf2( FSM_T *fsm, EVT_T e );
const struct trn_t *start_rev_cyc( FSM_T *fsm, EVT_T e );
const struct trn_t *start_dir_cyc( FSM_T *fsm, EVT_T e );
const struct trn_t *start_pulse( FSM_T *fsm, EVT_T e );
const struct trn_t *reset_cyc( FSM_T *fsm, EVT_T e );
const struct trn_t *inc_pulse( FSM_T *fsm, EVT_T e );


#endif
