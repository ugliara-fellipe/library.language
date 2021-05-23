#
# Copyright 2020 Fellipe Augusto Ugliara
#
# Use of this source code is governed by an ISC license that can be found 
# in the LICENSE file at https://github.com/ugliara-fellipe/library.language
#
# --------------------
#
# Run this script to formatter all the C code files
# It require clang-format, to install use the command:
#
#   $ sudo apt install clang-format
#
clang-format -i *.h
clang-format -i *.c
clang-format -i */*.h
clang-format -i */*.c
