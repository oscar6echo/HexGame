#! /bin/bash

mkdir -p obj
mkdir -p bin
g++ -g -Wall -Wno-sign-compare -fexceptions -c -std=c++11 ./src/main.cpp -o ./obj/main.o
g++ -g -Wall -Wno-sign-compare -fexceptions -c -std=c++11 ./src/HexGame.cpp -o ./obj/HexGame.o
g++ -g -Wall -Wno-sign-compare -fexceptions -c -std=c++11 ./src/Random.cpp -o ./obj/Random.o
g++ -g -Wall -Wno-sign-compare -fexceptions -c -std=c++11 ./src/TreeNode.cpp -o ./obj/TreeNode.o
g++ -g -Wall -Wno-sign-compare -fexceptions -c -std=c++11 ./src/Stone.cpp -o ./obj/Stone.o
g++ ./obj/main.o ./obj/HexGame.o ./obj/Random.o ./obj/TreeNode.o ./obj/Stone.o -o ./bin/main.out
