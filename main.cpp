#include "main.h"

using namespace std;
using std::atoi;

int main(int argc, char* argv[])
{
    ///Comment in for tests

//    exit(1);


    MPI_Init(NULL, NULL);
    OCTester test1 = OCTester();

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

////     Print off a hello world message
//    printf("Hello world from processor %s, rank %d"
//           " out of %d processors\n",
//           processor_name, world_rank, world_size);
//


    cout << "This is after finalize..." << endl;
    string message;
    char buffer[128];


    if(world_rank == 0){
        message = "This is a cool message! ";
        sprintf(buffer, "Hello");
        for(int w = 1; w < world_size; w++){
            MPI_Send(buffer, 128, MPI_CHAR, w, 0, MPI_COMM_WORLD);
        }
    }
    MPI_Status status;
    if(world_rank != 0){
        MPI_Recv(buffer,      /* message buffer */
        128,                 /* one data item */
        MPI_CHAR,           /* of type char real */
        MPI_ANY_SOURCE,    /* receive from any sender */
        MPI_ANY_TAG,       /* any type of message */
        MPI_COMM_WORLD,    /* default communicator */
        &status);

        cout << "FREAKING AWESOME! I, " << processor_name << ", got the message: " << buffer << " at node " << world_rank<<  endl;
    }


//    double startTime = omp_get_wtime();
    double endTime;
    double startTimeBF;
    double endTimeBF;
    double startingPoint = 0;

    int     levelsOfBF      = atoi(argv[1]);
    int     levelsOfDF      = atoi(argv[2]); //Depth for DF search to stop
    double  ratio           = atof(argv[3]); //Threshold ratio
    int     prefix          = atoi(argv[4]); //Prefix for result files
//    int     print           = atoi(argv[5]); //Level of printing
    int     print           = 2; //Level of printing
    //int     numberOfDeltas  = atoi(argv[6]); //Number of deltas in the list

    list<double> deltaValues;

//    if(numberOfDeltas != argc-7){
//            cout << "Number of deltas must match second argument " << argc-6 << " and " << numberOfDeltas << endl;
//            return 0;
//    }
//    for(int i = 7; i < argc; i++){
//            deltaValues.push_front(atof(argv[i]));
//    }
    list<double> testSequence;

    deltaValues = {1, -1, 0.5};

    //For regular runs
    Tree tree(startingPoint, deltaValues, prefix, ratio, print);
    //First argument is the number of levels the program should go down breadth first...
    tree.worldRank = world_rank;
    tree.worldSize = world_size;
    tree.numberOfThreads = world_size;
    int startingLevel = testSequence.size();

    if (startingLevel == 0){
        startingLevel = 1;
    }

//    tree = test1.getDefaultStartTree();
//    tree = test1.getNewLowerBoundTree();


    if(prefix == 100){
        tree = test1.getNewBaseTree();
    }
    if(prefix == 200){
        tree = test1.getSecondBaseTree();
    }

    startTimeBF = 1;
    for(int level = startingLevel; level < levelsOfBF+startingLevel; level++)
    {
        cout << "Level: " << level << endl;
        cout << "NodeQueue size: " << tree.nodeQueue.size() << endl;
        cout << endl;
        tree.addLevelBF(level);
        tree.treeCleanup();
    }
    endTimeBF = omp_get_wtime();

    cout << "Finished cleanup - starting depth first" << endl;

    tree.startDF(levelsOfBF, levelsOfDF);

    cout << endl;
    endTime = 1;

//    FILE * pFile;
//    char filename[30];
//    sprintf(filename, "%n_res_%d_Summary.txt", world_rank, prefix);
//    pFile = fopen (filename,"w");
//    fprintf(pFile, "Summary for run with id: %d \n \n", prefix);
//    fprintf(pFile, "#Threads: %d \n", tree.numberOfThreads);
//    fprintf(pFile, "#Levels of BF: %d \n", levelsOfBF);
//    fprintf(pFile, "#Max level for DF: %d \n", levelsOfDF);
//    fprintf(pFile, "Ratio: %3.2f \n", ratio);
//    fprintf(pFile, "Deltas: {");
//    list<double>::iterator i;
//    for(i = deltaValues.begin(); i != deltaValues.end(); i++)
//        fprintf(pFile, " %3.2f ", (*i));
//    fprintf(pFile, "} \n");
//    fprintf(pFile, "Total runtime: %.2f \n", endTime-startTime);
//    fprintf(pFile, "Time in BF expansion: %.2f \n", endTimeBF-startTimeBF);
//    fprintf(pFile, "Time in DF expansion: %.2f \n", tree.dfTime);
//    fprintf(pFile, "Time in proof: %.2f \n \n", tree.proofTime);
//    fprintf(pFile, "Number of proofs found: %d \n", tree.successes);
//
//     Finalize the MPI environment.
    MPI_Finalize();
}


