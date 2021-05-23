//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#include "recognizer/parser.h"
#include "recognizer/token.h"
#include "toolbelt.h"

static void _alloc_context_(parser_context_t *self, args_t arguments) {
  self->grammar_path = alloc(list_t);
  self->token_index = 0;
  self->ast_path = alloc(list_t);
  self->ast = alloc(ast_t);
}

static void _free_context_(parser_context_t *self) {
  dealloc(self->grammar_path);
  dealloc(self->ast_path);
  dealloc(self->ast);
}

static void _copy_context_(parser_context_t *self, parser_context_t *object) {
  object->grammar_path = copy(self->grammar_path);
  object->token_index = self->token_index;
  object->ast_path = copy(self->ast_path);
  object->ast = copy(self->ast);
}

static bool _equal_context_(parser_context_t *self, parser_context_t *object) {
  if (equal(self->ast_path, object->ast_path) &&
      equal(self->grammar_path, object->grammar_path) &&
      (self->token_index == object->token_index) &&
      equal(self->ast, object->ast)) {
    return true;
  }
  return false;
}

static void _inspect_context_(parser_context_t *self, inspect_t *inspect) {}

def_prototype_source(parser_context_t, _alloc_context_, _free_context_,
                     _copy_context_, _equal_context_, _inspect_context_);

static void _alloc_(parser_t *self, args_t arguments) {
  self->grammar = alloc(grammar_t);
  self->ast = alloc(ast_t);
}

static void _free_(parser_t *self) {
  dealloc(self->grammar);
  dealloc(self->ast);
}

static void _copy_(parser_t *self, parser_t *object) {
  object->grammar = copy(self->grammar);
}

static bool _equal_(parser_t *self, parser_t *object) {
  if (equal(self->grammar, object->grammar)) {
    return true;
  }
  return false;
}

static void _inspect_(parser_t *self, inspect_t *inspect) {}

def_prototype_source(parser_t, _alloc_, _free_, _copy_, _equal_, _inspect_);

static grammar_pointer_t *alloc_grammar_pointer(grammar_t *self,
                                                list_t *grammar_path) {
  grammar_path_t *last = list_get(grammar_path, grammar_path_t, LIST_TAIL);
  list_t *options = grammar_get_options(self, last->rule->value);
  grammar_pointer_t *pointer = alloc(grammar_pointer_t);
  pointer->option = list_get(options, list_t, last->option);
  if (last->item < pointer->option->size) {
    pointer->item = list_get(pointer->option, pair_t, last->item);
  } else {
    pointer->item = NULL;
  }
  return pointer;
}

static ast_pointer_t *alloc_ast_pointer(ast_t *ast, list_t *ast_path) {
  ast_pointer_t *pointer = alloc(ast_pointer_t);
  pointer->item = list_get(ast->root, ast_item_t, LIST_HEAD);
  list_for(ast_path, index, ast_path_t, path, {
    pointer->parent = pointer->item;
    if (path->item == LIST_INVALID_INDEX) {
      pointer->item = NULL;
    } else {
      pointer->item = list_get(pointer->item->branch, ast_item_t, path->item);
    }
  });
  return pointer;
}

static void unnest_ast_branch(list_t *branch) {
  for (int index = (branch->size - 1); index >= 0; --index) {
    ast_item_t *item = list_get(branch, ast_item_t, index);
    if (text_compare(item->define, "rule")) {
      unnest_ast_branch(item->branch);
      if (item->nest == false) {
        while (item->branch->size > 0) {
          list_add(branch, (index + 1), list_extract(item->branch, LIST_TAIL));
        }
        list_del(branch, index);
      }
    }
  }
}

void parser_perform(parser_t *self, list_t *tokens) {
  list_t *processing = alloc(list_t);
  list_t *finalized = alloc(list_t);
  parser_context_t *best_choice = NULL;

  const char *begin_rule = self->grammar->begin->value;
  list_t *begin_options = grammar_get_options(self->grammar, begin_rule);
  list_for(begin_options, index, list_t, items, {
    parser_context_t *context = alloc(parser_context_t);
    grammar_path_t *grammar_path = alloc(grammar_path_t, begin_rule, index, 0);
    list_add(context->grammar_path, LIST_TAIL, grammar_path);
    ast_path_t *ast_path = alloc(ast_path_t, LIST_INVALID_INDEX);
    list_add(context->ast_path, LIST_TAIL, ast_path);
    ast_item_t *ast_item = alloc(ast_item_t, "rule", begin_rule);
    list_add(context->ast->root, LIST_TAIL, ast_item);
    list_add(processing, LIST_TAIL, context);
  });

  while (processing->size > 0) {
    list_t *move_indexes = alloc(list_t);
    list_t *insert_context = alloc(list_t);

    list_for(processing, index, parser_context_t, context, {
      grammar_pointer_t *grammar_pointer =
          alloc_grammar_pointer(self->grammar, context->grammar_path);
      ast_pointer_t *ast_pointer =
          alloc_ast_pointer(context->ast, context->ast_path);
      token_t *token = list_get(tokens, token_t, context->token_index);

      if (text_compare(grammar_pointer->item->key, "token")) {
        const char *token_type =
            pair_value(text_t, grammar_pointer->item)->value;
        if (text_compare(token->type, token_type)) {
          ast_item_t *ast_item = alloc(ast_item_t, "token", token_type);
          pointer_set(ast_item->token, token);
          ast_item->valid = true;
          list_add(ast_pointer->parent->branch, LIST_TAIL, ast_item);
          context->token_index++;

          grammar_path_t *last_grammar_path =
              list_get(context->grammar_path, grammar_path_t, LIST_TAIL);

          if (grammar_pointer->item == NULL) {
            last_grammar_path->item = 0;
          } else {
            last_grammar_path->item++;
          }

          while (last_grammar_path->item >= grammar_pointer->option->size) {
            list_del(context->grammar_path, LIST_TAIL);
            list_del(context->ast_path, LIST_TAIL);

            ast_pointer->parent->valid = true;

            if (context->grammar_path->size == 0) {
              list_add(move_indexes, LIST_HEAD,
                       alloc(number_t, cast(double, index)));
              goto next_context;
            }

            last_grammar_path =
                list_get(context->grammar_path, grammar_path_t, LIST_TAIL);
            last_grammar_path->item++;

            dealloc(grammar_pointer);
            dealloc(ast_pointer);
            grammar_pointer =
                alloc_grammar_pointer(self->grammar, context->grammar_path);
            ast_pointer = alloc_ast_pointer(context->ast, context->ast_path);
          }

          if (context->token_index >= tokens->size) {
            list_add(move_indexes, LIST_HEAD,
                     alloc(number_t, cast(double, index)));
          }
        } else {
          ast_item_t *ast_item = alloc(ast_item_t, "token", token->type->value);
          pointer_set(ast_item->token, token);
          text_set(ast_item->expected_token_type, token_type);
          list_add(ast_pointer->parent->branch, LIST_TAIL, ast_item);
          list_add(move_indexes, LIST_HEAD,
                   alloc(number_t, cast(double, index)));
        }
      } else {
        list_add(move_indexes, LIST_HEAD, alloc(number_t, cast(double, index)));

        const char *sub_rule = pair_value(text_t, grammar_pointer->item)->value;
        list_t *sub_options = grammar_get_options(self->grammar, sub_rule);
        ast_path_t *ast_path =
            list_get(context->ast_path, ast_path_t, LIST_TAIL);
        ast_path->item = ast_pointer->parent->branch->size;
        ast_item_t *ast_item = alloc(ast_item_t, "rule", sub_rule);
        if (text_compare(grammar_pointer->item->key, "nest")) {
          ast_item->nest = true;
        } else {
          ast_item->nest = false;
        }
        list_add(ast_pointer->parent->branch, LIST_TAIL, ast_item);

        list_for(sub_options, index_option, list_t, option, {
          parser_context_t *sub_context = copy(context);
          grammar_path_t *sub_grammar_path =
              alloc(grammar_path_t, sub_rule, index_option, 0);
          list_add(sub_context->grammar_path, LIST_TAIL, sub_grammar_path);
          ast_path_t *sub_ast_path = alloc(ast_path_t, LIST_INVALID_INDEX);
          list_add(sub_context->ast_path, LIST_TAIL, sub_ast_path);
          list_add(insert_context, LIST_TAIL, sub_context);
        });
      }

    next_context:
      dealloc(grammar_pointer);
      dealloc(ast_pointer);
    });

    list_for(move_indexes, index_loop, number_t, number, {
      list_add(finalized, LIST_TAIL, list_extract(processing, number->value));
    });

    // keep best context and dealloc the others, reduces memory use
    if (best_choice == NULL) {
      best_choice = list_extract(finalized, LIST_HEAD);
    }
    while (finalized->size) {
      parser_context_t *choice = list_extract(finalized, LIST_HEAD);
      if (choice->token_index >= best_choice->token_index) {
        dealloc(best_choice);
        best_choice = choice;
      } else {
        dealloc(choice);
      }
    }

    while (insert_context->size) {
      list_add(processing, LIST_TAIL, list_extract(insert_context, LIST_HEAD));
    }

    dealloc(move_indexes);
    dealloc(insert_context);
  }

  // ast_branch_print(best_choice->ast->root);
  // printf("\n\n");

  unnest_ast_branch(best_choice->ast->root);

  dealloc(self->ast);
  self->ast = best_choice->ast;
  best_choice->ast = alloc(ast_t);
  dealloc(best_choice);

  dealloc(processing);
  dealloc(finalized);
}

ast_t *parser_extract_ast(parser_t *self) {
  ast_t *extract = self->ast;
  self->ast = alloc(ast_t);
  return extract;
}

#include "scanner.h"

void parser_test() {
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

  dealloc(parser);
  dealloc(scanner);
}
