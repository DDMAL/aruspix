#!/bin/bash


if [ $# -lt 1 ]; then
    echo "Missing version arguments"
    exit 0
fi

version=$1

cd installer
make VERSION=$version
mv Aruspix$version.dmg ../../bin
make clean
cd ..