//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#include "recognizer/walker.h"
#include "toolbelt.h"

static void _alloc_(walker_t *self, args_t arguments) {
  self->listener_enter = alloc(table_t);
  self->listener_exit = alloc(table_t);
}

static void _free_(walker_t *self) {
  dealloc(self->listener_enter);
  dealloc(self->listener_exit);
}

static void _copy_(walker_t *self, walker_t *object) {
  object->listener_enter = copy(self->listener_enter);
  object->listener_exit = copy(self->listener_exit);
}

static bool _equal_(walker_t *self, walker_t *object) {
  if (equal(self->listener_enter, object->listener_enter) &&
      equal(self->listener_exit, object->listener_exit)) {
    return true;
  }
  return false;
}

static void _inspect_(walker_t *self, inspect_t *inspect) {}

def_prototype_source(walker_t, _alloc_, _free_, _copy_, _equal_, _inspect_);

void walker_listener(walker_t *self, const char *rule, listener_t enter,
                     listener_t exit) {
  if (enter != NULL) {
    function_t *function_enter = alloc(function_t, enter);
    table_add(self->listener_enter, rule, function_enter);
  }
  if (exit != NULL) {
    function_t *function_exit = alloc(function_t, exit);
    table_add(self->listener_exit, rule, function_exit);
  }
}

static void walk_ast_branch(walker_t *self, list_t *branch) {
  list_for(branch, index, ast_item_t, item, {
    if (text_compare(item->define, "rule")) {
      pair_t *pair_enter =
          table_get_pair(self->listener_enter, item->type->value);
      pair_t *pair_exit =
          table_get_pair(self->listener_exit, item->type->value);

      listener_t enter = NULL;
      listener_t exit = NULL;
      if (pair_enter != NULL) {
        enter = pair_value(function_t, pair_enter)->value;
      }
      if (pair_exit != NULL) {
        exit = pair_value(function_t, pair_exit)->value;
      }

      if (enter != NULL) {
        enter(item, self->context);
      }
      walk_ast_branch(self, item->branch);
      if (exit != NULL) {
        exit(item, self->context);
      }
    }
  });
}

void walker_perform(walker_t *self, ast_t *ast) {
  walk_ast_branch(self, ast->root);
}

#include "parser.h"
#include "scanner.h"

static void begin_exit(ast_item_t *rule, object_t context) {
  printf("exit begin\n");
}

static void if_enter(ast_item_t *rule, object_t context) {
  printf("enter if\n");
}

static void else_enter(ast_item_t *rule, object_t context) {
  printf("enter else\n");
}

static void else_exit(ast_item_t *rule, object_t context) {
  printf("exit else\n");
}

void walker_test() {
  walker_t *walker = alloc(walker_t);
  assert(type_equal(walker, "walker_t") == true);

  scanner_t *scanner = alloc(scanner_t);
  assert(type_equal(scanner, "scanner_t") == true);
  scanner_add_model(scanner, "If", "^if");
  scanner_add_model(scanner, "Else", "^else");
  scanner_perform(scanner, "ififelse");

  parser_t *parser = alloc(parser_t);
  assert(type_equal(parser, "parser_t") == true);
  grammar_rule(parser->grammar, "begin", "if else Eot");
  grammar_rule(parser->grammar, "if", "If | If :if");
  grammar_rule(parser->grammar, "else", "Else");
  grammar_begin(parser->grammar, "begin");
  parser_perform(parser, scanner->tokens);

  // ast_branch_print(parser->ast->root);

  walker_listener(walker, "begin", NULL, begin_exit);
  walker_listener(walker, "if", if_enter, NULL);
  walker_listener(walker, "else", else_enter, else_exit);

  // walker_perform(walker, parser->ast);
  // printf("\n\n");
  // walker_perform(walker, parser->ast);

  dealloc(parser);
  dealloc(scanner);

  dealloc(walker);
}
