//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#ifndef SCANNER_H
#define SCANNER_H

#include "toolbelt.h"

typedef struct {
  table_t *models;
  list_t *tokens;
} scanner_t;

def_prototype_header(scanner_t);

void scanner_add_model(scanner_t *self, const char *type,
                       const char *expression);
void scanner_perform(scanner_t *self, const char *input);
list_t *scanner_extract_tokens(scanner_t *self);

void scanner_test();

#endif
