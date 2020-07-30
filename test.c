//
// Copyright (c) 2020, Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#include "ast.h"
#include "expression.h"
#include "fsm.h"
#include "grammar.h"
#include "parser.h"
#include "recognizer.h"
#include "regex.h"
#include "scanner.h"
#include "token.h"
#include "walker.h"

int main() {
  expression_test();
  fsm_test();
  regex_test();

  ast_test();
  grammar_test();
  parser_test();
  recognizer_test();
  scanner_test();
  token_test();
  walker_test();
}
