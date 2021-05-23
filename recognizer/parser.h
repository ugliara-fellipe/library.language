//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#ifndef PARSER_H
#define PARSER_H

#include "recognizer/ast.h"
#include "recognizer/grammar.h"
#include "toolbelt.h"

typedef struct {
  list_t *grammar_path;
  index_t token_index;
  list_t *ast_path;
  ast_t *ast;
} parser_context_t;

def_prototype_header(parser_context_t);

typedef struct {
  grammar_t *grammar;
  ast_t *ast;
} parser_t;

def_prototype_header(parser_t);

void parser_perform(parser_t *self, list_t *tokens);
ast_t *parser_extract_ast(parser_t *self);

void parser_test();

#endif
