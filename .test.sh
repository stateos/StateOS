#!/bin/bash

set -e

if [ ! -d examples ]; then
	make all -f makefile.gnucc
	exit 0
fi

for file in examples/*.c
do
	rm -f examples/main.*
	echo $file
	cp $file examples/main.c
	make all -f c.make
done

for file in examples/*.cpp
do
	rm -f examples/main.*
	echo $file
	cp $file examples/main.cpp
	make all -f cpp.make
done
