# makefile to build the whole thing

CPP=main.cpp simulation.cpp site.cpp individual.cpp 
HPP=simulation.hpp individual.hpp site.hpp parameters.hpp
EXE=migration_cues.exe
CXX=g++
CXXFLAGS=-Wall -Wconversion -ggdb #-O3

$(EXE) : $(CPP) $(HPP)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(CPP)


clean : 
	rm -rf $(EXE)
