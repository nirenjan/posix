#!/bin/sh

set -e
./autogen.sh

# Run the build in a separate workspace
mkdir build
cd build

# Run the build
../configure
make V=0

# Run distcheck
make distcheck

