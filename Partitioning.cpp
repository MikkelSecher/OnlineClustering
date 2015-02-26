#include "Partitioning.h"
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <inttypes.h>
#include <sstream>



using namespace std;

#define precision 0.1
/*********************************
********** CONSTRUCTORS **********
*********************************/
Partitioning::Partitioning()
{
    nNumberOfClusters = 0;
    nNumberOfPoints = 0;
}

Partitioning::Partitioning(double dPoint)
{
    // Constructor for the root of the tree. Only called once.
    cout << "Creating root partitioning..." << endl;
    nNumberOfClusters = 1;
    nNumberOfPoints = 1;


    adClusters[0][0] = dPoint;
    adClusters[0][1] = dPoint;
    points.push_back(dPoint);
    sortedPoints.push_back(dPoint);
    sortedPoints.sort();
    lastPoint = dPoint;
}

Partitioning::Partitioning(Partitioning *parent)
{
// General constructor for a partitioning.
// Takes the necessary data from the parent, and creates a new partitioning
// from it.
// TODO: Maybe this can be handled by copying the parent, instead of constructing a new object.
    nNumberOfClusters = parent->nNumberOfClusters;

    for(int i = 0; i < nNumberOfClusters; i++)
    {
       adClusters[i][0] = parent->adClusters[i][0];
       adClusters[i][1] = parent->adClusters[i][1];
    }
    nNumberOfPoints = parent->nNumberOfPoints;
    points = parent->points;
    sortedPoints = parent->sortedPoints;
}


/*********************************
******* POINT FUNCTIONALITY ******
*********************************/
int Partitioning::pointInRange(double dPoint)
{
    // Checks for the possibilities of handling the next point - grow, open new or put in existing?
    for(int i = 0; i < nNumberOfClusters; i++)
    {
        if(i < nNumberOfClusters) //
        {
            //if point is within reach of two clusters, open new cluster, grow the left and grow the right.
            //cannot happen when i==nNumberOfClusters
            if(leq(dPoint, adClusters[i][0]+1)
               and (dPoint > adClusters[i][1])
               and geq(dPoint, adClusters[i+1][1]-1)
               and (dPoint < adClusters[i+1][0]) )
            {
                return 3;
            }
        }

        //if the point out of reach of the first cluster, open a new cluster.
        if(i == 0 and dPoint < adClusters[i][0] and dPoint < (adClusters[i][1]-1) and !deq(dPoint, (adClusters[i][1]-1)))
        {
            return 0;
        }

        //If point is inside an existing cluster, there is no choice. Must be added to that cluster.
        if(dPoint < adClusters[i][1] and dPoint > adClusters[i][0])
            return 1;

        //If point is within reach (to the left) of an existing cluster, open new cluster, and grow cluster to the left
        if(leq(dPoint, adClusters[i][0]) and geq(dPoint, adClusters[i][1]-1))
        {
            if(i == 0)
                return -2;

            if (dPoint > (adClusters[i-1][1]))
            {
                return -2;
            }
        }

        //If point is within reach (to the right) of a cluster, open a new, and grow the existing
        if(leq(dPoint, adClusters[i][0]+1) and geq(dPoint, adClusters[i][1]))
        {
            if(i < nNumberOfClusters-1)
            {
                if(dPoint > adClusters[i+1][0])
                {
                    continue;
                }
            }
            return 2;
        }
    }
    return 0;                       //the point must get its own cluster, no choice
}

void Partitioning::makeRoomForCluster(double dPoint)
{
// Called when a cluster needs to be opened at another spot than at the end of
// the clusterlist. This finds the place it should be and moves everything else
// to the left.
    adClusters[nNumberOfClusters][0] = dPoint;              //create the new cluster in the end back of the array
    adClusters[nNumberOfClusters][1] = dPoint;              //

    for(int i = nNumberOfClusters; i > 0; i--)
    {
        if(dPoint < adClusters[i-1][0])
        {
            if(dPoint > adClusters[i-1][0] and dPoint < adClusters[i-1][1])
                cout << " --------------------------AA------------------------- " << endl;

            swap(adClusters[i][0], adClusters[i-1][0]);     //Shift the cluster back to its right position
            swap(adClusters[i][1], adClusters[i-1][1]);     //so that the clusters are sorted
        }
        else
        {
            return;
        }
    }

}

bool Partitioning::doesPointExist(double dPoint)
{
// Checks if a point already exists
// Returns FALSE if it already exists
    list<double>::iterator i;
    for(i = sortedPoints.begin(); i != sortedPoints.end(); i++)
    {
        //cout << "comparing " << dPoint << " to " << *i << endl;
        if(deq(*i, dPoint))
        {
            //cout << "already exists " << endl;
            return false;
        }
    }

    return true;
}

void Partitioning::addPointToCluster(double dPoint)
{
// Adds a point to a cluster, since the cluster doesn't change,
// it is done by just adding the point to the lists.
    difference = points.back()-dPoint;
    points.push_back(dPoint);
    sortedPoints.push_back(dPoint);
    sortedPoints.sort();
}

void Partitioning::growClusterRight(double dPoint)
{
// Grow the cluster to the right
    if(!doesPointExist(dPoint))
    {
        cout << "Point exists..." << endl;
        listPoints();
        cout << "Point: " << dPoint << endl;
        listClusters();

    }

    for(int i = 0; i < nNumberOfClusters; i++)
    {
        double epsilon = 0.01;

        if(dPoint > adClusters[i][1] and leq(dPoint, adClusters[i][0]+1))
        {
            if(i < nNumberOfClusters-1 and dPoint > adClusters[i+1][0])
            {
                continue;
            }
            adClusters[i][1] = dPoint;
            nNumberOfPoints++;
            difference = points.back()-dPoint;
            points.push_back(dPoint);
            sortedPoints.push_back(dPoint);
            sortedPoints.sort();
            return;
        }
    }
    cout << "Point: " << dPoint << endl;
    listClusters();
    cout << "DIDN'T ADD THE POINT RIGHT!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
}

void Partitioning::growClusterLeft(double dPoint)
{
// Grow the cluster to the left
    for(int i = 0; i < nNumberOfClusters; i++)
    {
        double epsilon = 0.01;

        if(geq(dPoint, adClusters[i][1]-1) and leq(dPoint, adClusters[i][1]))
        {
            adClusters[i][0] = dPoint;
            nNumberOfPoints++;
            difference = points.back()-dPoint;
            points.push_back(dPoint);
            sortedPoints.push_back(dPoint);
            sortedPoints.sort();
            return;


        }
    }
    cout << "Point: " << dPoint << endl;
    listClusters();
    cout << "DIDN'T ADD THE POINT LEFT ------------------------------ " << endl;
}

void Partitioning::openCluster(double dPoint)
{
// Opens a new cluster
    if(nNumberOfClusters == 0)      //if it is the first cluster, just open it.
    {
        adClusters[0][0] = dPoint;
        adClusters[0][1] = dPoint;
        nNumberOfPoints++;
        nNumberOfClusters++;
        difference = points.back()-dPoint;
        points.push_back(dPoint);
        sortedPoints.push_back(dPoint);
        sortedPoints.sort();
    }
    else
    {
        makeRoomForCluster(dPoint); //if not the first cluster, make room for it.
        nNumberOfPoints++;
        nNumberOfClusters++;
        difference = points.back()-dPoint;
        points.push_back(dPoint);
        sortedPoints.push_back(dPoint);
        sortedPoints.sort();
    }
}

void Partitioning::openClusterForce(double dPoint)
{
// Opens a new cluster
    if(nNumberOfClusters == 0)      //if it is the first cluster, just open it.
    {
        adClusters[0][0] = dPoint;
        adClusters[0][1] = dPoint;
        nNumberOfPoints++;
        nNumberOfClusters++;
        //difference = points.back()-dPoint;
        forcedPoints.push_back(dPoint);
        sortedPoints.push_back(dPoint);
        sortedPoints.sort();
    }
    else
    {
        makeRoomForCluster(dPoint); //if not the first cluster, make room for it.
        nNumberOfPoints++;
        nNumberOfClusters++;
        //difference = points.back()-dPoint;
        forcedPoints.push_back(dPoint);
        sortedPoints.push_back(dPoint);
        sortedPoints.sort();
    }
}

double Partitioning::calcRatio()
{
// Calculate and return the ratio of a partitioning.
// Returns the ratio.
    double dRatio;
    dRatio = (double)nNumberOfClusters/optimal();
    return dRatio;
}

int Partitioning::optimal()
{
    int clusterCount = 1;
    double currentStart = sortedPoints.front();

    list<double>::iterator i;

    for(i = sortedPoints.begin(); i != sortedPoints.end(); i++)
    {
        if(leq(*i, currentStart+1))
        {
            continue;
        }
        else
        {
            currentStart = (*i);
            clusterCount++;
        }
    }

    return clusterCount;
}


/*********************************
************* FORCING ************
*********************************/
bool Partitioning::force(double dRatio)
{
// Tries to force a partitioning to fail, by adding "forcepoints" that makes the ratio worse.
// Can probably be improved quite a lot.
// Returns FALSE if a partitioning is broken by force.
    //find unreachable intervals
    //TODO: Maybe move to its own function.

    endForce(dRatio);
    if(geq(calcRatio(), dRatio))
    {
        return false;
    }
    startForce(dRatio);
    if(geq(calcRatio(), dRatio))
    {
        return false;
    }

    simpleForce(dRatio);
    //test for new ratio
    if(geq(calcRatio(), dRatio))
    {
        return false;
    }
    return true;
}

void Partitioning::simpleForce(double dRatio)
{
// Regular forcing.
// Finds free spaces in the partitioning that are unreachable by any clusters, and
// adds points there.
    double freeSpace[30][2] = {{0}};

    double start;
    double finish;
    int pointsAdded = 0;

    for(int i = 0; i < nNumberOfClusters-1; i++)
    {

        start = (adClusters[i][0])+1;

        finish = adClusters[i+1][1]-1;

        if((start+precision) < (finish-precision))
        {
            freeSpace[i][0] = start;
            freeSpace[i][1] = finish;
        }
    }

    //put in points
    int intervals = nNumberOfClusters;


    for(int i = 0; i < intervals; i++ )
    {
        double offset = (freeSpace[i][1]-freeSpace[i][0])/2;
        double forcepoint = freeSpace[i][0]+ offset;

        if(!deq(0, forcepoint))
        {
            openClusterForce(forcepoint);
            if(calcRatio() >= dRatio)
                return;
            pointsAdded++;
        }
    }
}

void Partitioning::startForce(double dRatio)
{
    openClusterForce(adClusters[0][1]-1-precision);
}

void Partitioning::endForce(double dRatio)
{
    // Adds a point in the end of the partitioning just out of range of the existing clusters
    //but only if the cluster is full... TODO: Change this!!

    if(deq(adClusters[nNumberOfClusters-1][1] - adClusters[nNumberOfClusters-1][0], 1))
        openClusterForce(adClusters[nNumberOfClusters-1][1]+(precision/2));
}



/*********************************
********** TERMINAL IO ***********
*********************************/
void Partitioning::listClusters()
{
// Print out the clusters
    cout << "Number of clusters: " << nNumberOfClusters << " - ";
    for(int i = 0; i < nNumberOfClusters; i++)
    {
        cout << "[" << adClusters[i][0] << " , " << adClusters[i][1] << "]  ";
    }
    cout << endl;
}


void Partitioning::listPoints()
{
// Prints the points, in order of addition
    list<double>::iterator i;
    cout << "Points in the partitioning:" << endl;
    for(i = points.begin(); i != points.end(); i++)
    {
        cout << *i << "   ";
    }
    cout << endl;

}

void Partitioning::listSortedPoints()
{
// Prints the points, in sorted order
    list<double>::iterator i;
    cout << "Points in the partitioning (sorted):" << endl;
    for(i = sortedPoints.begin(); i != sortedPoints.end(); i++)
    {
        cout << *i << "   ";
    }
    cout << endl;
}

/*********************************
************** FILE IO ***********
*********************************/
void Partitioning::insertNodeLabel(bool forced, long long nodeID, int succesNumber, int prefix)
{
    FILE *pFile;
    char filename[20];
    string sNodeID = stringIt(nodeID);
    sprintf(filename, "res\\%d_%d_solution_%d .gml", prefix, omp_get_thread_num(), succesNumber);
    list<double>::iterator i;
    pFile = fopen (filename,"a");
    fprintf(pFile, "node \n [ \n id n%s \n", sNodeID.c_str());
    fprintf(pFile, "label \" ");

    if (forced)
    {
       fprintf(pFile, " Forced : ");
       for(i = forcedPoints.begin(); i != forcedPoints.end(); i++)
       {
           fprintf(pFile, " %2.2f ", (*i));
       }
       fprintf(pFile, " : ");

    }
    for(int i = 0; i < nNumberOfClusters; i++)
    {
        fprintf(pFile, "[ %2.2f , %2.2f ] ", adClusters[i][0], adClusters[i][1] );
    }
    fprintf(pFile, "Node ID %s: ", sNodeID.c_str());
    fprintf(pFile, " Ratio: %2.2f ", calcRatio());
    fprintf(pFile, "\" \n");
    fprintf(pFile, "] \n");
    fclose(pFile);
}

void Partitioning::insertEdgeLabel(long long parentID, long long nodeID, int succesNumber, int prefix)
{
    FILE *pFile;
    char filename[30];

    sprintf(filename, "res\\%d_%d_solution_%d .gml", prefix, omp_get_thread_num(), succesNumber);

    pFile = fopen (filename,"a");
    string sParentID = stringIt(parentID);
    string sNodeID = stringIt(nodeID);

    fprintf(pFile, " edge \n [ \n source n%s \n target n%s \n", sParentID.c_str(), sNodeID.c_str());
    list<double>::iterator i;
    fprintf(pFile, "label \"");
    for(i = points.begin(); i != points.end(); i++)
    {
        fprintf(pFile," %2.2f ", *i);
    }
    fprintf(pFile, " \" \n");
    fprintf(pFile, " \] \n"); //, (*i)->content.sortedPoints.back());
    fclose(pFile);
}

void Partitioning::listPointsToFile(int t)
{
    FILE *pFile;
    char filename[30];

    sprintf(filename, "%d_subProofSequences.txt", t);
    pFile = fopen (filename,"a");

    list<double>::iterator i;

    for(i = points.begin(); i != points.end(); i++)
    {
        fprintf(pFile, "%2.2f ", *i);
    }

    fclose(pFile);
}
void Partitioning::listClustersToFile(int t)
{
    FILE *pFile;
    char filename[30];

    sprintf(filename, "%d_subProofSequences.txt", t);
    pFile = fopen (filename,"a");
    for(int i = 0; i < nNumberOfClusters; i++)
    {
        fprintf(pFile, "[%2.2f , %2.2f] ", adClusters[i][0], adClusters[i][1]);
    }
    fclose(pFile);
}

void Partitioning::listClustersFile()
{
// Print out the clusters
    FILE * pFile;


    cout << ".";
    pFile = fopen ("myfile.tex","a");

    fprintf(pFile, "\\begin{tikzpicture}[scale=1.5] \n");
    fprintf(pFile, "\\\draw [dotted] (-1.2,0) -- (6,0);\n");
    fprintf(pFile, "\\foreach \\x in  {0,1,2,3,4,5}\n");
    fprintf(pFile, "\\draw[shift={(\\x,0)},color=black] (0pt,0pt) -- (0pt,-3pt) node[below] {$\\x$}; \n");


    for(int i = 0; i < nNumberOfClusters; i++)
    {
        fprintf (pFile, "\\draw[red, ultra thick] (%2.2f,0) -- (%2.2f,0); \n", adClusters[i][0] , adClusters[i][1]);
    }

    list<double>::iterator i;
    for(i = points.begin(); i != points.end(); i++)
    {
        fprintf(pFile, "\\draw [fill] (%f,0) circle [radius=0.05]; \n", *i);
    }

    fprintf(pFile, "\\end{tikzpicture}");
    fprintf(pFile, "\\");
    fprintf(pFile, "\\");
    fprintf(pFile, "\n");
    fclose (pFile);
}


/*********************************
********** OTHER STUFF ***********
*********************************/

std::string Partitioning::stringIt(long long number)
{
    std::string result;
    ostringstream convert;
    convert << number;

    result = convert.str();

    return result;
}


bool leq(double x, double y) //IS x LESS THAN OR EQUAL TO y?
{
    if(fabs(y-x) < 0.001)
        return true;

    if (x < y)
        return true;

    return false;
};

bool geq(double x, double y) //IS x GREATER THAN OR EQUAL TO y?
{
    if(fabs(y-x) < 0.001)
        return true;

    if (x > y)
        return true;

    return false;
};

bool deq(double x, double y) //ARE DOUBLES x AND y EQUAL?
{
    if(fabs(y-x) < 0.001)
        return true;
    return false;
};

