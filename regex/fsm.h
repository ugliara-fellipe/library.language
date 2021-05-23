//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#ifndef FSM_H
#define FSM_H

#include "regex/expression.h"
#include "toolbelt.h"

typedef struct {
  pointer_t *from;
  pointer_t *symbol;
  pointer_t *to;
} fsm_transition_t;

def_prototype_header(fsm_transition_t);

typedef struct {
  list_t *states;
  list_t *symbols;
  list_t *transitions;
  list_t *terminals;
} fsm_t;

def_prototype_header(fsm_t);

void fsm_from_expression(fsm_t *self, expression_t *expression);

void fsm_test();

#endif
