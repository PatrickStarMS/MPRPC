#!/bin/bash

set -e
echo `pwd`


if [ -d "$(pwd)/build" ]; then
    rm -rf `pwd`/build
else
    mkdir `pwd`/build
cd `pwd`/build &&
    cmake .. &&
    make

cd ..
cp -r `pwd`/src/include `pwd`/lib
fi