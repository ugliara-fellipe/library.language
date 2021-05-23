//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#ifndef REGEX_H
#define REGEX_H

#include "regex/expression.h"
#include "regex/fsm.h"
#include "toolbelt.h"

#define REGEX_INVALID_INDEX SIZE_MAX

typedef struct {
  expression_t *expression;
  fsm_t *fsm;
} regex_t;

def_prototype_header(regex_t);

typedef struct {
  index_t index;
  size_t size;
  bool match;
  text_t *value;
} regex_result_t;

def_prototype_header(regex_result_t);

void regex_perform(regex_t *self, const char *input, regex_result_t *result);

void regex_test();

#endif
