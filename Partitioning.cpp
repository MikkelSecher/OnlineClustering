#include "Partitioning.h"
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <inttypes.h>
#include <sstream>
#include <string.h>


using namespace std;

#define precision 0.1
/*********************************/
/********** CONSTRUCTORS **********/
/*********************************/
Partitioning::Partitioning()
{
    nNumberOfClusters = 0;
    nNumberOfPoints = 0;
}

Partitioning::Partitioning(double point)
{
    // Constructor for the root of the tree. Only called once.
//    cout << "Creating root partitioning..." << endl;
    nNumberOfClusters = 1;
    nNumberOfPoints = 1;


    adClusters[0][0] = point;
    adClusters[0][1] = point;
    points.push_back(point);
    sortedPoints.push_back(point);
    sortedPoints.sort();
    lastPoint = point;
    std::ostringstream s;
    s << point;

    pointHash.append(s.str());
//    cout << "Hash before " <<pointHash << " " << point <<endl;
    updateHashes();
//    cout << "Hash after " <<pointHash << endl;


}

Partitioning::Partitioning(Partitioning *parent)
{
// General constructor for a partitioning.
// Takes the necessary data from the parent, and creates a new partitioning
// from it.
// TODO: Maybe this can be handled by copying the parent, instead of constructing a new object.
    nNumberOfClusters = parent->nNumberOfClusters;

//    for(int i = 0; i < nNumberOfClusters; i++)
//    {
//       adClusters[i][0] = parent->adClusters[i][0];
//       adClusters[i][1] = parent->adClusters[i][1];
//    }
//
    memcpy ( adClusters , parent->adClusters, sizeof( parent->adClusters) );
    nNumberOfPoints = parent->nNumberOfPoints;
    points = parent->points;
    sortedPoints = parent->sortedPoints;
    pointHash.append(parent->pointHash);
}



/*********************************/
/******* POINT FUNCTIONALITY ******/
/*********************************/
int Partitioning::pointInRange(double point){

    // Checks for the possibilities of handling the next point - grow, open new or put in existing?
    for(int i = 0; i < nNumberOfClusters; i++)
    {
        if(i < nNumberOfClusters){
            //if point is within reach of two clusters, open new cluster, grow the left and grow the right.
            //cannot happen when i==nNumberOfClusters
            if(leq(point, adClusters[i][0]+1)
               and (point > adClusters[i][1])
               and geq(point, adClusters[i+1][1]-1)
               and (point < adClusters[i+1][0])
               ){
                return 3;
            }
        }

        //if the point out of reach of the first cluster, open a new cluster.
        if( i == 0 and point < adClusters[i][0] &&
           point < (adClusters[i][1]-1) &&
           !deq(point, (adClusters[i][1]-1) )
           ){
            return 0;
        }

        //If point is inside an existing cluster, there is no choice. Must be added to that cluster.
        if(point < adClusters[i][1] && point > adClusters[i][0])
            return 1;

        //If point is within reach (to the left) of an existing cluster, open new cluster, and grow cluster to the left
        if(leq(point, adClusters[i][0]) && geq(point, adClusters[i][1]-1)){
            if(i == 0){
                return -2;
            }
            if (point > (adClusters[i-1][1])){
                return -2;
            }
        }

        //If point is within reach (to the right) of a cluster, open a new, and grow the existing
        if(leq(point, adClusters[i][0]+1) && geq(point, adClusters[i][1])){
            if(i < nNumberOfClusters-1){
                if(point > adClusters[i+1][0]){
                    continue;
                }
            }
            return 2;
        }
    }
    return 0;                       //the point must get its own cluster, no choice
}

void Partitioning::makeRoomForCluster(double point){
// Called when a cluster needs to be opened at another spot than at the end of
// the clusterlist. This finds the place it should be and moves everything else
// to the left.
    adClusters[nNumberOfClusters][0] = point;              //create the new cluster in the end back of the array
    adClusters[nNumberOfClusters][1] = point;

    for(int i = nNumberOfClusters; i > 0; i--){
        if(point < adClusters[i-1][0]){
            if(point > adClusters[i-1][0] and point < adClusters[i-1][1]){
                cout << " --------------------------AA------------------------- " << endl;
            }
            swap(adClusters[i][0], adClusters[i-1][0]);     //Shift the cluster back to its right position
            swap(adClusters[i][1], adClusters[i-1][1]);     //so that the clusters are sorted
        }
        else{
            return;
        }
    }

}

bool Partitioning::doesPointExist(double point){
// Checks if a point already exists
// Returns FALSE if it already exists
    list<double>::iterator i;
    for( i = sortedPoints.begin(); i != sortedPoints.end(); i++ ) {
        if(deq(*i, point)) {
            return false;
        }
    }
    return true;
}

void Partitioning::addPointToCluster(double point, bool force){
// Adds a point to a cluster, since the cluster doesn't change,
// it is done by just adding the point to the lists.
    difference = points.back()-point;
    points.push_back(point);
    sortedPoints.push_back(point);
    sortedPoints.sort();
}

void Partitioning::growClusterRight(double point, bool force){
// Grow the cluster to the right
    if(!doesPointExist(point)){
        cout << "Point exists... right" << endl;
        listPoints();
        cout << "Point: " << point << endl;
        listClusters();
    }

    for(int i = 0; i < nNumberOfClusters; i++){
        if(geq(point, adClusters[i][1]) and leq(point, (adClusters[i][0]+1))){
            if(i < nNumberOfClusters-1 and point > adClusters[i+1][0]){
                continue;
            }

            adClusters[i][1] = point;
            nNumberOfPoints++;

            if(!force){
                difference = points.back()-point;
                points.push_back(point);
            } else {
                forcedPoints.push_back(point);
            }

            sortedPoints.push_back(point);
            sortedPoints.sort();
            return;
        }
    }
    cout << "Point: " << point << endl;
    listClusters();
    cout << "DIDN'T ADD THE POINT RIGHT!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
}

void Partitioning::growClusterLeft(double point, bool force){
// Grow the cluster to the left
    if(!doesPointExist(point)){
        cout << "Point exists... left" << endl;
        listPoints();
        cout << "Point: " << point << endl;
        listClusters();
    }

    for(int i = 0; i < nNumberOfClusters; i++){
        if(geq(point, adClusters[i][1]-1) and leq(point, adClusters[i][1])){
            adClusters[i][0] = point;
            nNumberOfPoints++;
            if(!force){
                difference = points.back()-point;
                points.push_back(point);
            } else {
                forcedPoints.push_back(point);
            }
            sortedPoints.push_back(point);
            sortedPoints.sort();
            return;
        }
    }
    cout << "Point: " << point << endl;
    listClusters();
    cout << "DIDN'T ADD THE POINT LEFT ------------------------------ " << endl;
}

void Partitioning::openCluster(double point, bool force){
// Opens a new cluster
    if(nNumberOfClusters == 0){      //if it is the first cluster, just open it.
        adClusters[0][0] = point;
        adClusters[0][1] = point;
        nNumberOfPoints++;
        nNumberOfClusters++;

        if(!force){
            difference = points.back()-point;
            points.push_back(point);
        } else {
            forcedPoints.push_back(point);
        }

        sortedPoints.push_back(point);
        sortedPoints.sort();
    }
    else{
        makeRoomForCluster(point); //if not the first cluster, make room for it.
        nNumberOfPoints++;
        nNumberOfClusters++;
        if(!force){
            difference = points.back()-point;
            points.push_back(point);
        } else {
            forcedPoints.push_back(point);
        }
        sortedPoints.push_back(point);
        sortedPoints.sort();
    }
}

double Partitioning::calcRatio(){
// Calculate and return the ratio of a partitioning.
// Returns the ratio.
    double dRatio;
    dRatio = (double)nNumberOfClusters/optimal();
    return dRatio;
}

int Partitioning::optimal(){
    int clusterCount = 1;
    double currentStart = sortedPoints.front();
    optSpace[0][0] = currentStart;
    optSpace[0][1] = currentStart;

    list<double>::iterator i;

    for(i = sortedPoints.begin(); i != sortedPoints.end(); i++){
        if(leq(*i, currentStart+1)){
            optSpace[clusterCount-1][1] = (*i);
            continue;
        } else {
            currentStart = (*i);
            clusterCount++;
            optSpace[clusterCount-1][0] = currentStart;
            optSpace[clusterCount-1][1] = currentStart;
        }
    }
//    cout << "Optimal cluster:        ";
//    for(int j = 0; j < clusterCount; j++)
//    {
//        cout << " [" << optSpace[j][0] << " , " << optSpace[j][1] << "] ";
//    }
//    cout << endl;
//    listClusters();
//    cout << "Optimal number of clusters is: " << clusterCount << endl;
//
//    cout << "" << endl;
    optimalClusters = clusterCount;
    return clusterCount;
}

void Partitioning::setAmbSpace(){
    nNumberOfAmbs = 0;

    //check if there is ambiguous space in the beginning
    if(!geq((adClusters[0][1]-adClusters[0][0]), 1)){
        ambSpace[0][0] = (adClusters[0][1]-1);
        ambSpace[0][1] = min((adClusters[0][0]+1), adClusters[1][0]);
        nNumberOfAmbs++;
    }

    //find ambiguous space in the middle of the partitioning
    for(int i = 1; i < nNumberOfClusters; i++){
        if(!geq((adClusters[i][1]-adClusters[i][0]), 1)){
            if(!leq((adClusters[i+1][0]-adClusters[i-1][1]),1)){
                ambSpace[nNumberOfAmbs][0] = adClusters[i-1][1];
                ambSpace[nNumberOfAmbs][1] = adClusters[i+1][0];
                nNumberOfAmbs++;
            }
        }
    }

    //check if there is ambiguous space in the end
    if(!geq((adClusters[nNumberOfClusters-1][1]-adClusters[nNumberOfClusters-1][0]),1)){
        double first = adClusters[nNumberOfClusters-2][0]+1;
        double second = adClusters[nNumberOfClusters-1][1]-1;

//        cout << "First and second points are: " << first << " and " << second << endl;

        ambSpace[nNumberOfAmbs][0] = max((adClusters[nNumberOfClusters-2][0]+1), (adClusters[nNumberOfClusters-1][1]-1));
        ambSpace[nNumberOfAmbs][1] = adClusters[nNumberOfClusters-1][0]+1;
        nNumberOfAmbs++;

//        cout << "So now the last abiguous space is from " << ambSpace[nNumberOfAmbs-1][0] << " to " << ambSpace[nNumberOfAmbs-1][1] << endl;

        if(geq((ambSpace[nNumberOfAmbs-1][1]-ambSpace[nNumberOfAmbs-1][0]),1)){
            ambSpace[nNumberOfAmbs][1] = adClusters[nNumberOfClusters-1][1];
            ambSpace[nNumberOfAmbs-1][1] = adClusters[nNumberOfClusters-1][0];
            ambSpace[nNumberOfAmbs][0] = adClusters[nNumberOfClusters-1][1];
        }
        nNumberOfAmbs++;
    }


//    cout << "Ambiguous space: ";
//
//    for(int j = 0; j < nNumberOfAmbs; j++)
//    {
//        cout << " [" << ambSpace[j][0] << " , " << ambSpace[j][1] << "] ";
//    }
//    cout << "" << endl;
//    cout << "" << endl;

}

list<double> Partitioning::getAmbPoints(){
    //find overlapping spaces and pick points within these spaces
    list<double> ambPoints;

    int ambPos = 0;
    int optPos = 0;
    double ambOffset;
    nNumberOfOverlaps = 0;
    while(ambPos != nNumberOfAmbs and optPos != nNumberOfClusters and nNumberOfAmbs != 0){
//        cout << endl;
//        cout << "opt: " << optSpace[optPos][0] << " , " << optSpace[optPos][1] << endl;
//        cout << "amb: " <<ambSpace[ambPos][0] << " , " << ambSpace[ambPos][1] << endl;
        if(!leq(optSpace[optPos][0], ambSpace[ambPos][1])) { //if optCluster starts after the end of ambCluster inc ambPos

            ambPos++;
            continue;
        }

        if(!leq(ambSpace[ambPos][0], optSpace[optPos][1])){ //if ambCluster starts after the end of optCluster inc optPos

            optPos++;
            continue;
        }
        if(deq(ambSpace[ambPos][0], optSpace[optPos][1])){
            optPos++;
            continue;
        }

        overlappingSpace[nNumberOfOverlaps][0] = max(ambSpace[ambPos][0], optSpace[optPos][0]);
        overlappingSpace[nNumberOfOverlaps][1] = min(ambSpace[ambPos][1], optSpace[optPos][1]);
        nNumberOfOverlaps++;
        ambPos++;
        optPos++;
    }

//    cout << "Overlapping space: ";
//    for(int j = 0; j < nNumberOfOverlaps; j++)
//    {
//        cout << " [" << overlappingSpace[j][0] << " , " << overlappingSpace[j][1] << "] ";
//    }
//    cout << "" << endl;
//    cout << "" << endl;

    for(int j = 0; j < nNumberOfOverlaps; j++){
        ambOffset = (overlappingSpace[j][1]-overlappingSpace[j][0])/2;
        if(doesPointExist(overlappingSpace[j][0]+ambOffset)){
            ambPoints.push_back(overlappingSpace[j][0]+ambOffset);
//            cout << "Added: "<< overlappingSpace[j][0]+ambOffset << endl;
        }
    }
    return ambPoints;
}

/*********************************/
/************* FORCING ************/
/*********************************/

list<double> Partitioning::getForcePoints(){
    double freeSpace[50][2] = {{0}};

    double start;
    double finish;

    list<double> forcePoints;

    list<double>::iterator it;


    for(int i = 1; i < nNumberOfClusters-1; i++)
    {

        start = (adClusters[i][0])+1;

        finish = adClusters[i+1][1]-1;

        if((start+precision) < (finish-precision))
        {
            freeSpace[i][0] = start;
            freeSpace[i][1] = finish;
        }
    }

    if(geq((adClusters[0][1] - adClusters[0][0]), 0) and !deq((adClusters[0][1] - adClusters[0][0]),0)){
        forcePoints.push_back(((adClusters[0][1])-1-precision));
    }
    if(!deq(adClusters[nNumberOfClusters-2][1]-adClusters[nNumberOfClusters-2][0], 0) and
       !deq(adClusters[nNumberOfClusters-1][1]-adClusters[nNumberOfClusters-1][0], 0)
       ){
        forcePoints.push_back(((adClusters[nNumberOfClusters-1][0])+1+precision));
    }

    if(!deq((adClusters[0][1] - adClusters[0][0]), 0) and !deq((adClusters[1][1] - adClusters[1][0]),0)){
        forcePoints.push_back(((adClusters[0][1])-1-precision));
    }

    if(geq(adClusters[nNumberOfClusters-1][1]-adClusters[nNumberOfClusters-1][0], 0) and
       !deq(adClusters[nNumberOfClusters-1][1]-adClusters[nNumberOfClusters-1][0], 0)
       ) {
        forcePoints.push_back(((adClusters[nNumberOfClusters-1][0])+1+precision));
    }

    forcePoints.unique();

    int intervals = nNumberOfClusters;

    for(int i = 0; i < intervals; i++ ){
        double offset = (freeSpace[i][1]-freeSpace[i][0])/2;
        double forcepoint = freeSpace[i][0]+ offset;

        if(!deq(0, forcepoint)){ //Why can't the point be 0?

            forcePoints.push_back(forcepoint);
        }
    }

//    cout << "Found forcepoints: ";
//    for(it = forcePoints.begin(); it != forcePoints.end(); it++)
//    {
//        cout << (*it) << " , ";
//    }
//    cout << endl;
    return forcePoints;
}

/*********************************/
/********** TERMINAL IO ***********/
/*********************************/
void Partitioning::listClusters(){
// Print out the clusters
    cout << "Number of clusters: " << nNumberOfClusters << " - ";
    for(int i = 0; i < nNumberOfClusters; i++){
        cout << "[" << adClusters[i][0] << " , " << adClusters[i][1] << "]  ";
    }
    cout << endl;
}

void Partitioning::listPoints(){
// Prints the points, in order of addition
    list<double>::iterator i;
    cout << "Points in the partitioning:" << endl;
    for(i = points.begin(); i != points.end(); i++){
        cout << *i << "   ";
    }
    cout << endl;

}

void Partitioning::listSortedPoints(){
// Prints the points, in sorted order
    list<double>::iterator i;
    cout << "Points in the partitioning (sorted):" << endl;
    for(i = sortedPoints.begin(); i != sortedPoints.end(); i++)
    {
        cout << *i << "   ";
    }
    cout << endl;
}



/*********************************/
/************** FILE IO ***********/
/*********************************/
void Partitioning::insertNodeLabel(bool level, long long nodeID, int succesNumber, int prefix){
    FILE *pFile;
    char filename[20];
    string sNodeID = stringIt(nodeID);
    sprintf(filename, "res\\%d_%d_solution_%d .gml", prefix, omp_get_thread_num(), succesNumber);
    list<double>::iterator i;
    pFile = fopen (filename,"a");
    fprintf(pFile, "node \n [ \n id n%s \n", sNodeID.c_str());
    fprintf(pFile, "label \" ");

    if (level){
       fprintf(pFile, " DF Level : ");
       for(i = forcedPoints.begin(); i != forcedPoints.end(); i++){
           fprintf(pFile, " %2.2f ", (*i));
       }
       fprintf(pFile, " : ");
    }



    for(int i = 0; i < nNumberOfClusters; i++){
        fprintf(pFile, "[ %2.2f , %2.2f ] ", adClusters[i][0], adClusters[i][1] );
    }
    fprintf(pFile, "Node ID %s: ", sNodeID.c_str());
    fprintf(pFile, " Ratio: %2.2f ", calcRatio());
    if(forced){
        fprintf(pFile, " (FORCED) ");
    }
    fprintf(pFile, "\" \n");
    fprintf(pFile, "] \n");

    fclose(pFile);
}

void Partitioning::insertEdgeLabel(long long parentID, long long nodeID, int succesNumber, int prefix){
    FILE *pFile;
    char filename[30];

    sprintf(filename, "res\\%d_%d_solution_%d .gml", prefix, omp_get_thread_num(), succesNumber);

    pFile = fopen (filename,"a");
    string sParentID = stringIt(parentID);
    string sNodeID = stringIt(nodeID);

    fprintf(pFile, " edge \n [ \n source n%s \n target n%s \n", sParentID.c_str(), sNodeID.c_str());
    list<double>::iterator i;
    fprintf(pFile, "label \"");

    for(i = points.begin(); i != points.end(); i++){
        fprintf(pFile," %2.2f ", *i);
    }

    if(forced){
        fprintf(pFile, " | ");
        for(i = forcedPoints.begin(); i != forcedPoints.end(); i++){
            fprintf(pFile," %2.2f ", *i);
        }
    }

    fprintf(pFile, " \" \n");
    fprintf(pFile, " \] \n"); //, (*i)->content.sortedPoints.back());
    fclose(pFile);
}



void Partitioning::listPointsToFile(int t){
    FILE *pFile;
    char filename[30];

    sprintf(filename, "%d_subProofSequences.txt", t);
    pFile = fopen (filename,"a");

    list<double>::iterator i;

    for(i = points.begin(); i != points.end(); i++){
        fprintf(pFile, "%2.2f ", *i);
    }

    fclose(pFile);
}

void Partitioning::listClustersToFile(int t){
    FILE *pFile;
    char filename[30];

    sprintf(filename, "%d_subProofSequences.txt", t);
    pFile = fopen (filename,"a");
    for(int i = 0; i < nNumberOfClusters; i++){
        fprintf(pFile, "[%2.2f , %2.2f] ", adClusters[i][0], adClusters[i][1]);
    }
    fclose(pFile);
}

void Partitioning::listClustersFile(){
// Print out the clusters
    FILE * pFile;


    cout << ".";
    pFile = fopen ("myfile.tex","a");

    fprintf(pFile, "\\begin{tikzpicture}[scale=1.5] \n");
    fprintf(pFile, "\\\draw [dotted] (-1.2,0) -- (6,0);\n");
    fprintf(pFile, "\\foreach \\x in  {0,1,2,3,4,5}\n");
    fprintf(pFile, "\\draw[shift={(\\x,0)},color=black] (0pt,0pt) -- (0pt,-3pt) node[below] {$\\x$}; \n");


    for(int i = 0; i < nNumberOfClusters; i++){
        fprintf (pFile, "\\draw[red, ultra thick] (%2.2f,0) -- (%2.2f,0); \n", adClusters[i][0] , adClusters[i][1]);
    }

    list<double>::iterator i;
    for(i = points.begin(); i != points.end(); i++){
        fprintf(pFile, "\\draw [fill] (%f,0) circle [radius=0.05]; \n", *i);
    }

    fprintf(pFile, "\\end{tikzpicture}");
    fprintf(pFile, "\\");
    fprintf(pFile, "\\");
    fprintf(pFile, "\n");
    fclose (pFile);
}


/*********************************/
/********** OTHER STUFF ***********/
/*********************************/

std::string Partitioning::stringIt(long long number){
    std::string result;
    ostringstream convert;
    convert << number;

    result = convert.str();

    return result;
}

void Partitioning::updateHashes(){
    pointHash.clear();
    std::ostringstream s;

    list<double>::iterator i;
    //cout << "Points in the partitioning:" << endl;
    for(i = points.begin(); i != points.end(); i++){
       s << *i;
       s << ' ';
    }
    pointHash.append(s.str());

    fullHash.clear();
    s << ' ';
    for(int i = 0; i < nNumberOfClusters; i++){
        s << adClusters[i][0];
        s << ' ';
        s << adClusters[i][1];
        s << ' ';
    }

    fullHash.append(s.str());
    //cout << "Hashes " << pointHash << " " << fullHash << endl;
}

/*********************************/
/******** GLOBAL FUNCTIONS ********/
/*********************************/

bool leq(double x, double y){ //IS x LESS THAN OR EQUAL TO y?
    if(fabs(y-x) < 0.001)
        return true;

    if (x < y)
        return true;

    return false;
};

bool geq(double x, double y){ //IS x GREATER THAN OR EQUAL TO y?

    if(fabs(y-x) < 0.001)
        return true;

    if (x > y)
        return true;

    return false;
};

bool deq(double x, double y){ //ARE DOUBLES x AND y EQUAL?

    if(fabs(y-x) < 0.001)
        return true;
    return false;
};

