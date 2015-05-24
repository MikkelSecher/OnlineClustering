#include "main.h"

using namespace std;
using std::atoi;

int main(int argc, char* argv[])
{
    ///Comment in for tests

    OCTester test1 = OCTester();
//    exit(1);

    double startTime = omp_get_wtime();
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

    startTimeBF = omp_get_wtime();
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
    endTime = omp_get_wtime();

    FILE * pFile;
    char filename[30];
    sprintf(filename, "res_%d_Summary.txt", prefix);
    pFile = fopen (filename,"w");
    fprintf(pFile, "Summary for run with id: %d \n \n", prefix);
    fprintf(pFile, "#Threads: %d \n", tree.numberOfThreads);
    fprintf(pFile, "#Levels of BF: %d \n", levelsOfBF);
    fprintf(pFile, "#Max level for DF: %d \n", levelsOfDF);
    fprintf(pFile, "Ratio: %3.2f \n", ratio);
    fprintf(pFile, "Deltas: {");
    list<double>::iterator i;
    for(i = deltaValues.begin(); i != deltaValues.end(); i++)
        fprintf(pFile, " %3.2f ", (*i));
    fprintf(pFile, "} \n");
    fprintf(pFile, "Total runtime: %.2f \n", endTime-startTime);
    fprintf(pFile, "Time in BF expansion: %.2f \n", endTimeBF-startTimeBF);
    fprintf(pFile, "Time in DF expansion: %.2f \n", tree.dfTime);
    fprintf(pFile, "Time in proof: %.2f \n \n", tree.proofTime);
    fprintf(pFile, "Number of proofs found: %d \n", tree.successes);
}


