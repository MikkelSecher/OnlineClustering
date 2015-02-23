CXX=g++

EXE = OnlineClustering

CXXFLAGS = -std=c++11 -O3 -Wall -Wextra -Wno-unused-parameter -fopenmp
CPPFLAGS = 

OBJES = main.o Tree.o TreeNode.o Partitioning.o 

LDFLAGS = -fopenmp
LDLIBS = -lpthread

all: $(EXE)

$(EXE): $(OBJES) 
	$(CXX) $(LDFLAGS) $(OBJES) $(LDLIBS) -o $(EXE)

clean: 
	rm -rf *o $(EXE)
	
	