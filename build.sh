#!/usr/bin/bash

SRC_FILES="solenoid-control.cpp SolenoidTimingDialog.cpp serial.cpp"


#remove prev build
rm -fv solenoid-control
#recompile
g++ -o solenoid-control -std=c++11 $SRC_FILES -g `wx-config --cxxflags --libs`


# Fix snap package interference (vscode)
#force to not look in snap folders
export GTK_PATH=
export GTK_MODULES=

#execute program
./solenoid-control                                       