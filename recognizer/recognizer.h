//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include "recognizer/parser.h"
#include "recognizer/scanner.h"
#include "toolbelt.h"

#include "recognizer/token.h"

typedef struct {
  text_t *input;
  scanner_t *scanner;
  parser_t *parser;
} recognizer_t;

def_prototype_header(recognizer_t);

void recognizer_error_message(const char *format, ...);
void recognizer_error_message_code(text_t *input, token_t *token,
                                   const char *format, ...);

bool recognizer_perform(recognizer_t *self, const char *input);

void recognizer_test();

#endif
