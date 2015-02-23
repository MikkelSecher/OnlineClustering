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
        bool live;
        bool visited;
        list<TreeNode*> children;
        TreeNode *parentNode;
        int livingChildren = 0;
        static long long nodeCounter;
        long long nId;
        TreeNode();
        TreeNode(double dPoint, TreeNode *parent);
        double workingSequence[10];


        /**** INITITALIZATION ****/
        void setParent(TreeNode *parent);
        void addChild(TreeNode *child);

        /**** CHECKING ****/
        bool doesChildExist(TreeNode *node);

    protected:
    private:

};

#endif // TREENODE_H
