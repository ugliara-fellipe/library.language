//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#include "regex/regex.h"
#include "toolbelt.h"

static void _alloc_(regex_t *self, args_t arguments) {
  char *expression = next_arg(arguments, char *);
  self->expression = alloc(expression_t, expression);
  self->fsm = alloc(fsm_t);
  fsm_from_expression(self->fsm, self->expression);
}

static void _free_(regex_t *self) {
  dealloc(self->expression);
  dealloc(self->fsm);
}

static void _copy_(regex_t *self, regex_t *object) {
  object->expression = copy(self->expression);
  object->fsm = copy(self->fsm);
}

static bool _equal_(regex_t *self, regex_t *object) {
  return equal(self->expression, object->expression);
}

static void _inspect_(regex_t *self, inspect_t *inspect) {
  inspect_value_node(inspect, self, "");

  inspect_add_edge(inspect, self, NULL, self->expression, NULL);
  inspect_add_edge(inspect, self, NULL, self->fsm, NULL);

  object_inspect(self->expression, inspect);
  object_inspect(self->fsm, inspect);
}

def_prototype_source(regex_t, _alloc_, _free_, _copy_, _equal_, _inspect_);

static void _alloc_result_(regex_result_t *self, args_t arguments) {
  self->index = 0;
  self->size = 0;
  self->match = false;
  self->value = alloc(text_t, "");
}

static void _free_result_(regex_result_t *self) { dealloc(self->value); }

static void _copy_result_(regex_result_t *self, regex_result_t *object) {
  object->index = self->index;
  object->size = self->size;
  object->match = self->match;
  object->value = copy(self->value);
}

static bool _equal_result_(regex_result_t *self, regex_result_t *object) {
  if (self->index == object->index && self->size == object->size &&
      self->match == object->match && equal(self->value, object->value)) {
    return true;
  }
  return false;
}

static void _inspect_result_(regex_result_t *self, inspect_t *inspect) {}

def_prototype_source(regex_result_t, _alloc_result_, _free_result_,
                     _copy_result_, _equal_result_, _inspect_result_);

static void assign_input_part(char *part, const char *input, index_t *index) {
  part[0] = input[(*index)];
  part[1] = '\0';
  (*index)++;
}

void regex_perform(regex_t *self, const char *input, regex_result_t *result) {
  result->index = 0;
  result->size = 0;
  result->match = false;
  text_set(result->value, "");

  index_t input_begin = 0;
  index_t input_index = 0;
  size_t input_size = strlen(input);
  char input_part[2];
  assign_input_part(input_part, input, &input_index);

  text_t *current_state = list_get(self->fsm->states, text_t, LIST_HEAD);

  while (input_index <= input_size) {
    bool not_find = true;
    list_for(self->fsm->transitions, index, fsm_transition_t, item, {
      text_t *from = cast(text_t *, item->from->value);
      text_t *symbol = cast(text_t *, item->symbol->value);
      text_t *to = cast(text_t *, item->to->value);
      if (equal(current_state, from)) {
        if ((text_compare(symbol, ".num") && isdigit(input_part[0])) ||
            (text_compare(symbol, ".alp") && isalpha(input_part[0])) ||
            (text_compare(symbol, ".chr")) ||
            (text_compare(symbol, input_part))) {
          text_append(result->value, input_part);
          assign_input_part(input_part, input, &input_index);
          current_state = to;
          not_find = false;
          break;
        }
      }
    });
    if (not_find) {
      list_for(self->fsm->terminals, index_terminal, pointer_t, item_terminal, {
        if (equal(cast(text_t *, item_terminal->value), current_state)) {
          if (self->expression->require_end) {
            text_set(result->value, "");
            return;
          }
          result->index = input_begin;
          result->size = (input_index - input_begin) - 1;
          result->match = true;
          return;
        }
      });
      if (self->expression->require_begin) {
        text_set(result->value, "");
        return;
      }
      input_begin++;
      if (input_begin < input_size) {
        text_set(result->value, "");
        input_index = input_begin;
        assign_input_part(input_part, input, &input_index);
        text_append(result->value, input_part);
        current_state = list_get(self->fsm->states, text_t, LIST_HEAD);
      } else {
        text_set(result->value, "");
        return;
      }
    }
  }
  list_for(self->fsm->terminals, index_terminal, pointer_t, item_terminal, {
    if (equal(cast(text_t *, item_terminal->value), current_state)) {
      result->index = input_begin;
      result->size = (input_index - input_begin) - 1;
      result->match = true;
      return;
    }
  });
  text_set(result->value, "");
}

void regex_test() {
  regex_t *regex = alloc(regex_t, "^a*b?(c+g|dp|df+)*e#(#C$");
  assert(type_equal(regex, "regex_t") == true);
  regex_result_t *result = alloc(regex_result_t);
  regex_perform(regex, "aaacgdffe(*", result);
  assert(result->index == 0);
  assert(result->size == 11);
  assert(result->match == true);
  assert(text_compare(result->value, "aaacgdffe(*"));
  dealloc(result);
  dealloc(regex);
}
