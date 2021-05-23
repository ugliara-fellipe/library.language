//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#include "recognizer/grammar.h"
#include "toolbelt.h"

static void _alloc_path_(grammar_path_t *self, args_t arguments) {
  char *rule = next_arg(arguments, char *);
  self->rule = alloc(text_t, rule);
  index_t option = next_arg(arguments, int);
  self->option = option;
  index_t item = next_arg(arguments, int);
  self->item = item;
}

static void _free_path_(grammar_path_t *self) { dealloc(self->rule); }

static void _copy_path_(grammar_path_t *self, grammar_path_t *object) {
  object->rule = copy(self->rule);
  object->option = self->option;
  object->item = self->item;
}

static bool _equal_path_(grammar_path_t *self, grammar_path_t *object) {
  if (equal(self->rule, object->rule) && (self->option == object->option) &&
      (self->item == object->item)) {
    return true;
  }
  return false;
}

static void _inspect_path_(grammar_path_t *self, inspect_t *inspect) {}

def_prototype_source(grammar_path_t, _alloc_path_, _free_path_, _copy_path_,
                     _equal_path_, _inspect_path_);

static void _alloc_pointer_(grammar_pointer_t *self, args_t arguments) {
  self->option = NULL;
  self->item = NULL;
}

static void _free_pointer_(grammar_pointer_t *self) {}

static void _copy_pointer_(grammar_pointer_t *self, grammar_pointer_t *object) {
  object->option = self->option;
  object->item = self->item;
}

static bool _equal_pointer_(grammar_pointer_t *self,
                            grammar_pointer_t *object) {
  if ((self->option == object->option) && (self->item == object->item)) {
    return true;
  }
  return false;
}

static void _inspect_pointer_(grammar_pointer_t *self, inspect_t *inspect) {}

def_prototype_source(grammar_pointer_t, _alloc_pointer_, _free_pointer_,
                     _copy_pointer_, _equal_pointer_, _inspect_pointer_);

static void _alloc_(grammar_t *self, args_t arguments) {
  self->rules = alloc(table_t);
  self->begin = alloc(text_t, "");
}

static void _free_(grammar_t *self) {
  dealloc(self->rules);
  dealloc(self->begin);
}

static void _copy_(grammar_t *self, grammar_t *object) {
  object->rules = copy(self->rules);
  object->begin = copy(self->begin);
}

static bool _equal_(grammar_t *self, grammar_t *object) {
  if (equal(self->rules, object->rules) && equal(self->begin, object->begin)) {
    return true;
  }
  return false;
}

static void _inspect_(grammar_t *self, inspect_t *inspect) {}

def_prototype_source(grammar_t, _alloc_, _free_, _copy_, _equal_, _inspect_);

void grammar_rule(grammar_t *self, const char *rule, const char *derivations) {
  list_t *options = alloc(list_t);
  text_for_split(options, derivations, "|", derivation, {
    list_t *items = alloc(list_t);
    text_for_split(items, derivation, " ", item, {
      text_t *text_item = NULL;
      pair_t *pair = NULL;
      if (isupper(item[0])) {
        text_item = alloc(text_t, item);
        pair = alloc(pair_t, "token", text_item);
      } else if (islower(item[0])) {
        text_item = alloc(text_t, item);
        pair = alloc(pair_t, "nest", text_item);
      } else if (item[0] == ':' && islower(item[1])) {
        text_item = alloc(text_t, item + 1);
        pair = alloc(pair_t, "unnest", text_item);
      }
      list_add(items, LIST_TAIL, pair);
    });
    list_add(options, LIST_TAIL, items);
  });
  table_add(self->rules, rule, options);
}

void grammar_begin(grammar_t *self, const char *rule) {
  text_set(self->begin, rule);
}

list_t *grammar_get_options(grammar_t *self, const char *rule) {
  return table_get(self->rules, list_t, rule);
}

void grammar_test() {
  grammar_t *grammar = alloc(grammar_t);
  assert(type_equal(grammar, "grammar_t") == true);
  grammar_rule(grammar, "a", "B A c C | B :c");
  grammar_begin(grammar, "a");
  dealloc(grammar);
}
