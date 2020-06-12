#!/bin/bash

if [ ! -d build  ];then
  mkdir build
fi
cd build

target=$1
if [ $target==all ]; then
	cmake ..
	make
fi
