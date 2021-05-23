//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#ifndef AST_H
#define AST_H

#include "recognizer/token.h"
#include "toolbelt.h"

typedef struct {
  index_t item;
} ast_path_t;

def_prototype_header(ast_path_t);

typedef struct {
  text_t *define;
  text_t *type;
  list_t *branch;
  pointer_t *token;
  bool nest;
  bool valid;
  text_t *expected_token_type;
} ast_item_t;

def_prototype_header(ast_item_t);

typedef struct {
  ast_item_t *parent;
  ast_item_t *item;
} ast_pointer_t;

def_prototype_header(ast_pointer_t);

typedef struct {
  list_t *root;
} ast_t;

def_prototype_header(ast_t);

token_t *ast_get_token(ast_item_t *rule, index_t index);
void ast_branch_print(list_t *branch);

void ast_test();

#endif
