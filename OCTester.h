#ifndef OCTESTER_H
#define OCTESTER_H

#include "Tree.h"
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace std;

class OCTester
{
    public:
        OCTester();
        static int failCount;
        static int successCount;


        bool startingSequence();
        void completeForceTest();
        void completeCheckPartitioningTest();
        void completeChildrenTest();
        void completeNodeTest();
        void completeChoiceTests();
        void completePointGenerationTests();
        void completeDepthFirstTests();


        /*** Tests for Forcing ***/
        bool easyForceTest();
        bool endForceTest();
        bool startForceTest();
        bool hardForceTest();
        bool harderForceTest();
        bool impossibleForceTest();
        bool unkownForceTest();

        /*** Tests for Check Partitioning ***/
        bool checkAboveRatioTest();
        bool checkForceNeededTest();
        bool checkUnforcableTest();
        bool checkForRootNodeTest();

        /*** Tests for child creation ***/
        bool hardTwoChoicesRightTest();
        bool addLevelBFTest();

        /*** Tests choice analysis ***/
        bool threeChoiceTest();
        bool twoChoiceRightTest();
        bool twoChoiceLeftTest();
        bool oneChoiceTest();
        bool noChoiceTest();
        bool existingPointTest();

        /*** Tests for creation and removal of nodes ***/
        bool createNewNodeTest();
        bool destroyNodeTest();

        /*** Tests for next point generator ***/
        bool threeUsualDeltasTest();
        bool withoutInputNodeTest();
        bool fromOnePointTest();

        /*** Tests depth first part ***/
        bool oneNodeDFSuccessTest();
        bool twoNodeDFSuccessTest();
        bool threeNodeDFSuccessTest();
        bool threeNodeDFFailTest();
        bool oneNodeHardDFSuccessTest();
        bool oneNodeHarderDFSuccessTest();
        bool oneNodeHardestDFSuccessTest();
        bool oneNodeUnknownDFTest();

        /*** Test Parallel Helper ***/
        bool splitSequenceQueue();

        /*** Test data generators ***/
        Tree getHardTwoChoicesRightTree(double);
        Tree getEasyForcableTree(double);
        Tree getStartForcableTree(double);
        Tree getEndForcableTree(double);
        Tree getHardForcableTree(double);
        Tree getHarderForcableTree(double);
        Tree getUnknownForceTree();
        Tree getStarterTestTree(double);
        Tree getDFsearchTreeOneNode();
        Tree getDFsearchTreeTwoNode();
        Tree getDFsearchTreeThreeNode();
        Tree getHardDFsearchTreeOneNode();
        Tree getHarderDFsearchTreeOneNode();
        Tree getHardestDFsearchTreeOneNode();
        Tree getFailingDFsearchTreeThreeNode();
        Tree getSimpleTree();
        Tree getChoiceTreeZeroOne();
        Tree getChoiceTreeTwoThree();
        Tree getExistPointTree();
        Tree getTreeAboveRatio();
        Tree getTreeForBFLevel();
        Tree getDefaultStartTree();
        Tree getNewLowerBoundTree();
        Tree getNewBaseTree();
        Tree getSecondBaseTree();
        /*** I/O functions ***/
        void successPrint(string);
        void failPrint(string);
        void headlinePrint(string);
        void bottomLine();

    protected:
    private:
};

#endif // OCTESTER_H
