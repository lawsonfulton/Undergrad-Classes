#!/bin/bash

./router 1 $1 $2 $3 &
sleep 0.1
./router 2 $1 $2 $3 &
sleep 0.1
./router 3 $1 $2 $3 &
sleep 0.1
./router 4 $1 $2 $3 &
sleep 0.1
./router 5 $1 $2 $3 &
sleep 0.1