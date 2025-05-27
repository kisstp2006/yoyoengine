#!/usr/bin/env bash

# Create a new build directory
mkdir "./build"
mkdir "./build/linux"

# Navigate into the build directory
cd "./build/linux"

# Run cmake only if CMakeCache.txt is not present
if [ ! -f CMakeCache.txt ]; then
    echo "Running cmake..."
    cmake -DCMAKE_BUILD_TYPE=Debug ../..
    if [ $? -eq 0 ]
    then
        echo "cmake configuration succeeded."
    else
        echo "cmake configuration failed."
        exit 1
    fi
else
    echo "CMakeCache.txt found. Skipping cmake configuration."
fi

# Run make to build the project
echo "Running make..."
make -j$(nproc)
if [ $? -eq 0 ]
then
    echo "make build succeeded."
else
    echo "make build failed."
    exit 1
fi