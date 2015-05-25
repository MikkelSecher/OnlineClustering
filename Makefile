CXX = mpic++

EXE = OnlineClustering

CXXFLAGS = -I /usr/lib/openmpi/include -std=c++11 -O3 -Wall -Wextra -Wno-unused-parameter -fopenmp
CPPFLAGS = 

OBJES = main.o Tree.o TreeNode.o Partitioning.o OCTester.o

LDFLAGS = -fopenmp
LDLIBS = -lpthread -lmpi_cxx -lmpi

all: $(EXE)

$(EXE): $(OBJES) 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJES) $(LDLIBS) -o $(EXE)

clean: 
	rm -rf *o $(EXE)
	
	
