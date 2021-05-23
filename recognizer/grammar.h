//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#ifndef GRAMMAR_H
#define GRAMMAR_H

#include "toolbelt.h"

typedef struct {
  text_t *rule;
  index_t option;
  index_t item;
} grammar_path_t;

def_prototype_header(grammar_path_t);

typedef struct {
  list_t *option;
  pair_t *item;
} grammar_pointer_t;

def_prototype_header(grammar_pointer_t);

typedef struct {
  table_t *rules;
  text_t *begin;
} grammar_t;

def_prototype_header(grammar_t);

void grammar_rule(grammar_t *self, const char *rule, const char *derivations);
void grammar_begin(grammar_t *self, const char *rule);

list_t *grammar_get_options(grammar_t *self, const char *rule);

void grammar_test();

#endif
