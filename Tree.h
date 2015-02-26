#ifndef TREE_H
#define TREE_H

#include "TreeNode.h"

#include <list>
#include <string>
#include "omp.h"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sstream>
#include <inttypes.h>

#define NUM_THREADS 14
class Tree
{
    public:
        /**** CONSTRUCTORS ****/
        Tree();
        Tree(double dStartingPoint, list<double>, int, double, int);
        Tree(list<double>, list<double>, int, double, int);

        /**** VARIABLES ****/
        int nCount = 0;
        int resPrefix;
        double ratio;
        int print;
        TreeNode *workingNode;
        int nCurrentLevel;
        int numberOfThreads = 0;
        int successes = 0;
        int deltas;
        std::vector<double> delta;
        double proofTime;
        double dfTime;
        list<TreeNode> nodes[NUM_THREADS];
        list<TreeNode> seqNodes;
        list<TreeNode*> nodeQueue;
        list<TreeNode*> seqQueue;
        list<TreeNode*> depthFirstQueue[NUM_THREADS];
        list<TreeNode*> succesfulNodes;
        list<TreeNode*> parNodeQueue[NUM_THREADS];
        list<TreeNode*> parSuccesfulNodes[NUM_THREADS];
        list<TreeNode>::iterator place;

        /**** HANDLERS OF NEW POINTS ****/
        void nextPoint(int level);
        void addPoint(int p, double dPoint);
        bool checkPartitioning();


        /**** GENERAL FUNCTIONS ****/
        bool compareNodes(TreeNode* node1, TreeNode* node2);
        void analyzeTree(int);


        /**** TREE CLEANUP ****/

        void treeCleanup();
        void normalize(TreeNode *node);


        /**** BREADTH-FIRST FUNCTIONS ****/
        void twoChoicesLeft(TreeNode *parent, double dPoint);
        void twoChoicesRight(TreeNode *parent, double dPoint);
        void oneChoice(TreeNode *parent, double dPoint);
        void threeChoices(TreeNode *parent, double dPoint);
        void noChoice(TreeNode *parent, double dPoint);


        /**** DEPTH-FIRST FUNCTIONS ****/
        void startDF(int levels, int);
        bool addSequence(double[10], int, int);
        void addPointDF(double dPoint);
        void twoChoicesLeftDF(TreeNode *parent, double dPoint);
        void twoChoicesRightDF(TreeNode *parent, double dPoint);
        void oneChoiceDF(TreeNode *parent, double dPoint);
        void threeChoicesDF(TreeNode *parent, double dPoint);
        void noChoiceDF(TreeNode *parent, double dPoint);
        void destroySubtree(TreeNode* node);
        bool backtrackSolution(TreeNode* node, list<double>, int, int);
        bool forwardCheck(TreeNode* node, list<double> points, int, int);
        void printChildren(TreeNode*);
        void subtreePrinter(TreeNode*, int);


        /**** SEQUENCE-CHECKER FUNCTIONS ****/
        void checkSequence(list<double>);
        void addSeqPoint(double);
        void twoChoicesLeftSeq(TreeNode *parent, double dPoint);
        void twoChoicesRightSeq(TreeNode *parent, double dPoint);
        void oneChoiceSeq(TreeNode *parent, double dPoint);
        void threeChoicesSeq(TreeNode *parent, double dPoint);
        void noChoiceSeq(TreeNode *parent, double dPoint);
        bool checkSeqPartitioning();

        /**** PARALLEL HELPER-FUNCTIONS ****/
        int splitNodeQueue(int);
        int splitSuccesQueue(int);
        double getParProofTime();
        double getParDFTime();


    protected:
    private:

};

bool node_compare(const TreeNode*,const TreeNode*);
bool node_pointscompare(const TreeNode*,const TreeNode*);

#endif // TREE_H
