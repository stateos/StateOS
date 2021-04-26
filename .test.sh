#!/bin/bash

set -e

for file in examples/*.c
do
	echo $file
	cp $file examples/main.c
	make -f examples.make all
	rm -f examples/main.*
done

for file in examples/*.cpp
do
	echo $file
	cp $file examples/main.cpp
	make -f examples.make all
	rm -f examples/main.*
done

make -f examples.make clean
