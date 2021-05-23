//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#include "recognizer/scanner.h"
#include "recognizer/token.h"
#include "regex/regex.h"
#include "toolbelt.h"

static void _alloc_(scanner_t *self, args_t arguments) {
  self->models = alloc(table_t);
  self->tokens = alloc(list_t);
}

static void _free_(scanner_t *self) {
  dealloc(self->models);
  dealloc(self->tokens);
}

static void _copy_(scanner_t *self, scanner_t *object) {
  object->models = copy(self->models);
}

static bool _equal_(scanner_t *self, scanner_t *object) {
  return equal(self->models, object->models);
}

static void _inspect_(scanner_t *self, inspect_t *inspect) {}

def_prototype_source(scanner_t, _alloc_, _free_, _copy_, _equal_, _inspect_);

void scanner_add_model(scanner_t *self, const char *type,
                       const char *expression) {
  regex_t *regex = alloc(regex_t, expression);
  // check if first char in type is upper
  table_add(self->models, type, regex);
}

static void move_location(token_t *token, index_t *line, index_t *column) {
  char *break_line = strstr(token->value->value, "\n");
  char *last = token->value->value;
  while (break_line != NULL) {
    (*line)++;
    (*column) = 0;
    last = break_line + 1;
    break_line = strstr(break_line + 1, "\n");
  }
  (*column) += strlen(last);
}

void scanner_perform(scanner_t *self, const char *input) {
  dealloc(scanner_extract_tokens(self));

  index_t line = 0;
  index_t column = 0;
  char *current = cast(char *, input);
  token_t *token = alloc(token_t, "", "", 0, 0, 0);
  token_t *unknown = alloc(token_t, "", "", 0, 0, 0);
  while (*current != '\0') {
    table_for(self->models, index, regex_t, key, regex, {
      regex_result_t *result = alloc(regex_result_t);
      regex_perform(regex, current, result);
      if (result->match) {
        if (result->size > token->size) {
          text_set(token->type, key->value);
          text_set(token->value, result->value->value);
          token->size = text_size(result->value);
        }
      }
      dealloc(result);
    });
    if (token->size > 0) {
      if (text_size(unknown->value)) {
        text_set(unknown->type, "Unknown");
        unknown->line = line;
        unknown->index = column;
        unknown->size = text_size(unknown->value);
        list_add(self->tokens, LIST_TAIL, unknown);
        move_location(unknown, &line, &column);
        unknown = alloc(token_t, "", "", 0, 0, 0);
      }
      token->line = line;
      token->index = column;
      move_location(token, &line, &column);
      current += token->size;
      if (text_contains(token->type, ":") == false) {
        list_add(self->tokens, LIST_TAIL, token);
      } else {
        dealloc(token);
      }
      token = alloc(token_t, "", "", 0, 0, 0);
    } else {
      char temp[2];
      temp[0] = *current;
      temp[1] = '\0';
      text_append(unknown->value, temp);
      current++;
    }
  }
  if (text_size(unknown->value)) {
    text_set(unknown->type, "Unknown");
    unknown->line = line;
    unknown->index = column;
    unknown->size = text_size(unknown->value);
    list_add(self->tokens, LIST_TAIL, unknown);
    move_location(unknown, &line, &column);
    unknown = alloc(token_t, "", "", 0, 0, 0);
  }
  token_t *eot = alloc(token_t, "Eot", "End of Tokens", line, column, 1);
  list_add(self->tokens, LIST_TAIL, eot);
  dealloc(unknown);
  dealloc(token);
}

list_t *scanner_extract_tokens(scanner_t *self) {
  list_t *extract = self->tokens;
  self->tokens = alloc(list_t);
  return extract;
}

void scanner_test() {
  scanner_t *scanner = alloc(scanner_t);
  assert(type_equal(scanner, "scanner_t") == true);
  scanner_add_model(scanner, "If", "^if");
  scanner_add_model(scanner, "Else", "^else");
  scanner_add_model(scanner, ":Break", "^\n");
  // scanner_add_model(scanner, "break", "^\n");
  scanner_perform(scanner, "if if \n else if l");

  /*
  list_for(scanner->tokens, index, token_t, item, {
    printf("%s %s %zu %zu %zu\n", item->type->value, item->value->value,
           item->line, item->index, item->size);
  });
  */

  dealloc(scanner);
}
