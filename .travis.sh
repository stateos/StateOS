#!/bin/bash

set -e

if [ ! -d examples ]; then
	make all GNUCC=arm-none-eabi- -f makefile.gnucc
	exit 0
fi

for file in examples/*.c\ *
do
	rm -f src/main.*
	echo $file
	cp "$file" src/main.c
	make all GNUCC=arm-none-eabi- -f makefile.gnucc
done

for file in examples/*.cpp*
do
	rm -f src/main.*
	echo $file
	cp "$file" src/main.cpp
	make all GNUCC=arm-none-eabi- -f makefile.gnucc
done
