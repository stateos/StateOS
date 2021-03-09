#!/bin/bash

set -e

if [ ! -d examples ]; then
	make all -f makefile.gnucc
	exit 0
fi

for file in examples/*.c
do
	rm -f src/main.*
	echo $file
	cp $file src/main.c
	make all -f examples_c.make
done

for file in examples/*.cpp
do
	rm -f src/main.*
	echo $file
	cp $file src/main.cpp
	make all -f examples_cpp.make
done
