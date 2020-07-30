#
# Copyright (c) 2020, Fellipe Augusto Ugliara
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

DATATYPE="../library.datatype/deploy"
MODULE_INC="-I $DATATYPE/include"
DATATYPE_INC="$MODULE_INC/abstract $MODULE_INC/composite $MODULE_INC/other $MODULE_INC/primitive $MODULE_INC/toolbelt"
DATATYPE_OBJS="$DATATYPE/object/*.o"

clang regex/expression.c -c $DATATYPE_INC -o deploy/object/expression.o
clang regex/fsm.c -c $DATATYPE_INC -o deploy/object/fsm.o
clang regex/regex.c -c $DATATYPE_INC -o deploy/object/regex.o

clang recognizer/ast.c -c $DATATYPE_INC -o deploy/object/ast.o
clang recognizer/grammar.c -c $DATATYPE_INC -o deploy/object/grammar.o
clang recognizer/parser.c -c $DATATYPE_INC -o deploy/object/parser.o
clang recognizer/recognizer.c -c $DATATYPE_INC -o deploy/object/recognizer.o
clang recognizer/scanner.c -c $DATATYPE_INC -I regex -o deploy/object/scanner.o
clang recognizer/token.c -c $DATATYPE_INC -o deploy/object/token.o
clang recognizer/walker.c -c $DATATYPE_INC -o deploy/object/walker.o

clang test.c deploy/object/*.o $DATATYPE_OBJS $DATATYPE_INC -I regex -I recognizer -o deploy/test

cp regex/*.h deploy/include/regex/
cp recognizer/*.h deploy/include/recognizer/
