CXX = mpic++

EXE = OnlineClustering

CXXFLAGS = $(shell mpicc --showme:compile) -std=c++11 -O3 -Wall -Wextra -Wno-unused-parameter
CPPFLAGS = 


OBJES = main.o Tree.o TreeNode.o Partitioning.o OCTester.o

LDFLAGS = 
LDLIBS = -lmpi_cxx $(shell mpicc --showme:link)

all: $(EXE)

$(EXE): $(OBJES) 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJES) $(LDLIBS) -o $(EXE)

clean: 
	rm -rf *o $(EXE)

