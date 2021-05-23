//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#include "regex/expression.h"
#include "toolbelt.h"

static bool expression_str_has_char(const char *str, char c) {
  if (strchr(str, c) != NULL) {
    return true;
  }
  return false;
}

#define assign_move(str, index, c) str[index++] = c

#define perform_priority_operators(operators, stack, stack_index, prefix,      \
                                   prefix_index)                               \
  do {                                                                         \
    if (stack_index > 0) {                                                     \
      while (expression_str_has_char(operators, stack[stack_index - 1])) {     \
        assign_move(prefix, prefix_index, stack[stack_index - 1]);             \
        stack_index--;                                                         \
        stack[stack_index] = '\0';                                             \
      }                                                                        \
    }                                                                          \
  } while (false)

static void convert_infix_to_prefix(expression_t *self) {
  size_t expression_size = text_size(self->expanded);
  char *stack = calloc(expression_size, sizeof(char));
  char *prefix = calloc(expression_size + 1, sizeof(char));
  text_t *reverse = copy(self->expanded);
  text_reverse(reverse);
  index_t stack_index = 0;
  index_t prefix_index = 0;
  for (index_t exp_index = 0; exp_index < expression_size; exp_index++) {
    if (reverse->value[exp_index + 1] == '#') {
      size_t count = 0;
      for (index_t sub_index = exp_index + 1; sub_index < expression_size;
           sub_index++) {
        if (reverse->value[sub_index] == '#') {
          count++;
        } else {
          break;
        }
      }
      if (count % 2 == 1) {
        assign_move(prefix, prefix_index, reverse->value[exp_index]);
        exp_index++;
        assign_move(prefix, prefix_index, reverse->value[exp_index]);
        continue;
      }
    }

    if (reverse->value[exp_index] == ')') {
      assign_move(stack, stack_index, reverse->value[exp_index]);
    } else if (reverse->value[exp_index] == '(') {
      while (stack_index > 0) {
        stack_index--;
        if (stack[stack_index] == ')') {
          stack[stack_index] = '\0';
          break;
        }
        assign_move(prefix, prefix_index, stack[stack_index]);
        stack[stack_index] = '\0';
      }
    } else if (reverse->value[exp_index] == '|') {
      perform_priority_operators("+*?.", stack, stack_index, prefix,
                                 prefix_index);
      assign_move(stack, stack_index, reverse->value[exp_index]);
    } else if (reverse->value[exp_index] == '.') {
      perform_priority_operators("+*?", stack, stack_index, prefix,
                                 prefix_index);
      assign_move(stack, stack_index, reverse->value[exp_index]);
    } else if (expression_str_has_char("+*?", reverse->value[exp_index])) {
      assign_move(stack, stack_index, reverse->value[exp_index]);
    } else {
      if (reverse->value[exp_index + 1] == '#') {
        assign_move(prefix, prefix_index, reverse->value[exp_index]);
        exp_index++;
      }
      assign_move(prefix, prefix_index, reverse->value[exp_index]);
    }
  }
  text_set(self->prefix, prefix);
  text_reverse(self->prefix);
  dealloc(reverse);
  free(prefix);
  free(stack);
}

static void parser_begin_end(expression_t *self, const char *value) {
  text_t *temp = alloc(text_t, value);
  if (text_size(temp) > 0) {
    if (temp->value[text_size(temp) - 1] == '$') {
      self->require_end = true;
      temp->value[text_size(temp) - 1] = '\0';
    } else {
      self->require_end = false;
    }
    if (temp->value[0] == '^') {
      self->require_begin = true;
      text_append(self->value, temp->value + 1);
    } else {
      self->require_begin = false;
      text_append(self->value, temp->value);
    }
  }
  dealloc(temp);
}

static void expand_concat_operator(expression_t *self) {
  self->expanded = alloc(text_t, "(");
  char symbol[2];
  symbol[1] = '\0';
  size_t count = 0;
  for (index_t index = 1; index < text_size(self->value); index++) {
    symbol[0] = self->value->value[index];
    text_append(self->expanded, symbol);
    if (!expression_str_has_char("(#|", self->value->value[index]) ||
        count % 2 == 1) {

      if (!expression_str_has_char("*?+)|", self->value->value[index + 1])) {
        text_append(self->expanded, ".");
      }
    }

    if (self->value->value[index] == '#') {
      count++;
    } else {
      count = 0;
    }
  }
}

static void _alloc_(expression_t *self, args_t arguments) {
  char *value = next_arg(arguments, char *);
  self->value = alloc(text_t, "(");
  parser_begin_end(self, value);
  text_append(self->value, ")");
  expand_concat_operator(self);
  self->prefix = alloc(text_t, "");
  convert_infix_to_prefix(self);
}

static void _free_(expression_t *self) {
  dealloc(self->value);
  dealloc(self->expanded);
  dealloc(self->prefix);
}

static void _copy_(expression_t *self, expression_t *object) {
  object->value = copy(self->value);
  object->expanded = copy(self->expanded);
  object->prefix = copy(self->prefix);
  object->require_begin = self->require_begin;
  object->require_end = self->require_end;
}

static bool _equal_(expression_t *self, expression_t *object) {
  return equal(self->value, object->value);
}

static void _inspect_(expression_t *self, inspect_t *inspect) {
  text_t *info = alloc(text_t, "");
  text_append(info, self->value->value);
  text_append(info, "\\n");
  text_append(info, self->expanded->value);
  text_append(info, "\\n");
  text_append(info, self->prefix->value);
  text_append(info, "\\n");

  index_t index = 0;
  while (info->value[index] != '\0') {
    if (info->value[index] == '|') {
      info->value[index] = '!';
    }
    index++;
  }

  inspect_value_node(inspect, self, info->value);
  dealloc(info);
}

def_prototype_source(expression_t, _alloc_, _free_, _copy_, _equal_, _inspect_);

void expression_test() {
  expression_t *expression = alloc(expression_t, "^a*b?(c+g##|df+)*e#($");
  assert(text_compare(expression->expanded, "(a*.b?.(c+.g.##|d.f+)*.e.#()") ==
         true);
  assert(text_compare(expression->prefix, "....*a?b*|..+cg##.d+fe#(") == true);
  assert(expression->require_begin == true);
  assert(expression->require_end == true);
  assert(type_equal(expression, "expression_t") == true);
  dealloc(expression);
}
