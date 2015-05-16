#ifndef PARTITIONING_H
#define PARTITIONING_H
#include <list>
#include "omp.h"
#include <string>
//#define NDEBUG
#include "assert.h"


using namespace std;

class Partitioning
{
    public:
        /**** CONSTRUCTORS ****/
        Partitioning();
        Partitioning(double dPoint);
        Partitioning(Partitioning *parent);

        int nNumberOfClusters;
        double adClusters[30][2] = {{0}};
        double optSpace[20][2] = {{NULL}};
        double ambSpace[20][2] = {{NULL}};
        double overlappingSpace[20][2] = {{NULL}};
        int nNumberOfPoints;
        int optimalClusters;
        int nNumberOfOverlaps;
        int nNumberOfAmbs = 0;
        double lastPoint;
        string pointHash;
        string fullHash;
        bool forced = false;

        list<double> points;
        list<double> sortedPoints;
        list<double> forcedPoints;
        double difference;

        /**** POINT FUNCTIONALITY ****/
        int pointInRange(double dPoint);
        void makeRoomForCluster(double dPoint);
        bool doesPointExist(double dPoint);
        void addPointToCluster(double dPoint, bool force);
        void growClusterRight(double dPoint, bool force);
        void growClusterLeft(double dPoint, bool force);
        void openCluster(double dPoint, bool force);

        double calcRatio();
        int optimal();
        void setAmbSpace();
        list<double> getAmbPoints();

        /**** FORCING ****/
        list<double> getForcePoints();

        /**** TERMINAL IO ****/
        void listClusters();
        void listPoints();
        void listSortedPoints();

        /**** FILE IO ****/
        void insertNodeLabel(bool level, long long nodeId, int successNumber, int prefix);
        void insertEdgeLabel(long long, long long, int, int);

        /**** OTHER STUFF ****/
        string stringIt(long long);
        void updateHashes();


    protected:
    private:
        static int nId;
};

bool leq(double x, double y);
bool geq(double x, double y);
bool deq(double x, double y);


#endif // PARTITIONING_H
