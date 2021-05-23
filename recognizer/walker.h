//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#ifndef WALKER_H
#define WALKER_H

#include "recognizer/ast.h"
#include "toolbelt.h"

typedef void (*listener_t)(ast_item_t *rule, object_t context);

typedef struct {
  table_t *listener_enter;
  table_t *listener_exit;
  object_t context;
} walker_t;

def_prototype_header(walker_t);

void walker_listener(walker_t *self, const char *rule, listener_t enter,
                     listener_t exit);
void walker_perform(walker_t *self, ast_t *ast);

void walker_test();

#endif
