//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "toolbelt.h"

typedef struct {
  text_t *value;
  text_t *expanded;
  text_t *prefix;
  bool require_begin;
  bool require_end;
} expression_t;

def_prototype_header(expression_t);

void expression_test();

#endif
