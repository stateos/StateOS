#!/bin/bash

set -e

for file in stdc++/*.cpp
do
	echo $file
	cp $file stdc++/main.cpp
	touch stdc++/main.cpp
	make -f stdc++.make all
	rm -f stdc++/main.*
done

make -f stdc++.make clean
