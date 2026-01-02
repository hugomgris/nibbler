#!/bin/bash

g++ -std=c++20 -fPIC -shared plugin.cpp -o libplugin.so
g++ -std=c++20 main.cpp -ldl -o host
./host

rm -f libplugin.so host
