#!/bin/sh

set -xe

if [ ! -d "build/" ]; then 
	mkdir build
fi

cc -ggdb -I nobuild/ nobuild.c -o build/nobuild.o

if [ $? -eq 0 ]; then
	clear
	./build/nobuild.o $1
fi