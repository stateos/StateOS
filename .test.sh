#!/bin/bash

set -e

for file in examples/*.c
do
	echo $file
	cp $file examples/main.c
	touch examples/main.c
	make -f examples.make all
done

for file in examples/*.cpp
do
	echo $file
	cp $file examples/main.cpp
	touch examples/main.cpp
	make -f examples.make all
done

make -f examples.make clean
