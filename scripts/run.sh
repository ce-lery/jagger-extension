#!/bin/bash

# export PATH=$PWD/jagger-2023-02-18/src:$PATH
# export LD_LIBRARY_PATH=/usr/local/lib/:$LD_LIBRARY_PATH
# export LIBRARY_PATH=/usr/local/lib/:$LIBRARY_PATH
# export CPLUS_INCLUDE_PATH=$PWD/jagger-2023-02-18/src:$PATH

mkdir -p build
cd build

cmake ..
make all

./JaggerExtension
# valgrind -s  --leak-check=full ./JaggerExtension

