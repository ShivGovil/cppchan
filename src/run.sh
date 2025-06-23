#!/bin/bash

rm -rf build .cache
cmake -B build
cd build
make
cd ..
./build/test/test_waitgroups
