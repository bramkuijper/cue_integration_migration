# makefile to build the whole thing

CPP=main.cpp simulation.cpp individual.cpp 
HPP=simulation.hpp individual.hpp parameters.hpp
EXE=main.exe
CXX=g++
CXXFLAGS=-Wall -Wconversion -O3

$(EXE) : $(CPP) $(HPP)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(CPP)
