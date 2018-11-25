#!/bin/bash
g++ -c -w mem.cpp
g++ -c -w driver.cpp 
g++ -o driver driver.o mem.o
./driver
