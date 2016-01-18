#!/bin/bash

ls -l input | awk '{ sum += $5 }; END { print "number of input characters: ",sum }'
ls -l output | awk '{ sum += $5 }; END { print "number of output characters: ",sum }'