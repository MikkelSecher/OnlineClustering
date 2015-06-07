#include "main.h"

using namespace std;
using std::atoi;

int main(int argc, char* argv[])
{
    ///Comment in for tests



    MPI_Init(NULL, NULL);
    OCTester test1 = OCTester();
//    exit(1);

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

    string message;



//    double startTime = omp_get_wtime();
    double endTime;
    double startTimeBF;
    double endTimeBF;
    double startingPoint = 0;

    int     levelsOfBF      = atoi(argv[1]);
    int     levelsOfDF      = atoi(argv[2]); //Depth for DF search to stop
    double  ratio           = atof(argv[3]); //Threshold ratio
    int     prefix          = atoi(argv[4]); //Prefix for result files
    int     print           = 2; //Level of printing

    list<double> deltaValues;

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

    startTimeBF = 1;
    //Build initial tree before sending data out to nodes
    if(world_rank == 0){
        cout << "Building BF tree" << endl;
        for(int level = startingLevel; level < levelsOfBF+startingLevel; level++){
            cout << "Level: " << level << endl;
            cout << "NodeQueue size: " << tree.nodeQueue.size() << endl;
            cout << endl;
            tree.addLevelBF(level);
            tree.treeCleanup();
        }
        endTimeBF = omp_get_wtime();



        tree.nodeQueue.sort( node_compare_sequence );
        tree.sequenceTree();
//        cout <<" Size of sequenceQueue " <<  tree.sequencedTreeQueue.size() << endl;


        tree.createMessages(world_size-1);

        int targetNode = 1;
        list<string>::iterator messageIterator = tree.messageQueue.begin();

        for(messageIterator = tree.messageQueue.begin(); messageIterator != tree.messageQueue.end(); messageIterator++){
            sendMessage(targetNode, (*messageIterator));
            ++targetNode;
        }

        MPI_Status status;

        for(int i=0 ; i< ; i++){
            status = receiveMessage(status);
            cout << "Done from " << status.MPI_SOURCE << endl;
        }
    }


    if(world_rank != 0){
        bool workLeft = true;
        while(workLeft){
            Tree myTree(startingPoint, deltaValues, prefix, ratio, print);

            string fullMessage = receiveMessage();

            if(fullMessage.compare("Stop") != 0){

                myTree.buildNodesFromString(fullMessage);

                myTree.startDF(0, levelsOfDF);

                sendMessage(0, "Done");
            }else{
                workLeft = false;
            }

        }

    }

//
//    if(world_rank != 0){
//        string message = receiveMessage();
//
//        cout <<" got message: "<< message << endl;
//
//    }

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


void sendMessage(int receiver, string message){
    char buffer[BUFFER_SIZE];
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    strcpy(buffer, message.c_str());

    MPI_Send(buffer, BUFFER_SIZE , MPI_CHAR, receiver, world_rank, MPI_COMM_WORLD);

}

string receiveMessage(){
    char buffer[BUFFER_SIZE];
    MPI_Status status;

    MPI_Recv(buffer,
        BUFFER_SIZE,
        MPI_CHAR,
        MPI_ANY_SOURCE,
        MPI_ANY_TAG,
        MPI_COMM_WORLD,
        &status);

    return buffer;
}

MPI_Status receiveMessage(MPI_Status status){
    char buffer[BUFFER_SIZE];

    MPI_Recv(buffer,
        BUFFER_SIZE,
        MPI_CHAR,
        MPI_ANY_SOURCE,
        MPI_ANY_TAG,
        MPI_COMM_WORLD,
        &status);


    return status;
}


