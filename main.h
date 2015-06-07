#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED
#include "omp.h"
#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <cassert>
#include <stdio.h>
#include <vector>
#include "OCTester.h"
#include "Tree.h"
#include <cstdlib>
#include <string>

#define BUFFER_SIZE 1000000

void printList();
void sortList(double listToSort[], int length);

void sendMessage(int, string);
string receiveMessage();
MPI_Status receiveMessage(MPI_Status);
#endif // MAIN_H_INCLUDED

