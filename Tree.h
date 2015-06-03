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
//#define CHUNK_SIZE 1000
class Tree
{
    public:
        /**** CONSTRUCTORS ****/
        Tree(double dStartingPoint, list<double>, int, double, int);
        Tree(list<double>, int prefix, double ratioIn);
        /**** VARIABLES ****/
        int             nCount = 0;
        int             resPrefix;
        double          ratio;
        int             print;
        TreeNode*       workingNode;
        int             nCurrentLevel;
        int             numberOfMiniQueues;
        int             numberOfThreads = NUM_THREADS;
        int             successes = 0;
        int             numberOfProofs = 0;
        int             deltas;
        int             startingPoint = 0;
        double          proofTime;
        double          dfTime;
        TreeNode*       rootNode;
        list<TreeNode>  nodes[NUM_THREADS];
        list<TreeNode*> nodeQueue;
        list<TreeNode*> depthFirstQueue[NUM_THREADS];
        list<TreeNode*> succesfulNodes;
        list<TreeNode*> parNodeQueue[NUM_THREADS];
        list<TreeNode*> parSuccesfulNodes[NUM_THREADS];
        list< list< list<double> > > proofSequences[NUM_THREADS];
        list<TreeNode>::iterator place;
        std::vector<double> delta;
        list<list<TreeNode*>> sequencedTreeQueue;
        list<TreeNode*> *parallelMiniQueues;
        int chunkSize = 1;

        /**** HANDLERS OF NEW POINTS ****/
        void addPoint(int p, double dPoint);
        bool checkPartitioning();
        bool force(TreeNode *node, TreeNode *originalNode, list<double> forcePoints, list<double> ambPoints);
        list<double> getNextPoints();
        list<double> getNextPoints(TreeNode *node);
        void dictatePointBF(double point,int  level);


        /**** GENERAL FUNCTIONS ****/

        TreeNode* newNode(int tid, double point, TreeNode* parent) ;
        void destroyNode(TreeNode *);
        void printFullTreeToFile(TreeNode *node);


        /**** TREE CLEANUP ****/
        void treeCleanup();
        void normalize(TreeNode *node);


        /**** BREADTH-FIRST FUNCTIONS ****/
        void addLevelBF(int level);
        void twoChoicesLeft(TreeNode *parent, double dPoint);
        void twoChoicesRight(TreeNode *parent, double dPoint);
        void oneChoice(TreeNode *parent, double dPoint);
        void threeChoices(TreeNode *parent, double dPoint);
        void noChoice(TreeNode *parent, double dPoint);

        int openClusterBF(TreeNode *parent, double dPoint, int tid);
        int growClusterRightBF(TreeNode *parent, double dPoint, int tid);
        int growClusterLeftBF(TreeNode *parent, double dPoint, int tid);
        int addToClusterBF(TreeNode *parent, double dPoint, int tid);

        /**** DEPTH-FIRST FUNCTIONS ****/
        int addPointDF(double dPoint);
        int addPointDF_experimental(double dPoint, TreeNode *node);
        int twoChoicesLeftDF(TreeNode *parent, double dPoint);
        int twoChoicesRightDF(TreeNode *parent, double dPoint);
        int oneChoiceDF(TreeNode *parent, double dPoint);
        int threeChoicesDF(TreeNode *parent, double dPoint);
        int noChoiceDF(TreeNode *parent, double dPoint);

        int openClusterDF(TreeNode *parent, double dPoint, int tid);
        int growClusterRightDF(TreeNode *parent, double dPoint, int tid);
        int growClusterLeftDF(TreeNode *parent, double dPoint, int tid);
        int addToClusterDF(TreeNode *parent, double dPoint, int tid);
        void destroySubtree(TreeNode* node);


        /**** PARALLEL HELPER-FUNCTIONS ****/
        double getParProofTime();
        double getParDFTime();
        void splitSequenceTree();

        /**** EXPERIMENTAL DF ****/
        bool miniQueueDF (list<TreeNode*> miniQueue, int levelsOfBF, int levelsOfDF);
        bool addPointRecursive (int level, int maxlevel);
        int addLevel(int level, int maxLevel, double point);
        void clearLevel(int level, double successPoint);

        void removeChildren(int tid, TreeNode* node);

        bool startDF(int DFlevel, int dfDepth);
        void sequenceTree();

        /***** Solution/Proof handlers ****/
        void addSolutionSequence(TreeNode* node);
        void makeNewSequenceReady();
        void removePartialSequences();
        void printSolutionSequences();
        double normalizeProof(list < list<double> > *sequence);
        void normalizeSolutions();
    protected:
    private:

};

bool node_compare(const TreeNode*,const TreeNode*);
bool node_pointscompare(const TreeNode*,const TreeNode*);

bool node_compare_sequence(const TreeNode*,const TreeNode*);
bool list_compare(const list<TreeNode*>,const list<TreeNode*>);

bool printDoubleList(string textToPrint, list<double> listToPrint);
bool printDoubleList(string textToPrint, list<double> listToPrint, double offset);
void listProofsToFiles(list< list< list<double> > > proofSequences, double ratioIn);
void listProofSequenceToTextFile( list< list<double> > proofLists);
void listInitializeTextFile( double ratio);

#endif // TREE_H
