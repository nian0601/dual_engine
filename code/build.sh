#!/bin/bash

CODE_HOME="$PWD"
OPTS=-g
cd asd > /dev/null
g++ $OPTS $CODE_HOME/asd -o asd
cd $CODE_HOME > /dev/null
