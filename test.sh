#!/bin/sh
./c99 bench/8q.c >8q.s
cc 8q.s -o 8q
./8q
