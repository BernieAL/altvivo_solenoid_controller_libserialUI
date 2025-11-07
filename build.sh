#!/usr/bin/bash

# Source files
SRC_FILES="main_gui.cpp SolenoidControl.cpp SolenoidState.cpp SerialComm.cpp SolenoidTimingDialog.cpp"

# Remove previous build
rm -fv solenoid-control

# Get wxWidgets flags (NOTE THE = SIGNS!)
WX_CXXFLAGS=`wx-config --cxxflags`
WX_LIBS=`wx-config --libs`

# Recompile with all flags
echo "Compiling..."
g++ -o solenoid-control -std=c++11 $SRC_FILES $WX_CXXFLAGS $WX_LIBS -lserialport -g


./solenoid-control 2>/dev/null