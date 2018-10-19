#!/bin/bash

g++ -o mutex spin.cc -lpthread
./mutex 2 1
./mutex 2 1000
./mutex 2 1000000
./mutex 4 1
./mutex 4 1000
./mutex 4 1000000
./mutex 6 1
./mutex 6 1000
./mutex 6 1000000
g++ -o spin -DUSE_SPINLOCK spin.cc -lpthread
./spin 2 1
./spin 2 1000
./spin 2 1000000
./spin 4 1
./spin 4 1000
./spin 4 1000000
./spin 6 1
./spin 6 1000
./spin 6 1000000

g++ -o rwlock -DUSE_RWLOCK rwlock.cc -lpthread
./rwlock 2
./rwlock 4
./rwlock 6
g++ -o mutex rwlock.cc -lpthread
./mutex 2
./mutex 4
./mutex 6

g++ -o cvlock cvlock.cc -lpthread
./cvlock 2
./cvlock 4
./cvlock 6
