#ifndef TREENODE_H
#define TREENODE_H

#include "Partitioning.h"

#include <list>
#include "omp.h"
#include <stdlib.h>
#include <iostream>
using namespace std;


class TreeNode
{
    public:
        int depth;
        Partitioning content;
        bool live = true;
        bool visited = false;
        bool rootNode = false;
        list<TreeNode*> children;
        TreeNode *parentNode;
        int livingChildren = 0;
        static long long nodeCounter;
        long long nId;
        TreeNode();
        TreeNode(double dPoint, TreeNode *parent);
        double workingSequence[10];
        list<TreeNode>::iterator trueLink;
        list<TreeNode*>::iterator queueLink;

        /**** INITITALIZATION ****/
        void setParent(TreeNode *parent);
        void addChild(TreeNode *child);

        /**** CHECKING ****/
        bool doesChildExist(TreeNode *node);

        /**** CONTENT FUNCTIONS ****/
        void listClusters();
        void listPoints();
        void listSortedPoints();
        bool doesPointExist(double point);
        void openCluster(double point, bool force);
        void openCluster(double point);
        void addPointToCluster(double point, bool force);
        void addPointToCluster(double point);
        void growClusterRight(double point, bool force);
        void growClusterRight(double point);
        void growClusterLeft(double point, bool force);
        void growClusterLeft(double point);
        string getPointHash();
        int pointInRange(double point);
        list<double> getAmbPoints();
        list<double> getForcePoints();
        double calcRatio();
    protected:
    private:

};

#endif // TREENODE_H
