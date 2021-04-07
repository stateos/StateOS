#!/bin/bash

set -e

for file in libstdc++/*.cpp
do
	echo $file
	cp $file libstdc++/main.cpp
	make -f libstdc++.make all
	rm -f libstdc++/main.*
done

make -f libstdc++.make clean
