#
# Copyright 2020 Fellipe Augusto Ugliara
#
# Use of this source code is governed by an ISC license that can be found 
# in the LICENSE file at https://github.com/ugliara-fellipe/library.language
#
# --------------------
#
# Run this script to deploy language library and tests
# It require clang; to install use the command:
#
#   $ sudo apt install clang
#
rm -rf ./deploy
mkdir deploy
mkdir deploy/object
mkdir deploy/include
mkdir deploy/include/regex
mkdir deploy/include/recognizer

TOOLBELT="../library.toolbelt/deploy"
TOOLBELT_INC="-I $TOOLBELT/include"
TOOLBELT_OBJS="$TOOLBELT/object/*.o"

clang regex/expression.c -c $TOOLBELT_INC -I . -o deploy/object/expression.o
clang regex/fsm.c -c $TOOLBELT_INC -I . -o deploy/object/fsm.o
clang regex/regex.c -c $TOOLBELT_INC -I . -o deploy/object/regex.o

clang recognizer/ast.c -c $TOOLBELT_INC -I . -o deploy/object/ast.o
clang recognizer/grammar.c -c $TOOLBELT_INC -I . -o deploy/object/grammar.o
clang recognizer/parser.c -c $TOOLBELT_INC -I . -o deploy/object/parser.o
clang recognizer/recognizer.c -c $TOOLBELT_INC -I . -o deploy/object/recognizer.o
clang recognizer/scanner.c -c $TOOLBELT_INC -I . -o deploy/object/scanner.o
clang recognizer/token.c -c $TOOLBELT_INC -I . -o deploy/object/token.o
clang recognizer/walker.c -c $TOOLBELT_INC -I . -o deploy/object/walker.o

clang test.c deploy/object/*.o $TOOLBELT_OBJS $TOOLBELT_INC -I . -o deploy/test

cp regex/*.h deploy/include/regex/
cp recognizer/*.h deploy/include/recognizer/
cp language.h deploy/include/
