#include "main.h"

using namespace std;
using std::atoi;


//USAGE OF ARGUMENTS X1 X2 X3 X[i]
// int X1 = number of levels before starting DF
// double X2 = Ratio
// int X3 = Number of delta values
// double X[i] = List of delta values, must correspond to Y in amount
// int Xn-2 = ID for the run, will be used for results and summary
// {0,1,2} Xn = Print all trees(2), print first tree(1), print no trees(0)

int main(int argc, char* argv[])
{
    double startTime = omp_get_wtime();
    double endTime;
    double startTimeBF;
    double endTimeBF;
    double startingPoint = 0;

    int depthFirstLevel = atoi(argv[1]);
    int dfDepth = atoi(argv[2]);
    double ratio = atof(argv[3]);
    int prefix = atoi(argv[4]);
    int print = atoi(argv[5]);
    int numberOfDeltas = atoi(argv[6]);

    list<double> deltaValues;

    list<double> testSequence;
    /** TESTING SEQUENCER **/
    //testSequence = {0.5};
    //list<double> testSequence = {2,3,1,4,0,5,-0.5,-1,0.5};
    //list<double> testSequence = {2,1,1.5,3,0,2.5,-0.5,0.5,-1};
    //testSequence = {3,4,5,6,2,1,0,2.5,7,8, 4.5, 8.5, 9, 10, 11, 9.5, 12};
    //testSequence = {0.00, 0.50, 1.50, 2.00, 2.50, 3.00,-0.50, 3.50, -1.00};

    //cout<< "Should never get here" << endl;


    //Check for correct argument numbers
    if(numberOfDeltas != argc-7)
    {
            cout << "number of deltas must match second argument " << argc-6 << " and " << numberOfDeltas << endl;
            return 0;
    }

    for(int i = 7; i < argc; i++)
    {
            deltaValues.push_front(atof(argv[i]));
    }
    cout << "Run ID: "<< prefix << endl;
    cout << "Ratio: " << ratio << endl;
    cout << "Deltas: ";
    list<double>::iterator i;
    for(i = deltaValues.begin(); i != deltaValues.end(); i++)
        cout << (*i) << " ";
    cout << endl;
    startTimeBF = omp_get_wtime();
    Tree tree(startingPoint, deltaValues, prefix, ratio, print);
    //Tree tree(testSequence, deltaValues, prefix, ratio, print);
    //exit(1);
    //First argument is the number of levels the program should go down breadth first...
    int startingLevel = testSequence.size();

    if (startingLevel == 0)
    {
        startingLevel = 1;
    }

    for(int i = startingLevel; i < depthFirstLevel+startingLevel; i++)
    {
        cout << "Level: " << i << endl;
        cout << "NodeQueue size: " << tree.nodeQueue.size() << endl;
        cout << endl;
        tree.nextPoint(i);
        tree.treeCleanup();
    }
    endTimeBF = omp_get_wtime();

    FILE * pFile;
    pFile = fopen ("myfile.tex","w"); //cleans the previous file
    fprintf(pFile, " ");
    fclose (pFile);
    pFile = fopen ("mytree.gml","w"); //cleans the previous file
    fprintf(pFile, " ");
    fclose (pFile);
    pFile = fopen ("subProofSequences.txt","w");
    fprintf(pFile, " ");
    fclose(pFile);



    cout << endl;
    cout << endl;
    cout << "Finished cleanup - starting depth first" << endl;
    tree.startDF(depthFirstLevel, dfDepth);
    cout << endl;
    endTime = omp_get_wtime();

    char filename[30];
    sprintf(filename, "res_%d_Summary.txt", prefix);
    pFile = fopen (filename,"w");
    fprintf(pFile, "Summary for run with id: %d \n \n", prefix);
    fprintf(pFile, "#Threads: %d \n", tree.numberOfThreads);

    fprintf(pFile, "#Levels before DF: %d \n", depthFirstLevel);
    fprintf(pFile, "#Levels with DF: %d \n", dfDepth);
    fprintf(pFile, "Ratio: %3.2f \n", ratio);
    fprintf(pFile, "Deltas: {");
    for(i = deltaValues.begin(); i != deltaValues.end(); i++)
        fprintf(pFile, " %3.2f ", (*i));
    fprintf(pFile, "} \n");
    fprintf(pFile, "Total runtime: %.2f \n", endTime-startTime);
    fprintf(pFile, "Time in BF expansion: %.2f \n", endTimeBF-startTimeBF);
    fprintf(pFile, "Time in DF expansion: %.2f \n", tree.dfTime);
    fprintf(pFile, "Time in proof: %.2f \n \n", tree.proofTime);
    fprintf(pFile, "Number of proofs found: %d \n", tree.successes);
    fprintf(pFile, "Number of nodes explored: %lld \n \n", tree.nodes[0].back().nodeCounter);
}


