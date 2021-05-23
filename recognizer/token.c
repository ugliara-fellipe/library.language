//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#include "recognizer/token.h"
#include "toolbelt.h"

static void _alloc_(token_t *self, args_t arguments) {
  char *type = next_arg(arguments, char *);
  self->type = alloc(text_t, type);
  char *value = next_arg(arguments, char *);
  self->value = alloc(text_t, value);
  index_t line = next_arg(arguments, int);
  self->line = line;
  index_t index = next_arg(arguments, int);
  self->index = index;
  size_t size = next_arg(arguments, int);
  self->size = size;
}

static void _free_(token_t *self) {
  dealloc(self->type);
  dealloc(self->value);
}

static void _copy_(token_t *self, token_t *object) {
  object->type = copy(self->type);
  object->value = copy(self->value);
  object->line = self->line;
  object->index = self->index;
  object->size = self->size;
}

static bool _equal_(token_t *self, token_t *object) {
  if (equal(self->type, object->type) && equal(self->value, object->value) &&
      (self->line == object->line) && (self->index == object->index) &&
      (self->size == object->size)) {
    return true;
  }
  return false;
}

static void _inspect_(token_t *self, inspect_t *inspect) {
  text_t *info = alloc(text_t, "");
  text_append(info, self->value->value);
  text_append(info, " : ");
  text_append(info, self->type->value);
  text_append(info, " : ");

  text_t *temp = alloc(text_t, "");
  text_from_int(temp, self->line);
  text_append(info, temp->value);
  text_append(info, ", ");
  text_from_int(temp, self->index);
  text_append(info, temp->value);

  inspect_value_node(inspect, self, info->value);
  dealloc(temp);
  dealloc(info);
}

def_prototype_source(token_t, _alloc_, _free_, _copy_, _equal_, _inspect_);

void token_test() {
  token_t *token = alloc(token_t, "a", "b", 1, 2, 0);
  assert(type_equal(token, "token_t") == true);
  assert(token->size == 0);
  dealloc(token);
}
