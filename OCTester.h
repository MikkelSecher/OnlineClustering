#ifndef OCTESTER_H
#define OCTESTER_H

#include "Tree.h"

class OCTester
{
    public:
        OCTester();
        bool startingSequence();
        int completeForceTest();
        int completeChildrenTest();

        Tree getTreeWith11Points();
        Tree getTreeWith15Points();

        /*** Force tests ***/
        bool easyForceTest();
        bool endForceTest();
        bool startForceTest();
        bool hardForceTest();
        bool harderForceTest();
        bool impossibleForceTest();

        /*** Test for child creation ***/
        int hardTwoChoicesRightTest();

        /*** Test data generators ***/
        Tree getHardTwoChoicesRightTree(double);
        Tree getEasyForcableTree(double);
        Tree getStartForcableTree(double);
        Tree getEndForcableTree(double);
        Tree getHardForcableTree(double);
        Tree getHarderForcableTree(double);
        Tree getStarterTestTree(double);

    protected:
    private:
};

#endif // OCTESTER_H
