#!/bin/bash

set -e

for file in examples/*.c
do
	echo $file
	make -s -f .example-test.make MAIN=$file all
done

for file in examples/*.cpp
do
	echo $file
	make -s -f .example-test.make MAIN=$file all
done

make -s -f .example-test.make clean
