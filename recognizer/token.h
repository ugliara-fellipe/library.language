//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#ifndef TOKEN_H
#define TOKEN_H

#include "toolbelt.h"

typedef struct {
  text_t *type;
  text_t *value;
  index_t line;
  index_t index;
  size_t size;
} token_t;

def_prototype_header(token_t);

void token_test();

#endif
