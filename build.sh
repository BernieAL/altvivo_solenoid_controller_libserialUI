#!/usr/bin/bash

SRC_FILES="main.cpp SolenoidControl.cpp SolenoidState.cpp SerialComm.cpp"

# Remove prev build
rm -fv test-solenoid

# Recompile
g++ -o test-solenoid -std=c++11 $SRC_FILES -g

# Execute program
./test-solenoid