#!/bin/bash

./cleanup.sh
make
./manager.out 10 10
./result.sh