//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#include "recognizer/recognizer.h"
#include "toolbelt.h"

static void _alloc_(recognizer_t *self, args_t arguments) {
  self->input = alloc(text_t, "");
  self->scanner = alloc(scanner_t);
  self->parser = alloc(parser_t);
}

static void _free_(recognizer_t *self) {
  dealloc(self->input);
  dealloc(self->scanner);
  dealloc(self->parser);
}

static void _copy_(recognizer_t *self, recognizer_t *object) {
  object->input = copy(self->input);
  object->scanner = copy(self->scanner);
  object->parser = copy(self->parser);
}

static bool _equal_(recognizer_t *self, recognizer_t *object) {
  if (equal(self->input, object->input) &&
      equal(self->scanner, object->scanner) &&
      equal(self->parser, object->parser)) {
    return true;
  }
  return false;
}

static void _inspect_(recognizer_t *self, inspect_t *inspect) {}

def_prototype_source(recognizer_t, _alloc_, _free_, _copy_, _equal_, _inspect_);

void recognizer_error_message(const char *format, ...) {
  va_list arguments;
  va_start(arguments, format);
  printf("error: ");
  vfprintf(stderr, format, arguments);
  va_end(arguments);
}

void recognizer_error_message_code(text_t *input, token_t *token,
                                   const char *format, ...) {
  text_t *line_text = alloc(text_t, "");
  index_t line_number = 0;
  text_for_split(lines, input->value, "\n", line, {
    if (line_number == token->line) {
      text_set(line_text, line);
      break;
    }
    line_number++;
  });

  index_t left_space_removed = 0;
  index_t index_line_text = 0;
  while (isspace(line_text->value[index_line_text])) {
    left_space_removed++;
    index_line_text++;
  }

  text_trim(line_text);

  index_t mark_index = token->index;
  size_t mark_size = 0;
  if (mark_index < left_space_removed) {
    mark_index = 0;
  } else {
    mark_index = mark_index - left_space_removed;
    if (text_contains(token->value, "\n")) {
      text_for_split(token, token->value, "\n", part, {
        mark_size = strlen(part);
        break;
      });
    } else {
      mark_size = token->size;
    }
    if (mark_index + mark_size > text_size(line_text)) {
      if (text_size(line_text) > mark_index) {
        mark_size = text_size(line_text) - mark_index;
      } else {
        mark_size = 0;
      }
    }
    if (mark_size > 0) {
      mark_size = mark_size - 1;
    }
    mark_index++;
  }
  printf("%zu: %zu: error: ", token->line + 1, token->index);
  va_list arguments;
  va_start(arguments, format);
  vfprintf(stdout, format, arguments);
  va_end(arguments);
  printf("\n");
  printf("  %s\n", line_text->value);
  for (index_t repeat = 0; repeat < mark_index + 1; repeat++) {
    printf(" ");
  }
  printf("^");
  for (index_t mark = 0; mark < mark_size; mark++) {
    printf("~");
  }
  printf("\n");
  dealloc(line_text);
}

static void found_match_false(ast_item_t *item, text_t *input, bool *result) {
  if (text_compare(item->define, "token")) {
    if (item->valid == false) {
      text_t *expected = alloc(text_t, "invalid grammar, found ");
      text_append(expected, item->type->value);
      text_append(expected, ", expected ");
      text_append(expected, item->expected_token_type->value);
      recognizer_error_message_code(input, pointer_get(item->token),
                                    expected->value);
      dealloc(expected);
      *result = false;
    }
  } else {
    list_for(item->branch, index, ast_item_t, subitem,
             { found_match_false(subitem, input, result); });
  }
}

bool recognizer_perform(recognizer_t *self, const char *input) {
  text_set(self->input, input);

  scanner_perform(self->scanner, self->input->value);
  parser_perform(self->parser, self->scanner->tokens);

  bool result = true;
  list_for(self->scanner->tokens, index, token_t, item, {
    if (text_compare(item->type, "Unknown")) {
      recognizer_error_message_code(self->input, item, "invalid token");
      result = false;
    }
  });

  found_match_false(list_get(self->parser->ast->root, ast_item_t, LIST_HEAD),
                    self->input, &result);

  return result;
}

#include "parser.h"
#include "scanner.h"

void recognizer_test() {
  recognizer_t *recognizer = alloc(recognizer_t);
  assert(type_equal(recognizer, "recognizer_t") == true);

  scanner_add_model(recognizer->scanner, "If", "^if");
  scanner_add_model(recognizer->scanner, "Else", "^else");

  grammar_rule(recognizer->parser->grammar, "begin", "if else Eot");
  grammar_rule(recognizer->parser->grammar, "if", "If | If :if");
  grammar_rule(recognizer->parser->grammar, "else", "Else");
  grammar_begin(recognizer->parser->grammar, "begin");

  // recognizer_perform(recognizer, "ififinvalidelse");
  // recognizer_perform(recognizer, "ififelseelse");

  dealloc(recognizer);
}
