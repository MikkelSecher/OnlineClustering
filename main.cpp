#include "main.h"

using namespace std;
using std::atoi;

int main(int argc, char* argv[])
{



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



    ///Performance variables
    double startTimeBF;
    double startTimeDF;
    double startTimeSend;
    double endTimeBF;
    double endTimeDF;
    double timeToSpinUp;
    double timeToSend;
    double timeInDF;
    double totalTime;
    int initialQueueSize;
    double startingPoint = 0;
    int messagesSent[world_size];

    int     levelsOfBF      = atoi(argv[1]);
    int     levelsOfDF      = atoi(argv[2]); //Depth for DF search to stop
    double  ratio           = atof(argv[3]); //Threshold ratio
    int     prefix          = atoi(argv[4]); //Prefix for result files
    int     numberOfChunks  = atoi(argv[5]); //number of chunks to split up in
    int     print           = 2; //Level of printing

    list<double> deltaValues;

    list<double> testSequence;

    deltaValues = {1, -1, 0.5};

    if(numberOfChunks < world_size){
        cout << "THERE MUST BE AT LEAST AS MANY CHUNKS AS THERE ARE NODES" << endl;
        exit(1);
    }


    //For regular runs

    Tree tree(startingPoint, deltaValues, prefix, ratio, print);
    //For serious MPI test
    tree = test1.getMPITree();

    //First argument is the number of levels the program should go down breadth first...
    tree.worldRank = world_rank;
    tree.worldSize = world_size;
    tree.numberOfThreads = world_size;
    int startingLevel = testSequence.size();

    if (startingLevel == 0){
        startingLevel = 1;
    }

    //Build initial tree before sending data out to nodes
    if(world_rank == 0){

        ///Build BF Tree
        startTimeBF = MPI_Wtime();
        cout << "Building BF tree" << endl;
        for(int level = startingLevel; level < levelsOfBF+startingLevel; level++){
            cout << "Level: " << level << endl;
            cout << "NodeQueue size: " << tree.nodeQueue.size() << endl;
            cout << endl;
            tree.addLevelBF(level);
            tree.treeCleanup();
        }

        timeToSpinUp = MPI_Wtime()-startTimeBF;

        startTimeSend = MPI_Wtime();
        ///Prepare nodeQueue to be sent
        tree.nodeQueue.sort( node_compare_sequence );
        tree.sequenceTree();
        initialQueueSize = tree.sequencedTreeQueue.size();
        ///Create the messages
        tree.createMessages(numberOfChunks);
        list<string>::iterator messageIterator = tree.messageQueue.begin();

        ///Send initial message to all nodes
        for(int targetNode = 1; targetNode < world_size; targetNode++){

            sendMessage(targetNode, tree.messageQueue.front());
            tree.messageQueue.pop_front();
            cout << "Message sent to " << targetNode << endl;
            messagesSent[targetNode] = 1;
        }

        timeToSend = MPI_Wtime() - startTimeSend;
        startTimeDF = MPI_Wtime();
        MPI_Status status;

        ///Wait to get "done" signal from a node, and send a new message with work
        int stopCount = 0;
        while (true){
            status = receiveMessage(status);

            cout << "Done from " << status.MPI_SOURCE << endl;
            if(tree.messageQueue.size() > 0){
                sendMessage(status.MPI_SOURCE, tree.messageQueue.front());
                cout << "Message sent to " << status.MPI_SOURCE << endl;
                tree.messageQueue.pop_front();
                ++messagesSent[status.MPI_SOURCE];
            }else{
                sendMessage(status.MPI_SOURCE, "Stop");
                ++stopCount;
                if(stopCount == world_size-1){
                    break;
                }
            }
        }

    }

    ///Work to be done by each worker node
    if(world_rank != 0){
        bool workLeft = true;
        while(workLeft){
            //Make new tree (empty)
            Tree myTree(startingPoint, deltaValues, prefix, ratio, print);

            myTree.worldRank = world_rank;
            string fullMessage = receiveMessage();

            //If the isn't the stop signal build nodequeue from the message
            if(fullMessage.compare("Stop") != 0){

                myTree.buildNodesFromString(fullMessage);
                //Start the actual proof searching
                myTree.startDF(0, levelsOfDF);
                //Report back to master
                sendMessage(0, "Done");
            }else{
                workLeft = false;
            }

        }

    }


    cout << endl;
    //Wait for all threads to finish work before calculating metrics
    MPI_Barrier(MPI_COMM_WORLD);
    if(world_rank == 0){

        endTimeDF = MPI_Wtime();

        timeInDF = endTimeDF - startTimeDF;
        totalTime = endTimeDF - startTimeBF;

        FILE * pFile;
        char filename[30];
        sprintf(filename, "summary.txt");
        pFile = fopen (filename,"a");
        fprintf(pFile, "--------------- Summary for run with id: %d ----------------\n \n", prefix);

        fprintf(pFile, "Number of nodes: %d \n", world_size);
        fprintf(pFile, "Initial queuesize: %d \n", initialQueueSize);
        fprintf(pFile, "The queue was split into %d chunks of size %d \n", numberOfChunks, initialQueueSize/numberOfChunks );
        fprintf(pFile, "#Levels of BF: %d \n", levelsOfBF);
        fprintf(pFile, "#Levels of DF: %d \n", levelsOfDF);
        fprintf(pFile, "Ratio: %3.2f \n", ratio);


        fprintf(pFile, "Total runtime: %.2f \n", totalTime);
        fprintf(pFile, "Time in BF expansion: %.2f \n", timeToSpinUp);
        fprintf(pFile, "Time in DF expansion: %.2f \n", timeInDF);

        fprintf(pFile, "Number of messages sent to individual nodes: \n", timeInDF);
        for(int i = 1; i < world_size; i++){
            fprintf(pFile, "Node %d : %d \n", i, messagesSent[i]);
        }

        fclose(pFile);
    }

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


