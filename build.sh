#!/bin/sh

cmake -B build -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=Y -GNinja
