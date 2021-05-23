//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#include "regex/fsm.h"
#include "toolbelt.h"

static void _alloc_transition_(fsm_transition_t *self, args_t arguments) {
  object_t from = next_arg(arguments, object_t);
  self->from = alloc(pointer_t, from);
  context_set(self->from, "from");
  object_t symbol = next_arg(arguments, object_t);
  self->symbol = alloc(pointer_t, symbol);
  context_set(self->symbol, "symbol");
  object_t to = next_arg(arguments, object_t);
  self->to = alloc(pointer_t, to);
  context_set(self->to, "to");
}

static void _free_transition_(fsm_transition_t *self) {
  dealloc(self->from);
  dealloc(self->symbol);
  dealloc(self->to);
}

static void _copy_transition_(fsm_transition_t *self,
                              fsm_transition_t *object) {
  object->from = copy(self->from);
  object->symbol = copy(self->symbol);
  object->to = copy(self->to);
}

static bool _equal_transition_(fsm_transition_t *self,
                               fsm_transition_t *object) {
  if (equal(self->from, object->from)) {
    if (equal(self->symbol, object->symbol)) {
      if (equal(self->to, object->to)) {
        return true;
      }
    }
  }
  return false;
}

static void _inspect_transition_(fsm_transition_t *self, inspect_t *inspect) {
  inspect_value_node(inspect, self, "");

  inspect_add_edge(inspect, self, NULL, self->from, NULL);
  inspect_add_edge(inspect, self, NULL, self->symbol, NULL);
  inspect_add_edge(inspect, self, NULL, self->to, NULL);

  object_inspect(self->from, inspect);
  object_inspect(self->symbol, inspect);
  object_inspect(self->to, inspect);
}

def_prototype_source(fsm_transition_t, _alloc_transition_, _free_transition_,
                     _copy_transition_, _equal_transition_,
                     _inspect_transition_);

static void _alloc_(fsm_t *self, args_t arguments) {
  self->states = alloc(list_t);
  self->symbols = alloc(list_t);
  self->transitions = alloc(list_t);
  self->terminals = alloc(list_t);
}

static void _free_(fsm_t *self) {
  dealloc(self->states);
  dealloc(self->symbols);
  dealloc(self->transitions);
  dealloc(self->terminals);
}

static void _copy_(fsm_t *self, fsm_t *object) {
  object->states = copy(self->states);
  object->symbols = copy(self->symbols);
  object->transitions = copy(self->transitions);
  object->terminals = copy(self->terminals);
}

static bool _equal_(fsm_t *self, fsm_t *object) {
  if (equal(self->states, object->states)) {
    if (equal(self->symbols, object->symbols)) {
      if (equal(self->transitions, object->transitions)) {
        if (equal(self->terminals, object->terminals)) {
          return true;
        }
      }
    }
  }
  return false;
}

static void _inspect_(fsm_t *self, inspect_t *inspect) {
  inspect_value_node(inspect, self, "");

  inspect_add_edge(inspect, self, NULL, self->states, NULL);
  inspect_add_edge(inspect, self, NULL, self->symbols, NULL);
  inspect_add_edge(inspect, self, NULL, self->transitions, NULL);
  inspect_add_edge(inspect, self, NULL, self->terminals, NULL);

  object_inspect(self->states, inspect);
  object_inspect(self->symbols, inspect);
  object_inspect(self->transitions, inspect);
  object_inspect(self->terminals, inspect);
}

def_prototype_source(fsm_t, _alloc_, _free_, _copy_, _equal_, _inspect_);

#define make_state(self, state)                                                \
  text_t *state = alloc(text_t, "");                                           \
  text_from_int(state, self->states->size);                                    \
  text_append(state, "`");                                                     \
  list_add(self->states, LIST_TAIL, state)

#define make_symbol(self, symbol)                                              \
  do {                                                                         \
    bool not_exist = true;                                                     \
    list_for(self->symbols, index, text_t, item, {                             \
      if (equal(item, symbol)) {                                               \
        dealloc(symbol);                                                       \
        symbol = item;                                                         \
        not_exist = false;                                                     \
        break;                                                                 \
      }                                                                        \
    });                                                                        \
    if (not_exist) {                                                           \
      list_add(self->symbols, LIST_TAIL, symbol);                              \
    }                                                                          \
  } while (false)

#define make_transition(self, from, symbol, to)                                \
  do {                                                                         \
    text_t *temp = symbol;                                                     \
    if (temp == NULL) {                                                        \
      temp = list_get(self->symbols, text_t, LIST_HEAD);                       \
    }                                                                          \
    fsm_transition_t *transition = alloc(fsm_transition_t, from, temp, to);    \
    list_add(self->transitions, LIST_TAIL, transition);                        \
  } while (false)

#define make_terminal(self, state)                                             \
  do {                                                                         \
    pointer_t *terminal = alloc(pointer_t, state);                             \
    list_add(self->terminals, LIST_TAIL, terminal);                            \
  } while (false)

static index_t nfa_perform_part(fsm_t *self, expression_t *expression,
                                index_t index, text_t *from, text_t *to) {
  index_t current = index;
  make_state(self, init);
  make_state(self, exit);
  make_transition(self, from, NULL, init);
  make_transition(self, exit, NULL, to);
  if (expression->prefix->value[current] == '.') {
    current++;
    make_state(self, join);
    current = nfa_perform_part(self, expression, current, init, join);
    current = nfa_perform_part(self, expression, current, join, exit);
  } else if (expression->prefix->value[current] == '|') {
    current++;
    current = nfa_perform_part(self, expression, current, init, exit);
    current = nfa_perform_part(self, expression, current, init, exit);
  } else if (expression->prefix->value[current] == '+') {
    current++;
    make_transition(self, exit, NULL, init);
    current = nfa_perform_part(self, expression, current, init, exit);
  } else if (expression->prefix->value[current] == '*') {
    current++;
    make_transition(self, init, NULL, exit);
    make_transition(self, exit, NULL, init);
    current = nfa_perform_part(self, expression, current, init, exit);
  } else if (expression->prefix->value[current] == '?') {
    current++;
    make_transition(self, init, NULL, exit);
    current = nfa_perform_part(self, expression, current, init, exit);
  } else {
    text_t *symbol = alloc(text_t, "");
    if (expression->prefix->value[current] == '#') {
      current++;
      if (expression->prefix->value[current] == 'N') {
        text_append(symbol, ".num");
      } else if (expression->prefix->value[current] == 'A') {
        text_append(symbol, ".alp");
      } else if (expression->prefix->value[current] == 'C') {
        text_append(symbol, ".chr");
      } else {
        char temp[] = {expression->prefix->value[current], '\0'};
        text_append(symbol, temp);
      }
    } else {
      char temp[] = {expression->prefix->value[current], '\0'};
      text_append(symbol, temp);
    }
    make_symbol(self, symbol);
    make_transition(self, init, symbol, exit);
    current++;
  }

  return current;
}

static void epsilon_closure(text_t *self, text_t *state, list_t *transitions) {
  list_for(transitions, index, fsm_transition_t, item, {
    text_t *from = cast(text_t *, item->from->value);
    text_t *symbol = cast(text_t *, item->symbol->value);
    text_t *to = cast(text_t *, item->to->value);
    if (equal(from, state) && text_compare(symbol, ".eps")) {
      text_t *temp = alloc(text_t, "`");
      text_append(temp, to->value);
      if (!text_contains(self, temp->value)) {
        text_append(self, to->value);
        epsilon_closure(self, to, transitions);
      }
      dealloc(temp);
    }
  });
}

static void transition_from_state_and_closures(fsm_t *nfa, text_t *dfa_from,
                                               list_t *dfa_states,
                                               list_t *dfa_transitions) {
  list_for(nfa->symbols, index, text_t, symbol_item, {
    if (!text_compare(symbol_item, ".eps")) {
      text_for_split(dfa_from, dfa_from->value, "`", part, {
        text_t *from_item = alloc(text_t, part);
        text_append(from_item, "`");
        list_for(nfa->transitions, index, fsm_transition_t, transition, {
          text_t *from = cast(text_t *, transition->from->value);
          text_t *symbol = cast(text_t *, transition->symbol->value);
          text_t *to = cast(text_t *, transition->to->value);
          if (equal(from, from_item) && equal(symbol, symbol_item)) {
            text_t *to_item = alloc(text_t, to->value);

            text_t *epsilon = alloc(text_t, "`");
            text_append(epsilon, to_item->value);
            epsilon_closure(epsilon, to_item, nfa->transitions);

            text_set(to_item, epsilon->value);
            dealloc(epsilon);

            bool state_not_exist = true;
            list_for(dfa_states, index, text_t, state, {
              if (text_size(state) == text_size(to_item)) {
                bool is_equal = true;
                text_for_split(state, state->value, "`", partsub, {
                  if (!text_contains(to_item, partsub)) {
                    is_equal = false;
                    break;
                  }
                });
                if (is_equal) {
                  state_not_exist = false;
                  dealloc(to_item);
                  to_item = state;
                  break;
                }
              }
            });

            fsm_transition_t *transition =
                alloc(fsm_transition_t, dfa_from, symbol_item, to_item);
            list_add(dfa_transitions, LIST_TAIL, transition);

            if (state_not_exist) {
              list_add(dfa_states, LIST_TAIL, to_item);
              transition_from_state_and_closures(nfa, to_item, dfa_states,
                                                 dfa_transitions);
            }
          }
        });
        dealloc(from_item);
      });
    }
  });
}

static void dfa_join_duplicate_transitions(fsm_t *self) {
  list_t *joined = alloc(list_t);
  list_t *remove = alloc(list_t);
  list_t *delete = alloc(list_t);

  list_for(self->transitions, index_first, fsm_transition_t, transition_first, {
    bool not_found = true;
    text_t *from_first = cast(text_t *, transition_first->from->value);
    text_t *symbol_first = cast(text_t *, transition_first->symbol->value);
    text_t *to_first = cast(text_t *, transition_first->to->value);

    list_for(joined, index_duplicate, fsm_transition_t, transition_duplicate, {
      text_t *from_duplicate =
          cast(text_t *, transition_duplicate->from->value);
      text_t *symbol_duplicate =
          cast(text_t *, transition_duplicate->symbol->value);
      text_t *to_duplicate = cast(text_t *, transition_duplicate->to->value);

      if (equal(from_first, from_duplicate) &&
          equal(symbol_first, symbol_duplicate)) {
        not_found = false;

        list_for(self->transitions, index_fix, fsm_transition_t, move_fix, {
          text_t *from_update = cast(text_t *, move_fix->from->value);
          text_t *symbol_update = cast(text_t *, move_fix->symbol->value);
          text_t *to_update = cast(text_t *, move_fix->to->value);

          if (equal(from_update, to_first)) {
            pointer_set(move_fix->from, to_duplicate);
            number_t *index_to_remove =
                alloc(number_t, (double)atoi(to_first->value));
            list_add(remove, LIST_HEAD, index_to_remove);
          }
        });
      }
    });
    if (not_found) {
      list_add(joined, LIST_TAIL, transition_first);
    } else {
      list_add(delete, LIST_TAIL, transition_first);
    }
  });

  while (self->transitions->size > 0) {
    list_extract(self->transitions, LIST_HEAD);
  }
  dealloc(self->transitions);
  self->transitions = joined;

  list_for(remove, index, number_t, item,
           { list_del(self->states, (index_t)item->value); });
  dealloc(remove);

  dealloc(delete);

  list_for(self->states, index, text_t, item, { text_from_int(item, index); });
}

static void dfa_from_nfa(fsm_t *self) {
  list_t *dfa_states = alloc(list_t);
  list_t *dfa_transitions = alloc(list_t);
  list_t *dfa_terminals = alloc(list_t);

  text_t *epsilon = alloc(text_t, "`");
  text_t *nfa_state = list_get(self->states, text_t, LIST_HEAD);
  text_append(epsilon, nfa_state->value);
  epsilon_closure(epsilon, nfa_state, self->transitions);
  list_add(dfa_states, LIST_TAIL, epsilon);

  transition_from_state_and_closures(self, epsilon, dfa_states,
                                     dfa_transitions);

  list_for(dfa_states, index, text_t, item, {
    list_for(self->terminals, index_ter, pointer_t, item_ter, {
      text_t *temp_term = alloc(text_t, "`");
      text_append(temp_term, cast(text_t *, item_ter->value)->value);
      if (text_contains(item, temp_term->value)) {
        pointer_t *terminal_ptr = alloc(pointer_t, item);
        list_add(dfa_terminals, LIST_TAIL, terminal_ptr);
      }
      dealloc(temp_term);
    });
  });

  list_for(dfa_states, index, text_t, item, { text_from_int(item, index); });

  list_del(self->symbols, LIST_HEAD);

  dealloc(self->states);
  dealloc(self->transitions);
  dealloc(self->terminals);

  self->states = dfa_states;
  self->transitions = dfa_transitions;
  self->terminals = dfa_terminals;

  dfa_join_duplicate_transitions(self);
}

void fsm_from_expression(fsm_t *self, expression_t *expression) {
  make_state(self, begin);
  make_state(self, end);
  text_t *empty = alloc(text_t, ".eps");
  make_symbol(self, empty);
  make_terminal(self, end);
  nfa_perform_part(self, expression, 0, begin, end);
  dfa_from_nfa(self);

  context_set(self->states, "states");
  context_set(self->symbols, "symbols");
  context_set(self->transitions, "transitions");
  context_set(self->terminals, "terminals");
}

void fsm_test() {
  fsm_t *fsm = alloc(fsm_t);
  assert(type_equal(fsm, "fsm_t") == true);
  expression_t *expression = alloc(expression_t, "a(c+|d*|z?|cp)f");
  fsm_from_expression(fsm, expression);
  dealloc(expression);
  dealloc(fsm);
}
