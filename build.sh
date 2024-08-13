#!/bin/sh

succ_build=0

echo $1

cmake -B ./tests/build -S ./tests -DCMAKE_EXPORT_COMPILE_COMMANDS=Y -GNinja

ninja -C ./tests/build

./tests/build/tests; ret_val=$?

if [ $ret_val -ne  $succ_build ]; then
  echo "Tests failed, check above messages to get more info"
  exit
fi

rm ./test

cmake -B build -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=Y -DBUILD_TARGET="$1" -GNinja

ninja -C ./build
