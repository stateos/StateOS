#!/bin/bash

set -e

for file in stdc++/*.cpp
do
	echo $file
	make -s -f .stdc++-test.make MAIN=$file all
done

make -s -f .stdc++-test.make clean
