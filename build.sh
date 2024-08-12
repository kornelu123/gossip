#!/bin/sh

cmake -B ./tests/build -S ./tests -DCMAKE_EXPORT_COMPILE_COMMANDS=Y -GNinja

ninja -C ./tests/build

touch ./tests/build/test

./tests/build/tests

cmake -B build -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=Y -GNinja
