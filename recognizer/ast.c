//
// Copyright 2020 Fellipe Augusto Ugliara
//
// Use of this source code is governed by an ISC license that can be found
// in the LICENSE file at https://github.com/ugliara-fellipe/library.language
//
#include "recognizer/ast.h"
#include "toolbelt.h"

static void _alloc_path_(ast_path_t *self, args_t arguments) {
  index_t item = next_arg(arguments, int);
  self->item = item;
}

static void _free_path_(ast_path_t *self) {}

static void _copy_path_(ast_path_t *self, ast_path_t *object) {
  object->item = self->item;
}

static bool _equal_path_(ast_path_t *self, ast_path_t *object) {
  if (self->item == object->item) {
    return true;
  }
  return false;
}

static void _inspect_path_(ast_path_t *self, inspect_t *inspect) {}

def_prototype_source(ast_path_t, _alloc_path_, _free_path_, _copy_path_,
                     _equal_path_, _inspect_path_);

static void _alloc_item_(ast_item_t *self, args_t arguments) {
  char *define = next_arg(arguments, char *);
  self->define = alloc(text_t, define);
  char *type = next_arg(arguments, char *);
  self->type = alloc(text_t, type);
  self->valid = false;
  self->branch = alloc(list_t);
  self->token = alloc(pointer_t, NULL);
  self->nest = true;
  self->expected_token_type = alloc(text_t, "");
}

static void _free_item_(ast_item_t *self) {
  dealloc(self->define);
  dealloc(self->type);
  dealloc(self->branch);
  dealloc(self->token);
  dealloc(self->expected_token_type);
}

static void _copy_item_(ast_item_t *self, ast_item_t *object) {
  object->define = copy(self->define);
  object->type = copy(self->type);
  object->valid = self->valid;
  object->branch = copy(self->branch);
  object->token = copy(self->token);
  object->nest = self->nest;
  object->expected_token_type = copy(self->expected_token_type);
}

static bool _equal_item_(ast_item_t *self, ast_item_t *object) {
  if (equal(self->define, object->define) && equal(self->type, object->type) &&
      equal(self->token, object->token) && (self->valid == object->valid) &&
      (self->nest == object->nest) && equal(self->branch, object->branch)) {
    return true;
  }
  return false;
}

static void _inspect_item_(ast_item_t *self, inspect_t *inspect) {}

def_prototype_source(ast_item_t, _alloc_item_, _free_item_, _copy_item_,
                     _equal_item_, _inspect_item_);

static void _alloc_pointer_(ast_pointer_t *self, args_t arguments) {
  self->parent = NULL;
  self->item = NULL;
}

static void _free_pointer_(ast_pointer_t *self) {}

static void _copy_pointer_(ast_pointer_t *self, ast_pointer_t *object) {
  object->parent = self->parent;
  object->item = self->item;
}

static bool _equal_pointer_(ast_pointer_t *self, ast_pointer_t *object) {
  if ((self->parent == object->parent) && (self->item == object->item)) {
    return true;
  }
  return false;
}

static void _inspect_pointer_(ast_pointer_t *self, inspect_t *inspect) {}

def_prototype_source(ast_pointer_t, _alloc_pointer_, _free_pointer_,
                     _copy_pointer_, _equal_pointer_, _inspect_pointer_);

static void _alloc_(ast_t *self, args_t arguments) {
  self->root = alloc(list_t);
}

static void _free_(ast_t *self) { dealloc(self->root); }

static void _copy_(ast_t *self, ast_t *object) {
  object->root = copy(self->root);
}

static bool _equal_(ast_t *self, ast_t *object) {
  return equal(self->root, object->root);
}

static void _inspect_(ast_t *self, inspect_t *inspect) {}

def_prototype_source(ast_t, _alloc_, _free_, _copy_, _equal_, _inspect_);

token_t *ast_get_token(ast_item_t *rule, index_t index) {
  return cast(token_t *,
              pointer_get(list_get(rule->branch, ast_item_t, index)->token));
}

static void ast_branch_print_level(list_t *branch, int level_identation) {
  list_for(branch, index, ast_item_t, item, {
    printf("%*s%s: %s: %d: %d\n", level_identation, "", item->define->value,
           item->type->value, item->valid, item->nest);
    if (text_compare(item->define, "rule")) {
      level_identation += 2;
      ast_branch_print_level(item->branch, level_identation);
      level_identation -= 2;
    }
  });
}

void ast_branch_print(list_t *branch) { ast_branch_print_level(branch, 0); }

void ast_test() {
  ast_t *ast = alloc(ast_t);
  assert(type_equal(ast, "ast_t") == true);
  dealloc(ast);
}
