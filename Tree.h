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

#define NUM_THREADS 4
class Tree
{
    public:
        /**** CONSTRUCTORS ****/
        Tree();
        Tree(double dStartingPoint, list<double>, int, double, int);
        Tree(list<double>, list<double>, int, double, int);

        TreeNode* newNode(int tid, double point, TreeNode* parent) ;

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
        list<list<TreeNode*>> sequencedTreeQueue;
        list<TreeNode*> depthFirstQueue[NUM_THREADS];
        list<TreeNode*> succesfulNodes;
        list<TreeNode*> parNodeQueue[NUM_THREADS];
        list<TreeNode*> parSuccesfulNodes[NUM_THREADS];
        list<TreeNode>::iterator place;

        /**** HANDLERS OF NEW POINTS ****/
        void nextPoint(int level);
        void addPoint(int p, double dPoint);
        bool checkPartitioning();
        bool force_experimental(TreeNode *node, TreeNode *originalNode, list<double> forcePoints, list<double> ambPoints);


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
        int addPointDF(double dPoint);
        int addPointDF_experimental(double dPoint, TreeNode *node);
        int twoChoicesLeftDF(TreeNode *parent, double dPoint);
        int twoChoicesRightDF(TreeNode *parent, double dPoint);
        int oneChoiceDF(TreeNode *parent, double dPoint);
        int threeChoicesDF(TreeNode *parent, double dPoint);
        int noChoiceDF(TreeNode *parent, double dPoint);
        void destroySubtree(TreeNode* node);
        bool backtrackSolution(TreeNode* node, list<double>, int, int);
        bool forwardCheck(TreeNode* node, list<double> points, int, int);
        void printChildren(TreeNode*);
        void subtreePrinter(TreeNode*, int);
        void initPrinter(int succesNumber);
        void closePrinter(int succesNumber);
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

        /**** EXPERIMENTAL DF ****/
        bool addPointRecursive (int level, int maxlevel);
        int addLevel(int level, int maxLevel, double point);
        void clearLevel(int level, double successPoint);

        list<double> calculateNextPoints(TreeNode *node);
        void removeChildren(int tid, TreeNode* node);

        void startDF_experimental(int DFlevel, int dfDepth);
        void sequenceTree();

    protected:
    private:

};

bool node_compare(const TreeNode*,const TreeNode*);
bool node_compare_sequence(const TreeNode*,const TreeNode*);
bool list_compare(const list<TreeNode*>,const list<TreeNode*>);
#endif // TREE_H
