#!/bin/bash

set -e

for file in examples/*.c
do
	echo $file
	cp $file examples/main.c
	touch examples/main.c
	make -f .example-test.make all
	rm -f examples/main.*
done

for file in examples/*.cpp
do
	echo $file
	cp $file examples/main.cpp
	touch examples/main.cpp
	make -f .example-test.make all
	rm -f examples/main.*
done

make -f .example-test.make clean
