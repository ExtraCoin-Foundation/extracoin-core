TARGET = mylib-test
SOURCES += main.cpp

include(../../test.pri)

# Relative path to libs
customLibs = ../extracoin

# Load and link all libs
include(../../loadlibs.pri)
